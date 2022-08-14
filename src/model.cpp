// summary:
// read face structure from obj file and cache to memory.
// include faces vec and vects.

#include "../include/model.h"

#include <fstream>
#include <iostream>
#include <sstream>

Model::Model(const char* filename) : verts_(), faces_() {
  std::ifstream in;
  in.open(filename, std::ifstream::in);

  if (in.fail()) {
    return;
  }

  std::string line;
  while (!in.eof()) {
    std::getline(in, line);
    std::istringstream iss(line.c_str());

    char trash;

    if (!line.compare(0, 2, "v ")) {
      iss >> trash;

      Vec3f v;
      iss >> v.x;
      iss >> v.y;
      iss >> v.z;

      verts_.emplace_back(v);
    } else if (!line.compare(0, 2, "f ")) {
      std::vector<int> f;
      int itrash, idx;
      iss >> trash;

      while (iss >> idx >> trash >> itrash >> trash >> itrash) {
        idx--;
        f.emplace_back(idx);
      }

      faces_.emplace_back(f);
    }
  }

  std::cout << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() { return static_cast<int>(verts_.size()); }

int Model::nfaces() { return static_cast<int>(faces_.size()); }

std::vector<int> Model::face(int idx) { return faces_[idx]; }

Vec3f Model::vert(int i) { return verts_[i]; }