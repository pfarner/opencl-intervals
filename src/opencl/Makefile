CXXFLAGS = -std=c++20 -g -ggdb -DCL_TARGET_OPENCL_VERSION=220
LDFLAGS  = -lOpenCL
CXXFILES = $(wildcard *.cc)
HEADERS  = $(wildcard *.h)
OBJECTS  = $(patsubst %.cc,%.o,$(CXXFILES))
SHELL    = /bin/zsh

%.o:	%.cc $(HEADERS)
	g++ -Wall $(CXXFLAGS) -Werror -c $<

all:	$(OBJECTS)
	@make test

clean:
	rm -rf *~(N) $(OBJECTS)

test:	test-gpu

test-gpu:
	@if ! yes -- -1 | python /home/presto/git/oclInspector/inspector.py | grep -q NVIDIA; then echo NO GPU FOUND; exit 1; else echo GPU found; fi
