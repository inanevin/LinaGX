// Uniform buffer object in descriptor set 0, binding 0
layout(set = 0, binding = 0) uniform UniformBufferObject {
    vec4 aq;
    vec2 hehe;
    float xd;
    vec4 arr[5];
    mat3 aqq;
    ivec2 iv;
} ubo;

layout( push_constant ) uniform constants
{
	vec4 data;
	mat4 render_matrix;
} PushConstants;


layout(set = 0, binding = 1) buffer StorageBuffer {
    vec4 data[];
} hmm;

// Texture sampler in descriptor set 1, binding 0
layout(set = 0, binding = 2) uniform sampler2D texSampler;
