#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 inUV;
layout(location = 0) out vec4 FragColor;

layout( push_constant ) uniform constants
{
	int objectID;
    int materialByteIndex;
} Constants;

layout(set = 2, binding = 0) readonly buffer MaterialData
{
   uint data[];
} materials;

layout (set = 2, binding = 1) uniform sampler2D textures[];

struct Material2
{
	vec4 baseColor;
    uint albedoIndex;
};

Material2 AsMaterial2(uint startIndex)
{
	Material2 mat;
	uint index = startIndex;

	// Base color
	mat.baseColor.r = uintBitsToFloat(materials.data[index++]);
	mat.baseColor.g = uintBitsToFloat(materials.data[index++]);
	mat.baseColor.b = uintBitsToFloat(materials.data[index++]);
	mat.baseColor.a = uintBitsToFloat(materials.data[index++]);

	// albedo index
    mat.albedoIndex = materials.data[index++];

	return mat;
}

void main()
{
	Material2 mat = AsMaterial2(Constants.materialByteIndex);
	vec4 albedo = texture(textures[mat.albedoIndex], inUV);
	FragColor = albedo * mat.baseColor;
}


