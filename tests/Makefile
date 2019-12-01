.PHONY: all clean style check

CXXFLAGS = -I../ -I../rapidjson/include

TARGETS= test_dynContainer

all: $(TARGETS)

test_dynContainer: test_dynContainer.cc
	mkoctfile-5.1.1 -Wall -Wextra $(CXXFLAGS) --link-stand-alone $< -o $@

style:
	astyle --style=gnu -s2 -n *.cc

check: test_dynContainer
	./test_dynContainer
	./check_load_save_roundtrip.sh medium1.json
	./check_load_save_roundtrip.sh big1.json

clean:
	rm -f *.o *.oct octave-workspace
	rm -f $(TARGETS)
