/* set byte in 64-bit Ascon word */
static inline unsigned long long SETBYTE(unsigned long long b, unsigned long long i){
  return b << (56 - 8 * i);
}

/* set padding byte in 64-bit Ascon word */
static inline unsigned long long PAD(unsigned long long i){
  return SETBYTE(0x80, i);
}

static inline unsigned long long ROR(unsigned long long x, unsigned long long n) {
  return (x << (64 - n)) | (x >> n);
}

static inline void ROUND(unsigned long long* s, unsigned long long C) {
  unsigned long long t[5];
  /* addition of round constant */
  s[2] ^= C;
  /* substitution layer */
  s[0] ^= s[4];
  s[4] ^= s[3];
  s[2] ^= s[1];
  /* start of keccak s-box */
  t[0] = s[0] ^ (~s[1] & s[2]);
  t[1] = s[1] ^ (~s[2] & s[3]);
  t[2] = s[2] ^ (~s[3] & s[4]);
  t[3] = s[3] ^ (~s[4] & s[0]);
  t[4] = s[4] ^ (~s[0] & s[1]);
  /* end of keccak s-box */
  t[1] ^= t[0];
  t[0] ^= t[4];
  t[3] ^= t[2];
  t[2] = ~t[2];
  /* printstate(" substitution layer", &t); */
  /* linear diffusion layer */
  s[0] = t[0] ^ ROR(t[0], 19) ^ ROR(t[0], 28);
  s[1] = t[1] ^ ROR(t[1], 61) ^ ROR(t[1], 39);
  s[2] = t[2] ^ ROR(t[2], 1 ) ^ ROR(t[2], 6);
  s[3] = t[3] ^ ROR(t[3], 10) ^ ROR(t[3], 17);
  s[4] = t[4] ^ ROR(t[4], 7 ) ^ ROR(t[4], 41);
}

/************************** permutations.h *************************/
static inline void P12(unsigned long long* s) {
  ROUND(s, 0xf0);
  ROUND(s, 0xe1);
  ROUND(s, 0xd2);
  ROUND(s, 0xc3);
  ROUND(s, 0xb4);
  ROUND(s, 0xa5);
  ROUND(s, 0x96);
  ROUND(s, 0x87);
  ROUND(s, 0x78);
  ROUND(s, 0x69);
  ROUND(s, 0x5a);
  ROUND(s, 0x4b);
}

static inline void P6(unsigned long long* s) {
  ROUND(s, 0x96);
  ROUND(s, 0x87);
  ROUND(s, 0x78);
  ROUND(s, 0x69);
  ROUND(s, 0x5a);
  ROUND(s, 0x4b);
}
/*******************************************************************/

int crypto_aead_encrypt_struc(unsigned long long* c,
                        const unsigned long long* m,
                        const unsigned long long* ad,
                        const unsigned long long* npub,
                        const unsigned long long* k) {

  /* compute ASCON_128_IV */
  unsigned long long ASCON_128_KEYBYTES = 16;
  unsigned long long ASCON_128_RATE = 8;
  unsigned long long ASCON_128_PA_ROUNDS = 12;
  unsigned long long ASCON_128_PB_ROUNDS = 6;
  unsigned long long ASCON_128_IV = ((ASCON_128_KEYBYTES * 8) << 56) | \
                                    ((ASCON_128_RATE * 8) << 48) |     \
                                    ((ASCON_128_PA_ROUNDS) << 40) |    \
                                    ((ASCON_128_PB_ROUNDS) << 32);

  /* set ciphertext size */
  unsigned long long mlen = 2;  // 16 bytes
  unsigned long long adlen = 2; // 16 bytes
  // unsigned long long clen = 4;  // 16 bytes + 16 bytes

  /* load key and nonce */
  const unsigned long long K0 = k[0];
  const unsigned long long K1 = k[1];
  const unsigned long long N0 = npub[0];
  const unsigned long long N1 = npub[1];

  /* initialize */
  unsigned long long s[5];
  s[0] = ASCON_128_IV;
  s[1] = K0;
  s[2] = K1;
  s[3] = N0;
  s[4] = N1;
  P12(s);
  s[3] ^= K0;
  s[4] ^= K1;

  unsigned long long i;
  /* full associated data blocks */
  for (i = 0; i < adlen; i++) {
    s[0] ^= ad[i];
    P6(s);
  }
  /* final associated data block */
  s[0] ^= PAD(0);
  P6(s);
  /* domain separation */
  s[4] ^= 1;

  /* full plaintext blocks */
  for (i = 0; i < mlen; i++) {
    s[0] ^= m[i];
    c[i] = s[0];
    P6(s);
  }
  /* final plaintext block */
  s[0] ^= PAD(0);

  /* finalize */
  s[1] ^= K0;
  s[2] ^= K1;
  P12(s);
  s[3] ^= K0;
  s[4] ^= K1;

  /* set tag */
  c[2] = s[3];
  c[3] = s[4];

  return 0;
}

