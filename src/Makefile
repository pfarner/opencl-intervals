MAKEFLAGS += -r # to match bare % instead of using implicit rule
CXXFLAGS = -std=c++20 -g -ggdb -DCL_TARGET_OPENCL_VERSION=220
LDFLAGS  = -lOpenCL
CXXFILES = $(wildcard *.cc)
HEADERS  = $(wildcard *.h) $(wildcard opencl/*.h)
OBJECTS  = $(patsubst %.cc,%.o,$(CXXFILES))
EXECS    = $(patsubst %.cc,%,$(CXXFILES))
SHELL    = /bin/zsh

.PHONY: opencl

.PRECIOUS: $(OBJECTS)

all:	$(EXECS)

%.o:	%.cc opencl $(HEADERS)
	g++ -Wall $(CXXFLAGS) -Werror  -c $<

%:	%.o $(OBJECTS) opencl
	g++ -Wall -Werror $(CXXFLAGS) $(LDFLAGS) $< opencl/*.o subdivider.o -o $@

opencl:
	make -C opencl

clean:
	make -C opencl clean
	rm -rf main *~(N) $(OBJECTS)
