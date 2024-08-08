  #include "image-processor.h"
  
  // private
  
  double PNM::gaussian(int x, int y, double sd) {
    double ePower = -((x * x + y * y) / (2 * sd * sd));
    double mainFrac = 1 / (2 * std::numbers::pi * sd * sd);

    return mainFrac * exp(ePower); 
  }
  
  void PNM::setMembers(string filename, int width, int height, int maxColor, int numChannels, vector<unsigned char> data) {
    this->filename = filename;
    this->width = width;
    this->height = height;
    this->maxColor = maxColor;
    this->numChannels = numChannels;
    this->data = data;
  }

// public

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

  // image filters

  void PNM::grayscale() {
    if (numChannels == 1) {
      return;
    }

    vector<unsigned char> newImgData;
    newImgData.resize(width * height);

    int gray;
    for (int i = 0; i < data.size(); i += numChannels) {
      gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
      newImgData[i / 3] = gray;
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
    for (int i = 0; i < data.size(); i += 3) {
      gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];

      data[i] = gray * 0.439216 * BRIGHTNESS;
      data[i + 1] = gray * 0.258824 * BRIGHTNESS;
      data[i + 2] = gray * 0.078431 * BRIGHTNESS;
    }
  }

  void PNM::tint(float r, float g, float b, float brightness) {
    if (numChannels == 1) {
      return;
    }

    r /= 255;
    g /= 255;
    b /= 255;

    for (int i = 0; i < data.size(); i += 3) {
      data[i] = data[i] * r * brightness;
      data[i + 1] = data[i + 1] * g * brightness;
      data[i + 2] = data[i + 2] * b * brightness;
    }
  }
  
  // most basic image segmentation technique
  void PNM::threshold(int epsilon) {
    grayscale();

    for (int i = 0; i < data.size(); i += 3) {
      if (data[i] > epsilon) {
        data[i] = 255;
        data[i + 1] = 255;
        data[i + 2] = 255;
      }
      else {
        data[i] = 0;
        data[i + 1] = 0;
        data[i + 2] = 0;
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
    for (int i = 0; i < data.size(); i += 3) {
      temp = data[i + channelOffset1];
      data[i + channelOffset1] = data[i  + channelOffset2];
      data[i + channelOffset2] = temp;
    }
  }


  // image warps
 

  void PNM::verticalFlip() {
    int temp[numChannels];

    for (int row = 0; row < height / 2; row++) {
      for (int col = 0; col < width; col++) {

        // stores top half of image for swap
        for (int i = 0; i < numChannels; i++) {
          temp[i] = data[(width * row + col) * numChannels + i];

        }

        // sets top half of image to bottom half
        for (int i = 0; i < numChannels; i++) {
          data[(width * row + col) * numChannels + i] = data[((height - row - 1) * width + col) * numChannels + i];
        }
        
        // sets bottom half of image to top half
        for (int i = 0; i < numChannels; i++) {
          data[((height - row - 1) * width + col) * numChannels + i] = temp[i];
        }
      }
    }
  }
  void PNM::verticalSymmetry(char direction) {
    for (int row = 0; row < height / 2; row++) {
      for (int col = 0; col < width; col++) {

        if (direction == 'T' || direction == 't') {
          for (int i = 0; i < numChannels; i++) {
            data[((height - row - 1) * width + col) * numChannels + i] = data[(width * row + col) * numChannels + i]; 
          }
        }
        else if (direction == 'B' || direction == 'b') {
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

        // stores left half of image for swap
        for (int i = 0; i < numChannels; i++) {
          temp[i] = data[(width * row + col) * numChannels + i];
        }

        // sets left half of image to right half
        for (int i = 0; i < numChannels; i++) {
          data[(width * row + col) * numChannels + i] = data[(((width * row) + (width - 1)) - col) * numChannels + i];
        }

        // sets right half of image to left half
        for (int i = 0; i < numChannels; i++) {
          data[(((width * row) + (width - 1)) - col) * numChannels + i] = temp[i];
        }
      }
    }
  }
  void PNM::horizontalSymmetry(char direction) {
    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width / 2; col++) {

        if (direction == 'L' || direction == 'l') {
          for (int i = 0; i < numChannels; i++) {
            data[(((width * row) + (width - 1)) - col) * numChannels + i] = data[(width * row + col) * numChannels + i]; 
          }
        }
        if (direction == 'R' || direction == 'r') {
          for (int i = 0; i < numChannels; i++) {
            data[(width * row + col) * numChannels + i] = data[(((width * row) + (width - 1)) - col) * numChannels + i];
          }
        }
      }
    }
  }
  
  vector<PNM> PNM::combinedSymmetry() {
    vector<PNM> symmetricalImages;
    
    PNM temp;
    for (int i = 0; i < 4; i++) {
      temp.setMembers(filename, width, height, maxColor, numChannels, data);

      switch(i) {
        case 0:
          temp.verticalSymmetry('t');
          temp.horizontalSymmetry('l');
          break;

        case 1:
          temp.verticalSymmetry('t');
          temp.horizontalSymmetry('r');
          break;

        case 2:
          temp.verticalSymmetry('b');
          temp.horizontalSymmetry('l');
          break;

        case 3:
          temp.verticalSymmetry('b');
          temp.horizontalSymmetry('r');
          break;
      }
      symmetricalImages.push_back(temp);
    }
    return symmetricalImages;
  }
