.PHONY: all clean style check

# use local checkout because the v1.1.0 release was 2016
CXXFLAGS = -I./rapidjson/include

TARGETS= load_json.oct save_json.oct

MKOCT=mkoctfile
OCT=octave

#force legacy test build
#MKOCT=mkoctfile-4.2.2
#OCT=octave-4.2.2

all: $(TARGETS)

rapidjson:
	git clone https://github.com/miloyip/rapidjson.git
	cd rapidjson && git checkout 35e480fc4ddf4ec4f7ad34d96353eef0aabf002d

%.oct: %.cc rapidjson
	$(MKOCT) -Wall -Wextra $(CXXFLAGS) $<

style:
	astyle --style=gnu -s2 -n *.cc

check: load_json.oct save_json.oct
	$(OCT) --no-gui-libs --eval "test load_json"
	$(OCT) --no-gui-libs --eval "test save_json"

clean:
	rm -f *.o *.oct octave-workspace
	rm -f $(TARGETS)
	$(MAKE) -C tests clean
