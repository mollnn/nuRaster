#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <bits/stdc++.h>
#include "camera.h"
#include "triangle.h"
#include <QImage>

class Renderer
{
public:
    void render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, const vec3& light_pos, const vec3& light_int,  int img_width, int img_height);

private:
};

#endif
