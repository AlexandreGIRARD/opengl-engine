#include "util.hh"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static float switch_off = 0;
static float fps = 0.f;
static float last_time = 0.f;

uint load_texture(const char *path)
{
    uint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Texture parameter -> Bilinear and repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nbChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nbChannels, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return tex;
}

uint load_cubemap(const std::string path, const std::string faces[6])
{
    auto toto = path + "/" + faces[0];

    uint cubemap;
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nb_channel;
    for (auto i=0; i < 6; i++)
    {
        auto tmp_string = path + "/" + faces[i];
        auto img_path = tmp_string.c_str();
        unsigned char *data = stbi_load(img_path, &width, &height, &nb_channel, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "Nope pas trouvé BG" << std::endl;

        stbi_image_free(data);
    }

    return cubemap;
}

float frame_rate(float time)
{
    float current_time = time;
    ++fps;
    if( current_time - last_time > 1.0f )
    {
        last_time = current_time;
        fps = 0;
    }
    return fps;
}
void GLAPIENTRY debug_callback(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar* message,
                               const void* userParam )
{
    {
        const char* _source;
        const char* _type;
        const char* _severity;

        switch (source) {
            case GL_DEBUG_SOURCE_API:
            _source = "API";
            break;

            case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            _source = "WINDOW SYSTEM";
            break;

            case GL_DEBUG_SOURCE_SHADER_COMPILER:
            _source = "SHADER COMPILER";
            break;

            case GL_DEBUG_SOURCE_THIRD_PARTY:
            _source = "THIRD PARTY";
            break;

            case GL_DEBUG_SOURCE_APPLICATION:
            _source = "APPLICATION";
            break;

            case GL_DEBUG_SOURCE_OTHER:
            _source = "UNKNOWN";
            break;

            default:
            _source = "UNKNOWN";
            break;
        }

        switch (type) {
            case GL_DEBUG_TYPE_ERROR:
            _type = "ERROR";
            break;

            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            _type = "DEPRECATED BEHAVIOR";
            break;

            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            _type = "UDEFINED BEHAVIOR";
            break;

            case GL_DEBUG_TYPE_PORTABILITY:
            _type = "PORTABILITY";
            break;

            case GL_DEBUG_TYPE_PERFORMANCE:
            _type = "PERFORMANCE";
            break;

            case GL_DEBUG_TYPE_OTHER:
            _type = "OTHER";
            break;

            case GL_DEBUG_TYPE_MARKER:
            _type = "MARKER";
            break;

            default:
            _type = "UNKNOWN";
            break;
        }

        switch (severity) {
            case GL_DEBUG_SEVERITY_HIGH:
            _severity = "HIGH";
            break;

            case GL_DEBUG_SEVERITY_MEDIUM:
            _severity = "MEDIUM";
            break;

            case GL_DEBUG_SEVERITY_LOW:
            _severity = "LOW";
            break;

            case GL_DEBUG_SEVERITY_NOTIFICATION:
            _severity = "NOTIFICATION";
            break;

            default:
            _severity = "UNKNOWN";
            break;
        }

        fprintf(stderr, "GL ERROR: Type = %s, Severity = %s, Message = %s\n",
            _type, _severity, message );
    }
}

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
