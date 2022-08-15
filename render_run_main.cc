#include <../include/tga_extension.h>
#include <../include/tgaimage.h>

const int width = 200;
const int height = 200;

Model* model = nullptr;

int main(int argc, char** argv) {
  TGAImage image(width, height, TGAImage::RGB);

  model = new Model("d://work//render-for-beginner//obj//human_head.obj");
  delete model;

  Vec2i t0[3] = {Vec2i(10, 70), Vec2i(50, 160), Vec2i(70, 80)};
  Vec2i t1[3] = {Vec2i(180, 50), Vec2i(150, 1), Vec2i(70, 180)};
  Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)};

  // triangle(t0[0], t0[1], t0[2], image, red, true);
  // triangle(t1[0], t1[1], t1[2], image, white, true);
  // triangle(t2[0], t2[1], t2[2], image, custom_green, true);

  // Vec2i pts[3] = {Vec2i(10, 10), Vec2i(100, 30), Vec2i(190, 160)};
  // triangle(pts, image, TGAColor(255, 0, 0));

  image.write_tga_file("d://work//render-for-beginner//output//output.tga");

  return 0;
}