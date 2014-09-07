#include "PrimitiveBase.h"
#include "RenderState.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include <stack>

PrimitiveBase::PrimitiveBase()
  :
  m_shader("material"),
  m_opacity(1.0f),
  m_overrideOpacity(false)
{
  GLMaterial::CheckShaderForUniforms(*m_shader);
}

void PrimitiveBase::DrawSceneGraph(const PrimitiveBase& rootNode, RenderState &render_state) {
  // This actually performs the traversal with the specified functions.
  rootNode.DepthFirstTraverse(
    [&render_state](const Parent_SceneGraphNode &node) {
      assert(dynamic_cast<const PrimitiveBase *>(&node) != nullptr && "unexpected non-PrimitiveBase nodes in scene graph");

      ModelView& modelView = render_state.GetModelView();
      modelView.Push();
      modelView.Translate(node.Translation());
      modelView.Multiply(Matrix3x3(node.LinearTransformation()));

      // Draw this node -- this is a virtual call to PrimitiveBase::Draw.
      const PrimitiveBase &primitive_base_node = static_cast<const PrimitiveBase &>(node);

      OpacityStack& opacityStack = render_state.GetOpacityStack();
      opacityStack.Push();
      if (primitive_base_node.OverrideOpacity()) {
        opacityStack.Opacity() = primitive_base_node.Opacity();
      } else {
        opacityStack.Multiply(primitive_base_node.Opacity());
      }

      primitive_base_node.Draw(render_state);
    }
  , 
    [&render_state](const Parent_SceneGraphNode &node) {
      // Restore the stack after this node and all its children are drawn.
      render_state.GetModelView().Pop();
      render_state.GetOpacityStack().Pop();
    }
  );
}
