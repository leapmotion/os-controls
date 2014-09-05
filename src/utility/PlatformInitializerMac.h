#pragma once

class PlatformInitializer
{
public:
  PlatformInitializer(void);
  ~PlatformInitializer(void);

  const char* BaseAppPath() const;

private:
  char *m_path;
};
