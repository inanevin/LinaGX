#version 450
#extension GL_EXT_nonuniform_qualifier : enable

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
    vec2 resolution;
    float nearPlane;
    float farPlane;
} sceneData;

layout(std430, set = 0, binding = 1) readonly buffer ObjectData
{
    Object objects[];
} objectData;

layout (set = 0, binding = 2) uniform sampler samplers[3];

layout (set = 1, binding = 0) uniform texture2D inputTexture;
layout (set = 1, binding = 1) uniform texture2D bloomTexture;

const float FXAA_REDUCE_MIN = 1.0f / 128.0f;
const float FXAA_REDUCE_MUL = 1.0f / 8.0f;
const float FXAA_SPAN_MAX   = 2.0f;

vec4 fxaa()
{
  vec4 color = vec4(0.0);
  vec3 luma = vec3(0.299, 0.587, 0.114);
  float lumaNW = dot(texture(sampler2D(inputTexture, samplers[0]), uv + vec2(-1.0, -1.0)/sceneData.resolution).rgb, luma);
  float lumaNE = dot(texture(sampler2D(inputTexture, samplers[0]), uv + vec2(1.0, -1.0)/sceneData.resolution).rgb, luma);
  float lumaSW = dot(texture(sampler2D(inputTexture, samplers[0]), uv + vec2(-1.0, 1.0)/sceneData.resolution).rgb, luma);
  float lumaSE = dot(texture(sampler2D(inputTexture, samplers[0]), uv + vec2(1.0, 1.0)/sceneData.resolution).rgb, luma);
  float lumaM  = dot(texture(sampler2D(inputTexture, samplers[0]), uv).rgb, luma);
  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
  
  vec2 fac = ((lumaNW + lumaNE) - (lumaSW + lumaSE)) < ((lumaNW + lumaSW) - (lumaNE + lumaSE)) ? vec2(0.0, 1.0) : vec2(1.0, 0.0);
  vec2 dir = vec2(-1.0, -1.0) + vec2(1.0, 1.0) * fac;
  
  float dirReduce = max(
      (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
      FXAA_REDUCE_MIN);
  
  float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
  dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
            max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
            dir * rcpDirMin)) / sceneData.resolution;
  
  vec3 rgbA = (1.0/2.0) * (
      texture(sampler2D(inputTexture, samplers[0]), uv.xy + dir * (1.0/3.0 - 0.5)).rgb +
      texture(sampler2D(inputTexture, samplers[0]), uv.xy + dir * (2.0/3.0 - 0.5)).rgb);
  vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
      texture(sampler2D(inputTexture, samplers[0]), uv.xy + dir * (0.0/3.0 - 0.5)).rgb +
      texture(sampler2D(inputTexture, samplers[0]), uv.xy + dir * (3.0/3.0 - 0.5)).rgb);
      
  float lumaB = dot(rgbB, luma);
  if ((lumaB < lumaMin) || (lumaB > lumaMax))
      color.rgb = rgbA;
  else
      color.rgb = rgbB;
  
  return color;
}
void main()
{
    vec4 txt = fxaa();
    vec4 bloom = texture(sampler2D(bloomTexture, samplers[2]), uv);
    txt += bloom * 0.3;
    vec3 mapped = vec3(1.0) - exp(-txt.rgb * 0.2f);
    FragColor = vec4(mapped, 1.0);
}


