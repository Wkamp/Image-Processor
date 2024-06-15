#include <iostream>
#include <fstream>
#include <vector>

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

  void grayscale() {
    int gray = 0;
    for (int i = 0; i < data.size(); i += 3) {
      gray = 0.2126 * data[i] + 0.7152 * data[i + 1] + 0.0722 * data[i + 2];
      data[i] = gray;
      data[i + 1] = gray;
      data[i + 2] = gray;
    }
  }
};

int main() {
  PPM image;
  
  image.read("mantis.ppm");
  image.grayscale();
  image.write("test.ppm");

}
