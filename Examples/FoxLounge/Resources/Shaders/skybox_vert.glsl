#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
//layout(location = 3) in vec4 inBoneIndices;
//layout(location = 4) in vec4 inBoneWeights;
layout (location = 0) out vec3 outWorldPos;

#define MAX_BONES 30

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
    vec2 resolution;
    float farPlane;
} sceneData;

layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform sampler samplers[3];

layout (set = 1, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec4 camPos;
} cameraData;

void main()
{
    outWorldPos = inPosition;
	mat4 rotView = mat4(mat3(cameraData.view));
	vec4 clipPos = cameraData.projection * rotView * vec4(outWorldPos, 1.0);
	gl_Position = clipPos.xyww;
}

