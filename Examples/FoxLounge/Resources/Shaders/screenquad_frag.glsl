#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

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
    float farPlane;
} sceneData;

layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform sampler samplers[2];

layout (set = 1, binding = 0) uniform texture2D inputTexture;
layout (set = 1, binding = 1) uniform texture2D bloomTexture;

void main()
{
    vec4 txt = texture(sampler2D(inputTexture, samplers[0]), uv);
    vec4 bloom = texture(sampler2D(bloomTexture, samplers[0]), uv);
    txt += bloom * 0.3;

    vec3 mapped = vec3(1.0) - exp(-txt.rgb * 0.2f);
    FragColor = vec4(mapped, 1.0);
}


