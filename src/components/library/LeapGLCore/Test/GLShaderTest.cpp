#include <array>
#include <iostream>
#include "GLTestFramework.h"
#include <gtest/gtest.h>
#include "Leap/GL/GLShader.h"
#include "Leap/GL/Internal/UniformUploader.h"
#include <memory>

// Convenience macro for annotated printing the value of variables.
#define FMT(x) #x << " = " << (x)

class GLShaderTest : public GLTestFramework_Headless { };

TEST_F(GLShaderTest, CompileAndLink) {
  std::string vertex_shader_source(
    "void main () {\n"
    "    gl_Position = ftransform();\n"
    "    gl_FrontColor = gl_Color;\n"
    "}\n"
  );
  std::string fragment_shader_source(
    "void main () {\n"
    "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
    "}\n"
  );
  EXPECT_NO_THROW_(auto valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
}

TEST_F(GLShaderTest, CompileSuccessfully) {
  // Make sure that passing a valid shader as the attached_shader param doesn't throw.
  {
    std::string vertex_shader_source(
      "void main () {\n"
      "    gl_Position = ftransform();\n"
      "    gl_FrontColor = gl_Color;\n"
      "}\n"
    );
    std::string fragment_shader_source(
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
      "}\n"
    );
    std::shared_ptr<GLShader> valid_shader;
    ASSERT_NO_THROW_(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
  }
}

TEST_F(GLShaderTest, CompileUnsuccessfully) {
  // Make sure that passing a valid shader as the attached_shader param doesn't throw.
  {
    std::string vertex_shader_source(
      "void main () {\n"
      "    this in an intentional compile error\n"
      "    gl_FrontColor = gl_Color;\n"
      "}\n"
    );
    std::string fragment_shader_source(
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
      "}\n"
    );
    std::shared_ptr<GLShader> valid_shader;
    ASSERT_ANY_THROW(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
  }
}

void SetTypedUniformCheckInVertexShader (const std::map<GLenum,std::string> &uniform_type_map, const std::string &glsl_version) {
  for (auto it = uniform_type_map.begin(); it != uniform_type_map.end(); ++it) {
    GLenum uniform_type = it->first;
    const std::string &uniform_type_name = it->second;
    // Skip the sampler types for now.
    if (uniform_type_name.find("sampler") == std::string::npos) {
      // Construct a vertex shader source code with one of each type of allowable uniform.
      std::string vertex_shader_source("#version " + glsl_version + "\n");
      vertex_shader_source += "uniform " + uniform_type_name + " test0;\n";
      vertex_shader_source += "uniform " + uniform_type_name + " test1;\n";
      vertex_shader_source +=
        "void main () {\n"
        "    bool condition = test0 == test1;\n" // This dumb indirection is to try to get around unused uniforms maybe being compiled out of the shader.
        "    if (condition) {\n"
        "        gl_Position = ftransform();\n"
        "        gl_FrontColor = gl_Color;\n"
        "    }\n"
        "}\n";
      // Dummy fragment shader source
      std::string fragment_shader_source(
        "#version " + glsl_version + "\n"
        "void main () {\n"
        "    gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "}\n"
      );
      std::cout << vertex_shader_source << '\n';
      // Construct the shader and the GLShaderInterface.
      std::shared_ptr<GLShader> shader;
      ASSERT_NO_THROW_(shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::REQUIRED));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::REQUIRED));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::OPTIONAL_NO_WARN));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::OPTIONAL_NO_WARN));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test0", uniform_type, VariableIs::OPTIONAL_BUT_WARN));
      EXPECT_NO_THROW_(shader->CheckForTypedUniform("test1", uniform_type, VariableIs::OPTIONAL_BUT_WARN));
    }
  }
}

TEST_F(GLShaderTest, SetTypedUniformCheckInVertexShader_OpenGL_2_1) {
  SetTypedUniformCheckInVertexShader(GLShader::OPENGL_2_1_UNIFORM_TYPE_MAP, "120");
}

