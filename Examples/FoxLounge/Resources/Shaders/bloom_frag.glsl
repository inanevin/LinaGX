#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_samplerless_texture_functions : enable

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
    vec2 resolution;
    float nearPlane;
    float farPlane;
} sceneData;

layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform sampler samplers[3];

layout (set = 1, binding = 0) uniform texture2D inputTexture;

layout( push_constant ) uniform constants
{
	int isHorizontal;
    int dummy;
} Constants;

float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / sceneData.resolution; // gets size of single texel
    vec3 result = texture(sampler2D(inputTexture, samplers[2]), uv).rgb * weight[0]; // current fragment's contribution
    if(Constants.isHorizontal == 1)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(sampler2D(inputTexture, samplers[2]), uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(sampler2D(inputTexture, samplers[2]), uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(sampler2D(inputTexture, samplers[2]), uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(sampler2D(inputTexture, samplers[2]), uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}


