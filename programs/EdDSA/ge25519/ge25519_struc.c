#include "../sc25519/sc25519_struc.h"
#include "../fe25519/fe25519_struc.h"
#include "ge25519_struc.h"

#define GE_WINDOWSIZE 2
#define GE_WINDOWMASK 3
#define LT(a, b) (((((a - b) ^ b) & (a ^ b)) ^ (a - b)) >> 31)
#define EQ(a, b) (1^LT(a, b)) & (1^LT(b, a))
#define LE(a, b) (1^LT(b, a))
#define NE(a, b) (1^EQ(a,b))


static void p1p1_to_p2_struc(unsigned int r[ge25519_p2_U32LEN], const unsigned int p[ge25519_p1p1_U32LEN])
{
  fe25519_mul_struc(r                 , p                 , p+3*fe25519_U32LEN);
  fe25519_mul_struc(r+  fe25519_U32LEN, p+  fe25519_U32LEN, p+2*fe25519_U32LEN);
  fe25519_mul_struc(r+2*fe25519_U32LEN, p+2*fe25519_U32LEN, p+3*fe25519_U32LEN);
}

static void p1p1_to_p3_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[ge25519_p1p1_U32LEN])
{
  p1p1_to_p2_struc(r, p);
  fe25519_mul_struc(r+3*fe25519_U32LEN, p, p+fe25519_U32LEN);
}

/* Constant-time version of: if(b) r = p */
static void cmov_p3_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[ge25519_p3_U32LEN], unsigned int b)
{
  fe25519_cmov_struc(r                 , p                 , b);
  fe25519_cmov_struc(r+  fe25519_U32LEN, p+  fe25519_U32LEN, b);
  fe25519_cmov_struc(r+2*fe25519_U32LEN, p+2*fe25519_U32LEN, b);
  fe25519_cmov_struc(r+3*fe25519_U32LEN, p+3*fe25519_U32LEN, b);
}

static void dbl_p1p1_struc(unsigned int r[ge25519_p1p1_U32LEN], const unsigned int p[ge25519_p2_U32LEN])
{
  unsigned int a[fe25519_U32LEN];
  unsigned int b[fe25519_U32LEN];
  unsigned int c[fe25519_U32LEN];
  unsigned int d[fe25519_U32LEN];

  fe25519_square_struc(a, p                 );
  fe25519_square_struc(b, p+  fe25519_U32LEN);
  fe25519_square_struc(c, p+2*fe25519_U32LEN);
  fe25519_add_struc(c, c, c);
  fe25519_neg_struc(d, a);

  fe25519_add_struc(r, p, p+fe25519_U32LEN);
  fe25519_square_struc(r, r);
  fe25519_sub_struc(r, r, a);
  fe25519_sub_struc(r, r, b);
  fe25519_add_struc(r+2*fe25519_U32LEN, d, b);
  fe25519_sub_struc(r+3*fe25519_U32LEN, r+2*fe25519_U32LEN, c);
  fe25519_sub_struc(r+fe25519_U32LEN, d, b);
}

