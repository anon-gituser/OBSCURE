#define sc25519_U32LEN 32

void sc25519_from32bytes_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[32]);
void sc25519_from64bytes_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[64]);
void sc25519_to32bytes_struc(unsigned int r[32], const unsigned int x[sc25519_U32LEN]);

void sc25519_add_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[sc25519_U32LEN], const unsigned int y[sc25519_U32LEN]);
void sc25519_mul_struc(unsigned int r[sc25519_U32LEN], const unsigned int x[sc25519_U32LEN], const unsigned int y[sc25519_U32LEN]);
