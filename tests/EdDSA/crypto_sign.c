static void sc25519_reduce_add_sub_struc(unsigned int r[32])
{
  unsigned int m[32] = {0xED, 0xD3, 0xF5, 0x5C, 0x1A, 0x63, 0x12, 0x58, 0xD6, 0x9C, 0xF7, 0xA2, 0xDE, 0xF9, 0xDE, 0x14,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};
  unsigned int i, b, c, pb = 0, nb;
  unsigned int t[32];

  for (i = 0; i < 32; i++)
  {
    c = pb + m[i];
    b = (((((r[i] - c) ^ c) & (r[i] ^ c)) ^ (r[i] - c)) >> 31);
    t[i] = r[i] - pb - m[i] + b * 256;
    pb = b;
  }
  nb = 1 - b;
  for (i = 0; i < 32; i++)
    r[i] = r[i] * b + t[i] * nb;
}

static void sc25519_barrett_reduce_struc(unsigned int r[32], const unsigned int x[64])
{
  unsigned int m[32] = {0xED, 0xD3, 0xF5, 0x5C, 0x1A, 0x63, 0x12, 0x58, 0xD6, 0x9C, 0xF7, 0xA2, 0xDE, 0xF9, 0xDE, 0x14,
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};

  unsigned int mu[33] = {0x1B, 0x13, 0x2C, 0x0A, 0xA3, 0xE5, 0x9C, 0xED, 0xA7, 0x29, 0x63, 0x08, 0x5D, 0x21, 0x06, 0x21,
                         0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F};

  unsigned int i, j;
  unsigned int q2[66] = {0};
  unsigned int r1[33];
  unsigned int r2[65] = {0};
  unsigned int carry;
  unsigned int b, pb = 0;

  unsigned int c, a;
  unsigned int X;
  unsigned int Y;
  for (i = 0; i < 33; i++)
  {
    for (j = 0; j < 33; j++)
    {
      a = i + j;
      c = (1 ^ (((((a - 31) ^ 31) & (a ^ 31)) ^ (a - 31)) >> 31));
      Y = 0;
      X = mu[i] * x[j + 31];
      q2[i + j] += (c * X + (1 - c) * Y);
    }
  }

  carry = q2[31] >> 8;
  q2[32] += carry;
  carry = q2[32] >> 8;
  q2[33] += carry;

  for (i = 0; i < 33; i++)
    r1[i] = x[i];
  for (i = 0; i < 32; i++)
  {
    for (j = 0; j < 33; j++)
    {
      a = i + j;
      c = (((((a - 33) ^ 33) & (a ^ 33)) ^ (a - 33)) >> 31);
      Y = 0;
      X = m[i] * q2[j + 33];
      r2[i + j] += (c * X + (1 - c) * Y);
    }
  }

  for (i = 0; i < 32; i++)
  {
    carry = r2[i] >> 8;
    r2[i + 1] += carry;
    r2[i] &= 0xff;
  }

  for (i = 0; i < 32; i++)
  {
    a = pb + r2[i];
    b = (((((r1[i] - a) ^ a) & (r1[i] ^ a)) ^ (r1[i] - a)) >> 31);
    r[i] = r1[i] - pb - r2[i] + b * 256;
    pb = b;
  }

  sc25519_reduce_add_sub_struc(r);
  sc25519_reduce_add_sub_struc(r);
}

void sc25519_from32bytes_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  unsigned int t[64] = {0};
  for (i = 0; i < 32; i++)
    t[i] = x[i];
  sc25519_barrett_reduce_struc(r, t);
}

void sc25519_from64bytes_struc(unsigned int r[32], const unsigned int x[64])

{
  unsigned int i;
  unsigned int t[64] = {0};
  for (i = 0; i < 64; i++)
    t[i] = x[i];
  sc25519_barrett_reduce_struc(r, t);
}

void sc25519_to32bytes_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i];
}

void sc25519_add_struc(unsigned int r[32], const unsigned int x[32], const unsigned int y[32])
{
  unsigned int i, carry;
  for (i = 0; i < 32; i++)
    r[i] = x[i] + y[i];
  for (i = 0; i < 31; i++)
  {
    carry = r[i] >> 8;
    r[i + 1] += carry;
    r[i] &= 0xff;
  }
  sc25519_reduce_add_sub_struc(r);
}

void sc25519_mul_struc(unsigned int r[32], const unsigned int x[32], const unsigned int y[32])
{
  unsigned int i, j, carry;
  unsigned int t[64];
  for (i = 0; i < 64; i++)
    t[i] = 0;

  for (i = 0; i < 32; i++)
    for (j = 0; j < 32; j++)
      t[i + j] += x[i] * y[j];

  for (i = 0; i < 63; i++)
  {
    carry = t[i] >> 8;
    t[i + 1] += carry;
    t[i] &= 0xff;
  }

  sc25519_barrett_reduce_struc(r, t);
}

void freeze_struc(unsigned int r[32])
{
  unsigned int i;
  unsigned int m, c;
  m = (1 ^ (((((r[31] - 127) ^ 127) & (r[31] ^ 127)) ^ (r[31] - 127)) >> 31)) & (1 ^ (((((127 - r[31]) ^ r[31]) & (127 ^ r[31])) ^ (127 - r[31])) >> 31));

  for (i = 30; i > 1; i--)
  {
    c = (1 ^ (((((r[i] - 255) ^ 255) & (r[i] ^ 255)) ^ (r[i] - 255)) >> 31)) & (1 ^ (((((255 - r[i]) ^ r[i]) & (255 ^ r[i])) ^ (255 - r[i])) >> 31));
    m *= c;
  }

  c = (1 ^ (((((r[0] - 237) ^ 237) & (r[0] ^ 237)) ^ (r[0] - 237)) >> 31));
  m *= c;

  r[31] -= m * 127;
  for (i = 30; i > 0; i--)
    r[i] -= m * 255;
  r[0] -= m * 237;
}

static void fe25519_reduce_mul_struc(unsigned int r[32])
{
  unsigned int t;
  unsigned int i, rep;

  for (rep = 0; rep < 2; rep++)
  {
    t = r[31] >> 7;
    r[31] &= 127;
    t *= 19;
    r[0] += t;
    for (i = 0; i < 31; i++)
    {
      t = r[i] >> 8;
      r[i + 1] += t;
      r[i] &= 255;
    }
  }
}

static void fe25519_reduce_add_sub_struc(unsigned int r[32])
{
  unsigned int t;
  unsigned int i, rep;

  for (rep = 0; rep < 4; rep++)
  {
    t = r[31] >> 7;
    r[31] &= 127;
    t *= 19;
    r[0] += t;
    for (i = 0; i < 31; i++)
    {
      t = r[i] >> 8;
      r[i + 1] += t;
      r[i] &= 255;
    }
  }
}

void fe25519_cmov_struc(unsigned int r[32], const unsigned int x[32], unsigned int b)

{
  unsigned int nb = 1 - b;
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = nb * r[i] + b * x[i];
}

void fe25519_add_struc(unsigned int r[32], const unsigned int x[32], const unsigned int y[32])
{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i] + y[i];
  fe25519_reduce_add_sub_struc(r);
}

void fe25519_sub_struc(unsigned int r[32], const unsigned int x[32], const unsigned int y[32])
{
  unsigned int i;
  unsigned int t[32];
  t[0] = x[0] + 0x1da;
  t[31] = x[31] + 0xfe;
  for (i = 1; i < 31; i++)
    t[i] = x[i] + 0x1fe;
  for (i = 0; i < 32; i++)
    r[i] = t[i] - y[i];
  fe25519_reduce_add_sub_struc(r);
}

void fe25519_setone_struc(unsigned int r[32])
{
  unsigned int i;
  r[0] = 1;
  for (i = 1; i < 32; i++)
    r[i] = 0;
}

