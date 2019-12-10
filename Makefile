all: r
.PHONY: all clean

CPP_FILES = $(shell find src/ -type f -name '*.cpp')


# compile shared library for R
r: bnnlib.cpp
	R CMD SHLIB --preclean bnnlib.cpp $(CPP_FILES)


# create R wrapper with SWIG
bnnlib.cpp: bnnlib.i
	swig -c++ -r -o bnnlib.cpp bnnlib.i


test:
	"$(R_HOME)/bin$(R_ARCH_BIN)/Rscript" lstm.R

readme:
	Rscript -e 'rmarkdown::render("README.rmd", "md_document")'
	

# compile with g++ to test c++98
# creates executable (a.out) that can be run on command line
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
	
	
	
	