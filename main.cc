#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <iostream>
#include <cmath>
#include <filesystem>

#include <program.hh>
#include <camera.hh>
#include <model.hh>

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

    // Shadow texture
    uint shadow_tex;
    glGenTextures(1, &shadow_tex);
    glBindTexture(GL_TEXTURE_2D, shadow_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // Framebuffer init
    uint FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_tex, 0);

    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Init main shaders
    auto shaders = program();
    shaders.add_shader("vertex.glsl", GL_VERTEX_SHADER);
    shaders.add_shader("fragment.glsl", GL_FRAGMENT_SHADER);
    shaders.link();

    // Init shadow_shader
    auto shadow_shaders = program();
    shadow_shaders.add_shader("shadow_vertex.glsl", GL_VERTEX_SHADER);
    shadow_shaders.add_shader("shadow_fragment.glsl", GL_FRAGMENT_SHADER);
    shadow_shaders.link();


    // Set shader
    shaders.use();
    shaders.addUniformVec3(vec3(0, 0.5, -1), "light_pos");
    shaders.addUniformVec3(vec3(1.0, 1.0, 1.0), "lightIntensity");


    // Camera view and projection matrices
    Camera cam = Camera(vec3(0, 2, -2), vec3(0, 0, 1), vec3(0, 1, 0));
    mat4 view = cam.look_at();
    shaders.addUniformVec3(cam.get_position(), "cam_pos");
    shaders.addUniformMat4(view, "view");

    mat4 projection = mat4(1.0);
    projection = perspective(radians(60.0f), (float)width / (float)height, 0.1f, 100.0f);
    shaders.addUniformMat4(projection, "projection");


    // Shadow map view and projection matrices
    shadow_shaders.use();
    Camera shadow_cam = Camera(vec3(0, 0.5, -1), vec3(0, 0, 0), vec3(0, 1, 0));
    view = shadow_cam.look_at();
    shadow_shaders.addUniformMat4(view, "view");

    projection = mat4(1.0);
    projection = ortho<float>(-10, 10, -10, 10, -10, 20);
    shadow_shaders.addUniformMat4(projection, "projection");

    // Add light matrices to shader
    shaders.use();
    shaders.addUniformMat4(view, "light_view");
    shaders.addUniformMat4(projection, "light_projection");

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

    model = rotate(model, radians(angle), vec3(0.0, 1.0, 0.0));
    auto teapot = Model("models/teapot.obj", model, diffuse1, spec, shininess);

    model = mat4(1.0);
    model = translate(model, vec3(-2,-0.5,2));
    auto cube = Model("models/cube.obj", model, diffuse2, spec, shininess);

    model = mat4(1.0);
    model = scale(model, vec3(5,5,5));
    model = translate(model, vec3(0,-0.2, 0));
    auto plane = Model("models/wall.obj", model, diffuse3, spec, shininess);


    // Render loop
    while(!glfwWindowShouldClose(window))
    {
        // Callback quit window with KEY_ESCAPE
        quit_window(window);

        // Update model matrices
        rad_off = pause_rotation(window, rad_off);
        auto teapot_model = teapot.get_model();
        teapot_model = rotate(teapot_model, radians(rad_off), vec3(0.0, 1.0, 0.0));
        teapot.set_model(teapot_model);

        auto cube_model = cube.get_model();
        cube_model = rotate(cube_model, radians(-rad_off), vec3(1.0, 0.0, 0.0));
        cube.set_model(cube_model);

        // Shadow computing
        shadow_shaders.use();
        glCullFace(GL_FRONT);
        glViewport(0,0,1024,1024);
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glDrawBuffer(GL_NONE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        teapot.draw(shadow_shaders);
        cube.draw(shadow_shaders);
        plane.draw(shadow_shaders);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders.use();

        glCullFace(GL_BACK);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadow_tex);

        //Draw objects
        teapot.draw(shaders);
        cube.draw(shaders);
        plane.draw(shaders);


        // Check and call events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Clear bufffer
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    // glDeleteFramebuffers(FBO);
    glfwTerminate();
    return 0;
}
