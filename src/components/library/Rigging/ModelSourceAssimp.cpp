#include "ModelSourceAssimp.h"
#include "Skeleton.h"

#include "assimp/postprocess.h"

namespace ai {
  unsigned int flags =
    aiProcess_Triangulate |
    aiProcess_FlipUVs |
    aiProcess_FixInfacingNormals |
    aiProcess_FindInstances |
    aiProcess_ValidateDataStructure |
    aiProcess_OptimizeMeshes |
    aiProcess_CalcTangentSpace |
    aiProcess_GenSmoothNormals |
    aiProcess_JoinIdenticalVertices |
    aiProcess_ImproveCacheLocality |
    aiProcess_LimitBoneWeights |
    aiProcess_RemoveRedundantMaterials |
    aiProcess_GenUVCoords |
    aiProcess_SortByPType |
    aiProcess_FindDegenerates |
    aiProcess_FindInvalidData |
    aiProcess_OptimizeMeshes;
  //	aiProcess_SplitLargeMeshes |
  //	aiProcess_FindInstances |

  Eigen::Vector3f get(const aiVector3D &v)
  {
    return Eigen::Vector3f(v.x, v.y, v.z);
  }

  Eigen::Quaternionf get(const aiQuaternion &q)
  {
    return Eigen::Quaternionf(q.w, q.x, q.y, q.z);
  }

  Eigen::Matrix4f get(const aiMatrix4x4 &m, float positionScaleFactor)
  {
    Eigen::Matrix4f result;
    result << m.a1, m.a2, m.a3, positionScaleFactor*m.a4,
      m.b1, m.b2, m.b3, positionScaleFactor*m.b4,
      m.c1, m.c2, m.c3, positionScaleFactor*m.c4,
      m.d1, m.d2, m.d3, m.d4;
    return result;
  }

  Rgba<float> get(const aiColor4D &c)
  {
    return Rgba<float>(c.r, c.g, c.b, c.a);
  }

  std::string get(const aiString &s)
  {
    return std::string(s.data);
  }

  std::vector<Eigen::Vector3f> getPositions(const aiMesh* aimesh, float positionScaleFactor)
  {
    std::vector<Eigen::Vector3f> positions;
    for (unsigned int i=0; i < aimesh->mNumVertices; ++i) {
      positions.push_back(positionScaleFactor * ai::get(aimesh->mVertices[i]));
    }
    return positions;
  }

  std::vector<Eigen::Vector3f> getNormals(const aiMesh* aimesh)
  {
    std::vector<Eigen::Vector3f> normals;
    for (unsigned int i=0; i < aimesh->mNumVertices; ++i) {
      normals.push_back(ai::get(aimesh->mNormals[i]));
    }
    return normals;
  }

  std::vector<Eigen::Vector2f> getTexCoords(const aiMesh* aimesh)
  {
    std::vector<Eigen::Vector2f> texCoords;
    for (unsigned int i=0; i < aimesh->mNumVertices; ++i) {
      texCoords.push_back(Eigen::Vector2f(aimesh->mTextureCoords[0][i].x,
        aimesh->mTextureCoords[0][i].y));
    }
    assert(texCoords.size() > 0);
    return texCoords;
  }

  std::vector<uint32_t> getIndices(const aiMesh* aimesh)
  {
    std::vector<uint32_t> indices;
    for (unsigned int i=0; i < aimesh->mNumFaces; ++i) {
      aiFace aiface = aimesh->mFaces[i];
      unsigned numIndices = aiface.mNumIndices;
      assert(numIndices <= 3);
      for (int n=0; n < 3; ++n) {
        if (numIndices == 2 && n == 2) {
          indices.push_back(aiface.mIndices[1]);
        } else {
          indices.push_back(aiface.mIndices[n]);
        }
      }
    }
    return indices;
  }

