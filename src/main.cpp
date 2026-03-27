/*
* File: main.cpp
* Project: trajan-engine
* Author: Collin Longoria
* Created on: 3/27/2026
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <iostream>
#include <stdexcept>
#include <memory>

#include "Opticus/render_device.hpp"
#include "Opticus/core.hpp"
#include "Opticus/swapchain.hpp"

void FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto* swapchain = static_cast<opticus::ISwapchain*>(glfwGetWindowUserPointer(window));

    if (width > 0 && height > 0 && swapchain) {
        swapchain->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    uint32_t width = 1280;
    uint32_t height = 720;
    GLFWwindow* window = glfwCreateWindow(width, height, "Trajan Engine", nullptr, nullptr);

    auto renderDevice = opticus::CreateRenderDevice(opticus::GraphicsAPI::Vulkan);

    opticus::DeviceInitDescriptor desc{};
    desc.api = opticus::GraphicsAPI::Vulkan;
    desc.preference = opticus::DevicePreference::HighPerformance;
    desc.enableValidationLayers = true;
    desc.frameBufferWidth = width;
    desc.frameBufferHeight = height;

    desc.nativeWindowHandle = glfwGetWin32Window(window);

    if (!renderDevice->Initialize(desc)) {
        std::cerr << "Failed to initialize Opticus RHI\n";
        return -1;
    }

    opticus::ISwapchain* swapchain = renderDevice->CreateSwapchain();
    glfwSetWindowUserPointer(window, swapchain);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        if (width == 0 || height == 0) {
            continue;
        }

        renderDevice->BeginFrame();

        auto* cmdList = renderDevice->GetGraphicsCommandList();

        opticus::Extent2D extent = swapchain->GetExtent();
        opticus::Color clearColor = { 0.05f, 0.05f, 0.08f, 1.0f }; // Dark slate blue

        cmdList->BeginRendering(extent, clearColor);

        cmdList->EndRendering();

        renderDevice->SubmitAndPresent();
    }

    renderDevice->WaitIdle();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
