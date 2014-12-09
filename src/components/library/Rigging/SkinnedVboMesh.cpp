#include "SkinnedVboMesh.h"
#include "ModelTargetSkinnedVboMesh.h"

#include "Skeleton.h"

namespace model {

  SkinnedVboMesh::MeshSection::MeshSection()
    : AMeshSection(), mVboMesh(GL_STATIC_DRAW)
  { }

  void SkinnedVboMesh::MeshSection::setVboMesh(size_t numVertices, size_t numIndices, GLenum primitiveType)
  {
    std::vector<VertexAttributes>& attributes = mVboMesh.IntermediateAttributes();
    attributes.resize(numVertices);
    if (!mIndices.IsInitialized()) {
      mIndices.Initialize(GL_ELEMENT_ARRAY_BUFFER);
    }
  }

  void SkinnedVboMesh::MeshSection::updateMesh(bool enableSkinning)
  {
    if (enableSkinning) {
      int i = 0;
      for (const auto& entry : mSkeleton->getBoneNames()) {
        if (i >= MAXBONES) {
          break;
        }
        NodeRef bone = entry.second;
        (*mBoneMatricesPtr)[bone->getBoneIndex()] = bone->getAbsoluteTransformation() * *bone->getOffset();
        (*mInvTransposeMatricesPtr)[bone->getBoneIndex()] = (*mBoneMatricesPtr)[bone->getBoneIndex()].inverse().transpose();
        ++i;
      }
      mIsAnimated = true;
    } else if (mIsAnimated) {
      mIsAnimated = false;
    }
  }

  SkinnedVboMeshRef SkinnedVboMesh::create(ModelSourceRef modelSource, SkeletonRef skeleton)
  {
    return SkinnedVboMeshRef(new SkinnedVboMesh(modelSource, skeleton));
  }

  SkinnedVboMesh::SkinnedVboMesh(ModelSourceRef modelSource, SkeletonRef skeleton)
    : mEnableSkinning(true)
  {
    assert(modelSource->getNumSections() > 0);

    for (unsigned int i = 0; i< modelSource->getNumSections(); ++i) {
      MeshVboSectionRef section = std::shared_ptr<SkinnedVboMesh::MeshSection>(new SkinnedVboMesh::MeshSection());
      section->setVboMesh(modelSource->getNumVertices(i), modelSource->getNumIndices(i), GL_TRIANGLES);
      section->setSkeleton(skeleton);
      mMeshSections.push_back(section);
    }
    mActiveSection = mMeshSections[0];

    ModelTargetSkinnedVboMesh target(this);
    modelSource->load(&target);

    for (size_t i=0; i<mMeshSections.size(); i++) {
      mMeshSections[i]->getVboMesh().UploadIntermediateAttributes();
      mMeshSections[i]->getVboMesh().ClearIntermediateAttributes();
    }
  }

  MeshVboSectionRef& SkinnedVboMesh::setActiveSection(int index)
  {
    assert(index >= 0 && index < (int)mMeshSections.size());
    mActiveSection = mMeshSections[index];
    return mActiveSection;
  }

  void SkinnedVboMesh::update()
  {
    for (MeshVboSectionRef section : mMeshSections) {
      section->updateMesh(mEnableSkinning);
    }
  }

} //end namespace model
