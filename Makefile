# Build src/*.cc -> bin/*.exe, linked against ROOT.
#
# Usage:
#   make          # build everything in src/
#   make clean    # remove built executables
#
# Requirement: ROOT environment must be sourced first, e.g.
#   . /raid5/root/root-v6.34.04/root/bin/thisroot.sh

SHELL := /bin/bash

ROOTCFLAGS := $(shell root-config --cflags)
ROOTLIBS   := $(shell root-config --libs)

CXX        := g++
CXXFLAGS   := -O2 -Wall -Wextra -std=c++17 -Iinclude $(ROOTCFLAGS)
LDFLAGS    := $(ROOTLIBS)

SRC := $(wildcard src/*.cc)
BIN := $(patsubst src/%.cc,bin/%.exe,$(SRC))

all: $(BIN)

bin/%.exe: src/%.cc include/variables.h | bin
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

bin:
	mkdir -p bin

clean:
	rm -f bin/*.exe

.PHONY: all clean
