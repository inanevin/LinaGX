
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec3 inNormal;
layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outNormalMetallic;
layout(location = 2) out vec4 outPositionAO;

#define MAX_BONES 30

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 projection;
	vec4 skyColor1;
	vec4 skyColor2;
    vec4 camPos;
	vec4 lightPosition;
    vec4 lightColor;
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

vec3 getNormalFromMap(vec3 normalMap)
{
    vec3 tangentNormal = normalMap * 2.0 - 1.0;

    vec3 Q1  = dFdx(inWorldPos);
    vec3 Q2  = dFdy(inWorldPos);
    vec2 st1 = dFdx(inUV);
    vec2 st2 = dFdy(inUV);

    vec3 N   = normalize(inNormal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main()
{
	Material mat = materials.materialData[Constants.material];
	vec4 txtEmissive = texture(sampler2D(allTextures[mat.emissive], allSamplers[0]), inUV) * mat.emissiveFac;
	vec4 txtBaseColor = texture(sampler2D(allTextures[mat.baseColor], allSamplers[0]), inUV) * mat.baseColorFac + txtEmissive;
	vec4 txtNormal = texture(sampler2D(allTextures[mat.normal], allSamplers[0]), inUV);
	vec4 txtMetallicRoughness = texture(sampler2D(allTextures[mat.metallicRoughness], allSamplers[0]), inUV);

    outAlbedoRoughness = vec4(txtBaseColor.rgb, txtMetallicRoughness.g * mat.roughness);
    outNormalMetallic = vec4(getNormalFromMap(txtNormal.rgb), txtMetallicRoughness.b * mat.metallic);
    outPositionAO = vec4(inWorldPos.rgb, 0.0f);
}


