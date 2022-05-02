#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "word.h"

#define KEY_LEN 16
#define NONCE_LEN 16
#define MAC_LEN 16
#define N_TEST 1000

/********* Prototypes *********/
int crypto_aead_encrypt_struc(
          unsigned long long* c,
    const unsigned long long* m,
    const unsigned long long* ad,
    const unsigned long long* npub,
    const unsigned long long* k);

int crypto_aead_decrypt_struc(
          unsigned long long* m,
    const unsigned long long* c,
    const unsigned long long* ad,
    const unsigned long long* npub,
    const unsigned long long* k);

int crypto_aead_encrypt(
    unsigned char *c, unsigned long long *clen,
    const unsigned char *m, unsigned long long mlen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *nsec,
    const unsigned char *npub,
    const unsigned char *k);

int crypto_aead_decrypt(
    unsigned char *m, unsigned long long *mlen,
    unsigned char *nsec,
    const unsigned char *c, unsigned long long clen,
    const unsigned char *ad, unsigned long long adlen,
    const unsigned char *npub,
    const unsigned char *k);

static void random_uchar(unsigned char *str, unsigned long long len);
static void bytes_to_uint64(unsigned long long *out, unsigned char *bytes,
                            unsigned long long bytelen);
static void uint64_to_bytes(unsigned char *out, unsigned long long *llus,
                            unsigned long long llulen);
void fprint_bstr_orig(unsigned char *str, unsigned long long len);
void fprint_bstr_struc(unsigned long long *str, unsigned long long len);
/******************************/

/**
 * REMARKS
 * 1. In the restructured version, `mlen` and `adlen` must be
 * multiple-of-8 numbers. This is because the bytetrings of the
 * `m` and `ad` are converted to the arrays of unsigned long long
 * (unsigned long long).
 * All computations on bytes in the original version
 * are transformed to computations on unsigned long long.
 *
 * 2. In the restructured version, `mlen` and `adlen` are hardcoded
 * in the functions of encryption and decryption. These two variables
 * are treated as two constants when being obfuscated.
 * If you want to test with other values of `mlen` and `adlen`,
 * ensure that the values below and those in the restructured version
 * are consistant.
 *
 * 3. NONCE_LEN, KEY_LEN and MAC_LEN are fixed to be 16 bytes for each
 * by default of the original version.
 */

int main()
{
  srand(time(NULL));

  // For original version
  unsigned long long mlen = 16, mlen1,
                     clen = mlen + MAC_LEN,
                     adlen = 16;
  unsigned char m[mlen], m1[mlen],
                c[clen],
                ad[adlen],
                nonce[NONCE_LEN],
                key[KEY_LEN];

  // For restructured version
  unsigned long long  mlen2 = mlen/8,
            clen2 = clen/8,
            adlen2= adlen/8;
  unsigned long long  m2[mlen2],
            c2[clen2],
            ad2[adlen2],
            nonce2[NONCE_LEN],
            key2[KEY_LEN];
  unsigned char c3[clen], m3[mlen];

  for (int i = 0; i < N_TEST; i++)
  {
    if (i % 100 == 0)
    {
      printf("\rTest %7d/%d", i, N_TEST);
      fflush(stdout);
    }
    // Randomize m, ad, nonce, key
    random_uchar(m, mlen);
    random_uchar(ad, adlen);
    random_uchar(nonce, NONCE_LEN);
    random_uchar(key, KEY_LEN);

    // Convert m, ad, nonce, key to uint64
    bytes_to_uint64(m2, m, mlen);
    bytes_to_uint64(ad2, ad, adlen);
    bytes_to_uint64(nonce2, nonce, NONCE_LEN);
    bytes_to_uint64(key2, key, KEY_LEN);

    // Encrypt
    crypto_aead_encrypt(c, &clen, m, mlen, ad, adlen, NULL, nonce, key);
    crypto_aead_encrypt_struc(c2, m2, ad2, nonce2, key2);

    // Convert c2 from uint64 to bytes
    uint64_to_bytes(c3, c2, clen2);

    // Check if c = c3
    if (memcmp(c, c3, clen) != 0){
      printf("\nError: encryption failed.\n");
      printf("----------------------------\n");
      printf("PT (Original)    : "); fprint_bstr_orig(m, mlen);
      printf("PT (Restructured): "); fprint_bstr_struc(m2, mlen2);
      printf("CT (Original)    : "); fprint_bstr_orig(c, clen);
      printf("CT (Restructured): "); fprint_bstr_struc(c2, clen2);
      printf("----------------------------\n");
      exit(EXIT_FAILURE);
    }

    // Decrypt
    int ret, ret2;
    ret = crypto_aead_decrypt(m1, &mlen1, NULL, c, clen, ad, adlen, nonce, key);
    ret2 = crypto_aead_decrypt_struc(m2, c2, ad2, nonce2, key2);
    if (ret != 0 || ret2 != 0){
      printf("\nError: Decryption.....MAC Failed!\n");
      exit(EXIT_FAILURE);
    }

    // Convert m2 from uint64 to bytes
    uint64_to_bytes(m3, m2, mlen2);

    // Check if m = m3
    if (mlen != mlen1 ||
        memcmp(m, m1, mlen) != 0 || memcmp(m, m3, mlen) != 0)
    {
      printf("\nError: Decryption.....Failed\n");
    }
  }
  printf("\rTest %d/%d ===> all tests correct.\n", N_TEST, N_TEST);

  return 0;
}

static void random_uchar(unsigned char *str, unsigned long long len)
{
  unsigned long long i;
  for (i = 0; i < len; i++)
  {
    str[i] = rand() % 256;
  }
}

static void uint64_to_bytes(unsigned char *out, unsigned long long *llus,
                            unsigned long long llulen)
{
  unsigned long long i;
  for (i = 0; i < llulen; i++){
    STOREBYTES(&out[i*8], llus[i], 8);
  }
}

static void bytes_to_uint64(unsigned long long *out, unsigned char *bytes,
                            unsigned long long bytelen)
{
  if (bytelen % 8 != 0){
    printf("\nError: Length must be multiple-of-8!\n");
    exit(EXIT_FAILURE);
  }
  unsigned long long i;
  for (i = 0; i < bytelen; i += 8){
    out[i/8] = LOADBYTES(bytes + i, 8);
  }
}

void fprint_bstr_orig(unsigned char *str, unsigned long long len)
{
  unsigned long long i;
  for (i = 0; i < len; i++)
  {
    fprintf(stdout, "%02X", str[i]);
  }
  fprintf(stdout, "\n");
}

void fprint_bstr_struc(unsigned long long *str, unsigned long long len)
{
  unsigned long long i;
  unsigned char *bstr = (unsigned char *) str;
  for (i = 0; i < len * 8; i++)
  {
    fprintf(stdout, "%02X", bstr[i]);
  }
  fprintf(stdout, "\n");
}
