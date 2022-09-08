#include <iostream>
#include <opencv2/opencv.hpp>

#include "../include/assignment.h"
#include "../include/global.hpp"
#include "OBJ_Loader.h"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Triangle.hpp"
#include "rasterizer.hpp"

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos) {
  Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

  Eigen::Matrix4f translate;
  translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1, -eye_pos[2],
      0, 0, 0, 1;

  view = translate * view;

  return view;
}

Eigen::Matrix4f get_model_matrix(float angle) {
  Eigen::Matrix4f rotation;
  angle = angle * MY_PI / 180.f;
  rotation << cos(angle), 0, sin(angle), 0, 0, 1, 0, 0, -sin(angle), 0,
      cos(angle), 0, 0, 0, 0, 1;

  Eigen::Matrix4f scale;
  scale << 2.5, 0, 0, 0, 0, 2.5, 0, 0, 0, 0, 2.5, 0, 0, 0, 0, 1;

  Eigen::Matrix4f translate;
  translate << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;

  return translate * rotation * scale;
}

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload) {
  return payload.position;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload) {
  Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() +
                                  Eigen::Vector3f(1.0f, 1.0f, 1.0f)) /
                                 2.f;
  Eigen::Vector3f result;
  result << return_color.x() * 255, return_color.y() * 255,
      return_color.z() * 255;
  return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec,
                               const Eigen::Vector3f& axis) {
  auto costheta = vec.dot(axis);
  return (2 * costheta * axis - vec).normalized();
}

struct light {
  Eigen::Vector3f position;
  Eigen::Vector3f intensity;
};

Eigen::Vector3f texture_fragment_shader(
    const fragment_shader_payload& payload) {
  Eigen::Vector3f return_color = {0, 0, 0};
  if (payload.texture) {
    // TODO: Get the texture value at the texture coordinates of the current
    // fragment
    auto u = payload.tex_coords[0];
    auto v = payload.tex_coords[1];

    if (u >= 0 && v >= 0) {
      return_color = payload.texture->getColorBilinear(u, v);
    }
  }

  Eigen::Vector3f texture_color;
  texture_color << return_color.x(), return_color.y(), return_color.z();

  Eigen::Vector3f k_ambient = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f k_diffuse = texture_color / 255.f;
  Eigen::Vector3f k_specular = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150;

  Eigen::Vector3f color = texture_color;
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  Eigen::Vector3f result_color = {0, 0, 0};

  for (auto& light : lights) {
    // lighting direction
    auto lighting = light.position - point;
    auto lighting_direction_normalized = lighting.normalized();

    // viewer direction
    auto v_direction_normalized = (eye_pos - point).normalized();

    auto r_square = lighting.squaredNorm();
    Eigen::Vector3f energy_arrived_shading_point = light.intensity / r_square;

    auto light_normal_cos_theta = normal.dot(lighting_direction_normalized);

    auto light_diffuse = k_diffuse.cwiseProduct(energy_arrived_shading_point) *
                         std::max(0.f, light_normal_cos_theta);

    Eigen::Vector3f h =
        (v_direction_normalized + lighting_direction_normalized).normalized();

    auto normal_h_cos_theta = normal.dot(h);

    auto light_specular =
        k_specular.cwiseProduct(energy_arrived_shading_point) *
        std::pow(std::max(0.f, normal_h_cos_theta), p);

    auto light_ambient = k_ambient.cwiseProduct(amb_light_intensity);

    result_color += light_ambient + light_specular + light_diffuse;
  }

  return result_color * 255.f;
}

// shading per pixel Blinn Phong Relectance model
// ambient diffuse specular
Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload) {
  Eigen::Vector3f k_ambient = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f k_diffuse = payload.color / 255.f;
  Eigen::Vector3f k_specular = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  // control specular how much
  float p = 150;

  Eigen::Vector3f color = payload.color;

  // this is shading point
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  Eigen::Vector3f result_color = {0, 0, 0};

  for (auto& light : lights) {
    // lighting direction
    auto lighting = light.position - point;
    auto lighting_direction_normalized = lighting.normalized();

    // viewer direction
    auto v_direction_normalized = (eye_pos - point).normalized();

    auto r_square = lighting.squaredNorm();
    Eigen::Vector3f energy_arrived_shading_point = light.intensity / r_square;

    auto light_normal_cos_theta = normal.dot(lighting_direction_normalized);

    auto light_diffuse = k_diffuse.cwiseProduct(energy_arrived_shading_point) *
                         std::max(0.f, light_normal_cos_theta);

    Eigen::Vector3f h =
        (v_direction_normalized + lighting_direction_normalized).normalized();

    auto normal_h_cos_theta = normal.dot(h);

    auto light_specular =
        k_specular.cwiseProduct(energy_arrived_shading_point) *
        std::pow(std::max(0.f, normal_h_cos_theta), p);

    auto light_ambient = k_ambient.cwiseProduct(amb_light_intensity);

    result_color += light_ambient + light_specular + light_diffuse;
  }

  return result_color * 255.f;
}

