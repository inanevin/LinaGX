#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 inBoneIndices;
layout(location = 4) in vec4 inBoneWeights;
layout(location = 0) out vec2 outUV;
layout(location = 1) out vec3 outWorldPos;
layout(location = 2) out vec3 outNormal;

#define MAX_BONES 30
#define MAX_BONE_INFLUENCES 4

struct Object
{
    mat4 modelMatrix;
    mat4 bones[MAX_BONES];
    mat4 normalMatrix;
    int hasSkin;
    int padding[15];
};

layout(set = 0, binding = 0) uniform SceneData
{
	vec4 skyColor1;
	vec4 skyColor2;
	vec4 lightPosition;
    vec4 lightColor;
} sceneData;


layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform texture2D allTextures[];
layout (set = 0, binding = 3) uniform sampler allSamplers[];

layout (set = 1, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec4 camPos;
} cameraData;

layout( push_constant ) uniform constants
{
	int objectID;
    int applyTerrainEffects;
} Constants;

void main() {
    vec4 skinnedPosition = vec4(0.0);

    Object object = objectData.objects[Constants.objectID];

    for(int i = 0; i < MAX_BONE_INFLUENCES; ++i)
    {
        int boneIndex = int(inBoneIndices[i]);
        float boneWeight = inBoneWeights[i];
        mat4 boneMatrix = object.bones[boneIndex];
        skinnedPosition += boneMatrix * vec4(inPosition, 1.0) * boneWeight;
    }

    skinnedPosition = vec4(inPosition, 1.0);
        
    outWorldPos = vec3(object.modelMatrix * skinnedPosition);
    outUV = vec2(uv.x, uv.y);
    outNormal = mat3(object.normalMatrix) * normal;
    gl_Position = cameraData.projection * cameraData.view *  vec4(outWorldPos, 1.0);
}
