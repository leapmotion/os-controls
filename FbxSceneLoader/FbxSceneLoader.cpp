#include "FbxSceneLoader.h"

#include "SDLController.h"

std::shared_ptr<FbxScene> ResourceLoader<FbxScene>::LoadResource (const std::string &name, ResourceManager<FbxScene> &calling_manager) {
  static FbxManager * gFbxManager = FbxManager::Create();

  FbxImporter * importer = FbxImporter::Create(gFbxManager, "");
  
  // Make a shared_ptr with the destructor of the shared_ptr calling Destroy()
  std::shared_ptr<FbxScene> scene(FbxScene::Create(gFbxManager, ""), [] (FbxScene* scene) {
    scene->Destroy();
  });
  
  std::string error;
  
  // Try initializing the file and importing the scene.  Using the default FbxIOSettings for now
  if (importer->Initialize(name.c_str())) {
    if (!importer->Import(scene.get())) {
      error = "error encountered while attempting to import scene from file \"" + name + "\"";
    }
  } else {
    error = "error encountered while attempting to initialize file \"" + name + "\"";
  }
  
  importer->Destroy();
  
  if (!error.empty()) {
    throw ResourceExceptionOfType<FbxScene>(error);
  }
  
  return scene;
}
