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

#ifndef LINAGX_EXAMPLE_CAMERA_HPP
#define LINAGX_EXAMPLE_CAMERA_HPP

#include "LinaGX/LinaGX.hpp"
#include "glm/glm.hpp"
#include "glm/detail/type_quat.hpp"

namespace LinaGX
{
    namespace Examples
    {
        class Camera
        {
        public:
            Camera()  = default;
            ~Camera() = default;

            void Initialize(LinaGX::Instance* lgx);
            void Tick(float dt);

            inline const glm::mat4& GetView() const
            {
                return m_view;
            }

            inline const glm::mat4& GetProj() const
            {
                return m_proj;
            }

            inline const glm::vec3& GetPosition() const
            {
                return m_position;
            }

        private:
            LinaGX::Instance* m_lgx             = nullptr;
            LinaGX::Window*   m_mainWindow      = nullptr;
            glm::vec3         m_position        = {};
            glm::quat         m_rotation        = glm::quat(1, 0, 0, 0);
            glm::mat4         m_view            = {};
            glm::mat4         m_proj            = {};
            glm::quat         m_initialRot      = {};
            float             m_angleX          = 0.0f;
            float             m_angleY          = 0.0f;
            float             m_targetX         = 0.0f;
            float             m_targetY         = 0.0f;
            bool              m_controlsEnabled = false;
        };
    } // namespace Examples
} // namespace LinaGX

#endif