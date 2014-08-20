#include "PrimitiveBase.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include <stack>

PrimitiveBase::PrimitiveBase()
  :
  m_shader("material"),
  m_material(m_shader),
  m_shader_matrices(m_shader)
{ }

void PrimitiveBase::DrawScene (RenderState &render_state) const {
  // Create a transform stack with the identity on the top.
  TransformStack transform_stack;
  transform_stack.push(Transform::Identity());
  // This function will be called on each node before its children are traversed to.
  std::function<void(const Parent_SceneGraphNode &)> pre_child_traversal_draw = [&transform_stack, &render_state] (const Parent_SceneGraphNode &node) {
    assert(dynamic_cast<const PrimitiveBase *>(&node) != nullptr && "unexpected non-PrimitiveBase nodes in scene graph");
    const PrimitiveBase &primitive_base_node = static_cast<const PrimitiveBase &>(node);
    // Pre-multiply each transformation, because the top of the stack is what is 
    // applied first to the geometry.
    transform_stack.push(transform_stack.top() * node.FullTransform());
    // Draw this node -- this is a virtual call to PrimitiveBase::Draw.
    primitive_base_node.Draw(render_state, transform_stack);
  };
  // This function will be called on each node after its children are traversed to.
  std::function<void(const Parent_SceneGraphNode &)> post_child_traversal_draw = [&transform_stack] (const Parent_SceneGraphNode &node) {
    // Restore the stack after this node and all its children are drawn.
    transform_stack.pop();
  };
  // This actually performs the traversal with the specified functions.
  DepthFirstTraverse(pre_child_traversal_draw, post_child_traversal_draw);
}
