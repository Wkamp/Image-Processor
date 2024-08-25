#include "image-processor.h"
#include <numbers>

using std::vector;
using std::string;

void symExample(PNM img) {
  vector<PNM> images;
  
  images = img.combinedReflection();

  for (int i = 0; i < images.size(); i++) {
    images[i].write("sym" + std::to_string(i));
  }

}


int main() {
  PNM img("ice.ppm");
  symExample(img);
}
