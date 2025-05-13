#include "pch.h"
#include "VulkanApp.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>

namespace VulkanCore
{
    VulkanApp::~VulkanApp()
    {

    }

    void VulkanApp::Init(const string appName)
    {
        _CreateInstance(appName);
    }

    void VulkanApp::DeInit()
    {
        vkDestroyInstance(_Instance, NULL);
        printf("Vulkan Instance Destroyed\n"); // TODO: Replace with Log
    }

    void VulkanApp::_CreateInstance(const string& appName)
    {
        // Validation layers ===
        const std::vector<const char*> VALIDATION_LAYERS = {
            "VK_LAYER_KHRONOS_validation"
        };

#ifdef NDEBUG
        const bool ENABLE_VALIDATION_LAYERS = false;
#else
        const bool ENABLE_VALIDATION_LAYERS = true;
#endif

        // Check if validation layers are supported

        const std::vector<const char*> Extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
            "VK_KHR_win32_surface",
#endif
#if defined(__APPLE__)
            "VK_MVK_macos_surface",
#endif
#if defined(__linux__)
            "VK_KHR_xcb_surface",
#endif
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            VK_EXT_DEBUG_REPORT_EXTENSION_NAME
        };

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Wilco";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;


        VkResult result = vkCreateInstance(&createInfo, nullptr, &_Instance);
        VALIDATE_VK_RESULT(result, "Create Instance");
        printf("Vulkan instance created\n"); // TODO: Replace with Log
    }
}

