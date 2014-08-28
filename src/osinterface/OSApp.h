#pragma once
#include <string>
#include <vector>

class OSApp
{
public:
  /// <returns>
  /// The path to the primary EXE of the application
  /// </returns>
  virtual std::wstring GetAppExe(void) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void CopyAppIcon(void* pMem, size_t nBytesMax) = 0;

  /// <summary>
  /// Enumerates the set of windows owned by this application
  /// </summary>
  std::vector<std::shared_ptr<OSWindow>> EnumerateVisibleTopLevelWindows(void) override;

  virtual bool operator==(const OSApp& rhs) const = 0;
};