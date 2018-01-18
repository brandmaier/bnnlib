

# create R wrapper
r:
	swig -c++ -r bnnlib.i


clean:
	rm bnnlib.R
	rm bnnlib.py
	rm bnnlib_wrap.cxx