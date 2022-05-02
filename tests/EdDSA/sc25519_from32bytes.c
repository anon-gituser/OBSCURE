

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

unsigned int sc25519_from32bytes_struc(unsigned int r[32], const unsigned int x[32])

{
  unsigned int i;
  unsigned int t[64] = {0};
  for (i = 0; i < 32; i++)
    t[i] = x[i];
  sc25519_barrett_reduce_struc(r, t);

  return 0;
}
