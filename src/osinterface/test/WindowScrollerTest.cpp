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

  void DoScrollBy(const OSPoint& deltaPixel, const OSPoint& deltaLine, bool isMomentum) override {
    scrollRequestsX.push_back(deltaPixel.x);
    scrollRequestsY.push_back(deltaPixel.y);
  }

  std::vector<float> scrollRequestsX;
  std::vector<float> scrollRequestsY;
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
  lock->ScrollBy(OSPointZero, 0, 50);

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
      lock->ScrollBy(OSPointZero, 20, 20);
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

#include <thread>

TEST_F(WindowScrollerTest, RealWindowScroll) {
  AutoCreateContext ctxt;
  CurrentContextPusher pshr(ctxt);
  AutoRequired<IWindowScroller> real;
  AutoRequired<WaitsForScrollTermination> wfst;
  ctxt->Initiate();

  // Scroll a bunch:
  {
    auto lock = real->BeginScroll();
    for(size_t i = 20; i--;) {
      lock->ScrollBy(OSPointMake(1400, 400), 0, 60);
      std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
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