void fe25519_setzero_struc(unsigned int r[32])
{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = 0;
}

void fe25519_neg_struc(unsigned int r[32], const unsigned int x[32])
{
  unsigned int t[32];
  unsigned int i;
  for (i = 0; i < 32; i++)
    t[i] = x[i];
  fe25519_setzero_struc(r);
  fe25519_sub_struc(r, r, t);
}

void fe25519_mul_struc(unsigned int r[32], const unsigned int x[32], const unsigned int y[32])
{
  unsigned int i, j;
  unsigned int t[63];
  for (i = 0; i < 63; i++)
    t[i] = 0;

  for (i = 0; i < 32; i++)
    for (j = 0; j < 32; j++)
      t[i + j] += x[i] * y[j];

  for (i = 32; i < 63; i++)
    r[i - 32] = t[i - 32] + 38 * t[i];

  r[31] = t[31];

  fe25519_reduce_mul_struc(r);
}

void fe25519_square_struc(unsigned int r[32], const unsigned int x[32])
{
  fe25519_mul_struc(r, x, x);
}

void fe25519_invert_struc(unsigned int r[32], const unsigned int x[32])
{
  unsigned int z2[32];
  unsigned int z9[32];
  unsigned int z11[32];
  unsigned int z2_5_0[32];
  unsigned int z2_10_0[32];
  unsigned int z2_20_0[32];
  unsigned int z2_50_0[32];
  unsigned int z2_100_0[32];
  unsigned int t0[32];
  unsigned int t1[32];
  unsigned int i;

  fe25519_square_struc(z2, x);
  fe25519_square_struc(t1, z2);
  fe25519_square_struc(t0, t1);
  fe25519_mul_struc(z9, t0, x);
  fe25519_mul_struc(z11, z9, z2);
  fe25519_square_struc(t0, z11);
  fe25519_mul_struc(z2_5_0, t0, z9);

  fe25519_square_struc(t0, z2_5_0);
  fe25519_square_struc(t1, t0);
  fe25519_square_struc(t0, t1);
  fe25519_square_struc(t1, t0);
  fe25519_square_struc(t0, t1);
  fe25519_mul_struc(z2_10_0, t0, z2_5_0);

  fe25519_square_struc(t0, z2_10_0);
  fe25519_square_struc(t1, t0);
  for (i = 2; i < 10; i += 2)
  {
    fe25519_square_struc(t0, t1);
    fe25519_square_struc(t1, t0);
  }
  fe25519_mul_struc(z2_20_0, t1, z2_10_0);

  fe25519_square_struc(t0, z2_20_0);
  fe25519_square_struc(t1, t0);
  for (i = 2; i < 20; i += 2)
  {
    fe25519_square_struc(t0, t1);
    fe25519_square_struc(t1, t0);
  }
  fe25519_mul_struc(t0, t1, z2_20_0);

  fe25519_square_struc(t1, t0);
  fe25519_square_struc(t0, t1);
  for (i = 2; i < 10; i += 2)
  {
    fe25519_square_struc(t1, t0);
    fe25519_square_struc(t0, t1);
  }
  fe25519_mul_struc(z2_50_0, t0, z2_10_0);

  fe25519_square_struc(t0, z2_50_0);
  fe25519_square_struc(t1, t0);
  for (i = 2; i < 50; i += 2)
  {
    fe25519_square_struc(t0, t1);
    fe25519_square_struc(t1, t0);
  }
  fe25519_mul_struc(z2_100_0, t1, z2_50_0);

  fe25519_square_struc(t1, z2_100_0);
  fe25519_square_struc(t0, t1);
  for (i = 2; i < 100; i += 2)
  {
    fe25519_square_struc(t1, t0);
    fe25519_square_struc(t0, t1);
  }
  fe25519_mul_struc(t1, t0, z2_100_0);

  fe25519_square_struc(t0, t1);
  fe25519_square_struc(t1, t0);
  for (i = 2; i < 50; i += 2)
  {
    fe25519_square_struc(t0, t1);
    fe25519_square_struc(t1, t0);
  }
  fe25519_mul_struc(t0, t1, z2_50_0);

  fe25519_square_struc(t1, t0);
  fe25519_square_struc(t0, t1);
  fe25519_square_struc(t1, t0);
  fe25519_square_struc(t0, t1);
  fe25519_square_struc(t1, t0);
  fe25519_mul_struc(r, t1, z11);
}

unsigned int fe25519_getparity_struc(const unsigned int x[32])
{
  unsigned int t[32];
  unsigned int i;
  for (i = 0; i < 32; i++)
    t[i] = x[i];
  freeze_struc(t);
  return t[0] & 1;
}

void fe25519_unpack_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i];
  r[31] &= 127;
}

void fe25519_pack_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i] % 256;

  unsigned int c, m;
  c = (1 ^ (((((r[31] - 127) ^ 127) & (r[31] ^ 127)) ^ (r[31] - 127)) >> 31)) & (1 ^ (((((127 - r[31]) ^ r[31]) & (127 ^ r[31])) ^ (127 - r[31])) >> 31));
  m = c;
  for (i = 30; i > 1; i--)
  {
    c = (1 ^ (((((r[i] - 255) ^ 255) & (r[i] ^ 255)) ^ (r[i] - 255)) >> 31)) & (1 ^ (((((255 - r[i]) ^ r[i]) & (255 ^ r[i])) ^ (255 - r[i])) >> 31));
    m *= c;
  }

  c = (1 ^ (((((r[0] - 237) ^ 237) & (r[0] ^ 237)) ^ (r[0] - 237)) >> 31));
  m *= c;

  r[31] -= m * 127;

  for (i = 30; i > 0; i--)
    r[i] -= m * 255;

  r[0] -= m * 237;
}
static void p1p1_to_p2_struc(unsigned int r[96], const unsigned int p[128])
{
  fe25519_mul_struc(r, p, p + 3 * 32);
  fe25519_mul_struc(r + 32, p + 32, p + 2 * 32);
  fe25519_mul_struc(r + 2 * 32, p + 2 * 32, p + 3 * 32);
}

static void p1p1_to_p3_struc(unsigned int r[128], const unsigned int p[128])
{
  p1p1_to_p2_struc(r, p);
  fe25519_mul_struc(r + 3 * 32, p, p + 32);
}

static void cmov_p3_struc(unsigned int r[128], const unsigned int p[128], unsigned int b)
{
  fe25519_cmov_struc(r, p, b);
  fe25519_cmov_struc(r + 32, p + 32, b);
  fe25519_cmov_struc(r + 2 * 32, p + 2 * 32, b);
  fe25519_cmov_struc(r + 3 * 32, p + 3 * 32, b);
}

static void dbl_p1p1_struc(unsigned int r[128], const unsigned int p[96])
{
  unsigned int a[32];
  unsigned int b[32];
  unsigned int c[32];
  unsigned int d[32];

  fe25519_square_struc(a, p);
  fe25519_square_struc(b, p + 32);
  fe25519_square_struc(c, p + 2 * 32);
  fe25519_add_struc(c, c, c);
  fe25519_neg_struc(d, a);

  fe25519_add_struc(r, p, p + 32);
  fe25519_square_struc(r, r);
  fe25519_sub_struc(r, r, a);
  fe25519_sub_struc(r, r, b);
  fe25519_add_struc(r + 2 * 32, d, b);
  fe25519_sub_struc(r + 3 * 32, r + 2 * 32, c);
  fe25519_sub_struc(r + 32, d, b);
}

