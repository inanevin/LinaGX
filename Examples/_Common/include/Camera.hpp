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

#ifndef LINAGX_EXAMPLE_CAMERA_HPP
#define LINAGX_EXAMPLE_CAMERA_HPP

#include "LinaGX/LinaGX.hpp"
#include "glm/glm.hpp"
#include "glm/detail/type_quat.hpp"

namespace LinaGX
{
    namespace Examples
    {
#define FAR_PLANE  1000.0f
#define NEAR_PLANE 0.1f
#define FOV        90.0f

        class Camera : public LinaGX::WindowListener
        {
        public:
            Camera()  = default;
            ~Camera() = default;

            virtual void OnWindowMouse(Window* window, uint32 button, InputAction inputAction) override;
            void         Initialize(LinaGX::Instance* lgx);
            void         Tick(float dt);

            inline const glm::mat4& GetView() const
            {
                return m_view;
            }

            inline const glm::mat4& GetProj() const
            {
                return m_proj;
            }

            inline glm::vec4 GetPosition() const
            {
                return glm::vec4(m_position.x, m_position.y, m_position.z, 0.0f);
            }

        private:
            void CalculateViewProj();

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
