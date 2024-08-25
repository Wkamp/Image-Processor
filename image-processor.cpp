#include "image-processor.h"

// random number generator initialization
std::random_device dev;
std::default_random_engine rng(dev());

// private


void PNM::setMembers(string filename, int width, int height, int maxColor, int numChannels, vector<unsigned char> data) {
  this->filename = filename;
  this->width = width;
  this->height = height;
  this->maxColor = maxColor;
  this->numChannels = numChannels;
  this->data = data;
}
double PNM::normalRand(double sd, double mean) {
  std::normal_distribution<double> normDist(mean, sd);
  return normDist(rng);
}
int PNM::uniformRand(int min, int max) {
  std::uniform_int_distribution<int> uniformDist(min, max);
  return uniformDist(rng);
}

void PNM::saltNoise(float noiseDensity) {
  if (numChannels != 1) {
    return;
  }

  int numSalt = noiseDensity * width * height;

  for (int i = 0; i < numSalt; i++) {
    int randRow = uniformRand(0, height - 1);
    int randCol = uniformRand(0, width - 1);

    data[width * randRow + randCol] = 255;
  }
}
void PNM::pepperNoise(float noiseDensity) {
  if (numChannels != 1) {
    return;
  }

  int numPepper = noiseDensity * width * height;

  for (int i = 0; i < numPepper; i++) {
    int randRow = uniformRand(0, height - 1);
    int randCol = uniformRand(0, width - 1);

    data[width * randRow + randCol] = 0;
  }
}

void PNM::pixelClip(int pixIndex) {
  if (data[pixIndex] > 255) {
    data[pixIndex] = 255;
  }
  else if (data[pixIndex] < 0) {
    data[pixIndex] = 0;
  }

  if (numChannels == 3) {
    if (data[pixIndex + 1] > 255) {
      data[pixIndex + 1] = 255;
    }
    else if (data[pixIndex + 1] < 0) {
      data[pixIndex + 1] = 0;
    }

    if (data[pixIndex + 2] > 255) {
      data[pixIndex + 2] = 255;
    }
    else if (data[pixIndex + 2] < 0) {
      data[pixIndex + 2] = 0;
    }

  }
}


void PNM::pixelSwap(int pixIndex1, int pixIndex2, vector<unsigned char>& vec) {
  unsigned int temp[numChannels];

  temp[0] = vec[pixIndex1];
  if (numChannels == 3) {
    temp[1] = vec[pixIndex1 + 1];
    temp[2] = vec[pixIndex1 + 2];
  }

  vec[pixIndex1] = vec[pixIndex2];
  if (numChannels == 3) {
    vec[pixIndex1 + 1] = vec[pixIndex2 + 1];
    vec[pixIndex1 + 2] = vec[pixIndex2 + 2];
  }

  vec[pixIndex2] = temp[0];
  if (numChannels == 3) {
    vec[pixIndex2 + 1] = temp[1];
    vec[pixIndex2 + 2] = temp[2];
  }
}

vector<int> PNM::pixelHSL(int pixIndex) {
  if (numChannels == 1) {
    return {0, 0, 0};
  }

  double r = data[pixIndex];
  double g = data[pixIndex + 1];
  double b = data[pixIndex + 2];
  vector<double> channels = {r, g, b};

  // normalize channels to 0-1
  channels[0] /= maxColor;
  channels[1] /= maxColor;
  channels[2] /= maxColor;
 
  vector<double> sortedChannels = channels;
  std::sort(sortedChannels.begin(), sortedChannels.end());
  const double ROUND = 0.5;
  const int DEGREES = 60;
  const int PERCENTAGE = 100;

  // calculates chroma and lightness
  double min = sortedChannels[0];
  double max = sortedChannels[2];
  double chroma = max - min;
  int lightness = ((max + min) / 2) * PERCENTAGE + ROUND;

  // calculates saturation and hue
  int saturation = 0;
  int hue = 0;
  if (chroma != 0) {
    if (lightness <= 50) {
      saturation = ((chroma) / (max + min)) * PERCENTAGE + ROUND;
    }
    else {
      saturation = ((chroma) / (2 - chroma)) * PERCENTAGE + ROUND;
    }

    if (max == channels[0]) {
      hue = ((channels[1] - channels[2]) / chroma) * DEGREES + ROUND;
    }
    else if (max == channels[1]) {
      hue = (2 + (channels[2] - channels[0]) / (chroma)) * DEGREES + ROUND;
    }
    else if (max == channels[2]) {
      hue = (4 + (channels[0] - channels[1]) / (chroma)) * DEGREES + ROUND;
    }
    
    // esures hue is non-negative and 0 to 360 degrees
    if (hue < 0) {
      hue += 360;
    }
  }

  return {hue, saturation, lightness};
}

