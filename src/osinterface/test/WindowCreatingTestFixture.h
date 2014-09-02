#pragma once
#include <string>

struct TestWindowProperties {
  // The title of the window
  std::string title;

  // The window process's PID
  uint32_t pid;
};

/// <summary>
/// Test fixture which allows test utility-type windows to be created
/// </summary>
/// <remarks>
/// The created window is arbitrary, and guaranteed to be well-defined in its behavior.  This fixture
/// must terminate all created processes when it returns.
/// </remarks>
class WindowCreatingTestFixture:
  public ContextMember
{
public:
  WindowCreatingTestFixture(void);
  virtual ~WindowCreatingTestFixture(void);

  static WindowCreatingTestFixture* New(void);

private:

public:
  /// <returns>
  /// The path to the application that will be launched by the test fixture
  /// </returns>
  virtual const wchar_t* GetApplicationPath(void) const = 0;

  /// <summary>
  /// Creates a test window known to exist somewhere on the screen
  /// </summary>
  virtual TestWindowProperties CreateTestWindow(void) = 0;
};

