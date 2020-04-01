wavelibsources := $(wildcard wavelib/*.cpp)
wavegensources := $(wildcard wavegen/*.cpp)
waveliballsources := $(wavelibsources) $(wildcard wavelib/*.h) $(wildcard wavelib/*.hpp)
wavegenallsources := $(wavegensources) $(wildcard wavegen/*.h) $(wildcard wavegen/*.hpp)

CXXFLAGS=--std=c++14 -O3
wavelibobjs := $(patsubst wavelib/%, obj/wavelib/%, $(patsubst %.cpp, %.o, $(wavelibsources)))
wavegenobjs := $(patsubst wavegen/%, obj/wavegen/%, $(patsubst %.cpp, %.o, $(wavegensources)))

all: outdirs bin/wavegen

test:
	@echo $(wavegenobjs)

outdirs: obj/wavelib obj/wavegen bin

obj/wavelib:
	mkdir -p obj/wavelib
obj/wavegen:
	mkdir -p obj/wavegen
bin:
	mkdir bin

bin/wavegen: $(wavegenobjs) $(wavelibobjs)
	$(CXX) $(wavegenobjs) $(wavelibobjs) -o bin/wavegen


wavelibdeps.d: $(waveliballsources) makedeps.sh
	./makedeps.sh wavelib > wavelibdeps.d

wavegendeps.d: $(wavegenallsources) makedeps.sh
	./makedeps.sh wavegen > wavegendeps.d

include wavelibdeps.d
include wavegendeps.d
