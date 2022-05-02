CC = cc
CFLAGS = -O3 -Wall -Wextra
SC_FOLDER = ../sc25519
FE_FOLDER = ../fe25519

all: test

test: 	test.o \
		ge25519.o \
		ge25519_struc.o \
		$(SC_FOLDER)/sc25519.o \
		$(SC_FOLDER)/sc25519_struc.o \
		$(FE_FOLDER)/fe25519.o \
		$(FE_FOLDER)/fe25519_struc.o
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

clean:
	rm *.o test