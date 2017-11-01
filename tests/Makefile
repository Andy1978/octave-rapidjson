.PHONY: all clean style check

CXXFLAGS = `pkg-config --cflags RapidJSON` -I../

TARGETS= test_dynContainer

all: $(TARGETS)

test_dynContainer: test_dynContainer.cc
	mkoctfile -Wall -Wextra $(CXXFLAGS) --link-stand-alone $< -o $@

style:
	astyle --style=gnu -s2 -n *.cc

check: test_dynContainer
	./test_dynContainer

clean:
	rm -f *.o *.oct octave-workspace
	rm -f $(TARGETS)
