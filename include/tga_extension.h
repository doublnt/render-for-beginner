#ifndef __TGA_EXTENSION_H__
#define __TGA_EXTENSION_H__

#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor custom_pink = TGAColor(238, 100, 40, 255);
const TGAColor custom_green = TGAColor(58, 255, 78, 255);

// 布雷森汉姆直线算法
void line(int x0, int y0, int x1, int y1, TGAImage& tga_image,
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

void line(const Vec2i& t0, const Vec2i& t1, TGAImage& tga_image,
               const TGAColor& color) {
  line(t0.x, t0.y, t1.x, t1.y, tga_image, color);
}

void face(Model* model, TGAImage& tga_image, int width, int height,
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

      line(x0, y0, x1, y1, tga_image, color);
    }
  }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& tga_image,
                   const TGAColor& color) {
  if (t0.y == t1.y && t0.y == t2.y) {
    return;
  }

  if (t0.y > t1.y) {
    std::swap(t0, t1);
  }

  if (t0.y > t2.y) {
    std::swap(t0, t2);
  }

  if (t1.y > t2.y) {
    std::swap(t1, t2);
  }

  int total_height = t2.y - t0.y;

  for (int i = 0; i < total_height; ++i) {
    bool second_half = i > (t1.y - t0.y) || t1.y == t0.y;
    int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;

    float alpha = static_cast<float>(i / total_height);
    float beta = static_cast<float>(i - (second_half ? t1.y - t0.y : 0)) /
                 segment_height;

    Vec2i a = t0 + (t2 - t0) * alpha;
    Vec2i b = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;

    if (a.x > b.x) {
      std::swap(a, b);
    }

    for (int j = a.x; j <= b.x; ++j) {
      tga_image.set(j, t0.y + i, color);
    }
  }
}
#endif  // __TGA_EXTENSION_H__