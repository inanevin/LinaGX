#version 450
layout (location = 0) in vec3 inWorldPos;
layout(location = 0) out vec4 outAlbedoRoughness;
layout(location = 1) out vec4 outNormalMetallic;
layout(location = 2) out vec4 outPosition;
layout(location = 3) out vec4 outEmission;

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


float rand(vec2 co)
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    vec3 normalizedDir = normalize(inWorldPos);

	float factor = clamp(normalizedDir.y * 1.67f + 0.940, 0.0f, 1.0f);
	vec4 color = mix(sceneData.skyColor1, sceneData.skyColor2, factor);
	float dither = rand(gl_FragCoord.xy) * 0.005; // 0.005 is the dither intensity
    outAlbedoRoughness = color + vec4(dither, dither, dither, 0.0f);
    outAlbedoRoughness.a = -1.0f;
	outNormalMetallic = vec4(0.0);
	outPosition = vec4(0.0);
    outEmission = vec4(0.0);
	// outFragColor = mix(sceneData.skyColor1, sceneData.skyColor2, factor);
	// outFragColor = vec4(inWorldPos, 1.0f);
}