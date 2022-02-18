#ifndef __SHADER_LAMBERT_H__
#define __SHADER_LAMBERT_H__

#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "shader.h"
#include "texture.h"

struct ShaderLambertPayloadA
{
    vec3 pos;
    vec3 normal;
    vec3 texcoord;
};

struct ShaderLambertPayloadV
{
    vec3 world_space_pos;
    vec3 normal;
    vec3 texcoord;
};

struct ShaderLambertUniforms
{
    mat4 mvp;
    vec3 kd;
    bool use_tex_kd;
    const Texture* tex_kd;
    vec3 light_pos;
    vec3 light_intensity;
};

class ShaderLambert : public Shader
{
public:
    ShaderLambertUniforms uniforms;

    std::tuple<vec4, ShaderLambertPayloadV> _vertexShader(const ShaderLambertPayloadA &attributes) const;

    vec4 _fragmentShader(const vec4 &pos, const ShaderLambertPayloadV &varying) const;

    virtual std::tuple<vec4, std::vector<float>> vertexShader(const std::vector<float> &attributes_raw) const override;

    virtual vec4 fragmentShader(const vec4 &pos, const std::vector<float> &varying) const override;

    virtual int attribSize() const override;
};


#endif