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

namespace LinaGX
{
    namespace Examples
    {

#define MAIN_WINDOW_ID   0
#define FRAMES_IN_FLIGHT 2
#define BACKBUFFER_COUNT 2

        struct PerFrameData
        {
            LinaGX::CommandStream* graphicsStream         = nullptr;
            LinaGX::CommandStream* transferStream         = nullptr;
            uint64                 transferSemaphoreValue = 0;
            uint16                 transferSemaphore      = 0;
        };

        struct Texture2D
        {
            LINAGX_STRING                     path      = "";
            uint32                            gpuHandle = 0;
            LINAGX_VEC<LinaGX::TextureBuffer> allLevels;
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
            void CreatePerFrameResources();
            void LoadModel(const char* path, LinaGX::ModelData& outModelData);
            void LoadTexture(const char* path, uint32 id);
            void StartTextureLoading();

        private:
            LinaGX::Instance*     m_lgx       = nullptr;
            LinaGX::Window*       m_window    = nullptr;
            uint8                 m_swapchain = 0;
            PerFrameData          m_pfd[FRAMES_IN_FLIGHT];
            LINAGX_VEC<Texture2D> m_textures;
        };

    } // namespace Examples
} // namespace LinaGX

#endif