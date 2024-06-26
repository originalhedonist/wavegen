wavelibsources := $(wildcard wavelib/*.cpp)
wavegensources := $(wildcard wavegen/*.cpp)
symphgensources := $(wildcard symphgen/*.cpp)
waveliballsources := $(wavelibsources) $(wildcard wavelib/*.h) $(wildcard wavelib/*.hpp)
wavegenallsources := $(wavegensources) $(wildcard wavegen/*.h) $(wildcard wavegen/*.hpp)
symphgenallsources := $(symphgensources) $(wildcard symphgen/*.h) $(wildcard symphgen/*.hpp)

CXX=g++
CXXFLAGS=--std=c++20 -O3
wavelibobjs := $(patsubst wavelib/%, obj/wavelib/%, $(patsubst %.cpp, %.o, $(wavelibsources)))
wavegenobjs := $(patsubst wavegen/%, obj/wavegen/%, $(patsubst %.cpp, %.o, $(wavegensources)))
symphgenobjs := $(patsubst symphgen/%, obj/symphgen/%, $(patsubst %.cpp, %.o, $(symphgensources)))

all: outdirs bin/wavegen bin/symphgen

test:
	@echo $(wavegenobjs)

outdirs: obj/wavelib obj/wavegen obj/symphgen bin

obj/wavelib:
	mkdir -p obj/wavelib
obj/wavegen:
	mkdir -p obj/wavegen
obj/symphgen:
	mkdir -p obj/symphgen

%.wav: %.json bin/wavegen *.exprtk
	bin/wavegen $< $@

%.mp3 : %.wav
	lame $<

bin:
	mkdir bin

bin/wavegen: outdirs wavelibdeps.d wavegendeps.d $(wavegenobjs) $(wavelibobjs)
	$(CXX) $(wavegenobjs) $(wavelibobjs) -o bin/wavegen

bin/symphgen: outdirs wavelibdeps.d symphgendeps.d $(symphgenobjs) $(wavelibobjs)
	$(CXX) $(symphgenobjs) $(wavelibobjs) -o bin/symphgen

wavelibdeps.d: $(waveliballsources) makedeps.sh
	./makedeps.sh wavelib > wavelibdeps.d

wavegendeps.d: $(wavegenallsources) makedeps.sh
	./makedeps.sh wavegen > wavegendeps.d

symphgendeps.d: $(symphgenallsources) makedeps.sh
	./makedeps.sh symphgen > symphgendeps.d

include wavelibdeps.d
include wavegendeps.d
include symphgendeps.d

