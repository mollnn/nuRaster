#include "shadernormal.h"

std::tuple<vec4, ShaderNormalPayloadV> ShaderNormal::_vertexShader(const ShaderNormalPayloadA &attributes) const
{
    ShaderNormalPayloadV varying;
    vec4 pos = uniforms.mvp * vec4(attributes.pos, 1.0f);
    varying.normal = (uniforms.mvp * vec4(attributes.normal, 0.0f)).xyz();
    return {pos, varying};
}

vec4 ShaderNormal::_fragmentShader(const vec4 &pos, const ShaderNormalPayloadV &varying) const
{
    return varying.normal.normalized() * 0.5f + 0.5f;
}

std::tuple<vec4, std::vector<float>> ShaderNormal::vertexShader(const std::vector<float> &attributes_raw) const
{
    ShaderNormalPayloadA attributes = *(ShaderNormalPayloadA *)&*attributes_raw.begin();
    auto [pos, varying] = _vertexShader(attributes);
    float *varying_raw = (float *)&varying;
    return {pos, std::vector<float>(varying_raw, varying_raw + sizeof(varying) / sizeof(float))};
}

vec4 ShaderNormal::fragmentShader(const vec4 &pos, const std::vector<float> &varying) const
{
    ShaderNormalPayloadV attributes = *(ShaderNormalPayloadV *)&*varying.begin();
    return _fragmentShader(pos, attributes);
}

int ShaderNormal::attribSize() const
{
    return sizeof(ShaderNormalPayloadA);
}