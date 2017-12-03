.PHONY: all clean style check

CXXFLAGS = `pkg-config --cflags RapidJSON`

TARGETS= load_json.oct save_json.oct doit

#MKOCT= mkoctfile-4.2.1
MKOCT= mkoctfile-4.3.0+ -Wno-deprecated-declarations

all: $(TARGETS)

load_json.oct: load_json.cc
	$(MKOCT) -Wall -Wextra $(CXXFLAGS) $<

save_json.oct: save_json.cc
	$(MKOCT) -Wall -Wextra $(CXXFLAGS) $<

test_dynContainer: test_dynContainer.cc
	$(MKOCT) -Wall -Wextra $(CXXFLAGS) --link-stand-alone $< -o $@

doit: doit.cc
	$(MKOCT) -Wall -Wextra --link-stand-alone $< -o $@

style:
	astyle --style=gnu -s2 -n *.cc

check: load_json.oct
	octave --no-gui --eval "test load_json"

clean:
	rm -f *.o *.oct octave-workspace
	rm -f $(TARGETS)
	$(MAKE) -C tests clean
