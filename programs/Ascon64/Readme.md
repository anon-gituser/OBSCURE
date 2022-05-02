Ascon64
====

This folder contains 2 implementations of Ascon
(`ascon128v12_*`), as well as a main (`test.c`) to check if both
those implementations seem functionally equivalent
(experimentally). Those 2 implementations are:

 - `ascon128v12_orig.c`: the reference implementation, taken from
   the NIST submission (Implementations/crypto_aead/ascon128v12/ref)

 - `ascon128v12_struc.c`: the reference implementation simplified a
   bit, so that our compiler can compile it.



## Differences between the 2 version

To obtain the `struct` from the `orig` version, we did the following
modifications:
 - removed the structure `state_t`. Used an array containing 5 elements instead.
 - replaced pass-by-address in function calls with pass-by-pointer.
 - removed `if` (whose condition depended on `adlen` and `mlen`).
 - removed parameters `clen`, `mlen`, `adlen` and `nsec` (unused) in the prototypes.
 - fixed `mlen` and `adlen` to 16 in the function.
 - change type of pointers to `unsigned long long*` instead of `unsigned char*`
    + removed conversion functions from `unsigned char *` to `unsigned long long *` and vice versa.
    + replaced `while` by `for` and changed the number of iterations.
 - changed all scalar types to `unsigned long long`.
 - removed all casts of numbers to `unsigned long long`.


## Compiling/obfuscating the _struc.c version

To generate a code that can be compiled from `ascon128v12_struc.c`,
you'll need to:

 - remove either `crypto_aead_encrypt_struc` or `crypto_aead_decrypt_struc`
 - run `cpp -P ascon128v12_struc.c -o Ascon.c` in order to
   remove comments and `#define`s


## Testing that both versions are (seemingly) equivalent

Simply run:

    make
    ./test
