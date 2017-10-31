.PHONY: all clean style check

CXXFLAGS = `pkg-config --cflags RapidJSON`

TARGETS= load_json.oct test_dynContainer

all: $(TARGETS)

load_json.oct: load_json.cc
	mkoctfile -Wall -Wextra $(CXXFLAGS) $<

test_dynContainer: test_dynContainer.cc
	mkoctfile -Wall -Wextra $(CXXFLAGS) --link-stand-alone $< -o $@

style:
	astyle --style=gnu -s2 -n *.cc

clean:
	rm -f *.o *.oct octave-workspace
	rm -f $(TARGETS)