  model::MaterialInfo getTexture(const aiScene* aiscene, const aiMesh *aimesh, const std::string& modelPath, const std::string& rootPath)
  {
    model::MaterialInfo matInfo;
    // Handle material info
    aiMaterial *mtl = aiscene->mMaterials[aimesh->mMaterialIndex];

    aiString name;
    mtl->Get(AI_MATKEY_NAME, name);
    // Culling
    int twoSided;
    if ((AI_SUCCESS == mtl->Get(AI_MATKEY_TWOSIDED, twoSided)) && twoSided) {
      matInfo.mTwoSided = true;
    } else {
      matInfo.mTwoSided = false;
    }

    aiColor4D dcolor, scolor, acolor, ecolor, tcolor;
    if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_DIFFUSE, dcolor)) {
      matInfo.mDiffuse = ai::get(dcolor);
    }
    if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_SPECULAR, scolor)) {
      matInfo.mSpecular = ai::get(scolor);
    }
    if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_AMBIENT, acolor)) {
      matInfo.mAmbient = ai::get(acolor);
    }
    if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_EMISSIVE, ecolor)) {
      matInfo.mEmission = ai::get(ecolor);
    }
    if (AI_SUCCESS == mtl->Get(AI_MATKEY_COLOR_TRANSPARENT, tcolor)) {
      matInfo.mTransparentColor = ai::get(tcolor);
    }

    return matInfo;
  }

  std::vector<model::BoneWeights> getBoneWeights(const aiMesh* aimesh, const model::Skeleton* skeleton, float scaleFactor)
  {
    std::vector<model::BoneWeights> boneWeights;
    unsigned int nbBones = aimesh->mNumBones;
    std::string name = ai::get(aimesh->mName);

    // Create a list of empty bone weights mirroring the # of vertices
    for (unsigned v=0; v < aimesh->mNumVertices; ++v) {
      boneWeights.push_back(model::BoneWeights());
    }

    for (unsigned b=0; b < nbBones; ++b){
      model::NodeRef bone = skeleton->getBone(ai::get(aimesh->mBones[b]->mName));

      // Set the bone offset matrix if it hasn't been already
      if (bone->getOffset() == nullptr) {
        bone->setOffsetMatrix(ai::get(aimesh->mBones[b]->mOffsetMatrix, scaleFactor));
      }

      // Add the bone weight information to the correct vertex index
      aiBone* aibone = aimesh->mBones[b];
      for (unsigned int w=0; w<aibone->mNumWeights; ++w) {
        float weight = aibone->mWeights[w].mWeight;
        int boneWeightIndex = int(aibone->mWeights[w].mVertexId);
        boneWeights[boneWeightIndex].addWeight(bone, weight);
      }
    }
    return boneWeights;
  }

  const aiNode* findMeshNode(const std::string& meshName, const aiScene* aiscene, const aiNode* ainode)
  {
    for (unsigned i=0; i<ainode->mNumMeshes; ++i) {
      if (meshName == ai::get(aiscene->mMeshes[ainode->mMeshes[i]]->mName)) {
        return ainode;
      }
    }

    for (unsigned n=0; n < ainode->mNumChildren; ++n) {
      const aiNode* goalNode = findMeshNode(meshName, aiscene, ainode->mChildren[n]);
      if (goalNode != nullptr)
        return goalNode;
    }

    return nullptr;
  }

  model::NodeRef generateNodeHierarchy(model::Skeleton* skeleton,
    const aiNode* ainode,
    const std::unordered_set<std::string>& boneNames,
    float scaleFactor,
    const std::shared_ptr<model::Node>& parent = nullptr,
    const Eigen::Matrix4f& derivedTrans = Eigen::Matrix4f::Identity(),
    int level = 0)
  {
    assert(ainode);

    Eigen::Matrix4f derivedTransformation(derivedTrans);
    derivedTransformation *= ai::get(ainode->mTransformation, scaleFactor);
    std::string name = ai::get(ainode->mName);

    // store transform
    aiVector3D position, scaling;
    aiQuaternion rotation;
    ainode->mTransformation.Decompose(scaling, rotation, position);
    model::NodeRef node = model::NodeRef(new model::Node(scaleFactor * ai::get(position), ai::get(rotation), ai::get(scaling), name, parent, level));

    if (boneNames.count(name) > 0) {
      skeleton->addBone(name, node);
    }

    for (unsigned int c=0; c < ainode->mNumChildren; ++c) {
      model::NodeRef child = generateNodeHierarchy(skeleton, ainode->mChildren[c], boneNames, scaleFactor, node, derivedTransformation, level + 1);
      node->addChild(child);
    }
    return node;
  }

  model::SkeletonRef getSkeleton(const aiScene* aiscene, bool hasAnimations, float scaleFactor, const aiNode* root)
  {
    root = (root) ? root : aiscene->mRootNode;

    std::unordered_set<std::string> boneNames;
    for (unsigned int m=0; m < aiscene->mNumMeshes; ++m) {
      aiMesh * mesh = aiscene->mMeshes[m];
      for (unsigned int b=0; b < mesh->mNumBones; ++b) {
        std::string name = ai::get(mesh->mBones[b]->mName);
        boneNames.insert(name);
      }
    }

    model::SkeletonRef skeleton = model::Skeleton::create();
    skeleton->setRootNode(generateNodeHierarchy(skeleton.get(), root, boneNames, scaleFactor));
    return skeleton;
  }

} //end namespace ai

