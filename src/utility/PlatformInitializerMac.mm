#include "stdafx.h"
#include "PlatformInitializerMac.h"
#include <Foundation/NSObjCRuntime.h>
#include <mach-o/dyld.h>
#include <objc/runtime.h>
#include <unistd.h>

#include <ApplicationServices/ApplicationServices.h>
#include <Security/Authorization.h>

PlatformInitializer::PlatformInitializer(void)
{
  // Change the current directory to be that of the either the executable or,
  // preferably, the Resources directory if the executable is within an
  // application bundle.
  char execPath[PATH_MAX+1] = {0};
  uint32_t pathSize = sizeof(execPath);
  if (!_NSGetExecutablePath(execPath, &pathSize)) {
    char fullPath[PATH_MAX+1] = {0};
    if (realpath(execPath, fullPath)) {
      std::string path(fullPath);
      size_t pos = path.find_last_of('/');

      if (pos != std::string::npos) {
        path.erase(pos+1);
      }
      if (!path.empty()) {
        chdir(path.c_str());
      }
      const char* resources = "../Resources";
      if (!access(resources, R_OK)) {
        chdir(resources);
      }
    }
  }

  //
  // The isOpaque method in the SFOpenGLView class of SFML always returns YES
  // (as it just uses the default implementation of NSOpenGLView). This
  // causes us to always get an opaque view. We workaround this problem by
  // replacing that method with our own implementation that returns the
  // opaqueness based on the enclosing window, all thanks to the power of
  // Objective-C.
  //
  method_setImplementation(class_getInstanceMethod(NSClassFromString(@"SFOpenGLView"), @selector(isOpaque)),
                           imp_implementationWithBlock(^BOOL(id self, id arg) { return NO; }));
}

PlatformInitializer::~PlatformInitializer(void)
{

}
