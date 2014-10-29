#pragma once

#include <string>

namespace Leap {
namespace GL {

class GLTexture2;
class GLTexture2Params;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.

// The only value that must be set in the passed-in GLTexture2Params is "target".
// If it is desired to specify any TexParameter values, this must be done before
// calling this function.  The other properties (width, height, internal format,
// pixel data format, pixel data type) are all determined from the loaded image.
// The GLTexture2Params property of the returned GLTexture2 will be fully populated
// with all the determined values.
// NOTE: In principle, the internal format property could be specified beforehand,
// in which case it would be a hint to OpenGL for how the texture should be stored
// internally.
GLTexture2 *LoadGLTexture2UsingFreeImage (const std::string &filepath, const GLTexture2Params &params);
