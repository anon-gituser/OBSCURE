#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "crypto_hashblocks_sha512_struc.h"
#include "crypto_hashblocks_sha512.h"


#define SHR(x,c) ((x) >> (c))
void test_SHRge32()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned int c = (rand() % 32) + 32;
  unsigned long long ru = SHR(u, c);
  SHRge32(a, c);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_SHRge32: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("c         : %u\n", c);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_SHRlt32()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned int c = (rand() % 32);
  unsigned long long ru = SHR(u, c);
  SHRlt32(a, c);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_SHRlt32: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("c         : %u\n", c);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

#define SHL(x,c) ((x) << (c))
void test_SHLge32()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned int c = (rand() % 32) + 32;
  unsigned long long ru = SHL(u, c);
  SHLge32(a, c);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_SHLge32: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("c         : %u\n", c);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_SHLlt32()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned int c = (rand() % 32);
  unsigned long long ru = SHL(u, c);
  SHLlt32(a, c);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_SHLlt32: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("c         : %u\n", c);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

#define ROTR(x,c) (((x) >> (c)) | ((x) << (64 - (c))))
void test_ROTR28()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 28);
  ROTR28(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR28: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR34()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 34);
  ROTR34(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR34: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR39()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 39);
  ROTR39(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR39: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR1()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 1);
  ROTR1(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR1: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR8()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 8);
  ROTR8(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR8: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR19()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 19);
  ROTR19(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR19: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR61()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 61);
  ROTR61(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR61: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR14()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 14);
  ROTR14(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROR14: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR18()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 18);
  ROTR18(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROTR18: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

void test_ROTR41()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = ROTR(u, 41);
  ROTR41(a);
  unsigned long long ra = a[0] * (1ull<<32) + a[1];

  printf("test_ROTR41: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", a[0], a[1]);
    exit(EXIT_FAILURE);
  }
}

#define Sigma0(x) (ROTR(x,28) ^ ROTR(x,34) ^ ROTR(x,39))
void test_Sigma0()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = Sigma0(u);
  unsigned int r[2];
  Sigma0_struc(r, a);
  unsigned long long ra = r[0] * (1ull<<32) + r[1];

  printf("test_Sigma0: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", r[0], r[1]);
    exit(EXIT_FAILURE);
  }
}

#define sigma0(x) (ROTR(x, 1) ^ ROTR(x, 8) ^ SHR(x,7))
void test_sigma0()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = sigma0(u);
  unsigned int r[2];
  sigma0_struc(r, a);
  unsigned long long ra = r[0] * (1ull<<32) + r[1];

  printf("test_sigma0: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", r[0], r[1]);
    exit(EXIT_FAILURE);
  }
}

#define Sigma1(x) (ROTR(x,14) ^ ROTR(x,18) ^ ROTR(x,41))
void test_Sigma1()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = Sigma1(u);
  unsigned int r[2];
  Sigma1_struc(r, a);
  unsigned long long ra = r[0] * (1ull<<32) + r[1];

  printf("test_Sigma1: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", r[0], r[1]);
    exit(EXIT_FAILURE);
  }
}

#define sigma1(x) (ROTR(x,19) ^ ROTR(x,61) ^ SHR(x,6))
void test_sigma1()
{
  unsigned int a[2];
  a[0] = rand(); a[1] = rand();
  unsigned long long u = a[0] * (1ull<<32) + a[1];
  unsigned long long ru = sigma1(u);
  unsigned int r[2];
  sigma1_struc(r, a);
  unsigned long long ra = r[0] * (1ull<<32) + r[1];

  printf("test_sigma1: ");
  if (ru == ra) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : %llu\n", u);
    printf("Expected  : %llu\n", ru);
    printf("Returned  : %llu\n", ra);
    printf("Return u32: (%u, %u)\n", r[0], r[1]);
    exit(EXIT_FAILURE);
  }
}

#define Ch(x,y,z) ((x & y) ^ (~x & z))
void test_Ch()
{
  unsigned int x[2], y[2], z[2];
  x[0] = rand(); x[1] = rand();
  y[0] = rand(); y[1] = rand();
  z[0] = rand(); z[1] = rand();

  unsigned long long ux = x[0] * (1ull<<32) + x[1];
  unsigned long long uy = y[0] * (1ull<<32) + y[1];
  unsigned long long uz = z[0] * (1ull<<32) + z[1];

  unsigned long long ur = Ch(ux, uy, uz);
  unsigned int r[2];
  Ch_struc(r, x, y, z);
  unsigned long long ur2 = r[0] * (1ull<<32) + r[1];

  printf("test_Ch: ");
  if (ur == ur2) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : (%llu, %llu, %llu)\n", ux, uy, uz);
    printf("Expected  : %llu\n", ur);
    printf("Returned  : %llu\n", ur2);
    exit(EXIT_FAILURE);
  }
}

