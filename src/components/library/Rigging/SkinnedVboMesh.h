#pragma once

#include "AMeshSection.h"

#include "Leap/GL/Shader.h"
#include "Leap/GL/VertexBuffer.h"

#include <vector>
#include <memory>

using namespace Leap::GL;

namespace model {

  typedef std::shared_ptr<class SkinnedVboMesh> SkinnedVboMeshRef;

  typedef VertexBuffer<VertexAttribute<GL_FLOAT_VEC3>, // position
    VertexAttribute<GL_FLOAT_VEC3>, // normal
    VertexAttribute<GL_FLOAT_VEC2>, // tex coord
    VertexAttribute<GL_FLOAT_VEC4>, // bone weights
    VertexAttribute<GL_FLOAT_VEC4>> // bone indices
    VertexBuffer;

  typedef VertexBuffer::Attributes VertexAttributes;

  class SkinnedVboMesh
  {
  public:
    static const int MAXBONES = 92;

    struct MeshSection : public AMeshSection
    {
      MeshSection();
      void updateMesh(bool enableSkinning = true) override;

      VertexBuffer& getVboMesh() { return mVboMesh; }
      const VertexBuffer& getVboMesh() const { return mVboMesh; }
      void setVboMesh(size_t numVertices, size_t numIndices, GLenum primitiveType);

      Buffer& getIndices() { return mIndices; }
      const Buffer& getIndices() const { return mIndices; }

      std::array<Eigen::Matrix4f, MAXBONES>* mBoneMatricesPtr;
      std::array<Eigen::Matrix4f, MAXBONES>* mInvTransposeMatricesPtr;
      EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    private:
      VertexBuffer mVboMesh;
      Buffer mIndices;
    };
    typedef std::shared_ptr<SkinnedVboMesh::MeshSection> MeshVboSectionRef;

    static SkinnedVboMeshRef create(ModelSourceRef modelSource, std::shared_ptr<Skeleton> skeleton = nullptr);

    void update();

    MeshVboSectionRef& getActiveSection() { return mActiveSection; }
    const MeshVboSectionRef& getActiveSection() const { return mActiveSection; }
    MeshVboSectionRef& setActiveSection(int index);
    std::vector<MeshVboSectionRef>& getSections() { return mMeshSections; }
    const std::vector<MeshVboSectionRef>& getSections() const { return mMeshSections; }

    std::shared_ptr<Skeleton>& getSkeleton() { return mActiveSection->getSkeleton(); }
    const std::shared_ptr<Skeleton>& getSkeleton() const { return mActiveSection->getSkeleton(); }
    void setSkeleton(const std::shared_ptr<Skeleton>& skeleton) { mActiveSection->setSkeleton(skeleton); }
    bool hasSkeleton() const { return mActiveSection->getSkeleton() != nullptr; }

    void setDefaultTransformation(const Eigen::Matrix4f& transformation) { mActiveSection->setDefaultTransformation(transformation); }
    const Eigen::Matrix4f& getDefaultTranformation() const { return mActiveSection->getDefaultTranformation(); }

    void setEnableSkinning(bool enabled) { mEnableSkinning = enabled; }

    friend struct SkinnedVboMesh::MeshSection;

    std::array<Eigen::Matrix4f, MAXBONES> mBoneMatrices;
    std::array<Eigen::Matrix4f, MAXBONES> mInvTransposeMatrices;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  protected:
    bool mEnableSkinning;
    SkinnedVboMesh(ModelSourceRef modelSource, std::shared_ptr<Skeleton> skeleton = nullptr);
    MeshVboSectionRef mActiveSection;
    std::vector< MeshVboSectionRef > mMeshSections;
  };


  typedef std::shared_ptr<SkinnedVboMesh::MeshSection> MeshVboSectionRef;

} //end namespace model
