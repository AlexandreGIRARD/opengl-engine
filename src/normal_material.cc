#include "normal_material.hh"

Normal_Material::Normal_Material(vec3 diffuse, vec3 specular, float shininess)
    : _diffuse(diffuse)
    , _specular(specular)
    , _shininess(shininess)
{}

void Normal_Material::set_uniforms(program p)
{
    p.addUniformUint(0, "is_textured");
    
    p.addUniformVec3(_diffuse, "mtl.diffuse");
    p.addUniformVec3(_specular, "mtl.specular");
    p.addUniformFloat(_shininess, "mtl.shininess");
}
