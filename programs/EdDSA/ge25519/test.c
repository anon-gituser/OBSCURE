#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "../fe25519/fe25519_struc.h"
#include "../sc25519/sc25519_struc.h"
#include "ge25519.h"
#include "ge25519_struc.h"

/********************************************************************
 *                         utils
 ********************************************************************/

void printt(unsigned int x, unsigned int y, unsigned int z, char *msg)
{
  printf("%s = (%u, %u, %u)\n", msg, x, y, z);
}

void print_u32(unsigned int *s, unsigned int len, char *msg){
  printf("%s = ", msg);
  unsigned int i;
  for (i=0; i<len; i++){
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

void print_ge25519_bstr(unsigned int s[ge25519_U32LEN]){
  unsigned int i;
  printf("x = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s[i]                 ); printf("\n");
  printf("y = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s[i+  fe25519_U32LEN]); printf("\n");
  printf("z = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s[i+2*fe25519_U32LEN]); printf("\n");
  printf("t = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s[i+3*fe25519_U32LEN]); printf("\n");
  printf("\n");
}

void print_ge25519(ge25519 s){
  unsigned int i;
  printf("x = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s.x.v[i]); printf("\n");
  printf("y = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s.y.v[i]); printf("\n");
  printf("z = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s.z.v[i]); printf("\n");
  printf("t = "); for (i=0; i<fe25519_U32LEN;i++) printf("%u ", s.t.v[i]); printf("\n");
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

void u32_to_ge25519(ge25519 *r, unsigned int x[ge25519_U32LEN]){
  unsigned int i;
  for (i=0; i<fe25519_U32LEN; i++){
    r->x.v[i] = x[i];
    r->y.v[i] = x[i+fe25519_U32LEN];
    r->z.v[i] = x[i+2*fe25519_U32LEN];
    r->t.v[i] = x[i+3*fe25519_U32LEN];
  }
}

void u32_to_sc25519(sc25519 *r, unsigned int x[sc25519_U32LEN]){
  unsigned int i;
  for (i=0; i<sc25519_U32LEN; i++){
    r->v[i] = x[i];
  }
}

void cmp_rets(ge25519 r, unsigned int x[ge25519_U32LEN], char *s){
  if (   memcmp(x                 , r.x.v, fe25519_U32LEN) != 0
      || memcmp(x+  fe25519_U32LEN, r.y.v, fe25519_U32LEN) != 0
      || memcmp(x+2*fe25519_U32LEN, r.z.v, fe25519_U32LEN) != 0
      || memcmp(x+3*fe25519_U32LEN, r.t.v, fe25519_U32LEN) != 0)
  {
    printf("Failure: %s\n", s);
    printf("Returned: \n"); print_ge25519_bstr(x);
    printf("Expected: \n"); print_ge25519(r);
    exit(EXIT_FAILURE);
  }
}

void cmp_bytes(unsigned char *x1, unsigned int *x2, unsigned int len, char *msg){
  unsigned int i;
  for (i=0; i<len; i++){
    if (x1[i] != (unsigned char) x2[i]){
      printf("Failure: %s\n", msg);
      printf("Returned: "); print_ge25519_bstr(x2); // len 32 only
      printf("Expected: "); print_bstr(x1, len);
      exit(EXIT_FAILURE);
    }
  }
}

/********************************************************************
 *                         test functions
 ********************************************************************/
void test_ge25519_add()
{
  unsigned int  x2[ge25519_U32LEN],
                y2[ge25519_U32LEN],
                z2[ge25519_U32LEN];
  randomize_uint(x2, ge25519_U32LEN);
  randomize_uint(y2, ge25519_U32LEN);

  ge25519 x,y,z;
  u32_to_ge25519(&x, x2);
  u32_to_ge25519(&y, y2);

  // ge25519_add (including add_p1p1, p1p1_to_p3 and p1p1_to_p2)
  ge25519_add_struc(z2, x2, y2);
  ge25519_add(&z, &x, &y);
  cmp_rets(z, z2, "ge25519_add");
}

void test_ge25519_scalarmult()
{
  unsigned int  x2[ge25519_U32LEN],
                z2[ge25519_U32LEN];
  randomize_uint(x2, ge25519_U32LEN);

  ge25519 x,z;
  u32_to_ge25519(&x, x2);
  unsigned int a2[sc25519_U32LEN];
  randomize_bytes(a2, sc25519_U32LEN);
  sc25519 a;
  u32_to_sc25519(&a, a2);
  ge25519_scalarmult_struc(z2, x2, a2);
  ge25519_scalarmult(&z, &x, &a);
  cmp_rets(z, z2, "ge25519_scalarmult");
}

void test_ge25519_scalarmult_base()
{
  unsigned int  z2[ge25519_U32LEN];
  ge25519 z;

  unsigned int a2[sc25519_U32LEN];
  randomize_bytes(a2, sc25519_U32LEN);
  sc25519 a;
  u32_to_sc25519(&a, a2);
  ge25519_scalarmult_base_struc(z2, a2);
  ge25519_scalarmult_base(&z, &a);
  cmp_rets(z, z2, "ge25519_scalarmult_base");
}

void test_ge25519_pack()
{
  unsigned int i;
  unsigned int  x2[ge25519_U32LEN];
  randomize_uint(x2, ge25519_U32LEN);

  ge25519 x;
  u32_to_ge25519(&x, x2);
  unsigned int b2[32];
  randomize_bytes(b2, 32);
  unsigned char b[32];
  for(i=0;i<32;i++) b[i] = (unsigned char) b2[i];
  ge25519_pack_struc(b2, x2);
  ge25519_pack(b, &x);
  cmp_bytes(b, b2, 32, "ge25519_pack");
}

/********************************************************************
 *                         main
 ********************************************************************/
int main(){
  srand(time(NULL));
  test_ge25519_add();
  test_ge25519_scalarmult();
  test_ge25519_scalarmult_base();
  test_ge25519_pack();

  printf("OK\n");
  return 0;
}