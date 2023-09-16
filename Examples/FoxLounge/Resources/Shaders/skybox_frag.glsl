#version 450
layout (location = 0) in vec3 inWorldPos;
layout (location = 0) out vec4 outFragColor;

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 proj;
	vec4 skyColor1;
	vec4 skyColor2;
} sceneData;

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
    outFragColor = color + vec4(dither, dither, dither, 0.0);
	// outFragColor = mix(sceneData.skyColor1, sceneData.skyColor2, factor);
	// outFragColor = vec4(inWorldPos, 1.0f);
}