static void add_p1p1_struc(unsigned int r[128], const unsigned int p[128], const unsigned int q[128])
{
  unsigned int ecd[32] = {0xA3, 0x78, 0x59, 0x13, 0xCA, 0x4D, 0xEB, 0x75, 0xAB, 0xD8, 0x41, 0x41, 0x4D, 0x0A, 0x70, 0x00,
                          0x98, 0xE8, 0x79, 0x77, 0x79, 0x40, 0xC7, 0x8C, 0x73, 0xFE, 0x6F, 0x2B, 0xEE, 0x6C, 0x03, 0x52};
  unsigned int a[32];
  unsigned int b[32];
  unsigned int c[32];
  unsigned int d[32];
  unsigned int t[32];
  unsigned int fd[32];
  fe25519_unpack_struc(fd, ecd);

  fe25519_sub_struc(a, p + 32, p);
  fe25519_sub_struc(t, q + 32, q);
  fe25519_mul_struc(a, a, t);
  fe25519_add_struc(b, p, p + 32);
  fe25519_add_struc(t, q, q + 32);
  fe25519_mul_struc(b, b, t);
  fe25519_mul_struc(c, p + 3 * 32, q + 3 * 32);
  fe25519_mul_struc(c, c, fd);
  fe25519_add_struc(c, c, c);
  fe25519_mul_struc(d, p + 2 * 32, q + 2 * 32);
  fe25519_add_struc(d, d, d);
  fe25519_sub_struc(r, b, a);
  fe25519_sub_struc(r + 3 * 32, d, c);
  fe25519_add_struc(r + 2 * 32, d, c);
  fe25519_add_struc(r + 32, b, a);
}

void ge25519_add_struc(unsigned int r[128], const unsigned int p[128], const unsigned int q[128])
{
  unsigned int grp1p1[128];
  add_p1p1_struc(grp1p1, p, q);
  p1p1_to_p3_struc(r, grp1p1);
}

void ge25519_scalarmult_struc(unsigned int r[128], const unsigned int p[128], const unsigned int s[32])
{
  unsigned int ge25519_neutral_x[32] = {0};
  unsigned int ge25519_neutral_y[32] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int ge25519_neutral_z[32] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int ge25519_neutral_t[32] = {0};

  unsigned int i, j, k;
  unsigned int g[128];

  fe25519_unpack_struc(g, ge25519_neutral_x);
  fe25519_unpack_struc(g + 32, ge25519_neutral_y);
  fe25519_unpack_struc(g + 2 * 32, ge25519_neutral_z);
  fe25519_unpack_struc(g + 3 * 32, ge25519_neutral_t);

  unsigned int n = (1 << 2);
  unsigned int pre[n * 128];
  unsigned int t[128];
  unsigned int tp1p1[128];
  unsigned int w;
  unsigned int sb[32];
  sc25519_to32bytes_struc(sb, s);

  for (i = 0; i < 128; i++)
    pre[i] = g[i];

  for (i = 0; i < 128; i++)
    pre[i + 128] = p[i];
  for (i = 2; i < n; i += 2)
  {
    dbl_p1p1_struc(tp1p1, pre + i / 2 * 128);
    p1p1_to_p3_struc(pre + i * 128, tp1p1);
    add_p1p1_struc(tp1p1, pre + i * 128, pre + 128);
    p1p1_to_p3_struc(pre + (i + 1) * 128, tp1p1);
  }

  unsigned int c, jt;
  unsigned int X[128];
  unsigned int Y[128];

  unsigned int lw = 8 - 2;
  for (i = 32; i > 0; i--)
  {
    for (j = 0; j <= lw; j += 2)
    {
      jt = lw - j;
      for (k = 0; k < 2 - 1; k++)
      {
        dbl_p1p1_struc(tp1p1, g);
        p1p1_to_p2_struc(g, tp1p1);
      }
      dbl_p1p1_struc(tp1p1, g);
      p1p1_to_p3_struc(g, tp1p1);

      w = (sb[i - 1] >> jt) & 3;
      for (k = 0; k < 128; k++)
        t[k] = pre[k];
      for (k = 1; k < n; k++)
      {
        c = (1 ^ (((((k - w) ^ w) & (k ^ w)) ^ (k - w)) >> 31)) & (1 ^ (((((w - k) ^ k) & (w ^ k)) ^ (w - k)) >> 31));
        cmov_p3_struc(t, pre + k * 128, c);
      }

      add_p1p1_struc(tp1p1, g, t);
      c = (1 ^ (1 ^ (((((jt - 0) ^ 0) & (jt ^ 0)) ^ (jt - 0)) >> 31)) & (1 ^ (((((0 - jt) ^ jt) & (0 ^ jt)) ^ (0 - jt)) >> 31)));
      p1p1_to_p2_struc(X, tp1p1);
      for (k = 0; k < 32; k++)
        X[3 * 32 + k] = g[3 * 32 + k];
      p1p1_to_p3_struc(Y, tp1p1);
      for (k = 0; k < 128; k++)
        g[k] = c * X[k] + (1 - c) * Y[k];
    }
  }
  for (i = 0; i < 32; i++)
  {
    r[i] = g[i];
    r[i + 32] = g[i + 32];
    r[i + 2 * 32] = g[i + 2 * 32];
    r[i + 3 * 32] = g[i + 3 * 32];
  }
}