#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
void test_Maj()
{
  unsigned int x[2], y[2], z[2];
  x[0] = rand(); x[1] = rand();
  y[0] = rand(); y[1] = rand();
  z[0] = rand(); z[1] = rand();

  unsigned long long ux = x[0] * (1ull<<32) + x[1];
  unsigned long long uy = y[0] * (1ull<<32) + y[1];
  unsigned long long uz = z[0] * (1ull<<32) + z[1];

  unsigned long long ur = Maj(ux, uy, uz);
  unsigned int r[2];
  Maj_struc(r, x, y, z);
  unsigned long long ur2 = r[0] * (1ull<<32) + r[1];

  printf("test_Maj: ");
  if (ur == ur2) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : (%llu, %llu, %llu)\n", ux, uy, uz);
    printf("Expected  : %llu\n", ur);
    printf("Returned  : %llu\n", ur2);
    exit(EXIT_FAILURE);
  }
}

void test_U64_ADD()
{
  unsigned int x[2], y[2];
  x[0] = rand(); x[1] = rand();
  y[0] = rand(); y[1] = rand();

  unsigned long long ux = x[0] * (1ull<<32) + x[1];
  unsigned long long uy = y[0] * (1ull<<32) + y[1];

  unsigned long long ur = ux + uy;
  unsigned int r[2];
  U64_ADD(r, x, y);
  unsigned long long ur2 = r[0] * (1ull<<32) + r[1];

  printf("test_U64_ADD: ");
  if (ur == ur2) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : (%llu, %llu)\n", ux, uy);
    printf("Expected  : %llu\n", ur);
    printf("Returned  : %llu\n", ur2);
    exit(EXIT_FAILURE);
  }
}

#define M(w0,w14,w9,w1) w0 = sigma1(w14) + w9 + sigma0(w1) + w0;
#define M_struc(w0, w14, w9, w1) \
  sigma1_struc(T1, w14); \
  sigma0_struc(T2, w1); \
  U64_ADD(w0, w0, T1); \
  U64_ADD(w0, w0, w9); \
  U64_ADD(w0, w0, T2);
void test_M()
{
  unsigned int w0[2], w14[2], w9[2], w1[2], w0c[2];
  w0[0] = rand(); w0[1] = rand(); w0c[0] = w0[0]; w0c[1] = w0[1];
  w14[0] = rand(); w14[1] = rand();
  w9[0] = rand(); w9[1] = rand();
  w1[0] = rand(); w1[1] = rand();

  unsigned long long u0, u14, u9, u1, u0c;
  u0 = w0c[0] * (1ull<<32) + w0c[1]; u0c = u0;
  u14 = w14[0] * (1ull<<32) + w14[1];
  u9 = w9[0] * (1ull<<32) + w9[1];
  u1 = w1[0] * (1ull<<32) + w1[1];

  unsigned int T1[2], T2[2];
  M_struc(w0, w14, w9, w1)
  unsigned long long r = w0[0] * (1ull<<32) + w0[1];
  M(u0, u14, u9, u1)

  printf("test_M: ");
  if (r == u0) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : (%llu, %llu, %llu, %llu)\n", u0c, u14, u9, u1);
    printf("Expected  : %llu\n", u0);
    printf("Returned  : %llu\n", r);
    exit(EXIT_FAILURE);
  }
}

#define F(w,k) \
  T1 = h + Sigma1(e) + Ch(e,f,g) + k + w; \
  T2 = Sigma0(a) + Maj(a,b,c); \
  h = g; \
  g = f; \
  f = e; \
  e = d + T1; \
  d = c; \
  c = b; \
  b = a; \
  a = T1 + T2;
#define F_struc(w,k) \
  Sigma1_struc(rs,xe); Ch_struc(rc,xe,xf,xg); \
  U64_ADD(xT1, xh, rs); U64_ADD(xT1, xT1, rc); U64_ADD(xT1, xT1, k); U64_ADD(xT1, xT1, w); \
  Sigma0_struc(rs,xa); Maj_struc(rm,xa,xb,xc); \
  U64_ADD(xT2, rs, rm); \
  xh[0] = xg[0]; xh[1] = xg[1]; \
  xg[0] = xf[0]; xg[1] = xf[1]; \
  xf[0] = xe[0]; xf[1] = xe[1]; \
  U64_ADD(xe, xd, xT1); \
  xd[0] = xc[0]; xd[1] = xc[1]; \
  xc[0] = xb[0]; xc[1] = xb[1]; \
  xb[0] = xa[0]; xb[1] = xa[1]; \
  U64_ADD(xa, xT1, xT2);
