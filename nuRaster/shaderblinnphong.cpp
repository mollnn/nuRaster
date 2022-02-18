#include "shaderblinnphong.h"

std::tuple<vec4, ShaderBlinnPhongPayloadV> ShaderBlinnPhong::_vertexShader(const ShaderBlinnPhongPayloadA &attributes) const
{
    ShaderBlinnPhongPayloadV varying;
    vec4 pos = uniforms.mvp * vec4(attributes.pos, 1.0f);
    varying.normal = (uniforms.mvp * vec4(attributes.normal, 0.0f)).xyz();
    varying.world_space_pos = attributes.pos;
    varying.texcoord = attributes.texcoord;
    return {pos, varying};
}

vec4 ShaderBlinnPhong::_fragmentShader(const vec4 &pos, const ShaderBlinnPhongPayloadV &varying) const
{
    vec3 light_fragpos = uniforms.light_pos - varying.world_space_pos;

    vec3 ka = uniforms.use_tex_ka ? uniforms.tex_ka->pixelUV(varying.texcoord[0], varying.texcoord[1]) : uniforms.ka;
    vec3 ambient = ka * uniforms.light_ambient;

    vec3 kd = uniforms.use_tex_kd ? uniforms.tex_kd->pixelUV(varying.texcoord[0], varying.texcoord[1]) : uniforms.kd;
    vec3 diffuse = kd * uniforms.light_intensity * std::max(0.0f, light_fragpos.normalized().dot(varying.normal.normalized())) / light_fragpos.norm2() / 3.14159f;

    vec3 ks = uniforms.use_tex_ks ? uniforms.tex_ks->pixelUV(varying.texcoord[0], varying.texcoord[1]) : uniforms.ks;
    float cs = (2.0f + uniforms.ns) / (4.0f * 3.14159f * (2.0f - pow(2.0f, -0.5f * uniforms.ns)));
    vec3 specular = ks * uniforms.light_intensity * std::max(0.0f, light_fragpos.normalized().dot(varying.normal.normalized())) / light_fragpos.norm2() * cs;

    return ambient + diffuse + specular;
}

std::tuple<vec4, std::vector<float>> ShaderBlinnPhong::vertexShader(const std::vector<float> &attributes_raw) const
{
    ShaderBlinnPhongPayloadA attributes = *(ShaderBlinnPhongPayloadA *)&*attributes_raw.begin();
    auto [pos, varying] = _vertexShader(attributes);
    float *varying_raw = (float *)&varying;
    return {pos, std::vector<float>(varying_raw, varying_raw + sizeof(varying) / sizeof(float))};
}

vec4 ShaderBlinnPhong::fragmentShader(const vec4 &pos, const std::vector<float> &varying) const
{
    ShaderBlinnPhongPayloadV attributes = *(ShaderBlinnPhongPayloadV *)&*varying.begin();
    return _fragmentShader(pos, attributes);
}

int ShaderBlinnPhong::attribSize() const
{
    return sizeof(ShaderBlinnPhongPayloadA);
}