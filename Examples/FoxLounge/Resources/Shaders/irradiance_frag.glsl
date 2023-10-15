#version 450
layout (location = 0) in vec3 inWorldPos;
layout(location = 0) out vec4 outFragColor;

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
    float farPlane;
    float fxaaReduceMin;
    float fxaaReduceMul;
    float fxaaSpanMax;
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

layout (set = 1, binding = 1) uniform textureCube environmentMap;

const float PI = 3.14159265359;

void main()
{

    // The world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
   vec3 N = normalize(vec3(inWorldPos.r, inWorldPos.g, -inWorldPos.b));

    vec3 irradiance = vec3(0.0);   
    
    // tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(up, N));
    up         = normalize(cross(N, right));
       
    float sampleDelta = 0.025;
    float nrSamples = 0.0;

    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // spherical to cartesian (in tangent space)
            vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N; 

            irradiance += texture(samplerCube(environmentMap, samplers[0]), sampleVec).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));
    outFragColor = vec4(irradiance, 1.0);
}