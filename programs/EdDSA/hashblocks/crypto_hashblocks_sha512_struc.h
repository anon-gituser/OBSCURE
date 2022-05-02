
void SHRge32(unsigned int x[2], const unsigned int c); // OK
void SHRlt32(unsigned int x[2], const unsigned int c); // OK
void SHLge32(unsigned int x[2], const unsigned int c); // OK
void SHLlt32(unsigned int x[2], const unsigned int c); // OK
void ROTR28(unsigned int x[2]); // OK
void ROTR34(unsigned int x[2]); // OK
void ROTR39(unsigned int x[2]); // OK
void ROTR1(unsigned int x[2]); // OK
void ROTR8(unsigned int x[2]); // OK
void ROTR19(unsigned int x[2]); // OK
void ROTR61(unsigned int x[2]); // OK
void ROTR14(unsigned int x[2]); // OK
void ROTR18(unsigned int x[2]); // OK
void ROTR41(unsigned int x[2]); // OK
void Sigma0_struc(unsigned int r[2], const unsigned int x[2]); // OK
void sigma0_struc(unsigned int r[2], const unsigned int x[2]); // OK
void Sigma1_struc(unsigned int r[2], const unsigned int x[2]); // OK
void sigma1_struc(unsigned int r[2], const unsigned int x[2]); // OK
void Ch_struc(unsigned int r[2], const unsigned int x[2], const unsigned int y[2], const unsigned int z[2]); // OK
void Maj_struc(unsigned int r[2], const unsigned int x[2], const unsigned int y[2], const unsigned int z[2]); // OK
void U64_ADD(unsigned int r[2], const unsigned int x[2], const unsigned int y[2]); // OK
// M OK
// F OK

void crypto_hashblocks_sha512_struc(unsigned int statebytes[16], const unsigned int in[32]);