#include "shader_builder.h"

osg::Shader *ShaderBuilder::vertexShader(const ShaderBuilder::ShaderType _shader, const ShaderColor::Palette _palette)
{
    const std::string vertexSourceStandardBegin =
            "#version 130 \n"
            "uniform float zmin;"
            "uniform float deltaz;"
            "uniform float alpha;"
            "uniform float pointsize;"

            "out vec3 vertex_light_position;"
            "out vec3 vertex_light_half_vector;"
            "out vec3 vertex_normal;"
            "out vec4 fcolor;";

    const std::string vertexSourceStandardEnd =
            "void main(void)"
            "{"
            // Calculate the normal value for this vertex, in world coordinates (multiply by gl_NormalMatrix)
            "    vertex_normal = normalize(gl_NormalMatrix * gl_Normal);"
            // Calculate the light position for this vertex
            "    vertex_light_position = normalize(gl_LightSource[0].position.xyz);"

            // Calculate the light's half vector
            "    vertex_light_half_vector = normalize(gl_LightSource[0].halfVector.xyz);"

            "    vec4 v = vec4(gl_Vertex);"
            "    float val = (v.z-zmin) / deltaz;"
            ""
            "    vec3 RGB = colorPalette(val);"
            "    fcolor = vec4( RGB.x, RGB.y, RGB.z, alpha);"
            "    gl_Position = gl_ModelViewProjectionMatrix*v;"
            "    gl_PointSize = 4.0 * pointsize / gl_Position.w;"
            "}";

    // without shading
    const std::string vertexSourceNoShader =
            "#version 130 \n"
            "uniform float alpha;"
            "varying vec2 texture_coordinate;"
            "out vec4 fcolor;"
            "void main(void)"
            "{"
            "   vec4 v = vec4(gl_Vertex);"
            "   vec4 color = gl_Color;"
            "   fcolor = vec4( color.x, color.y, color.z, alpha);"
            "   gl_Position = gl_ModelViewProjectionMatrix*v;"
            // Passing The Texture Coordinate Of Texture Unit 0 To The Fragment Shader
            "   texture_coordinate = vec2(gl_MultiTexCoord0);"
            "}";


    // gray shading
    const std::string vertexSourceGrayShader =
            "#version 130\n"
            "uniform float alpha;"
            "uniform float zmin;"
            "uniform float deltaz;"
            "out vec4 fcolor;"
            "void main(void)"
            "{"
            "   vec4 v = vec4(gl_Vertex);"
            "   float val = (v.z-zmin) / deltaz;"
            "   if(val < 0.0) val = 0.0;"
            "   if(val > 1.0) val = 1.0;"
            "   fcolor = vec4( val, val, val, alpha);"
            "   gl_Position = gl_ModelViewProjectionMatrix*v;"
            "   gl_TexCoord[0]	=	gl_MultiTexCoord0;"
            "}";

    // EyeDomeLighting shading
    const std::string vertexSourceEyeDomeLighting =
            "#version 130\n"
            "uniform float alpha;"
            "out vec4 fcolor;"
            "void main(void)"
            "{"
            "   gl_TexCoord[0]	=	gl_MultiTexCoord0;"
            "   vec4 v = vec4(gl_Vertex);"
            "   vec4 color = gl_Color;"
//            "   color.a = alpha;"
            "   fcolor = vec4( color.x, color.y, color.z, alpha);"
            "   gl_Position = gl_ModelViewProjectionMatrix*v;"
            "}";

    const std::string vertexSourceTextureShader =
    "#version 330\n"
    "layout (location = 0) in vec3 Position;"
    "layout (location = 1) in vec2 TexCoord;"
    "uniform mat4 gWVP;"
    "out vec2 TexCoord0;"
    "void main()"
    "{"
    "    gl_Position = gWVP * vec4(Position, 1.0);"
    "    TexCoord0 = TexCoord;"
    "};";

    std::string vertexSource;
    switch(_shader)
    {
    case NoShader:
        vertexSource = vertexSourceNoShader;
        break;

    case GrayShader:
        vertexSource = vertexSourceGrayShader;
        break;

    case Standard:
        vertexSource = vertexSourceStandardBegin;
        vertexSource += ShaderColor::shaderSource(_palette);
        vertexSource += vertexSourceStandardEnd;
        break;

    case EyeDomeLighting:
        vertexSource = vertexSourceEyeDomeLighting;
        break;

    case TextureShader:
        vertexSource = vertexSourceTextureShader;
        break;
    }


    osg::Shader* vShader = new osg::Shader( osg::Shader::VERTEX, vertexSource );

    return vShader;
}

