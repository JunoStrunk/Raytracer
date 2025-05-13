#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <string>

using string = std::string;

namespace VulkanCore
{
#define VALIDATE_VK_RESULT(res, msg)                                                        \
    if (res != VK_SUCCESS)                                                                  \
    {                                                                                       \
        fprintf(stderr, "Error in %s:%d - %s, code %x\n", __FILE__, __LINE__, msg, res);    \
        exit(EXIT_FAILURE);                                                                 \
    }
}