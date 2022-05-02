#include "fe25519_struc.h"

#define LT(a, b) (((((a - b) ^ b) & (a ^ b)) ^ (a - b)) >> 31)
#define EQ(a, b) (1^LT(a, b)) & (1^LT(b, a))
#define LE(a, b) (1^LT(b, a))

void freeze_struc(unsigned int r[fe25519_U32LEN])
{
  unsigned int i;
  unsigned int m, c;
  m = EQ(r[31], 127);

  for(i=30; i>1; i--){
    c = EQ(r[i], 255);
    m *= c;
  }

  c = LE(237, r[0]);
  m *= c;

  r[31] -= m*127;
  for(i=30; i>0; i--) r[i] -= m*255;
  r[0] -= m*237;
}

static void fe25519_reduce_mul_struc(unsigned int r[fe25519_U32LEN])
{
  unsigned int t;
  unsigned int i,rep;

  for(rep=0;rep<2;rep++)
  {
    t = r[31] >> 7;
    r[31] &= 127;
    t *= 19;
    r[0] += t;
    for(i=0;i<31;i++)
    {
      t = r[i] >> 8;
      r[i+1] += t;
      r[i] &= 255;
    }
  }
}

static void fe25519_reduce_add_sub_struc(unsigned int r[fe25519_U32LEN])
{
  unsigned int t;
  unsigned int i,rep;

  for(rep=0;rep<4;rep++)
  {
    t = r[31] >> 7;
    r[31] &= 127;
    t *= 19;
    r[0] += t;
    for(i=0;i<31;i++)
    {
      t = r[i] >> 8;
      r[i+1] += t;
      r[i] &= 255;
    }
  }
}

void fe25519_cmov_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], unsigned int b)
// b holds the value of 1 or 0
{
  unsigned int nb = 1-b;
  unsigned int i;
  for(i=0;i<32;i++) r[i] = nb * r[i] + b * x[i];
}

void fe25519_add_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int y[fe25519_U32LEN])
{
  unsigned int i;
  for(i=0;i<32;i++) r[i] = x[i] + y[i];
  fe25519_reduce_add_sub_struc(r);
}

void fe25519_sub_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int y[fe25519_U32LEN])
{
  unsigned int i;
  unsigned int t[32];
  t[0] = x[0] + 0x1da;
  t[31] = x[31] + 0xfe;
  for(i=1;i<31;i++) t[i] = x[i] + 0x1fe;
  for(i=0;i<32;i++) r[i] = t[i] - y[i];
  fe25519_reduce_add_sub_struc(r);
}

void fe25519_setone_struc(unsigned int r[fe25519_U32LEN])
{
  unsigned int i;
  r[0] = 1;
  for(i=1;i<32;i++) r[i]=0;
}


void fe25519_setzero_struc(unsigned int r[fe25519_U32LEN])
{
  unsigned int i;
  for(i=0;i<32;i++) r[i]=0;
}


void fe25519_neg_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN])
{
  unsigned int t[fe25519_U32LEN];
  unsigned int i;
  for(i=0;i<32;i++) t[i]=x[i];
  fe25519_setzero_struc(r);
  fe25519_sub_struc(r, r, t);
}

void fe25519_mul_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int y[fe25519_U32LEN])
{
  unsigned int i,j;
  unsigned int t[63];
  for(i=0;i<63;i++) t[i] = 0;

  for(i=0;i<32;i++) for(j=0;j<32;j++) t[i+j] += x[i] * y[j];

  for(i=32;i<63;i++) r[i-32] = t[i-32] + 38*t[i];

  r[31] = t[31]; /* result now in r[0]...r[31] */

  fe25519_reduce_mul_struc(r);
}

void fe25519_square_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN])
{
  fe25519_mul_struc(r, x, x);
}

