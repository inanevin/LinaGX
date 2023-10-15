#version 450
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_samplerless_texture_functions : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 outBloom;

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

layout (set = 1, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec4 camPos;
} cameraData;

layout (set = 1, binding = 1) uniform texture2D gBuffer[5];
layout (set = 1, binding = 2) uniform textureCube irradianceMap;
layout (set = 1, binding = 3) uniform textureCube prefilterMap;
layout (set = 1, binding = 4) uniform texture2D brdfLUT;
layout (set = 1, binding = 5) uniform textureCube shadowsDepth;
layout (set = 1, binding = 6) uniform texture2D ssaoTextures[3];
layout (set = 1, binding = 7) uniform SSAOData
{
    vec4 samples[64];
} ssaoData;

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

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  


// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, vec3 lightPos, vec3 viewPos)
{
    vec3 fragToLight = fragPos - lightPos; 
    fragToLight = vec3(fragToLight.r, fragToLight.g, -fragToLight.b);
    float currentDepth = length(fragToLight);


    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / sceneData.farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(samplerCube(shadowsDepth, samplers[1]), fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *=  sceneData.farPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);
    return shadow;

    //float closestDepth = texture(samplerCube(shadowsDepth, samplers[1]), fragToLight).r;
    //closestDepth *= sceneData.farPlane;
    /// float bias = max(0.05 * (1.0 - dot(normal, fragToLight)), 0.005);
    //float bias =0.05;
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    //return shadow;
}  

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;
const vec2 noiseScale = vec2(800.0/4.0, 800.0/4.0); 

float CalculateSSAO(vec3 initialFragPos)
{
    vec2 noiseScale = vec2(sceneData.resolution.x / 4.0, sceneData.resolution.y / 4.0);
    vec3 fragPos = initialFragPos;

    vec3 normal = texture(sampler2D(ssaoTextures[1], samplers[0]), uv).rgb;

    //get input for SSAO algorithm
    vec3 randomVec = normalize(texture(sampler2D(ssaoTextures[2], samplers[1]), uv * noiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 smp = ssaoData.samples[i].rgb;
        vec3 samplePos = TBN * smp; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = cameraData.projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        offset.y = 1.0f - offset.y;

        // get sample depth
        float sampleDepth = texture(sampler2D(ssaoTextures[0], samplers[0]), offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    return occlusion;
}


void main()
{
    vec4 ssaoAndFogFragPos = texture(sampler2D(ssaoTextures[0], samplers[0]), uv);

    vec4 albedoRoughness = texture(sampler2D(gBuffer[0], samplers[0]), uv);

    if(albedoRoughness.a < 0.0f)
    {
        FragColor = vec4(albedoRoughness.rgb, 1.0f);
        outBloom = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 normalMetallic = texture(sampler2D(gBuffer[1], samplers[0]), uv);
    vec4 position = texture(sampler2D(gBuffer[2], samplers[0]), uv);
    vec4 emission = texture(sampler2D(gBuffer[3], samplers[0]), uv);

    vec3 albedo = albedoRoughness.rgb;
    float metallic = normalMetallic.a;
    float roughness = albedoRoughness.a;
    vec3 N = normalMetallic.rgb;
    vec3 camPos = vec3(cameraData.camPos.x, cameraData.camPos.y, cameraData.camPos.z);
    vec3 V = normalize(camPos - position.rgb);
	vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // calculate per-light radiance
    vec3 lightPos = vec3(sceneData.lightPosition.x, sceneData.lightPosition.y, sceneData.lightPosition.z);
    vec3 lightCol = vec3(sceneData.lightColor.x, sceneData.lightColor.y, sceneData.lightColor.z);

    {
        vec3 L = normalize(lightPos - position.rgb);
        vec3 H = normalize(V + L);
        float distance = length(lightPos - position.rgb);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightCol * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = (numerator / denominator);
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // vec3 reflectionContribution =  environment.rgb * clamp(0.3 - roughness, 0.0, 1.0);
        //Lo += reflectionContribution;

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

       
    float shadow = ShadowCalculation(position.rgb, lightPos, camPos);
    Lo *= (1.0 - shadow);
 
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    
    vec3 irradiance = texture(samplerCube(irradianceMap, samplers[0]), N).rgb;
    vec3 diffuse      = irradiance * albedo * CalculateSSAO(ssaoAndFogFragPos.rgb);
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(samplerCube(prefilterMap, samplers[0]), R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(sampler2D(brdfLUT, samplers[0]), vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y) * 0.2;
    vec3 ambient = (kD * diffuse + specular);

    vec3 color = ambient  + Lo;
    color += emission.rgb;
    vec3 fogColor = mix(vec3(sceneData.skyColor1), vec3(sceneData.skyColor2), uv.y); // Linearly interpolate between your gradient skybox colors
    color = mix(color, sceneData.skyColor2.rgb, ssaoAndFogFragPos.w);

    float brightness = dot(emission.rgb, vec3(0.2126, 0.7152, 0.0722));

    if(brightness > 1.0)
        outBloom = vec4(emission.rgb, 1.0);
    else
        outBloom = vec4(0.0, 0.0, 0.0, 1.0);

    FragColor = vec4(color, 1.0f);

}


