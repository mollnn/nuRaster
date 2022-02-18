#include "renderer.h"
#include "texture.h"
#include "pipeline.h"
#include <QTime>
#include <QDebug>
#include "shader.h"
#include "shadernormal.h"
#include "shaderlambert.h"
#include "matblinnphong.h"
#include <QMatrix4x4>

void Renderer::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height)
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

    QMatrix4x4 mvp;
    mvp.perspective(camera.fov_h * std::max(1.0f, camera.aspect), 1, 1, 10000);
    vec3 look_at_center = camera.pos + camera.gaze * 100.0f;
    mvp.lookAt({camera.pos[0], camera.pos[1], camera.pos[2]},
               {look_at_center[0], look_at_center[1], look_at_center[2]},
               {camera.up[0], camera.up[1], camera.up[2]});

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
        if (material == nullptr)
            continue;

        ShaderLambert shader;
        shader.uniforms.mvp = {
            {mvp(0, 0), mvp(1, 0), mvp(2, 0), mvp(3, 0)},
            {mvp(0, 1), mvp(1, 1), mvp(2, 1), mvp(3, 1)},
            {mvp(0, 2), mvp(1, 2), mvp(2, 2), mvp(3, 2)},
            {mvp(0, 3), mvp(1, 3), mvp(2, 3), mvp(3, 3)}};

        shader.uniforms.kd = material->Kd_;
        shader.uniforms.use_tex_kd = material->usetex_Kd_;
        shader.uniforms.tex_kd = &material->map_Kd_;
        
        shader.uniforms.light_pos = vec3(200.0f, 500.0f, -300.0f);
        shader.uniforms.light_intensity = 1000000.0f;

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
