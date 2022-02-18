#ifndef __SHADER_BLINN_PHONG_H__
#define __SHADER_BLINN_PHONG_H__

#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "shader.h"
#include "texture.h"

struct ShaderBlinnPhongPayloadA
{
    vec3 pos;
    vec3 normal;
    vec3 texcoord;
};

struct ShaderBlinnPhongPayloadV
{
    vec3 world_space_pos;
    vec3 normal;
    vec3 texcoord;
};

struct ShaderBlinnPhongUniforms
{
    mat4 mvp;

    vec3 ka;
    bool use_tex_ka;
    const Texture *tex_ka;

    vec3 kd;
    bool use_tex_kd;
    const Texture *tex_kd;

    vec3 ks;
    float ns;
    bool use_tex_ks;
    const Texture *tex_ks;

    vec3 light_pos;
    vec3 light_intensity;
    vec3 light_ambient;
};

class ShaderBlinnPhong : public Shader
{
public:
    ShaderBlinnPhongUniforms uniforms;

    std::tuple<vec4, ShaderBlinnPhongPayloadV> _vertexShader(const ShaderBlinnPhongPayloadA &attributes) const;

    vec4 _fragmentShader(const vec4 &pos, const ShaderBlinnPhongPayloadV &varying) const;

    virtual std::tuple<vec4, std::vector<float>> vertexShader(const std::vector<float> &attributes_raw) const override;

    virtual vec4 fragmentShader(const vec4 &pos, const std::vector<float> &varying) const override;

    virtual int attribSize() const override;
};

#endif