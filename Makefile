LIBS = -g -lGL -lGLU -lglut
all : main.o BVHObject.o camera.o
	g++ -std=c++11 main.o BVHObject.o camera.o $(LIBS) -o main

main.o : main.cpp BVHObject.h camera.h
	g++ -std=c++11 -c -g main.cpp $(LIBS) -o main.o

BVHObject.o : BVHObject.h
	g++ -std=c++11 -c -g BVHObject.cpp $(LIBS) -o BVHObject.o

camera.o : camera.h
	g++ -std=c++11 -c -g camera.cpp $(LIBS) -o camera.o

clean :
	rm *.o