int PNM::partition(vector<unsigned char>& vec, int low, int high) {
  int middle = low + (3 * ((high - low) / 6));
  int pivot = vec[middle];
  int i = low - 3;
  int j = high + 3;

  while (true) {
    do {
      i += 3;
    } while (vec[i] < pivot);

    do {
      j -= 3;
    } while (vec[j] > pivot); 

    if (i >= j) {return j;}

    pixelSwap(i, j, vec);
  }
}
void PNM::quickSort(vector<unsigned char>& vec, int low, int high) {
  if (low < high) {
    int pivot = partition(vec, low, high);
    quickSort(vec, low, pivot);
    quickSort(vec, pivot + 3, high);
  }
}


vector<int> PNM::rotateCoordinates(int row, int col, double theta) {
  vector<int> coordinates(2);
  coordinates[0] = std::floor(col * std::sin(theta) + row * std::cos(theta));
  coordinates[1] = std::floor(col * std::cos(theta) - row * std::sin(theta));

  return coordinates;
}

double PNM::gaussian(int row, int col, double sd) {
  double ePower = -((col * col + row * row) / (2 * sd * sd));
  double mainFrac = 1 / (2 * std::numbers::pi * sd * sd);

  return mainFrac * exp(ePower); 
}


vector<unsigned char> PNM::meanBlur(int radius) {
  vector<unsigned char> newImgData = data;
  int kernalSize = 2 * radius + 1;
  vector<vector<int>> kernal(kernalSize, vector<int>(kernalSize));

  for (int row = radius; row < height - radius; row++) {
    for (int col = radius; col < width - radius; col++) {

      vector<int> channelSum(numChannels);
      for (int kernalRow = -radius; kernalRow <= radius; kernalRow++) {
        for (int kernalCol = -radius; kernalCol <= radius; kernalCol++) {
          int kernalPixelIndex = (width * (row + kernalRow) + (col + kernalCol)) * numChannels;

          channelSum[0] += data[kernalPixelIndex];
          if (numChannels == 3) {
            channelSum[1] += data[kernalPixelIndex + 1];
            channelSum[2] += data[kernalPixelIndex + 2];
          }

        }
      }
      int pixelIndex = (width * row + col) * numChannels;

      newImgData[pixelIndex] = channelSum[0] / (kernalSize * kernalSize);
      if (numChannels == 3) {
        newImgData[pixelIndex + 1] = channelSum[1] / (kernalSize * kernalSize);
        newImgData[pixelIndex + 2] = channelSum[2] / (kernalSize * kernalSize);
      }
    }
  }
  return newImgData;
}

// public
PNM::PNM() {}
PNM::PNM(const string& filename) {
  read(filename);
}