TEST_F(GLShaderTest, DISABLED_SetTypedUniformCheckInVertexShader_OpenGL_3_3) {
  SetTypedUniformCheckInVertexShader(GLShader::OPENGL_3_3_UNIFORM_TYPE_MAP, "330");
}

class GLShaderTest_Visible : public GLTestFramework_Visible { };

void RenderRectangle () {
  glDisable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
    
  static const GLuint VERTEX_COUNT = 4;
  const GLfloat param = 0.7f;
  const GLfloat vertex_array[VERTEX_COUNT*2] = {
    -param, -param,
     param, -param,
     param,  param,
    -param,  param
  };
  static const GLfloat TEXTURE_COORD_ARRAY[VERTEX_COUNT*2] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
  };

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, vertex_array);
  glDrawArrays(GL_TRIANGLE_FAN, 0, VERTEX_COUNT);
}

TEST_F(GLShaderTest_Visible, Vec2UniformSettingEquivalence) {
  std::string vertex_shader_source(
    "#version 120\n"
    "void main () {\n"
    "    gl_Position = ftransform();\n"
    "    gl_FrontColor = gl_Color;\n"
    "}\n"
  );
  std::string fragment_shader_source(
    "uniform vec2 thingy0;\n"
    "uniform vec2 thingy1;\n"
    "void main () {\n"
    "    gl_FragColor = vec4(thingy0.x == thingy1.x ? 1 : 0,\n"
    "                        thingy0.y == thingy1.y ? 1 : 0,\n"
    "                        1,\n"
    "                        1);\n"
    "}\n"
  );

  // This will throw if there is an error.
  GLShader shader(vertex_shader_source, fragment_shader_source);
  shader.Bind();
  std::array<float,2> v{{2.0f, 3.0f}};
  glUniform2f(shader.LocationOfUniform("thingy0"), v[0], v[1]);
  glUniform2fv(shader.LocationOfUniform("thingy1"), 1, &v[0]);
  RenderRectangle();
  shader.Unbind();

  // Finish the frame before delaying. 
  EndFrame();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}

TEST_F(GLShaderTest_Visible, Vec3UniformSettingEquivalence) {
  std::string vertex_shader_source(
    "#version 120\n"
    "void main () {\n"
    "    gl_Position = ftransform();\n"
    "    gl_FrontColor = gl_Color;\n"
    "}\n"
  );
  std::string fragment_shader_source(
    "uniform vec3 thingy0;\n"
    "uniform vec3 thingy1;\n"
    "void main () {\n"
    "    gl_FragColor = vec4(thingy0.x == thingy1.x ? 1 : 0,\n"
    "                        thingy0.y == thingy1.y ? 1 : 0,\n"
    "                        thingy0.z == thingy1.z ? 1 : 0,\n"
    "                        1);\n"
    "}\n"
  );

  // This will throw if there is an error.
  GLShader shader(vertex_shader_source, fragment_shader_source);
  shader.Bind();
  std::array<float,3> v{{2.0f, 3.0f, 4.0f}};
  glUniform3f(shader.LocationOfUniform("thingy0"), v[0], v[1], v[2]);
  glUniform3fv(shader.LocationOfUniform("thingy1"), 1, &v[0]);
  RenderRectangle();
  shader.Unbind();

  // Finish the frame before delaying. 
  EndFrame();
  
  SDL_Delay(1000); // Delay so the human's pitiful visual system can keep up.
}








class Internal_UniformSetterTest : public GLTestFramework_Headless { };

