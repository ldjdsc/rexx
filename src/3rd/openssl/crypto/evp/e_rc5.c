
#include <stdio.h>
#include "cryptlib.h"

#ifndef OPENSSL_NO_RC5

# include <openssl/evp.h>
# include <openssl/objects.h>
# include "evp_locl.h"
# include <openssl/rc5.h>

static int r_32_12_16_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                               const unsigned char *iv, int enc);
static int rc5_ctrl(EVP_CIPHER_CTX *c, int type, int arg, void *ptr);

typedef struct {
    int rounds;                 /* number of rounds */
    RC5_32_KEY ks;              /* key schedule */
} EVP_RC5_KEY;

# define data(ctx)       EVP_C_DATA(EVP_RC5_KEY,ctx)

IMPLEMENT_BLOCK_CIPHER(rc5_32_12_16, ks, RC5_32, EVP_RC5_KEY, NID_rc5,
                       8, RC5_32_KEY_LENGTH, 8, 64,
                       EVP_CIPH_VARIABLE_LENGTH | EVP_CIPH_CTRL_INIT,
                       r_32_12_16_init_key, NULL, NULL, NULL, rc5_ctrl)

static int rc5_ctrl(EVP_CIPHER_CTX *c, int type, int arg, void *ptr)
{
    switch (type) {
    case EVP_CTRL_INIT:
        data(c)->rounds = RC5_12_ROUNDS;
        return 1;

    case EVP_CTRL_GET_RC5_ROUNDS:
        *(int *)ptr = data(c)->rounds;
        return 1;

    case EVP_CTRL_SET_RC5_ROUNDS:
        switch (arg) {
        case RC5_8_ROUNDS:
        case RC5_12_ROUNDS:
        case RC5_16_ROUNDS:
            data(c)->rounds = arg;
            return 1;

        default:
            EVPerr(EVP_F_RC5_CTRL, EVP_R_UNSUPORTED_NUMBER_OF_ROUNDS);
            return 0;
        }

    default:
        return -1;
    }
}

static int r_32_12_16_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
                               const unsigned char *iv, int enc)
{
    RC5_32_set_key(&data(ctx)->ks, EVP_CIPHER_CTX_key_length(ctx),
                   key, data(ctx)->rounds);
    return 1;
}

#endif
