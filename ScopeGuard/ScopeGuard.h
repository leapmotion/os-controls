#pragma once

// See ScopeGuard.
enum class BindFlags : GLuint { NONE = 0, BIND = (1 << 0), UNBIND = (1 << 1), BIND_AND_UNBIND = BIND|UNBIND };

// A class which provides a convenient RAII way to handle binding and unbinding objects, where the bind
// and unbind operations can be defined.
template <typename Class_, void(*BindingFunction_)(const Class_ &), void(*UnbindingFunction_)(const Class_ &)>
class ScopeGuard {
public:
  
  // Construction of a ScopeGuard will bind the guarded object if necessary (specified by bind_flags).
  // The guarded object must live at least as long as this object, unless the bind flags don't require
  // unbinding.
  ScopeGuard (const Class_ &guarded_object, BindFlags bind_flags) : m_guarded_object(guarded_object), m_bind_flags(bind_flags) {
    if (GLuint(m_bind_flags) & GLuint(BindFlags::BIND)) {
      BindingFunction_(m_guarded_object);
    }
  }
  // Destruction of a ScopeGuard will unbind the guarded object if necessary (specified by bind_flags).
  // If Release() has been called before destruction, then the guarded object will be unbound then, and not 
  // upon destruction.
  ~ScopeGuard () {
    Release();
  }

  // Unbinds the guarded object (if necessary) at the time of this call, and disables unbinding upon destruction.
  void Release () {
    if (GLuint(m_bind_flags) & GLuint(BindFlags::UNBIND)) {
      UnbindingFunction_(m_guarded_object);
      m_bind_flags = BindFlags(GLuint(m_bind_flags) & ~GLuint(BindFlags::UNBIND)); // unset the UNBIND flag.
    }
  }

private:
  
  const Class_ &m_guarded_object;
  BindFlags m_bind_flags;
};
