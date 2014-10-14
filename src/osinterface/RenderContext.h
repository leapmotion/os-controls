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
  /// Make this current rendering context active or not.
  /// </summary>
  virtual void SetActive(bool active = true) = 0;

  virtual void FlushBuffer(void) = 0;

  static bool IsShaderAvailable(void) {
    std::shared_ptr<RenderContext> rootContext{std::move(GetRootContext())};
    static std::once_flag s_flag;
    std::call_once(s_flag, [] {
      if (glewInit() != GLEW_OK) {
        throw std::runtime_error("Failed to initialize GLEW");
      }
    });
    return GLEW_ARB_shading_language_100 &&
           GLEW_ARB_shader_objects       &&
           GLEW_ARB_vertex_shader        &&
           GLEW_ARB_fragment_shader;
  }

protected:
  std::shared_ptr<RenderContext> m_rootRenderContext;

  RenderContext(std::shared_ptr<RenderContext> rootRenderContext) : m_rootRenderContext(std::move(rootRenderContext)) {}

  virtual void* GetNativeContext(void) const = 0;

  static std::shared_ptr<RenderContext> GetRootContext() {
    static std::shared_ptr<RenderContext> s_rootContext(Root());
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
