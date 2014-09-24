// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "AutoLaunchWin.h"

#include <sstream>
#include <codecvt>
#include <locale>
#include <Windows.h>

const static wchar_t* s_RegKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

AutoLaunch* AutoLaunch::New(void)
{
  return new AutoLaunchWin;
}

AutoLaunchWin::AutoLaunchWin()
{
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

  //Compute the exe path.
  const size_t MAX_UTF16_BYTES = MAX_PATH * 2;
  WCHAR path[MAX_UTF16_BYTES];
  const DWORD len = GetModuleFileNameW(nullptr, path, MAX_UTF16_BYTES);

  if (len == 0) {
    throw std::runtime_error("Couldn't locate our .exe");
  }

  std::wstringstream stream;
  stream << L"\"" << path << L"\"";
  m_command = converter.to_bytes(stream.str());
}

bool AutoLaunchWin::IsAutoLaunch() {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

  char value[MAX_PATH] = {};
  DWORD size = MAX_PATH;
  LSTATUS read = RegGetValueW(HKEY_CURRENT_USER, s_RegKey, converter.from_bytes(m_appName).c_str(), RRF_RT_REG_SZ, NULL, value, &size);

  return read == ERROR_SUCCESS;
}

bool AutoLaunchWin::SetAutoLaunch(bool shouldLaunch) {

  HKEY hKey;
  LONG openRes = RegOpenKeyExW(HKEY_CURRENT_USER, s_RegKey, 0, KEY_ALL_ACCESS, &hKey);
  if (openRes != ERROR_SUCCESS)
    return false;

  LONG writeRes = 0;
  if (shouldLaunch) {
    std::string commandLine = m_command + " " + m_commandLineOptions;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    std::wstring wideCommand = converter.from_bytes(commandLine);
    LONG writeRes = RegSetValueExW(hKey, converter.from_bytes(m_appName).c_str(), 0, REG_SZ, (LPBYTE)wideCommand.c_str(), (wideCommand.size() + 1)*sizeof(wchar_t));
  }
  else {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
    LONG writeRes = RegDeleteValueW(hKey, converter.from_bytes(m_appName).c_str());
  }

  if (writeRes != ERROR_SUCCESS)
    return false;

  return true;
}
