#pragma once

#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes

namespace Leap {
namespace GL {

// For use in specifying the storage convention for matrix-valued arguments to various functions.
enum MatrixStorageConvention { COLUMN_MAJOR, ROW_MAJOR };

// For use in specifying error-checking policy.
enum ErrorPolicy { THROW, ASSERT };

// For use in ShaderFrontend in specifying a uniform.
template <typename Name_, Name_ NAME_, GLenum GL_TYPE_, size_t ARRAY_LENGTH_, typename CppType_> struct UniformSpecification;

template <typename Name_, Name_ NAME_, GLenum GL_TYPE_, typename CppType_> using Uniform = UniformSpecification<Name_,NAME_,GL_TYPE_,1,CppType_>;
template <typename Name_, Name_ NAME_, GLenum GL_TYPE_, size_t ARRAY_LENGTH_, typename CppType_> using UniformArray = UniformSpecification<Name_,NAME_,GL_TYPE_,ARRAY_LENGTH_,CppType_>;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