bool PNM::read(const string& filename) {
  std::fstream fin;
  fin.open(filename, std::ios::in | std::ios::binary);

  if (!fin) {
    std::cerr << "Error: Failed to open " << filename << std::endl;
    return false;
  }

  string magicNumber;
  fin >> magicNumber;
  if (magicNumber != "P6" && magicNumber != "P5") {
    std::cerr << "Error: Incorrect file type, not PPM P5 or P6" << std::endl;
    return false;
  }
  if (magicNumber == "P6") {
    numChannels = 3;
  }
  else {
    numChannels = 1;
  }

  this->filename = filename;
  fin >> width >> height >> maxColor;
  fin.ignore(1); // ignores newline after max color;

  data.resize(width * height * numChannels);
  fin.read(reinterpret_cast<char*>(data.data()), data.size()); 
  if (!fin) {
    std::cerr << "Error: Failed to read pixel data" << std::endl;
    return false;
  }
  fin.close();

  return true;
}

bool PNM::write() {
  return write(filename, data, width, height, maxColor, numChannels);
}
bool PNM::write(const string& filename) {
  return write(filename, data, width, height, maxColor, numChannels);
}
bool PNM::write(const string& filename, vector<unsigned char> data, int width, int height, int maxColor, int numChannels) {
  std::fstream fout;
  fout.open(filename, std::ios::out | std::ios::binary);

  if (!fout) {
    std::cerr << "Error: Failed to open " << filename << std::endl;
    return false;
  }

  // file header
  if (numChannels == 3) {
    fout << "P6\n" << width << " " << height << "\n" << maxColor << "\n";
  }
  else {
    fout << "P5\n" << width << " " << height << "\n" << maxColor << "\n";
  }

  fout.write(reinterpret_cast<char*>(data.data()), data.size());
  fout.close();

  return true;
}
int PNM::getWidth() {
  return width;
}
int PNM::getHeight() {
  return height;
}
int PNM::getNumChannels() {
  return numChannels;
}

void PNM::setWidth(int width) {
  this->width = width;
}
void PNM::setHeight(int height) {
  this->height = height;
}

void PNM::setAllRChannels(int value) {
  if (value > 255) {
    value = 255;
  }
  else if (value < 0) {
    value = 0;
  }

  for (int i = 0; i < data.size(); i += 3) {
    data[i] = value;
  }
}
void PNM::setAllGChannels(int value) {
  if (numChannels != 3) {
    return;
  }
  if (value > 255) {
    value = 255;
  }
  else if (value < 0) {
    value = 0;
  }

  for (int i = 1; i < data.size(); i += 3) {
    data[i] = value;
  }

}
void PNM::setAllBChannels(int value) {
  if (numChannels != 3) {
    return;
  }
  if (value > 255) {
    value = 255;
  }
  else if (value < 0) {
    value = 0;
  }

  for (int i = 2; i < data.size(); i += 3) {
    data[i] = value;
  }

}

// image filters

int PNM::brightness(int pixIndex) {
  if (numChannels == 1) {
    return data[pixIndex];
  }

  return (data[pixIndex] + data[pixIndex + 1] + data[pixIndex + 2]) / numChannels;
}

int PNM::luminence(int pixIndex, int standard/*=709*/) {
  if (numChannels == 1) {
    return data[pixIndex];
  }
  double magicWeightR = 1;
  double magicWeightG = 1;
  double magicWeightB = 1;

  if (standard == 709) {
    magicWeightR = 0.2126;
    magicWeightG = 0.7152; 
    magicWeightB = 0.0722;
  }
  else if (standard == 601) {
    magicWeightR = 0.299;
    magicWeightG = 0.587; 
    magicWeightB = 0.114;
  }

  return data[pixIndex] * magicWeightR + data[pixIndex + 1] * magicWeightG + data[pixIndex + 2] * magicWeightB;
}

void PNM::grayscale(int standard/*=709*/) {
  if (numChannels == 1) {
    return;
  }

  vector<unsigned char> newImgData;
  newImgData.resize(width * height);

  int gray;
  for (int i = 0; i < data.size(); i += numChannels) {
    if (standard != 709 && standard != 601) {
      newImgData[i / 3] = brightness(i);
    }
    else {
      newImgData[i / 3] = luminence(i, standard);
    }
  }

  numChannels = 1;
  data = newImgData;
  if (filename.substr(filename.size() - 4, 4) == ".ppm") {
      filename.replace(filename.size() - 4, 4, ".pgm");
  }
}

