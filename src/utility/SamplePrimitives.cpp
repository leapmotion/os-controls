#include "stdafx.h"
#include "SamplePrimitives.h"
#include <Primitives.h>
#include <Leap/GL/Texture2.h>
#include <SVGPrimitive.h>

static const char sc_box[] = R"svg(<svg  xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><rect x="10" y="10" height="100" width="100" style="stroke:#ff0000; fill: #0000ff"/></svg>)svg";

std::shared_ptr<SVGPrimitive> MakeBoxPrimitive(void) {
  return std::shared_ptr<SVGPrimitive>(
    new SVGPrimitive(sc_box)
  );
}

std::shared_ptr<ImagePrimitive> MakePatternedTexture(size_t cx, size_t cy) {
  std::vector<uint32_t> sampleSpace(cx * cy);

  // Fill with solid bands:
  uint32_t colors [] = {0xFFFF00FF, 0xFFFF0000, 0xFF0000FF, 0xFF00FF00};
  for(size_t i = cy; i--;)
    std::fill(
      sampleSpace.data() + i * cx,
      sampleSpace.data() + (i + 1) * cx,
      colors[(i / 3) % 4]
    );

  // Data we'll be filling:
  Texture2PixelData data(GL_RGBA, GL_UNSIGNED_BYTE, sampleSpace.data(), sampleSpace.size()*sizeof(uint32_t));

  // Create a new texture if we have to:
  Texture2Params params(cx, cy);
  params.SetInternalFormat(GL_RGB8);
  params.SetTexParameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  params.SetTexParameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return std::make_shared<ImagePrimitive>(
    std::make_shared<Texture2>(params, data)
  );
}