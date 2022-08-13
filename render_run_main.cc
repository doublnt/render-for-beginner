#include <../include/tga_extension.h>
#include <../include/tgaimage.h>

const int width = 400;
const int height = 400;

Model* model = nullptr;

int main(int argc, char** argv) {
  TGAImage image(width, height, TGAImage::RGB);

  model = new Model("d://work//render-for-beginner//obj//human_head.obj");
  // face(model, image, width, height, white);

  Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
  Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
  Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

  triangle(t0[0], t0[1], t0[2], image, red);
  triangle(t1[0], t1[1], t1[2], image, white);
  triangle(t2[0], t2[1], t2[2], image, custom_green);

  image.write_tga_file("d://work//render-for-beginner//output//output.tga");

  delete model;
  return 0;
}