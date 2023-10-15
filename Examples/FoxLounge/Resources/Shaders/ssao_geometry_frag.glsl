
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec4 inFragPos;
layout(location = 1) in vec3 inNormal;
layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;

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


float GetFogFactor(float coord)
{
    float density = 0.02f;
    float res = exp(density * coord);
    return 1.0 - clamp(res, 0.0, 1.0);
}

void main()
{
  outPosition = vec4(inFragPos.rgb, GetFogFactor(inFragPos.z / inFragPos.w));
  outNormal = vec4(normalize(inNormal), 1.0f);
}


