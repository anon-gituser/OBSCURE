CC = cc
CFLAGS = -O3 -Wall -Wextra

all: test

test: 	test.o \
		fe25519.o \
		fe25519_struc.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm *.o test