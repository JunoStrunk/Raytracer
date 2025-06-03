#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdbool>
#include <string>
#include <sstream>
#include <crtdefs.h>

using string = std::string;

namespace VulkanCore
{
#define VABORTCODE 0xE0000001
#define VABORT() RaiseException(VABORTCODE, 0, 0, 0)
#define VERROR(_expr, msg) if(!(_expr)) { _RPT1(_CRT_ERROR, "Error: %s\n"#_expr, msg); VABORT(); }

#define VLOG(_stream)                           \
    {                                           \
        std::ostringstream str;                 \
        str << _stream;                         \
        printf(str.str().c_str());              \
    }

#define VALIDATE_VK_RESULT(res, msg)                                                        \
    if (res != VK_SUCCESS)                                                                  \
    {                                                                                       \
        fprintf(stderr, "Error in %s:%d - %s, code %x\n", __FILE__, __LINE__, msg, res);    \
        exit(EXIT_FAILURE);                                                                 \
    }
}