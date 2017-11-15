/*
 * Copyright (C) 2017 BlueKitchen GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 * 4. Any redistribution, use, or modification is done solely for
 *    personal benefit and not for any commercial purpose or for
 *    monetary gain.
 *
 * THIS SOFTWARE IS PROVIDED BY BLUEKITCHEN GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHIAS
 * RINGWALD OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Please inquire about commercial licensing options at 
 * contact@bluekitchen-gmbh.com
 *
 */

/*
 * btstack_crypto.h
 *
 * Central place for all crypto-related functions with completion callbacks to allow
 * using of MCU crypto peripherals or the Bluetooth controller
 */

#ifndef __BTSTACK_CTRYPTO_H
#define __BTSTACK_CTRYPTO_H

#include "btstack_defines.h"

#if defined __cplusplus
extern "C" {
#endif

#define CMAC_TEMP_API

typedef enum {
	BTSTACK_CRYPTO_RANDOM,
	BTSTACK_CRYPTO_AES128,
	BTSTACK_CRYPTO_CMAC_GENERATOR,
	BTSTACK_CRYPTO_CMAC_MESSAGE,
	BTSTACK_CRYPTO_EC_P192_GENERATE_KEY,
	BTSTACK_CRYPTO_EC_P192_CALCULATE_DHKEY,
} btstack_crypto_operation_t;

typedef struct {
	btstack_context_callback_registration_t context_callback;
	btstack_crypto_operation_t              operation;	
} btstack_crypto_t;

typedef struct {
	btstack_crypto_t btstack_crypto;
	uint8_t  * buffer;
	uint16_t   size;
} btstack_crypto_random_t;

typedef struct {
	btstack_crypto_t btstack_crypto;
	const uint8_t  * key;
	const uint8_t  * plaintext;
	uint8_t  * ciphertext;
} btstack_crypto_aes128_t;

typedef struct {
	btstack_crypto_t btstack_crypto;
	const uint8_t  * key;
	uint16_t         size;
	union {
		uint8_t (*get_byte_callback)(uint16_t pos);
		const uint8_t * message;
	};
	uint8_t  * hash;
} btstack_crypto_aes128_cmac_t;

typedef struct {
	btstack_crypto_t btstack_crypto;
	uint8_t * public_key;
    uint8_t * dhkey;
} btstack_crypto_ec_p192_t;

/** 
 * Initialize crypto functions
 */
void btstack_crypto_init(void);

/** 
 * Generate random data
 * @param request
 * @param buffer for output
 * @param size of requested random data
 * @param callback
 * @param callback_arg
 * @note request needs to stay avaliable until callback (i.e. not provided on stack)
 */
void btstack_crypto_random_generate(btstack_crypto_random_t * request, uint8_t * buffer, uint16_t size, void (* callback)(void * arg), void * callback_arg);

/** 
 * Encrypt plaintext using AES128
 * @param request
 * @param key (16 bytes)
 * @param plaintext (16 bytes)
 * @param ciphertext (16 bytes)
 * @param callback
 * @param callback_arg
 * @note request needs to stay avaliable until callback (i.e. not provided on stack)
 */
void btstack_crypto_aes128_encrypt(btstack_crypto_aes128_t * request, const uint8_t * key, const uint8_t * plaintext, uint8_t * ciphertext, void (* callback)(void * arg), void * callback_arg);

/**
 * Calculate Cipher-based Message Authentication Code (CMAC) using AES128 and a generator function to provide data
 * @param request
 * @param key (16 bytes)
 * @param size of message
 * @param generator provides byte at requested position
 * @param callback
 * @param callback_arg
 */
void btstack_crypto_aes128_cmac_generator(btstack_crypto_aes128_cmac_t * request, const uint8_t * key, uint16_t size, uint8_t (*get_byte_callback)(uint16_t pos), uint8_t * hash, void (* callback)(void * arg), void * callback_arg);

/**
 * Calculate Cipher-based Message Authentication Code (CMAC) using AES128 and complete message
 * @param request
 * @param key (16 bytes)
 * @param len of message
 * @param message
 * @param hash result
 * @param callback
 * @param callback_arg
 */
void btstack_crypto_aes128_cmac_message(btstack_crypto_aes128_cmac_t * request, const uint8_t * key, uint16_t len, const uint8_t * message,  uint8_t * hash, void (* callback)(void * arg), void * callback_arg);

/**
 * Generate Elliptic Curve Public/Private Key Pair (FIPS P-256)
 * @note BTstack uses a single ECC key pair per reset. 
 * @note If LE Controller is used for ECC, private key cannot be read or managed
 * @param request
 * @param public_key (64 bytes)
 * @param callback
 * @param callback_arg
 */
void btstack_crypto_ec_p192_generate_key(btstack_crypto_ec_p192_t * request, uint8_t * public_key, void (* callback)(void * arg), void * callback_arg);

/**
 * Calculate Diffie-Hellman Key based on local private key and remote public key
 * @param request
 * @param public_key (64 bytes)
 * @param dhkey (32 bytes)
 * @param callback
 * @param callback_arg
 */
void btstack_crypto_ec_p192_calculate_dhkey(btstack_crypto_ec_p192_t * request, const uint8_t * public_key, uint8_t * dhkey, void (* callback)(void * arg), void * callback_arg);

#if defined __cplusplus
}
#endif

#endif /* __BTSTACK_CTRYPTO_H */
