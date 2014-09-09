#include "stdafx.h"
#include "Config.h"

#include <cstdio>

class ConfigTest :
  public testing::Test
{};

TEST_F(ConfigTest, ValidateGetSet) {
  Config config;
  config.Set("stringfoop", "213. I am a string with spaces0 and a number");
  config.Set("foop", 42.42);
  config.Set("bofoop", true);

  int i = config.Get<int>("foop");
  float f = config.Get<float>("foop");
  const std::string& s = config.Get<const std::string&>("stringfoop");
  bool b = config.Get<bool>("bofoop");

  ASSERT_EQ(42, i);
  ASSERT_EQ(42.42f, f);
  ASSERT_STREQ("213. I am a string with spaces0 and a number", s.c_str());
  ASSERT_EQ(true, b);
}

TEST_F(ConfigTest, ValidateSaveLoad) {
  {
    Config config;
    config.Set("stringfoop", "213. I am a string with spaces0 and a number");
    config.Set("foop", 42.42);
    config.Set("bofoop", true);
    
    config.Save("tmpconfig.json");
  }

  {
    Config config;
    config.Load("tmpconfig.json");

    int i = config.Get<int>("foop");
    float f = config.Get<float>("foop");
    const std::string& s = config.Get<const std::string&>("stringfoop");
    bool b = config.Get<bool>("bofoop");

    ASSERT_EQ(42, i);
    ASSERT_EQ(42.42f, f);
    ASSERT_STREQ("213. I am a string with spaces0 and a number", s.c_str());
    ASSERT_EQ(true, b);
  }
  std::remove("tmpconfig.json");
}