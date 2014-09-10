// Copyright (c) 2010 - 2014 Leap Motion. All rights reserved. Proprietary and confidential.
#include "stdafx.h"
#include "OSScreen.h"
#include <Primitives.h>
#include <GLTexture2.h>
#include <ShellScalingApi.h>
#include <SFML/Graphics/Image.hpp>

#include <fstream>
#include <cmath>

HMODULE hshcore = LoadLibrary("shcore");
auto g_GetDpiForMonitor = (decltype(GetDpiForMonitor)*) GetProcAddress(hshcore, "GetDpiForMonitor");

void OSScreen::Update()
{
  MONITORINFOEX info;
  info.cbSize = sizeof(MONITORINFOEX);
  GetMonitorInfo(m_screenID, &info);
  info.rcMonitor.left;
  info.rcMonitor.top;
  info.rcMonitor.right;
  info.rcMonitor.bottom;
  m_bounds = OSRect(static_cast<float>(info.rcMonitor.left),
                    static_cast<float>(info.rcMonitor.top),
                    static_cast<float>(info.rcMonitor.right - info.rcMonitor.left),
                    static_cast<float>(info.rcMonitor.bottom - info.rcMonitor.top));
  m_isPrimary = ((info.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY);

  // Try to get the DPI for this monitor, if we can find the entrypoint:
  if(g_GetDpiForMonitor) {
    UINT dpiX;
    UINT dpiY;
    g_GetDpiForMonitor(m_screenID, MDT_RAW_DPI, &dpiX, &dpiY);
    m_pixelsPerInch = static_cast<float>(std::sqrt(dpiX * dpiX + dpiY * dpiY));
  }
  else
    // Default on Windows is 96/in
    m_pixelsPerInch = 96;
}

std::shared_ptr<ImagePrimitive> OSScreen::GetBackgroundTexture(std::shared_ptr<ImagePrimitive> img) const
{
  WCHAR pvParam[1024] = {0};
  if (!SystemParametersInfoW(SPI_GETDESKWALLPAPER, 1024, pvParam, 0)) {
    return img;
  }
  std::wstring pathString(pvParam);
  if (pathString.empty()) {
    return img;
  }
  std::ifstream ifs(pathString.c_str(), std::ios::in | std::ios::binary);
  std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  ifs.close();
  if (str.empty()) {
    return img;
  }
  sf::Image image;
  if (!image.loadFromMemory(str.data(), str.size())) {
    return img;
  }
  str.clear();
  const uint8_t* dstBytes = static_cast<const uint8_t*>(image.getPixelsPtr());
  if (!dstBytes) {
    return img;
  }
  const auto size = image.getSize();
  const size_t width = static_cast<size_t>(size.x);
  const size_t height = static_cast<size_t>(size.y);
  const size_t totalBytes = width*height*4;

  std::shared_ptr<GLTexture2> texture = img->Texture();
  if (texture) {
    const auto& params = texture->Params();
    if (params.Height() != height || params.Width() != width) {
      texture.reset();
    }
  }
  if (texture) {
    texture->UpdateTexture(dstBytes); // Very dangerous function interface!
  } else {
    GLTexture2Params params{static_cast<GLsizei>(width), static_cast<GLsizei>(height)};
    params.SetTarget(GL_TEXTURE_2D);
    params.SetInternalFormat(GL_RGBA8);
    params.SetTexParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GLTexture2PixelDataReference pixelData{GL_RGBA, GL_UNSIGNED_BYTE, dstBytes, totalBytes};

    texture = std::make_shared<GLTexture2>(params, pixelData);
    img->SetTexture(texture);
    img->SetScaleBasedOnTextureSize();
  }
  texture->Bind();
  glGenerateMipmap(GL_TEXTURE_2D);
  texture->Unbind();

  return img;
}
