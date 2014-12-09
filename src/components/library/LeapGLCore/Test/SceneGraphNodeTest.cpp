#include <gtest/gtest.h>
#include <ostream>
#include "Leap/GL/SceneGraphNode.h"
#include "Leap/GL/SceneGraphNodeValues.h"

using namespace Leap::GL;

// #define FORMAT_VALUE(x) #x << " = " << (x)

class SceneGraphNodeTest : public testing::Test { };

// This is like ParticularSceneGraphNodeProperties, but has all values.
template <typename AffineTransformScalar, int AFFINE_TRANSFORM_DIM, typename AlphaMaskScalar>
class AllSceneGraphNodeProperties
  :
  public NodeProperties<AllSceneGraphNodeProperties<AffineTransformScalar,AFFINE_TRANSFORM_DIM,AlphaMaskScalar>,
                        NodeProperty<AffineTransformValue<AffineTransformScalar,AFFINE_TRANSFORM_DIM>>,
                        NodeProperty<AlphaMaskValue<AlphaMaskScalar>>,
                        NodeProperty<NameValue>>
{
public:

  typedef NodeProperty<AffineTransformValue<AffineTransformScalar,AFFINE_TRANSFORM_DIM>> AffineTransformProperty_;
  typedef NodeProperty<AlphaMaskValue<AlphaMaskScalar>> AlphaMaskProperty_;
  typedef NodeProperty<NameValue> NameProperty_;

  typedef AffineTransformValue<AffineTransformScalar,AFFINE_TRANSFORM_DIM> AffineTransformValue_;
  typedef AlphaMaskValue<AlphaMaskScalar> AlphaMaskValue_;
  typedef NameValue NameValue_;

  // Named accessors

  const AffineTransformProperty_ &AffineTransformProperty () const { return this->Head(); }
  AffineTransformProperty_ &AffineTransformProperty () { return this->Head(); }
  const AlphaMaskProperty_ &AlphaMaskProperty () const { return this->Body().Head(); }
  AlphaMaskProperty_ &AlphaMaskProperty () { return this->Body().Head(); }
  const NameProperty_ &NameProperty () const { return this->Body().Body().Head(); }
  NameProperty_ &NameProperty () { return this->Body().Body().Head(); }

  const AffineTransformValue_ &AffineTransform () const { return this->Head().Value(); }
  AffineTransformValue_ &AffineTransform () { return this->Head().Value(); }
  const AlphaMaskValue_ &AlphaMask () const { return this->Body().Head().Value(); }
  AlphaMaskValue_ &AlphaMask () { return this->Body().Head().Value(); }
  const NameValue_ &Name () const { return this->Body().Body().Head().Value(); }
  NameValue_ &Name () { return this->Body().Body().Head().Value(); }
};

