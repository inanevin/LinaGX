/*
This file is a part of: LinaVG
https://github.com/inanevin/LinaVG

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2022-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
            SH_LightingSimple,
            SH_LightingAdvanced,
            SH_Default,
            SH_Max,
        };

        enum RTType
        {
            RT_WorldDepth = 0,
            RT_GBufPositionAO,
            RT_GBufAlbedoRoughness,
            RT_GBufNormalMetallic,
            RT_LightingPass,
            RT_Reflections,
            RT_Max,
        };

        enum PassType
        {
            PS_ObjectsDefault = 0,
            PS_ObjectsReflections,
            PS_LightingDefault,
            PS_LightingReflections,
            PS_FinalQuad,
            PS_Max,
        };

        enum PipelineLayoutType
        {
            PL_GlobalSet = 0,
            PL_DefaultObjects,
            PL_LightingPassSimple,
            PL_LightingPassAdvanced,
            PL_FinalQuadPass,
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
            glm::vec4 padding[7];
        };

        struct GPUSceneData
        {
            glm::vec4 skyColor1;
            glm::vec4 skyColor2;
            glm::vec4 lightPos;
            glm::vec4 lightColor;
        };

        struct GPUObjectData
        {
            glm::mat4 modelMatrix;
            glm::mat4 bones[MAX_BONES];
            glm::mat4 normalMatrix;
            int       hasSkin     = 0;
            int       padding[15] = {0};
        };

        struct VertexSkinned
        {
            LinaGX::LGXVector3 position = {};
            LinaGX::LGXVector2 uv       = {};
            LinaGX::LGXVector3 normal   = {};
            // LinaGX::LGXVector4 inBoneIndices = {};
            // LinaGX::LGXVector4 inBoneWeights = {};
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

            // Matrix.
            static glm::mat4 TranslateRotateScale(const LinaGX::LGXVector3& pos, const LinaGX::LGXVector4& rot, const LinaGX::LGXVector3& scale);
            static glm::mat4 CalculateGlobalMatrix(LinaGX::ModelNode* node);

            static LinaGX::DescriptorSetDesc GetSetDescriptionGlobal();
            static LinaGX::DescriptorSetDesc GetSetDescriptionLightingPassSimple();
            static LinaGX::DescriptorSetDesc GetSetDescriptionLightingPassAdvanced();
            static LinaGX::DescriptorSetDesc GetSetDescriptionFinalPass();
            static LinaGX::DescriptorSetDesc GetSetDescriptionObjectPass();
            static LinaGX::DescriptorSetDesc GetSetDescriptionObjectMaterial();

            static LinaGX::TextureDesc GetRTDesc(const char* debugName, uint32 width, uint32 height);
            static LinaGX::TextureDesc GetRTDescCube(const char* debugName, uint32 width, uint32 height);
            static LinaGX::TextureDesc GetDepthDesc(const char* debugName, uint32 width, uint32 height);

            static uint32 CreateShader(LinaGX::Instance* lgx, const char* vertex, const char* fragment, LinaGX::CullMode cullMode, LinaGX::Format passFormat, LinaGX::CompareOp depthCompare, LinaGX::FrontFace front, bool depthWrite, bool gBuffer, bool useCustomLayout, uint16 layout, const char* debugName);
        };
    } // namespace Examples
} // namespace LinaGX

#endif
