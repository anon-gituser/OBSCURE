void sc25519_to32bytes_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i];
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

void fe25519_neg_struc(unsigned int r[32], const unsigned int x[32])
{
  unsigned int t[32];
  unsigned int i;
  for (i = 0; i < 32; i++)
    t[i] = x[i];
  fe25519_setzero_struc(r);
  fe25519_sub_struc(r, r, t);
}

void fe25519_unpack_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i];
  r[31] &= 127;
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

unsigned int ge25519_scalarmult_base_struc(unsigned int r[128], const unsigned int s[32])
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
  return 0;
}
