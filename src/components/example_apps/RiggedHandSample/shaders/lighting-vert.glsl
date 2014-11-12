#version 120

const int MAXBONES = 92;

uniform mat4 projection_times_model_view_matrix;
uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;

// attribute arrays
attribute vec3 position;
attribute vec3 normal;
attribute vec2 tex_coord;
attribute vec4 boneWeights;
attribute vec4 boneIndices;

uniform bool isAnimated;
uniform mat4 boneMatrices[MAXBONES];
uniform mat4 invTransposeMatrices[MAXBONES];

varying vec3 outPosition;
varying vec3 outNormal;
varying vec2 outTexCoord;

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
	outPosition = (model_view_matrix * pos).xyz;
	outNormal = (normal_matrix * norm).xyz;
 	outTexCoord = tex_coord;
 	outTexCoord.y = 1.0 - outTexCoord.y;

	gl_Position = projection_times_model_view_matrix * pos;
}
