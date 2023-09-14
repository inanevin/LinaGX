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

void main()
{
	float factor = clamp(inWorldPos.y * 1.67f + 0.940, 0.0f, 1.0f);
	outFragColor = mix(sceneData.skyColor1, sceneData.skyColor2, factor);
	// outFragColor = vec4(inWorldPos, 1.0f);
}