SOURCES = exec.o alpha.o disasm.o
HEADERS = alpha.h util.h help.h mem.h
CFLAGS = -O3 -I. -g -Wall -std=gnu99 -fomit-frame-pointer
CXXFLAGS = -O3 -I. -g -Wall -fomit-frame-pointer
CC=gcc
CXX=g++
alpha: $(HEADERS) $(SOURCES) Makefile interface.o
	$(CXX) $(SOURCES) interface.o $(CXXFLAGS) -o alpha
all: alpha alpha-disasm
clean:
	@rm -rf *.o
	@rm -f alpha
	@rm -f *~
	@rm -f a.out
	@rm -f help.h
help.h: gen-help-string.cpp ../doc/SPEC
	@echo "Auto-generating help..."
	@g++ gen-help-string.cpp
	@cat ../doc/SPEC | ./a.out > help.h
	@rm a.out
	@echo "Done."
library: $(HEADERS) $(SOURCES) Makefile
	$(CXX) $(SOURCES) -shared -O3 -o libalpha.so
alpha-dynamic: library interface.o Makefile
	$(CXX) interface.o -O3 -o alpha -lalpha
install: alpha library
	sudo cp alpha /usr/local/bin
	sudo cp libalpha.so /usr/local/lib
