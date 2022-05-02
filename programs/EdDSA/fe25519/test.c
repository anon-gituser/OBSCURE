#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "fe25519.h"
#include "fe25519_struc.h"

/********************************************************************
 *                         utils
 ********************************************************************/
void print_fe25519_bstr(unsigned int s[fe25519_U32LEN]){
  unsigned int i;
  for (i=0; i<fe25519_U32LEN; i++){
    printf("%u ", s[i]);
  }
  printf("\n");
}

void print_fe25519(fe25519 *s){
  unsigned int i;
  for (i=0; i<fe25519_U32LEN;i++){
    printf("%u ", s->v[i]);
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

void u32_to_fe25519(fe25519 *r, unsigned int x[fe25519_U32LEN]){
  unsigned int i;
  for (i=0; i<fe25519_U32LEN; i++){
    r->v[i] = x[i];
  }
}

void cmp_rets(fe25519 r, unsigned int x[fe25519_U32LEN], char *msg){
  printf("%s: ", msg);
  if (memcmp(x, r.v, fe25519_U32LEN) != 0){
    printf("Failed\n");
    printf("Returned: "); print_fe25519_bstr(x);
    printf("Expected: "); print_fe25519_bstr(r.v);
    exit(EXIT_FAILURE);
  }
  printf("OK\n");
}

void cmp_bytes(unsigned char *x1, unsigned int *x2, unsigned int len, char *msg){
  unsigned int i;
  printf("%s: ", msg);
  for (i=0; i<len; i++){
    if (x1[i] != (unsigned char) x2[i]){
      printf("Failed\n");
      printf("Returned: "); print_fe25519_bstr(x2); // len 32 only
      printf("Expected: "); print_bstr(x1, len);
      exit(EXIT_FAILURE);
    }
  }
  printf("OK\n");
}

/********************************************************************
 *                         test functions
 ********************************************************************/
void test_fe25519_cmov()
{
  unsigned int  x2[fe25519_U32LEN],
                y2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);
  randomize_uint(y2, fe25519_U32LEN);
  randomize_uint(z2, fe25519_U32LEN);

  fe25519 x,y,z;
  u32_to_fe25519(&x, x2);
  u32_to_fe25519(&y, y2);
  u32_to_fe25519(&z, z2);

  fe25519_cmov_struc(z2, x2, 0);
  fe25519_cmov(&z, &x, 0);
  cmp_rets(z, z2, "fe25519_cmov");
}

void test_fe25519_add()
{
  unsigned int  x2[fe25519_U32LEN],
                y2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);
  randomize_uint(y2, fe25519_U32LEN);

  fe25519 x,y,z;
  u32_to_fe25519(&x, x2);
  u32_to_fe25519(&y, y2);

  fe25519_add_struc(z2, x2, y2);
  fe25519_add(&z, &x, &y);
  cmp_rets(z, z2, "fe25519_add");
}

void test_fe25519_sub()
{
  unsigned int  x2[fe25519_U32LEN],
                y2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);
  randomize_uint(y2, fe25519_U32LEN);

  fe25519 x,y,z;
  u32_to_fe25519(&x, x2);
  u32_to_fe25519(&y, y2);

  fe25519_sub_struc(z2, x2, y2);
  fe25519_sub(&z, &x, &y);
  cmp_rets(z, z2, "fe25519_sub");
}

void test_fe25519_mul()
{
  unsigned int  x2[fe25519_U32LEN],
                y2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);
  randomize_uint(y2, fe25519_U32LEN);

  fe25519 x,y,z;
  u32_to_fe25519(&x, x2);
  u32_to_fe25519(&y, y2);

  fe25519_mul_struc(z2, x2, y2);
  fe25519_mul(&z, &x, &y);
  cmp_rets(z, z2, "fe25519_mul");
}

void test_fe25519_setone()
{
  unsigned int z2[fe25519_U32LEN];
  fe25519 z;

  fe25519_setone_struc(z2);
  fe25519_setone(&z);
  cmp_rets(z, z2, "fe25519_setone");
}