TEST_F(Internal_UniformSetterTest, SetABunch) {
  // Make sure that passing a valid shader as the attached_shader param doesn't throw.
  {
    std::string vertex_shader_source(
      "uniform bool condition;\n"
      "uniform bvec2 conditions;\n"
      "uniform vec3 vec_three;\n"
      "uniform float array[4];\n"
      "uniform vec2 vec2_array[4];\n"
      "uniform mat2 m2x2;\n"
      "uniform mat3 m3x3[2];\n"
      "uniform float dumb[4];\n"
      "struct Thingy {\n"
      "    int n;\n"
      "    bool p[2];\n"
      "};\n"
      "uniform Thingy thinger[2];\n"
      "void main () {\n"
      "    gl_Position = ftransform();\n"
      "    vec2 blah = vec2_array[0] + vec2_array[1] + vec2_array[2] + vec2_array[3];"
      "    float avg = (array[0] + array[1] + array[2] + array[3]) / 4.0;\n"
      "    float garbage = m2x2[0][0] + m3x3[0][0][0] + dumb[1] + dumb[2];"
      "    gl_FrontColor = vec4(vec_three.x, vec_three.y, vec_three.z+garbage, thinger[0].p[1]&&condition&&conditions.x&&conditions.y ? float(thinger[0].n) : avg + blah.x + blah.y);\n"
      "}\n"
    );
    std::string fragment_shader_source(
      "void main () {\n"
      "    gl_FragColor = vec4(1.0, 0.2, 0.3, 0.5);\n"
      "}\n"
    );
    std::shared_ptr<GLShader> valid_shader;
    ASSERT_NO_THROW_(valid_shader = std::make_shared<GLShader>(vertex_shader_source, fragment_shader_source));
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "vec_three")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "vec2_array")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "vec2_array[0]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "vec2_array[1]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "vec2_array[2]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "vec2_array[3]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "array")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "array[0]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "array[1]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "array[2]")) << '\n';
    std::cout << FMT(glGetUniformLocation(valid_shader->ProgramHandle(), "array[3]")) << '\n';
    std::cout << FMT(valid_shader->LocationOfUniform("vec_three")) << '\n';
    std::cout << FMT(valid_shader->LocationOfUniform("array")) << '\n';
    std::cout << FMT(valid_shader->LocationOfUniform("array[0]")) << '\n';

    valid_shader->Bind();


    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_BOOL>::Upload(valid_shader->LocationOfUniform("condition"), true);
    )
    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_BOOL_VEC2>::Upload(valid_shader->LocationOfUniform("conditions"), false, true);
    )
    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_BOOL_VEC2>::Upload(valid_shader->LocationOfUniform("conditions"), std::array<GLint,2>{{false, true}});
    )
    typedef std::array<GLint,2> GLint2;
    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_BOOL_VEC2>::Upload<GLint2>(valid_shader->LocationOfUniform("conditions"), {{false, true}});
    )


    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_FLOAT_VEC3>::Upload(valid_shader->LocationOfUniform("vec_three"), std::array<GLfloat,3>{{1.0f, 2.0f, 3.0f}});
    )
    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_FLOAT_VEC3>::Upload(valid_shader->LocationOfUniform("vec_three"), 1.0f, 2.0f, 3.0f);
    )
    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_FLOAT>::UploadArray<4>(valid_shader->LocationOfUniform("array"), std::array<GLfloat,4>{{8.0f, 4.0f, 2.0f, 1.0f}});
    )
    typedef std::array<GLfloat,4> GLfloat4;
    GL_THROW_UPON_ERROR(
      (Leap::GL::Internal::UniformUploader<GL_FLOAT>::UploadArray<4,GLfloat4>)(valid_shader->LocationOfUniform("array"), {{8.0f, 4.0f, 2.0f, 1.0f}});
    )



    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_BOOL>("condition", true);
    )
    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_BOOL_VEC2>("conditions", false, true);
    )
    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_BOOL_VEC2>("conditions", std::array<GLint,2>{{false, true}});
    )
    typedef std::array<GLint,2> GLint2;
    GL_THROW_UPON_ERROR(
      (valid_shader->UploadUniform<GL_BOOL_VEC2,GLint2>)("conditions", {{false, true}});
    )


    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_FLOAT_VEC3>("vec_three", std::array<GLfloat,3>{{1.0f, 2.0f, 3.0f}});
    )
    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_FLOAT_VEC3>("vec_three", 1.0f, 2.0f, 3.0f);
    )
    GL_THROW_UPON_ERROR(
      (valid_shader->UploadUniformArray<GL_FLOAT,4>)("array", std::array<GLfloat,4>{{8.0f, 4.0f, 2.0f, 1.0f}});
    )
    typedef std::array<GLfloat,4> GLfloat4;
    GL_THROW_UPON_ERROR(
      (valid_shader->UploadUniformArray<GL_FLOAT,4,GLfloat4>)("array", {{8.0f, 4.0f, 2.0f, 1.0f}});
    )


    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_FLOAT_MAT2>::Upload(
        valid_shader->LocationOfUniform("m2x2"),
        std::array<std::array<GLfloat,2>,2>{{
          {{1.0f, 2.0f}},
          {{3.0f, 4.0f}}
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )
    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_FLOAT_MAT2>::Upload(
        valid_shader->LocationOfUniform("m2x2"),
        std::array<GLfloat,2*2>{{
          1.0f, 2.0f,
          3.0f, 4.0f
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )
    GL_THROW_UPON_ERROR(
      (Leap::GL::Internal::UniformUploader<GL_FLOAT_MAT2>::Upload<std::array<GLfloat,2*2>>)(
        valid_shader->LocationOfUniform("m2x2"),
        {{
          1.0f, 2.0f,
          3.0f, 4.0f
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )


    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_FLOAT_MAT2>(
        "m2x2",
        std::array<std::array<GLfloat,2>,2>{{
          {{1.0f, 2.0f}},
          {{3.0f, 4.0f}}
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )
    GL_THROW_UPON_ERROR(
      valid_shader->UploadUniform<GL_FLOAT_MAT2>(
        "m2x2", 
        std::array<GLfloat,2*2>{{
          1.0f, 2.0f,
          3.0f, 4.0f
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )
    GL_THROW_UPON_ERROR(
      (valid_shader->UploadUniform<GL_FLOAT_MAT2,std::array<GLfloat,2*2>>)(
        "m2x2",
        {{
          1.0f, 2.0f,
          3.0f, 4.0f
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )


    GL_THROW_UPON_ERROR(
      Leap::GL::Internal::UniformUploader<GL_FLOAT_MAT3>::UploadArray<2>(
        valid_shader->LocationOfUniform("m3x3"),
        std::array<std::array<GLfloat,3*3>,2>{{
          {{
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f,
            7.0f, 8.0f, 9.0f
          }},
          {{
            11.0f, 12.0f, 13.0f,
            14.0f, 15.0f, 16.0f,
            17.0f, 18.0f, 19.0f
          }}
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )
    GL_THROW_UPON_ERROR(
      (Leap::GL::Internal::UniformUploader<GL_FLOAT_MAT3>::UploadArray<2,std::array<std::array<GLfloat,3*3>,2>>)(
        valid_shader->LocationOfUniform("m3x3"),
        {{
          {{
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f,
            7.0f, 8.0f, 9.0f
          }},
          {{
            11.0f, 12.0f, 13.0f,
            14.0f, 15.0f, 16.0f,
            17.0f, 18.0f, 19.0f
          }}
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )

    GL_THROW_UPON_ERROR(
      (valid_shader->UploadUniformArray<GL_FLOAT_MAT3,2>)(
        "m3x3",
        std::array<std::array<GLfloat,3*3>,2>{{
          {{
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f,
            7.0f, 8.0f, 9.0f
          }},
          {{
            11.0f, 12.0f, 13.0f,
            14.0f, 15.0f, 16.0f,
            17.0f, 18.0f, 19.0f
          }}
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )
    GL_THROW_UPON_ERROR(
      (valid_shader->UploadUniformArray<GL_FLOAT_MAT3,2,std::array<std::array<GLfloat,3*3>,2>>)(
        "m3x3",
        {{
          {{
            1.0f, 2.0f, 3.0f,
            4.0f, 5.0f, 6.0f,
            7.0f, 8.0f, 9.0f
          }},
          {{
            11.0f, 12.0f, 13.0f,
            14.0f, 15.0f, 16.0f,
            17.0f, 18.0f, 19.0f
          }}
        }},
        Leap::GL::MatrixStorageConvention::ROW_MAJOR);
    )

    valid_shader->Unbind();
  }
}
