CC       = gcc
CFLAGS   = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2
LDFLAGS  = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

.PHONY: all clean

all: libnand.so example_static example_dynamic testy

vvector.o: vvector.c vvector.h utils.h
memory_tests.o: memory_tests.c memory_tests.h
nand.o: nand.c nand.h utils.h vvector.h vvector.c

libnand.so: vvector.o memory_tests.o nand.o
	gcc ${LDFLAGS} -o $@ $^

clean:
	rm -rf *.o *.so
