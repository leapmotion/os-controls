#include "ModelTargetSkinnedVboMesh.h"

#include "Skeleton.h"
#include "SkinnedVboMesh.h"

namespace model {

  ModelTargetSkinnedVboMesh::ModelTargetSkinnedVboMesh(SkinnedVboMesh * mesh)
    : mSkinnedVboMesh(mesh)
  {

  }

  void ModelTargetSkinnedVboMesh::setActiveSection(int index)
  {
    mSkinnedVboMesh->setActiveSection(index);
  }

  std::shared_ptr<Skeleton> ModelTargetSkinnedVboMesh::getSkeleton() const
  {
    return mSkinnedVboMesh->getSkeleton();
  }

  void ModelTargetSkinnedVboMesh::loadIndices(const std::vector<uint32_t>& indices)
  {
    const int numBytes = static_cast<int>(indices.size() * sizeof(uint32_t));
    BufferObject& indexBufferObject = mSkinnedVboMesh->getActiveSection()->getIndices();
    indexBufferObject.Bind();
    indexBufferObject.BufferData(indices.data(), numBytes, GL_STATIC_DRAW);
    indexBufferObject.Unbind();
  }

  template <int DIM>
  struct FloatVec {
    FloatVec(const Eigen::Matrix<float, DIM, 1>& eigenVec) {
      for (int i=0; i<DIM; i++) {
        data[i] = eigenVec[i];
      }
    }
    float data[DIM];
  };

  void ModelTargetSkinnedVboMesh::loadVertexPositions(const std::vector<Eigen::Vector3f>& positions)
  {
    VertexBuffer& buffer = mSkinnedVboMesh->getActiveSection()->getVboMesh();
    std::vector<VertexBuffer::Attributes>& attributes = buffer.IntermediateAttributes();
    for (size_t i=0; i<positions.size(); i++) {
      std::get<0>(attributes[i]).ReinterpretAs<FloatVec<3>>() = FloatVec<3>(positions[i]);
    }
  }

  void ModelTargetSkinnedVboMesh::loadVertexNormals(const std::vector<Eigen::Vector3f>& normals)
  {
    mSkinnedVboMesh->getActiveSection()->setHasNormals(true); //FIXME: remove this
    VertexBuffer& buffer = mSkinnedVboMesh->getActiveSection()->getVboMesh();
    std::vector<VertexBuffer::Attributes>& attributes = buffer.IntermediateAttributes();
    for (size_t i=0; i<normals.size(); i++) {
      std::get<1>(attributes[i]).ReinterpretAs<FloatVec<3>>() = FloatVec<3>(normals[i]);
    }
  }

  void ModelTargetSkinnedVboMesh::loadTex(const std::vector<Eigen::Vector2f>& texCoords, const MaterialInfo& matInfo)
  {
    mSkinnedVboMesh->getActiveSection()->setMatInfo(matInfo);

    VertexBuffer& buffer = mSkinnedVboMesh->getActiveSection()->getVboMesh();
    std::vector<VertexBuffer::Attributes>& attributes = buffer.IntermediateAttributes();
    for (size_t i=0; i<texCoords.size(); i++) {
      std::get<2>(attributes[i]).ReinterpretAs<FloatVec<2>>() = FloatVec<2>(texCoords[i]);
    }
  }

  void ModelTargetSkinnedVboMesh::loadSkeleton(const SkeletonRef& skeleton)
  {
    if (skeleton->getNumBones() > SkinnedVboMesh::MAXBONES)
      throw ModelTargetException("Skeleton has more bones than maximal the number allowed.");

    mSkinnedVboMesh->setSkeleton(skeleton);
  }

  void ModelTargetSkinnedVboMesh::loadBoneWeights(const std::vector<BoneWeights>& boneWeights)
  {
    VertexBuffer& buffer = mSkinnedVboMesh->getActiveSection()->getVboMesh();
    std::vector<VertexBuffer::Attributes>& attributes = buffer.IntermediateAttributes();

    for (size_t i=0; i<boneWeights.size(); i++) {
      const BoneWeights& boneWeight = boneWeights[i];
      VertexBuffer::Attributes& cur = attributes[i];
      FloatVec<4>& vWeights = std::get<3>(attributes[i]).ReinterpretAs<FloatVec<4>>();
      FloatVec<4>& vIndices = std::get<4>(attributes[i]).ReinterpretAs<FloatVec<4>>();
      vWeights = FloatVec<4>(Eigen::Vector4f::Zero());
      vIndices = FloatVec<4>(Eigen::Vector4f::Zero());
      for (unsigned int b =0; b < boneWeight.mActiveNbWeights; ++b) {
        NodeRef bone = boneWeight.getBone(b);
        vWeights.data[b] = boneWeight.getWeight(b);
        //FIXME: Maybe use ints on the desktop?
        vIndices.data[b] = static_cast<float>(bone->getBoneIndex());
      }
    }

    mSkinnedVboMesh->getActiveSection()->mBoneMatricesPtr = &mSkinnedVboMesh->mBoneMatrices;
    mSkinnedVboMesh->getActiveSection()->mInvTransposeMatricesPtr = &mSkinnedVboMesh->mInvTransposeMatrices;
  }

  void ModelTargetSkinnedVboMesh::loadDefaultTransformation(const Eigen::Matrix4f& transformation)
  {
    mSkinnedVboMesh->setDefaultTransformation(transformation);
  }

} //end namespace model
