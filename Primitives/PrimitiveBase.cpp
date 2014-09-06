#include "PrimitiveBase.h"
#include "RenderState.h"

#include "GLShader.h"
#include "GLShaderBindingScopeGuard.h"
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
    ModelView& model_view = render_state.GetModelView();
    // TODO: make a ScopeGuard for model view matrix.
    model_view.Push();
    model_view.Multiply(global_properties.AffineTransform().AsFullMatrix());
    node.MakeAdditionalModelViewTransformations(model_view);

    GLShaderBindingScopeGuard bso(node.Shader(), BindFlags::BIND_AND_UNBIND); // binds node.Shader() now, unbinds upon end of scope.
    
    GLShaderMatrices::UploadUniforms(node.Shader(), model_view.Matrix(), render_state.GetProjection().Matrix(), BindFlags::NONE);
    node.Material().UploadUniforms(node.Shader(), global_properties.AlphaMask(), BindFlags::NONE);

    node.Draw(render_state);

    model_view.Pop(); // TODO: once the ScopeGuard for the model view matrix is created, this goes away.
  });
}
