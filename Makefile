.PHONY: clean style

CXXFLAGS = `pkg-config --cflags RapidJSON`

load_json.oct: load_json.cc
	mkoctfile -Wall -Wextra $(CXXFLAGS) $<

style:
	astyle --style=ansi -s2 *.cc

clean:
	rm -f *.o *.oct
