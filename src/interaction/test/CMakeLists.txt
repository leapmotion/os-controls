#Simple test harness for iterating on SystemWipeRecognizer - not intended to be a unit test.

add_executable(SystemWipeRecognizerTest main.cpp)
set_property(TARGET SystemWipeRecognizerTest PROPERTY FOLDER "Tests")

target_link_libraries(SystemWipeRecognizerTest PUBLIC interaction LeapListener)
