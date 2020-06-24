#include "textured_material.hh"

Textured_Material::Textured_Material(std::string path)
{
    auto tmp_name = path + "basecolor.jpg";
    _diffuse = load_texture(tmp_name.c_str());
    tmp_name = path + "normal.jpg";
    _normal = load_texture(tmp_name.c_str());
    tmp_name = path + "height.jpg";
    _height = load_texture(tmp_name.c_str());
}

void Textured_Material::set_uniforms(program p)
{
    p.addUniformUint(1, "is_textured");

    p.addUniformTexture(0, "tex.diffuse");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _diffuse);

    p.addUniformTexture(1, "tex.normal");
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, _normal);

    p.addUniformTexture(2, "tex.height");
    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, _height);
}
