CC := gcc
RM := rm -f

CFLAGS := -Wall -pedantic -I. -pthread -lm
OBJSe := cola.o
DEPSe := cola.h

all: contencionEspera base exclusionMutua

exclusionMutua: $(OBJSe) exclusionMutua.c $(DEPSe)
	$(CC) exclusionMutua.c cola.o -o exclusionMutua $(CFLAGS)


contencionEspera: $(OBJS) contencionEspera.c $(DEPS)
	$(CC) contencionEspera.c -o contencionEspera $(CFLAGS)

base: $(OBJS) base.c $(DEPS)
	$(CC) base.c -o base $(CFLAGS)

%.o: %.c $(DEPSe)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) base
