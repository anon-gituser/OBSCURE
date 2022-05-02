CC = cc
CFLAGS = -O3 -Wall -Wextra

all: test

test: test.o \
	crypto_hashblocks_sha512_struc.o \
	crypto_hashblocks_sha512.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm *.o test