#ifndef __SHADER_H__
#define __SHADER_H__

#include <bits/stdc++.h>
#include "vec4.h"


class Shader
{
public:
    virtual std::tuple<vec4, std::vector<float>> vertexShader(const std::vector<float> &attributes) const = 0;
    virtual vec4 fragmentShader(const vec4& pos, const std::vector<float> &varying) const = 0;
    virtual int attribSize() const = 0;
};

#endif