void ge25519_scalarmult_base_struc(unsigned int r[128], const unsigned int s[32])
{
  unsigned int ge25519_base_x[32] = {0x1A, 0xD5, 0x25, 0x8F, 0x60, 0x2D, 0x56, 0xC9, 0xB2, 0xA7, 0x25, 0x95, 0x60, 0xC7, 0x2C, 0x69,
                                     0x5C, 0xDC, 0xD6, 0xFD, 0x31, 0xE2, 0xA4, 0xC0, 0xFE, 0x53, 0x6E, 0xCD, 0xD3, 0x36, 0x69, 0x21};
  unsigned int ge25519_base_y[32] = {0x58, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                                     0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66};
  unsigned int ge25519_base_z[32] = {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  unsigned int ge25519_base_t[32] = {0xA3, 0xDD, 0xB7, 0xA5, 0xB3, 0x8A, 0xDE, 0x6D, 0xF5, 0x52, 0x51, 0x77, 0x80, 0x9F, 0xF0, 0x20,
                                     0x7D, 0xE3, 0xAB, 0x64, 0x8E, 0x4E, 0xEA, 0x66, 0x65, 0x76, 0x8B, 0xD7, 0x0F, 0x5F, 0x87, 0x67};

  unsigned int t[128];
  fe25519_unpack_struc(t, ge25519_base_x);
  fe25519_unpack_struc(t + 32, ge25519_base_y);
  fe25519_unpack_struc(t + 2 * 32, ge25519_base_z);
  fe25519_unpack_struc(t + 3 * 32, ge25519_base_t);
  ge25519_scalarmult_struc(r, t, s);
}

void ge25519_pack_struc(unsigned int r[32], const unsigned int p[128])
{
  unsigned int tx[32], ty[32], zi[32];

  fe25519_invert_struc(zi, p + 2 * 32);
  fe25519_mul_struc(tx, p, zi);
  fe25519_mul_struc(ty, p + 32, zi);
  fe25519_pack_struc(r, ty);
  r[31] ^= fe25519_getparity_struc(tx) << 7;
}

void SHRge32(unsigned int x[2], const unsigned int c)

{
  x[1] = x[0] >> (c - 32);
  x[0] = 0;
}

void SHRlt32(unsigned int x[2], const unsigned int c)

{
  unsigned int t;
  t = 0xffffffff >> (32 - c);
  t &= x[0];
  t <<= (32 - c);

  x[1] >>= c;
  x[1] |= t;
  x[0] >>= c;
}

void SHLge32(unsigned int x[2], const unsigned int c)

{
  x[0] = x[1] << (c - 32);
  x[1] = 0;
}

void SHLlt32(unsigned int x[2], const unsigned int c)

{
  unsigned int t;
  t = 0xffffffff << (32 - c);
  t &= x[1];
  t >>= (32 - c);

  x[0] <<= c;
  x[0] |= t;
  x[1] <<= c;
}

void ROTR28(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRlt32(a, 28);
  SHLge32(b, 36);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void ROTR34(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRge32(a, 34);
  SHLlt32(b, 30);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void ROTR39(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRge32(a, 39);
  SHLlt32(b, 25);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void Sigma0_struc(unsigned int r[2], const unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];
  unsigned int c[2];
  c[0] = x[0];
  c[1] = x[1];

  ROTR28(a);
  ROTR34(b);
  ROTR39(c);
  r[0] = a[0] ^ b[0] ^ c[0];
  r[1] = a[1] ^ b[1] ^ c[1];
}

void ROTR14(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRlt32(a, 14);
  SHLge32(b, 50);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void ROTR18(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRlt32(a, 18);
  SHLge32(b, 46);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void ROTR41(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRge32(a, 41);
  SHLlt32(b, 23);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void Sigma1_struc(unsigned int r[2], const unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];
  unsigned int c[2];
  c[0] = x[0];
  c[1] = x[1];

  ROTR14(a);
  ROTR18(b);
  ROTR41(c);
  r[0] = a[0] ^ b[0] ^ c[0];
  r[1] = a[1] ^ b[1] ^ c[1];
}

void ROTR1(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRlt32(a, 1);
  SHLge32(b, 63);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void ROTR8(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRlt32(a, 8);
  SHLge32(b, 56);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void sigma0_struc(unsigned int r[2], const unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];
  unsigned int c[2];
  c[0] = x[0];
  c[1] = x[1];

  ROTR1(a);
  ROTR8(b);
  SHRlt32(c, 7);
  r[0] = a[0] ^ b[0] ^ c[0];
  r[1] = a[1] ^ b[1] ^ c[1];
}

void ROTR19(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRlt32(a, 19);
  SHLge32(b, 45);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void ROTR61(unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];

  SHRge32(a, 61);
  SHLlt32(b, 3);
  x[0] = a[0] | b[0];
  x[1] = a[1] | b[1];
}

void sigma1_struc(unsigned int r[2], const unsigned int x[2])
{
  unsigned int a[2];
  a[0] = x[0];
  a[1] = x[1];
  unsigned int b[2];
  b[0] = x[0];
  b[1] = x[1];
  unsigned int c[2];
  c[0] = x[0];
  c[1] = x[1];

  ROTR19(a);
  ROTR61(b);
  SHRlt32(c, 6);
  r[0] = a[0] ^ b[0] ^ c[0];
  r[1] = a[1] ^ b[1] ^ c[1];
}

void Ch_struc(unsigned int r[2], const unsigned int x[2], const unsigned int y[2], const unsigned int z[2])
{
  r[0] = (x[0] & y[0]) ^ (~x[0] & z[0]);
  r[1] = (x[1] & y[1]) ^ (~x[1] & z[1]);
}

void Maj_struc(unsigned int r[2], const unsigned int x[2], const unsigned int y[2], const unsigned int z[2])
{
  r[0] = (x[0] & y[0]) ^ (x[0] & z[0]) ^ (y[0] & z[0]);
  r[1] = (x[1] & y[1]) ^ (x[1] & z[1]) ^ (y[1] & z[1]);
}

void U64_ADD(unsigned int r[2], const unsigned int x[2], const unsigned int y[2])

{
  r[0] = x[0] + y[0];
  r[1] = x[1] + y[1];

  unsigned int c;
  c = (((((r[1] - y[1]) ^ y[1]) & (r[1] ^ y[1])) ^ (r[1] - y[1])) >> 31);
  r[0] += c;
}
void crypto_hashblocks_sha512_struc(unsigned int statebytes[16], const unsigned int in[32])

{
  unsigned int state[16];
  unsigned int a[2], b[2], c[2], d[2], e[2], f[2], g[2], h[2];
  unsigned int T1[2], T2[2];
  unsigned int rs[2], rc[2], rm[2];

  a[0] = statebytes[0];
  a[1] = statebytes[1];
  state[0] = a[0];
  state[1] = a[1];
  b[0] = statebytes[2];
  b[1] = statebytes[3];
  state[2] = b[0];
  state[3] = b[1];
  c[0] = statebytes[4];
  c[1] = statebytes[5];
  state[4] = c[0];
  state[5] = c[1];
  d[0] = statebytes[6];
  d[1] = statebytes[7];
  state[6] = d[0];
  state[7] = d[1];
  e[0] = statebytes[8];
  e[1] = statebytes[9];
  state[8] = e[0];
  state[9] = e[1];
  f[0] = statebytes[10];
  f[1] = statebytes[11];
  state[10] = f[0];
  state[11] = f[1];
  g[0] = statebytes[12];
  g[1] = statebytes[13];
  state[12] = g[0];
  state[13] = g[1];
  h[0] = statebytes[14];
  h[1] = statebytes[15];
  state[14] = h[0];
  state[15] = h[1];

  unsigned int w0[2], w1[2], w2[2], w3[2], w4[2], w5[2], w6[2], w7[2],
      w8[2], w9[2], w10[2], w11[2], w12[2], w13[2], w14[2], w15[2];

  unsigned int F0_k0[2], F0_k1[2], F0_k2[2], F0_k3[2], F0_k4[2], F0_k5[2], F0_k6[2], F0_k7[2],
      F0_k8[2], F0_k9[2], F0_k10[2], F0_k11[2], F0_k12[2], F0_k13[2], F0_k14[2], F0_k15[2];
  F0_k0[0] = 1116352408;
  F0_k0[1] = 3609767458;
  F0_k1[0] = 1899447441;
  F0_k1[1] = 602891725;
  F0_k2[0] = 3049323471;
  F0_k2[1] = 3964484399;
  F0_k3[0] = 3921009573;
  F0_k3[1] = 2173295548;
  F0_k4[0] = 961987163;
  F0_k4[1] = 4081628472;
  F0_k5[0] = 1508970993;
  F0_k5[1] = 3053834265;
  F0_k6[0] = 2453635748;
  F0_k6[1] = 2937671579;
  F0_k7[0] = 2870763221;
  F0_k7[1] = 3664609560;
  F0_k8[0] = 3624381080;
  F0_k8[1] = 2734883394;
  F0_k9[0] = 310598401;
  F0_k9[1] = 1164996542;
  F0_k10[0] = 607225278;
  F0_k10[1] = 1323610764;
  F0_k11[0] = 1426881987;
  F0_k11[1] = 3590304994;
  F0_k12[0] = 1925078388;
  F0_k12[1] = 4068182383;
  F0_k13[0] = 2162078206;
  F0_k13[1] = 991336113;
  F0_k14[0] = 2614888103;
  F0_k14[1] = 633803317;
  F0_k15[0] = 3248222580;
  F0_k15[1] = 3479774868;

  unsigned int F1_k0[2], F1_k1[2], F1_k2[2], F1_k3[2], F1_k4[2], F1_k5[2], F1_k6[2], F1_k7[2],
      F1_k8[2], F1_k9[2], F1_k10[2], F1_k11[2], F1_k12[2], F1_k13[2], F1_k14[2], F1_k15[2];
  F1_k0[0] = 3835390401;
  F1_k0[1] = 2666613458;
  F1_k1[0] = 4022224774;
  F1_k1[1] = 944711139;
  F1_k2[0] = 264347078;
  F1_k2[1] = 2341262773;
  F1_k3[0] = 604807628;
  F1_k3[1] = 2007800933;
  F1_k4[0] = 770255983;
  F1_k4[1] = 1495990901;
  F1_k5[0] = 1249150122;
  F1_k5[1] = 1856431235;
  F1_k6[0] = 1555081692;
  F1_k6[1] = 3175218132;
  F1_k7[0] = 1996064986;
  F1_k7[1] = 2198950837;
  F1_k8[0] = 2554220882;
  F1_k8[1] = 3999719339;
  F1_k9[0] = 2821834349;
  F1_k9[1] = 766784016;
  F1_k10[0] = 2952996808;
  F1_k10[1] = 2566594879;
  F1_k11[0] = 3210313671;
  F1_k11[1] = 3203337956;
  F1_k12[0] = 3336571891;
  F1_k12[1] = 1034457026;
  F1_k13[0] = 3584528711;
  F1_k13[1] = 2466948901;
  F1_k14[0] = 113926993;
  F1_k14[1] = 3758326383;
  F1_k15[0] = 338241895;
  F1_k15[1] = 168717936;

  unsigned int F2_k0[2], F2_k1[2], F2_k2[2], F2_k3[2], F2_k4[2], F2_k5[2], F2_k6[2], F2_k7[2],
      F2_k8[2], F2_k9[2], F2_k10[2], F2_k11[2], F2_k12[2], F2_k13[2], F2_k14[2], F2_k15[2];
  F2_k0[0] = 666307205;
  F2_k0[1] = 1188179964;
  F2_k1[0] = 773529912;
  F2_k1[1] = 1546045734;
  F2_k2[0] = 1294757372;
  F2_k2[1] = 1522805485;
  F2_k3[0] = 1396182291;
  F2_k3[1] = 2643833823;
  F2_k4[0] = 1695183700;
  F2_k4[1] = 2343527390;
  F2_k5[0] = 1986661051;
  F2_k5[1] = 1014477480;
  F2_k6[0] = 2177026350;
  F2_k6[1] = 1206759142;
  F2_k7[0] = 2456956037;
  F2_k7[1] = 344077627;
  F2_k8[0] = 2730485921;
  F2_k8[1] = 1290863460;
  F2_k9[0] = 2820302411;
  F2_k9[1] = 3158454273;
  F2_k10[0] = 3259730800;
  F2_k10[1] = 3505952657;
  F2_k11[0] = 3345764771;
  F2_k11[1] = 106217008;
  F2_k12[0] = 3516065817;
  F2_k12[1] = 3606008344;
  F2_k13[0] = 3600352804;
  F2_k13[1] = 1432725776;
  F2_k14[0] = 4094571909;
  F2_k14[1] = 1467031594;
  F2_k15[0] = 275423344;
  F2_k15[1] = 851169720;

  unsigned int F3_k0[2], F3_k1[2], F3_k2[2], F3_k3[2], F3_k4[2], F3_k5[2], F3_k6[2], F3_k7[2],
      F3_k8[2], F3_k9[2], F3_k10[2], F3_k11[2], F3_k12[2], F3_k13[2], F3_k14[2], F3_k15[2];
  F3_k0[0] = 430227734;
  F3_k0[1] = 3100823752;
  F3_k1[0] = 506948616;
  F3_k1[1] = 1363258195;
  F3_k2[0] = 659060556;
  F3_k2[1] = 3750685593;
  F3_k3[0] = 883997877;
  F3_k3[1] = 3785050280;
  F3_k4[0] = 958139571;
  F3_k4[1] = 3318307427;
  F3_k5[0] = 1322822218;
  F3_k5[1] = 3812723403;
  F3_k6[0] = 1537002063;
  F3_k6[1] = 2003034995;
  F3_k7[0] = 1747873779;
  F3_k7[1] = 3602036899;
  F3_k8[0] = 1955562222;
  F3_k8[1] = 1575990012;
  F3_k9[0] = 2024104815;
  F3_k9[1] = 1125592928;
  F3_k10[0] = 2227730452;
  F3_k10[1] = 2716904306;
  F3_k11[0] = 2361852424;
  F3_k11[1] = 442776044;
  F3_k12[0] = 2428436474;
  F3_k12[1] = 593698344;
  F3_k13[0] = 2756734187;
  F3_k13[1] = 3733110249;
  F3_k14[0] = 3204031479;
  F3_k14[1] = 2999351573;
  F3_k15[0] = 3329325298;
  F3_k15[1] = 3815920427;

  unsigned int F4_k0[2], F4_k1[2], F4_k2[2], F4_k3[2], F4_k4[2], F4_k5[2], F4_k6[2], F4_k7[2],
      F4_k8[2], F4_k9[2], F4_k10[2], F4_k11[2], F4_k12[2], F4_k13[2], F4_k14[2], F4_k15[2];
  F4_k0[0] = 3391569614;
  F4_k0[1] = 3928383900;
  F4_k1[0] = 3515267271;
  F4_k1[1] = 566280711;
  F4_k2[0] = 3940187606;
  F4_k2[1] = 3454069534;
  F4_k3[0] = 4118630271;
  F4_k3[1] = 4000239992;
  F4_k4[0] = 116418474;
  F4_k4[1] = 1914138554;
  F4_k5[0] = 174292421;
  F4_k5[1] = 2731055270;
  F4_k6[0] = 289380356;
  F4_k6[1] = 3203993006;
  F4_k7[0] = 460393269;
  F4_k7[1] = 320620315;
  F4_k8[0] = 685471733;
  F4_k8[1] = 587496836;
  F4_k9[0] = 852142971;
  F4_k9[1] = 1086792851;
  F4_k10[0] = 1017036298;
  F4_k10[1] = 365543100;
  F4_k11[0] = 1126000580;
  F4_k11[1] = 2618297676;
  F4_k12[0] = 1288033470;
  F4_k12[1] = 3409855158;
  F4_k13[0] = 1501505948;
  F4_k13[1] = 4234509866;
  F4_k14[0] = 1607167915;
  F4_k14[1] = 987167468;
  F4_k15[0] = 1816402316;
  F4_k15[1] = 1246189591;

  w0[0] = in[0];
  w0[1] = in[1];
  w1[0] = in[2];
  w1[1] = in[3];
  w2[0] = in[4];
  w2[1] = in[5];
  w3[0] = in[6];
  w3[1] = in[7];
  w4[0] = in[8];
  w4[1] = in[9];
  w5[0] = in[10];
  w5[1] = in[11];
  w6[0] = in[12];
  w6[1] = in[13];
  w7[0] = in[14];
  w7[1] = in[15];
  w8[0] = in[16];
  w8[1] = in[17];
  w9[0] = in[18];
  w9[1] = in[19];
  w10[0] = in[20];
  w10[1] = in[21];
  w11[0] = in[22];
  w11[1] = in[23];
  w12[0] = in[24];
  w12[1] = in[25];
  w13[0] = in[26];
  w13[1] = in[27];
  w14[0] = in[28];
  w14[1] = in[29];
  w15[0] = in[30];
  w15[1] = in[31];

  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k0);
  U64_ADD(T1, T1, w0);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k1);
  U64_ADD(T1, T1, w1);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k2);
  U64_ADD(T1, T1, w2);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k3);
  U64_ADD(T1, T1, w3);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k4);
  U64_ADD(T1, T1, w4);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k5);
  U64_ADD(T1, T1, w5);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k6);
  U64_ADD(T1, T1, w6);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k7);
  U64_ADD(T1, T1, w7);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k8);
  U64_ADD(T1, T1, w8);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k9);
  U64_ADD(T1, T1, w9);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k10);
  U64_ADD(T1, T1, w10);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k11);
  U64_ADD(T1, T1, w11);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k12);
  U64_ADD(T1, T1, w12);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k13);
  U64_ADD(T1, T1, w13);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k14);
  U64_ADD(T1, T1, w14);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F0_k15);
  U64_ADD(T1, T1, w15);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;

  sigma1_struc(T1, w14);
  sigma0_struc(T2, w1);
  U64_ADD(w0, w0, T1);
  U64_ADD(w0, w0, w9);
  U64_ADD(w0, w0, T2);
  ;
  sigma1_struc(T1, w15);
  sigma0_struc(T2, w2);
  U64_ADD(w1, w1, T1);
  U64_ADD(w1, w1, w10);
  U64_ADD(w1, w1, T2);
  ;
  sigma1_struc(T1, w0);
  sigma0_struc(T2, w3);
  U64_ADD(w2, w2, T1);
  U64_ADD(w2, w2, w11);
  U64_ADD(w2, w2, T2);
  ;
  sigma1_struc(T1, w1);
  sigma0_struc(T2, w4);
  U64_ADD(w3, w3, T1);
  U64_ADD(w3, w3, w12);
  U64_ADD(w3, w3, T2);
  ;
  sigma1_struc(T1, w2);
  sigma0_struc(T2, w5);
  U64_ADD(w4, w4, T1);
  U64_ADD(w4, w4, w13);
  U64_ADD(w4, w4, T2);
  ;
  sigma1_struc(T1, w3);
  sigma0_struc(T2, w6);
  U64_ADD(w5, w5, T1);
  U64_ADD(w5, w5, w14);
  U64_ADD(w5, w5, T2);
  ;
  sigma1_struc(T1, w4);
  sigma0_struc(T2, w7);
  U64_ADD(w6, w6, T1);
  U64_ADD(w6, w6, w15);
  U64_ADD(w6, w6, T2);
  ;
  sigma1_struc(T1, w5);
  sigma0_struc(T2, w8);
  U64_ADD(w7, w7, T1);
  U64_ADD(w7, w7, w0);
  U64_ADD(w7, w7, T2);
  ;
  sigma1_struc(T1, w6);
  sigma0_struc(T2, w9);
  U64_ADD(w8, w8, T1);
  U64_ADD(w8, w8, w1);
  U64_ADD(w8, w8, T2);
  ;
  sigma1_struc(T1, w7);
  sigma0_struc(T2, w10);
  U64_ADD(w9, w9, T1);
  U64_ADD(w9, w9, w2);
  U64_ADD(w9, w9, T2);
  ;
  sigma1_struc(T1, w8);
  sigma0_struc(T2, w11);
  U64_ADD(w10, w10, T1);
  U64_ADD(w10, w10, w3);
  U64_ADD(w10, w10, T2);
  ;
  sigma1_struc(T1, w9);
  sigma0_struc(T2, w12);
  U64_ADD(w11, w11, T1);
  U64_ADD(w11, w11, w4);
  U64_ADD(w11, w11, T2);
  ;
  sigma1_struc(T1, w10);
  sigma0_struc(T2, w13);
  U64_ADD(w12, w12, T1);
  U64_ADD(w12, w12, w5);
  U64_ADD(w12, w12, T2);
  ;
  sigma1_struc(T1, w11);
  sigma0_struc(T2, w14);
  U64_ADD(w13, w13, T1);
  U64_ADD(w13, w13, w6);
  U64_ADD(w13, w13, T2);
  ;
  sigma1_struc(T1, w12);
  sigma0_struc(T2, w15);
  U64_ADD(w14, w14, T1);
  U64_ADD(w14, w14, w7);
  U64_ADD(w14, w14, T2);
  ;
  sigma1_struc(T1, w13);
  sigma0_struc(T2, w0);
  U64_ADD(w15, w15, T1);
  U64_ADD(w15, w15, w8);
  U64_ADD(w15, w15, T2);
  ;

  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k0);
  U64_ADD(T1, T1, w0);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k1);
  U64_ADD(T1, T1, w1);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k2);
  U64_ADD(T1, T1, w2);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k3);
  U64_ADD(T1, T1, w3);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k4);
  U64_ADD(T1, T1, w4);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k5);
  U64_ADD(T1, T1, w5);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k6);
  U64_ADD(T1, T1, w6);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k7);
  U64_ADD(T1, T1, w7);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k8);
  U64_ADD(T1, T1, w8);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k9);
  U64_ADD(T1, T1, w9);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k10);
  U64_ADD(T1, T1, w10);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k11);
  U64_ADD(T1, T1, w11);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k12);
  U64_ADD(T1, T1, w12);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k13);
  U64_ADD(T1, T1, w13);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k14);
  U64_ADD(T1, T1, w14);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F1_k15);
  U64_ADD(T1, T1, w15);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;

  sigma1_struc(T1, w14);
  sigma0_struc(T2, w1);
  U64_ADD(w0, w0, T1);
  U64_ADD(w0, w0, w9);
  U64_ADD(w0, w0, T2);
  ;
  sigma1_struc(T1, w15);
  sigma0_struc(T2, w2);
  U64_ADD(w1, w1, T1);
  U64_ADD(w1, w1, w10);
  U64_ADD(w1, w1, T2);
  ;
  sigma1_struc(T1, w0);
  sigma0_struc(T2, w3);
  U64_ADD(w2, w2, T1);
  U64_ADD(w2, w2, w11);
  U64_ADD(w2, w2, T2);
  ;
  sigma1_struc(T1, w1);
  sigma0_struc(T2, w4);
  U64_ADD(w3, w3, T1);
  U64_ADD(w3, w3, w12);
  U64_ADD(w3, w3, T2);
  ;
  sigma1_struc(T1, w2);
  sigma0_struc(T2, w5);
  U64_ADD(w4, w4, T1);
  U64_ADD(w4, w4, w13);
  U64_ADD(w4, w4, T2);
  ;
  sigma1_struc(T1, w3);
  sigma0_struc(T2, w6);
  U64_ADD(w5, w5, T1);
  U64_ADD(w5, w5, w14);
  U64_ADD(w5, w5, T2);
  ;
  sigma1_struc(T1, w4);
  sigma0_struc(T2, w7);
  U64_ADD(w6, w6, T1);
  U64_ADD(w6, w6, w15);
  U64_ADD(w6, w6, T2);
  ;
  sigma1_struc(T1, w5);
  sigma0_struc(T2, w8);
  U64_ADD(w7, w7, T1);
  U64_ADD(w7, w7, w0);
  U64_ADD(w7, w7, T2);
  ;
  sigma1_struc(T1, w6);
  sigma0_struc(T2, w9);
  U64_ADD(w8, w8, T1);
  U64_ADD(w8, w8, w1);
  U64_ADD(w8, w8, T2);
  ;
  sigma1_struc(T1, w7);
  sigma0_struc(T2, w10);
  U64_ADD(w9, w9, T1);
  U64_ADD(w9, w9, w2);
  U64_ADD(w9, w9, T2);
  ;
  sigma1_struc(T1, w8);
  sigma0_struc(T2, w11);
  U64_ADD(w10, w10, T1);
  U64_ADD(w10, w10, w3);
  U64_ADD(w10, w10, T2);
  ;
  sigma1_struc(T1, w9);
  sigma0_struc(T2, w12);
  U64_ADD(w11, w11, T1);
  U64_ADD(w11, w11, w4);
  U64_ADD(w11, w11, T2);
  ;
  sigma1_struc(T1, w10);
  sigma0_struc(T2, w13);
  U64_ADD(w12, w12, T1);
  U64_ADD(w12, w12, w5);
  U64_ADD(w12, w12, T2);
  ;
  sigma1_struc(T1, w11);
  sigma0_struc(T2, w14);
  U64_ADD(w13, w13, T1);
  U64_ADD(w13, w13, w6);
  U64_ADD(w13, w13, T2);
  ;
  sigma1_struc(T1, w12);
  sigma0_struc(T2, w15);
  U64_ADD(w14, w14, T1);
  U64_ADD(w14, w14, w7);
  U64_ADD(w14, w14, T2);
  ;
  sigma1_struc(T1, w13);
  sigma0_struc(T2, w0);
  U64_ADD(w15, w15, T1);
  U64_ADD(w15, w15, w8);
  U64_ADD(w15, w15, T2);
  ;

  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k0);
  U64_ADD(T1, T1, w0);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k1);
  U64_ADD(T1, T1, w1);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k2);
  U64_ADD(T1, T1, w2);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k3);
  U64_ADD(T1, T1, w3);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k4);
  U64_ADD(T1, T1, w4);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k5);
  U64_ADD(T1, T1, w5);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k6);
  U64_ADD(T1, T1, w6);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k7);
  U64_ADD(T1, T1, w7);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k8);
  U64_ADD(T1, T1, w8);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k9);
  U64_ADD(T1, T1, w9);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k10);
  U64_ADD(T1, T1, w10);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k11);
  U64_ADD(T1, T1, w11);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k12);
  U64_ADD(T1, T1, w12);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k13);
  U64_ADD(T1, T1, w13);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k14);
  U64_ADD(T1, T1, w14);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F2_k15);
  U64_ADD(T1, T1, w15);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;

  sigma1_struc(T1, w14);
  sigma0_struc(T2, w1);
  U64_ADD(w0, w0, T1);
  U64_ADD(w0, w0, w9);
  U64_ADD(w0, w0, T2);
  ;
  sigma1_struc(T1, w15);
  sigma0_struc(T2, w2);
  U64_ADD(w1, w1, T1);
  U64_ADD(w1, w1, w10);
  U64_ADD(w1, w1, T2);
  ;
  sigma1_struc(T1, w0);
  sigma0_struc(T2, w3);
  U64_ADD(w2, w2, T1);
  U64_ADD(w2, w2, w11);
  U64_ADD(w2, w2, T2);
  ;
  sigma1_struc(T1, w1);
  sigma0_struc(T2, w4);
  U64_ADD(w3, w3, T1);
  U64_ADD(w3, w3, w12);
  U64_ADD(w3, w3, T2);
  ;
  sigma1_struc(T1, w2);
  sigma0_struc(T2, w5);
  U64_ADD(w4, w4, T1);
  U64_ADD(w4, w4, w13);
  U64_ADD(w4, w4, T2);
  ;
  sigma1_struc(T1, w3);
  sigma0_struc(T2, w6);
  U64_ADD(w5, w5, T1);
  U64_ADD(w5, w5, w14);
  U64_ADD(w5, w5, T2);
  ;
  sigma1_struc(T1, w4);
  sigma0_struc(T2, w7);
  U64_ADD(w6, w6, T1);
  U64_ADD(w6, w6, w15);
  U64_ADD(w6, w6, T2);
  ;
  sigma1_struc(T1, w5);
  sigma0_struc(T2, w8);
  U64_ADD(w7, w7, T1);
  U64_ADD(w7, w7, w0);
  U64_ADD(w7, w7, T2);
  ;
  sigma1_struc(T1, w6);
  sigma0_struc(T2, w9);
  U64_ADD(w8, w8, T1);
  U64_ADD(w8, w8, w1);
  U64_ADD(w8, w8, T2);
  ;
  sigma1_struc(T1, w7);
  sigma0_struc(T2, w10);
  U64_ADD(w9, w9, T1);
  U64_ADD(w9, w9, w2);
  U64_ADD(w9, w9, T2);
  ;
  sigma1_struc(T1, w8);
  sigma0_struc(T2, w11);
  U64_ADD(w10, w10, T1);
  U64_ADD(w10, w10, w3);
  U64_ADD(w10, w10, T2);
  ;
  sigma1_struc(T1, w9);
  sigma0_struc(T2, w12);
  U64_ADD(w11, w11, T1);
  U64_ADD(w11, w11, w4);
  U64_ADD(w11, w11, T2);
  ;
  sigma1_struc(T1, w10);
  sigma0_struc(T2, w13);
  U64_ADD(w12, w12, T1);
  U64_ADD(w12, w12, w5);
  U64_ADD(w12, w12, T2);
  ;
  sigma1_struc(T1, w11);
  sigma0_struc(T2, w14);
  U64_ADD(w13, w13, T1);
  U64_ADD(w13, w13, w6);
  U64_ADD(w13, w13, T2);
  ;
  sigma1_struc(T1, w12);
  sigma0_struc(T2, w15);
  U64_ADD(w14, w14, T1);
  U64_ADD(w14, w14, w7);
  U64_ADD(w14, w14, T2);
  ;
  sigma1_struc(T1, w13);
  sigma0_struc(T2, w0);
  U64_ADD(w15, w15, T1);
  U64_ADD(w15, w15, w8);
  U64_ADD(w15, w15, T2);
  ;

  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k0);
  U64_ADD(T1, T1, w0);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k1);
  U64_ADD(T1, T1, w1);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k2);
  U64_ADD(T1, T1, w2);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k3);
  U64_ADD(T1, T1, w3);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k4);
  U64_ADD(T1, T1, w4);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k5);
  U64_ADD(T1, T1, w5);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k6);
  U64_ADD(T1, T1, w6);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k7);
  U64_ADD(T1, T1, w7);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k8);
  U64_ADD(T1, T1, w8);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k9);
  U64_ADD(T1, T1, w9);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k10);
  U64_ADD(T1, T1, w10);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k11);
  U64_ADD(T1, T1, w11);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k12);
  U64_ADD(T1, T1, w12);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k13);
  U64_ADD(T1, T1, w13);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k14);
  U64_ADD(T1, T1, w14);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F3_k15);
  U64_ADD(T1, T1, w15);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;

  sigma1_struc(T1, w14);
  sigma0_struc(T2, w1);
  U64_ADD(w0, w0, T1);
  U64_ADD(w0, w0, w9);
  U64_ADD(w0, w0, T2);
  ;
  sigma1_struc(T1, w15);
  sigma0_struc(T2, w2);
  U64_ADD(w1, w1, T1);
  U64_ADD(w1, w1, w10);
  U64_ADD(w1, w1, T2);
  ;
  sigma1_struc(T1, w0);
  sigma0_struc(T2, w3);
  U64_ADD(w2, w2, T1);
  U64_ADD(w2, w2, w11);
  U64_ADD(w2, w2, T2);
  ;
  sigma1_struc(T1, w1);
  sigma0_struc(T2, w4);
  U64_ADD(w3, w3, T1);
  U64_ADD(w3, w3, w12);
  U64_ADD(w3, w3, T2);
  ;
  sigma1_struc(T1, w2);
  sigma0_struc(T2, w5);
  U64_ADD(w4, w4, T1);
  U64_ADD(w4, w4, w13);
  U64_ADD(w4, w4, T2);
  ;
  sigma1_struc(T1, w3);
  sigma0_struc(T2, w6);
  U64_ADD(w5, w5, T1);
  U64_ADD(w5, w5, w14);
  U64_ADD(w5, w5, T2);
  ;
  sigma1_struc(T1, w4);
  sigma0_struc(T2, w7);
  U64_ADD(w6, w6, T1);
  U64_ADD(w6, w6, w15);
  U64_ADD(w6, w6, T2);
  ;
  sigma1_struc(T1, w5);
  sigma0_struc(T2, w8);
  U64_ADD(w7, w7, T1);
  U64_ADD(w7, w7, w0);
  U64_ADD(w7, w7, T2);
  ;
  sigma1_struc(T1, w6);
  sigma0_struc(T2, w9);
  U64_ADD(w8, w8, T1);
  U64_ADD(w8, w8, w1);
  U64_ADD(w8, w8, T2);
  ;
  sigma1_struc(T1, w7);
  sigma0_struc(T2, w10);
  U64_ADD(w9, w9, T1);
  U64_ADD(w9, w9, w2);
  U64_ADD(w9, w9, T2);
  ;
  sigma1_struc(T1, w8);
  sigma0_struc(T2, w11);
  U64_ADD(w10, w10, T1);
  U64_ADD(w10, w10, w3);
  U64_ADD(w10, w10, T2);
  ;
  sigma1_struc(T1, w9);
  sigma0_struc(T2, w12);
  U64_ADD(w11, w11, T1);
  U64_ADD(w11, w11, w4);
  U64_ADD(w11, w11, T2);
  ;
  sigma1_struc(T1, w10);
  sigma0_struc(T2, w13);
  U64_ADD(w12, w12, T1);
  U64_ADD(w12, w12, w5);
  U64_ADD(w12, w12, T2);
  ;
  sigma1_struc(T1, w11);
  sigma0_struc(T2, w14);
  U64_ADD(w13, w13, T1);
  U64_ADD(w13, w13, w6);
  U64_ADD(w13, w13, T2);
  ;
  sigma1_struc(T1, w12);
  sigma0_struc(T2, w15);
  U64_ADD(w14, w14, T1);
  U64_ADD(w14, w14, w7);
  U64_ADD(w14, w14, T2);
  ;
  sigma1_struc(T1, w13);
  sigma0_struc(T2, w0);
  U64_ADD(w15, w15, T1);
  U64_ADD(w15, w15, w8);
  U64_ADD(w15, w15, T2);
  ;

  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k0);
  U64_ADD(T1, T1, w0);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k1);
  U64_ADD(T1, T1, w1);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k2);
  U64_ADD(T1, T1, w2);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k3);
  U64_ADD(T1, T1, w3);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k4);
  U64_ADD(T1, T1, w4);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k5);
  U64_ADD(T1, T1, w5);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k6);
  U64_ADD(T1, T1, w6);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k7);
  U64_ADD(T1, T1, w7);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k8);
  U64_ADD(T1, T1, w8);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k9);
  U64_ADD(T1, T1, w9);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k10);
  U64_ADD(T1, T1, w10);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k11);
  U64_ADD(T1, T1, w11);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k12);
  U64_ADD(T1, T1, w12);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k13);
  U64_ADD(T1, T1, w13);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k14);
  U64_ADD(T1, T1, w14);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;
  Sigma1_struc(rs, e);
  Ch_struc(rc, e, f, g);
  U64_ADD(T1, h, rs);
  U64_ADD(T1, T1, rc);
  U64_ADD(T1, T1, F4_k15);
  U64_ADD(T1, T1, w15);
  Sigma0_struc(rs, a);
  Maj_struc(rm, a, b, c);
  U64_ADD(T2, rs, rm);
  h[0] = g[0];
  h[1] = g[1];
  g[0] = f[0];
  g[1] = f[1];
  f[0] = e[0];
  f[1] = e[1];
  U64_ADD(e, d, T1);
  d[0] = c[0];
  d[1] = c[1];
  c[0] = b[0];
  c[1] = b[1];
  b[0] = a[0];
  b[1] = a[1];
  U64_ADD(a, T1, T2);
  ;

  U64_ADD(a, a, state);
  U64_ADD(b, b, state + 2);
  U64_ADD(c, c, state + 4);
  U64_ADD(d, d, state + 6);
  U64_ADD(e, e, state + 8);
  U64_ADD(f, f, state + 10);
  U64_ADD(g, g, state + 12);
  U64_ADD(h, h, state + 14);

  state[0] = a[0];
  state[1] = a[1];
  state[2] = b[0];
  state[3] = b[1];
  state[4] = c[0];
  state[5] = c[1];
  state[6] = d[0];
  state[7] = d[1];
  state[8] = e[0];
  state[9] = e[1];
  state[10] = f[0];
  state[11] = f[1];
  state[12] = g[0];
  state[13] = g[1];
  state[14] = h[0];
  state[15] = h[1];

  unsigned int i;
  for (i = 0; i < 16; i++)
    statebytes[i] = state[i];
}

