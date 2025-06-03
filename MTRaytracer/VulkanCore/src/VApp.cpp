#include "pch.h"
#include "VApp.h"

namespace VulkanCore
{

    void VApp::Init(const string& appName, GLFWwindow* window)
    {
        m_CoreInstance.Init(appName, window);
    }

    void VApp::DeInit()
    {
        m_CoreInstance.DeInit();
    }
}

