const int MAXBONES = 92;

attribute vec3 position;
attribute vec3 normal;
attribute vec2 texcoord;
attribute vec4 boneWeights;
attribute vec4 boneIndices;

uniform mat4 modelView;
uniform mat4 projection;
uniform mat4 normalMatrix;

uniform bool isAnimated;
uniform mat4 boneMatrices[MAXBONES];
uniform mat4 invTransposeMatrices[MAXBONES];

varying vec3 outPosition;
varying vec3 outNormal;
varying vec2 outTexCoord;
varying mat3 modelToCamera;

void main()
{	
	vec4 pos = vec4(position, 1.0);
	vec4 norm = vec4(normal, 0.0);
	if( isAnimated ) {
		pos =	boneMatrices[int(boneIndices.x)] * pos * boneWeights.x +
		boneMatrices[int(boneIndices.y)] * pos * boneWeights.y +
		boneMatrices[int(boneIndices.z)] * pos * boneWeights.z +
		boneMatrices[int(boneIndices.w)] * pos * boneWeights.w ;
		
		norm =  invTransposeMatrices[int(boneIndices.x)] * norm * boneWeights.x +
		invTransposeMatrices[int(boneIndices.y)] * norm * boneWeights.y +
		invTransposeMatrices[int(boneIndices.z)] * norm * boneWeights.z +
		invTransposeMatrices[int(boneIndices.w)] * norm * boneWeights.w ;
		pos.w = 1.0;
		norm.w = 0.0;
	}
	outPosition = (modelView * pos).xyz;
	outNormal = (normalMatrix * norm).xyz;
  outTexCoord = texcoord;
  outTexCoord.y = 1.0 - outTexCoord.y;

	gl_Position = projection * modelView * pos;
}