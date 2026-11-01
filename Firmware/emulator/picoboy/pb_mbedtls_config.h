// Nur was hashlib und cryptolib brauchen.
#ifndef PB_MBEDTLS_CONFIG_H
#define PB_MBEDTLS_CONFIG_H
#define MBEDTLS_MD5_C
#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_AES_C
#define MBEDTLS_CIPHER_MODE_CBC
#define MBEDTLS_CIPHER_MODE_CTR
#endif
