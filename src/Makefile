SOURCES = exec.o alpha.o interface.o
HEADERS = alpha.h util.h
CFLAGS = -O3 -I. -g -Wall -std=gnu99
CXXFLAGS = -O3 -I. -g -Wall
alpha-disasm:
	cd disasm;make;cp alpha-disasm ..
all: $(SOURCES) $(HEADERS) Makefile alpha-disasm
	g++ $(SOURCES) -O3 -o alpha -g
clean:
	rm -rf *.o
	rm -f alpha
	rm -f *~
	rm -f a.out
	cd disasm;make clean;
