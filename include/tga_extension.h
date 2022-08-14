#ifndef __TGA_EXTENSION_H__
#define __TGA_EXTENSION_H__

#include "model.h"
#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor custom_pink = TGAColor(238, 100, 40, 255);
const TGAColor custom_green = TGAColor(58, 255, 78, 255);

// 布雷森汉姆直线算法
// 两个点 (x0, y0) (x1, y1) 绘制直线
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

// 给定两个二维向量，可以认为是两个点 vec0 ,vec1 绘制直线
void line(const Vec2i& vec0, const Vec2i& vec1, TGAImage& tga_image,
          const TGAColor& color) {
  line(vec0.x, vec0.y, vec1.x, vec1.y, tga_image, color);
}

// 给定三个点，绘制三角形 边框
void triangle(Vec2i vec0, Vec2i vec1, Vec2i vec2, TGAImage& tga_image,
              const TGAColor& color, bool is_filled) {
  // 若 veco.y = vec1.y = vec2.y 说明三个点在一条水平线上，
  // 直接 return
  if (vec0.y == vec1.y == vec2.y) {
    return;
  }

  // 调整三个点的 在 y 轴上的顺序，确保 vec0 在最下，
  // vec2 在最上

  if (vec0.y > vec1.y) {
    std::swap(vec1, vec0);
  }

  if (vec0.y > vec2.y) {
    std::swap(vec2, vec0);
  }

  if (vec1.y > vec2.y) {
    std::swap(vec1, vec2);
  }

  // 使用连线的方式绘制
  // line(vec1, vec0, tga_image, color);
  // line(vec0, vec2, tga_image, color);
  // line(vec2, vec1, tga_image, color);

  if (is_filled) {
    auto total_height = vec2.y - vec0.y;

    // 需要实现的是找出三角形中所有的点，绘制起来
    // 实际上其实就是填充两点这条线段中的所有点

    for (int y = vec0.y; y <= vec1.y; ++y) {
      int segment_height = vec1.y - vec0.y + 1;

      float alpha = static_cast<float>(y - vec0.y) / total_height;
      float beta = static_cast<float>(y - vec0.y) / segment_height;

      Vec2i start = vec0 + (vec2 - vec0) * alpha;
      Vec2i end = vec0 + (vec1 - vec0) * beta;

      tga_image.set(start.x, y, red);
      tga_image.set(end.x, y, custom_green);
    }
  }
}

#endif  // __TGA_EXTENSION_H__