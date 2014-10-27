#pragma once

#include "ModelIo.h"

#include "EigenTypes.h"
#include "Color.h"

#include "assimp/types.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "assimp/Importer.hpp"	//OO version Header!

#include <unordered_set>

namespace ai {

  //! Assimp loader settings/flags.
  extern unsigned int flags;

  //! Convert aiVector3D to Eigen::Vector3f.
  inline Eigen::Vector3f get(const aiVector3D &v);
  //! Convert aiQuaternion to Eigen::Quaternionf.
  inline Eigen::Quaternionf get(const aiQuaternion &q);
  //! Convert aiMatrix4x4 to Eigen::Matrix4f.
  inline Eigen::Matrix4f get(const aiMatrix4x4 &m);
  //! Convert aiColor4D to Color.
  inline Color get(const aiColor4D &c);
  //! Convert aiString to std::string.
  inline std::string get(const aiString &s);
  //! Extract vertex positions from an assimp mesh section.
  std::vector<Eigen::Vector3f> getPositions(const aiMesh* aimesh);
  //! Extract vertex normals from an assimp mesh section.
  std::vector<Eigen::Vector3f> getNormals(const aiMesh* aimesh);
  //! Extract vertex texture coordinates from an assimp mesh section.
  std::vector<Eigen::Vector2f> getTexCoords(const aiMesh* aimesh);
  //! Extract vertex indices from an assimp mesh section.
  std::vector<uint32_t> getIndices(const aiMesh* aimesh);
  //! Extract material information (including textures) for a mesh section.
  model::MaterialInfo getTexture(const aiScene* aiscene, const aiMesh *aimesh, const std::string& modelPath, const std::string& rootPath = "");
  //! Extract skeletal bone weights for each vertex of an assimp mesh section.
  std::vector<model::BoneWeights>	getBoneWeights(const aiMesh* aimesh, const model::Skeleton* skeleton);
  //! Extract a mesh section's default transformation (use when there is no bones)
  Eigen::Matrix4f getDefaultTransformation(const std::string& name, const aiScene* aiscene, model::Skeleton* skeleton);

  //! Construct skeleton from assimp scene.
  std::shared_ptr<class model::Skeleton> getSkeleton(const aiScene* aiscene,
    bool hasAnimations,
    const aiNode* root = nullptr);
  //! Traverse assimp nodes to find the aiNode with specified name.
  const aiNode* findMeshNode(const std::string& meshName,
    const aiScene* aiscene,
    const aiNode* ainode);
}

namespace model {

  typedef std::shared_ptr<ModelSourceAssimp> ModelSourceAssimpRef;

  class ModelSourceAssimp : public ModelSource {

    struct SectionInfo {
      SectionInfo()
        : mHasNormals(false), mHasSkeleton(false), mHasMaterials(false), mNumVertices(0), mNumIndices(0)
      { }
      bool mHasNormals;
      bool mHasSkeleton;
      bool mHasMaterials;
      size_t mNumVertices;
      size_t mNumIndices;
      void log();
    };
    bool mHasSkeleton;
    bool mHasAnimations;
  public:
    static ModelSourceAssimpRef	create(const std::string& modelPath, const std::string& rootAssetFolderPath = "");

    virtual size_t getNumSections() const override { return mSections.size(); }
    virtual size_t getNumVertices(int section = 0) const override { return mSections[section].mNumVertices; }
    virtual size_t getNumIndices(int section = 0)  const override { return mSections[section].mNumIndices; }
    virtual bool hasNormals(int section = 0) const override { return mSections[section].mHasNormals; }
    virtual bool hasSkeleton(int section = 0) const override { return mSections[section].mHasSkeleton; }
    virtual bool hasMaterials(int section = 0) const override { return mSections[section].mHasMaterials; }
    virtual bool hasAnimations() const override { return mHasAnimations; }

    virtual void load(ModelTarget *target) override;

  protected:
    ModelSourceAssimp(const std::string& modelPath, const std::string& rootAssetFolderPath = "");
  private:
    //! Assimp importer instance which cannot be destroyed until the scene loading is complete.
    std::unique_ptr<Assimp::Importer> mImporter;
    //! Assimp scene pointer which is parsed and loaded into cinder.
    const aiScene* mAiScene;

    //! File path to the model.
    std::string mModelPath;
    //! Root asset folder (textures in a model may not reside in the same directory as the model).
    std::string	mRootAssetFolderPath;
    //! Information extracted (upon class instantiation) from assimp about each model section
    std::vector<SectionInfo> mSections;
  };

} //end namespace model
