#include "skybox.hh"

#define STB_IMAGE_IMPLEMENTATION  // Required by stb_image.h
#include "stb_image.h"


Skybox::Skybox(std::vector<std::string> faces)
    : _faces(faces)
{}

unsigned int Skybox::loadCubemap()
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < _faces.size(); i++)
    {
        unsigned char *data = stbi_load(
            _faces[i].c_str(), &width, &height, &nrChannels, 0
        );
        if (data)
        {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: "
                << _faces[i]
                << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Skybox::setup_program()
{
    _program = program();
    _program.add_shader("skybox_vertex.glsl", GL_VERTEX_SHADER);
    _program.add_shader("skybox_vertex.glsl", GL_VERTEX_SHADER);
    _program.link();
    _program.use();
}

void Skybox::draw_cubemap(unsigned int cubemapTexture)
{
    unsigned int VAO;

    // glViewport(0,0,2048,2048);
    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    // glCullFace(GL_FRONT);

    glDepthMask(GL_FALSE);
    // skyboxShader.use();
    _program.use();
    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE);
}
 // SKYBOX : END