static void add_p1p1_struc(unsigned int r[ge25519_p1p1_U32LEN], const unsigned int p[ge25519_p3_U32LEN], const unsigned int q[ge25519_p3_U32LEN])
{
  unsigned int ecd[32] = {0xA3, 0x78, 0x59, 0x13, 0xCA, 0x4D, 0xEB, 0x75, 0xAB, 0xD8, 0x41, 0x41, 0x4D, 0x0A, 0x70, 0x00,
                          0x98, 0xE8, 0x79, 0x77, 0x79, 0x40, 0xC7, 0x8C, 0x73, 0xFE, 0x6F, 0x2B, 0xEE, 0x6C, 0x03, 0x52};
  unsigned int a[fe25519_U32LEN];
  unsigned int b[fe25519_U32LEN];
  unsigned int c[fe25519_U32LEN];
  unsigned int d[fe25519_U32LEN];
  unsigned int t[fe25519_U32LEN];
  unsigned int fd[fe25519_U32LEN];
  fe25519_unpack_struc(fd, ecd);

  fe25519_sub_struc(a, p+fe25519_U32LEN, p); // A = (Y1-X1)*(Y2-X2)
  fe25519_sub_struc(t, q+fe25519_U32LEN, q);
  fe25519_mul_struc(a, a, t);
  fe25519_add_struc(b, p, p+fe25519_U32LEN); // B = (Y1+X1)*(Y2+X2)
  fe25519_add_struc(t, q, q+fe25519_U32LEN);
  fe25519_mul_struc(b, b, t);
  fe25519_mul_struc(c, p+3*fe25519_U32LEN, q+3*fe25519_U32LEN); //C = T1*k*T2
  fe25519_mul_struc(c, c, fd);
  fe25519_add_struc(c, c, c);       //XXX: Can save this addition by precomputing 2*ecd
  fe25519_mul_struc(d, p+2*fe25519_U32LEN, q+2*fe25519_U32LEN); //D = Z1*2*Z2
  fe25519_add_struc(d, d, d);
  fe25519_sub_struc(r, b, a); // E = B-A
  fe25519_sub_struc(r+3*fe25519_U32LEN, d, c); // F = D-C
  fe25519_add_struc(r+2*fe25519_U32LEN, d, c); // G = D+C
  fe25519_add_struc(r+  fe25519_U32LEN, b, a); // H = B+A
}

void ge25519_add_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[ge25519_p3_U32LEN], const unsigned int q[ge25519_p3_U32LEN])
{
  unsigned int grp1p1[ge25519_p1p1_U32LEN];
  add_p1p1_struc(grp1p1, p, q);
  p1p1_to_p3_struc(r, grp1p1);
}

void ge25519_scalarmult_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[ge25519_p3_U32LEN], const unsigned int s[32])
{
  unsigned int ge25519_neutral_x[32] = {0};
  unsigned int ge25519_neutral_y[32] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned int ge25519_neutral_z[32] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned int ge25519_neutral_t[32] = {0};

  unsigned int i,j,k;
  unsigned int g[ge25519_p3_U32LEN];

  fe25519_unpack_struc(g                 , ge25519_neutral_x);
  fe25519_unpack_struc(g+  fe25519_U32LEN, ge25519_neutral_y);
  fe25519_unpack_struc(g+2*fe25519_U32LEN, ge25519_neutral_z);
  fe25519_unpack_struc(g+3*fe25519_U32LEN, ge25519_neutral_t);

  unsigned int n = (1 << GE_WINDOWSIZE);
  unsigned int pre[n*ge25519_p3_U32LEN];
  unsigned int t[ge25519_p3_U32LEN];
  unsigned int tp1p1[ge25519_p1p1_U32LEN];
  unsigned int w;
  unsigned int sb[32];
  sc25519_to32bytes_struc(sb, s);

  // Precomputation
  // pre[0] = g;
  for(i=0;i<ge25519_U32LEN;i++) pre[i] = g[i];
  // pre[1] = *p;
  for(i=0;i<ge25519_U32LEN;i++) pre[i+ge25519_p3_U32LEN] = p[i];
  for(i=2;i<n;i+=2)
  {
    dbl_p1p1_struc(tp1p1, pre+i/2*ge25519_p3_U32LEN);
    p1p1_to_p3_struc(pre+i*ge25519_p3_U32LEN, tp1p1);
    add_p1p1_struc(tp1p1, pre+i*ge25519_p3_U32LEN, pre+ge25519_p1p1_U32LEN);
    p1p1_to_p3_struc(pre+(i+1)*ge25519_p1p1_U32LEN, tp1p1);
  }

  unsigned int c, jt;
  unsigned int X[ge25519_p3_U32LEN];
  unsigned int Y[ge25519_p3_U32LEN];
  // Fixed-window scalar multiplication
  unsigned int lw = 8-GE_WINDOWSIZE;
  for(i=32;i>0;i--)
  {
    for(j=0; j<=lw; j+=GE_WINDOWSIZE)
    {
      jt = lw - j;
      for(k=0; k<GE_WINDOWSIZE-1; k++)
      {
        dbl_p1p1_struc(tp1p1, g);
        p1p1_to_p2_struc(g, tp1p1);
      }
      dbl_p1p1_struc(tp1p1, g);
      p1p1_to_p3_struc(g, tp1p1);

      // Cache-timing resistant loading of precomputed value:
      w = (sb[i-1] >> jt) & GE_WINDOWMASK;
      for(k=0;k<ge25519_p3_U32LEN;k++) t[k] = pre[k];
      for(k=1;k<n;k++){
        c = EQ(k, w);
        cmov_p3_struc(t, pre+k*ge25519_p3_U32LEN, c);
      }

      add_p1p1_struc(tp1p1, g, t);
      c = NE(jt, 0);
      p1p1_to_p2_struc(X, tp1p1);
      for(k=0;k<fe25519_U32LEN;k++) X[3*fe25519_U32LEN+k] = g[3*fe25519_U32LEN+k]; // copy gt
      p1p1_to_p3_struc(Y, tp1p1);
      for(k=0;k<ge25519_p3_U32LEN;k++) g[k] = c*X[k] + (1-c)*Y[k];
    }
  }
  for(i=0;i<fe25519_U32LEN;i++){
    r[i]                  = g[i];
    r[i+  fe25519_U32LEN] = g[i+fe25519_U32LEN];
    r[i+2*fe25519_U32LEN] = g[i+2*fe25519_U32LEN];
    r[i+3*fe25519_U32LEN] = g[i+3*fe25519_U32LEN];
  }
}