void PNM::invertColor() {
  for (int i = 0; i < data.size(); i += numChannels) {
    data[i] = maxColor - data[i];

    if (numChannels == 3) {
      data[i + 1] = maxColor - data[i + 1];
      data[i + 2] = maxColor - data[i + 2];
    }
  }
}

void PNM::sepia() {
  if (numChannels == 1) {
    return;
  }

  const int BRIGHTNESS = 2;

  int gray;
  for (int i = 0; i < data.size(); i += numChannels) {
    gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];

    data[i] = gray * 0.439216 * BRIGHTNESS;
    data[i + 1] = gray * 0.258824 * BRIGHTNESS;
    data[i + 2] = gray * 0.078431 * BRIGHTNESS;
  }
}

void PNM::tint(float r, float g, float b, float brightness/*=1*/) {
  if (numChannels == 1) {
    return;
  }

  r /= 255;
  g /= 255;
  b /= 255;

  for (int i = 0; i < data.size(); i += numChannels) {
    data[i] = data[i] * r * brightness;
    data[i + 1] = data[i + 1] * g * brightness;
    data[i + 2] = data[i + 2] * b * brightness;
    pixelClip(i);
  }
}

void PNM::noise(string type, float noiseDensity) {
  if (type == "salt" || type == "Salt") {
    saltNoise(noiseDensity);
  }
  else if (type == "pepper" || type == "Pepper") {
    pepperNoise(noiseDensity);
  }
}

// most basic image segmentation technique
void PNM::threshold(int epsilon/*=100*/) {
  grayscale();

  for (int i = 0; i < data.size(); i += numChannels) {
    if (data[i] > epsilon) {
      data[i] = 255;
    }
    else {
      data[i] = 0;
    }
  }
}

void PNM::channelSwap(char channel1, char channel2) {
  if (numChannels == 1) {
    return;
  }

  std::map<char, int> channelToIndex;
  channelToIndex['r'] = 0;
  channelToIndex['g'] = 1;
  channelToIndex['b'] = 2;

  int channelOffset1 =  channelToIndex[channel1];
  int channelOffset2 =  channelToIndex[channel2];

  unsigned int temp;
  for (int i = 0; i < data.size(); i += numChannels) {
    temp = data[i + channelOffset1];
    data[i + channelOffset1] = data[i  + channelOffset2];
    data[i + channelOffset2] = temp;
  }
}

void PNM::blur(string blurType, int radius) {
  if (blurType == "mean") {
    data = meanBlur(radius);
  }
}

void PNM::chromaShift(int rshift, int gshift, int bshift, int threshold/*=0*/) {
  if (numChannels == 1) {
    return;
  }
  vector<unsigned char> newImgData = data;
  rshift *= numChannels;
  gshift *= numChannels;
  bshift *= numChannels;

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      int pixIndex = (width * row + col) * numChannels;
      if (brightness(pixIndex) < threshold) {
        continue;
      }

      if (pixIndex + rshift > 0 && pixIndex + rshift < data.size()) {
        newImgData[pixIndex + rshift] = data[pixIndex];
      }
      if (pixIndex + gshift > 0 && pixIndex + gshift < data.size()) {
        newImgData[pixIndex + gshift + 1] = data[pixIndex + 1];
      }
      if (pixIndex + bshift > 0 && pixIndex + bshift < data.size()) {
        newImgData[pixIndex + bshift + 2] = data[pixIndex + 2];
      }
    }
  }

  data = newImgData;
}

// change to gaussian blur when added later
void PNM::sharpen(double sharpness, int radius) {
  vector<unsigned char> blurredImage = meanBlur(radius);
  for (int i = 0; i < data.size(); i += numChannels) { 
    data[i] = data[i] + sharpness * (data[i] - blurredImage[i]);
    if (numChannels == 3) {
      data[i + 1] = data[i + 1] + sharpness * (data[i + 1] - blurredImage[i + 1]);
      data[i + 2] = data[i + 2] + sharpness * (data[i + 2] - blurredImage[i + 2]);
    }
  }
}


