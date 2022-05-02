CC = cc
CFLAGS = -O3 -Wall -Wextra

HASHBLOCKS_FOLDER = ../hashblocks

all: test

test: 	test.o \
		crypto_hash_sha512.o \
		crypto_hash_sha512_struc.o \
		$(HASHBLOCKS_FOLDER)/crypto_hashblocks_sha512.o \
		$(HASHBLOCKS_FOLDER)/crypto_hashblocks_sha512_struc.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm *.o test