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

#pragma once

#include "LinaGX/Common/Types.hpp"
#include "LinaGX/Common/CommonData.hpp"
#include "LinaGX/Common/Defines/Format.hpp"
#include <assert.h>

namespace LinaGX
{

    /// <summary>
    /// Variety of window styles to use, check the enums for description.
    /// </summary>
    enum class WindowStyle
    {
        WindowedApplication,   // Most OS-default application, windowed with title bar and system buttons
        BorderlessApplication, // Borderless-application, which still supports for example OS animations, taskbar interaction etc.
        Borderless,            // True borderless, a lifeless and soulless application window only displaying what you draw.
        BorderlessAlpha,       // Borderless but supports alpha blending,
        BorderlessFullscreen,  // Borderless full-screen by default, you can change the size later on
        Fullscreen,            // Full-screen exclusive by default, you can change the size later on.
    };

    enum class BackendAPI
    {
        Vulkan,
        DX12,
        Metal
    };

    enum class PreferredGPUType
    {
        Integrated,
        Discrete,
    };

    enum class LogLevel
    {
        None,
        OnlyErrors,
        Normal,
        Verbose,
    };

    struct VulkanConfiguration
    {
        bool   flipViewport            = true;
        uint32 extraGraphicsQueueCount = 0;
        uint32 enableVulkanFeatures    = 0;
    };

    struct DX12Configuration
    {
        bool allowTearing = true;
    };

    struct MetalConfiguration
    {
    };

    struct FormatSupportInfo
    {
        Format format                 = Format::UNDEFINED;
        bool   sampled                = false;
        bool   colorAttachment        = false;
        bool   depthStencilAttachment = false;
    };

    struct GPUInformation
    {
        LINAGX_STRING                 deviceName = "";
        LINAGX_VEC<FormatSupportInfo> supportedTexture2DFormats;

        uint64 totalCPUVisibleGPUMemorySize     = 0;
        uint64 dedicatedVideoMemory             = 0;
        uint64 minConstantBufferOffsetAlignment = 0;
        uint64 minStorageBufferOffsetAlignment  = 0;
    };

    struct GPULimits
    {
        uint32 textureLimit       = 512;
        uint32 samplerLimit       = 512;
        uint32 bufferLimit        = 512;
        uint32 maxSubmitsPerFrame = 30;
        uint32 maxDescriptorSets  = 512;
    };

    struct PerformanceStatistics
    {
        uint64 totalFrames = 0;
    };

    typedef void (*LogCallback)(const char*, ...);

    struct Configuration
    {
        BackendAPI          api                             = BackendAPI::Vulkan;
        PreferredGPUType    gpu                             = PreferredGPUType::Discrete;
        const char*         appName                         = "LinaGX App";
        uint32              framesInFlight                  = 2;
        uint32              backbufferCount                 = 2;
        GPULimits           gpuLimits                       = {};
        VulkanConfiguration vulkanConfig                    = {};
        DX12Configuration   dx12Config                      = {};
        MetalConfiguration  mtlConfig                       = {};
        LogCallback         errorCallback                   = nullptr;
        LogCallback         infoCallback                    = nullptr;
        LogLevel            logLevel                        = LogLevel::Normal;
        bool                mutexLockCreationDeletion       = false;
        bool                multithreadedQueueSubmission    = false;
        bool                enableAPIDebugLayers            = true;
        bool                enableShaderDebugInformation    = false;
        bool                serializeShaderDebugInformation = false;
    };

    extern Configuration         Config;
    extern GPUInformation        GPUInfo;
    extern PerformanceStatistics PerformanceStats;

#define LOGT(...)                                                                                            \
    if (Config.infoCallback && Config.logLevel != LogLevel::None && Config.logLevel != LogLevel::OnlyErrors) \
        Config.infoCallback(__VA_ARGS__);
#define LOGE(...)                                                       \
    if (Config.errorCallback && Config.logLevel != LogLevel::None)      \
    {                                                                   \
        Config.errorCallback(__VA_ARGS__);                              \
        Config.errorCallback("File: %s, Line: %d", __FILE__, __LINE__); \
    }

#define LOGV(...)                                                    \
    if (Config.infoCallback && Config.logLevel == LogLevel::Verbose) \
        Config.infoCallback(__VA_ARGS__);

#define LOGA(condition, ...)                  \
    if (!(condition) && Config.errorCallback) \
        Config.errorCallback(__VA_ARGS__);    \
    assert(condition);

} // namespace LinaGX
