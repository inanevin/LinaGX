#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 inBoneIndices;
layout(location = 4) in vec4 inBoneWeights;
layout (location = 0) out vec3 outWorldPos;

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 projection;
	vec4 skyColor1;
	vec4 skyColor2;
    vec4 camPos;
	int lightCount;
    int padding[3];
} sceneData;


void main()
{
    outWorldPos = inPosition;
	mat4 rotView = mat4(mat3(sceneData.view));
	vec4 clipPos = sceneData.projection * rotView * vec4(outWorldPos, 1.0);
	gl_Position = clipPos.xyww;
}

