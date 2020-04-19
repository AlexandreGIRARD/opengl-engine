#include <program.hh>
#include <fstream>
#include <sstream>
#include <iostream>

program::program()
{
    _id_program = glCreateProgram();
}

program::~program()
{}

void program::add_shader(std::string path, int shader_type)
{
    // Read file
    std::ifstream shader("shaders/" + path);
    std::stringstream buffer;
    buffer << shader.rdbuf();
    shader.close();
    std::string tmp_string = buffer.str();
    const char *code = tmp_string.c_str();

    // Create shader
    uint id = glCreateShader(shader_type);
    glShaderSource(id, 1, &code, NULL);
    glCompileShader(id);

    int success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "Error at shader compilation: " << path << "\n" << infoLog << std::endl;
        exit(-1);
    }

    glAttachShader(_id_program, id);
    glDeleteShader(id);

    _id_shaders.emplace_back(id);
}

void program::link()
{
    glLinkProgram(_id_program);
}

void program::use()
{
    glUseProgram(_id_program);
}

void program::addUniformTexture(const uint unit, const char *name)
{
    glUniform1i(glGetUniformLocation(_id_program, name), unit);
}

void program::addUniformVec3(const glm::vec3 vector, const char *name)
{
    glUniform3f(glGetUniformLocation(_id_program, name), vector.x, vector.y, vector.z);
}

void program::addUniformMat4(glm::mat4 &matrix, const char *name)
{
    int location = glGetUniformLocation(_id_program, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void program::addUniformUint(uint val, const char* name)
{
    glUniform1ui(glGetUniformLocation(_id_program, name), val);
}

void program::addUniformFloat(float val, const char* name)
{
    glUniform1f(glGetUniformLocation(_id_program, name), val);
}
