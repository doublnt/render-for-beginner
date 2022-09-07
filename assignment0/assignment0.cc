#include "../include/assignment.h"

int main(int argc, char** argv) {
  Eigen::Matrix<float, 3, 1> matrix_f_3_1;
  matrix_f_3_1(0, 0) = 2.f;
  matrix_f_3_1(1, 0) = 1.f;
  matrix_f_3_1(2, 0) = 0.f;
  std::cout << matrix_f_3_1 << std::endl;

  Games101::transform(matrix_f_3_1);
  std::cout << matrix_f_3_1 << std::endl;

  return 0;
}