Eigen::Vector3f displacement_fragment_shader(
    const fragment_shader_payload& payload) {
  Eigen::Vector3f k_ambient = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f k_diffuse = payload.color;
  Eigen::Vector3f k_specular = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150;

  Eigen::Vector3f color = payload.color;
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  float kh = 0.2, kn = 0.1;

  Eigen::Vector3f n = normal.normalized();
  auto x{normal.x()}, y{normal.y()}, z{normal.z()};

  Eigen::Vector3f t(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z),
                    z * y / sqrt(x * x + z * z));
  t = t.normalized();

  auto b = n.cross(t).normalized();

  Eigen::Matrix3f TBN;
  TBN << t, b, n;

  auto u = payload.tex_coords[0];
  auto v = payload.tex_coords[1];

  // u and v less than 0 is illegal
  if (u <= 0 || v <= 0) {
    return Eigen::Vector3f{0, 0, 0};
  }

  auto u_v = payload.texture->getColorBilinear(u, v).norm();
  auto h = payload.texture->height;
  auto w = payload.texture->width;

  auto dU = kh * kn *
            (payload.texture->getColorBilinear(u + 1.f / w, v).norm() - u_v);
  auto dV = kh * kn *
            (payload.texture->getColorBilinear(u, v + 1.f / h).norm() - u_v);

  Eigen::Vector3f ln{-dU, -dV, 1.f};

  point = point + kn * u_v * n.normalized();
  normal = (TBN * ln.normalized()).normalized();

  Eigen::Vector3f result_color = {0, 0, 0};

  for (auto& light : lights) {
    // lighting direction
    auto lighting = light.position - point;
    auto lighting_direction_normalized = lighting.normalized();

    // viewer direction
    auto v_direction_normalized = (eye_pos - point).normalized();

    auto r_square = lighting.squaredNorm();
    Eigen::Vector3f energy_arrived_shading_point = light.intensity / r_square;

    auto light_normal_cos_theta = normal.dot(lighting_direction_normalized);

    auto light_diffuse = k_diffuse.cwiseProduct(energy_arrived_shading_point) *
                         std::max(0.f, light_normal_cos_theta);

    Eigen::Vector3f h =
        (v_direction_normalized + lighting_direction_normalized).normalized();

    auto normal_h_cos_theta = normal.dot(h);

    auto max_no = std::max(0.f, normal_h_cos_theta);
    auto light_specular =
        k_specular.cwiseProduct(energy_arrived_shading_point) *
        std::pow(max_no, p);

    auto light_ambient = k_ambient.cwiseProduct(amb_light_intensity);

    result_color += light_ambient + light_specular + light_diffuse;
  }

  return result_color * 255.f;
}

Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload) {
  Eigen::Vector3f k_ambient = Eigen::Vector3f(0.005, 0.005, 0.005);
  Eigen::Vector3f k_diffuse = payload.color;
  Eigen::Vector3f k_specular = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

  auto l1 = light{{20, 20, 20}, {500, 500, 500}};
  auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

  std::vector<light> lights = {l1, l2};
  Eigen::Vector3f amb_light_intensity{10, 10, 10};
  Eigen::Vector3f eye_pos{0, 0, 10};

  float p = 150;

  Eigen::Vector3f color = payload.color;
  Eigen::Vector3f point = payload.view_pos;
  Eigen::Vector3f normal = payload.normal;

  float kh = 0.2, kn = 0.1;

  Eigen::Vector3f n{normal};
  auto x{n.x()}, y{n.y()}, z{n.z()};

  Eigen::Vector3f t(x * y / sqrt(x * x + z * z), sqrt(x * x + z * z),
                    z * y / sqrt(x * x + z * z));
  t = t.normalized();

  auto b = n.cross(t).normalized();

  Eigen::Matrix3f TBN;
  TBN << t, b, n;

  auto u = payload.tex_coords[0];
  auto v = payload.tex_coords[1];

  // u and v less than 0 is illegal
  if (u < 0 || v < 0) {
    return Eigen::Vector3f{0, 0, 0};
  }

  auto u_v = payload.texture->getColorBilinear(u, v).norm();
  auto h = payload.texture->height;
  auto w = payload.texture->width;

  auto dU = kh * kn *
            (payload.texture->getColorBilinear(u + 1.f / w, v).norm() - u_v);
  auto dV = kh * kn *
            (payload.texture->getColorBilinear(u, v + 1.f / h).norm() - u_v);

  Eigen::Vector3f ln{-dU, -dV, 1};

  n = TBN * ln;
  Eigen::Vector3f result_color = {0, 0, 0};
  result_color = n.normalized();

  return result_color * 255.f;
}

