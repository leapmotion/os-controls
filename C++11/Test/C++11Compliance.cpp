#include <gtest/gtest.h>
#include <iostream>

class CPPComplianceTest : public testing::Test { };

// Verifying that variadic templates work.

void PrintArgs () { }

template <typename T, typename... Types>
void PrintArgs (const T &arg, Types... args) {
  std::cout << arg << " ";
  PrintArgs(args...);
}

TEST_F(CPPComplianceTest, VariadicTemplate) {
  PrintArgs(1234567, 42.0f, "hello");
  std::cout << '\n';
}
