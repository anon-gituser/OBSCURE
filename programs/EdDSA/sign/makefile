CC = cc
CFLAGS = -O3 -Wall -Wextra

all: test

test: 	test.o \
		crypto_sign_struc.o \
		crypto_sign_orig.o \
		randombytes.o \
		fe25519.o ge25519.o sc25519.o \
		crypto_verify_32.o \
		crypto_hashblocks_sha512.o \
		crypto_hash_sha512.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm *.o test