#include "stdafx.h"
#include "ExposeView.h"

ExposeView::ExposeView() {
  
}

ExposeView::~ExposeView() {
  
}

void ExposeView::AutoInit() {
  auto self = shared_from_this();
  m_rootNode->AddChild(self);
}

void ExposeView::CloseView() {
  
}

void ExposeView::AnimationUpdate(const RenderFrame& frame) {
  
}