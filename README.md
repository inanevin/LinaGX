<br/>
<p align="center">
  <img src="https://github.com/inanevin/LinaGX/assets/3519379/7aa81d2b-9a9b-4ef8-8a29-375a555adb31">
</p>
<br/>
<div align="center">
  
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/inanevin) 
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/59f8c18dd1a24b478fe6459344252e3c)](https://www.codacy.com/gh/inanevin/LinaVG/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=inanevin/LinaVG&amp;utm_campaign=Badge_Grade)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaVG/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)
  
</div>

LinaGX is a cross-platform graphics abstraction library that seamlessly integrates Vulkan, DirectX12 and Metal. It's primary purpose is to streamline graphics development process by offering a unified API that mirrors the best functionalities of modern graphics APIs. LinaGX is more than a simple abstraction layer, as it also simplifies challenges such as cross-platform shader compilation, queue management, work submission and syncronization, meanwhile providing framework functionalities for window management, input processing, as well as image and GLTF loading.

<p align="center">
  <img src="https://github.com/inanevin/LinaGX/assets/3519379/26e7ebe4-2925-43a3-8039-ebf303fb8c5a" alt="Hello">
  <br>
  <small><em>Screenshot from one of the examples, FoxLounge.</em></small>
</p>

LinaGX streamlines the creation of realtime rendering applications, providing an all-in-one toolkit that eliminates the need for additional libraries. Experienced graphics programmers will find it extremely easy to develop, offering simplicity alongside the option for detailed control and verbosity. For beginners, LinaGX is a practical gateway into graphics programming, though a basic understanding of modern graphics APIs is beneficial to quickly grasp its concepts.

LinaGX is primarily aimed for desktop platforms: Windows and MacOS. It can also be used for mobile platforms, however mobile platform support is not tested and maintained. LinaGX is originally developed for and used in [Lina Engine](https://github.com/inanevin/LinaEngine), however it's completely decoupled from it and can be used as a standalone library.

This repository contains LinaGX source code and a couple of example projects demonstrating the flexibilty of the API to develop graphics code ranging from a simple triangle all the way to a deferred PBR renderer.

# Feature Highlights

- **Common API:** Unified API that brings key features from Vulkan and DX12 together, aimed at choosing the most flexible ways to program graphics code. For example, resource binding is handled with Descriptor Sets, meanwhile queue syncronization API is similar to how DX12 works.
- **Shader Cross-compilation:** Write shaders in GLSL, LinaGX compiles them into SPIRV for Vulkan, IDxc Blob for DX12, and MSL for Metal, with support for serialization and reuse.
- **Shader Reflection and Pipeline Automation:** Offers an option for manual pipeline layout and vertex input definition, or automated creation using shader reflection info from SPIRV-Cross.
- **Render Loops:** Manages image acquisition, synchronization with image and presentation semaphores, work submission, and buffering for multiple frames-in-flight internally.
- **Command Streams:** Use Command Stream API to simply record commands into a stream object & dump them into work submission anytime you want. 
- **Queue Management and User Semaphores:** Enables creation of multiple device queues with internal management of physical queues, complemented by custom user semaphores for synchronizing CPU and GPU work.
- **Swapchains:** Simplifies swapchain creation and management, including resize operations and presentation handling.
- **Resources:** Facilitates easy creation of CPU/GPU resources and transfer operations over copy queues, handling any necessary staging buffers, particularly for texture copying.
- **Utilities:** Provides utilities for loading textures, generating mipmaps and loading/parsing GLTF models. 
- **Windowing and Input:** Supports creation of multiple application windows in native or borderless styles, along with comprehensive input callbacks and state polling/querying.
  
# Platform Support & Min Specs

**Source code is tested with compilers:**
- MSVC 14.36.xxx
- Clang 17.0.1

**Vulkan:**
- Supported on Windows and Android.
- Target devices minimum Vulkan version: 1.3
- Minimum GLSL version for development: 450
- If enabling custom features, the minimum requirements of such features apply.

**DX12:**
- Supported on Windows.
- Target devices minimum feature level: D3D_FEATURE_LEVEL_11_0
- Minimum shader model SM6_0

**Metal:**
- Supported on MacOS and iOS devices.

# API Basic Examples

More examples and documentation can be found in [wiki](https://github.com/inanevin/LinaGX/wiki).

### Initializing LinaGX

```cpp
LinaGX::Config.api             = BackendAPI::Vulkan;
LinaGX::Config.gpu             = PreferredGPUType::Discrete;
LinaGX::Config.framesInFlight  = FRAMES_IN_FLIGHT;
LinaGX::Config.backbufferCount = BACK_BUFFER;
LinaGX::Config.gpuLimits       = {};
LinaGX::Config.logLevel        = LogLevel::Verbose;
LinaGX::Config.errorCallback   = LogError;
LinaGX::Config.infoCallback    = LogInfo;
_lgx                           = new LinaGX::Instance();
_lgx->Initialize();
```

### Resource and Texture Creation

```cpp
ResourceDesc bufferDesc = ResourceDesc{
    .size          = vertexBufferSize,
    .typeHintFlags = TH_VertexBuffer,
    .heapType      = ResourceHeap::GPUOnly,
    .debugName     = "VertexBufferGPU",
};
_vertexBufferGPU     = _lgx->CreateResource(bufferDesc);

TextureDesc desc = {
    .format      = Format::R8G8B8A8_UNORM,
    .flags       = LinaGX::TextureFlags::TF_CopyDest | LinaGX::TextureFlags::TF_Sampled,
    .width       = loadedTextureData.width,
    .height      = loadedTextureData.height,
    .mipLevels   = static_cast<uint32>(txtData1.size()),
    .arrayLength = 2,
    .debugName   = "Lina Logo",
};
_textureGPU = _lgx->CreateTexture(desc);
```

### Recording Commands

```cpp
CMDBindVertexBuffers* bind = currentFrame.stream->AddCommand<CMDBindVertexBuffers>();
bind->slot                 = 0;
bind->resource             = _vertexBufferGPU;
bind->vertexSize           = sizeof(Vertex);
bind->offset               = 0;
```
### Work Submission

```cpp
SubmitDesc submit = {
    .targetQueue      = _lgx->GetPrimaryQueue(CommandType::Transfer),
    .streams          = &currentFrame.copyStream,
    .streamCount      = 1,
    .useWait          = false,
    .useSignal        = true,
    .signalCount      = 1,
    .signalSemaphores = &currentFrame.copySemaphore,
    .signalValues     = &currentFrame.copySemaphoreValue,
};
_lgx->SubmitCommandStreams(submit);
```

# Example Projects

## [00-WindowCreation](https://github.com/inanevin/LinaGX/tree/master/Examples/00-WindowCreation)

Demonstrates how to create an simple app window using LinaGX windowing API. NOthing is rendered on the window, absolute starters.

## [01-Triangle](https://github.com/inanevin/LinaGX/tree/master/Examples/01-Triangle)

A simple UV triangle demonstrating shader cross-compilation, swapchains, render passes, frame submission and presentation.

![Triangle](https://github.com/inanevin/LinaGX/assets/3519379/b5adb7fa-9ff0-4697-a97e-1bdac9ea5071)

## [02-TexturesAndBinding](https://github.com/inanevin/LinaGX/tree/master/Examples/02-TexturesAndBinding)

Building on top of the previous examples, introduces transfer streams for copying data to GPU using copy queues, vertex & index buffers, descriptor sets, texture loading utilities and texture transfers.

![TexturesAndBinding](https://github.com/inanevin/LinaGX/assets/3519379/bee0fcd7-4200-466c-bc73-bd833d3cb4c5)

## [03-RenderTargetsGLTF](https://github.com/inanevin/LinaGX/tree/master/Examples/03-RenderTargetsGLTF)

Building on top of the previous examples, introduces GLTF loading, skinning, and multiple render targets.

![RenderTargetsGLTF](https://github.com/inanevin/LinaGX/assets/3519379/85af5b92-ca4d-46ae-b9d1-107aa6e67bd2)

## [04-BindlessIndirectComputeQueue](https://github.com/inanevin/LinaGX/tree/master/Examples/04-BindlessIndirectComputeQueue)

Simple scene demonstrating usage of compute shaders, explicit compute queue and syncronization, as well as bindless textures and indirect rendering.

![BindlessIndirect](https://github.com/inanevin/LinaGX/assets/3519379/c1743253-f326-47d6-a948-4b30d1391634)

## [05-FoxLounge](https://github.com/inanevin/LinaGX/tree/master/Examples/05-FoxLounge)

A complete, comprehensive example, showing the flexibility and power of LinaGX, by providing a deferred PBR renderer, with point lights, shadow mapping, realtime IBL creation with irradiance, BRDF and prefilter steps, SSAO, bloom and other post-processing.

![ezgif com-crop](https://github.com/inanevin/LinaGX/assets/3519379/7b640d2d-c505-4528-a37e-af1e5362e34e)

# Installation

Download a release from [Releases](https://github.com/inanevin/LinaGX/releases).

Recommended way of using LinaGX is to link it using CMake within your application. If you'd like to pre-complie yourself, you can still do so via CMake, but you also have to pre-compile the dependency libraries, such as SPIRV-Cross and glslang.

CMake project offers only a single option, and it is to enable building of examples which are disabled by default. Use ```LINAGX_BUILD_EXAMPLES``` option.

```shell
cmake DLINAGX_BUILD_EXAMPLES=ON
```

Or tick it on if you are using CMake GUI.

# License

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT)

[LICENSE.MD](https://github.com/inanevin/LinaVG/blob/master/LICENSE)

# Contributing

Any contributions and PR are welcome.

# Support

You can join [Lina Engine's Discord channel](https://discord.gg/QYeTkEtRMB) to talk about the Lina Project.
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)

