wavelibsources := $(wildcard wavelib/*.cpp)
wavegensources := $(wildcard wavegen/*.cpp)
waveliballsources := $(wavelibsources) $(wildcard wavelib/*.h) $(wildcard wavelib/*.hpp)
wavegenallsources := $(wavegensources) $(wildcard wavegen/*.h) $(wildcard wavegen/*.hpp)

CXXFLAGS=--std=c++11 -O3

wavelibobjs := $(patsubst wavelib/%, obj/wavelib/%, $(patsubst %.cpp, %.o, $(wavelibsources)))

all: obj/wavelib obj/wavegen
	@echo $(wavelibobjs)

obj/wavelib:
	mkdir -p obj/wavelib
obj/wavegen:
	mkdir -p obj/wavegen


wavelibdeps.d: $(waveliballsources) makedeps.sh
	./makedeps.sh wavelib > wavelibdeps.d

wavegendeps.d: $(wavegenallsources)

include wavelibdeps.d

