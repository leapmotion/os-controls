#include <gtest/gtest.h>
#include "SceneGraphNodeProperties.h"

class SceneGraphNodePropertiesTest : public testing::Test { };

template <typename Scalar, int DIM>
void TestTransformProperty () {
  typedef TransformProperty<Scalar,DIM> TProp;
  typedef typename TProp::ValueType ValueType;

  ValueType id;
  id.setIdentity();

  // The Value() value of a default-constructed TransformProperty should be the identity transformation.
  {
    TProp prop;
    EXPECT_EQ(true, prop.IsValid());
    EXPECT_EQ(ApplyType::OPERATE, prop.ApplyType());
    EXPECT_EQ(Scalar(0), (id.affine() - prop.Value().affine()).squaredNorm());
  }

  // Composing identity transforms
  {
    TProp prop1;
    TProp prop2;
    TProp prop3(prop1);
    prop3.Apply(prop2);
    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(Scalar(0), (id.affine() - prop3.Value().affine()).squaredNorm());
  }

  // Composing non-identity transforms
  {
    TProp prop1;
    TProp prop2;
    prop1.Value().scale(Scalar(2));
    prop2.Value().scale(Scalar(3));

    TProp prop3(prop1);
    prop3.Apply(prop2);

    ValueType id_times_6;
    id_times_6.setIdentity();
    id_times_6.scale(Scalar(6));
    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(Scalar(0), (id_times_6.affine() - prop3.Value().affine()).squaredNorm());
  }

  // Inverting a transform
  {
    TProp prop;
    prop.Value().scale(Scalar(2));
    prop.Invert();
    EXPECT_EQ(true, prop.IsValid());

    ValueType id_times_one_half;
    id_times_one_half.setIdentity();
    id_times_one_half.scale(Scalar(1)/Scalar(2));
    EXPECT_EQ(Scalar(0), (id_times_one_half.affine() - prop.Value().affine()).squaredNorm());
  }

  // Testing the REPLACE apply type (the default is OPERATE)
  {
    TProp prop1;
    TProp prop2;
    prop1.Value().scale(Scalar(2));
    prop2.Value().scale(Scalar(3));
    prop2.SetApplyType(ApplyType::REPLACE);

    TProp prop3(prop1);
    prop3.Apply(prop2);

    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(ApplyType::REPLACE, prop3.ApplyType());
    EXPECT_EQ(Scalar(0), (prop2.Value().affine() - prop3.Value().affine()).squaredNorm());
  }

  // Testing that inversion of a REPLACE apply type invalidates
  {
    TProp prop;
    prop.Value().scale(Scalar(2));
    prop.SetApplyType(ApplyType::REPLACE);
    prop.Invert();
    EXPECT_EQ(ApplyType::REPLACE, prop.ApplyType());
    EXPECT_EQ(false, prop.IsValid());
  }
}

TEST_F(SceneGraphNodePropertiesTest, TransformProperty) {
  TestTransformProperty<float,2>();
  TestTransformProperty<float,3>();
  TestTransformProperty<double,2>();
  TestTransformProperty<double,3>();
}

template <typename Scalar>
void TestAlphaMaskProperty () {
  typedef AlphaMaskProperty<Scalar> AlphaMask;

  // The Value() value of a default-constructed AlphaMaskProperty should be the identity transformation.
  {
    AlphaMask prop;
    EXPECT_EQ(true, prop.IsValid());
    EXPECT_EQ(ApplyType::OPERATE, prop.ApplyType());
    EXPECT_EQ(Scalar(1), prop.Value());
  }

  // Composing identity transforms
  {
    AlphaMask prop1;
    AlphaMask prop2;
    AlphaMask prop3(prop1);
    prop3.Apply(prop2);
    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(Scalar(1), prop3.Value());
  }

  // Composing non-identity transforms
  {
    AlphaMask prop1;
    AlphaMask prop2;
    prop1.Value() = Scalar(0.75);
    prop2.Value() = Scalar(0.5);

    AlphaMask prop3(prop1);
    prop3.Apply(prop2);

    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(Scalar(0.375), prop3.Value());
  }

  // Inverting an alpha mask should invalidate it
  {
    AlphaMask prop;
    prop.Invert();
    EXPECT_EQ(false, prop.IsValid());
  }

  // Testing the REPLACE apply type (the default is OPERATE)
  {
    AlphaMask prop1;
    AlphaMask prop2;
    prop1.Value() = Scalar(0.5);
    prop2.Value() = Scalar(0.75);
    prop2.SetApplyType(ApplyType::REPLACE);

    AlphaMask prop3(prop1);
    prop3.Apply(prop2);

    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(ApplyType::REPLACE, prop3.ApplyType());
    EXPECT_EQ(Scalar(0.75), prop3.Value());
  }

  // Testing that inversion of a REPLACE apply type invalidates
  {
    AlphaMask prop;
    prop.Value() = Scalar(1);
    prop.SetApplyType(ApplyType::REPLACE);
    prop.Invert();
    EXPECT_EQ(ApplyType::REPLACE, prop.ApplyType());
    EXPECT_EQ(false, prop.IsValid());
  }
}

TEST_F(SceneGraphNodePropertiesTest, AlphaMaskProperty) {
  TestAlphaMaskProperty<float>();
  TestAlphaMaskProperty<double>();
}

template <typename TransformScalar, int TRANSFORM_DIM, typename AlphaMaskScalar>
void TestSceneGraphNodeProperties () {
  typedef SceneGraphNodeProperties<TransformScalar,TRANSFORM_DIM,AlphaMaskScalar> Properties;

  // Testing composition
  {
    Properties p1;
    p1.Transform().Value().scale(TransformScalar(2));
    p1.AlphaMask().Value() = AlphaMaskScalar(0.5);

    Properties p2;
    p2.Transform().Value().scale(TransformScalar(3));
    p2.AlphaMask().Value() = AlphaMaskScalar(0.75);

    typename Properties::TransformProperty_::ValueType id_times_6;
    id_times_6.setIdentity();
    id_times_6.scale(TransformScalar(6));

    Properties p3(p1);
    p3.Apply(p2);
    EXPECT_EQ(TransformScalar(0), (p3.Transform().Value().affine() - id_times_6.affine()).squaredNorm());
    EXPECT_EQ(AlphaMaskScalar(0.375), p3.AlphaMask().Value());
  }

  // Testing inversion
  {
    Properties p;
    p.Transform().Value().scale(TransformScalar(2));
    p.AlphaMask().Value() = AlphaMaskScalar(0.5);
    p.Invert();

    typename Properties::TransformProperty_::ValueType id_times_one_half;
    id_times_one_half.setIdentity();
    id_times_one_half.scale(TransformScalar(1)/TransformScalar(2));

    EXPECT_EQ(true, p.Transform().IsValid());
    EXPECT_EQ(TransformScalar(0), (id_times_one_half.affine() - p.Transform().Value().affine()).squaredNorm());

    EXPECT_EQ(false, p.AlphaMask().IsValid());
  }
}

TEST_F(SceneGraphNodePropertiesTest, SceneGraphNodeProperties) {
  TestSceneGraphNodeProperties<float,2,float>();
  TestSceneGraphNodeProperties<float,3,float>();
  TestSceneGraphNodeProperties<double,2,double>();
  TestSceneGraphNodeProperties<double,3,double>();
}
