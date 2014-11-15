#include "ModelIo.h"
#include "Node.h"
#include "ModelSourceAssimp.h"

namespace model {

  void BoneWeights::addWeight(const NodeRef& bone, float weight) {
    static const float EPSILON = static_cast<float>(1E-5);
    if (mActiveNbWeights == NB_WEIGHTS) {
      // TODO: we should warn the user here?
      return;
    } else {
      mWeights[mActiveNbWeights] = weight;
      mBones[mActiveNbWeights] = bone;
      ++mActiveNbWeights;

      // If we have NB_WEIGHTS bone weights and their sum isn't 1.0,
      // we renormalize the weights.
      if (mActiveNbWeights == NB_WEIGHTS) {
        float sum = 0.0f;
        for (int i=0; i < NB_WEIGHTS; ++i) {
          sum += mWeights[i];
        }

        if (sum < 1.0f - EPSILON && sum !=0) {
          for (int i=0; i < NB_WEIGHTS; ++i) {
            mWeights[i] *= 1.0f / sum;
          }
        }
      }
    }
  }


  void ModelTarget::setActiveSection(int index) { }

  void ModelTarget::loadName(std::string name) { }

  void ModelTarget::loadTex(const std::vector<Eigen::Vector2f>& texCoords, const MaterialInfo& texInfo) { }

  void ModelTarget::loadVertexNormals(const std::vector<Eigen::Vector3f>& normals) { }

  void ModelTarget::loadSkeleton(const std::shared_ptr<Skeleton>& skeleton) { }

  void ModelTarget::loadBoneWeights(const std::vector<BoneWeights>& boneWeights) { }

  void ModelTarget::loadDefaultTransformation(const Eigen::Matrix4f& transformation) { }

  ModelSourceAssimpRef loadModel(const std::string& modelPath, const std::string& rootAssetFolderPath, float scaleFactor)
  {
    return ModelSourceAssimp::create(modelPath, rootAssetFolderPath, scaleFactor);
  }

  ModelIoException::ModelIoException(const std::string &message) throw()
    //: ModelIoException() no constructor delegation in VS2012 :(
  {
    mMessage[0] = 0;
    strncpy(mMessage, message.c_str(), 255);
  }

} //end namespace model
