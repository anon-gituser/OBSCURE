
unsigned int sc25519_to32bytes_struc(unsigned int r[32], const unsigned int x[32])
{
  unsigned int i;
  for (i = 0; i < 32; i++)
    r[i] = x[i];

  return 0;
}
