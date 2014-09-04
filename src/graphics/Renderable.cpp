#include "stdafx.h"
#include "Renderable.h"

void Renderable::ZOrderList::Add(const std::shared_ptr<Renderable>& renderable) {
  push_front(renderable);
  renderable->m_pContainingList = this;
  renderable->m_pos = begin();
}

void Renderable::ZOrderList::BringToFront(Renderable* renderable) {
  // Move this entry to the front of the list:
  splice(
    end(),
    *this,
    renderable->m_pos,
    std::next(renderable->m_pos)
  );
}

void Renderable::RemoveFromParent(void) {
  if(!m_pContainingList)
    return;

  m_pContainingList->erase(m_pos);
  m_pos = m_pContainingList->end();
  m_pContainingList = nullptr;
}