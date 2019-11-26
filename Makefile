all: r
.PHONY: all clean

# create R wrapper
bnnlib.cpp: bnnlib.i
	swig -c++ -r -o bnnlib.cpp bnnlib.i

CPP_FILES = $(shell find src/ -type f -name '*.cpp')

# complile for R
r: bnnlib.cpp
	R CMD SHLIB bnnlib.cpp $(CPP_FILES)

# compile with g++ to test c++98
gpp:
	g++ -std=c++98 $(CPP_FILES)

clean:
	rm bnnlib.R
	rm bnnlib.py
	rm bnnlib_wrap.cpp