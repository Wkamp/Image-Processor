#include "image-processor.h"

using std::vector;
using std::string;

void symExample(PNM img, const string outputPath) {
  vector<PNM> images;
  
  images = img.combinedReflection();

  for (int i = 0; i < images.size(); i++) {
    string filename = "sym" + std::to_string(i + 1);
    images[i].write(outputPath + filename);
  }

}


int main() {
  PNM ice("images/ice.ppm");
  PNM beach("images/beach.ppm");
  PNM snow("images/snow.ppm");

  const string outputPath = "exampleOutput/";

  ice.threshold(100);
  symExample(ice, outputPath);

  beach.grayscale();
  beach.noise("salt", 0.05);
  beach.noise("pepper", 0.05);
  beach.write(outputPath + "noisyBeach");

  snow.chromaShift(200, -100, 300, 70);
  snow.write(outputPath + "shiftedSnow.ppm");
}
