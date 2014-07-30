#pragma once

#include <Eigen/Dense>
#include <Eigen/StdVector>

// geometry storage types
typedef double MATH_TYPE;
// matrices
typedef Eigen::Matrix<MATH_TYPE, 1, 1> Matrix1x1;
typedef Eigen::Matrix<MATH_TYPE, 2, 2> Matrix2x2;
typedef Eigen::Matrix<MATH_TYPE, 2, 3> Matrix2x3;
typedef Eigen::Matrix<MATH_TYPE, 3, 3> Matrix3x3;
typedef Eigen::Matrix<MATH_TYPE, 3, 2> Matrix3x2;
typedef Eigen::Matrix<MATH_TYPE, 4, 4> Matrix4x4;
typedef Eigen::Matrix<MATH_TYPE, Eigen::Dynamic, Eigen::Dynamic> MatrixD;

// vectors
typedef Eigen::Matrix<MATH_TYPE, 1, 1> Vector1;
typedef Eigen::Matrix<MATH_TYPE, 2, 1> Vector2;
typedef Eigen::Matrix<MATH_TYPE, 3, 1> Vector3;
typedef Eigen::Matrix<MATH_TYPE, 4, 1> Vector4;
typedef Eigen::Matrix<MATH_TYPE, 5, 1> Vector5;
typedef Eigen::Matrix<MATH_TYPE, 6, 1> Vector6;
typedef Eigen::Matrix<MATH_TYPE, 7, 1> Vector7;
typedef Eigen::Matrix<MATH_TYPE, 8, 1> Vector8;
typedef Eigen::Matrix<MATH_TYPE, 9, 1> Vector9;
typedef Eigen::Matrix<MATH_TYPE, 10, 1> Vector10;
typedef Eigen::Matrix<MATH_TYPE, Eigen::Dynamic, 1> VectorD;

// arrays
typedef Eigen::Array<MATH_TYPE, 2, 1> Array2;
typedef Eigen::Array<MATH_TYPE, 3, 1> Array3;
typedef Eigen::Array<MATH_TYPE, 4, 1> Array4;
typedef Eigen::Array<MATH_TYPE, 5, 1> Array5;
typedef Eigen::Array<MATH_TYPE, 6, 1> Array6;
typedef Eigen::Array<MATH_TYPE, 7, 1> Array7;
typedef Eigen::Array<MATH_TYPE, 8, 1> Array8;
typedef Eigen::Array<MATH_TYPE, 9, 1> Array9;
typedef Eigen::Array<MATH_TYPE, 10, 1> Array10;
typedef Eigen::Array<MATH_TYPE, Eigen::Dynamic, 1> ArrayD;

// standard library containers
typedef std::vector<Vector2, Eigen::aligned_allocator<Vector2> > stdvectorV2;
typedef std::vector<Vector3, Eigen::aligned_allocator<Vector3> > stdvectorV3;
