CC = cc
CFLAGS = -O3 -Wall -Wextra

all: test

test: test.o ascon128v12_orig.o ascon128v12_struc.o
	$(CC) $(CFLAGS) $^ -o $@

ascon128v12_orig.o: ascon128v12_orig.c \
		ascon.h permutations.h round.h word.h api.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm *.o