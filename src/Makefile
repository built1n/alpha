SOURCES = exec.o alpha.o disasm.o
HEADERS = alpha.h util.h help.h mem.h
CFLAGS = -O3 -I. -g -Wall -std=gnu99 -fomit-frame-pointer -fPIC
CXXFLAGS = -O3 -I. -g -Wall -fomit-frame-pointer
CC=gcc
CXX=g++
alpha: $(HEADERS) $(SOURCES) interface.o Makefile
	$(CXX) $(SOURCES) $(CXXFLAGS) interface.o -o alpha 
alpha-dynamic: $(HEADERS) $(SOURCES) interface.o Makefile library
	$(CXX) $(SOURCES) $(CXXFLAGS) interface.o -o alpha -lalpha
all: alpha
clean:
	@rm -rf *.o
	@rm -f alpha
	@rm -f *~
	@rm -f a.out
	@rm -f help.h
	@rm -f libalpha.so
help.h: gen-help-string.cpp ../doc/SPEC
	@echo "Auto-generating help..."
	@g++ gen-help-string.cpp
	@cat ../doc/SPEC | ./a.out > help.h
	@rm a.out
	@echo "Done."
library: $(SOURCES) $(HEADERS) Makefile
	$(CXX) $(SOURCES) $(CXXFLAGS) -o libalpha.so -shared -O3 
install: alpha library
	cp libalpha.so /usr/local/lib
	cp alpha /usr/local/bin
