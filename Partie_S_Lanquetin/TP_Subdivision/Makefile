CC_OPTIONS=-g

geom3d.o : geom3d.cpp geom3d.hpp
	g++ $(CC_OPTIONS) -c geom3d.cpp -o geom3d.o

maillage.o : maillage.cpp maillage.hpp
	g++ $(CC_OPTIONS) -c  maillage.cpp -o maillage.o

maillage_subdivision.o : maillage_subdivision.cpp maillage_subdivision.hpp
	g++ $(CC_OPTIONS) -c  maillage_subdivision.cpp -o maillage_subdivision.o

surface_subdivision : maillage.o maillage_subdivision.o geom3d.o surface_subdivision.cpp 
	g++ $(CC_OPTIONS) maillage.o maillage_subdivision.o geom3d.o surface_subdivision.cpp \
	 -o surface_subdivision -lGL -lGLU -lglut -lGLEW  -lm

all : 	surface_subdivision

clean :
	rm -rf *.o
	
