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
void bresenham_line(int x0, int y0, int x1, int y1, TGAImage& tga_image,
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
void bresenham_line(const Vec2i& vec0, const Vec2i& vec1, TGAImage& tga_image,
                    const TGAColor& color) {
  bresenham_line(vec0.x, vec0.y, vec1.x, vec1.y, tga_image, color);
}

template <class T>
// 使用 向量的叉乘来判断 一个像素点是否在三角形内部
bool inside_triangle(const Vec2<T>& src_point, const Vec2<T>& vec0,
                     const Vec2<T>& vec1, const Vec2<T>& vec2) {
  auto res0 = (src_point - vec0) * (vec2 - vec0);
  auto res1 = (src_point - vec2) * (vec1 - vec2);
  auto res2 = (src_point - vec1) * (vec0 - vec1);

  if (vec1.x > vec0.x) {
    res0 = (src_point - vec0) * (vec1 - vec0);
    res1 = (src_point - vec1) * (vec2 - vec1);
    res2 = (src_point - vec2) * (vec0 - vec2);
  }

  if ((res0 >= 0 && res1 >= 0 && res2 >= 0) ||
      (res0 <= 0 && res1 <= 0 && res2 <= 0)) {
    return true;
  }

  return false;
}

std::vector<Vec2f> get_sample_list(const Vec2f& vec, int ratio_count) {
  std::vector<Vec2f> vec_list;

  float offset = 1.f / ratio_count;

  int index = 0;
  while (index++ < std::sqrt(ratio_count)) {
    vec_list.emplace_back(Vec2f(vec.x + offset, vec.y + offset));
    vec_list.emplace_back(Vec2f(vec.x + offset, vec.y - offset));
    vec_list.emplace_back(Vec2f(vec.x - offset, vec.y - offset));
    vec_list.emplace_back(Vec2f(vec.x - offset, vec.y + offset));

    offset *= 2;
  }

  return vec_list;
}

// MSAA 算法进行 抗锯齿
bool inside_triangle_return_prox(const Vec2f& src_point, const Vec2f& vec0,
                                 const Vec2f& vec1, const Vec2f& vec2,
                                 float& prox, int ratio_count) {
  // 分成 2*2 的采样点。
  auto list = get_sample_list(src_point, ratio_count);
  int appro_count = 0;

  for (auto& li : list) {
    if (inside_triangle<float>(li, vec0, vec1, vec2)) {
      ++appro_count;
    }
  }
  if (appro_count == 0) {
    return false;
  }

  prox = static_cast<float>(appro_count) / ratio_count;

  return true;
}

// 通过 Sampling 采样的方法来填充一个三角形，
// 给定三个点。
// 提供一个 contains 函数，判断这个点是否在三角形内
// 如果在的话，就绘制
void triangle(Vec2i& vec0, Vec2i& vec1, Vec2i& vec2, TGAImage& tga_image,
              const TGAColor& color) {
  // vec0 vec1 vec2 从下到上排序好。
  if (vec0.y > vec1.y) {
    std::swap(vec0, vec1);
  }

  if (vec0.y > vec2.y) {
    std::swap(vec0, vec2);
  }

  if (vec1.y > vec2.y) {
    std::swap(vec1, vec2);
  }

  // 有一个愚蠢的办法， 就是把所有的 TgaImage width
  // 和 Height 进去遍历
  int width = tga_image.width();
  int height = tga_image.height();

  // 或者就是算出包围这个三角形的一个矩形
  // 相当于 算出 Bounding Box
  auto left_x = vec0.x;
  if (vec1.x < left_x) {
    left_x = vec1.x;
  }
  if (vec2.x < left_x) {
    left_x = vec2.x;
  }

  auto right_x = vec0.x;
  if (vec1.x > right_x) {
    right_x = vec1.x;
  }
  if (vec2.x > right_x) {
    right_x = vec2.x;
  }

  TGAColor rgba_color(color);

  Vec2f vec0_f(vec0.x, vec0.y);
  Vec2f vec1_f(vec1.x, vec1.y);
  Vec2f vec2_f(vec2.x, vec2.y);

  for (int i = left_x; i <= right_x; ++i) {
    for (int j = vec0.y; j <= vec2.y; ++j) {
      Vec2f curr_point(i, j);
      float approx = 1.f;
      if (inside_triangle_return_prox(curr_point, vec0_f, vec1_f, vec2_f,
                                      approx, 16)) {
        rgba_color.bgra[3] = static_cast<int>(approx * 255);
        tga_image.set(i, j, rgba_color);
      }
    }
  }
}

#endif  // __TGA_EXTENSION_H__