#include "stdafx.h"
#include "Renderable.h"

void Renderable::ZOrderList::Add(const std::shared_ptr<Renderable>& renderable) {
  push_front(renderable);
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