/*
 * Copyright (C) 2018 Zhiyi Zhang
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef NDN_SECURITY_RNG_H_
#define NDN_SECURITY_RNG_H_

#include "../ndn-error-code.h"
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ndn_rng_function type
 *
 * This type is the same as micro-ecc/uECC.h::uECC_RNG_Function
 *
 * The RNG function should fill 'size' random bytes into 'dest'.
 * It should return 1 if 'dest' was filled with random data,
 * or 0 if the random data could not be generated.
 * The filled-in values should be either truly random, or from a cryptographically-secure PRNG.
 * Setting a correctly functioning RNG function improves the resistance to side-channel attacks.
 **/
typedef int (*ndn_ECC_RNG_Function)(uint8_t* dest, unsigned size);

/**
 * Generate a random number. 
 *
 * YOU CANNOT USE THIS FUNCTION IF YOU ARE USING THE
 * DEFAULT SECURITY BACKEND.
 *
 * @param dest Buffer to store random number.
 * @param size Length of random number to generate.
 */
int ndn_lite_rng(uint8_t *dest, unsigned size);

#ifdef __cplusplus
}
#endif

#endif // NDN_SECURITY_RNG_H_
