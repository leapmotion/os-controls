#pragma once

#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes

namespace Leap {
namespace GL {

// For use in specifying the storage convention for matrix-valued arguments to various functions.
enum MatrixStorageConvention { COLUMN_MAJOR, ROW_MAJOR };

// For use in specifying error-checking policy.
enum ErrorPolicy { THROW, ASSERT };

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
