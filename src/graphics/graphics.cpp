#include "stdafx.h"
#include "graphics.h"
#include "RenderEngineNode.h"
#include "MediaView.h"

void GraphicsInitialize(void) {
  AutoRequired<RenderEngineNode> rootNode;

  //Debug
  auto node = RenderEngineNode::Create<MediaView>(Vector3(300, 300, 0), 5.0f);

  rootNode->AddChild(node);
}