
#version 450
 
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 inBoneIndices;
layout(location = 4) in vec4 inBoneWeights;

layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outWorldPos;
layout(location = 2) out vec3 outNormal;

#define MAX_BONES 31
#define MAX_BONE_INFLUENCES 4
#define LIGHTS 1

struct LightData
{
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 viewProj;
    vec4 camPosition;
    LightData lights[LIGHTS];
} scene;

struct Object
{
    mat4 modelMatrix;
    mat4 bones[MAX_BONES];
    mat4 normalMatrix;
};

layout(std140, set = 1, binding = 0) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout( push_constant ) uniform constants
{
	int objectID;
    int hasSkin;
} Constants;


void main() {
    vec4 skinnedPosition = vec4(0.0);

    for(int i = 0; i < MAX_BONE_INFLUENCES; ++i)
    {
        int boneIndex = int(inBoneIndices[i]);
        float boneWeight = inBoneWeights[i];

        mat4 boneMatrix = objectData.objects[Constants.objectID].bones[boneIndex];
        skinnedPosition += boneMatrix * vec4(inPosition, 1.0) * boneWeight;
    }

    if(Constants.hasSkin == 0)
        skinnedPosition = vec4(inPosition, 1.0);
        
    outWorldPos = vec3(objectData.objects[Constants.objectID].modelMatrix * skinnedPosition);
    outUV = vec2(uv.x, uv.y);
    outNormal = mat3(objectData.objects[Constants.objectID].normalMatrix) * normal;
    gl_Position = scene.viewProj * vec4(outWorldPos, 1.0);
}
