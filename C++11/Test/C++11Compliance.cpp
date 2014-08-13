#include <gtest/gtest.h>
#include <iostream>

class CPPComplianceTest:
  public testing::Test
{};

// Verifying that variadic templates work.

void PrintArgs () { 
  std::cout << std::endl;
}

template <typename T, typename... Types>
void PrintArgs (const T &arg, Types... args) {
  std::cout << arg << " ";
  PrintArgs(args...);
}


TEST_F(CPPComplianceTest, VariadicTemplate) {
  PrintArgs(1234567, 42.0f, "hello", false);

  ASSERT_EQ(42, 42) << "What's the question?";
}
