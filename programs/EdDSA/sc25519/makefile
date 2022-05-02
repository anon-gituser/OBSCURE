CC = cc
CFLAGS = -O3 -Wall -Wextra

all: test

test: 	test.o \
		sc25519.o \
		sc25519_struc.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm *.o test