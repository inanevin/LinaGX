
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

layout (set = 0, binding = 2) uniform sampler defaultSampler;

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

layout (set = 2, binding = 1) uniform texture2D textures[4];

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

float encode(float n) {
    return n * 0.5 + 0.5;
}

const vec3 boundingMin = vec3(-100.0, -100.0, -100.0);
const vec3 boundingMax = vec3(100.0, 100.0, 100.0);

vec3 encodev3(vec3 p) {
    return (p - boundingMin) / (boundingMax - boundingMin);
}

void main()
{
  // Terrain has special texture, silly way to detect that.
  if(materialData.specialTexture != -1)
  {
      vec2 usedUV = vec2(inUV.x, inUV.y);
      vec2 tiledUV = usedUV * vec2(16,16);
      vec4 txtBaseColor = texture(sampler2D(textures[0], defaultSampler), tiledUV);
      vec4 txtNormal = texture(sampler2D(textures[1], defaultSampler), tiledUV);
      vec4 txtMetallicRoughness = texture(sampler2D(textures[2], defaultSampler), tiledUV);
  
      float alphaFactor1 = length(inUV - vec2(0.5, 0.5));
      float alphaFactor2 = clamp((0.5 - alphaFactor1) * 2.190, 0.0, 1.0);
      float alphaFactor = pow(alphaFactor2, 2.0);
      float alphaFactorExp = clamp(pow(alphaFactor, 6), 0.0, 1.0);
  
      vec2 uvSample1 = usedUV * vec2(17.1, 16);
      vec4 noiseSampled1 = texture(sampler2D(textures[3], defaultSampler), uvSample1) * 0.02f;
      vec2 uvSample2 = vec2(usedUV.x, usedUV.y - 1.0f) * vec2(2.1, 2.1) + noiseSampled1.xy;
      vec4 noiseSampled2 = texture(sampler2D(textures[3], defaultSampler), uvSample2);
      vec4 noiseClamped = clamp(noiseSampled2, vec4(0.0), vec4(1.0));
  
      // fin base color
      vec4 finalBaseColor = txtBaseColor * noiseClamped;
  
      float roughnessFactor = (noiseClamped - vec4(0.65)).r * alphaFactorExp;
      roughnessFactor = clamp(roughnessFactor * 34, 0.0, 1.0);
  
      // fin normal
      vec3 finalNormal = mix(getNormalFromMap(txtNormal.rgb), vec3(0.0), roughnessFactor);
      // finalNormal = getNormalFromMap(txtNormal.rgb);

      // fin roughness
      float finalRoughness = clamp(txtMetallicRoughness.g - roughnessFactor, 0.0, 1.0);
  
	  outAlbedoRoughness = vec4(finalBaseColor.rgb, finalRoughness);
      outNormalMetallic = vec4(finalNormal, 0.0f);
      outPositionAO = vec4(inWorldPos, 0.0f);
  }
  else
  {
     vec4 txtEmissive = texture(sampler2D(textures[3], defaultSampler), inUV) * materialData.emissiveFac;
	 vec4 txtBaseColor = texture(sampler2D(textures[0], defaultSampler), inUV) * materialData.baseColorFac + txtEmissive;
	 vec4 txtNormal = texture(sampler2D(textures[1], defaultSampler), inUV);
	 vec4 txtMetallicRoughness = texture(sampler2D(textures[2], defaultSampler), inUV);
     vec3 finalNormal = getNormalFromMap(txtNormal.rgb);

     outAlbedoRoughness = vec4(txtBaseColor.rgb, txtMetallicRoughness.g * materialData.roughness);
     outNormalMetallic = vec4(getNormalFromMap(txtNormal.rgb),  txtMetallicRoughness.b * materialData.metallic);
     outPositionAO = vec4(inWorldPos, 0.0f);
  }
	
}


