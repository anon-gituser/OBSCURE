#define ge25519_U32LEN 128
#define ge25519_p1p1_U32LEN 128
#define ge25519_p3_U32LEN 128
#define ge25519_p2_U32LEN 96

unsigned int ge25519_unpack_vartime_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[32]);
void ge25519_add_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[ge25519_p3_U32LEN], const unsigned int q[ge25519_p3_U32LEN]);
void ge25519_scalarmult_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int p[ge25519_p3_U32LEN], const unsigned int s[32]);
void ge25519_scalarmult_base_struc(unsigned int r[ge25519_p3_U32LEN], const unsigned int s[32]);
void ge25519_pack_struc(unsigned int r[32], const unsigned int p[ge25519_p3_U32LEN]);
