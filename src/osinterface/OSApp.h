#pragma once
#include <string>
#include <vector>

class GLTexture2;
class OSWindow;

class OSApp
{
public:
  /// <returns>
  /// The path to the primary EXE of the application
  /// </returns>
  virtual std::wstring GetAppExe(void) = 0;

  /// <summary>
  /// Loads a texture containing the application icon
  /// </summary>
  virtual void GetAppTexture(GLTexture2& tex) = 0;

  /// <summary>
  /// Compares this instance to another instance
  /// </summary>
  virtual bool operator==(const OSApp& rhs) const = 0;
};