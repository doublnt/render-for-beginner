#include <../include/tga_extension.h>
#include <../include/tgaimage.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor custom_pink = TGAColor(238, 100, 40, 255);
const TGAColor custom_green = TGAColor(58, 255, 78, 255);

const int width = 800;
const int height = 800;

Model* model = nullptr;

int main(int argc, char** argv) {
  TGAImage image(width, height, TGAImage::RGB);

  model = new Model("d://work//render-for-beginner//obj//human_head.obj");
  plot_face(model, image, width, height, white);
  image.flip_vertically();
  image.write_tga_file("d://work//render-for-beginner//output//output.tga");

  delete model;
  return 0;
}