add_executable(EigenTypesTest EigenTest.cpp)
target_link_libraries(EigenTypesTest EigenTypes GTest)
set_property(TARGET EigenTypesTest PROPERTY FOLDER "Tests")
add_test(NAME EigenTypesTest COMMAND $<TARGET_FILE:EigenTypesTest>)
