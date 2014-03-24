SOURCES = exec.o alpha.o interface.o disasm.o
SOURCES_DISASM = exec.o alpha.o interface-disasm.o disasm.o
HEADERS = alpha.h util.h
CFLAGS = -O3 -I. -g -Wall -std=gnu99 -fomit-frame-pointer
CXXFLAGS = -O3 -I. -g -Wall -fomit-frame-pointer
alpha: $(SOURCES) $(HEADERS) Makefile
	g++ $(SOURCES) $(CXXFLAGS) -o alpha 
alpha-disasm: $(SOURCES_DISASM) $(HEADERS) Makefile
	g++ $(SOURCES_DISASM) -O3 -o alpha-disasm -g
all: alpha alpha-disasm
clean:
	@rm -rf *.o
	@rm -f alpha alpha-disasm
	@rm -f *~
	@rm -f a.out
library:
	g++ $(SOURCES) -O3 -o libalpha.so.1.0.0 -shared
