#include <Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "../include/assignment.h"
#include "Triangle.hpp"
#include "rasterizer.hpp"

Eigen::Matrix4f get_rotation(Vector3f axis, float angle) {
  Eigen::Matrix4f rotation_across_axis = Eigen::Matrix4f::Identity();

  // 先旋转到 角度 到 z 轴， 角度 为 axis 和 z 轴的 点乘 求出 余弦夹角
  // 然后再 旋转 angle 角度，然后再 旋转到 axis 轴
  auto angle_cal = angle / 180 * MY_PI;

  // 罗德里德斯 旋转公式
  Eigen::Vector4f axis_homogenious;
  axis_homogenious << axis[0], axis[1], axis[2], 0;

  Eigen::Matrix4f n = Eigen::Matrix4f::Identity();
  n << 0, -axis[2], axis[1], 0, axis[2], 0, -axis[0], 0, -axis[1], axis[0], 0,
      0, 0, 0, 0, 1;

  rotation_across_axis = std::cos(angle_cal) * rotation_across_axis +
                         (1 - std::cos(angle_cal)) * axis_homogenious *
                             axis_homogenious.transpose() +
                         std::sin(angle_cal) * n;

  return rotation_across_axis;
}

// 视图变换
// @param eye_pos Eigen::Vector3f 可视角矩阵
Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos) {
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

  Eigen::Matrix4f translate;
  translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1, -eye_pos[2],
      0, 0, 0, 1;

  view = translate * view;

  return view;
}

int main(int argc, const char** argv) {
  float angle = 0;
  bool command_line = false;
  std::string filename = "D:/work/cpp-program/output/output1.png";

  if (argc >= 3) {
    command_line = true;
    angle = std::stof(argv[2]);  // -r by default
    if (argc == 4) {
      filename = std::string(argv[3]);
    } else
      return 0;
  }

  rst::rasterizer r(700, 700);

  Eigen::Vector3f eye_pos = {0, 0, 5};

  std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

  std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

  auto pos_id = r.load_positions(pos);
  auto ind_id = r.load_indices(ind);

  int key = 0;
  int frame_count = 0;

  if (command_line) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r.set_model(Games101::get_model_matrix(angle));
    r.set_view(get_view_matrix(eye_pos));
    r.set_projection(Games101::get_projection_matrix(45, 1, 0.1, 50));

    r.draw(pos_id, ind_id, rst::Primitive::Triangle);
    cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());

    image.convertTo(image, CV_8UC3, 1.0f);

    cv::imwrite(filename, image);

    return 0;
  }

  while (key != 27) {
    r.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r.set_model(Games101::get_model_matrix(angle));
    r.set_view(get_view_matrix(eye_pos));
    r.set_projection(Games101::get_projection_matrix(45, 1, 0.1, 50));

    r.draw(pos_id, ind_id, rst::Primitive::Triangle);

    cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::imshow("image", image);
    key = cv::waitKey(10);

    std::cout << "frame count: " << frame_count++ << '\n' << std::endl;

    if (key == 'a') {
      angle += 10;
    } else if (key == 'd') {
      angle -= 10;
    }
  }

  return 0;
}
