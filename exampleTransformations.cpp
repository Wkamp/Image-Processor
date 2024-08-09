#include "image-processor.h"
#include <numbers>

using std::vector;
using std::string;

void symExample(PNM img) {
  vector<PNM> images;
  
  images = img.combinedSymmetry();

  for (int i = 0; i < images.size(); i++) {
    images[i].write("sym" + std::to_string(i));
  }

}


int main() {
  PNM img;
  img.read("ice.ppm");
  img.rotate(std::numbers::pi / 2, false);
  symExample(img);

}