void test_F()
{
  unsigned int w[2], k[2];
  w[0] = rand(); w[1] = rand();
  k[0] = rand(); k[1] = rand();

  unsigned long long uw, uk, uwc;
  uw = w[0] * (1ull<<32) + w[1]; uwc = uw;
  uk = k[0] * (1ull<<32) + k[1];

  unsigned int xa[2], xb[2], xc[2], xd[2], xe[2], xf[2], xg[2], xh[2];
  xa[0] = rand(); xa[1] = rand();
  xb[0] = rand(); xb[1] = rand();
  xc[0] = rand(); xc[1] = rand();
  xd[0] = rand(); xd[1] = rand();
  xe[0] = rand(); xe[1] = rand();
  xf[0] = rand(); xf[1] = rand();
  xg[0] = rand(); xg[1] = rand();
  xh[0] = rand(); xh[1] = rand();
  unsigned long long a, b, c, d, e, f, g, h;
  a = xa[0] * (1ull<<32) + xa[1];
  b = xb[0] * (1ull<<32) + xb[1];
  c = xc[0] * (1ull<<32) + xc[1];
  d = xd[0] * (1ull<<32) + xd[1];
  e = xe[0] * (1ull<<32) + xe[1];
  f = xf[0] * (1ull<<32) + xf[1];
  g = xg[0] * (1ull<<32) + xg[1];
  h = xh[0] * (1ull<<32) + xh[1];

  unsigned int rs[2], rc[2], rm[2], xT1[2], xT2[2];
  unsigned long long T1, T2;

  F_struc(w, k)
  F(uw, uk)

  unsigned long long ta, tb, tc, td, te, tf, tg, th;
  ta = xa[0] * (1ull<<32) + xa[1];
  tb = xb[0] * (1ull<<32) + xb[1];
  tc = xc[0] * (1ull<<32) + xc[1];
  td = xd[0] * (1ull<<32) + xd[1];
  te = xe[0] * (1ull<<32) + xe[1];
  tf = xf[0] * (1ull<<32) + xf[1];
  tg = xg[0] * (1ull<<32) + xg[1];
  th = xh[0] * (1ull<<32) + xh[1];

  printf("test_F: ");
  if (a == ta && b == tb && c == tc && d == td &&\
      e == te && f == tf && g == tg && h == th) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u64 : (%llu, %llu)\n", uwc, uk);
    printf("Expected  : (a,b,c,d,e,f,g,h) = (%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu)\n", a,b,c,d,e,f,g,h);
    printf("Returned  : (a,b,c,d,e,f,g,h) = (%llu,%llu,%llu,%llu,%llu,%llu,%llu,%llu)\n", ta,tb,tc,td,te,tf,tg,th);
    exit(EXIT_FAILURE);
  }
}

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
static const unsigned char iv[64] = {
  0x6a,0x09,0xe6,0x67,0xf3,0xbc,0xc9,0x08,
  0xbb,0x67,0xae,0x85,0x84,0xca,0xa7,0x3b,
  0x3c,0x6e,0xf3,0x72,0xfe,0x94,0xf8,0x2b,
  0xa5,0x4f,0xf5,0x3a,0x5f,0x1d,0x36,0xf1,
  0x51,0x0e,0x52,0x7f,0xad,0xe6,0x82,0xd1,
  0x9b,0x05,0x68,0x8c,0x2b,0x3e,0x6c,0x1f,
  0x1f,0x83,0xd9,0xab,0xfb,0x41,0xbd,0x6b,
  0x5b,0xe0,0xcd,0x19,0x13,0x7e,0x21,0x79
};
static const unsigned int iv_struc[64] = {
  0x6a09e667,0xf3bcc908,
  0xbb67ae85,0x84caa73b,
  0x3c6ef372,0xfe94f82b,
  0xa54ff53a,0x5f1d36f1,
  0x510e527f,0xade682d1,
  0x9b05688c,0x2b3e6c1f,
  0x1f83d9ab,0xfb41bd6b,
  0x5be0cd19,0x137e2179
};
void test_crypto_hashblocks_sha512()
{
  unsigned int i;
  unsigned char h[64];
  for(i=0; i<64; i++) h[i] = iv[i];
  unsigned int xh[16];
  for(i=0; i<16; i++) xh[i] = iv_struc[i];

  // inlen = 128 (1 block) only
  unsigned long long inlen = 128;
  unsigned char in[inlen];
  for(i=0; i<inlen; i++) in[i] = rand() % 256;

  unsigned int xinlen = inlen/4;
  unsigned int xin[inlen/4];
  bytes_to_u32(xin, in, inlen);

  crypto_hashblocks_sha512_struc(xh, xin);
  crypto_hashblocks_sha512(h, in, inlen);

  unsigned int rh[16];
  bytes_to_u32(rh, h, 64);
  printf("test_crypto_hashblocks_sha512: ");
  if (memcmp(xh, rh, 16) == 0) {
    printf("OK\n");
  }
  else {
    printf("Failed\n");
    printf("Input u32 : "); for(i=0; i<xinlen; i++) printf("%u, ", xin[i]); printf("\n");
    printf("Expected  : "); for(i=0; i<16; i++) printf("%u, ", rh[i]); printf("\n");
    printf("Returned  : "); for(i=0; i<16; i++) printf("%u, ", xh[i]); printf("\n");
    exit(EXIT_FAILURE);
  }
}


int main()
{
  srand(time(NULL));

  test_ROTR14();
  test_ROTR18();
  test_ROTR41();

  test_Sigma0();
  test_sigma0();
  test_Sigma1();
  test_sigma1();
  test_Ch();
  test_Maj();
  test_U64_ADD();
  test_M();
  test_F();
  test_crypto_hashblocks_sha512();

  return 0;
}