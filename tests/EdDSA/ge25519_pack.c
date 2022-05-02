
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

unsigned int fe25519_getparity_struc(const unsigned int x[32])
{
  unsigned int t[32];
  unsigned int i;
  for (i = 0; i < 32; i++)
    t[i] = x[i];
  freeze_struc(t);
  return t[0] & 1;
}

unsigned int ge25519_pack_struc(unsigned int r[32], const unsigned int p[128])
{
  unsigned int tx[32], ty[32], zi[32];

  fe25519_invert_struc(zi, p + 2 * 32);
  fe25519_mul_struc(tx, p, zi);
  fe25519_mul_struc(ty, p + 32, zi);
  fe25519_pack_struc(r, ty);
  r[31] ^= fe25519_getparity_struc(tx) << 7;
  return 0;
}