TEST_F(SceneGraphNodeTest, PropertyDeltas) {
  typedef AllSceneGraphNodeProperties<float,2,float> Props;
  typedef SceneGraphNode<Props> Node;

  typedef Props::AffineTransformValue_ AffineTransform;
  AffineTransform a, b, c, d, e;
  a.setIdentity();
  b.setIdentity();
  c.setIdentity();
  d.setIdentity();
  e.setIdentity();

  a.scale(2.0f);
  b.translate(EigenTypes::Vector2f(1.0f, -3.0f));
  c.linear() << 0.0f, -1.0f,
                1.0f,  0.0f;
  d.scale(0.25f);
  d.translate(EigenTypes::Vector2f(4.0f, 5.0f));
  e.linear() << 1.0f,  2.0f,
                3.0f,  5.0f;
  e.scale(0.125f);
  e.translate(EigenTypes::Vector2f(0.0f, 3.0f));

  auto A = std::shared_ptr<Node>(new Node());
  A->LocalProperties().AffineTransform() = a;
  A->LocalProperties().AlphaMask() = 0.75;
  A->LocalProperties().Name() = "A";

  auto B = std::shared_ptr<Node>(new Node());
  B->LocalProperties().AffineTransform() = b;
  B->LocalProperties().AlphaMask() = 0.25;
  B->LocalProperties().Name() = "B";
  
  auto C = std::shared_ptr<Node>(new Node());
  C->LocalProperties().AffineTransform() = c;
  C->LocalProperties().AlphaMask() = 0.5;
  C->LocalProperties().Name() = "C";
  
  auto D = std::shared_ptr<Node>(new Node());
  D->LocalProperties().AffineTransform() = d;
  D->LocalProperties().AlphaMask() = 0.625;
  D->LocalProperties().Name() = "D";

  auto E = std::shared_ptr<Node>(new Node());
  E->LocalProperties().AffineTransform() = e;
  E->LocalProperties().AlphaMask() = 0.375;
  E->LocalProperties().Name() = "E";

  //      A
  //     / \     '
  //    B   C
  //       / \   '
  //      D   E

  A->AddChild(B);
  A->AddChild(C);
  C->AddChild(D);
  C->AddChild(E);

  EXPECT_EQ(A->LocalProperties(), A->PropertiesDeltaToRootNode());
  EXPECT_EQ(A->LocalProperties()*B->LocalProperties(), B->PropertiesDeltaToRootNode());
  EXPECT_EQ(A->LocalProperties()*C->LocalProperties(), C->PropertiesDeltaToRootNode());
  EXPECT_EQ(A->LocalProperties()*C->LocalProperties()*D->LocalProperties(), D->PropertiesDeltaToRootNode());
  EXPECT_EQ(A->LocalProperties()*C->LocalProperties()*E->LocalProperties(), E->PropertiesDeltaToRootNode());

  EXPECT_EQ(A->LocalProperties().Inverse(), A->PropertiesDeltaFromRootNode());
  EXPECT_EQ(B->LocalProperties().Inverse()*A->LocalProperties().Inverse(), B->PropertiesDeltaFromRootNode());
  EXPECT_EQ(C->LocalProperties().Inverse()*A->LocalProperties().Inverse(), C->PropertiesDeltaFromRootNode());
  EXPECT_EQ(D->LocalProperties().Inverse()*C->LocalProperties().Inverse()*A->LocalProperties().Inverse(), D->PropertiesDeltaFromRootNode());
  EXPECT_EQ(E->LocalProperties().Inverse()*C->LocalProperties().Inverse()*A->LocalProperties().Inverse(), E->PropertiesDeltaFromRootNode());

  EXPECT_EQ(B->LocalProperties().Inverse(), A->PropertiesDeltaTo(*B));
  EXPECT_EQ(C->LocalProperties().Inverse(), A->PropertiesDeltaTo(*C));
  EXPECT_EQ(D->LocalProperties().Inverse()*C->LocalProperties().Inverse(), A->PropertiesDeltaTo(*D));
  EXPECT_EQ(E->LocalProperties().Inverse()*C->LocalProperties().Inverse(), A->PropertiesDeltaTo(*E));

  #define FORMAT_PROPERTY(x) #x << " =\n" << (x)

  int test_number = 0;
  std::set<std::shared_ptr<Node>> nodes{A, B, C, D, E};
  for (auto i = nodes.begin(); i != nodes.end(); ++i) {
    const std::shared_ptr<Node> &from = *i;
    for (auto j = nodes.begin(); j != nodes.end(); ++j) {
      const std::shared_ptr<Node> &to = *j;
      EXPECT_EQ(to->GlobalProperties().Inverse()*from->GlobalProperties(), from->PropertiesDeltaTo(*to));
      if (!(to->GlobalProperties().Inverse()*from->GlobalProperties() == from->PropertiesDeltaTo(*to))) {
        std::cout << "\ntest number " << test_number << " ----------------------------------------------------\n";
        std::cout << FORMAT_PROPERTY(to->GlobalProperties()) << '\n';
        std::cout << FORMAT_PROPERTY(to->GlobalProperties().Inverse()) << '\n';
        std::cout << FORMAT_PROPERTY(from->GlobalProperties()) << '\n';
        std::cout << FORMAT_PROPERTY(to->GlobalProperties().Inverse()*from->GlobalProperties()) << '\n';
        std::cout << FORMAT_PROPERTY(from->PropertiesDeltaTo(*to)) << '\n';
      }
      ++test_number;
    }
  }
}

