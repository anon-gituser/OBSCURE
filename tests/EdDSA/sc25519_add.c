

void sc25519_reduce_add_sub_struc(unsigned int r[32])
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

unsigned int sc25519_add_struc(unsigned int r[32], const unsigned int x[32], const unsigned int y[32])
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
  return 0;
}
