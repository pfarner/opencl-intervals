MAKEFLAGS += -r # to match bare % instead of using implicit rule
CXXFLAGS = -std=c++20 -g -ggdb -DCL_TARGET_OPENCL_VERSION=220 -I..
LDFLAGS  = -lOpenCL
CXXFILES = $(wildcard *.cc)
HEADERS  = $(wildcard *.h) $(wildcard opencl/*.h) $(wildcard intervals/*.h)
OBJECTS  = $(patsubst %.cc,%.o,$(CXXFILES))
EXECS    = $(patsubst %.cc,%,$(CXXFILES))
SHELL    = /bin/zsh

.PHONY: intervals

.PRECIOUS: $(OBJECTS)

all:	$(EXECS)

%.o:	%.cc ../opencl ../intervals $(HEADERS)
	g++ -Wall $(CXXFLAGS) -Werror  -c $<

%:	%.o $(OBJECTS) ../opencl/*.o ../intervals/*.o
	g++ -Wall -Werror $(CXXFLAGS) $(LDFLAGS) $< ../opencl/*.o ../intervals/*.o -o $@

intervals:
	make -C ../intervals

clean:
	make -C ../intervals clean
	rm -rf circle *~(N) $(OBJECTS) plot