void ge25519_scalarmult_base_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int s[32])
{
  unsigned int ge25519_base_x[32] = {0x1A, 0xD5, 0x25, 0x8F, 0x60, 0x2D, 0x56, 0xC9, 0xB2, 0xA7, 0x25, 0x95, 0x60, 0xC7, 0x2C, 0x69,
                                    0x5C, 0xDC, 0xD6, 0xFD, 0x31, 0xE2, 0xA4, 0xC0, 0xFE, 0x53, 0x6E, 0xCD, 0xD3, 0x36, 0x69, 0x21};
  unsigned int ge25519_base_y[32] = {0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                                    0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
  unsigned int ge25519_base_z[32] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned int ge25519_base_t[32] = {0xA3, 0xDD, 0xB7, 0xA5, 0xB3, 0x8A, 0xDE, 0x6D, 0xF5, 0x52, 0x51, 0x77, 0x80, 0x9F, 0xF0, 0x20,
                                    0x7D, 0xE3, 0xAB, 0x64, 0x8E, 0x4E, 0xEA, 0x66, 0x65, 0x76, 0x8B, 0xD7, 0x0F, 0x5F, 0x87, 0x67};

  unsigned int t[ge25519_p3_U32LEN];
  fe25519_unpack_struc(t                 , ge25519_base_x);
  fe25519_unpack_struc(t+  fe25519_U32LEN, ge25519_base_y);
  fe25519_unpack_struc(t+2*fe25519_U32LEN, ge25519_base_z);
  fe25519_unpack_struc(t+3*fe25519_U32LEN, ge25519_base_t);
  ge25519_scalarmult_struc(r, t, s);
}

void ge25519_pack_struc(unsigned int r[32], const unsigned int p[ge25519_p3_U32LEN])
{
  unsigned int tx[fe25519_U32LEN], ty[fe25519_U32LEN], zi[fe25519_U32LEN];

  fe25519_invert_struc(zi, p+2*fe25519_U32LEN);
  fe25519_mul_struc(tx, p, zi);
  fe25519_mul_struc(ty, p+fe25519_U32LEN, zi);
  fe25519_pack_struc(r, ty);
  r[31] ^= fe25519_getparity_struc(tx) << 7;
}
