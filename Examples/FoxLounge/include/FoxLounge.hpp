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
#include "Utility.hpp"

namespace LinaGX
{
    namespace Examples
    {
        struct PerFrameData
        {
            LinaGX::CommandStream* graphicsStream         = nullptr;
            LinaGX::CommandStream* transferStream         = nullptr;
            uint64                 transferSemaphoreValue = 0;
            uint16                 transferSemaphore      = 0;
            uint16                 globalSet              = 0;
            uint16                 cameraSet0             = 0;
            uint16                 cameraSetCubemap       = 0;

            uint32 rscCameraData0        = 0;
            uint8* rscCameraDataMapping0 = nullptr;

            uint32 rscCameraDataCubemap        = 0;
            uint8* rscCameraDataMappingCubemap = nullptr;

            uint32 rscSceneData         = 0;
            uint8* rscSceneDataMapping  = nullptr;
            uint32 rscObjDataCPU        = 0;
            uint32 rscObjDataGPU        = 0;
            uint8* rscObjDataCPUMapping = nullptr;

            uint16 lightingPassMaterialSet  = 0;
            uint16 finalQuadPassMaterialSet = 0;
        };

        struct Texture2D
        {
            std::string                        path      = "";
            uint32                             gpuHandle = 0;
            std::vector<LinaGX::TextureBuffer> allLevels;
        };

        struct Material
        {
            std::string                                         name = "";
            GPUMaterialData                                     gpuMat;
            std::unordered_map<LinaGX::GLTFTextureType, uint32> textureIndices;
            uint16                                              descriptorSets[FRAMES_IN_FLIGHT];
            uint32                                              stagingResources[FRAMES_IN_FLIGHT];
            uint32                                              gpuResources[FRAMES_IN_FLIGHT];
        };

        struct RenderTarget
        {
            std::vector<RenderPassColorAttachment> colorAttachments;
            RenderPassDepthStencilAttachment       depthStencilAttachment;
        };

        struct Pass
        {
            RenderTarget renderTargets[FRAMES_IN_FLIGHT];
        };

        struct MeshPrimitive
        {
            GPUBuffer vtxBuffer;
            GPUBuffer indexBuffer;
            uint32    materialIndex = 0;
            uint32    indexCount    = 0;
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

        class Example : public App
        {
        public:
            virtual void Initialize() override;
            virtual void Shutdown() override;
            virtual void OnTick() override;

        private:
            void ConfigureInitializeLinaGX();
            void CreateMainWindow();
            void SetupStreams();
            void LoadTexture(const char* path, uint32 id);
            void SetupTextures();
            void SetupSamplers();
            void SetupMaterials();
            void LoadAndParseModels();
            void SetupShaders();
            void SetupPasses();
            void SetupGlobalResources();
            void SetupPipelineLayouts();
            void SetupGlobalDescriptorSet();
            void CreateDynamicTextures();
            void DestroyDynamicTextures();

            void BeginPass(PassType pass, uint32 width = 0, uint32 height = 0, uint32 layer = 0);
            void EndPass();
            void CaptureCubemap();
            void DrawObjects(Shader shader);
            void DrawSkybox();
            void BindMaterialSet(uint16 materialSet);
            void DrawFullscreenQuad();
            void BindShader(uint32 target);
            void BindCameraDescriptor(uint16 set, uint32 offset);
            void ReflectionPass();
            void CollectPassBarrier(PassType pass, LinaGX::TextureBarrierState target);
            void ExecPassBarriers();
            void BuildCommands(uint32 frameIndex);

        private:
            LinaGX::Instance*      m_lgx       = nullptr;
            LinaGX::Window*        m_window    = nullptr;
            uint8                  m_swapchain = 0;
            PerFrameData           m_pfd[FRAMES_IN_FLIGHT];
            std::vector<Texture2D> m_textures;

            // GPUBuffer m_indxBuffer;
            // GPUBuffer m_vtxBuffer;

            std::vector<uint32>      m_samplers;
            std::vector<uint16>      m_shaders;
            std::vector<WorldObject> m_worldObjects;
            std::vector<Material>    m_materials;
            std::vector<Pass>        m_passes;

            uint32 m_skyboxIndexCount = 0;
            Camera m_camera;
            int32  m_boundShader = -1;

            std::vector<uint16>                 m_pipelineLayouts;
            std::vector<LinaGX::TextureBarrier> m_passBarriers;
        };

    } // namespace Examples
} // namespace LinaGX

#endif