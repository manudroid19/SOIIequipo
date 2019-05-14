CC := gcc
RM := rm -f

CFLAGS := -Wall -pedantic -I. -pthread -lm
OBJS :=
DEPS :=

all: contencionEspera base

contencionEspera: $(OBJS) contencionEspera.c $(DEPS)
	$(CC) contencionEspera.c -o contencionEspera $(CFLAGS)

base: $(OBJS) base.c $(DEPS)
	$(CC) base.c -o base $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) base
