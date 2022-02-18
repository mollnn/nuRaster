#ifndef __SHADER_NORMAL_H__
#define __SHADER_NORMAL_H__

#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "shader.h"

struct ShaderNormalPayloadA
{
    vec3 pos;
    vec3 normal;
    vec3 texcoord;
};

struct ShaderNormalPayloadV
{
    vec3 normal;
};

struct ShaderNormalUniforms
{
    mat4 mvp;
};

class ShaderNormal : public Shader
{
public:
    ShaderNormalUniforms uniforms;

    std::tuple<vec4, ShaderNormalPayloadV> _vertexShader(const ShaderNormalPayloadA &attributes) const;

    vec4 _fragmentShader(const vec4 &pos, const ShaderNormalPayloadV &varying) const;

    virtual std::tuple<vec4, std::vector<float>> vertexShader(const std::vector<float> &attributes_raw) const override;

    virtual vec4 fragmentShader(const vec4 &pos, const std::vector<float> &varying) const override;

    virtual int attribSize() const override;
};


#endif