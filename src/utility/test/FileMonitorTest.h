// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <gtest/gtest.h>

#include <locale>
#include <codecvt>

class FileMonitorTest:
  public testing::Test
{
  public:
    boost::filesystem::path GetTemporaryName() {
#if __ANDROID__
      static uint32_t s_counter = 123456;
      std::ostringstream oss;
      oss << "tmp." << ++s_counter << ".ABCDEF";
      return boost::filesystem::path(oss.str());
#else
      return boost::filesystem::unique_path();
#endif
    }

    static boost::filesystem::path GetTemporaryPath() {
#if __ANDROID__
      return boost::filesystem::path("/data/local/tmp") / GetTemporaryName();
#else
      return boost::filesystem::canonical(boost::filesystem::temp_directory_path()) / boost::filesystem::unique_path();
#endif
    }
    static bool SetFileContent(const boost::filesystem::path& path, const std::string& content) {
      boost::filesystem::ofstream ofs(path, boost::filesystem::ofstream::out | boost::filesystem::ofstream::trunc);
      if (!ofs.good()) {
        return false;
      }
      ofs.write(content.data(), content.size());
      ofs.close();
      return true;
    }
};
