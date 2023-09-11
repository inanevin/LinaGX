/*
This file is a part of: LinaGX
https://github.com/inanevin/LinaGX

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

#include "App.hpp"
#include <iostream>
#include <cstdarg>
#include "FoxLounge.hpp"

namespace LinaGX::Examples
{

    void Example::ConfigureInitializeLinaGX()
    {
        LinaGX::Config.logLevel      = LogLevel::Verbose;
        LinaGX::Config.errorCallback = LogError;
        LinaGX::Config.infoCallback  = LogInfo;

        BackendAPI api = BackendAPI::Vulkan;

#ifdef LINAGX_PLATFORM_APPLE
        api = BackendAPI::Metal;
#endif

        LinaGX::InitInfo initInfo = InitInfo{
            .api                   = api,
            .gpu                   = PreferredGPUType::Integrated,
            .framesInFlight        = FRAMES_IN_FLIGHT,
            .backbufferCount       = BACKBUFFER_COUNT,
            .checkForFormatSupport = {Format::B8G8R8A8_UNORM, Format::D32_SFLOAT},
        };

        m_lgx = new LinaGX::Instance();
        m_lgx->Initialize(initInfo);
    }

    void Example::CreateMainWindow()
    {
        m_window = m_lgx->GetWindowManager().CreateApplicationWindow(MAIN_WINDOW_ID, "LinaGX SSBO", 0, 0, 800, 800, WindowStyle::WindowedApplication);
        m_window->SetCallbackClose([this]() { Quit(); });

        m_swapchain = m_lgx->CreateSwapchain({
            .format       = Format::B8G8R8A8_UNORM,
            .x            = 0,
            .y            = 0,
            .width        = m_window->GetSize().x,
            .height       = m_window->GetSize().y,
            .window       = m_window->GetWindowHandle(),
            .osHandle     = m_window->GetOSHandle(),
            .isFullscreen = false,
            .vsyncMode    = VsyncMode::None,
        });
    }

    void Example::CreatePerFrameResources()
    {
        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd                  = m_pfd[i];
            pfd.graphicsStream         = m_lgx->CreateCommandStream(100, CommandType::Graphics);
            pfd.transferStream         = m_lgx->CreateCommandStream(100, CommandType::Transfer, 10000);
            pfd.transferSemaphoreValue = 0;
            pfd.transferSemaphore      = m_lgx->CreateUserSemaphore();
        }
    }

    void Example::LoadModel(const char* path, LinaGX::ModelData& outModelData, LINAGX_VEC<LinaGX::ModelTexture>& outTextures)
    {
        LOGT("Loading model...");
        LinaGX::LoadGLTFBinary(path, outModelData);

        outTextures.reserve(outTextures.size() + static_cast<size_t>(outModelData.allTexturesCount));

        for (uint32 i = 0; i < outModelData.allTexturesCount; i++)
        {
            const auto& texture = outModelData.allTextures[i];
            outTextures.push_back(texture);
        }
    }

    void Example::Initialize()
    {
        App::Initialize();

        ConfigureInitializeLinaGX();
        CreateMainWindow();
        CreatePerFrameResources();

        // Load environment.
        LINAGX_VEC<LinaGX::ModelTexture> textures;
        LinaGX::ModelData                environmentModelData = {};
        LoadModel("Resources/Models/FoxLounge/FoxLounge.glb", environmentModelData, textures);

        // Load fox as well.
        LinaGX::ModelData foxData = {};
        LoadModel("Resources/Models/Fox/Fox.glb", foxData, textures);

        auto& pfd = m_pfd[0];

        for (const auto& tb : textures)
        {
            m_textures.push_back({});
            auto& t2d = m_textures[m_textures.size() - 1];

            t2d.name = tb.name;
            t2d.allLevels.push_back(tb.buffer);
            // LinaGX::GenerateMipmaps(tb.buffer, t2d.allLevels, MipmapFilter::Default, static_cast<ImageChannelMask>(tb.buffer.bytesPerPixel - 1), false);

            Texture2DDesc desc = {
                .usage     = Texture2DUsage::ColorTexture,
                .width     = tb.buffer.width,
                .height    = tb.buffer.height,
                .mipLevels = static_cast<uint32>(t2d.allLevels.size()),
                .format    = tb.buffer.bytesPerPixel == 8 ? LinaGX::Format::R16G16B16A16_UNORM : LinaGX::Format::R8G8B8A8_UNORM,
                .debugName = t2d.name.empty() ? "LinaGX Texture" : t2d.name.c_str(),
            };

            t2d.gpuHandle = m_lgx->CreateTexture2D(desc);

            CMDCopyBufferToTexture2D* copyTxt = pfd.transferStream->AddCommand<CMDCopyBufferToTexture2D>();
            copyTxt->destTexture              = t2d.gpuHandle;
            copyTxt->mipLevels                = static_cast<uint32>(t2d.allLevels.size()),
            copyTxt->buffers                  = pfd.transferStream->EmplaceAuxMemory<TextureBuffer>(t2d.allLevels.data(), sizeof(TextureBuffer) * t2d.allLevels.size());
            copyTxt->destinationSlice         = 0;
        }

        m_lgx->CloseCommandStreams(&pfd.transferStream, 1);
        pfd.transferSemaphoreValue++;
        m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &pfd.transferStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &pfd.transferSemaphore, .signalValues = &pfd.transferSemaphoreValue});
        m_lgx->WaitForUserSemaphore(pfd.transferSemaphore, pfd.transferSemaphoreValue);
    }

    void Example::Shutdown()
    {
        // First get rid of the window.
        m_lgx->GetWindowManager().DestroyApplicationWindow(MAIN_WINDOW_ID);

        m_lgx->DestroySwapchain(m_swapchain);

        for (const auto& t2d : m_textures)
        {
            m_lgx->DestroyTexture2D(t2d.gpuHandle);
        }

        for (uint32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            auto& pfd = m_pfd[i];
            m_lgx->DestroyCommandStream(pfd.graphicsStream);
            m_lgx->DestroyCommandStream(pfd.transferStream);
            m_lgx->DestroyUserSemaphore(pfd.transferSemaphore);
        }

        // Terminate renderer & shutdown app.
        delete m_lgx;
        App::Shutdown();
    }

    void Example::OnTick()
    {
        // Check for window inputs.
        m_lgx->PollWindowsAndInput();

        // Let LinaGX know we are starting a new frame.
        m_lgx->StartFrame();

        auto& currentFrame = m_pfd[m_lgx->GetCurrentFrameIndex()];

        // Render pass begin
        {
            Viewport                  viewport        = {.x = 0, .y = 0, .width = m_window->GetSize().x, .height = m_window->GetSize().y, .minDepth = 0.0f, .maxDepth = 1.0f};
            ScissorsRect              sc              = {.x = 0, .y = 0, .width = m_window->GetSize().x, .height = m_window->GetSize().y};
            CMDBeginRenderPass*       beginRenderPass = currentFrame.graphicsStream->AddCommand<CMDBeginRenderPass>();
            RenderPassColorAttachment colorAttachment;
            colorAttachment.clearColor                       = {0.6f, 0.6f, 0.6f, 1.0f};
            colorAttachment.texture                          = static_cast<uint32>(m_swapchain);
            colorAttachment.isSwapchain                      = true;
            colorAttachment.loadOp                           = LoadOp::Clear;
            colorAttachment.storeOp                          = StoreOp::Store;
            beginRenderPass->colorAttachmentCount            = 1;
            beginRenderPass->colorAttachments                = currentFrame.graphicsStream->EmplaceAuxMemory<RenderPassColorAttachment>(colorAttachment);
            beginRenderPass->viewport                        = viewport;
            beginRenderPass->scissors                        = sc;
            beginRenderPass->depthStencilAttachment.useDepth = beginRenderPass->depthStencilAttachment.useStencil = false;
        }

        // End render pass
        {
            CMDEndRenderPass* end = currentFrame.graphicsStream->AddCommand<CMDEndRenderPass>();
        }

        // This does the actual *recording* of every single command stream alive.
        m_lgx->CloseCommandStreams(&currentFrame.graphicsStream, 1);

        // Submit work on gpu.
        m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Graphics), .streams = &currentFrame.graphicsStream, .streamCount = 1});

        // Present main swapchain.
        m_lgx->Present({.swapchains = &m_swapchain, .swapchainCount = 1});

        // Let LinaGX know we are finalizing this frame.
        m_lgx->EndFrame();
    }

} // namespace LinaGX::Examples
