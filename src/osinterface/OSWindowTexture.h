#pragma once

struct OSPoint;
struct OSSize;
class OSWindow;

/// <summary>
/// Represents a window texture
/// </summary>
class OSWindowTexture {
public:
  OSWindowTexture(OSWindow& osw):
    osw(osw)
  {}

private:
  OSWindow& osw;

public:
  /// <summary>
  /// </summary>
  virtual void SetPosition(OSPoint pt) = 0;

  /// <summary>
  /// </summary>
  virtual void SetSize(OSSize sz) = 0;

  /// <summary>
  /// </summary>
  virtual void SetOpacity(float opacity) = 0;

  /// <summary>
  /// </summary>
  virtual void Render(void) = 0;
};