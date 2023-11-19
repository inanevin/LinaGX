
<br/>
<p align="center">
  <img src="https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084496-3065e44f-9399-48a5-b14e-cdb720370a00.png">
</p>
<br/>
<div align="center">

[![License](https://img.shields.io/badge/license-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/inanevin) 
[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/inanevin/LinaVG/issues) 
![GitHub issues](https://img.shields.io/github/issues/inanevin/LinaEngine.svg)
  
</div>

LinaGX is a cross-platform rendering library that seamlessly integrates Vulkan, DirectX12 and Metal. It accelerates development by offering a unified API that simplifies cross-platform shader compilation, queue management, work submission and synchronization, meanwhile offering extra capabilities such as window management, input, as well as image and GLTF loading.


<p align="center">
  <img src="https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084408-9056eee4-9f3a-460a-bdea-46759bdd6c05.png">
  <br>
  <small><em>Screenshot from one of the examples, FoxLounge.</em></small>
</p>

LinaGX is primarily aimed for desktop development on Windows and MacOS, however Vulkan and Metal backend makes it possible to use it for mobile platforms, eventhough mobile support is not officially tested and maintained.

LinaGX is originally developed for and used in [Lina Engine](https://github.com/inanevin/LinaEngine), however it's completely decoupled from it and can be used as a standalone library.

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

![Triangle](https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084531-b4df4e6a-6d77-4cf6-b4cf-5b42d79004c3.png)

## [02-TexturesAndBinding](https://github.com/inanevin/LinaGX/tree/master/Examples/02-TexturesAndBinding)

Building on top of the previous examples, introduces transfer streams for copying data to GPU using copy queues, vertex & index buffers, descriptor sets, texture loading utilities and texture transfers.

![TexturesAndBinding](https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084535-49e2c0e3-013f-4381-af45-1920ea237028.png)

## [03-RenderTargetsGLTF](https://github.com/inanevin/LinaGX/tree/master/Examples/03-RenderTargetsGLTF)

Building on top of the previous examples, introduces GLTF loading, skinning, and multiple render targets.

![RenderTargetsGLTF](https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084118-7b640d2d-c505-4528-a37e-af1e5362e34e.gif?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=AKIAIWNJYAX4CSVEH53A%2F20231119%2Fus-east-1%2Fs3%2Faws4_request&X-Amz-Date=20231119T190934Z&X-Amz-Expires=300&X-Amz-Signature=26b0c1e5a715bbf3d173c400693418bb4fde44f14db874ebdf3040d5ff7f1fe8&X-Amz-SignedHeaders=host&actor_id=3519379&key_id=0&repo_id=655905592)

## [04-BindlessIndirectComputeQueue](https://github.com/inanevin/LinaGX/tree/master/Examples/04-BindlessIndirectComputeQueue)

Simple scene demonstrating usage of compute shaders, explicit compute queue and syncronization, as well as bindless textures and indirect rendering.

![BindlessIndirect](https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084540-f32f8789-7a04-4b70-97d8-e868653678c9.png)

## [05-FoxLounge](https://github.com/inanevin/LinaGX/tree/master/Examples/05-FoxLounge)

A complete, comprehensive example, showing the flexibility and power of LinaGX, by providing a deferred PBR renderer, with point lights, shadow mapping, realtime IBL creation with irradiance, BRDF and prefilter steps, SSAO, bloom and other post-processing.

![FoxLounge2](https://github-production-user-asset-6210df.s3.amazonaws.com/3519379/284084578-02aab616-a10f-4d92-a17b-030a38609068.png)

# Installation

Download a release from [Releases](https://github.com/inanevin/LinaGX/releases).

Recommended way of using LinaGX is to link it using CMake within your application. If you'd like to pre-complie yourself, you can still do so via CMake, but you also have to pre-compile the dependency libraries, such as SPIRV-Cross and glslang.

Example projects are disabled by default. Use ```LINAGX_BUILD_EXAMPLES``` option to enable them during CMake generation.

```shell
cmake DLINAGX_BUILD_EXAMPLES=ON
```

By default, LinaGX builds for both Vulkan and DX12 on Windows for runtime switching between graphics APIs. If this is not required for your use case, you can use ```LINAGX_DISABLE_VK``` or ```LINAGX_DISABLE_DX12``` to disable one of the graphics backends. Using both options will fail CMake project generation.

# [License (BSD 2-clause)](http://opensource.org/licenses/BSD-2-Clause)

<a href="http://opensource.org/licenses/BSD-2-Clause" target="_blank">
<img align="right" src="https://opensource.org/wp-content/uploads/2022/10/osi-badge-dark.svg" width="100" height="130">
</a>

	Copyright 2023 Inan Evin
	
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

# Contributing

Any contributions and PR are welcome.

# Support

[![Lina Discord](https://badgen.net/discord/members/QYeTkEtRMB)](https://discord.gg/QYeTkEtRMB)

You can join [Lina Engine's Discord channel](https://discord.gg/QYeTkEtRMB) to talk about the Lina Project.


