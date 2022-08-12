#ifndef TGA_EXTENSION
#define TGA_EXTENSION

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

#endif