PKGNAME := $(shell sed -n "s/Package: *\([^ ]*\)/\1/p" DESCRIPTION)
PKGVERS := $(shell sed -n "s/Version: *\([^ ]*\)/\1/p" DESCRIPTION)
PKGSRC  := $(shell basename `pwd`)

all: r
.PHONY: all clean

CPP_FILES = $(shell find src/ -type f -name '*.cpp')


rd:
	Rscript -e 'roxygen2::roxygenise(".")'

build:
	cd ..;\
	R CMD build $(PKGSRC)

install:
	cd ..;\
	R CMD INSTALL $(PKGNAME)_$(PKGVERS).tar.gz

check: build
	cd ..;\
	R CMD check --as-cran $(PKGNAME)_$(PKGVERS).tar.gz

# compile shared library for R
rclean: bnnlib.cpp $(CPP_FILES)
	R CMD SHLIB --preclean bnnlib.cpp $(CPP_FILES)

r: bnnlib.cpp $(CPP_FILES)
	R CMD SHLIB bnnlib.cpp $(CPP_FILES)


# create R wrapper with SWIG
bnnlib.cpp: bnnlib.i
	swig -c++ -r -o bnnlib.cpp bnnlib.i


test:
	"$(R_HOME)/bin$(R_ARCH_BIN)/Rscript" lstm.R

readme:
	Rscript -e 'rmarkdown::render("README.rmd", "md_document")'
	

# compile with g++ to test c++98
# creates executable (a.out) that can be run on command line
gpp: $(CPP_FILES)
	g++ -std=c++98 $(CPP_FILES)

clean:
	rm bnnlib.R
	rm bnnlib.cpp
	rm bnnlib.o
	rm bnnlib.so

# build python wrapper with SWIG (builds bnnlib.cpp)
python: bnnlib.cpp
	swig -c++ -python -o bnnlib.cpp bnnlib.i
	
	
	
	