osg::Shader *ShaderBuilder::fragmentShader(const ShaderBuilder::ShaderType _shader)
{
    const std::string fragmentSourceStandard =
            "#version 130 \n"
            "uniform bool hasmesh;"
            "uniform bool lighton;"

            "in vec4 fcolor;"
            "in vec3 vertex_light_position;"
            "in vec3 vertex_light_half_vector;"
            "in vec3 vertex_normal;"

            "void main() {"
            "   vec4 color = fcolor;"
            "   if(!hasmesh || lighton)"
            "   {"
            "      color = fcolor;"
            "   }"
            "   else"
            "   {"
            // Calculate the ambient term
            "      vec4 ambient_color = gl_FrontMaterial.ambient * gl_LightSource[0].ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;"

            // Calculate the diffuse term
            "      vec4 diffuse_color = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;"

            // Calculate the specular value
            "      vec4 specular_color = gl_FrontMaterial.specular * gl_LightSource[0].specular * pow(max(dot(vertex_normal, vertex_light_half_vector), 0.0) , gl_FrontMaterial.shininess);"

            // Set the diffuse value (darkness). This is done with a dot product between the normal and the light
            // and the maths behind it is explained in the maths section of the site.
            "      float diffuse_value = max(dot(vertex_normal, vertex_light_position), 0.0);"

            // Set the output color of our current pixel
            "      vec4 material_color = ambient_color + diffuse_color * diffuse_value + specular_color;"

            "      color.r = material_color.r * fcolor.r;"
            "      color.g = material_color.g * fcolor.g;"
            "      color.b = material_color.b * fcolor.b;"
            "   }"
            "   gl_FragColor = color;"
            "}";

    // without shading
    const std::string fragmentSourceNoShader =
            "in vec4 fcolor;"
            "varying vec2 texture_coordinate;"
            "uniform sampler2D my_color_texture;"
            "void main()"
            "{"
//            "   gl_FragColor = fcolor;"
            "   gl_FragColor= texture2D(my_color_texture, texture_coordinate);"
            "}";


    const std::string fragmentSourceEyeDomeLighting =

            "uniform	sampler2D	s1_color;"
            "uniform	sampler2D	s2_depth;"
            "uniform float		Pix_scale;"		    //	(relative) pixel scale in image
            "uniform vec2		Neigh_pos_2D[8];"	//	array of neighbors (2D positions)
            "uniform float		Exp_scale;"			//	exponential scale factor (for computed AO)

            "uniform float		zmin;"					//	minimal depth in image
            "uniform float		deltaz;"				//	delta Z

            "uniform float		Sx;"
            "uniform float		Sy;"

            "uniform vec3		Light_dir;"
            /**************************************************/


            //  Obscurance (pseudo angle version)
            //	z		neighbour relative elevation
            //	dist	distance to the neighbourx
            "float obscurance(float z, float dist)"
            "{"
            "    return max(0.0, z) / dist;"
            "}"

            "float ztransform(float z_b)"
            "{"

            "   float ZM = zmin + deltaz;"
            //'1/z' depth-buffer transformation correction
            "   float z_n = 2.0 * z_b - 1.0;"
            "   z_b = 2.0 * zmin / (ZM + zmin - z_n * (deltaz));"
            "   z_b = z_b * ZM / (deltaz);"

            "    return clamp(1.0 - z_b, 0.0, 1.0);"
            "}"

            "float computeObscurance(float depth, float scale)"
            "{"
            // Light-plane point
            "   vec4 P = vec4( Light_dir.xyz , -dot(Light_dir.xyz,vec3(0.0,0.0,depth)) );"

            "  float sum = 0.0;"
            " float Zoom = 1.0;"

            // contribution of each neighbor
            " for(int c=0; c<8; c++)"
            "{"
            "   vec2 N_rel_pos = scale * Zoom / vec2(Sx,Sy) * Neigh_pos_2D[c];"	//neighbor relative position
            "  vec2 N_abs_pos = gl_TexCoord[0].st + N_rel_pos;"					//neighbor absolute position

            //version with background shading
            " float Zn = ztransform( texture2D(s2_depth,N_abs_pos).r );"		//depth of the real neighbor
            "float Znp = dot( vec4(N_rel_pos, Zn, 1.0) , P );"				//depth of the in-plane neighbor

            "sum += obscurance( Znp, scale );"
            "}"

            "return	sum;"
            "}"

            "void main (void)"
            "{"
            //ambient occlusion
            "   vec3 rgb = texture2D(s1_color,gl_TexCoord[0].st).rgb;"
            "   float depth = ztransform( texture2D(s2_depth,gl_TexCoord[0].st).r );"

            "    if( depth > 0.01 )"
            "   {"
            "       float f = computeObscurance(depth, Pix_scale);"
            "       f = exp(-Exp_scale*f);"

            "       gl_FragData[0]	=	vec4(f*rgb, 1.0);"
            "   }"
            "   else"
            "   {"
            "       gl_FragData[0]	=	vec4(rgb, 1.0);"
            "   }"
            "}";

    const std::string fragmentSourceTextureShader =
    "in vec2 TexCoord0;"
//    "out vec4 FragColor;"
    "uniform sampler2D gSampler;"
    "void main()"
    "{"
    "    gl_FragColor = texture2D(gSampler, TexCoord0.st);"
    "};";


    std::string fragmentSource;
    switch(_shader)
    {
    case NoShader:
        fragmentSource = fragmentSourceNoShader;
        break;

    case GrayShader:
        fragmentSource = fragmentSourceNoShader;
        break;

    case Standard:
        fragmentSource = fragmentSourceStandard;
        break;

    case EyeDomeLighting:
        fragmentSource = fragmentSourceEyeDomeLighting;
        break;

    case TextureShader:
        fragmentSource = fragmentSourceTextureShader;
        break;
    }

    osg::Shader* fShader = new osg::Shader( osg::Shader::FRAGMENT, fragmentSource );

    return fShader;
}