// image warps


void PNM::verticalFlip() {
  int temp[numChannels];

  for (int row = 0; row < height / 2; row++) {
    for (int col = 0; col < width; col++) {
      int topIndex = (width * row + col) * numChannels;
      int bottomIndex = ((height - row - 1) * width + col) * numChannels; 

      pixelSwap(topIndex, bottomIndex, data);
    }
  }
}
void PNM::verticalReflection(char direction/*='t'*/) {
  for (int row = 0; row < height / 2; row++) {
    for (int col = 0; col < width; col++) {

      if (direction == 't' || direction == 'T') { // 'T' is for top
        for (int i = 0; i < numChannels; i++) {
          data[((height - row - 1) * width + col) * numChannels + i] = data[(width * row + col) * numChannels + i]; 
        }
      }
      else if (direction == 'b' || direction == 'B') { // 'B' is for bottom
        for (int i = 0; i < numChannels; i++) {
          data[(width * row + col) * numChannels + i] = data[((height - row - 1) * width + col) * numChannels + i];
        }
      }
    }
  }
}

void PNM::horizontalFlip() {
  int temp[numChannels];

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width / 2; col++) {
      int leftIndex = (width * row + col) * numChannels;
      int rightIndex = (((width * row) + (width - 1)) - col) * numChannels; 

      pixelSwap(leftIndex, rightIndex, data);
    }
  }
}
void PNM::horizontalReflection(char direction/*='l'*/) {
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width / 2; col++) {

      if (direction == 'l' || direction == 'L') { // 'L' is for left
        for (int i = 0; i < numChannels; i++) {
          data[(((width * row) + (width - 1)) - col) * numChannels + i] = data[(width * row + col) * numChannels + i]; 
        }
      }
      else if (direction == 'r' || direction == 'R') { // 'R' is for right
        for (int i = 0; i < numChannels; i++) {
          data[(width * row + col) * numChannels + i] = data[(((width * row) + (width - 1)) - col) * numChannels + i];
        }
      }
    }
  }
}

vector<PNM> PNM::combinedReflection() {
  vector<PNM> reflectedImages;
  
  PNM temp;
  for (int i = 0; i < 4; i++) {
    temp.setMembers(filename, width, height, maxColor, numChannels, data);

    switch(i) {
      case 0:
        temp.verticalReflection('t');
        temp.horizontalReflection('l');
        break;

      case 1:
        temp.verticalReflection('t');
        temp.horizontalReflection('r');
        break;

      case 2:
        temp.verticalReflection('b');
        temp.horizontalReflection('l');
        break;

      case 3:
        temp.verticalReflection('b');
        temp.horizontalReflection('r');
        break;
    }
    reflectedImages.push_back(temp);
  }
  return reflectedImages;
}

void PNM::rectCrop(std::array<int, 2> upperLeft, int newWidth, int newHeight) {
  if (newWidth > width || newHeight > height) {
    return;
  }

  vector<unsigned char> newImgData(newWidth * newHeight * numChannels);

  for (int row = 0; row < newHeight; row++) {
    for (int col = 0; col < newWidth; col++) {
      int oldIndex = ((upperLeft[0] + row) * width + (upperLeft[1] + col)) * numChannels;
      int newIndex = (newWidth * row + col) * numChannels;

      newImgData[newIndex] = data[oldIndex];
      if (numChannels == 3) {
        newImgData[newIndex + 1] = data[oldIndex + 1];
        newImgData[newIndex + 2] = data[oldIndex + 2];
      }
    }
  }

  width = newWidth;
  height = newHeight;
  data = newImgData;
}

