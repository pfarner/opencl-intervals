CXXFLAGS = -g -ggdb -DCL_TARGET_OPENCL_VERSION=220
LDFLAGS  = -lOpenCL
CXXFILES = $(wildcard *.cc)
HEADERS  = $(wildcard *.h)
OBJECTS  = $(patsubst %.cc,%.o,$(CXXFILES))
SHELL    = /bin/zsh

%.o:	%.cc $(HEADERS)
	g++ -Wall $(CXXFLAGS) -c $<
	# -Werror 

main:	$(OBJECTS)
	g++ -Wall -Werror $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf main *~(N) $(OBJECTS)

test:
	echo CXXFILES=$(CXXFILES)
	echo OBJECTS=$(OBJECTS)
