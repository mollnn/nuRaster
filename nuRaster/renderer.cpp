#include "renderer.h"
#include "texture.h"
#include "pipeline.h"
#include <QTime>
#include <QDebug>

void Renderer::render(const Camera &camera, const std::vector<Triangle> &triangles, QImage &img, int SPP, int img_width, int img_height)
{
    img = QImage(QSize(img_width, img_height), QImage::Format_RGB888);
    img.fill(Qt::black);

    QTime time;
    time.start();
    auto time_last = time.elapsed();

    std::cout << "Rendering... " << std::endl;

    Texture target(img_width, img_height);

    Pipeline pipeline;
    pipeline.drawcall({}, target);

    for (int i = 0; i < img_height; i++)
    {
        for (int j = 0; j < img_width; j++)
        {
            vec3 c = target.pixel(j, i) * 255.0f;
            img.setPixelColor(j, i, QColor(c[0], c[1], c[2]));
        }
    }

    std::cout << std::fixed << std::setprecision(2) << "Rendering... " << 100.0 << "%"
              << "   " << time.elapsed() * 0.001 << " secs used" << std::endl;
}
