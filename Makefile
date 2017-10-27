.PHONY: clean style

CXXFLAGS = `pkg-config --cflags RapidJSON`

load_json.oct: load_json.cc
	mkoctfile -Wall -Wextra $(CXXFLAGS) $<

style:
	astyle --style=gnu -s2 *.cc

clean:
	rm -f *.o *.oct