int main(int argc, const char** argv) {
  std::vector<Triangle*> TriangleList;

  float angle = 140.0;
  bool command_line = false;

  std::string filename = "output.png";
  objl::Loader Loader;
  std::string obj_path = "D:/work/cpp-program/assignment3/models/spot/";

  // Load .obj File
  bool loadout = Loader.LoadFile(
      "D:/work/cpp-program/assignment3/models/spot/"
      "spot_triangulated_good.obj");

  for (auto& mesh : Loader.LoadedMeshes) {
    for (int i = 0; i < mesh.Vertices.size(); i += 3) {
      Triangle* t = new Triangle();
      for (int j = 0; j < 3; j++) {
        t->setVertex(j, Vector4f(mesh.Vertices[i + j].Position.X,
                                 mesh.Vertices[i + j].Position.Y,
                                 mesh.Vertices[i + j].Position.Z, 1.0));
        t->setNormal(j, Vector3f(mesh.Vertices[i + j].Normal.X,
                                 mesh.Vertices[i + j].Normal.Y,
                                 mesh.Vertices[i + j].Normal.Z));
        t->setTexCoord(j, Vector2f(mesh.Vertices[i + j].TextureCoordinate.X,
                                   mesh.Vertices[i + j].TextureCoordinate.Y));
      }
      TriangleList.push_back(t);
    }
  }

  rst::rasterizer r_square(700, 700);

  auto texture_path = "hmap.jpg";
  r_square.set_texture(Texture(obj_path + texture_path));

  std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader =
      phong_fragment_shader;

  // argc = 3;
  // argv[1] = "output.png";
  // argv[2] = "bump";

  if (argc >= 2) {
    command_line = true;
    filename = std::string(argv[1]);

    if (argc == 3 && std::string(argv[2]) == "texture") {
      std::cout << "Rasterizing using the texture shader\n";
      active_shader = texture_fragment_shader;
      texture_path = "spot_texture.png";
      r_square.set_texture(Texture(obj_path + texture_path));
    } else if (argc == 3 && std::string(argv[2]) == "normal") {
      std::cout << "Rasterizing using the normal shader\n";
      active_shader = normal_fragment_shader;
    } else if (argc == 3 && std::string(argv[2]) == "phong") {
      std::cout << "Rasterizing using the phong shader\n";
      active_shader = phong_fragment_shader;
    } else if (argc == 3 && std::string(argv[2]) == "bump") {
      std::cout << "Rasterizing using the bump shader\n";
      active_shader = bump_fragment_shader;
    } else if (argc == 3 && std::string(argv[2]) == "displacement") {
      std::cout << "Rasterizing using the displacement shader\n";
      active_shader = displacement_fragment_shader;
    }
  }

  Eigen::Vector3f eye_pos = {0, 0, 10};

  r_square.set_vertex_shader(vertex_shader);
  r_square.set_fragment_shader(active_shader);

  int key = 0;
  int frame_count = 0;

  if (command_line) {
    r_square.clear(rst::Buffers::Color | rst::Buffers::Depth);
    r_square.set_model(get_model_matrix(angle));
    r_square.set_view(get_view_matrix(eye_pos));
    r_square.set_projection(Games101::get_projection_matrix(45.0, 1, 0.1, 50));

    r_square.draw(TriangleList);
    cv::Mat image(700, 700, CV_32FC3, r_square.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

    cv::imwrite(filename, image);

    return 0;
  }

  while (key != 27) {
    r_square.clear(rst::Buffers::Color | rst::Buffers::Depth);

    r_square.set_model(get_model_matrix(angle));
    r_square.set_view(get_view_matrix(eye_pos));
    r_square.set_projection(Games101::get_projection_matrix(45.0, 1, 0.1, 50));

    // r_square.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
    r_square.draw(TriangleList);
    cv::Mat image(700, 700, CV_32FC3, r_square.frame_buffer().data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

    cv::imshow("image", image);
    cv::imwrite(filename, image);
    key = cv::waitKey(10);

    if (key == 'a') {
      angle -= 0.1;
    } else if (key == 'd') {
      angle += 0.1;
    }
  }
  return 0;
}
