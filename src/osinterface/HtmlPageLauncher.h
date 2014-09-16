#pragma once
#include <autowiring/ContextMember.h>

class HtmlPageLauncher:
  public ContextMember
{
public:
  HtmlPageLauncher(void);
  ~HtmlPageLauncher(void);

  static HtmlPageLauncher* New(void);

private:

public:
  /// <summary>
  /// Opens the users preferred browser, navigating to the specified URL
  /// </summary>
  /// <param name="url">The URL to be launched</param>
  virtual void LaunchPage(const char* url) = 0;
};

