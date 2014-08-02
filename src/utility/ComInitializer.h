#pragma once

class ComInitializer
{
public:
  /// <summary>
  /// Calls CoInitialize for multithreaded COM, or throws an exception if this could not be done
  /// </summary>
  /// <remarks>
  /// Has no effect on platforms other than Windows
  /// </remarks>
  ComInitializer(void);
  ~ComInitializer(void);

#ifdef _MSC_VER
private:
  const HRESULT m_hr;

public:
  operator HRESULT() const { return m_hr; }
#endif
};
