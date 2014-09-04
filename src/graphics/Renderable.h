#pragma once
#include "Vector.h"

struct RenderFrame;

/// <summary>
/// One of a linked list of renderable entities
/// </summary>
class Renderable
{
public:
  typedef std::list<std::shared_ptr<Renderable>> t_renderList;

  /// <summary>
  /// Counterpart collection for the Renderable, maintains a z-ordering for renderables
  /// </summary>
  /// <remarks>
  /// This collection allows for efficient insertion and removal of Renderables from a linked list that
  /// maintains a z-order of the same.  It also provides convenience splice operations to allow members
  /// of this list to be quickly brought to the front of the z-order
  ///
  /// Callers must not add a renderable to more than one list a time, or undefined behavior could
  /// result.  This object is not internally synchronized.
  /// </remarks>
  struct ZOrderList:
    t_renderList
  {
    /// <summary>
    /// Inserts this item at the bottom of the z-order
    /// </summary>
    void Add(const std::shared_ptr<Renderable>& renderable);

    /// <summary>
    /// Moves the specified renderable entity to the front of the render stack
    /// </summary>
    void BringToFront(Renderable* renderable);
  };

private:
  // Position in the z-order list of this renderable:
  t_renderList::iterator m_pos;

public:
  /// <returns>
  /// The position of this renderable entity
  /// </reutrns>
  virtual OSVector2 Translation(void) const = 0;

  /// <summary>
  /// Invoked before any calls to Render to give animation entities opportunity to update their state
  /// </summary>
  virtual void AnimationUpdate(const RenderFrame& frame) = 0;

  /// <summary>
  /// Invoked to perform actual drawing operations on this renderable
  /// </summary>
  virtual void Render(const RenderFrame& frame) const = 0;
};