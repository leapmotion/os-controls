#pragma once

typedef HANDLE HTHUMBNAIL;

/// <summary>
/// Represents a proxy window that is used to obtain screenshots of other windows
/// </summary>
class DwmThumbnailWindow
{
public:
  DwmThumbnailWindow(HWND hwnd);
  ~DwmThumbnailWindow(void);

  // Source window for the thumbnail
  const HWND hwnd;

  // Thumbnail ID
  const HTHUMBNAIL thumbnailID;

  /// <summary>
  /// Updates the contents of the proxy window with the target monitor window
  /// </summary>
  /// <returns>True on success, false if there is an unrecoverable problem</returns>
  bool Update(void) const;

private:
  HTHUMBNAIL RegisterThumbnail(void);
};

