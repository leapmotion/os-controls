#pragma once

#include "ModelIo.h"
#include <vector>

#include "EigenTypes.h"

namespace model {

class Skeleton;
class SkinnedVboMesh;

class ModelTargetSkinnedVboMesh : public ModelTarget {
public:	
	ModelTargetSkinnedVboMesh( SkinnedVboMesh *mesh );
	
	virtual void	setActiveSection( int index ) override;
	virtual std::shared_ptr<Skeleton> getSkeleton() const override;
	
	virtual void	loadVertexPositions( const std::vector<Eigen::Vector3f>& positions ) override;
	virtual void	loadVertexNormals( const std::vector<Eigen::Vector3f>& normals ) override;
	virtual void	loadIndices( const std::vector<uint32_t>& indices ) override;
	virtual void	loadTex( const std::vector<Eigen::Vector2f>& texCoords, const MaterialInfo& matInfo ) override;
  	virtual void	loadSkeleton( const std::shared_ptr<Skeleton>& skeleton ) override;
	virtual void	loadBoneWeights( const std::vector<BoneWeights>& boneWeights ) override;
 	virtual void	loadDefaultTransformation( const Eigen::Matrix4f& transformation ) override;
private:
  	SkinnedVboMesh*			mSkinnedVboMesh;
};

} //end namespace model