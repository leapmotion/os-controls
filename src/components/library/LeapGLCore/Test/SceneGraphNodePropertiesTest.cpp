#include <gtest/gtest.h>
#include "Leap/GL/SceneGraphNodeProperty.h"
#include "Leap/GL/SceneGraphNodeValues.h"

class SceneGraphNodePropertiesTest : public testing::Test { };

template <typename Scalar, int DIM>
void TestAffineTransformProperty () {
  typedef NodeProperty<AffineTransformValue<Scalar,DIM>> TProp;
  typedef typename TProp::ValueType ValueType;

  ValueType id;
  id.setIdentity();

  // The Value() value of a default-constructed AffineTransformValue should be the identity transformation.
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
    prop3.Apply(prop2, Operate::ON_RIGHT);
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
    prop3.Apply(prop2, Operate::ON_RIGHT);

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
    prop3.Apply(prop2, Operate::ON_RIGHT);

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

TEST_F(SceneGraphNodePropertiesTest, Property_AffineTransform) {
  TestAffineTransformProperty<float,2>();
  TestAffineTransformProperty<float,3>();
  TestAffineTransformProperty<double,2>();
  TestAffineTransformProperty<double,3>();
}

template <typename Scalar>
void TestAlphaMaskProperty () {
  typedef NodeProperty<AlphaMaskValue<Scalar>> AlphaMask;

  // The Value() value of a default-constructed AlphaMaskValue should be the identity transformation.
  {
    AlphaMask prop;
    EXPECT_EQ(true, prop.IsValid());
    EXPECT_EQ(ApplyType::OPERATE, prop.ApplyType());
    EXPECT_DOUBLE_EQ(Scalar(1), prop.Value());
  }

  // Composing identity transforms
  {
    AlphaMask prop1;
    AlphaMask prop2;
    AlphaMask prop3(prop1);
    prop3.Apply(prop2, Operate::ON_RIGHT);
    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_DOUBLE_EQ(Scalar(1), prop3.Value());
  }

  // Composing non-identity transforms
  {
    AlphaMask prop1;
    AlphaMask prop2;
    prop1.Value() = Scalar(0.75);
    prop2.Value() = Scalar(0.5);

    AlphaMask prop3(prop1);
    prop3.Apply(prop2, Operate::ON_RIGHT);

    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_DOUBLE_EQ(Scalar(0.375), prop3.Value());
  }

  // Inverting an alpha mask should invalidate it
  {
    AlphaMask prop;
    prop.Invert();
    EXPECT_DOUBLE_EQ(false, prop.IsValid());
  }

  // Testing the REPLACE apply type (the default is OPERATE)
  {
    AlphaMask prop1;
    AlphaMask prop2;
    prop1.Value() = Scalar(0.5);
    prop2.Value() = Scalar(0.75);
    prop2.SetApplyType(ApplyType::REPLACE);

    AlphaMask prop3(prop1);
    prop3.Apply(prop2, Operate::ON_RIGHT);

    EXPECT_EQ(true, prop3.IsValid());
    EXPECT_EQ(ApplyType::REPLACE, prop3.ApplyType());
    EXPECT_DOUBLE_EQ(Scalar(0.75), prop3.Value());
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

TEST_F(SceneGraphNodePropertiesTest, Property_AlphaMask) {
  TestAlphaMaskProperty<float>();
  TestAlphaMaskProperty<double>();
}

template <typename AffineTransformScalar, int AFFINE_TRANSFORM_DIM, typename AlphaMaskScalar>
void TestSceneGraphNodeProperties () {
  typedef ParticularSceneGraphNodeProperties<AffineTransformScalar,AFFINE_TRANSFORM_DIM,AlphaMaskScalar> Properties;

  // Testing composition
  {
    Properties p1;
    p1.AffineTransform().scale(AffineTransformScalar(2));
    p1.AlphaMask() = AlphaMaskScalar(0.5);

    Properties p2;
    p2.AffineTransform().scale(AffineTransformScalar(3));
    p2.AlphaMask() = AlphaMaskScalar(0.75);

    typename Properties::AffineTransformValue_ id_times_6;
    id_times_6.setIdentity();
    id_times_6.scale(AffineTransformScalar(6));

    Properties p3(p1);
    p3.Apply(p2, Operate::ON_RIGHT);
    EXPECT_EQ(AffineTransformScalar(0), (p3.AffineTransform().affine() - id_times_6.affine()).squaredNorm());
    EXPECT_DOUBLE_EQ(AlphaMaskScalar(0.375), p3.AlphaMask());
  }

  // Testing inversion
  {
    Properties p;
    p.AffineTransform().scale(AffineTransformScalar(2));
    p.AlphaMask() = AlphaMaskScalar(0.5);
    p.Invert();

    typename Properties::AffineTransformValue_ id_times_one_half;
    id_times_one_half.setIdentity();
    id_times_one_half.scale(AffineTransformScalar(1)/AffineTransformScalar(2));

    EXPECT_EQ(true, p.AffineTransformProperty().IsValid());
    EXPECT_EQ(AffineTransformScalar(0), (id_times_one_half.affine() - p.AffineTransform().affine()).squaredNorm());

    EXPECT_EQ(false, p.AlphaMaskProperty().IsValid());
  }
}

TEST_F(SceneGraphNodePropertiesTest, SceneGraphNodeProperties) {
  TestSceneGraphNodeProperties<float,2,float>();
  TestSceneGraphNodeProperties<float,3,float>();
  TestSceneGraphNodeProperties<double,2,double>();
  TestSceneGraphNodeProperties<double,3,double>();
}
