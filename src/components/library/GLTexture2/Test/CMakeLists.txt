add_executable(GLTexture2Test GLTexture2Test.cpp)
target_link_libraries(GLTexture2Test GLTexture2 GLTestFramework GTest)
set_property(TARGET GLTexture2Test PROPERTY FOLDER "Tests")
add_test(NAME GLTexture2Test COMMAND $<TARGET_FILE:GLTexture2Test>)
