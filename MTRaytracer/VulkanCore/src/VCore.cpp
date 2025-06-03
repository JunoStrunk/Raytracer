#include "VCore.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>

namespace VulkanCore
{
    void VCore::Init(const string& appName, GLFWwindow* window)
    {
        _CreateInstance(appName);
        _CreateDebugCallback();
        _CreateSurface(window);
        _SelectPhysicalDevice();
        _CreateLogicalDevice();
    }

    void VCore::DeInit()
    {
        PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessenger = VK_NULL_HANDLE;
        vkDestroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
        VERROR(vkDestroyDebugUtilsMessenger != nullptr, "Cannot find address of vkDestroyDebugUtilsMessenger\n");

        vkDestroyDebugUtilsMessenger(m_Instance, m_DebugMessenger, nullptr);

        VLOG("DebugCallbackDestroyed\n");

        PFN_vkDestroySurfaceKHR vkDestroySurface = VK_NULL_HANDLE;
        vkDestroySurface = (PFN_vkDestroySurfaceKHR)vkGetInstanceProcAddr(m_Instance, "vkDestroySurfaceKHR");
        VERROR(vkDestroySurface != nullptr, "Cannot find address of vkDestroySurfaceKHR");

        vkDestroySurfaceKHR(m_Instance, m_Surface, NULL);
        VLOG("SurfaceDestroyed\n");

        vkDestroyDevice(m_Device, nullptr);
        VLOG("LogicalDeviceDestroyed\n");

        vkDestroyInstance(m_Instance, NULL);
        VLOG("VulkanInstanceDestroyed\n");
    }

    void VCore::_CreateInstance(const string& appName)
    {
        // Validation layers ===
        const std::vector<const char*> ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

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
        createInfo.flags = 0;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = uint32_t(ValidationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers.data();
        createInfo.ppEnabledExtensionNames = Extensions.data();
        createInfo.enabledExtensionCount = uint32_t(Extensions.size());


        VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
        VALIDATE_VK_RESULT(result, "Create Instance");
        VLOG("Vulkan instance created\n");
    }

    void VCore::_CreateDebugCallback()
    {
        VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo{};
        MessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        MessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        MessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        MessengerCreateInfo.pfnUserCallback = &DebugCallback;
        MessengerCreateInfo.pUserData = nullptr;

        PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessenger = VK_NULL_HANDLE;
        vkCreateDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
        VERROR(vkCreateDebugUtilsMessenger != nullptr, "Cannot find address of vkCreateDebugUtilsMessenger\n");

        VkResult res = vkCreateDebugUtilsMessenger(m_Instance, &MessengerCreateInfo, nullptr, &m_DebugMessenger);
        VALIDATE_VK_RESULT(res, "Debug Utils Messenger");

        VLOG("Debug Utils Messenger Created\n");
    }

    void VCore::_CreateSurface(GLFWwindow* window)
    {
        if (glfwCreateWindowSurface(m_Instance, window, NULL, &m_Surface) == VK_ERROR_INITIALIZATION_FAILED)
        {
            VLOG("Error creating surface");
            exit(EXIT_FAILURE);
        }

        VLOG("Surface Created");
    }

    void VCore::_SelectPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
        VERROR(deviceCount > 0, "Failed to find GPUs with Vulkan Support");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (const auto& device : devices)
        {
            if (_isDeviceSuitable(device))
            {
                m_PhysicalDevice = device;
                break;
            }
        }

        VERROR(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
    }

    void VCore::_CreateLogicalDevice()
    {
        QueueFamilyIndices indices = _FindQueueFamilies(m_PhysicalDevice);
        
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        VkResult createDeviceResult = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
        VERROR(createDeviceResult == VK_SUCCESS, "Failed to create logical device");

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    }

    bool VCore::_isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = _FindQueueFamilies(device);

        return indices.isComplete();
    }

    /// @brief Look at the device's queues, see if it has one that supports graphics
    /// @param device - device to check queues
    /// @return Wrapper for the index of the queue that supports the graphics bit
    QueueFamilyIndices VCore::_FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        size_t i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }
}