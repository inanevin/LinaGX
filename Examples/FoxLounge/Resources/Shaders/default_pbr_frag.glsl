// https://learnopengl.com/PBR

#version 450
layout(location = 0) in vec2 inUV;
layout(location = 1) in vec3 inWorldPos;
layout(location = 2) in vec3 inNormal;
layout(location = 0) out vec4 FragColor;

#define LIGHTS 1

struct LightData
{
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform SceneData
{
    mat4 viewProj;
    vec4 camPosition;
    LightData lights[LIGHTS];
} scene;

layout(set = 2, binding = 0) uniform MaterialData
{
    vec4 baseColorTint;
    vec4 emissiveOcclusionFactors;
    vec4 metallicRoughnessAlphaCutoff;
} material;


layout (set = 2, binding = 1) uniform sampler2D albedo;
layout (set = 2, binding = 2) uniform sampler2D normal;
layout (set = 2, binding = 3) uniform sampler2D metallicRoughness;
layout (set = 2, binding = 4) uniform sampler2D ao;
layout (set = 2, binding = 5) uniform sampler2D emissive;

const float PI = 3.14159265359;
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normal, inUV).xyz * 2.0 - 1.0;

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
    vec4 albedoTexture = texture(albedo, inUV) * material.baseColorTint;
    
    if(albedoTexture.a < material.metallicRoughnessAlphaCutoff.z)
    {
        discard;
    }

    vec3 albedo     = albedoTexture.rgb;
    vec4 metallicRoughness = texture(metallicRoughness, inUV);
    float metallic  = metallicRoughness.r * material.metallicRoughnessAlphaCutoff.x;
    float roughness = metallicRoughness.g * material.metallicRoughnessAlphaCutoff.y;
    float ao        = texture(ao, inUV).r;

    vec3 camPos = vec3(scene.camPosition.x, scene.camPosition.y, scene.camPosition.z);
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - inWorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    for(int i = 0; i < LIGHTS; ++i) 
    {
        // calculate per-light radiance
        vec3 lightPos = vec3(scene.lights[i].position.x, scene.lights[i].position.y, scene.lights[i].position.z);
        vec3 lightCol = vec3(scene.lights[i].color.x, scene.lights[i].color.y, scene.lights[i].color.z);
        vec3 L = normalize(lightPos - inWorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPos - inWorldPos);
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
    }   

    vec3 ambient = vec3(0.05) * albedo;
    ambient = mix(ambient, ambient * ao, material.emissiveOcclusionFactors.a);

    vec3 color = ambient + Lo;
    color = color / (color + vec3(1.0));
    FragColor = vec4(color, 1.0);
}


