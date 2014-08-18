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
typedef Eigen::Matrix<float, 2, 2> Matrix2x2f;
typedef Eigen::Matrix<float, 3, 3> Matrix3x3f;
typedef Eigen::Matrix<float, 4, 4> Matrix4x4f;

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
typedef Eigen::Matrix<float, 2, 1> Vector2f;
typedef Eigen::Matrix<float, 3, 1> Vector3f;
typedef Eigen::Matrix<float, 4, 1> Vector4f;

// standard library containers
typedef std::vector<Vector2, Eigen::aligned_allocator<Vector2> > stdvectorV2;
typedef std::vector<Vector3, Eigen::aligned_allocator<Vector3> > stdvectorV3;
typedef std::vector<Vector2f, Eigen::aligned_allocator<Vector2f> > stdvectorV2f;
typedef std::vector<Vector3f, Eigen::aligned_allocator<Vector3f> > stdvectorV3f;
typedef std::vector<Vector4f, Eigen::aligned_allocator<Vector4f> > stdvectorV4f;

//Marshaling functions
//NOTE:I really, really tried to make this a template function, but got stuck in template hell and did not
//have time to make it work.  This is actually fairly robust, so we'll use it untill someone more familiar
//with Eigen has the time to sort this out. --WG
#define ProjectVector(_outDim, data) data.block<_outDim,1>(0,0)
