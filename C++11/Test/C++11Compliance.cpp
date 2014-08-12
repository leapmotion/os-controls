#include <gtest/gtest.h>
#include <iostream>

class CPPComplianceTest:
  public testing::Test
{};

// Verifying that variadic templates work.

void PrintArgs () { }

template <typename T>
void PrintArgs (const T &arg) {
  std::cout << arg << std::endl;
}

template <typename T1, typename T2, typename... Types>
void PrintArgs (const T1 &arg1, const T2 &arg2, Types... args) {
  std::cout << arg1 << " "<< arg2 << " ";
  PrintArgs(args...);
}


TEST_F(CPPComplianceTest, VariadicTemplate) {
  PrintArgs(1234567, 42.0f, "hello", false);

  ASSERT_EQ(42, 42) << "What's the question?";
}
