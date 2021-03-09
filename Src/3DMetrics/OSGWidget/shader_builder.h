#ifndef SHADERBUILDER_H
#define SHADERBUILDER_H

#include "shader_color.h"

#include <osg/Shader>

class ShaderBuilder
{
public:

    enum ShaderType
    {
        NoShader, // only alpha processing
        GrayShader, // gray palette in Z
        Standard, // standard palette from Shader Color
        EyeDomeLighting
    };

    static osg::Shader * vertexShader(const ShaderType _shader, const ShaderColor::Palette _palette);
    static osg::Shader * fragmentShader(const ShaderType _shader);

};

#endif // SHADERBUILDER_H