void test_fe25519_setzero()
{
  unsigned int z2[fe25519_U32LEN];
  fe25519 z;

  fe25519_setzero_struc(z2);
  fe25519_setzero(&z);
  cmp_rets(z, z2, "fe25519_setzero");
}

void test_fe25519_neg()
{
  unsigned int  x2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);

  fe25519 x,z;
  u32_to_fe25519(&x, x2);

  fe25519_neg_struc(z2, x2);
  fe25519_neg(&z, &x);
  cmp_rets(z, z2, "fe25519_neg");
}

void test_fe25519_square()
{
  unsigned int  x2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);

  fe25519 x,z;
  u32_to_fe25519(&x, x2);

  fe25519_square_struc(z2, x2);
  fe25519_square(&z, &x);
  cmp_rets(z, z2, "fe25519_square");
}

void test_fe25519_invert()
{
  unsigned int  x2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);

  fe25519 x,z;
  u32_to_fe25519(&x, x2);

  fe25519_invert_struc(z2, x2);
  fe25519_invert(&z, &x);
  cmp_rets(z, z2, "fe25519_invert");
}

void test_fe25519_getparity()
{
  unsigned int  x2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);

  fe25519 x;
  u32_to_fe25519(&x, x2);

  unsigned int pa2 = fe25519_getparity_struc(x2);
  unsigned int pa  = fe25519_getparity(&x);
  if (pa != pa2){
    printf("Failure: fe25519_getparity\n");
    printf("Returned: %u\n", pa2);
    printf("Expected: %u\n", pa);
    exit(EXIT_FAILURE);
  }
}

void test_fe25519_unpack()
{
  unsigned int z2[fe25519_U32LEN];
  fe25519 z;

  unsigned int a2[32];
  unsigned char a[32];
  randomize_bytes(a2, 32);

  unsigned int i;
  for (i=0; i<32; i++) a[i] = (unsigned int) a2[i];

  fe25519_unpack_struc(z2, a2);
  fe25519_unpack(&z, a);
  cmp_rets(z, z2, "fe25519_unpack");
}

void test_fe25519_pack()
{
  unsigned int  x2[fe25519_U32LEN],
                z2[fe25519_U32LEN];
  randomize_uint(x2, fe25519_U32LEN);

  fe25519 x;
  u32_to_fe25519(&x, x2);

  unsigned char a[32];
  fe25519_pack_struc(z2, x2);
  fe25519_pack(a, &x);
  cmp_bytes(a, z2, 32, "fe25519_pack");
}

#define LT(a, b) (((((a - b) ^ b) & (a ^ b)) ^ (a - b)) >> 31)
#define EQ(a, b) (1^LT(a, b)) & (1^LT(b, a))
void test_eq()
{
  unsigned int a = rand();
  unsigned int b = rand();
  unsigned int c1 = (a == b);
  unsigned int c2 = EQ(a, b);

  printf("test_eq: ");
  if (c1 != c2){
    printf("Failed\n");
    printf("a = %u\n", a);
    printf("b = %u\n", b);
    printf("Expected: %u\n", c1);
    printf("Returned: %u\n", c2);
    exit(EXIT_FAILURE);
  }
  printf("OK\n");
}

#define LE(a, b) (1^LT(b, a))
void test_le()
{
  unsigned int a = rand();
  unsigned int b = rand();
  unsigned int c1 = (a <= b);
  unsigned int c2 = LE(a, b);

  printf("test_le: ");
  if (c1 != c2){
    printf("Failed\n");
    printf("a = %u\n", a);
    printf("b = %u\n", b);
    printf("Expected: %u\n", c1);
    printf("Returned: %u\n", c2);
    exit(EXIT_FAILURE);
  }
  printf("OK\n");
}


/********************************************************************
 *                         main
 ********************************************************************/
int main(){
  srand(time(NULL));

  test_fe25519_cmov();
  test_fe25519_add();
  test_fe25519_sub();
  test_fe25519_mul();
  test_fe25519_setone();
  test_fe25519_setzero();
  test_fe25519_neg();
  test_fe25519_square();
  test_fe25519_invert();
  test_fe25519_getparity();
  test_fe25519_unpack();
  test_fe25519_pack();
  test_eq();
  test_le();

  return 0;
}