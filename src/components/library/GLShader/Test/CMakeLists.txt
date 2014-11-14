add_executable(GLShaderTest GLShaderTest.cpp)
target_link_libraries(GLShaderTest GLShaderLoader GLTestFramework GTest)
set_property(TARGET GLShaderTest PROPERTY FOLDER "Tests")
add_test(NAME GLShaderTest COMMAND $<TARGET_FILE:GLShaderTest>)