namespace model {

  void ModelSourceAssimp::SectionInfo::log()
  {
  }

  ModelSourceAssimp::ModelSourceAssimp(const std::string& modelPath, const std::string& rootAssetFolderPath, float scaleFactor)
  {
    mScaleFactor = scaleFactor;
    mModelPath = modelPath;
    mRootAssetFolderPath = rootAssetFolderPath;

    mImporter = std::unique_ptr<Assimp::Importer>(new Assimp::Importer());
    mImporter->SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
    mAiScene = mImporter->ReadFile(mModelPath, ai::flags);

    if (!mAiScene) {
      throw LoadErrorException(mImporter->GetErrorString());
    } else if (!mAiScene->HasMeshes()) {
      throw LoadErrorException("Scene has no meshes.");
    }

    for (unsigned int m=0; m < mAiScene->mNumMeshes; ++m) {
      aiMesh * mesh = mAiScene->mMeshes[m];
      SectionInfo sectionInfo;
      sectionInfo.mHasSkeleton = mesh->HasBones();
      sectionInfo.mHasNormals = mesh->HasNormals();
      sectionInfo.mNumVertices = mesh->mNumVertices;
      sectionInfo.mNumIndices = 3 * mesh->mNumFaces;
      sectionInfo.mHasMaterials = mAiScene->HasMaterials() && mesh->GetNumUVChannels() > 0;
      mSections.push_back(sectionInfo);
    }

    mHasAnimations = mAiScene->HasAnimations();
    for (const SectionInfo& section : mSections) {
      mHasSkeleton = mHasSkeleton || section.mHasSkeleton;
    }
  }

  ModelSourceAssimpRef ModelSourceAssimp::create(const std::string& modelPath, const std::string& rootAssetFolderPath, float scaleFactor)
  {
    return ModelSourceAssimpRef(new ModelSourceAssimp(modelPath, rootAssetFolderPath, scaleFactor));
  }

  void ModelSourceAssimp::load(ModelTarget *target)
  {
    SkeletonRef skeleton = target->getSkeleton();
    if (mHasSkeleton && skeleton == nullptr) {
      skeleton = ai::getSkeleton(mAiScene, mHasAnimations, mScaleFactor);
    }

    for (unsigned int i=0; i< mAiScene->mNumMeshes; ++i) {
      mSections[i].log();

      const aiMesh* aimesh = mAiScene->mMeshes[i];
      std::string name = ai::get(aimesh->mName);

      target->setActiveSection(i);
      target->loadName(name);
      target->loadIndices(ai::getIndices(aimesh));
      target->loadVertexPositions(ai::getPositions(aimesh, mScaleFactor));

      if (mSections[i].mHasNormals) {
        target->loadVertexNormals(ai::getNormals(aimesh));
      }

      if (mSections[i].mHasMaterials) {
        target->loadTex(ai::getTexCoords(aimesh),
          ai::getTexture(mAiScene, aimesh, mModelPath, mRootAssetFolderPath));
      }

      if (mSections[i].mHasSkeleton && skeleton) {
        target->loadSkeleton(skeleton);
        target->loadBoneWeights(ai::getBoneWeights(aimesh, skeleton.get(), mScaleFactor));
      } else {
        const aiNode* ainode = ai::findMeshNode(name, mAiScene, mAiScene->mRootNode);
        if (ainode) {
          target->loadDefaultTransformation(ai::get(ainode->mTransformation, mScaleFactor));
        }
      }
    }
  }

} //end namespace model
