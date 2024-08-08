example: exampleTransformations.o image-processor.o
	g++ image-processor.o exampleTransformations.o -o example

exampleTransformations.o: exampleTransformations.cpp
	g++ -c exampleTransformations.cpp

image-processor.o: image-processor.cpp image-processor.h
	g++ -c image-processor.cpp -std=c++20
	
clean:
	rm *.o example
