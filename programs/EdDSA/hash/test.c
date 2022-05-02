#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "crypto_hash_sha512.h"
#include "crypto_hash_sha512_struc.h"

#define N_TESTS 10

void bytes_to_u32(unsigned int *r, const unsigned char *in, unsigned int inlen)
{
  if (inlen % 4 != 0){
    printf("Invalid inlen!\n");
    exit(EXIT_FAILURE);
  }
  unsigned int i;
  for(i=0; i<inlen; i+=4){
    r[i/4] = 0;
    r[i/4] |= (in[i  ] << 24);
    r[i/4] |= (in[i+1] << 16);
    r[i/4] |= (in[i+2] <<  8);
    r[i/4] |= (in[i+3]      );
  }
}

void test_hash()
{
  unsigned int i;
  // inlen = 24 u32 or 96 bytes only
  unsigned long long inlen = 24 * 4;
  unsigned char in[inlen], out[64];
  unsigned int xinlen = inlen/4;
  unsigned int xin[xinlen], xout[16];

  for(i=0; i<inlen; i++) in[i] = rand() % 256;
  bytes_to_u32(xin, in, inlen);

  crypto_hash_sha512_struc(xout, xin);
  crypto_hash_sha512(out, in, inlen);

  unsigned int rout[16];
  bytes_to_u32(rout, out, 64);


  if (memcmp(rout, xout, 16) == 0){
    printf("OK\n");
  }
  else {
    printf("Failed!\n");
    printf("Input   : "); for(i=0; i<inlen; i++) printf("%02x", in[i]); printf("\n");
    printf("Expected: "); for(i=0; i<16; i++) printf("%u, ", rout[i]); printf("\n");
    printf("Returned: "); for(i=0; i<16; i++) printf("%u, ", xout[i]); printf("\n");
  }
}

int main()
{
  srand(time(NULL));
  unsigned int t;
  for(t=0; t<N_TESTS; t++){
    printf("Test %u: ", t);
    test_hash();
  }
  return 0;
}