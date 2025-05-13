//#pragma once
//#include "Types.h"
//#include <iostream>
//#include <stdexcept>
//#include <cstdlib>
//#include <vector>
//#include <optional>
//
//#include <vulkan/vulkan.h>
//
//class HelloTriangleApplication {
//
//    const uint32_t WIDTH = 800;
//    const uint32_t HEIGHT = 600;
//
//    // Validation layers ===
//    const std::vector<const char*> VALIDATION_LAYERS = {
//        "VK_LAYER_KHRONOS_validation"
//    };
//
//#ifdef NDEBUG
//    const bool ENABLE_VALIDATION_LAYERS = false;
//#else
//    const bool ENABLE_VALIDATION_LAYERS = true;
//#endif
//
//public:
//    void Run() {
//        _InitWindow();
//        _InitVulkan();
//        _MainLoop();
//        _Cleanup();
//    }
//
//private:
//    GLFWwindow* _Window;
//    VkInstance _Instance;
//
//    void _InitWindow()
//    {
//        glfwInit();
//        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // Set client api to no api, since we aren't using opengl
//        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable resizing windows
//
//        _Window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
//    }
//
//    void _InitVulkan()
//    {
//        _CreateInstance();
//        _PickPhysicalDevice();
//    }
//
//    void _MainLoop()
//    {
//        while (!glfwWindowShouldClose(_Window))
//        {
//            glfwPollEvents();
//        }
//    }
//
//    void _Cleanup()
//    {
//        vkDestroyInstance(_Instance, nullptr);
//
//        glfwDestroyWindow(_Window);
//
//        glfwTerminate();
//    }
//
//    void _CreateInstance()
//    {
//        // Check if validation layers are supported
//        if (ENABLE_VALIDATION_LAYERS && !_CheckValidationLayerSupport())
//        {
//            throw std::runtime_error("validation layers requested, but not available.");
//        }
//
//        /*
//        Now, to create an instance we'll first have to fill in a struct with some information about our application.
//        This data is technically optional, but it may provide some useful information to the driver in order to
//        optimize our specific application (e.g. because it uses a well-known graphics engine with certain special
//        behavior). This struct is called VkApplicationInfo:
//        */
//
//        VkApplicationInfo appInfo{};
//        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//        appInfo.pApplicationName = "Hello Triangle";
//        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//        appInfo.pEngineName = "No Engine";
//        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//        appInfo.apiVersion = VK_API_VERSION_1_0;
//
//        // This one is not optional
//        VkInstanceCreateInfo createInfo{};
//        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//        createInfo.pApplicationInfo = &appInfo;
//
//        // Need extensions to interface with window system (Because cross platform)
//        uint32_t glfwExtensionCount = 0;
//        const char** glfwExtensions;
//        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//        createInfo.enabledExtensionCount = glfwExtensionCount;
//        createInfo.ppEnabledExtensionNames = glfwExtensions;
//
//        if (ENABLE_VALIDATION_LAYERS)
//        {
//            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
//            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
//        }
//        else
//        {
//            createInfo.enabledLayerCount = 0;
//        }
//
//        if (vkCreateInstance(&createInfo, nullptr, &_Instance) != VK_SUCCESS)
//        {
//            throw std::runtime_error("failed to create instance!");
//        }
//    }
//
//    bool _CheckValidationLayerSupport()
//    {
//        uint32_t layerCount;
//        vkEnumerateInstanceLayerProperties(&layerCount, nullptr); // Get how many layers are possible
//
//        std::vector<VkLayerProperties> availableLayers(layerCount);
//        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()); // Actually fill with layer data
//
//        for (const char* layerName : VALIDATION_LAYERS)
//        {
//            bool layerFound = false;
//
//            for (const auto& layerProperties : availableLayers)
//            {
//                if (strcmp(layerName, layerProperties.layerName) == 0)
//                {
//                    layerFound = true;
//                    break;
//                }
//            }
//
//            if (!layerFound) // If there is any layer that isn't supported, exit
//            {
//                return false;
//            }
//        }
//
//        return true;
//    }
//
//    void _PickPhysicalDevice()
//    {
//        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
//
//        uint32_t deviceCount = 0;
//        vkEnumeratePhysicalDevices(_Instance, &deviceCount, nullptr);
//
//        if (deviceCount == 0)
//        {
//            throw std::runtime_error("failed to find GPUs with Vulkan support!");
//        }
//
//        std::vector<VkPhysicalDevice> devices(deviceCount);
//        vkEnumeratePhysicalDevices(_Instance, &deviceCount, devices.data());
//
//        for (const auto& device : devices)
//        {
//            if (_IsDeviceSuitable(device))
//            {
//                physicalDevice = device;
//                break;
//            }
//        }
//
//        if (physicalDevice == VK_NULL_HANDLE)
//        {
//            throw std::runtime_error("failed to find a suitable GPU!");
//        }
//    }
//
//    bool _IsDeviceSuitable(VkPhysicalDevice device)
//    {
//        //VkPhysicalDeviceFeatures deviceFeatures;
//        //vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
//
//        //VkPhysicalDeviceProperties deviceProperties;
//        //vkGetPhysicalDeviceProperties(device, &deviceProperties);
//
//        //// If it's discrete and has a geometry shader, then it is suitable!
//        //return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
//
//        QueueFamilyIndices indices = _FindQueueFamilies(device);
//
//        return indices.GraphicsFamily.has_value();
//    }
//
//    struct QueueFamilyIndices
//    {
//        std::optional<uint32_t> GraphicsFamily;
//    };
//
//    QueueFamilyIndices _FindQueueFamilies(VkPhysicalDevice device)
//    {
//        QueueFamilyIndices indices;
//
//        // Assign index to queue families that could be found
//        uint32_t queueFamilyCount = 0;
//        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr); // Get number of queue families
//
//        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data()); // Get number of queue families
//
//        int i = 0;
//        for (const auto& queueFamily : queueFamilies)
//        {
//            // https://registry.khronos.org/vulkan/specs/latest/man/html/VkQueueFlagBits.html
//            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
//            {
//                indices.GraphicsFamily = i;
//            }
//        }
//
//        return indices;
//    }
//};
//
////int main(int argc, char* argv[]) {
////    /*HelloTriangleApplication app;
////
////    try {
////        app.Run();
////    }
////    catch (const std::exception& e) {
////        std::cerr << e.what() << std::endl;
////        return EXIT_FAILURE;
////    }
////
////    return EXIT_SUCCESS;*/
////
////
////}