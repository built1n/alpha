SOURCES = exec.o alpha.o interface.o
HEADERS = alpha.h util.h
CFLAGS = -O3 -I. -g -Wall -std=gnu99
CXXFLAGS = -O3 -I. -g -Wall
all: $(SOURCES) $(HEADERS) Makefile
	g++ $(SOURCES) -O3 -o alpha -g
clean:
	rm -rf *.o
	rm -f alpha
	rm -f *~
	rm -f a.out
