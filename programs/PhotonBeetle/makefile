CC = cc
CFLAGS = -O3 -Wall -Wextra

all: test

test: test.o photonbeetle128v1_struc.o photonbeetle128v1_orig.o photon.o
	$(CC) $(CFLAGS) $^ -o $@

photonbeetle128v1_orig.o: photonbeetle128v1_orig.c beetle.h photon.h
	$(CC) $(CFLAGS) -c $< -o $@

photon.o: photon.c photon.h
	$(CC) $(CFLAGS) -c $< -o $@

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm *.o