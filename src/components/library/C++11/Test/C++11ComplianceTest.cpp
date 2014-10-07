#include <gtest/gtest.h>
#include <iostream>

class CPP11ComplianceTest : public testing::Test { };

// Verifying that variadic templates work.

void PrintArgs () { 
  std::cout << std::endl;
}

template <typename T, typename... Types>
void PrintArgs (const T &arg, Types... args) {
  std::cout << arg << " ";
  PrintArgs(args...);
}


TEST_F(CPP11ComplianceTest, VariadicTemplate) {
  PrintArgs(1234567, 42.0f, "hello", false);

  ASSERT_EQ(42, 42) << "What's the question?";
}
