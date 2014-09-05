#pragma once

class PlatformInitializer
{
public:
  /// <summary>
  /// Calls CoInitialize for multithreaded COM, or throws an exception if this could not be done
  /// </summary>
  PlatformInitializer(void);
  ~PlatformInitializer(void);

  const char* BaseAppPath() const;

private:
  const HRESULT m_hr;
  TCHAR m_appPath[MAX_PATH];

public:
  operator HRESULT() const { return m_hr; }
};
