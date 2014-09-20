#include "stdafx.h"
#include "AutoLaunch.h"

#include <sstream>
#include <codecvt>
#include <locale>
#include <Windows.h>

const static wchar_t* s_RegKey = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

AutoLaunch::AutoLaunch(const char* appName, const char* commandLineOptions):
m_appName(appName)
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
  if (commandLineOptions)
    stream << L" " << converter.from_bytes(commandLineOptions);

  m_command = converter.to_bytes(stream.str());
}

bool AutoLaunch::IsAutoLaunch() {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

  char value[MAX_PATH] = {};
  DWORD size = MAX_PATH;
  LSTATUS read = RegGetValueW(HKEY_CURRENT_USER, s_RegKey, converter.from_bytes(m_appName).c_str(), RRF_RT_REG_SZ, NULL, value, &size);

  return read == ERROR_SUCCESS;
}

void AutoLaunch::SetAutoLaunch() {
  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> converter;
}