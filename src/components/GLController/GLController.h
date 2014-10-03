#pragma once

#include <string>

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes

// This class bundles all the GL usage/state into a single point of control.
// TODO: think about renaming this to GLController, because "context" means
// something different in autowiring.
class GLController {
public:

  GLController ();
  ~GLController ();

  // Sets up the GL state.  Throws an exception to denote error.
  void Initialize ();
  // Shuts everything down, in order opposite of initialization.  No exceptions should be thrown.
  void Shutdown ();

  void BeginRender () const;
  void EndRender () const;

  // Accessor methods

  static std::string GetString (GLenum name);

private:

};

