CC := gcc
RM := rm -f

CFLAGS := -Wall -pedantic -I. -pthread -lm
OBJSe := cola.o
DEPSe := cola.h

all: contencionEspera noApropiativa exclusionMutua esperaCircular

exclusionMutua: $(OBJSe) exclusionMutua.c $(DEPSe)
	$(CC) exclusionMutua.c cola.o -o exclusionMutua $(CFLAGS)

noApropiativa: $(OBJS) noApropiativa.c $(DEPS)
	$(CC) noApropiativa.c -o noApropiativa $(CFLAGS)

esperaCircular: $(OBJS) esperaCircular.c $(DEPS)
	$(CC) esperaCircular.c -o esperaCircular $(CFLAGS)

contencionEspera: $(OBJS) contencionEspera.c $(DEPS)
	$(CC) contencionEspera.c -o contencionEspera $(CFLAGS)

%.o: %.c $(DEPSe)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) cola.o contencionEspera noApropiativa exclusionMutua esperaCircular
