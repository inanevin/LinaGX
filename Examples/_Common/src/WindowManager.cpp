#include "WindowManager.hpp"
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

#pragma once

#define GLAD_GL_IMPLEMENTATION
#include "GLFW/glfw3.h"
#include <iostream>

namespace LinaGX
{
    namespace Examples
    {

        static void GLFWErrorCallback(int error, const char* desc)
        {
            std::cerr << "LinaGX Examples: GLFW Error: " << error << " Description: " << desc << std::endl;
        }

        void* WindowManager::CreateWindow(LinaGX::BackendAPI backendAPI, int width, int height, const char* title)
        {
            // Initialize GLFW
            if (!glfwInit())
            {
                return nullptr;
            }

                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            glfwSetErrorCallback(GLFWErrorCallback);

            m_windowHandle = (void*)(glfwCreateWindow(width, height, title, NULL, NULL));

            if (!m_windowHandle)
            {
                // Assert window creation.
                GLFWErrorCallback(0, "LinaGX: GLFW window failed to initialize!");
                glfwTerminate();
                return nullptr;
            }

            // set user pointer for callbacks.
            glfwSetWindowUserPointer((GLFWwindow*)m_windowHandle, this);

            auto windowResizeFunc = [](GLFWwindow* w, int wi, int he) {

            };

            auto windowCloseFunc = [](GLFWwindow* w) {

            };

            auto windowKeyFunc = [](GLFWwindow* w, int key, int scancode, int action, int modes) {

            };

            auto windowButtonFunc = [](GLFWwindow* w, int button, int action, int modes) {

            };

            auto windowMouseScrollFunc = [](GLFWwindow* w, double xOff, double yOff) {
            };

            auto windowCursorPosFunc = [](GLFWwindow* w, double xPos, double yPos) {

            };

            auto windowFocusFunc = [](GLFWwindow* w, int f) {

            };

            std::cout << "LinaGX Examples: Window initialized successfully." << std::endl;
            return m_windowHandle;
        }

        void WindowManager::Poll()
        {
            glfwPollEvents();
        }

        void WindowManager::Shutdown()
        {
            glfwTerminate();
        }

    } // namespace Examples
} // namespace LinaGX
