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
    float farPlane;
} sceneData;

layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform sampler samplers[2];

layout (set = 1, binding = 0) uniform CameraData
{
    mat4 view;
    mat4 projection;
    vec4 camPos;
} cameraData;

layout (set = 1, binding = 1) uniform texture2D gBuffer[2];
layout (set = 1, binding = 2) uniform texture2D noiseTexture;
layout (set = 1, binding = 3) uniform SSAOData
{
    vec4 samples[64];
} ssaoData;

layout( push_constant ) uniform constants
{
	int isHorizontal;
    int dummy;
} Constants;

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;
const vec2 noiseScale = vec2(800.0/4.0, 800.0/4.0); 


void main()
{
    vec3 fragPos = texture(sampler2D(gBuffer[0], samplers[0]), uv).rgb;
    vec3 normal = texture(sampler2D(gBuffer[1], samplers[0]), uv).rgb;

    //get input for SSAO algorithm
    vec3 randomVec = normalize(texture(sampler2D(noiseTexture, samplers[1]), uv * noiseScale).xyz);

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
        float sampleDepth = texture(sampler2D(gBuffer[0], samplers[0]), offset.xy).z; // get depth value of kernel sample

        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    FragColor = vec4(occlusion, occlusion, occlusion, GetFogFactor(fragPos.z / fragPos.w));
}


