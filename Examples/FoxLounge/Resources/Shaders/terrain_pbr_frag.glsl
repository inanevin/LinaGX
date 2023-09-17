
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec3 inNormal;
layout(location = 0) out vec4 FragColor;

#define MAX_BONES 30

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 projection;
	vec4 skyColor1;
	vec4 skyColor2;
	vec4 pad[2];
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
	int objectID;
    int material;
} Constants;


void main()
{
	Material mat = materials.materialData[Constants.material];
    vec2 tiledUV = inUV * vec2(16,16);
    vec4 txtBaseColor = texture(sampler2D(allTextures[mat.baseColor], allSamplers[0]), tiledUV);
    vec4 txtNormal = texture(sampler2D(allTextures[mat.normal], allSamplers[0]), tiledUV);
    vec4 txtMetallicRoughness = texture(sampler2D(allTextures[mat.metallicRoughness], allSamplers[0]), tiledUV);
    float alphaFactor = clamp(pow(clamp((0.5 - length(inUV * vec2(0.5, 0.5))) * 2.190, 0.0, 1.0), 2.0), 0.0, 1.0);

    vec2 uv1 = inUV * vec2(1,1);
    vec2 uvSample1 = uv1 * vec2(17.1, 16);
    vec4 noiseSampled1 = texture(sampler2D(allTextures[mat.specialTexture], allSamplers[0]), uvSample1) * 0.02;
    vec2 uvSample2 = uv1 * vec2(2.1, 2.1) + noiseSampled1.xy;
    vec4 noiseSampled2 = texture(sampler2D(allTextures[mat.specialTexture], allSamplers[0]), uvSample2);
    float noiseClamped = clamp(noiseSampled2, vec4(0.0), vec4(1.0)).r;
    vec4 finalBaseColor = txtBaseColor * noiseClamped;

    float normalFactor = clamp(((noiseClamped -0.65) * clamp(pow(alphaFactor, 6), 0.0, 1.0) * 34.0), 0.0, 1.0);
    vec4 finalNormal = mix(txtNormal, vec4(0.0), normalFactor);
    float finalRoughness = clamp(txtMetallicRoughness.r - normalFactor, 0.0, 1.0);
	FragColor = finalBaseColor;
}


