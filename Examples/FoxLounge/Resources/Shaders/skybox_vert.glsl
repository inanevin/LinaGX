#version 450
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 0) out vec3 outWorldPos;

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 proj;
	vec4 skyColor1;
	vec4 skyColor2;
} sceneData;

void main()
{
    outWorldPos = inPosition;
	mat4 rotView = mat4(mat3(sceneData.view));
	vec4 clipPos = sceneData.proj * rotView * vec4(outWorldPos, 1.0);
	gl_Position = clipPos.xyww;
}

