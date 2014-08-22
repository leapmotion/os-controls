#pragma once
#include <autowiring/AutoPacket.h>
#include <functional>
#include <unordered_map>

/// <summary>
/// Simple utility type which holds a list of lambda functions
/// </summary>
struct LambdaList {
  std::vector<std::function<void()>> m_lambdas;

  template<class _Fx>
  void operator+=(_Fx&& fx) {
    m_lambdas.push_back(std::forward<_Fx>(fx));
  }
};

/// <summary>
/// An UML nonheirarchial extended state machine
/// </summary>
/// <param name="S">An enumeration of possible states</param>
/// <param name="A">An enumeration containing the input alphabet</param>
template<typename S, typename A>
class ExtendedStateMachine {
public:
  ExtendedStateMachine(S initialState):
    m_state(initialState)
  {}

private:
  // Our current state
  S m_state;

  struct hasher {
  public:
    size_t operator()(const std::pair<S, A>& val) {
      return (size_t)val.first + (size_t)val.second;
    }
  };

  // The transition function, which computes f(S, A) : S
  // This lets us figure out which state to move to given a current state and an input symbol
  std::unordered_map<std::pair<S, A>, S, hasher> m_transitions;

  // State calls, made whenever a state is entered
  std::unordered_map<S, LambdaList> m_functions;

public:
  /// <summary>
  /// Appends the specified symbol to the input alphabet and executes transitions
  /// </summary>
  void operator+=(A symbol) {
    // Based on our current state, consult the transition function 
    auto q = m_transitions.find(std::make_pair(m_state, symbol));
    if(q == m_transitions.end())
      // Nope, no transitions from this state--end here
      return;

    // Move to the desired state
    m_state = q->second;

    // Now run the lambdas here:
    for(const auto& lambda : m_functions[m_state].m_lambdas)
      lambda();
  }

  /// <summary>
  /// Adds an action to be undertaken when a particular state is active
  /// </summary>
  LambdaList& operator[](S state) {
    return m_functions[state];
  }
};