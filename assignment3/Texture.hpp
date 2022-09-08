//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include <Eigen>
#include <opencv2/opencv.hpp>

#include "../include/global.hpp"

class Texture {
 private:
  cv::Mat image_data;

 public:
  Texture(const std::string& name) {
    image_data = cv::imread(name);
    cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
    width = image_data.cols;
    height = image_data.rows;
  }

  int width, height;

  Eigen::Vector3f getColor(float u, float v) {
    auto u_img = u * width;
    auto v_img = (1 - v) * height;
    auto color = image_data.at<cv::Vec3b>(v_img, u_img);
    return Eigen::Vector3f(color[0], color[1], color[2]);
  }

  float lerp(float x, float v0, float v1) { return v0 + x * (v1 - v0); }

  // double linear interpolate
  Eigen::Vector3f getColorBilinear(float u, float v) {
    auto u_img = u * width;
    auto v_img = (1 - v) * height;

    auto point_u = std::floor(u_img);
    auto point_v = std::floor(v_img);

    // get surrord four point
    auto left_point_u = point_u - 1;
    auto left_point_v = point_v;

    auto bottom_point_u = point_u;
    auto bottom_point_v = point_v - 1;

    auto cross_point_u = point_u - 1;
    auto cross_point_v = point_v - 1;

    auto s = std::abs(u_img - cross_point_u);
    auto t = std::abs(v_img - cross_point_v);

    auto u0 = lerp(s, point_u, left_point_u);
    auto u1 = lerp(s, bottom_point_u, cross_point_u);

    auto final_u = lerp(t, u0, u1);

    auto v0 = lerp(s, point_v, left_point_v);
    auto v1 = lerp(s, bottom_point_v, cross_point_v);

    auto final_v = lerp(t, v0, v1);

    cv::Vec3b color;

    if (final_u < 0 || final_v < 0) {
      color = image_data.at<cv::Vec3b>(u_img, v_img);
    } else {
      color = image_data.at<cv::Vec3b>(final_u, final_v);
    }

    return Eigen::Vector3f(color[0], color[1], color[2]);
  }
};
#endif  // RASTERIZER_TEXTURE_H
