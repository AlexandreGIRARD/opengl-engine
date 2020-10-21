#include <iostream>
#include <cmath>
#include <filesystem>
#include <memory>

#include "scene.hh"

using namespace glm;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./engine <scene.json>" << std::endl;
        exit(-1);
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    uint width = 1920;
    uint height = 1080;
    auto window = init_window(width, height);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        exit(-1);
    }

    // Debug
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(debug_callback, 0);

    // Tell OpenGL window size
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Enable z-buffer computation
    glEnable(GL_DEPTH_TEST);

    // Patch size = 3
    glPatchParameteri(GL_PATCH_VERTICES, 3);

    Scene scene = Scene(argv[1]);

    // Delta time setup
    double time = glfwGetTime();
    double delta = 0.0;

    // Mouse event setup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    double xpos, ypos;

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        // Callback quit window with KEY_ESCAPE
        quit_window(window);

        // Compute delta time
        delta = glfwGetTime() - time;
        time = glfwGetTime();
        // std::cout << time << std::endl;
        frame_rate(time);

        // Get mouse event (position variations)
        glfwGetCursorPos(window, &xpos, &ypos);

        scene.render(window, delta, xpos, ypos);

        // Check and call events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clear bufffer
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glfwTerminate();
    return 0;
}
