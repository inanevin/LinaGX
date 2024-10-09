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

#include "Camera.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <chrono>

namespace LinaGX::Examples
{

#define MOVE_SPEED   16.0f
#define ROTATE_SPEED 32.0f
#define ROTATE_AMT   3.0f

    std::chrono::steady_clock::time_point now;

    void Camera::Initialize(LinaGX::Instance* lgx)
    {
        m_lgx        = lgx;
        m_mainWindow = m_lgx->GetWindowManager().GetWindow(0);
        m_initialRot = m_rotation;
        now          = std::chrono::high_resolution_clock::now();
        m_mainWindow->AddListener(this);
    }

    void Camera::OnWindowMouse(Window *window, uint32 button, InputAction inputAction)
    {
        if(button != LINAGX_MOUSE_0)
            return;
        
        if(inputAction != InputAction::Pressed)
            return;
        
        //m_mainWindow->SetMouseVisible(false);
        //m_mainWindow->ConfineMouseToCenter();
        m_controlsEnabled = true;
    }

    void Camera::Tick(float dt)
    {
        if (m_controlsEnabled && m_lgx->GetInput().GetKey(LINAGX_KEY_ESCAPE))
        {
            m_controlsEnabled = false;
            //m_mainWindow->SetMouseVisible(true);
            //m_mainWindow->FreeMouse();
        }

        // m_controlsEnabled = true;
        if (m_controlsEnabled)
        {
            // Calc rot.
            auto md = m_lgx->GetInput().GetMouseDelta();

#ifdef LINAGX_PLATFORM_WINDOWS
            const int clampAmt = 10;
#else
            const int clampAmt = 3;
#endif

            md.x = std::clamp((int)md.x, -clampAmt, clampAmt);
            md.y = std::clamp((int)md.y, -clampAmt, clampAmt);

            m_targetX -= md.y * dt * ROTATE_AMT;
            m_targetY -= md.x * dt * ROTATE_AMT;
            m_angleX           = LinaGX::Lerp(m_angleX, m_targetX, dt * ROTATE_SPEED);
            m_angleY           = LinaGX::Lerp(m_angleY, m_targetY, dt * ROTATE_SPEED);
            const glm::quat qx = glm::angleAxis(m_angleX, glm::vec3(1, 0, 0));
            const glm::quat qy = glm::angleAxis(m_angleY, glm::vec3(0, 1, 0));
            m_rotation         = qy * qx;

            // Calc pos.
            float moveX = 0.0f, moveY = 0.0f;
            if (m_lgx->GetInput().GetKey(LINAGX_KEY_W))
                moveY = 1.0f;
            else if (m_lgx->GetInput().GetKey(LINAGX_KEY_S))
                moveY = -1.0f;

            if (m_lgx->GetInput().GetKey(LINAGX_KEY_D))
                moveX = 1.0f;
            else if (m_lgx->GetInput().GetKey(LINAGX_KEY_A))
                moveX = -1.0f;

            const glm::vec3 forwardVec = m_rotation * glm::vec3(0.0f, 0.0f, -1.0f);
            const glm::vec3 rightVec   = m_rotation * glm::vec3(1.0f, 0.0f, 0.0f);
            m_position += moveY * dt * glm::normalize(forwardVec) * MOVE_SPEED;
            m_position += moveX * dt * glm::normalize(rightVec) * MOVE_SPEED;
        }

        // Produce matrices.
        CalculateViewProj();
    }

    void Camera::CalculateViewProj()
    {
        glm::mat4 rotationMatrix    = glm::mat4_cast(glm::inverse(m_rotation));
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_position);
        m_view                      = rotationMatrix * translationMatrix;
        m_proj                      = glm::perspective(DEG2RAD(FOV), static_cast<float>(m_mainWindow->GetSize().x) / static_cast<float>(m_mainWindow->GetSize().y), NEAR_PLANE, FAR_PLANE);
    }

} // namespace LinaGX::Examples