unsigned int crypto_hash_sha512_struc(unsigned int out[16], const unsigned int in[24])

{
  unsigned int iv_struc[64] = {
      0x6a09e667, 0xf3bcc908,
      0xbb67ae85, 0x84caa73b,
      0x3c6ef372, 0xfe94f82b,
      0xa54ff53a, 0x5f1d36f1,
      0x510e527f, 0xade682d1,
      0x9b05688c, 0x2b3e6c1f,
      0x1f83d9ab, 0xfb41bd6b,
      0x5be0cd19, 0x137e2179};
  unsigned int inlen = 24;
  unsigned int h[16];
  unsigned int padded[64];
  unsigned int i;

  for (i = 0; i < 16; ++i)
    h[i] = iv_struc[i];

  for (i = 0; i < inlen; ++i)
    padded[i] = in[i];
  padded[inlen] = 0x80000000;

  for (i = inlen + 1; i < 29; ++i)
    padded[i] = 0;
  padded[29] = 0;
  padded[30] = 0;

  padded[31] = 768;
  crypto_hashblocks_sha512_struc(h, padded);

  for (i = 0; i < 16; ++i)
    out[i] = h[i];
  return 0;
}

unsigned int crypto_sign_struc(
    unsigned int *sm,
    const unsigned int *m,
    const unsigned int *sk)

