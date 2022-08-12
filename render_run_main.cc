#include <../include/tga_extension.h>
#include <../include/tgaimage.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor custom_pink = TGAColor(238, 100, 40, 255);
const TGAColor custom_green = TGAColor(58, 255, 78, 255);

int main(int argc, char** argv) {
  TGAImage image(100, 100, TGAImage::RGB);
  plot_line(0, 0, 50, 50, image, red);
  plot_line(40, 20, 35, 47, image, custom_pink);
  plot_line(50, 50, 0, 0, image, custom_green);

  image.write_tga_file("d://work//render-for-beginner//output//output.tga");
  return 0;
}