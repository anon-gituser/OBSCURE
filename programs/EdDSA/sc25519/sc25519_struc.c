#include "sc25519_struc.h"

#define LT(a, b) (((((a - b) ^ b) & (a ^ b)) ^ (a - b)) >> 31)
#define EQ(a, b) (1^LT(a, b)) & (1^LT(b, a))
#define LE(a, b) (1^LT(b, a))

static void sc25519_reduce_add_sub_struc(unsigned int r[sc25519_U32LEN])
{
  unsigned int m[32] = {0xED, 0xD3, 0xF5, 0x5C, 0x1A, 0x63, 0x12, 0x58, 0xD6, 0x9C, 0xF7, 0xA2, 0xDE, 0xF9, 0xDE, 0x14,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
  unsigned int i, b, c, pb=0, nb;
  unsigned int t[32];

  for(i=0;i<32;i++)
  {
    c = pb+m[i];
    b = LT(r[i], c);
    t[i] = r[i]-pb-m[i]+b*256;
    pb = b;
  }
  nb = 1-b;
  for(i=0;i<32;i++) r[i] = r[i]*b + t[i]*nb;
}

static void sc25519_barrett_reduce_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[64])
{
  unsigned int m[32] = {0xED, 0xD3, 0xF5, 0x5C, 0x1A, 0x63, 0x12, 0x58, 0xD6, 0x9C, 0xF7, 0xA2, 0xDE, 0xF9, 0xDE, 0x14,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};

  unsigned int mu[33] = {0x1B, 0x13, 0x2C, 0x0A, 0xA3, 0xE5, 0x9C, 0xED, 0xA7, 0x29, 0x63, 0x08, 0x5D, 0x21, 0x06, 0x21,
                        0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F};

  unsigned int i,j;
  unsigned int q2[66] = {0};
  unsigned int r1[33];
  unsigned int r2[65] = {0};
  unsigned int carry;
  unsigned int b, pb=0;

  /**
   * Transform a condition into:
   *    Z = c*X + (1-c)*Y
   */

  unsigned int c, a;
  unsigned int X;
  unsigned int Y;
  for(i=0;i<33;i++){
    for(j=0;j<33;j++){
      a = i+j;
      c = LE(31, a);
      Y = 0;
      X = mu[i]*x[j+31];
      q2[i+j] += (c*X + (1-c)*Y);
    }
  }

  carry = q2[31] >> 8;
  q2[32] += carry;
  carry = q2[32] >> 8;
  q2[33] += carry;

  for(i=0;i<33;i++)r1[i] = x[i];
  for(i=0;i<32;i++){
    for(j=0;j<33;j++){
      a = i+j;
      c = LT(a, 33);
      Y = 0;
      X = m[i]*q2[j+33];
      r2[i+j] += (c*X + (1-c)*Y);
    }
  }

  for(i=0;i<32;i++)
  {
    carry = r2[i] >> 8;
    r2[i+1] += carry;
    r2[i] &= 0xff;
  }

  for(i=0;i<32;i++)
  {
    a = pb+r2[i];
    b = LT(r1[i], a);
    r[i] = r1[i]-pb-r2[i]+b*256;
    pb = b;
  }

  sc25519_reduce_add_sub_struc(r);
  sc25519_reduce_add_sub_struc(r);
}

void sc25519_from32bytes_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[32])
// Note: x holds value of unsigned char
//       const unsigned char x[32]
{
  unsigned int i;
  unsigned int t[64] = {0};
  for(i=0;i<32;i++) t[i] = x[i];
  sc25519_barrett_reduce_struc(r, t);
}

void sc25519_from64bytes_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[64])
// Note: x holds value of unsigned char
//       const unsigned char x[64]
{
  unsigned int i;
  unsigned int t[64] = {0};
  for(i=0;i<64;i++) t[i] = x[i];
  sc25519_barrett_reduce_struc(r, t);
}

void sc25519_to32bytes_struc(unsigned int r[32], const unsigned int x[sc25519_U32LEN])
// Note: r holds value of unsigned char
//       unsigned char r[32]
//       So x should hold values in [0,255]
{
  unsigned int i;
  for(i=0;i<32;i++) r[i] = x[i];
}

void sc25519_add_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[sc25519_U32LEN], const unsigned int y[sc25519_U32LEN])
{
  unsigned int i, carry;
  for(i=0;i<32;i++) r[i] = x[i] + y[i];
  for(i=0;i<31;i++)
  {
    carry = r[i] >> 8;
    r[i+1] += carry;
    r[i] &= 0xff;
  }
  sc25519_reduce_add_sub_struc(r);
}


void sc25519_mul_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[sc25519_U32LEN], const unsigned int y[sc25519_U32LEN])
{
  unsigned int i,j,carry;
  unsigned int t[64];
  for(i=0;i<64;i++)t[i] = 0;

  for(i=0;i<32;i++) for(j=0;j<32;j++) t[i+j] += x[i] * y[j];

  /* Reduce coefficients */
  for(i=0;i<63;i++)
  {
    carry = t[i] >> 8;
    t[i+1] += carry;
    t[i] &= 0xff;
  }

  sc25519_barrett_reduce_struc(r, t);
}
