#include "VCore.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <algorithm>

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

        for (auto imageView : m_SCImageViews)
        {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }
        VLOG("ImageViewsDestroyed\n");

        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
        VLOG("SwapchainDestroyed\n");

        vkDestroyDevice(m_Device, nullptr);
        VLOG("LogicalDeviceDestroyed\n");

        vkDestroyInstance(m_Instance, NULL);
        VLOG("VulkanInstanceDestroyed\n");
    }

    void VCore::_CreateInstance(const string& appName)
    {
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
        createInfo.enabledLayerCount = uint32_t(m_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
        createInfo.ppEnabledExtensionNames = m_Extensions.data();
        createInfo.enabledExtensionCount = uint32_t(m_Extensions.size());


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
        createInfo.enabledExtensionCount = uint32_t(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;

        VkResult createDeviceResult = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device);
        VERROR(createDeviceResult == VK_SUCCESS, "Failed to create logical device");

        vkGetDeviceQueue(m_Device, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    }

    void VCore::_CreateSwapChain(GLFWwindow* window)
    {
        SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(m_PhysicalDevice);

        VkSurfaceFormatKHR surfaceFormat = _ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        VkExtent2D extent = _ChooseSwapExtent(window, swapChainSupport.capabilities);

        /* According to Vulkan-Tutorial.com...
        * However, simply sticking to this minimum means that we may sometimes
        * have to wait on the driver to complete internal operations before we
        * can acquire another image to render to. Therefore it is recommended to
        * request at least one more image than the minimum:
        */
        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VkResult result = vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain);
        VERROR(result == VK_SUCCESS, "Unable to create swap chain!");

        // Retrieve images from swap chain
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
        m_SCImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SCImages.data());

        // Set format and extent
        m_SCImageFormat = surfaceFormat.format;
        m_SCExtent = extent;
    }

    void VCore::_CreateImageViews()
    {
        m_SCImageViews.resize(m_SCImages.size());

        for (size_t i = 0; i < m_SCImages.size(); ++i)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SCImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SCImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            VkResult result = vkCreateImageView(m_Device, &createInfo, nullptr, &m_SCImageViews[i]);
            VERROR(result == VK_SUCCESS, "Couldn't create image view");
        }
    }

    bool VCore::_isDeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        QueueFamilyIndices indices = _FindQueueFamilies(device);

        bool swapChainAdequate = false;
        SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

        return indices.isComplete() && swapChainAdequate;
    }

    /// @brief Look at the device's queues, see if it has one that supports graphics
    /// @param device - device to check queues
    /// @return Wrapper for the index of the queue that supports the graphics bit
    VCore::QueueFamilyIndices VCore::_FindQueueFamilies(VkPhysicalDevice device)
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

    VCore::SwapChainSupportDetails VCore::_QuerySwapChainSupport(VkPhysicalDevice device)
    {
        SwapChainSupportDetails details;

        // Get surface capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);
        
        // Get physical device formats
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        // Get Presentation modes
        uint32_t presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModesCount, nullptr);

        if (presentModesCount != 0)
        {
            details.presentModes.resize(presentModesCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModesCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VCore::_ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            // Format is BGRA in that order with an 8 bit unsigned integer each
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        
        // If we can't find our preferred, default to first.
        return availableFormats[0];
    }
    
    VkExtent2D VCore::_ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != uint32_t(std::numeric_limits<uint32_t>::max))
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        
            return actualExtent;
        }
    }
}