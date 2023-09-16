#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 FragColor;

layout(set = 0, binding = 0) uniform SceneData
{
	mat4 view;
	mat4 proj;
	vec4 skyColor1;
	vec4 skyColor2;
} sceneData;

layout (set = 1, binding = 0) uniform texture2D allTextures[];
layout (set = 1, binding = 1) uniform sampler allSamplers[];

layout (push_constant) uniform constants
{
    uint textureID;
    uint samplerID;
} Constants;

void main()
{
    FragColor = texture(sampler2D(allTextures[Constants.textureID], allSamplers[Constants.samplerID]), uv);
}


