PhotonBeetle
====

This folder contains 2 implementations of Ascon
(`photonbeetle128v1_*`), as well as a main (`test.c`) to check if both
those implementations seem functionally equivalent
(experimentally). Those 2 implementations are:

 - `photonbeetle128v1_orig.c`: the reference implementation, taken from
   the NIST submission (Implementations/crypto_aead/photonbeetleaead128rate128v1/ref)

 - `photonbeetle128v1_struc.c`: the reference implementation simplified a
   bit, so that our compiler can compile it.



## Differences between the 2 version

To obtain the `struct` from the `orig` version, we did the following
modifications:
 - removed the type `uint8_t` for a state. Used the type `unsigned int` instead. Groupped 4 bytes to an `unsigned int` number, then used shift operations to take bytes from that number.
 - Added the function named `rearrange_bytes` to arrange bytes in big-endian order, since our machines are little-endian.
 - removed `if` (whose condition depended on `adlen` and `mlen`).
 - removed the functions `memcpy`, `memset`, `memcmp` and replaced them with loops on arrays.
 - replaced `while` by `for`.
 - removed parameters `clen`, `mlen`, `adlen` and `nsec` (unused) in the prototypes.
 - fixed `mlen` and `adlen` to 16 in the function.
 - change type of pointers to `unsigned int*` instead of `uint8_t*`
 - changed all scalar types to `unsigned int`.
 - removed the function `selectConst` and fixed the constants.


## Compiling/obfuscating the _struc.c version

To generate a code that can be compiled from `ascon128v12_struc.c`,
you'll need to:

 - remove either `crypto_aead_encrypt_struc` or `crypto_aead_decrypt_struc`
 - run `cpp -P photonbeetle128v1_struc.c -o Photon.c` in order to
   remove comments and `#define`s


## Testing that both versions are (seemingly) equivalent

Simply run:

    make
    ./test
