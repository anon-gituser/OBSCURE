#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "sc25519.h"
#include "sc25519_struc.h"

// Utils
void print_sc25519_bstr(unsigned int s[sc25519_U32LEN]){
  unsigned int i;
  for (i=0; i<sc25519_U32LEN; i++){
    printf("%u ", s[i]);
  }
  printf("\n");
}

void print_bstr(unsigned char *s, unsigned int len){
  unsigned int i;
  for (i=0; i<len; i++){
    printf("%u ", s[i]);
  }
  printf("\n");
}

void print_sc25519(sc25519 *s){
  unsigned int i;
  for (i=0; i<sc25519_U32LEN;i++){
    printf("%u ", s->v[i]);
  }
  printf("\n");
}

void randomize_uint(unsigned int *s, unsigned int len){
  unsigned int i;
  for (i=0; i<len; i++){
    s[i] = rand();
  }
}

void randomize_bytes(unsigned int *s, unsigned int len){
  unsigned int i;
  for (i=0; i<len; i++){
    s[i] = rand() % 256;
  }
}

void u32_to_sc25519(sc25519 *r, unsigned int x[sc25519_U32LEN]){
  unsigned int i;
  for (i=0; i<sc25519_U32LEN; i++){
    r->v[i] = x[i];
  }
}

void cmp_rets(sc25519 r, unsigned int x[sc25519_U32LEN], char *msg){
  if (memcmp(x, r.v, sc25519_U32LEN) != 0){
    printf("Failure: %s\n", msg);
    printf("Returned: "); print_sc25519_bstr(x);
    printf("Expected: "); print_sc25519_bstr(r.v);
    exit(EXIT_FAILURE);
  }
}

void cmp_bytes(unsigned char *x1, unsigned int *x2, unsigned int len, char *msg){
  unsigned int i;
  for (i=0; i<len; i++){
    if (x1[i] != (unsigned char) x2[i]){
      printf("Failure: %s\n", msg);
      printf("Returned: "); print_sc25519_bstr(x2); // len 32 only
      printf("Expected: "); print_bstr(x1, len);
      exit(EXIT_FAILURE);
    }
  }
}

int main(){
  srand(time(NULL));
  unsigned int i;

  unsigned int  x2[sc25519_U32LEN],
                y2[sc25519_U32LEN],
                z2[sc25519_U32LEN];
  randomize_uint(x2, sc25519_U32LEN);
  randomize_uint(y2, sc25519_U32LEN);
  randomize_uint(z2, sc25519_U32LEN);

  sc25519 x,y,z;
  u32_to_sc25519(&x, x2);
  u32_to_sc25519(&y, y2);
  u32_to_sc25519(&z, z2);

  // sc25519_add
  sc25519_add_struc(z2, x2, y2);
  sc25519_add(&z, &x, &y);
  cmp_rets(z, z2, "sc25519_add");

  // sc25519_mul
  sc25519_mul_struc(z2, x2, y2);
  sc25519_mul(&z, &x, &y);
  cmp_rets(z, z2, "sc25519_mul");

  // sc25519_from32bytes
  unsigned int a2[32];
  unsigned char a[32];
  randomize_bytes(a2, 32);
  for (i=0; i<32; i++) a[i] = (unsigned char) a2[i];
  sc25519_from32bytes_struc(z2, a2);
  sc25519_from32bytes(&z, a);
  cmp_rets(z, z2, "sc25519_from32bytes");

  // sc25519_from64bytes
  unsigned int b2[64];
  unsigned char b[64];
  randomize_bytes(b2, 64);
  for (i=0; i<64; i++) b[i] = (unsigned char) b2[i];
  sc25519_from64bytes_struc(z2, b2);
  sc25519_from64bytes(&z, b);
  cmp_rets(z, z2, "sc25519_from64bytes");

  sc25519_to32bytes_struc(a2, x2);
  sc25519_to32bytes(a, &x);


  printf("OK\n");
  return 0;
}