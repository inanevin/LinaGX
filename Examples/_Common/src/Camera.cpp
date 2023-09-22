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

#include "Camera.hpp"
#include "LinaGX/Core/InputMappings.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

namespace LinaGX::Examples
{

#define MOVE_SPEED   6.0f
#define ROTATE_SPEED 16.0f
#define ROTATE_AMT   6.0f

    void Camera::Initialize(LinaGX::Instance* lgx)
    {
        m_lgx        = lgx;
        m_mainWindow = m_lgx->GetWindowManager().GetWindow(0);
        m_initialRot = m_rotation;
    }

    void Camera::Tick(float dt)
    {
        if (m_controlsEnabled && m_lgx->GetInput().GetKeyDown(LINAGX_KEY_ESCAPE))
        {
            m_controlsEnabled = false;
            m_mainWindow->SetMouseVisible(true);
            m_mainWindow->FreeMouse();
        }

        if (!m_controlsEnabled)
        {
            if (m_lgx->GetInput().GetMouseButtonDown(LINAGX_MOUSE_0))
            {
                m_mainWindow->SetMouseVisible(false);
                m_mainWindow->ConfineMouseToCenter();
                m_controlsEnabled = true;
            }
        }

        if (m_controlsEnabled)
        {
            // Calc rot.
            {
                auto mt = m_lgx->GetInput().GetMouseDelta();
                m_targetX -= mt.y * dt * ROTATE_AMT;
                m_targetY -= mt.x * dt * ROTATE_AMT;
                m_angleX           = LinaGX::Lerp(m_angleX, m_targetX, dt * ROTATE_SPEED);
                m_angleY           = LinaGX::Lerp(m_angleY, m_targetY, dt * ROTATE_SPEED);
                const glm::quat qx = glm::angleAxis(m_angleX, glm::vec3(1, 0, 0));
                const glm::quat qy = glm::angleAxis(m_angleY, glm::vec3(0, 1, 0));
                m_rotation         = m_initialRot * qy * qx;
            }

            // Calc pos.
            {
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
        }

        // Produce matrices.
        {
            glm::mat4 rotationMatrix    = glm::mat4_cast(glm::inverse(m_rotation));
            glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_position);
            m_view                      = rotationMatrix * translationMatrix;
            m_proj                      = glm::perspective(DEG2RAD(90.0f), static_cast<float>(m_mainWindow->GetSize().x) / static_cast<float>(m_mainWindow->GetSize().y), 0.1f, 500.0f);
        }
    }

} // namespace LinaGX::Examples
