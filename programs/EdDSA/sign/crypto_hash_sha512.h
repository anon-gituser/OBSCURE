#ifndef CRYPTO_HASH_SHA512_H
#define CRYPTO_HASH_SHA512_H

#define crypto_hash_sha512_ref_BYTES 64
#define crypto_hash_sha512_BYTES crypto_hash_sha512_ref_BYTES

int crypto_hash_sha512(unsigned char *out,const unsigned char *in,unsigned long long inlen);

#endif