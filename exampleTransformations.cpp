#include "image-processor.h"

using std::vector;
using std::string;

int main() {
  PNM img;
  vector<PNM> images;
  
  img.read("ice.ppm");
  images = img.combinedSymmetry();

  string filename;
  for (int i = 0; i < images.size(); i++) {
    images[i].write("sym" + std::to_string(i));
  }
}
