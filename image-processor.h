#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <bits/stdc++.h>
#include <numbers>
#include <math.h>

using std::vector;
using std::string;

class PNM {
private:
  string filename;
  int width;
  int height;
  int maxColor;
  int numChannels;
  vector<unsigned char> data;
  
  void setMembers(string filename, int width, int height, int maxColor, int numChannels, vector<unsigned char> data);

  double gaussian(int x, int y, double sd);

  vector<int> rotateCoordinates(int row, int col, double theta);

public:
  bool read(const string& filename);

  bool write();
  bool write(const string& filename);
  bool write(const string& filename, vector<unsigned char> data, int width, int height, int maxColor, int numChannels);

  // image filters

  void grayscale();

  void invertColor();

  void sepia();

  void tint(float r, float g, float b, float brightness=1);
  
  // most basic image segmentation technique
  void threshold(int epsilon=100);

  void channelSwap(char channel1, char channel2);


  // image warps
 

  void verticalFlip();
  void verticalSymmetry(char direction='t');
  
  void horizontalFlip();
  void horizontalSymmetry(char direction='l');
  
  vector<PNM> combinedSymmetry();

  void rotate(double theta, bool degrees=true);

};
