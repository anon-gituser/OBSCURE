#define fe25519_U32LEN 32

void fe25519_cmov_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], unsigned int b);
void fe25519_add_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int y[fe25519_U32LEN]);
void fe25519_sub_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int y[fe25519_U32LEN]);
void fe25519_mul_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int y[fe25519_U32LEN]);
void fe25519_setone_struc(unsigned int r[fe25519_U32LEN]);
void fe25519_setzero_struc(unsigned int r[fe25519_U32LEN]);
void fe25519_neg_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN]);
void fe25519_square_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN]);
void fe25519_pow_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], const unsigned int *e);
unsigned int fe25519_sqrt_vartime_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN], unsigned int parity);
void fe25519_invert_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[fe25519_U32LEN]);
unsigned int fe25519_getparity_struc(const unsigned int x[fe25519_U32LEN]);
void fe25519_unpack_struc(unsigned int r[fe25519_U32LEN], const unsigned int x[32]);
void fe25519_pack_struc(unsigned int r[32], const unsigned int x[fe25519_U32LEN]);
