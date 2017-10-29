.PHONY: all clean style check

CXXFLAGS = `pkg-config --cflags RapidJSON`

all: load_json.oct doit.oct

load_json.oct: load_json.cc
	mkoctfile -Wall -Wextra $(CXXFLAGS) $<

doit.oct: doit.cc dynContainer.h
	mkoctfile -Wall -Wextra $(CXXFLAGS) $<

check: doit.oct
#	octave --no-gui --eval "[a,b,c,d] = doit()"
	octave --no-gui --eval "cell2mat (doit())"

style:
	astyle --style=gnu -s2 -n *.cc

clean:
	rm -f *.o *.oct octave-workspace
