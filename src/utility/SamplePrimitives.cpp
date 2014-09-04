#include "stdafx.h"
#include "SamplePrimitives.h"
#include "SVGPrimitive.h"

static const char sc_box[] = R"svg(<svg  xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><rect x="10" y="10" height="100" width="100" style="stroke:#ff0000; fill: #0000ff"/></svg>)svg";

std::shared_ptr<SVGPrimitive> MakeBoxPrimitive(void) {
  return std::shared_ptr<SVGPrimitive>(
    new SVGPrimitive(sc_box)
  );
}