all: r
.PHONY: all clean

CPP_FILES = $(shell find src/ -type f -name '*.cpp')


# compile shared library for R
r: bnnlib.cpp
	R CMD SHLIB bnnlib.cpp $(CPP_FILES)


# create R wrapper with SWIG
bnnlib.cpp: bnnlib.i
	swig -c++ -r -o bnnlib.cpp bnnlib.i



# compile with g++ to test c++98
gpp:
	g++ -std=c++98 $(CPP_FILES)

clean:
	rm bnnlib.R
#	rm bnnlib.py
	rm bnnlib.cpp
	rm bnnlib.o
	rm bnnlib.so

# build python wrapper with SWIG (builds bnnlib.cpp)
python:
	swig -c++ -python -o bnnlib.cpp bnnlib.i
	
	
	
	