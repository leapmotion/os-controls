#include "stdafx.h"
#include "WindowScroller.h"

class MockWindowScroller:
  public IWindowScroller
{
public:
  MockWindowScroller(void)
  {
    AutoCurrentContext()->Initiate();
  }
  
  static MockWindowScroller* New(void) { return new MockWindowScroller; }

  void DoScrollBy(uint32_t virtualX, uint32_t virtualY, double unitsX, double unitsY) override {
    scrollRequestsX.push_back(unitsX);
    scrollRequestsY.push_back(unitsY);
  }

  std::vector<double> scrollRequestsX;
  std::vector<double> scrollRequestsY;
};

class WaitsForScrollTermination:
  public WindowScrollerEvents
{
public:
  WaitsForScrollTermination(void):
    called(false)
  {}

  std::mutex m_lock;
  std::condition_variable cv;
  bool called;

  void OnScrollStopped(void) override {
    std::lock_guard<std::mutex> lk(m_lock);
    called = true;
    cv.notify_all();
  }
};

class WindowScrollerTest:
  public testing::Test
{};

TEST_F(WindowScrollerTest, ValidateExclusivity) {
  AutoRequired<MockWindowScroller> mock;
  auto lock = mock->BeginScroll();
  lock->ScrollBy(0, 0, 0, 50);

  // Verify the scroll request got through and contains the right value
  ASSERT_FALSE(mock->scrollRequestsY.empty() || mock->scrollRequestsX.empty()) << "Scroll request was not properly forwarded";
}

TEST_F(WindowScrollerTest, MomentumBehavior) {
  AutoRequired<MockWindowScroller> mock;
  AutoRequired<WaitsForScrollTermination> wfst;

  // Scroll a bunch:
  {
    auto lock = mock->BeginScroll();
    for(size_t i = 100; i--;)
      lock->ScrollBy(0, 0, 20, 20);
  }

  // Now verify that we get a few scroll operations happening after our primary scroll operations:
  std::unique_lock<std::mutex> lk(wfst->m_lock);
  ASSERT_TRUE(
    wfst->cv.wait_for(
      lk,
      std::chrono::seconds(3),
      [&] { return wfst->called; }
    )
  ) << "Scroll operation took too long to wrap up after a momentum scroll was started";
}