void PNM::rotate(double theta, bool degrees/*=true*/) {
  if (degrees) { // converts to radians
    theta *= std::numbers::pi / 180;
  }
  vector<int> tempRotatedCoordinates(2);

  int oldWidth = width;
  int oldHeight = height;

  // computes rotated image height and width
  width = std::abs(oldWidth * std::cos(theta)) + std::abs(oldHeight * std::sin(theta));
  height = std::abs(oldWidth * std::sin(theta)) + std::abs(oldHeight * std::cos(theta)) + 1; // off-by-one error without +1, proper fix should be done

  vector<unsigned char> newImgData(width * height * numChannels);

  // computes minimum rotated coordinates to avoid out-of-bounds indicies
  vector<int> rowBoundsIndices(4);
  vector<int> colBoundsIndices(4);

  tempRotatedCoordinates = rotateCoordinates(0, 0, theta);
  rowBoundsIndices[0] = tempRotatedCoordinates[0];
  colBoundsIndices[0] = tempRotatedCoordinates[1];

  tempRotatedCoordinates = rotateCoordinates(0, oldWidth, theta);
  rowBoundsIndices[1] = tempRotatedCoordinates[0];
  colBoundsIndices[1] = tempRotatedCoordinates[1];

  tempRotatedCoordinates = rotateCoordinates(oldHeight, oldWidth, theta);
  rowBoundsIndices[2] = tempRotatedCoordinates[0];
  colBoundsIndices[2] = tempRotatedCoordinates[1];

  tempRotatedCoordinates = rotateCoordinates(oldHeight, 0, theta);
  rowBoundsIndices[3] = tempRotatedCoordinates[0];
  colBoundsIndices[3] = tempRotatedCoordinates[1];

  auto min1 = std::min_element(rowBoundsIndices.begin(), rowBoundsIndices.end());
  int minRow = std::abs(*min1);

  auto min2 = std::min_element(colBoundsIndices.begin(), colBoundsIndices.end());
  int minCol = std::abs(*min2);

  // computes rotation
  for (int row = 0; row < oldHeight; row++) {
    for (int col = 0; col < oldWidth; col++) {
      int oldIndex = (oldWidth * row + col) * numChannels;
      tempRotatedCoordinates = rotateCoordinates(row, col, theta);

      int newRowIndex = tempRotatedCoordinates[0] + minRow;
      int newColIndex = tempRotatedCoordinates[1] + minCol;
      int newIndex = (width * newRowIndex + newColIndex) * numChannels;

      newImgData[newIndex] = data[oldIndex];

      if (numChannels == 3) {
        newImgData[newIndex + 1] = data[oldIndex + 1];
        newImgData[newIndex + 2] = data[oldIndex + 2];
      }
    }
  }

  data = newImgData;
}

void PNM::testSort() {
  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      int minIndex = (width * row + col) * numChannels;

      for (int k = (width * row + col) * numChannels; k < (height * width) * numChannels; k += numChannels) {
        if (data[minIndex] > data[k]) {
          pixelSwap(minIndex, k, data);
        }
      }
    }
  }
}

void PNM::scale(double widthScale, double heightScale, string interpolation/*="neighbor"*/) {
  vector<unsigned char> newImgData(width * widthScale * height * heightScale * numChannels);
  const double ROUND = 0.5;

  for (int row = 0; row < height * heightScale; row++) {
    for (int col = 0; col < width * widthScale; col++) {
      int newIndex = (width * widthScale * row + col) * numChannels;

      int oldRowIndex = (row / heightScale) + ROUND;
      int oldColIndex = (col / widthScale) + ROUND;
      int oldIndex = (width * oldRowIndex + oldColIndex) * numChannels;

      newImgData[newIndex] = data[oldIndex];
      if (numChannels == 3) {
        newImgData[newIndex + 1] = data[oldIndex + 1];
        newImgData[newIndex + 2] = data[oldIndex + 2];
      }
    }
  }

  data = newImgData;
  width *= widthScale;
  height *= heightScale;
}

void PNM::pixelSort(char direction/*='l'*/, string sortCriteria, bool stable /*=false*/) {
  quickSort(data, 0, width * height * numChannels - 3);
}
