#pragma once
#include <GL/glew.h>
#include <memory>
#include <mutex>
#include <exception>

class RenderContext
{
public:
  virtual ~RenderContext(void) {}

  static RenderContext* New(void);

  RenderContext(const RenderContext&) = delete;
  RenderContext& operator=(const RenderContext&) = delete;

  /// <summary>
  /// Make this the active rendering context or not.
  /// </summary>
  virtual void SetActive(bool active = true) = 0;

  virtual void FlushBuffer(void) = 0;

  static bool IsShaderAvailable(void) {
    static const bool s_shaderAvailable = [] {
      std::shared_ptr<RenderContext> rootContext{std::move(GetRootContext())};
      return GLEW_ARB_shading_language_100 &&
             GLEW_ARB_shader_objects       &&
             GLEW_ARB_vertex_shader        &&
             GLEW_ARB_fragment_shader;
    }();
    return s_shaderAvailable;
  }

protected:
  std::shared_ptr<RenderContext> m_rootRenderContext;

  RenderContext(std::shared_ptr<RenderContext> rootRenderContext) : m_rootRenderContext(std::move(rootRenderContext)) {}

  virtual void* GetNativeContext(void) const = 0;

  static std::shared_ptr<RenderContext> GetRootContext() {
    static std::shared_ptr<RenderContext> s_rootContext(Root());

    static const int s_glewInitialized = [] {
      return glewInit();
    }();
    return s_rootContext;
  }

private:
  /// <summary>
  /// Create the root render context that is shared by all children
  /// </summary>
  static RenderContext* Root(void);

  friend class RenderContextResource;
};

class RenderContextResource
{
public:
  RenderContextResource() : m_rootContext{std::move(RenderContext::GetRootContext())} {}
private:
  std::shared_ptr<RenderContext> m_rootContext;
};
