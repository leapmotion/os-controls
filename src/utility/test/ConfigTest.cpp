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

TEST_F(ConfigTest, ValidateNotFoundException) {
  Config config;

  bool threw = false;
  try{
    int ui = config.Get<int>("foop");
  }
  catch (std::runtime_error e)
  {
    threw = true;
  }
  ASSERT_TRUE(threw);
}

TEST_F(ConfigTest, ValidateMultiLoad) {
  {
    Config config;
    config.Set("a", 1);
    config.Set("b", 2);

    config.Save("file1.json");

    config.Set("c", 3);
    config.Set("a", 4);

    config.Save("file2.json");
  }
  {
    Config config;
    config.Load("file1.json");
    {
      int a = config.Get<int>("a");
      int b = config.Get<int>("b");

      bool threw = false;
      try{
        int c = config.Get<int>("c");
      }
      catch (std::runtime_error e){
        threw = true;
      }

      ASSERT_TRUE(threw);
      ASSERT_EQ(a, 1);
      ASSERT_EQ(b, 2);
    }
    
    config.Load("file2.json");
    {
      int a = config.Get<int>("a");
      int b = config.Get<int>("b");
      int c = config.Get<int>("c");

      ASSERT_EQ(a, 4);
      ASSERT_EQ(b, 2);
      ASSERT_EQ(c, 3);
    }
  }
  std::remove("file1.json");
  std::remove("file2.json");
}