void fe25519_invert_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN])
{
	unsigned int z2[fe25519_U32LEN];
	unsigned int z9[fe25519_U32LEN];
	unsigned int z11[fe25519_U32LEN];
	unsigned int z2_5_0[fe25519_U32LEN];
	unsigned int z2_10_0[fe25519_U32LEN];
	unsigned int z2_20_0[fe25519_U32LEN];
	unsigned int z2_50_0[fe25519_U32LEN];
	unsigned int z2_100_0[fe25519_U32LEN];
	unsigned int t0[fe25519_U32LEN];
	unsigned int t1[fe25519_U32LEN];
	unsigned int i;

	/* 2 */ fe25519_square_struc(z2,x);
	/* 4 */ fe25519_square_struc(t1,z2);
	/* 8 */ fe25519_square_struc(t0,t1);
	/* 9 */ fe25519_mul_struc(z9,t0,x);
	/* 11 */ fe25519_mul_struc(z11,z9,z2);
	/* 22 */ fe25519_square_struc(t0,z11);
	/* 2^5 - 2^0 = 31 */ fe25519_mul_struc(z2_5_0,t0,z9);

	/* 2^6 - 2^1 */ fe25519_square_struc(t0,z2_5_0);
	/* 2^7 - 2^2 */ fe25519_square_struc(t1,t0);
	/* 2^8 - 2^3 */ fe25519_square_struc(t0,t1);
	/* 2^9 - 2^4 */ fe25519_square_struc(t1,t0);
	/* 2^10 - 2^5 */ fe25519_square_struc(t0,t1);
	/* 2^10 - 2^0 */ fe25519_mul_struc(z2_10_0,t0,z2_5_0);

	/* 2^11 - 2^1 */ fe25519_square_struc(t0,z2_10_0);
	/* 2^12 - 2^2 */ fe25519_square_struc(t1,t0);
	/* 2^20 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square_struc(t0,t1); fe25519_square_struc(t1,t0); }
	/* 2^20 - 2^0 */ fe25519_mul_struc(z2_20_0,t1,z2_10_0);

	/* 2^21 - 2^1 */ fe25519_square_struc(t0,z2_20_0);
	/* 2^22 - 2^2 */ fe25519_square_struc(t1,t0);
	/* 2^40 - 2^20 */ for (i = 2;i < 20;i += 2) { fe25519_square_struc(t0,t1); fe25519_square_struc(t1,t0); }
	/* 2^40 - 2^0 */ fe25519_mul_struc(t0,t1,z2_20_0);

	/* 2^41 - 2^1 */ fe25519_square_struc(t1,t0);
	/* 2^42 - 2^2 */ fe25519_square_struc(t0,t1);
	/* 2^50 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square_struc(t1,t0); fe25519_square_struc(t0,t1); }
	/* 2^50 - 2^0 */ fe25519_mul_struc(z2_50_0,t0,z2_10_0);

	/* 2^51 - 2^1 */ fe25519_square_struc(t0,z2_50_0);
	/* 2^52 - 2^2 */ fe25519_square_struc(t1,t0);
	/* 2^100 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square_struc(t0,t1); fe25519_square_struc(t1,t0); }
	/* 2^100 - 2^0 */ fe25519_mul_struc(z2_100_0,t1,z2_50_0);

	/* 2^101 - 2^1 */ fe25519_square_struc(t1,z2_100_0);
	/* 2^102 - 2^2 */ fe25519_square_struc(t0,t1);
	/* 2^200 - 2^100 */ for (i = 2;i < 100;i += 2) { fe25519_square_struc(t1,t0); fe25519_square_struc(t0,t1); }
	/* 2^200 - 2^0 */ fe25519_mul_struc(t1,t0,z2_100_0);

	/* 2^201 - 2^1 */ fe25519_square_struc(t0,t1);
	/* 2^202 - 2^2 */ fe25519_square_struc(t1,t0);
	/* 2^250 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square_struc(t0,t1); fe25519_square_struc(t1,t0); }
	/* 2^250 - 2^0 */ fe25519_mul_struc(t0,t1,z2_50_0);

	/* 2^251 - 2^1 */ fe25519_square_struc(t1,t0);
	/* 2^252 - 2^2 */ fe25519_square_struc(t0,t1);
	/* 2^253 - 2^3 */ fe25519_square_struc(t1,t0);
	/* 2^254 - 2^4 */ fe25519_square_struc(t0,t1);
	/* 2^255 - 2^5 */ fe25519_square_struc(t1,t0);
	/* 2^255 - 21 */ fe25519_mul_struc(r,t1,z11);
}

unsigned int fe25519_getparity_struc(const unsigned int x[fe25519_U32LEN])
{
  unsigned int t[fe25519_U32LEN];
  unsigned int i;
  for(i=0;i<32;i++) t[i] = x[i];
  freeze_struc(t);
  return t[0] & 1;
}

void fe25519_unpack_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[32])
// Note: x holds the values of unsigned char
//       const unsigned int x[32]
{
  unsigned int i;
  for(i=0;i<32;i++) r[i] = x[i];
  r[31] &= 127;
}

void fe25519_pack_struc(unsigned int r[32], const unsigned int x[fe25519_U32LEN])
// Note: r holds the values of unsigned char
//       unsigned char r[32]
//       x should holds the values in range [0, 255]
{
  unsigned int i;
  for(i=0;i<32;i++) r[i] = x[i] % 256;

  /* freeze_struc byte array */
  unsigned int c, m;
  c = EQ(r[31], 127);
  m = c;
  for(i=30; i>1; i--){
    c = EQ(r[i], 255);
    m *= c;
  }

  c = LE(237, r[0]);
  m *= c;

  r[31] -= m*127;

  for(i=30; i>0; i--) r[i] -= m*255;

  r[0] -= m*237;
}
