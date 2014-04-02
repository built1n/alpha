SOURCES = exec.o alpha.o interface.o disasm.o
HEADERS = alpha.h util.h help.h memory.h
CFLAGS = -O3 -I. -g -Wall -std=gnu99 -fomit-frame-pointer
CXXFLAGS = -O3 -I. -g -Wall -fomit-frame-pointer
CC=gcc
CXX=g++
alpha: $(HEADERS) $(SOURCES) Makefile
	$(CXX) $(SOURCES) $(CXXFLAGS) -o alpha 
all: alpha alpha-disasm
clean:
	@rm -rf *.o
	@rm -f alpha alpha-disasm
	@rm -f *~
	@rm -f a.out
	@rm help.h
help.h: gen-help-string.cpp ../doc/SPEC
	@echo "Auto-generating help..."
	@g++ gen-help-string.cpp
	@cat ../doc/SPEC | ./a.out > help.h
	@rm a.out
	@echo "Done."
