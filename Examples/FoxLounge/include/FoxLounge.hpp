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

#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#include "App.hpp"
#include "LinaGX/LinaGX.hpp"
#include "Camera.hpp"
#include "glm/glm.hpp"
#include <vector>
#include <string>

namespace LinaGX
{
    namespace Examples
    {

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2
#define BACKBUFFER_COUNT 2
#define MAX_BONES        30

        enum Shader
        {
            Skybox = 0,
            SCQuad,
            Terrain,
            Default,
            Max,
        };

        typedef std::unordered_map<LinaGX::GLTFTextureType, const char*> TextureMapping;

        struct PerFrameData
        {
            LinaGX::CommandStream* graphicsStream         = nullptr;
            LinaGX::CommandStream* transferStream         = nullptr;
            uint64                 transferSemaphoreValue = 0;
            uint16                 transferSemaphore      = 0;
            uint16                 dscSet0                = 0;
            uint16                 dscSet1                = 0;
            uint16                 dscSet2                = 0;
            uint32                 rscSceneData           = 0;
            uint8*                 rscSceneDataMapping    = nullptr;
            uint32                 rscObjDataCPU          = 0;
            uint32                 rscObjDataGPU          = 0;
            uint8*                 rscObjDataCPUMapping   = nullptr;
            uint32                 rscMatDataCPU          = 0;
            uint32                 rscMatDataGPU          = 0;
            uint8*                 rscMatDataCPUMapping   = nullptr;
            uint32                 rtWorldColor           = 0;
            uint32                 rtWorldDepth           = 0;
            uint32                 rtWorldColorIndex      = 0;
        };

        struct Texture2D
        {
            std::string                        path      = "";
            uint32                             gpuHandle = 0;
            std::vector<LinaGX::TextureBuffer> allLevels;
        };

        struct GPUMaterialData
        {
            glm::vec4 baseColorFac      = {};
            glm::vec4 emissiveFac       = {};
            uint32    baseColor         = 0;
            uint32    metallicRoughness = 0;
            uint32    emissive          = 0;
            uint32    normal            = 0;
            uint32    specialTexture    = 0;
            float     metallic          = 0.0f;
            float     roughness         = 0.0f;
            float     alphaCutoff       = 0.0f;
        };

        struct Material
        {
            std::string                                         name = "";
            GPUMaterialData                                     gpuMat;
            Shader                                              shader = Shader::Default;
            std::unordered_map<LinaGX::GLTFTextureType, uint32> textureIndices;
        };

        struct MeshPrimitive
        {
            uint32 materialIndex     = 0;
            uint32 vertexBufferStart = 0;
            uint32 indexBufferStart  = 0;
            uint32 indexCount        = 0;
        };

        struct WorldObject
        {
            glm::mat4                  globalMatrix;
            glm::mat4                  invBindMatrix;
            std::string                name      = "";
            bool                       isSkinned = false;
            bool                       hasMesh   = false;
            std::vector<MeshPrimitive> primitives;
            bool                       manualDraw = false;
        };

        struct VertexSkinned
        {
            LinaGX::LGXVector3 position      = {};
            LinaGX::LGXVector2 uv            = {};
            LinaGX::LGXVector3 normal        = {};
            LinaGX::LGXVector4 inBoneIndices = {};
            LinaGX::LGXVector4 inBoneWeights = {};
        };

        struct VertexBuffer
        {
            uint32 staging = 0;
            uint32 gpu     = 0;
        };

        struct GPUSceneData
        {
            glm::mat4 view;
            glm::mat4 proj;
            glm::vec4 skyColor1;
            glm::vec4 skyColor2;
            glm::vec4 pad[2];
        };

        struct GPUObjectData
        {
            glm::mat4 modelMatrix;
            glm::mat4 bones[MAX_BONES];
            glm::mat4 normalMatrix;
            int       hasSkin     = 0;
            int       padding[15] = {0};
        };

        struct GPUConstants
        {
            uint32 int1;
            uint32 int2;
        };

        class Example : public App
        {
        public:
            virtual void Initialize() override;
            virtual void Shutdown() override;
            virtual void OnTick() override;

        private:
            void   ConfigureInitializeLinaGX();
            void   CreateMainWindow();
            void   SetupPerFrameResources();
            void   LoadTexture(const char* path, uint32 id);
            void   SetupTextures();
            void   SetupSamplers();
            void   SetupRenderTargets();
            void   SetupMaterials();
            void   LoadAndParseModels();
            void   SetupDescriptorSets();
            uint16 CreateShader(const char* vertex, const char* fragment, LinaGX::CullMode cullMode, LinaGX::Format passFormat, bool useCustomLayout, uint16 customLayout, LinaGX::CompareOp depthCompare, LinaGX::FrontFace front, bool blend, bool depthWrite);

            void DrawObjects();
            void DrawSkybox();
            void BindShader(uint32 target);

        private:
            LinaGX::Instance*      m_lgx       = nullptr;
            LinaGX::Window*        m_window    = nullptr;
            uint8                  m_swapchain = 0;
            PerFrameData           m_pfd[FRAMES_IN_FLIGHT];
            std::vector<Texture2D> m_textures;

            uint32       m_indexBufferStaging = 0;
            uint32       m_indexBufferGPU     = 0;
            VertexBuffer m_vtxBuffer;

            std::vector<WorldObject> m_worldObjects;
            std::vector<uint32>      m_samplers;
            std::vector<Material>    m_materials;
            std::vector<uint16>      m_shaders;

            uint32 m_skyboxIndexCount = 0;
            uint16 m_pipelineLayout;
            uint16 m_pipelineLayout2;
            Camera m_camera;
            int32  m_boundShader = -1;
        };

    } // namespace Examples
} // namespace LinaGX

#endif