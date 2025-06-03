#pragma once

#include "Types.h"
#include <optional>

namespace VulkanCore
{
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily;

        bool isComplete()
        {
            return graphicsFamily.has_value();
        }
    };

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
        // Init Functions
        void _CreateInstance(const string& appName);
        void _CreateDebugCallback();
        void _CreateSurface(GLFWwindow* window);
        void _SelectPhysicalDevice();
        void _CreateLogicalDevice();

        // Utility (For Init)
        bool _isDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device);

        VkInstance m_Instance = nullptr;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface;
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
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

