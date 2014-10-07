#include "EigenTypes.h"
#include <gtest/gtest.h>
#include <iostream>

class EigenTest : public testing::Test { };

#define FORMAT_VALUE(x) #x << " = " << (x)
#define FORMAT_VECTOR(x) #x << " = " << (x).transpose()
#define FORMAT_MATRIX(x) #x << " = \n" << (x) << '\n'

template <int TARGET_DIM, int SOURCE_DIM>
void TestVectorAdaptToDim () {
  typedef Eigen::Matrix<int,TARGET_DIM,1> TargetVector;
  typedef Eigen::Matrix<int,SOURCE_DIM,1> SourceVector;
  SourceVector source;
  source.setConstant(4);
  TargetVector target(VectorAdaptToDim<TARGET_DIM>(source, 5));
  std::cout << "adapting SOURCE_DIM " << SOURCE_DIM << " to TARGET_DIM " << TARGET_DIM << '\n'
            << FORMAT_VECTOR(source) << ", " << FORMAT_VECTOR(target) << '\n' << '\n';
  static const int MIN_DIM = TARGET_DIM < SOURCE_DIM ? TARGET_DIM : SOURCE_DIM;
  for (int i = 0; i < MIN_DIM; ++i) {
    EXPECT_EQ(4, target(i));
  }
  for (int i = MIN_DIM; i < TARGET_DIM; ++i) {
    EXPECT_EQ(5, target(i));
  }
}

template <int SOURCE_DIM>
void TestVectorAdaptToDim_Loop () {
  TestVectorAdaptToDim<1,SOURCE_DIM>();
  TestVectorAdaptToDim<2,SOURCE_DIM>();
  TestVectorAdaptToDim<3,SOURCE_DIM>();
  TestVectorAdaptToDim<4,SOURCE_DIM>();
  TestVectorAdaptToDim<5,SOURCE_DIM>();
  TestVectorAdaptToDim<6,SOURCE_DIM>();
  TestVectorAdaptToDim<7,SOURCE_DIM>();
}

TEST_F(EigenTest, TestVectorAdaptToDim_2_4) {
  TestVectorAdaptToDim_Loop<1>();
  TestVectorAdaptToDim_Loop<2>();
  TestVectorAdaptToDim_Loop<3>();
  TestVectorAdaptToDim_Loop<4>();
  TestVectorAdaptToDim_Loop<5>();
  TestVectorAdaptToDim_Loop<6>();
  TestVectorAdaptToDim_Loop<7>();
}

template <int TARGET_DIM, int SOURCE_DIM>
void TestSquareMatrixAdaptToDim () {
  typedef Eigen::Matrix<int,TARGET_DIM,TARGET_DIM> TargetSquareMatrix;
  typedef Eigen::Matrix<int,SOURCE_DIM,SOURCE_DIM> SourceSquareMatrix;
  SourceSquareMatrix source;
  source.setConstant(4);
  TargetSquareMatrix target(SquareMatrixAdaptToDim<TARGET_DIM>(source, 5));
  std::cout << "adapting SOURCE_DIM " << SOURCE_DIM << " to TARGET_DIM " << TARGET_DIM << '\n'
            << FORMAT_MATRIX(source) << FORMAT_MATRIX(target) << '\n';
  static const int MIN_DIM = TARGET_DIM < SOURCE_DIM ? TARGET_DIM : SOURCE_DIM;
  for (int r = 0; r < MIN_DIM; ++r) {
    for (int c = 0; c < MIN_DIM; ++c) {
      EXPECT_EQ(4, target(r,c));
    }
  }
  for (int r = 0; r < MIN_DIM; ++r) {
    for (int c = MIN_DIM; c < TARGET_DIM; ++c) {
      EXPECT_EQ(0, target(r,c));
    }
  }
  for (int r = MIN_DIM; r < TARGET_DIM; ++r) {
    for (int c = 0; c < MIN_DIM; ++c) {
      EXPECT_EQ(0, target(r,c));
    }
  }
  for (int r = MIN_DIM; r < TARGET_DIM; ++r) {
    for (int c = MIN_DIM; c < TARGET_DIM; ++c) {
      if (r == c) {
        EXPECT_EQ(5, target(r,c));
      } else {
        EXPECT_EQ(0, target(r,c));
      }
    }
  }
}

template <int SOURCE_DIM>
void TestSquareMatrixAdaptToDim_Loop () {
  TestSquareMatrixAdaptToDim<1,SOURCE_DIM>();
  TestSquareMatrixAdaptToDim<2,SOURCE_DIM>();
  TestSquareMatrixAdaptToDim<3,SOURCE_DIM>();
  TestSquareMatrixAdaptToDim<4,SOURCE_DIM>();
  TestSquareMatrixAdaptToDim<5,SOURCE_DIM>();
  TestSquareMatrixAdaptToDim<6,SOURCE_DIM>();
  TestSquareMatrixAdaptToDim<7,SOURCE_DIM>();
}

TEST_F(EigenTest, TestSquareMatrixAdaptToDim_2_4) {
  TestSquareMatrixAdaptToDim_Loop<1>();
  TestSquareMatrixAdaptToDim_Loop<2>();
  TestSquareMatrixAdaptToDim_Loop<3>();
  TestSquareMatrixAdaptToDim_Loop<4>();
  TestSquareMatrixAdaptToDim_Loop<5>();
  TestSquareMatrixAdaptToDim_Loop<6>();
  TestSquareMatrixAdaptToDim_Loop<7>();
}

