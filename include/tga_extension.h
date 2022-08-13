#ifndef __TGA_EXTENSION_H__
#define __TGA_EXTENSION_H__

#include "model.h"
#include "tgaimage.h"

// 布雷森汉姆直线算法
void plot_line(int x0, int y0, int x1, int y1, TGAImage& tga_image,
               const TGAColor& color) {
  bool steep = std::abs(y1 - y0) > std::abs(x1 - x0);

  if (steep) {
    std::swap(x0, y0);
    std::swap(x1, y1);
  }

  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  int deltax = x1 - x0;
  int deltay = std::abs(y1 - y0);
  auto error = deltax / 2;

  int ystep;
  int y = y0;

  if (y0 < y1) {
    ystep = 1;
  } else {
    ystep = -1;
  }

  for (int x = x0; x < x1; ++x) {
    if (steep) {
      tga_image.set(y, x, color);
    } else {
      tga_image.set(x, y, color);
    }
    error = error - deltay;

    if (error < 0) {
      y = y + ystep;
      error = error + deltax;
    }
  }
}

void plot_face(Model* model, TGAImage& tga_image, int width, int height,
               const TGAColor& color) {
  int nfaces_count = model->nfaces();
  for (int i = 0; i < nfaces_count; ++i) {
    std::vector<int> face = model->face(i);

    for (int j = 0; j < 3; ++j) {
      Vec3f v0 = model->vert(face[j]);
      Vec3f v1 = model->vert(face[(j + 1) % 3]);

      int x0 = (v0.x + 1.0) * width / 2.0;
      int y0 = (v0.y + 1.0) * height / 2.0;

      int x1 = (v1.x + 1.0) * width / 2.0;
      int y1 = (v1.y + 1.0) * height / 2.0;

      plot_line(x0, y0, x1, y1, tga_image, color);
    }
  }

  tga_image.flip_vertically();
}

#endif  // __TGA_EXTENSION_H__