#pragma once

#include "Types.h"
#include "VCore.h"

namespace VulkanCore
{
    class VApp
    {
    public:
        VApp() = default;
        VApp(const VApp& other) = delete;
        VApp& operator=(const VApp& rhs) = delete;
        VApp(const VApp&& other) = delete;
        VApp& operator=(const VApp&& rhs) = delete;
        ~VApp() = default;

        void Init(const string& appName, GLFWwindow* window);
        void DeInit();
    private:

        VCore m_CoreInstance;
    };
}
