#include "PrimitiveBase.h"
#include "RenderState.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include <stack>

PrimitiveBase::PrimitiveBase()
  :
  m_shader("material")
{
  GLMaterial::CheckShaderForUniforms(*m_shader);
}

void PrimitiveBase::DrawSceneGraph(const PrimitiveBase& rootNode, RenderState &render_state) {
  // TODO: the existing model view matrix can be inputted as the initial state of global_properties
  // in the call to DepthFirstTraverse.
  rootNode.DepthFirstTraverse<PrimitiveBase>([&render_state](const PrimitiveBase &node, const Properties &global_properties) {
    // Set the model view (TODO: change this to not be in the RenderState, since it's tracked by DepthFirstTraverse)
    ModelView& modelView = render_state.GetModelView();
    modelView.Push();
    modelView.Multiply(global_properties.AffineTransform().AsFullMatrix());

    render_state.SetAlphaMask(global_properties.AlphaMask());

    // TODO: upload matrix uniforms here instead of in each Primitive subclass Draw method.
    //       upload material uniforms here instead of in each Primitive subclass Draw method.

    node.Draw(render_state);

    modelView.Pop();
  });
}
