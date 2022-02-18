#include "renderer.h"
#include "texture.h"
#include "pipeline.h"
#include <QTime>
#include <QDebug>
#include "shader.h"
#include "shadernormal.h"
#include "shaderlambert.h"
#include "shaderblinnphong.h"
#include "matblinnphong.h"
#include "mat4.h"

void Renderer::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, const vec3 &light_pos, const vec3 &light_int, int img_width, int img_height)
{
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    std::cout << "Rendering... " << std::endl;

    Texture color_buffer(img_width, img_height);
    Texture z_buffer(img_width, img_height);

    Pipeline pipeline;

    float clip_near = -1.0f;
    float clip_far = -10000.0f;
    float clip_up = -clip_near * tan(camera.fov_h * 3.14159 / 180 / 2);
    float clip_right = clip_up * camera.aspect;

    mat4 view_translate = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {-camera.pos, 1}};
    mat4 view_rotate = {{camera.gaze.cross(camera.up), 0}, {camera.up, 0}, {-camera.gaze, 0}, {0, 0, 0, 1}};
    view_rotate = view_rotate.T();
    mat4 ortho = {{1.0f / clip_right, 0.0f, 0.0f, 0.0f},
                  {0.0f, 1.0f / clip_up, 0.0f, 0.0f},
                  {0.0f, 0.0f, 2.0f / (clip_near - clip_far), 0.0f},
                  {0.0f, 0.0f, (clip_near + clip_far) / (clip_far - clip_near), 1.0f}};
    mat4 persp2ortho = {{clip_near, 0, 0, 0}, {0, clip_near, 0, 0}, {0, 0, clip_near + clip_far, 1}, {0, 0, -clip_near * clip_far, 0}};
    mat4 mvp = ortho * persp2ortho * view_rotate * view_translate;

    int width = color_buffer.width();
    int height = color_buffer.height();

    // Clear
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            color_buffer.pixel(x, y) = vec3(0.1, 0.5, 0.7);
            z_buffer.pixel(x, y) = 1e18f;
        }
    }

    // Classify primitives by material, build shader for each
    std::map<const Material *, int> material_map;
    for (const auto &triangle : triangles)
    {
        material_map[triangle.mat]++;
    }
    int material_count = 0;
    std::vector<const Material *> material_list;
    for (auto &[key, value] : material_map)
    {
        value = material_count++;
        material_list.push_back(key);
    }
    std::vector<std::vector<const Triangle *>> primitive_partitions(material_count);
    for (const auto &triangle : triangles)
    {
        primitive_partitions[material_map[triangle.mat]].push_back(&triangle);
    }

    for (int material_id = 0; material_id < material_count; material_id++)
    {
        const MatBlinnPhong *material = dynamic_cast<const MatBlinnPhong *>(material_list[material_id]);

        ShaderBlinnPhong shader;

        shader.uniforms.mvp = mvp;

        if (material != nullptr)
        {
            shader.uniforms.ka = material->Ka_;
            shader.uniforms.use_tex_ka = material->usetex_Ka_;
            shader.uniforms.tex_ka = &material->map_Ka_;

            shader.uniforms.kd = material->Kd_;
            shader.uniforms.use_tex_kd = material->usetex_Kd_;
            shader.uniforms.tex_kd = &material->map_Kd_;

            shader.uniforms.ks = material->Ks_;
            shader.uniforms.ns = material->Ns_;
            shader.uniforms.use_tex_ks = material->usetex_Ks_;
            shader.uniforms.tex_ks = &material->map_Ks_;
        }
        else
        {
            shader.uniforms.ka = 0.0f;
            shader.uniforms.use_tex_ka = false;

            shader.uniforms.kd = 0.7f;
            shader.uniforms.use_tex_kd = false;

            shader.uniforms.ks = 0.0f;
            shader.uniforms.ns = 1.0f;
            shader.uniforms.use_tex_ks = false;
        }

        shader.uniforms.light_pos = light_pos;
        shader.uniforms.light_intensity = light_int;

        std::vector<float> vbo;
        for (const auto &triangle : primitive_partitions[material_id])
        {
            for (int i = 0; i < 3; i++)
            {
                for (int j = 0; j < 3; j++)
                    vbo.push_back(triangle->p[i][j]);
                for (int j = 0; j < 3; j++)
                    vbo.push_back(triangle->n[i][j]);
                for (int j = 0; j < 3; j++)
                    vbo.push_back(triangle->t[i][j]);
            }
        }
        pipeline.drawcall(vbo, shader, color_buffer, z_buffer);
    }

    for (int i = 0; i < img_height; i++)
    {
        for (int j = 0; j < img_width; j++)
        {
            vec3 c = max(0.0f, min(1.0f, color_buffer.pixel(j, i))) * 255.0f;

            img.setPixelColor(j, i, QColor(c[0], c[1], c[2]));
        }
    }

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
}
