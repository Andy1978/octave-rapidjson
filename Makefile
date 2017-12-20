.PHONY: all clean style check

CXXFLAGS = `pkg-config --cflags RapidJSON`

TARGETS= load_json.oct save_json.oct

MKOCT= mkoctfile-4.2.1
#MKOCT= mkoctfile-4.3.0+ -Wno-deprecated-declarations

all: $(TARGETS)

%.oct: %.cc
	$(MKOCT) -Wall -Wextra $(CXXFLAGS) $<

style:
	astyle --style=gnu -s2 -n *.cc

check: load_json.oct save_json.oct
	octave --no-gui-libs --eval "test load_json"
	octave --no-gui-libs --eval "test save_json"

clean:
	rm -f *.o *.oct octave-workspace
	rm -f $(TARGETS)
	$(MAKE) -C tests clean
