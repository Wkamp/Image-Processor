#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using std::fstream; 
using std::vector;
using std::string;

// images are all ppm6
class PPM {
private:
  string filename;
  int width;
  int height;
  int maxColor;
  vector<unsigned char> data;

public:
  bool read(const string& filename) {
    fstream fin;
    fin.open(filename, std::ios::in | std::ios::binary);

    if (!fin) {
      std::cerr << "Error: Failed to open " << filename << std::endl;
      return false;
    }

    string magicNumber;
    fin >> magicNumber;
    if (magicNumber != "P6") {
      std::cerr << "Error: Incorrect file type, not PPM P6" << std::endl;
      return false;
    }
    this->filename = filename;

    fin >> width >> height >> maxColor;
    fin.ignore(1); // ignores newline after max color;

    data.resize(width * height * 3); // multiply by 3 as each pixel contains r, g, b color data
    fin.read(reinterpret_cast<char*>(data.data()), data.size()); 
    if (!fin) {
      std::cerr << "Error: Failed to read pixel data" << std::endl;
      return false;
    }
    fin.close();

    return true;
  }

  bool write() {
    return write(filename, data, width, height, maxColor);
  }

  bool write(const string& filename) {
    return write(filename, data, width, height, maxColor);
  }

  bool write(const string& filename, vector<unsigned char> data, int width, int height, int maxColor) {
    fstream fout;

    fout.open(filename, std::ios::out | std::ios::binary);

    if (!fout) {
      std::cerr << "Error: Failed to open " << filename << std::endl;
      return false;
    }

    fout << "P6\n" << width << " " << height << "\n" << maxColor << "\n"; // file header

    fout.write(reinterpret_cast<char*>(data.data()), data.size());
    fout.close();

    return true;
  }

  // image filters

  void grayscale() {
    int gray = 0;
    for (int i = 0; i < data.size(); i += 3) {
      gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
      data[i] = gray;
      data[i + 1] = gray;
      data[i + 2] = gray;
    }
  }

  void invertColors() {
    for (int i = 0; i < data.size(); i += 3) {
      data[i] = maxColor - data[i]; 
      data[i + 1] = maxColor - data[i + 1];
      data[i + 2] = maxColor - data[i + 2];
    }
  }


  void sepia() {
    grayscale();
    const int brighten = 2;
    for (int i = 0; i < data.size(); i += 3) {
      data[i] = data[i] * 0.439216 * brighten;
      data[i + 1] = data[i + 1] * 0.258824 * brighten;
      data[i + 2] = data[i + 2] * 0.078431 * brighten;
    }
  }

  void tint(float r, float g, float b, float brighten=1, bool gray=false) {
    if (gray) {
      grayscale();
    }
    r /= 255;
    g /= 255;
    b /= 255;

    for (int i = 0; i < data.size(); i += 3) {
      data[i] = data[i] * r * brighten;
      data[i + 1] = data[i + 1] * g * brighten;
      data[i + 2] = data[i + 2] * b * brighten;
    }
  }

  // most basic image segmentation technique
  void thresholding(int threshold=100) {
    grayscale();

    for (int i = 0; i < data.size(); i += 3) {
      if (data[i] > threshold) {
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

  // image warps
  
  void verticalFlip() {
    unsigned char temp[3];
    int iterations = 0;
    for (int i = data.size() - 1; i > data.size() / 2; i -= 3) {
      temp[0] = data[iterations];
      temp[1] = data[iterations + 1];
      temp[2] = data[iterations + 2];
      
      data[iterations] = data[i - 2];
      data[iterations + 1] = data[i - 1]; 
      data[iterations + 2] = data[i]; 

      data[i] = temp[2];
      data[i - 1] = temp[1];
      data[i - 2] = temp[0];
      
      iterations += 3;
    }
  }

};

int main() {
  PPM img;
  
  img.read("bike.ppm");
  img.sepia();
  img.write("test.ppm");

}
