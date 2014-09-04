#pragma once

#include <algorithm>
#include <Eigen/Geometry>
#include "EigenTypes.h"
#include <string>

enum class ApplyType { OPERATE, REPLACE };

template <typename ValueType_, typename Derived>
class Property {
public:

  typedef ValueType_ ValueType;

  Property ()
    :
    m_is_valid(true),
    m_apply_type(ApplyType::OPERATE)
  {
    AsDerived().SetIdentity();
  }

  bool IsValid () const { return m_is_valid; }
  const ValueType &Value () const { return m_value; }
  ::ApplyType ApplyType () const { return m_apply_type; }

  void SetIsValid (bool is_valid) { m_is_valid = is_valid; }
  void SetApplyType (::ApplyType apply_type) { m_apply_type = apply_type; }
  // This can be used to directly modify the value.  You are responsible for
  // correctly setting the "is valid" flag resulting from this.
  ValueType &Value () { return m_value; }

  void Apply (const Property &other) {
    switch (other.m_apply_type) {
      case ::ApplyType::OPERATE:
        AsDerived().ApplyAsOperation(other.AsDerived());
        m_is_valid = m_is_valid && other.m_is_valid;
        break;
      case ::ApplyType::REPLACE:
        m_value = other.m_value;
        m_is_valid = other.m_is_valid;
        m_apply_type = ::ApplyType::REPLACE;
        break;
    }
  }
  // Inverting an OPERATE apply type property can result in invalidation, depending
  // on its value.  Inverting a REPLACE apply type necessarily results in invalidation.
  void Invert () {
    switch (m_apply_type) {
      case ::ApplyType::OPERATE:
        AsDerived().InvertAsOperation(); // This call should set m_is_valid to false if not invertible.
        break;
      case ::ApplyType::REPLACE:
        m_is_valid = false; // There is no way to invert a replacement.
        break;
    }
  }

  const Derived &AsDerived () const { return *static_cast<const Derived *>(this); }
  Derived &AsDerived () { return *static_cast<Derived *>(this); }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:

  bool m_is_valid;
  ValueType m_value;

private:

  ::ApplyType m_apply_type;
};

// A Transform is a translation and a linear transformation, in the block matrix form
//   [ L_00 L_01 L_02 T_0 ]
//   [ L_10 L_11 L_12 T_1 ]
//   [ L_20 L_21 L_22 T_2 ]
//   [ 0    0    0    1   ]
// where the 3x3 L matrix is the linear transformation and the 3x1 T column matrix
// is the translation vector.  The semantic is that this transformation acts on a vector X
// of the form
//   [ X_0 ]
//   [ X_1 ]
//   [ X_2 ]
//   [ 1   ]
// and is equivalent to the expression
//   L*X + T.
// The 3x4 matrix A consisting of only the L and T parts is called the affine transformation,
// and if X' is the 4x1 column matrix written above (the one with the 1 at the bottom), then
// the transformation acts simply as
//   A*X'
// and produces the same 3x1 column matrix value as the expression L*X + T.
template <typename Scalar, int DIM>
class TransformProperty
  :
  public Property<Eigen::Transform<Scalar,DIM,Eigen::AffineCompact>,
                  TransformProperty<Scalar,DIM>>
{
public:

  void SetIdentity () {
    this->m_is_valid = true; // The annoying use of the "this->" prefix is because of templates.
    this->m_value.setIdentity();
  }
  void SetValue (const Eigen::Transform<Scalar,DIM,Eigen::AffineCompact> &value) {
    this->m_is_valid = true; // Perhaps we should check for NaNs.
    this->m_value = value;
  }

  void ApplyAsOperation (const TransformProperty &other) {
    this->m_value = this->m_value * other.m_value;
  }
  void InvertAsOperation () {
    // NOTE: This has issues if DIM > 3; there is some odd static assert that
    // is preventing it from working, and there is no need to debug it now.
    this->m_value = this->m_value.inverse(Eigen::Affine);
    // TODO: check validity of inverse and set m_is_valid
  }

  // Is this necessary in subclass?
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

template <typename Scalar>
class AlphaMaskProperty
  :
  public Property<Scalar,AlphaMaskProperty<Scalar>>
{
public:

  // This function sets the alpha mask to 1, which indicates no change in color.
  void SetIdentity () {
    this->m_is_valid = true;
    this->m_value = Scalar(1);
  }
  // This function clamps the alpha mask to within [0,1].
  void SetValue (const Scalar &value) {
    this->m_is_valid = true;
    this->m_value = std::min(std::max(value, Scalar(0)), Scalar(1));
  }

  // Alpha masking is simply multiplication of alpha mask values.
  void ApplyAsOperation (const AlphaMaskProperty &other) {
    this->m_value = this->m_value * other.m_value;
  }
  // There is no reasonable inversion apply_type for alpha masking, because alpha values
  // are clamped to within [0,1], and this is not even close to being a field.
  void InvertAsOperation () {
    this->m_is_valid = false;
  }
};

template <typename... Properties_> class PropertiesTuple;

// Recursive definition of the PropertiesTuple tuple.
template <typename HeadProperty, typename... BodyProperties_>
class PropertiesTuple<HeadProperty,BodyProperties_...> {
public:

  typedef PropertiesTuple<BodyProperties_...> BodyProperties;

  PropertiesTuple () { }

  const HeadProperty &Head () const { return m_head; }
  HeadProperty &Head () { return m_head; }
  const BodyProperties &Body () const { return m_body; }
  BodyProperties &Body () { return m_body; }

  void Apply (const PropertiesTuple &other) {
    m_head.Apply(other.m_head);
    m_body.Apply(other.m_body);
  }
  void Invert () {
    m_head.Invert();
    m_body.Invert();
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  HeadProperty m_head;
  BodyProperties m_body;
};

// Base-case definition of the PropertiesTuple tuple.
template <typename HeadProperty>
class PropertiesTuple<HeadProperty> {
public:

  PropertiesTuple () { }

  const HeadProperty &Head () const { return m_head; }
  HeadProperty &Head () { return m_head; }

  void Apply (const PropertiesTuple &other) {
    m_head.Apply(other.m_head);
  }
  void Invert () {
    m_head.Invert();
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:

  HeadProperty m_head;
};

// This is a particular set of properties to be used in SceneGraphNode:
// - Affine transformation (for position/orientation/size/shearing)
// - Alpha masking (for transparency blending).
template <typename TransformScalar, int TRANSFORM_DIM, typename AlphaMaskScalar>
class SceneGraphNodeProperties
  :
  public PropertiesTuple<TransformProperty<TransformScalar,TRANSFORM_DIM>,
                         AlphaMaskProperty<AlphaMaskScalar>>
{
public:

  typedef TransformProperty<TransformScalar,TRANSFORM_DIM> TransformProperty_;
  typedef AlphaMaskProperty<AlphaMaskScalar> AlphaMaskProperty_;

  // Named accessors

  const TransformProperty_ &Transform () const { return this->Head(); }
  TransformProperty_ &Transform () { return this->Head(); }
  const AlphaMaskProperty_ &AlphaMask () const { return this->Body().Head(); }
  AlphaMaskProperty_ &AlphaMask () { return this->Body().Head(); }
};

