# PNM Image Processor
## A learning project featuring a variety of image filters and warps
I got interested in image processing after seeing it's variety of applications in machine learning, but my university only offers a class for graduate students.
As such I'm going through the Gonzales & Woods book, and implementing a variety of image processing functions to get a better understanding of processing techniques.

## Example Program
I've included an example program to showcase some of the image filters and warps.
### Compilation Instructions
```
make
```
OR
```
g++ exampleTransformations.cpp image-processor.cpp -std=c++20
```
***Note** must be compiled using -std=c++20 flag as the numbers header is used in the project.

## Usage
You need to convert your images to .ppm or .pgm(grayscale) format, the easiest way is to use [imageMagick](https://imagemagick.org/script/download.php).\
If you're on windows you will not be able view the converted files without third party software such as photoshop, or an online tool like [photopea](https://www.photopea.com/).
To avoid installing additional software on windows I'd recommend just converting your image back to an easily viewable format such as .png, after you've run the program.

## Why use the PNM format?
It's hands-down the simplest image format. I may at some point add jpeg or png support, but I'm more interested in visual image manipulation and not compression techniques.
