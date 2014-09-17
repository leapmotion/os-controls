#include "stdafx.h"
#include "HtmlPageLauncher.h"

class HtmlPageLauncherTest:
  public testing::Test
{};

TEST_F(HtmlPageLauncherTest, LaunchPage) {
  AutoRequired<HtmlPageLauncher> launcher;
  launcher->LaunchPage("http://www.google.com/");
}
