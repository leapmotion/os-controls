#include <gtest/gtest.h>
#include <ostream>
#include "SceneGraphNode.h"
#include "SceneGraphNodeValues.h"

// #define FORMAT_VALUE(x) #x << " = " << (x)

class SceneGraphNodeTest : public testing::Test { };

TEST_F(SceneGraphNodeTest, PropertyDeltas) {
  typedef ParticularSceneGraphNodeProperties<float,2,float> Props;
  typedef SceneGraphNode<Props> Node;

  typedef Props::AffineTransformValue_ AffineTransform;
  AffineTransform a, b, c, d, e;
  a.setIdentity();
  b.setIdentity();
  c.setIdentity();
  d.setIdentity();
  e.setIdentity();

  a.scale(2.0f);
  b.translate(Vector2f(1.0f, -3.0f));
  c.linear() << 0.0f, -1.0f,
                1.0f,  0.0f;
  d.scale(0.25f);
  d.translate(Vector2f(4.0f, 5.0f));
  e.linear() << 1.0f,  2.0f,
                3.0f,  5.0f;
  e.scale(0.125f);
  e.translate(Vector2f(0.0f, 3.0f));

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
  //     / \
  //    B   C
  //       / \
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

