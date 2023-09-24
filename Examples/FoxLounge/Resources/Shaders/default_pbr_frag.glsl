
#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec3 inNormal;
layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outNormalMetallic;
layout(location = 2) out vec4 outPositionAO;

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
} sceneData;


layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform texture2D allTextures[];
layout (set = 0, binding = 3) uniform sampler defaultSampler;


layout (set = 1, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec4 camPos;
    vec4 padding[7];
} cameraData;


layout (set = 2, binding = 0) uniform MaterialData
{
    vec4 baseColorFac;
    vec4 emissiveFac;
    int baseColor;
    int metallicRoughness;
    int emissive;
    int normal;
    int specialTexture;
    float metallic;
    float roughness;
    float alphaCutoff;
} materialData;

layout( push_constant ) uniform constants
{
	int objectID;
    int dummy;
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
     // vec4 txtEmissive = texture(sampler2D(allTextures[materialData.emissive], defaultSampler), inUV) * materialData.emissiveFac;
	 // vec4 txtBaseColor = texture(sampler2D(allTextures[materialData.baseColor], defaultSampler), inUV) * materialData.baseColorFac + txtEmissive;
	 // vec4 txtNormal = texture(sampler2D(allTextures[materialData.normal], defaultSampler), inUV);
	 // vec4 txtMetallicRoughness = texture(sampler2D(allTextures[materialData.metallicRoughness], defaultSampler), inUV);
     // outAlbedoRoughness = vec4(txtBaseColor.rgb, txtMetallicRoughness.g * materialData.roughness);
     // outNormalMetallic = vec4(getNormalFromMap(txtNormal.rgb), txtMetallicRoughness.b * materialData.metallic);
     // outPositionAO = vec4(inWorldPos.rgb, 0.0f);

     outAlbedoRoughness = vec4(0.5f, 0.5f, 0.5, 1.0f);
     outNormalMetallic = vec4(1.0f);
     outPositionAO = vec4(inWorldPos.rgb, 0.0f);

   //// Terrain has special texture, silly way to detect that.
   //if(materialData.specialTexture != -1)
   //{
   //    vec2 usedUV = vec2(inUV.x, inUV.y);
   //    vec2 tiledUV = usedUV * vec2(16,16);
   //    vec4 txtBaseColor = texture(sampler2D(allTextures[materialData.baseColor], defaultSampler), tiledUV);
   //    vec4 txtNormal = texture(sampler2D(allTextures[materialData.normal], defaultSampler), tiledUV);
   //    vec4 txtMetallicRoughness = texture(sampler2D(allTextures[materialData.metallicRoughness], defaultSampler), tiledUV);
   //
   //    float alphaFactor1 = length(inUV - vec2(0.5, 0.5));
   //    float alphaFactor2 = clamp((0.5 - alphaFactor1) * 2.190, 0.0, 1.0);
   //    float alphaFactor = pow(alphaFactor2, 2.0);
   //    float alphaFactorExp = clamp(pow(alphaFactor, 6), 0.0, 1.0);
   //
   //    vec2 uvSample1 = usedUV * vec2(17.1, 16);
   //    vec4 noiseSampled1 = texture(sampler2D(allTextures[materialData.specialTexture], defaultSampler), uvSample1) * 0.02f;
   //    vec2 uvSample2 = vec2(usedUV.x, usedUV.y - 1.0f) * vec2(2.1, 2.1) + noiseSampled1.xy;
   //    vec4 noiseSampled2 = texture(sampler2D(allTextures[materialData.specialTexture], defaultSampler), uvSample2);
   //    vec4 noiseClamped = clamp(noiseSampled2, vec4(0.0), vec4(1.0));
   //
   //    // fin base color
   //    vec4 finalBaseColor = txtBaseColor * noiseClamped;
   //
   //    float roughnessFactor = (noiseClamped - vec4(0.65)).r * alphaFactorExp;
   //    roughnessFactor = clamp(roughnessFactor * 34, 0.0, 1.0);
   //
   //    // fin normal
   //    vec4 finalNormal = txtNormal;
   //
   //    // fin roughness
   //    float finalRoughness = clamp(txtMetallicRoughness.g - roughnessFactor, 0.0, 1.0);
   //
	//    outAlbedoRoughness = vec4(finalBaseColor.rgb, finalRoughness);
   //    outNormalMetallic = vec4(getNormalFromMap(finalNormal.rgb), 0.0f);
   //    outPositionAO = vec4(inWorldPos.rgb, 0.0f);
   //}
   //else
   //{
	//    vec4 txtEmissive = texture(sampler2D(allTextures[materialData.emissive], defaultSampler), inUV) * materialData.emissiveFac;
	//    vec4 txtBaseColor = texture(sampler2D(allTextures[materialData.baseColor], defaultSampler), inUV) * materialData.baseColorFac + txtEmissive;
	//    vec4 txtNormal = texture(sampler2D(allTextures[materialData.normal], defaultSampler), inUV);
	//    vec4 txtMetallicRoughness = texture(sampler2D(allTextures[materialData.metallicRoughness], defaultSampler), inUV);
   //    outAlbedoRoughness = vec4(txtBaseColor.rgb, txtMetallicRoughness.g * materialData.roughness);
   //    outNormalMetallic = vec4(getNormalFromMap(txtNormal.rgb), txtMetallicRoughness.b * materialData.metallic);
   //    outPositionAO = vec4(inWorldPos.rgb, 0.0f);
   //}
	
}


