#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

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
	int textureID;
    int samplerID;
} Constants;

const float PI = 3.14159265359;


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{

    vec4 albedoRoughness = texture(sampler2D(allTextures[Constants.textureID], allSamplers[Constants.samplerID]), uv);
    vec4 normalMetallic = texture(sampler2D(allTextures[Constants.textureID + 1], allSamplers[Constants.samplerID]), uv);
    vec4 positionAO = texture(sampler2D(allTextures[Constants.textureID + 2], allSamplers[Constants.samplerID]), uv);

    vec3 albedo = albedoRoughness.rgb;
    float metallic = normalMetallic.a;
    float roughness = albedoRoughness.a;
    vec3 N = normalMetallic.rgb;
    vec3 camPos = vec3(sceneData.camPos.x, sceneData.camPos.y, sceneData.camPos.z);
    vec3 V = normalize(camPos - positionAO.rgb);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // calculate per-light radiance
    vec3 lightPos = vec3(sceneData.lightPosition.x, sceneData.lightPosition.y, sceneData.lightPosition.z);
    vec3 lightCol = vec3(sceneData.lightColor.x, sceneData.lightColor.y, sceneData.lightColor.z);
    vec3 L = normalize(lightPos - positionAO.rgb);
    vec3 H = normalize(V + L);
    float distance = length(lightPos - positionAO.rgb);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = lightCol * attenuation;

    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);   
    float G   = GeometrySmith(N, V, L, roughness);      
    vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
       
    vec3 numerator    = NDF * G * F; 
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
    vec3 specular = numerator / denominator;
    
    // kS is equal to Fresnel
    vec3 kS = F;
    // for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light); to preserve this
    // relationship the diffuse component (kD) should equal 1.0 - kS.
    vec3 kD = vec3(1.0) - kS;
    // multiply kD by the inverse metalness such that only non-metals 
    // have diffuse lighting, or a linear blend if partly metal (pure metals
    // have no diffuse light).
    kD *= 1.0 - metallic;	  

    // scale light by NdotL
    float NdotL = max(dot(N, L), 0.0);        

    // add to outgoing radiance Lo
    Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    
    vec3 ambient = vec3(0.05) * albedo + Lo;
    vec3 mapped = vec3(1.0) - exp(-ambient * 1.0f);
    // FragColor = texture(sampler2D(allTextures[Constants.textureID + 1], allSamplers[Constants.samplerID]), uv);
    FragColor = vec4(mapped, 1.0f);
}


