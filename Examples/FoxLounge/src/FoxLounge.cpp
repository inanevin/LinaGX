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
#include "FoxLounge.hpp"
#include "ThreadPool.hpp"
#include <iostream>
#include <cstdarg>
#include <string>
#include <vector>
#include <filesystem>

namespace LinaGX::Examples
{

    void Example::ConfigureInitializeLinaGX()
    {
        LinaGX::Config.logLevel      = LogLevel::Verbose;
        LinaGX::Config.errorCallback = LogError;
        LinaGX::Config.infoCallback  = LogInfo;

        BackendAPI api = BackendAPI::DX12;

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

    void Example::LoadModel(const char* path, LinaGX::ModelData& outModelData)
    {
        LOGT("Loading model...");
        LinaGX::LoadGLTFBinary(path, outModelData);
    }

    void Example::LoadTexture(const char* path, uint32 id)
    {
        LinaGX::TextureBuffer outData;
        LinaGX::LoadImageFromFile(path, outData, ImageChannelMask::RGBA);
        auto& txt = m_textures.at(id);
        txt.allLevels.push_back(outData);
        txt.path            = path;
        const uint32 levels = LinaGX::CalculateMipLevels(outData.width, outData.height);
        LinaGX::GenerateMipmaps(outData, txt.allLevels, MipmapFilter::Default, ImageChannelMask::RGBA, false);
    }

    void Example::StartTextureLoading()
    {
        std::vector<std::string> texturePaths;

        // Fetch all texture paths from io.
        {
            const std::string basePathLounge = "Resources/Textures/FoxLounge/";
            const std::string basePathFox    = "Resources/Textures/Fox/";
            try
            {
                for (const auto& entry : std::filesystem::directory_iterator(basePathLounge))
                {
                    if (entry.is_regular_file())
                        texturePaths.push_back(entry.path().string());
                }

                for (const auto& entry : std::filesystem::directory_iterator(basePathFox))
                {
                    if (entry.is_regular_file())
                        texturePaths.push_back(entry.path().string());
                }
            }
            catch (std::filesystem::filesystem_error& e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        texturePaths.push_back("Resources/Textures/FoxLounge/concrete_cat_statue_nor_gl.png");

        // Load textures in parallel.
        {
            m_textures.resize(texturePaths.size());
            ThreadPool pool;
            uint32     id = 0;
            for (const auto& path : texturePaths)
            {
                pool.AddTask([path, this, id]() { LoadTexture(path.c_str(), id); });
                id++;
            }
            pool.Run();
            pool.Join();
        }

        // Create gpu handles & record transfer commands.
        {
            auto& pfd = m_pfd[0];
            for (auto& txt : m_textures)
            {
                Texture2DDesc desc = {
                    .usage     = Texture2DUsage::ColorTexture,
                    .width     = txt.allLevels[0].width,
                    .height    = txt.allLevels[0].height,
                    .mipLevels = static_cast<uint32>(txt.allLevels.size()),
                    .format    = txt.allLevels[0].bytesPerPixel == 8 ? LinaGX::Format::R16G16B16A16_UNORM : LinaGX::Format::R8G8B8A8_UNORM,
                    .debugName = txt.path.c_str(),
                };

                txt.gpuHandle = m_lgx->CreateTexture2D(desc);

                CMDCopyBufferToTexture2D* copyTxt = pfd.transferStream->AddCommand<CMDCopyBufferToTexture2D>();
                copyTxt->destTexture              = txt.gpuHandle;
                copyTxt->mipLevels                = static_cast<uint32>(txt.allLevels.size()),
                copyTxt->buffers                  = pfd.transferStream->EmplaceAuxMemory<TextureBuffer>(txt.allLevels.data(), sizeof(TextureBuffer) * txt.allLevels.size());
                copyTxt->destinationSlice         = 0;
            }

            // Send the transfers & wait on the cpu for upload.
            m_lgx->CloseCommandStreams(&pfd.transferStream, 1);
            pfd.transferSemaphoreValue++;
            m_lgx->SubmitCommandStreams({.targetQueue = m_lgx->GetPrimaryQueue(CommandType::Transfer), .streams = &pfd.transferStream, .streamCount = 1, .useSignal = true, .signalCount = 1, .signalSemaphores = &pfd.transferSemaphore, .signalValues = &pfd.transferSemaphoreValue});
            m_lgx->WaitForUserSemaphore(pfd.transferSemaphore, pfd.transferSemaphoreValue);
        }

        // Dont need them buffers but leave allLevels vector as is as we might need some info later.
        {
            for (auto& txt : m_textures)
            {
                for (auto& level : txt.allLevels)
                    LinaGX::FreeImage(level.pixels);
            }
        }
    }

    void Example::Initialize()
    {
        App::Initialize();
        ConfigureInitializeLinaGX();
        CreateMainWindow();
        CreatePerFrameResources();
        StartTextureLoading();
    }

    void Example::Shutdown()
    {
        m_lgx->Join();

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
