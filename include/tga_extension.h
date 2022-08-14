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

void triangle(Vec2i* pts, TGAImage& image, const TGAColor& color) {
  Vec2i bboxmin(image.width() - 1, image.height() - 1);
  Vec2i bboxmax(0, 0);

  Vec2i clamp(image.width() - 1, image.height() - 1);

  for (int i = 0; i < 3; ++i) {
    bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
    bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

    bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
    bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
  }

  Vec2i p;

  for (p.x = bboxmin.x; p.x <= bboxmax.x; ++p.x) {
    for (p.y = bboxmin.y; p.y <= bboxmax.y; ++p.y) {
      Vec3f bc_screen = barycentric(pts, p);
      if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
        continue;
      }
      image.set(p.x, p.y, color);
    }
  }
}

void face(Model* model, TGAImage& tga_image, int width, int height,
          const TGAColor& color) {
  int nfaces_count = model->nfaces();
  Vec3f light_dir(0, 0, -1);
  for (int i = 0; i < nfaces_count; ++i) {
    std::vector<int> face = model->face(i);

    Vec2i screen_coords[3];
    Vec3f world_coords[3];

    for (int j = 0; j < 3; ++j) {
      Vec3f v = model->vert(face[j]);
      screen_coords[j] =
          Vec2i((v.x + 1.0) * width / 2.0, (v.y + 1.0) * height / 2.0);
      world_coords[j] = v;
    }

    Vec3f n = (world_coords[2] - world_coords[0]) ^
              (world_coords[1] - world_coords[0]);
    n.normalize();

    float intensity = n * light_dir;

    if (intensity > 0) {
      triangle(screen_coords[0], screen_coords[1], screen_coords[2], tga_image,
               TGAColor(intensity * 255, intensity * 255, intensity * 255));
    }
  }
}

#endif  // __TGA_EXTENSION_H__