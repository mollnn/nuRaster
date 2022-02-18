#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <bits/stdc++.h>
#include "texture.h"
#include "shader.h"
#include <QDebug>

class Pipeline
{
public:
    void drawcall(const std::vector<float> &vbo, const Shader &shader, Texture &color_buffer, Texture &z_buffer);
};

#endif