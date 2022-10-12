#include <Eigen>
#include <iostream>

#include "global.hpp"

class Games101 {
public:
  template <class T>
  static void transform(Eigen::Matrix<T, 3, 1> &homogenious_point) {
    // 逆时针旋转44度，再平移(1,2)
    // 齐次坐标
    // 首先是旋转矩阵的定义，齐次坐标
    float angle = -45.0 / 180.0;
    Eigen::Matrix<float, 3, 3> rotate;
    rotate << std::cosf(angle), -std::sinf(angle), 0.f, std::sinf(angle),
        std::cosf(angle), 0.f, 0.f, 0.f, 1.f;

    Eigen::Matrix<float, 3, 3> transition;
    transition << 1.f, 0.f, 1.f, 0.f, 1.f, 2.f, 0.f, 0.f, 1.f;

    homogenious_point = transition * rotate * homogenious_point;
  }

  // 模型变换，当前是 绕 z 轴旋转
  // @param rotation_angle float
  static Eigen::Matrix4f get_model_matrix(float rotation_angle) {
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    auto cal_angle = rotation_angle / 180 * MY_PI;

    model << std::cos(cal_angle), -std::sin(cal_angle), 0, 0,
        std::sin(cal_angle), std::cos(cal_angle), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

    return model;
  }

  // 获得 投影矩阵
  // @param eye_fov 可视角
  // @param aspect_ratio 长宽比
  // @param zNear z轴上的 near
  // @param zFar z轴上的 far
  static Eigen::Matrix4f get_projection_matrix(float eye_fov,
                                               float aspect_ratio, float zNear,
                                               float zFar) {
    // project_to_ortho matrix
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    projection << zNear, 0, 0, 0, 0, zNear, 0, 0, 0, 0, zNear + zFar,
        -1 * zNear * zFar, 0, 0, 1, 0;

    // 用可视角进行计算， eye_fov 求 正交
    auto t = std::tan(eye_fov / 2 / 180 * MY_PI) * std::abs(zNear);
    auto b = -t;

    auto r = t * aspect_ratio;
    auto l = -r;

    // ortho_matrix
    Eigen::Matrix4f scale_matrix = Eigen::Matrix4f::Identity();
    scale_matrix << 2 / (r - l), 0, 0, 0, 0, 2 / (t - b), 0, 0, 0, 0,
        2 / (zNear - zFar), 0, 0, 0, 0, 1;

    // 平移变换矩阵
    Eigen::Matrix4f translation_matrix = Eigen::Matrix4f::Identity();
    translation_matrix << 1, 0, 0, -1 * (r + l) / 2, 0, 1, 0, -1 * (t + b) / 2,
        0, 0, 1, -1 * (zFar + zNear) / 2, 0, 0, 0, 1;

    // 这边 再进行 z 轴翻转 180 的原因是 z轴给定的值 默认是 正的，其实
    // 应该是 负数
    // projection matrix equal M(ortho) * M(projection_to_ortho)
    return get_model_matrix(180) * scale_matrix * translation_matrix *
           projection;
  }

  static bool inside_triangle(const Vector3f *_v, const Vector3f &point) {
    Vector3f v[3];
    for (int i = 0; i < 3; i++) {
      v[i] = {_v[i].x(), _v[i].y(), 1.0};
    }
    Vector3f f0, f1, f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    Vector3f p(point[0], point[1], 1.);
    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) &&
        (p.dot(f2) * f2.dot(v[1]) > 0))
      return true;
    return false;
  }
};