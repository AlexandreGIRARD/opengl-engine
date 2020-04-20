#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <cmath>
#include <filesystem>
#include <memory>

#include <program.hh>
#include <camera.hh>
#include <model.hh>
#include <light.hh>

using namespace glm;

float switch_off = 0;

void quit_window(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float pause_rotation(GLFWwindow *window, float rad_off)
{
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        auto tmp = switch_off;
        switch_off = rad_off;
        return tmp;
    }
    return rad_off;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}


GLFWwindow *init_window(uint width, uint height)
{
    GLFWwindow *window = glfwCreateWindow(width, height, "ZIZI", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    return window;
}

int main(int argc, char *argv[])
{
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

    // Tell OpenGL window size
    glViewport(0, 0, width, height);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Enable z-buffer computation
    glEnable(GL_DEPTH_TEST);

    // Init main shaders
    auto shaders = program();
    shaders.add_shader("vertex.glsl", GL_VERTEX_SHADER);
    shaders.add_shader("fragment.glsl", GL_FRAGMENT_SHADER);
    shaders.link();

    // Set shader
    shaders.use();

    // Camera view and projection matrices
    Camera cam = Camera(vec3(0, 2, -2), vec3(0, 0, 1), vec3(0, 1, 0));
    mat4 view = cam.look_at();
    shaders.addUniformVec3(cam.get_position(), "cam_pos");
    shaders.addUniformMat4(view, "view");

    mat4 projection = mat4(1.0);
    projection = perspective(radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
    shaders.addUniformMat4(projection, "projection");

    // Light init
    Light light1 = Light(DIRECTIONAL, vec3(0, 0.5, -1), vec3(1, 1, 1), vec3(1, 1, 1));
    light1.setup_program(vec3(0, 0, 0), vec3(0, 0.5, -1));
    light1.set_light_in_program(shaders);

    // Material setting
    vec3 diffuse1 = vec3(1,0,0); //Red
    vec3 diffuse2 = vec3(0,1,1); //Cyan
    vec3 diffuse3 = vec3(0,1,0); //Green
    vec3 spec = vec3(0.7, 0.7, 0.7); // rubber
    float shininess = 0.25f;

    // Models matrix
    mat4 model = mat4(1.0);
    model = translate(model, vec3(2,-1,2));
    auto angle = -150.f;
    auto rad_off = 0.2f;

    std::vector<std::shared_ptr<Model>> models;

    model = rotate(model, radians(angle), vec3(0.0, 1.0, 0.0));
    auto teapot = std::make_shared<Model>("models/teapot.obj", model, diffuse1, spec, shininess);
    models.emplace_back(teapot);

    model = mat4(1.0);
    model = translate(model, vec3(-2,-0.5,2));
    auto cube = std::make_shared<Model>("models/cube.obj", model, diffuse2, spec, shininess);
    models.emplace_back(cube);

    model = mat4(1.0);
    model = scale(model, vec3(5,5,5));
    model = translate(model, vec3(0,-0.2, 0));
    auto plane = std::make_shared<Model>("models/wall.obj", model, diffuse3, spec, shininess);
    models.emplace_back(plane);

    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        // Callback quit window with KEY_ESCAPE
        quit_window(window);

        // Move camera
        cam.update(window);

        // Update camera view and projection matrices
        mat4 view = cam.look_at();
        shaders.addUniformVec3(cam.get_position(), "cam_pos");
        shaders.addUniformMat4(view, "view");

        mat4 projection = mat4(1.0);
        projection = perspective(radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
        shaders.addUniformMat4(projection, "projection");

        // Update model matrices
        rad_off = pause_rotation(window, rad_off);
        auto teapot_model = teapot->get_model();
        teapot_model = rotate(teapot_model, radians(rad_off), vec3(0.0, 1.0, 0.0));
        teapot->set_model(teapot_model);

        auto cube_model = cube->get_model();
        cube_model = rotate(cube_model, radians(-rad_off), vec3(1.0, 0.0, 0.0));
        cube->set_model(cube_model);

        // Shadow computing
        light1.draw_shadow_map(models);

        // Render
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shaders.use();
        glCullFace(GL_BACK);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light1.get_map());

        //Draw objects
        for (auto model : models)
            model->draw(shaders);

        // Check and call events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clear bufffer
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    glfwTerminate();
    return 0;
}
