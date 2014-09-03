#include "stdafx.h"
#include "OSWindow.h"
#include <Primitives.h>
#include <GLTexture2.h>

OSWindow::OSWindow(void)
{
}

OSWindow::~OSWindow(void)
{
}

void OSWindow::GetWindowTexture(ImagePrimitive& img) {
  // Recover our extent
  auto sz = GetSize();

  // Create a sample space where we will keep our image:
  std::vector<uint32_t> sampleSpace(sz.cx * sz.cy);

  // Fill with solid bands:
  uint32_t colors[] = {0xFFFF00FF, 0xFFFF0000, 0xFF0000FF, 0xFF00FF00};
  for(size_t i = sz.cy; i--;) {
    std::fill(
      &sampleSpace[i * sz.cx],
      &sampleSpace[(i + 1) * sz.cx],
      colors[(i / 3) % 4]
    );
  }

  // Create a new texture if we have to:
  auto tex = img.Texture();
  GLTexture2Params params = tex->Params();
  if(
    sz.cx != params.Width() ||
    sz.cy != params.Width()
  ) {
    params.SetWidth(sz.cx);
    params.SetHeight(sz.cy);
    tex = std::shared_ptr<GLTexture2>(new GLTexture2(params));
    img.SetTexture(tex);
  }

  // Subimage:
  tex->UpdateTexture(sampleSpace.data());
}