{
  unsigned int mlen = 16;
  unsigned int sck[32], scs[32], scsk[32];
  unsigned int sbk[32];
  unsigned int ger[128];
  unsigned int r[32];
  unsigned int s[32];
  unsigned int i;
  unsigned int hmg[16];
  unsigned int hmr[16];
  unsigned int hm[64];

  for (i = 0; i < mlen; i++)
    sm[8 + i] = m[i];
  for (i = 0; i < 8; i++)
    sm[i] = sk[8 + i];

  crypto_hash_sha512_struc(hmg, sm);
  for (i = 0; i < 16; i++)
  {
    hm[i * 4] = hmg[i] >> 24;
    hm[i * 4 + 1] = (hmg[i] >> 16) & 0xff;
    hm[i * 4 + 2] = (hmg[i] >> 8) & 0xff;
    hm[i * 4 + 3] = (hmg[i]) & 0xff;
  }

  sc25519_from64bytes_struc(sck, hm);
  ge25519_scalarmult_base_struc(ger, sck);
  ge25519_pack_struc(r, ger);

  for (i = 0; i < 8; i++)
  {
    sm[i] = (r[i * 4] << 24) |
            (r[i * 4 + 1] << 16) |
            (r[i * 4 + 2] << 8) |
            (r[i * 4 + 3]);
  }

  crypto_hash_sha512_struc(hmr, sm);
  for (i = 0; i < 16; i++)
  {
    hm[i * 4] = hmr[i] >> 24;
    hm[i * 4 + 1] = (hmr[i] >> 16) & 0xff;
    hm[i * 4 + 2] = (hmr[i] >> 8) & 0xff;
    hm[i * 4 + 3] = (hmr[i]) & 0xff;
  }
  sc25519_from64bytes_struc(scs, hm);
  sc25519_mul_struc(scs, scs, sck);

  for (i = 0; i < 8; i++)
  {
    sbk[i * 4] = sk[i] >> 24;
    sbk[i * 4 + 1] = (sk[i] >> 16) & 0xff;
    sbk[i * 4 + 2] = (sk[i] >> 8) & 0xff;
    sbk[i * 4 + 3] = (sk[i]) & 0xff;
  }
  sc25519_from32bytes_struc(scsk, sbk);
  sc25519_add_struc(scs, scs, scsk);

  sc25519_to32bytes_struc(s, scs);
  for (i = 0; i < 8; i++)
  {
    sm[mlen + 8 + i] = (s[i * 4] << 24) |
                       (s[i * 4 + 1] << 16) |
                       (s[i * 4 + 2] << 8) |
                       (s[i * 4 + 3]);
  }

  return 0;
}
