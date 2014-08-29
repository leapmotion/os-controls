#pragma once
#include <atomic>

template<class T>
struct lockable_property;

/// <summary>
/// Lockable property specialization, used in cases where a cheap exclusive lock is what's desired
/// </summary>
template<>
struct lockable_property<void>
{
  /// <summary>
  /// The lock type proper, which must be held in order to access the underlying property
  /// </summary>
  class lock_type {
  public:
    lock_type(void) :
      prop(nullptr)
    {}

    lock_type(lockable_property& prop) :
      prop(&prop)
    {
      // Try to set the lock property, otherwise give up
      State locked = State::Locked;
      if(!prop.locked.compare_exchange_strong(locked, State::Unlocked))
        // Failed to lock, give up
        this->prop = nullptr;
    }

  private:
    lockable_property* prop;

  public:
    // Operator overloads:
    operator bool(void) const { return !!prop; }

    /// <summary>
    /// Releases the lock held by this property
    /// </summary>
    void reset(void) {
      prop->locked = State::Unlocked;
      prop = nullptr;
    }
  };

protected:
  enum class State {
    Locked,
    Unlocked
  };

  std::atomic<State> locked;

public:
  /// <returns>
  /// True if this property has been locked against modifications
  /// </returns>
  bool is_locked(void) const {
    return locked == State::Locked;
  }

  /// <summary>
  /// Attempts to lock the property
  /// </summary>
  lock_type lock(void) {
    return lock_type(*this);
  }

  // Operator overloads:
  operator bool(void) const { return is_locked(); }
};

/// <summary>
/// Represents a property that maybe locked against modification, but still permitted read access
/// </summary>
template<class T>
struct lockable_property:
  lockable_property<void>
{
  /// <summary>
  /// The lock type proper, which must be held in order to access the underlying property
  /// </summary>
  class lock_type:
    public lockable_property<void>::lock_type
  {
  public:
    lock_type(void) {}
    lock_type(lockable_property& prop) :
      lockable_property<void>(prop)
    {}

    // Operator overloads:
    T& operator*(void) const { return static_cast<lockable_property<T>*>(prop)->val; }
    T* operator->(void) const { return &static_cast<lockable_property<T>*>(prop)->val; }
  };

private:
  std::atomic<T> val;

public:
  /// <returns>
  /// The value locked by this type
  /// </returns>
  T get(void) const { return val; }

  /// <summary>
  /// Assigns the internally locked value if this property is not locked
  /// </summary>
  /// <returns>True if the assignment succeeded</returns>
  bool operator=(const T& rhs) {
    auto lock = lock_type(*this);
    if(!lock)
      return false;

    val = rhs;
    return true;
  }

  /// <summary>
  /// Attempts to lock the property
  /// </summary>
  /// <remarks>
  /// This method shadows a method by the same name in the base class
  /// </remarks>
  lock_type lock(void) {
    return lock_type(*this);
  }
};
