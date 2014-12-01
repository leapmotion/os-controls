add_executable(C++11Test C++11ComplianceTest.cpp)
target_link_libraries(C++11Test C++11 GTest)
set_property(TARGET C++11Test PROPERTY FOLDER "Tests")
add_test(NAME C++11Test COMMAND $<TARGET_FILE:C++11Test>)