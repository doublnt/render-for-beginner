#ifndef __GEOMETRY_EX_H__
#define __GEOMETRY_EX_H__

#include "geometry.h"

// 求重心
inline Vec3f barycentric(Vec2i *pts, Vec2i p) {
  Vec3f u = Vec3f(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - p.x) ^
            Vec3f(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - p.y);

  if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
  return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

#endif  // __GEOMETRY_EX_H__