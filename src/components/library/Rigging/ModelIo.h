#pragma once

#include "EigenTypes.h"
#include "GLTexture2Image.h"
#include "Leap/GL/Rgba.h"

#include <array>
#include <memory>

namespace model {

  class Skeleton;
  class Node;
  class ModelTarget;

  typedef std::shared_ptr<class ModelSource> ModelSourceRef;
  typedef std::shared_ptr<ModelTarget> ModelTargetRef;

  struct MaterialInfo
  {
    MaterialInfo()
      : mTransparentColor(Leap::GL::Rgba<float>::One())
      , mAmbient(1.0f, 0.5f, 0.25f, 1.0f)
      , mDiffuse(0.0f, 0.0f, 0.0f, 1.0f)
      , mSpecular(0.0f, 0.0f, 0.0f, 1.0f)
      , mShininess(1.0f)
      , mEmission(0.0f, 0.0f, 0.0f, 1.0f)
      , mUseAlpha(false)
      , mHasMaterial(false)
      , mTwoSided(false)
    { }
    GLTexture2Image mTexture;
    Leap::GL::Rgba<float> mTransparentColor;
    Leap::GL::Rgba<float> mAmbient;
    Leap::GL::Rgba<float> mDiffuse;
    Leap::GL::Rgba<float> mSpecular;
    float mShininess;
    Leap::GL::Rgba<float> mEmission;
    bool mUseAlpha;
    bool mHasMaterial;
    bool mTwoSided;
  };

  class BoneWeights {
  public:
    BoneWeights() : mActiveNbWeights(0), mWeights() { }
    static const int NB_WEIGHTS = 4;
    void addWeight(const std::shared_ptr<Node>& bone, float weight);
    float getWeight(int index) const  { return mWeights[index]; }
    const std::shared_ptr<Node>&	getBone(int index) const { return mBones[index]; }
    size_t mActiveNbWeights;
  private:
    std::array<float, NB_WEIGHTS> mWeights;
    std::array<std::shared_ptr<Node>, NB_WEIGHTS> mBones;
  };

  class ModelSource {
  public:
    virtual size_t getNumSections() const = 0;
    virtual size_t getNumVertices(int section) const = 0;
    virtual size_t getNumIndices(int section)  const = 0;
    virtual bool hasNormals(int section) const = 0;
    virtual bool hasSkeleton(int section) const = 0;
    virtual bool hasMaterials(int section) const = 0;
    virtual bool hasAnimations() const = 0;

    virtual	void load(ModelTarget *target) = 0;
  };

  class ModelIoException : public std::exception
  {
  public:
    ModelIoException() { mMessage[0] = 0; }
    ModelIoException(const std::string &message) throw();

    virtual const char * what() const throw() { return mMessage; }

  private:
    char mMessage[256];
  };

  class LoadErrorException : public ModelIoException {
  public:
    LoadErrorException() : ModelIoException() { }
    LoadErrorException(const std::string &message) throw() : ModelIoException("Load error:" + message) { };
  };

  class ModelTargetException : public ModelIoException {
  public:
    ModelTargetException() : ModelIoException() {}
    ModelTargetException(const std::string &message) throw() : ModelIoException("Target error:" + message) { };
  };

  extern std::shared_ptr<class ModelSourceAssimp> loadModel(const std::string& modelPath, const std::string& rootAssetFolderPath, float scaleFactor);

  class ModelTarget {
  public:
    virtual void setActiveSection(int index);
    virtual std::shared_ptr<Skeleton> getSkeleton() const = 0;

    virtual void loadName(std::string name);
    virtual void loadVertexPositions(const std::vector<Eigen::Vector3f>& positions) = 0;
    virtual void loadIndices(const std::vector<uint32_t>& indices) = 0;
    virtual void loadTex(const std::vector<Eigen::Vector2f>& texCoords, const MaterialInfo& texInfo);
    virtual void loadVertexNormals(const std::vector<Eigen::Vector3f>& normals);
    virtual void loadSkeleton(const std::shared_ptr<Skeleton>& skeleton);
    virtual void loadBoneWeights(const std::vector<BoneWeights>& boneWeights);
    virtual void loadDefaultTransformation(const Eigen::Matrix4f& transformation);
  };

} //end namespace model
