#include "../hashblocks/crypto_hashblocks_sha512_struc.h"

#define LT(a, b) (((((a - b) ^ b) & (a ^ b)) ^ (a - b)) >> 31)
#define blocks crypto_hashblocks_sha512_struc
unsigned int crypto_hash_sha512_struc(unsigned int out[16],const unsigned int in[24])
// Note: inlen = 24 u32 or (24*4 bytes) only
{
  unsigned int iv_struc[64] = {
    0x6a09e667,0xf3bcc908,
    0xbb67ae85,0x84caa73b,
    0x3c6ef372,0xfe94f82b,
    0xa54ff53a,0x5f1d36f1,
    0x510e527f,0xade682d1,
    0x9b05688c,0x2b3e6c1f,
    0x1f83d9ab,0xfb41bd6b,
    0x5be0cd19,0x137e2179
  };
  unsigned int inlen = 24;
  unsigned int h[16];
  unsigned int padded[64];
  unsigned int i;
  // unsigned int bytes = 96; // inlen * 4

  for (i = 0;i < 16;++i) h[i] = iv_struc[i];

  for (i = 0;i < inlen;++i) padded[i] = in[i];
  padded[inlen] = 0x80000000;

  // Replace if-else by Z = c*X + (1-c)*Y

  for (i = inlen + 1;i < 29;++i) padded[i] = 0;
  padded[29] = 0;
  padded[30] = 0; // bytes >> 29
  // padded[31] =  (((bytes >> 21) & 0xff) << 24) |
  //               (((bytes >> 13) & 0xff) << 16) |
  //               (((bytes >>  5) & 0xff) <<  8) |
  //               (((bytes <<  3) & 0xff)      );
  padded[31] = 768;
  blocks(h, padded);

  for (i = 0;i < 16;++i) out[i] = h[i];
  return 0;
}