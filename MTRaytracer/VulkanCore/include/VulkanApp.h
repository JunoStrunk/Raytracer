#pragma once

#include "Types.h"

namespace VulkanCore
{

    class VulkanApp
    {
    public:
        VulkanApp() = default;
        VulkanApp(const VulkanApp& other) = delete;
        VulkanApp& operator=(const VulkanApp& rhs) = delete;
        VulkanApp(const VulkanApp&& other) = delete;
        VulkanApp& operator=(const VulkanApp&& rhs) = delete;
        ~VulkanApp();

        void Init(const string appName);
        void DeInit();
    private:
        void _CreateInstance(const string& appName);

        VkInstance _Instance = nullptr;
    };

}
