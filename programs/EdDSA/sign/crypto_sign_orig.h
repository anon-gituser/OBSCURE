#ifndef CRYPTO_SIGN_H
#define CRYPTO_SIGN_H

int crypto_sign_keypair(
    unsigned char *pk,
    unsigned char *sk
    );

int crypto_sign_orig(
    unsigned char *sm,unsigned long long *smlen,
    const unsigned char *m,unsigned long long mlen,
    const unsigned char *sk
    );

int crypto_sign_open_orig(
    unsigned char *m,unsigned long long *mlen,
    const unsigned char *sm,unsigned long long smlen,
    const unsigned char *pk
    );

#endif