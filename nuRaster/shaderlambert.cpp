#include "shaderlambert.h"

std::tuple<vec4, ShaderLambertPayloadV> ShaderLambert::_vertexShader(const ShaderLambertPayloadA &attributes) const
{
    ShaderLambertPayloadV varying;
    vec4 pos = uniforms.mvp * vec4(attributes.pos, 1.0f);
    varying.normal = vec4(attributes.normal, 0.0f).xyz();
    varying.world_space_pos = attributes.pos;
    varying.texcoord = attributes.texcoord;
    return {pos, varying};
}

vec4 ShaderLambert::_fragmentShader(const vec4 &pos, const ShaderLambertPayloadV &varying) const
{
    vec3 light_fragpos = uniforms.light_pos - varying.world_space_pos;
    vec3 kd = uniforms.use_tex_kd ? uniforms.tex_kd->pixelUV(varying.texcoord[0], varying.texcoord[1]) : uniforms.kd;
    return vec4(kd * uniforms.light_intensity * std::max(0.0f, light_fragpos.normalized().dot(varying.normal.normalized())) / light_fragpos.norm2() / 3.14159f, 1.0f);
}

std::tuple<vec4, std::vector<float>> ShaderLambert::vertexShader(const std::vector<float> &attributes_raw) const
{
    ShaderLambertPayloadA attributes = *(ShaderLambertPayloadA *)&*attributes_raw.begin();
    auto [pos, varying] = _vertexShader(attributes);
    float *varying_raw = (float *)&varying;
    return {pos, std::vector<float>(varying_raw, varying_raw + sizeof(varying) / sizeof(float))};
}

vec4 ShaderLambert::fragmentShader(const vec4 &pos, const std::vector<float> &varying) const
{
    ShaderLambertPayloadV attributes = *(ShaderLambertPayloadV *)&*varying.begin();
    return _fragmentShader(pos, attributes);
}

int ShaderLambert::attribSize() const
{
    return sizeof(ShaderLambertPayloadA);
}