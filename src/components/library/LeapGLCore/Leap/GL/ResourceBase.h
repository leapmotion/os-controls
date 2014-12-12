#pragma once

#include <cassert>

namespace Leap {
namespace GL {

/// @brief Static interface class for classes acquiring resources, providing initialize/shutdown methods.
/// @details This class offers the public methods IsInitialized, Initialize<...>, and Shutdown.
/// TODO write more about this.
/// A subclass C of this class must inherit ResourceBase<C> and implement the following methods.
/// - bool IsInitialized_Implementation () const;
/// - void Initialize_Implementation (...);
/// - void Shutdown_Implementation ();
///
/// The IsInitialized_Implementation method must return true if the resource is considered
/// initialized (the definition of "is initialized" depends on the resource type).
///
/// The Initialize_Implementation method may accept any arguments (and have any number of
/// overloads).  Its pre-condition is that IsInitialized_Implementation() TODO FINISH
///
/// Subclasses are responsible for calling Shutdown before or during their own destruction
/// (this can't be done in the ResourceBase destructor, because that is only called after
/// the subclass' destructor, which destroys all of its members).
template <typename Derived_>
class ResourceBase {
public:

  bool IsInitialized () const {
    return AsDerived().IsInitialized_Implementation();
  }
  template <typename... Types_>
  void Initialize (Types_... args) {
    Shutdown();
    AsDerived().Initialize_Implementation(args...);
    assert(IsInitialized() && "Initialize_Implementation or IsInitialized_Implementation incorrectly defined.");
  }
  void Shutdown () {
    if (IsInitialized()) {
      AsDerived().Shutdown_Implementation();
      assert(!IsInitialized() && "Shutdown_Implementation or IsInitialized_Implementation incorrectly defined.");
    }
  }

protected:

  const Derived_ &AsDerived () const { return *static_cast<const Derived_ *>(this); }
  Derived_ &AsDerived () { return *static_cast<Derived_ *>(this); }
};

} // end of namespace GL
} // end of namespace Leap