int crypto_aead_decrypt_struc(unsigned long long* m,
                        const unsigned long long* c,
                        const unsigned long long* ad,
                        const unsigned long long* npub,
                        const unsigned long long* k) {

  /* compute ASCON_128_IV */
  unsigned long long ASCON_128_KEYBYTES = 16;
  unsigned long long ASCON_128_RATE = 8;
  unsigned long long ASCON_128_PA_ROUNDS = 12;
  unsigned long long ASCON_128_PB_ROUNDS = 6;
  unsigned long long ASCON_128_IV = ((ASCON_128_KEYBYTES * 8) << 56) | \
                                    ((ASCON_128_RATE * 8) << 48) |     \
                                    ((ASCON_128_PA_ROUNDS) << 40) |    \
                                    ((ASCON_128_PB_ROUNDS) << 32);

  unsigned long long mlen = 2;  // 16 bytes
  unsigned long long adlen = 2; // 16 bytes
  unsigned long long tlen = 2;  // 16 bytes: tag
  unsigned long long clen = mlen + tlen;

  /* load key and nonce */
  const unsigned long long K0 = k[0];
  const unsigned long long K1 = k[1];
  const unsigned long long N0 = npub[0];
  const unsigned long long N1 = npub[1];

  /* initialize */
  unsigned long long s[5];
  s[0] = ASCON_128_IV;
  s[1] = K0;
  s[2] = K1;
  s[3] = N0;
  s[4] = N1;
  P12(s);
  s[3] ^= K0;
  s[4] ^= K1;

  unsigned long long i;
  /* full associated data blocks */
  for (i = 0; i < adlen; i++) {
    s[0] ^= ad[i];
    P6(s);
  }
  /* final associated data block */
  s[0] ^= PAD(0);
  P6(s);
  /* domain separation */
  s[4] ^= 1;

  /* full ciphertext blocks */
  clen -= tlen; // remove length for mac
  unsigned long long c0;
  for (i = 0; i < clen; i++) {
    c0 = c[i];
    m[i] = s[0] ^ c0;
    s[0] = c0;
    P6(s);
  }
  /* final ciphertext block */
  s[0] ^= PAD(0);

  /* finalize */
  s[1] ^= K0;
  s[2] ^= K1;
  P12(s);
  s[3] ^= K0;
  s[4] ^= K1;

  /* set tag */
  unsigned long long t[2];
  t[0] = s[3];
  t[1] = s[4];

  /* verify tag (should be constant time, check compiler output) */
  unsigned long long result = 0;
  for (i = 0; i < tlen; ++i)
    result |= c[i+clen] ^ t[i];

  /*result = 0 if OK, otherwise result != 0 (Not -1)*/
  result = (((result - 1) >> 8) & 1) - 1;
  return result;
}
