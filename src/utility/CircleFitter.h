// Copyright (c) 2010 - 2013 Leap Motion. All rights reserved. Proprietary and confidential.
#pragma once
#include <Eigen/Dense>

template<int DIMS>
class CircleFitter {
public:
  //Type definitions for fitter
  typedef Eigen::Matrix<double,DIMS,1> PTYPE;
  typedef Eigen::Matrix<double,DIMS+2,1> VTYPE;
  typedef Eigen::Matrix<double,DIMS+2,DIMS+2> MTYPE;
  
  //Constructor
  CircleFitter() { Reset(); }
  
  //Resets the fitter
  void Reset() { m_matrix.setZero(); }
  
  //Requires DIM+1 points to do a fit
  inline void AddPoint(const PTYPE& point, double weight=1.0) {
    VTYPE v;
    v << point.squaredNorm(), point, 1.0;
    m_matrix += (v*weight)*v.transpose();
  }
  
  //Performs a circle fit (returns smallest singular value)
  //NOTE: The smaller the return value, the better the fit
  double Fit() {
    const Eigen::JacobiSVD<MTYPE, Eigen::NoQRPreconditioner> svd(m_matrix, Eigen::ComputeFullV);
    VTYPE coeffs = svd.matrixV().col(DIMS+1);
    coeffs *= -0.5/coeffs[0];
    m_center = coeffs.template segment<DIMS>(1);
    m_radius = std::sqrt(m_center.squaredNorm() + 2*coeffs[DIMS+1]);
    return svd.singularValues()[DIMS+1];
  }
  
  //Returns the total weight of all data added so far
  inline double Weight() const {
    return m_matrix(DIMS + 1, DIMS + 1);
  }
  
  //The covariance matrix for the current circle coefficients
  inline const MTYPE& Matrix() const {
    return m_matrix;
  }
  
  //Returns the fit coefficients (Must call 'Fit' first)
  inline double Radius() const {
    return m_radius;
  }
  
  //Returns the center of the circle (Must call 'Fit' first)
  inline const PTYPE& Center() const {
    return m_center;
  }
  
private:
  //Covariance matrix and solved fitted coefficients
  MTYPE m_matrix;
  PTYPE m_center;
  double m_radius;
};
