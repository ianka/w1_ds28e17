.PHONY: all clean

CC=gcc
CFLAGS+=-std=c99

all: ds7505-configure ds7505-readconfig ds7505-readtemp ds7505-recall ds7505-reset ds7505-store

clean:
	-rm *.o ds7505-configure ds7505-readconfig ds7505-readtemp ds7505-recall ds7505-reset ds7505-store

ds7505-configure: ds7505-configure.o
	$(CC) -o $@ $^

ds7505-readconfig: ds7505-readconfig.o
	$(CC) -o $@ $^

ds7505-readtemp: ds7505-readtemp.o
	$(CC) -o $@ $^

ds7505-recall: ds7505-recall.o
	$(CC) -o $@ $^

ds7505-reset: ds7505-reset.o
	$(CC) -o $@ $^

ds7505-store: ds7505-store.o
	$(CC) -o $@ $^

