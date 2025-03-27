# PNM Image Processor
## A learning project featuring a variety of image filters and warps
I got interested in image processing after seeing its variety of applications in machine learning, but my university only offers a class for graduate students.
As such I'm going through the Gonzales & Woods book, and implementing a variety of image processing functions to get a better understanding of processing techniques.

For more details on how the processor works you can read the [blog post on my website](https://eiron.xyz/image-processor/index.html)

## Example Program
I've included an example program to showcase some of the image filters and warps.
### Compilation Instructions
#### Linux
```
make
```
or
```
g++ exampleTransformations.cpp image-processor.cpp -std=c++20 -o example
```
#### Windows
```
gcc exampleTransformations.cpp image-processor.cpp -std=c++20 -lstdc++ -o example
```

***Note** must be compiled using -std=c++20 flag as the numbers header is used in the project.

## Usage
You need to convert your images to .ppm or .pgm(grayscale) format, the easiest way is to use [imageMagick](https://imagemagick.org/script/download.php).

If you're on windows you will not be able view the converted files without third party software such as photoshop, or an online tool like [photopea](https://www.photopea.com/).
To avoid installing additional software on windows I'd recommend just converting your image back to an easily viewable format such as .png, after you've run the program.

## Why use the PNM format?
It's hands-down the simplest image format. I may at some point explore some compression techniques, but I don't intend to support modern image formats as this is a learning project.
