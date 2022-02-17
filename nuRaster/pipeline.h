#ifndef __PIPELINE_H__
#define __PIPELINE_H__

#include <bits/stdc++.h>
#include "texture.h"

class Pipeline
{
public:
    void drawcall(const std::vector<float> &vbo, Texture &render_target)
    {
        int width = render_target.width();  
        int height = render_target.height();
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                render_target.pixel(x, y) = vec3(0.1, 0.5, 0.7);
            }
        }
    }
};

#endif