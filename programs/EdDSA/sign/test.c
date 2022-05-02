#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "api.h"
#include "randombytes.h"
#include "crypto_sign_orig.h"

#define MAXTEST_BYTES 32

/********************************************************************/
/*                        PROTOTYPES                                */
/********************************************************************/
unsigned int crypto_sign_struc(
          unsigned int *sm,
    const unsigned int *m,
    const unsigned int *sk);

/********************************************************************/
/*                        UTILS                                     */
/********************************************************************/
void random_uchar(unsigned char *str, unsigned long long len)
{
  unsigned long long i;
  for (i = 0; i < len; i++)
  {
    str[i] = rand() % 256;
  }
}

void load_bigendian(unsigned int *r, const unsigned char *x, unsigned long long bytelen)
{
  if (bytelen % 4 != 0){
    printf("Invalid length!\n");
    exit(EXIT_FAILURE);
  }
  unsigned long long i;
  for(i=0; i<bytelen/4; i++){
    r[i] =(x[i*4  ] << 24) |
          (x[i*4+1] << 16) |
          (x[i*4+2] <<  8) |
          (x[i*4+3]      ) ;
  }
}

void store_bigendian(unsigned char *r, const unsigned int *x, unsigned long long bytelen)
{
  if (bytelen % 4 != 0){
    printf("Invalid length!\n");
    exit(EXIT_FAILURE);
  }
  unsigned long long i;
  for(i=0; i<bytelen/4; i++){
    r[i*4  ] = x[i] >> 24;
    r[i*4+1] = x[i] >> 16;
    r[i*4+2] = x[i] >>  8;
    r[i*4+3] = x[i]      ;
  }
}

void printbytes(unsigned char *str, unsigned long long len)
{
  unsigned long long i;
  for (i = 0; i < len; i++)
  {
    fprintf(stdout, "%02X", str[i]);
  }
  fprintf(stdout, "\n");
}

void fprint_bstr_struc(unsigned int *str, unsigned int len)
{
  unsigned int i;
  unsigned char *ptr = (unsigned char *)str;
  for (i = 0; i < len * 4; i++)
  {
    fprintf(stdout, "%02X", ptr[i]);
  }
  fprintf(stdout, "\n");
}

/* ********************************************************************
 *                        MAIN
 **********************************************************************/
int main()
{
  unsigned char sk[CRYPTO_SECRETKEYBYTES],
                pk[CRYPTO_PUBLICKEYBYTES];

  // mlen must be fixed to 64 bytes
  unsigned long long mlen = 64;
  unsigned long long mlen1;
  unsigned long long smlen = 0;
  unsigned char m[mlen];
  unsigned char m1[mlen];
  unsigned char sm[mlen + CRYPTO_BYTES];

  unsigned int mlen2 = mlen/4; // 16
  unsigned int smlen2 = mlen2 + 16;
  unsigned int m2[mlen2];
  unsigned int sm2[smlen2];

  unsigned int sk2[16];

  randombytes(m, mlen);
  crypto_sign_keypair(pk, sk);

  load_bigendian(sk2, sk, 64);
  load_bigendian(m2, m, mlen);

  crypto_sign_orig(sm, &smlen, m, mlen, sk);
  crypto_sign_struc(sm2, m2, sk2);

  unsigned char sm1[smlen];
  store_bigendian(sm1, sm2, smlen);

  if ((smlen != smlen2*4) || (memcmp(sm, sm1, smlen) != 0))
  {
    printf("Sign Failed!\n");
    printf("sk       : ");
    printbytes(sk, 64);
    printf("pk       : ");
    printbytes(pk, 32);
    printf("m        : ");
    printbytes(m, mlen);
    printf("sm_orig  : ");
    printbytes(sm, smlen);
    printf("sm_struc : ");
    printbytes(sm1, smlen);
    exit(EXIT_FAILURE);
  }

  int ret;
  ret = crypto_sign_open_orig(m1, &mlen1, sm, smlen, pk);
  if (ret != 0 || mlen != mlen1 || memcmp(m1, m, mlen) != 0)
  {
    printf("Verify Failed (Original Version)!\n");
    printf("sk       : ");
    printbytes(sk, 32);
    printf("pk       : ");
    printbytes(pk, 32);
    printf("m        : ");
    printbytes(m, mlen);
    printf("sm_orig  : ");
    printbytes(sm, smlen);
    exit(EXIT_FAILURE);
  }

  printf("OK!\n");
  return 0;
}
