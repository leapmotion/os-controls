#pragma once
#include "Vector.h"
#include <SFML/Window/GlResource.hpp>

struct RenderFrame;

/// <summary>
/// One of a linked list of renderable entities
/// </summary>
class Renderable : public sf::GlResource
{
public:
  typedef std::list<std::shared_ptr<Renderable>> t_renderList;

  Renderable(void) :
    m_pContainingList(nullptr),
    position{0.0f, 0.0f}
  {}

  Renderable(OSVector2 position) :
    m_pContainingList(nullptr),
    position{position}
  {}

  ~Renderable() {
    RemoveFromParent();
  }

  /// <returns>
  /// True if this window would potentially draw something to the screen
  /// </returns>
  /// <remarks>
  /// This routine is a prospective optimization.  If this method returns false, then the corresponding
  /// call to Render will not be made.  If all Renderables in the current scene are invisible, then no
  /// attempt will be made to flip the backing buffer.  This can result in significant CPU savings.
  /// </remarks>
  virtual bool IsVisible() const { return true; }

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
    private t_renderList
  {
    const_iterator begin(void) const { return t_renderList::begin(); }
    const_iterator end(void) const { return t_renderList::end(); }

    /// <summary>
    /// Inserts this item at the bottom of the z-order
    /// </summary>
    void Add(const std::shared_ptr<Renderable>& renderable);

    /// <summary>
    /// Remove this item at the bottom of the z-order
    /// </summary>
    void Remove(const std::shared_ptr<Renderable>& renderable);

    /// <summary>
    /// Moves the specified renderable entity to the front of the render stack
    /// </summary>
    void BringToFront(Renderable* renderable);
  };

private:
  // The collection of which this renderable is a member
  t_renderList* m_pContainingList;

  // Position in the z-order list of this renderable:
  t_renderList::const_iterator m_pos;

public:
  /// <summary>
  /// The position of this renderable entity
  /// </summary>
  OSVector2 position;

  /// <summary>
  /// Removes this renderable from any attached z-order list
  /// </summary>
  /// <remarks>
  /// This method is idempotent.  This method is not thread safe.
  /// </remarks>
  void RemoveFromParent(void);

  /// <summary>
  /// Invoked before any calls to Render to give animation entities opportunity to update their state
  /// </summary>
  virtual void AnimationUpdate(const RenderFrame& frame) {}

  /// <summary>
  /// Invoked to perform actual drawing operations on this renderable
  /// </summary>
  virtual void Render(const RenderFrame& frame) const = 0;
};
