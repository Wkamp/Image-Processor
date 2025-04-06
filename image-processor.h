#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <cstring>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <random>
#include <bits/stdc++.h>
#include <numbers>
#include <math.h>
#include <filesystem>

using std::vector;
using std::string;

class PNM {
private:
  std::filesystem::path filepath;
  int width;
  int height;
  int maxColor;
  int numChannels;
  vector<unsigned char> data;

  void setMembers(std::filesystem::path filepath, int width, int height, int maxColor, int numChannels, vector<unsigned char> data);

  double normalRand(double sd, double mean);
  int uniformRand(int min, int max);

  int brightness(int pixIndex);
  int luminence(int pixIndex, int standard=709);

  void saltNoise(float noiseDensity);
  void pepperNoise(float noiseDensity);
  /* void gaussianNoise(double sd, double mean=0); */

  void pixelClip(int pixIndex);

  void pixelSwap(int pixIndex1, int pixIndex2, vector<unsigned char>& vec);

  vector<int> pixelHSL(int pixIndex);
  
  int partition(vector<unsigned char>& vec, int low, int high);
  void quickSort(vector<unsigned char>& vec, int low, int high);

  vector<int> rotateCoordinates(int row, int col, double theta);
  
  double gaussian(int row, int col, double sd);

  vector<unsigned char> meanBlur(int radius);

  void testSort();

public:
  PNM();
  PNM(const std::filesystem::path& filepath);
  bool read(const std::filesystem::path& filepath);

  bool write();
  bool write(const std::filesystem::path& filepath);
  bool write(const std::filesystem::path& filepath, vector<unsigned char> data, int width, int height, int maxColor, int numChannels);

  int getWidth();
  int getHeight();
  int getNumChannels();

  void setWidth(int width);
  void setHeight(int height);

  void setAllRChannels(int value);
  void setAllGChannels(int value);
  void setAllBChannels(int value);

  // image filters

  void grayscale(int standard=709);

  void invertColor();

  void sepia();

  void tint(float r, float g, float b, float brightness=1);

  void noise(string type, float noiseDensity);

  // most basic image segmentation technique
  void threshold(int epsilon=100);

  void channelSwap(char channel1, char channel2);

  void blur(string blurType, int radius);

  void chromaShift(int rshift, int gshift, int bshift, int threshold=0);

  // image warps
 
  void verticalFlip();
  void verticalReflection(char direction='t');
  
  void horizontalFlip();
  void horizontalReflection(char direction='l');
  
  vector<PNM> combinedReflection();

  void rectCrop(std::array<int, 2> upperLeft, int newWidth, int newHeight);

  void rotate(double theta, bool degrees=true);
  
  void sharpen(double sharpness, int radius);

  void scale(double widthScale, double heightScale, string interpolation="neighbor");

  void pixelSort(char direction, string sortCriteria, bool stable=false);
};
