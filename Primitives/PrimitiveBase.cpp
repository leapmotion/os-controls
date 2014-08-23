#include "PrimitiveBase.h"
#include "RenderState.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include <stack>

PrimitiveBase::PrimitiveBase()
  :
  m_shader("material"),
  m_material(m_shader),
  m_shader_matrices(m_shader)
{ }

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
      primitive_base_node.Draw(render_state);
    }
  , 
    [&render_state](const Parent_SceneGraphNode &node) {
      // Restore the stack after this node and all its children are drawn.
      render_state.GetModelView().Pop();
    }
  );
}
