#include "image-processor.h"
#include <numbers>

using std::vector;
using std::string;

void symExample(PNM img) {
  vector<PNM> images;
  
  images = img.combinedReflection();

  for (int i = 0; i < images.size(); i++) {
    images[i].write("sym" + std::to_string(i) + ".pgm");
  }

}


int main() {
  PNM ice("ice.ppm");
  PNM beach("beach.ppm");
  PNM snow("snow.ppm");

  ice.threshold(100);
  symExample(ice);

  beach.grayscale();
  beach.noise("salt", 0.05);
  beach.noise("pepper", 0.05);
  beach.write("noisyBeach.pgm");

  snow.chromaShift(200, -100, 300, 70);
  snow.write("shiftedSnow.ppm");
}
