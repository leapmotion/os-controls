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
  size_t cx = (size_t) ceil(sz.width);
  size_t cy = (size_t) ceil(sz.height);
  std::vector<uint32_t> sampleSpace(cx * cy);

  // Fill with solid bands:
  uint32_t colors[] = {0xFFFF00FF, 0xFFFF0000, 0xFF0000FF, 0xFF00FF00};
  for(size_t i = cy; i--;) {
    std::fill(
      &sampleSpace[i * cx],
      &sampleSpace[(i + 1) * cx],
      colors[(i / 3) % 4]
    );
  }

  // Create a new texture if we have to:
  auto tex = img.Texture();
  GLTexture2Params params = tex->Params();
  if(
    cx != params.Width() ||
    cy != params.Width()
  ) {
    params.SetWidth(cx);
    params.SetHeight(cy);
    tex = std::shared_ptr<GLTexture2>(new GLTexture2(params));
    img.SetTexture(tex);
  }

  // Subimage:
  tex->UpdateTexture(sampleSpace.data());
}
