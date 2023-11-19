/* 
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

Author: Inan Evin
http://www.inanevin.com

The 2-Clause BSD License

Copyright (c) [2023-] Inan Evin

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef EXAMPLEUTILITY_HPP
#define EXAMPLEUTILITY_HPP

#include "LinaGX/LinaGX.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/quaternion.hpp"
#include <string>
#include <unordered_map>

namespace LinaGX
{
    namespace Examples
    {

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2
#define BACKBUFFER_COUNT 2
#define MAX_BONES        30

        typedef std::unordered_map<LinaGX::GLTFTextureType, const char*> TextureMapping;

        enum Shader
        {
            SH_Quad = 0,
            SH_Lighting,
            SH_Default,
            SH_Skybox,
            SH_Irradiance,
            SH_Prefilter,
            SH_BRDF,
            SH_Shadows,
            SH_Bloom,
            SH_SSAOGeometry,
            SH_Max,
        };

        enum PassType
        {
            PS_Objects = 0,
            PS_Lighting,
            PS_FinalQuad,
            PS_Irradiance,
            PS_Prefilter,
            PS_BRDF,
            PS_Shadows,
            PS_BloomHori,
            PS_BloomVert,
            PS_SSAOGeometry,
            PS_Max,
        };

        enum PipelineLayoutType
        {
            PL_GlobalSet = 0,
            PL_Objects,
            PL_SSAOGeometry,
            PL_Simple,
            PL_Lighting,
            PL_SimpleQuad,
            PL_FinalQuad,
            PL_Irradiance,
            PL_Max,
        };

        struct GPUConstants
        {
            uint32 int1;
            uint32 int2;
        };

        struct GPUMaterialData
        {
            glm::vec4 baseColorFac      = {};
            glm::vec4 emissiveFac       = {};
            int32     baseColor         = 0;
            int32     metallicRoughness = 0;
            int32     emissive          = -1;
            int32     normal            = 0;
            int32     specialTexture    = -1;
            float     metallic          = 0.0f;
            float     roughness         = 0.0f;
            float     alphaCutoff       = 0.0f;
        };

        struct GPUCameraData
        {
            glm::mat4 view;
            glm::mat4 proj;
            glm::vec4 camPosition;
        };

        struct GPUSceneData
        {
            glm::vec4 skyColor1;
            glm::vec4 skyColor2;
            glm::vec4 lightPos;
            glm::vec4 lightColor;
            glm::vec2 resolution;
            float     nearPlane;
            float     farPlane;
        };

        struct GPUObjectData
        {
            glm::mat4 modelMatrix;
            glm::mat4 bones[MAX_BONES];
            glm::mat4 normalMatrix;
            int       hasSkin     = 0;
            int       padding[15] = {0};
        };

        struct GPUSSAOData
        {
            glm::vec4 samples[64];
        };

        struct Vertex
        {
            LinaGX::LGXVector3 position = {};
            LinaGX::LGXVector2 uv       = {};
            LinaGX::LGXVector3 normal   = {};
            // LinaGX::LGXVector4 inBoneIndices = {};
            // LinaGX::LGXVector4 inBoneWeights = {};
        };

        enum DrawObjectFlags
        {
            DrawDefault = 1 << 0,
            DrawSkybox  = 1 << 1,
        };

        struct GPUBuffer
        {
            uint32 staging = 0;
            uint32 gpu     = 0;
        };

        class Utility
        {
        public:
            // Texture.
            static std::unordered_map<LINAGX_STRINGID, TextureMapping> GetMaterialTextureMapping();
            static std::vector<LINAGX_STRINGID>                        GetLinearTextures();

            static uint32 GetPaddedItemSize(uint32 itemSize, uint32 alignment);

            static std::string PassToString(PassType pt);

            // Matrix.
            static glm::mat4 TranslateRotateScale(const LinaGX::LGXVector3& pos, const LinaGX::LGXVector4& rot, const LinaGX::LGXVector3& scale);
            static glm::mat4 CalculateGlobalMatrix(LinaGX::ModelNode* node);

            static LinaGX::DescriptorSetDesc GetSetDescriptionGlobal();
            static LinaGX::DescriptorSetDesc GetSetDescriptionLightingPass();
            static LinaGX::DescriptorSetDesc GetSetDescriptionSimpleQuadPass();
            static LinaGX::DescriptorSetDesc GetSetDescriptionFinalPass();
            static LinaGX::DescriptorSetDesc GetSetDescriptionObjectPass();
            static LinaGX::DescriptorSetDesc GetSetDescriptionObjectMaterial();
            static LinaGX::DescriptorSetDesc GetSetDescriptionIrradiancePass();

            static LinaGX::TextureDesc GetRTDesc(const char* debugName, uint32 width, uint32 height);
            static LinaGX::TextureDesc GetTxtDescCube(const char* debugName, uint32 width, uint32 height);
            static LinaGX::TextureDesc GetRTDescCube(const char* debugName, uint32 width, uint32 height);
            static LinaGX::TextureDesc GetRTDescCubeDepth(const char* debugName, uint32 width, uint32 height);
            static LinaGX::TextureDesc GetRTDescPrefilter(const char* debugName, uint32 width, uint32 height, uint32 mipLevels = 0);
            static LinaGX::TextureDesc GetDepthDesc(const char* debugName, uint32 width, uint32 height);

            static uint32 CreateShader(LinaGX::Instance* lgx, const char* vertex, const char* fragment, LinaGX::CullMode cullMode, LinaGX::Format passFormat, LinaGX::CompareOp depthCompare, LinaGX::FrontFace front, bool depthWrite, uint32 attachmentCount, bool useCustomLayout, uint16 layout, const char* debugName);
        };
    } // namespace Examples
} // namespace LinaGX

#endif
