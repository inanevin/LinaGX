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

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 projection;
	vec4 skyColor1;
	vec4 skyColor2;
    vec4 camPos;
	int lightCount;
    int padding[3];
} sceneData;

struct Object
{
    mat4 modelMatrix;
    mat4 bones[MAX_BONES];
    mat4 normalMatrix;
    int hasSkin;
    int padding[15];
};

layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout( push_constant ) uniform constants
{
	int objectID;
    int materialID;
} Constants;

void main() {
    Object object = objectData.objects[Constants.objectID];
    outWorldPos = vec3(object.modelMatrix * vec4(inPosition, 1.0));
    outUV = vec2(uv.x, uv.y);
    outNormal = mat3(object.normalMatrix) * normal;
    gl_Position = sceneData.projection * sceneData.view *  vec4(outWorldPos, 1.0);
}
