#pragma once

#include "Types.h"
#include <optional>
#include <vector>

namespace VulkanCore
{
    class VCore
    {
    public:
        VCore() = default;
        VCore(const VCore& other) = delete;
        VCore& operator=(const VCore& rhs) = delete;
        VCore(const VCore&& other) = delete;
        VCore& operator=(const VCore&& rhs) = delete;
        ~VCore() = default;

        void Init(const string& appName, GLFWwindow* window);
        void DeInit();

    private:

        // Private structs
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;

            bool isComplete()
            {
                return graphicsFamily.has_value();
            }
        };

        struct SwapChainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        // Init Functions
        void _CreateInstance(const string& appName);
        void _CreateDebugCallback();
        void _CreateSurface(GLFWwindow* window);
        void _SelectPhysicalDevice();
        void _CreateLogicalDevice();
        void _CreateSwapChain(GLFWwindow* window);
        void _CreateImageViews();

        // Utility (For Init)
        bool _isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);

        // Utility for Swap Chain
        SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);
        VkSurfaceFormatKHR _ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkExtent2D _ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);

        // Member Variables
        VkInstance m_Instance = nullptr;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;

        // SwapChain
        VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
        VkFormat m_SCImageFormat;
        VkExtent2D m_SCExtent;
        std::vector<VkImage> m_SCImages;
        std::vector<VkImageView> m_SCImageViews;

        // Layers and Extensions
        const std::vector<const char*> m_ValidationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        const std::vector<const char*> m_Extensions = {
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

        const std::vector<const char*> m_DeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
    };

}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    printf("Debug callback: %s\n", pCallbackData->pMessage);
    // Get severity string
    // Get debug type
    printf("Objects:\n");

    for(uint32_t i = 0; i < pCallbackData->objectCount; ++i)
    {
        printf("%llx ", pCallbackData->pObjects[i].objectHandle);
    }

    return VK_FALSE;
}

