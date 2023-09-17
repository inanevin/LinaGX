#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout (set = 1, binding = 0) uniform texture2D allTextures[];
layout (set = 1, binding = 1) uniform sampler allSamplers[];

struct Material
{
	vec4 baseColorFac;
    vec4 emissiveFac;
    uint baseColor;
    uint metallicRoughness;
    uint emissive;
    uint normal;
    uint specialTexture;
    float metallic;
    float roughness;
    float alphaCutoff;
};

layout (set = 2, binding = 0) readonly buffer MaterialData
{
	Material materialData[];
} materials;

layout( push_constant ) uniform constants
{
	int textureID;
    int samplerID;
} Constants;


void main()
{
    FragColor = texture(sampler2D(allTextures[Constants.textureID + 1], allSamplers[Constants.samplerID]), uv);
}


