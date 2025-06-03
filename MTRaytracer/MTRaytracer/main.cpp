#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080
#define APP_NAME "MyMultithreadedVulkanRaytracerMyPrecious"

#include "VApp.h"

int main(int argc, char* argv[]) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, APP_NAME, nullptr, nullptr);

    VulkanCore::VApp app;
    app.Init(APP_NAME, window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    app.DeInit();

    glfwTerminate();

    return 0;

}