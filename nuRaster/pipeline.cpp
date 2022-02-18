#include "pipeline.h"

void Pipeline::drawcall(const std::vector<float> &vbo, const Shader &shader, Texture &color_buffer, Texture &z_buffer)
{
    int width = color_buffer.width();
    int height = color_buffer.height();
    int attrib_size = shader.attribSize();
    int vertices_count = vbo.size() / (attrib_size / 4);
    int primitive_count = vertices_count / 3;

    std::vector<std::tuple<vec4, std::vector<float>>> vertex_payload;

    // Vertex Shader Stage
    for (int i = 0; i < vertices_count; i++)
    {
        std::vector<float> attributes(vbo.begin() + i * (attrib_size / 4), vbo.begin() + (i + 1) * (attrib_size / 4));
        auto [pos, varyings] = shader.vertexShader(attributes);
        vertex_payload.push_back({pos, varyings});
    }

    // Rasterize and Interpolate
    for (int i = 0; i < primitive_count; i++)
    {
        auto [p0, v0] = vertex_payload[i * 3 + 0];
        auto [p1, v1] = vertex_payload[i * 3 + 1];
        auto [p2, v2] = vertex_payload[i * 3 + 2];

        vec3 sp0((p0[0] / p0[3] * 0.5f + 0.5f) * width, (p0[1] / p0[3] * 0.5f + 0.5f) * height, 0.0f);
        vec3 sp1((p1[0] / p1[3] * 0.5f + 0.5f) * width, (p1[1] / p1[3] * 0.5f + 0.5f) * height, 0.0f);
        vec3 sp2((p2[0] / p2[3] * 0.5f + 0.5f) * width, (p2[1] / p2[3] * 0.5f + 0.5f) * height, 0.0f);

        float inv_w0 = 1.0f / p0[3];
        float inv_w1 = 1.0f / p1[3];
        float inv_w2 = 1.0f / p2[3];

        int y_min = height - 1, y_max = 0, x_min = width - 1, x_max = 0;

        x_min = std::min(x_min, int(sp0[0]));
        x_min = std::min(x_min, int(sp1[0]));
        x_min = std::min(x_min, int(sp2[0]));

        x_max = std::max(x_max, int(sp0[0]) + 1);
        x_max = std::max(x_max, int(sp1[0]) + 1);
        x_max = std::max(x_max, int(sp2[0]) + 1);

        y_min = std::min(y_min, int(sp0[1]));
        y_min = std::min(y_min, int(sp1[1]));
        y_min = std::min(y_min, int(sp2[1]));

        y_max = std::max(y_max, int(sp0[1]) + 1);
        y_max = std::max(y_max, int(sp1[1]) + 1);
        y_max = std::max(y_max, int(sp2[1]) + 1);

        x_min = std::max(x_min, 0);
        x_max = std::max(x_max, 0);
        x_min = std::min(x_min, width - 1);
        x_max = std::min(x_max, width - 1);

        y_min = std::max(y_min, 0);
        y_max = std::max(y_max, 0);
        y_min = std::min(y_min, height - 1);
        y_max = std::min(y_max, height - 1);

        std::swap(y_min, y_max);
        y_min = height - 1 - y_min;
        y_max = height - 1 - y_max;

        for (int y = y_min; y <= y_max; y++)
        {
            for (int x = x_min; x <= x_max; x++)
            {
                vec3 sp = vec3(x, height - y - 1, 0.0f);
                vec3 spsp0 = sp0 - sp;
                vec3 spsp1 = sp1 - sp;
                vec3 spsp2 = sp2 - sp;
                float area0 = spsp1.cross(spsp2)[2];
                float area1 = spsp2.cross(spsp0)[2];
                float area2 = spsp0.cross(spsp1)[2];
                if (area0 > 0 && area1 > 0 && area2 > 0)
                {
                    float area = area0 + area1 + area2;
                    float bc1 = area1 / area;
                    float bc2 = area2 / area;
                    float bc0 = 1.0f - bc1 - bc2;

                    float inv_w = bc0 * inv_w0 + bc1 * inv_w1 + bc2 * inv_w2;
                    vec4 ws_pos = (bc0 * inv_w0 * p0 + bc1 * inv_w1 * p1 + bc2 * inv_w2 * p2) / inv_w;
                    vec4 frag_pos = vec4(x * 1.0f / width, y * 1.0f / height, 0.0f, 0.0f);
                    std::vector<float> varying;
                    for (int j = 0; j < v0.size(); j++)
                    {
                        varying.push_back((bc0 * inv_w0 * v0[j] + bc1 * inv_w1 * v1[j] + bc2 * inv_w2 * v2[j]) / inv_w);
                    }
                    vec4 color = shader.fragmentShader(ws_pos, varying);
                    float z = ws_pos[2];
                    // Depth test
                    if (z < z_buffer.pixel(x, y)[0])
                    {
                        z_buffer.pixel(x, y) = z;
                        color_buffer.pixel(x, y) = color.xyz();
                    }
                }
            }
        }
    }
}