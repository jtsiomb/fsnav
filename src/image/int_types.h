/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/

/* This file defines pecifies some useful types for cross-platform compatibility. */
#ifndef TYPES_H_
#define TYPES_H_

#include <stdlib.h>

#if (__STDC_VERSION__ >= 199900) || defined(__GLIBC__) || defined(__MACH__) || defined(HAVE_STDINT_H)
#include <stdint.h>
#elif defined(unix) || defined(__unix__)
#include <sys/types.h>
#elif defined(_MSC_VER)
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else

/* no stdint, not unix, not gcc, no MSVC, find out the hard way, from C limits */
#include <limits.h>

#if CHAR_BIT != 8
#error "all bets are off, no 8bit chars."
#endif	/* CHAR_BITS != 8 */

#if USHRT_MAX == 65535
typedef short int16_t;
typedef unsigned short uint16_t;
#elif UINT_MAX == 65535
typedef int int16_t;
typedef unsigned int uint16_t;
#else
#error "can't find a 16bit integer"
#endif	/* int16 */

#if USHRT_MAX == 4294967295U
typedef short int32_t;
typedef unsigned short int32_t;
#elif UINT_MAX == 4294967295U
typedef int int32_t;
typedef unsigned int uint32_t;
#elif ULONG_MAX == 4294967295U
typedef long int32_t;
typedef unsigned long uint32_t;
#else
#error "can't find a 32bit integer"
#endif	/* int32 */

/* NOTE: no reliable way to obtain an int64 if we don't know the
 * compiler, so skip that one for now, leave it undefined.
 */

#endif	/* stdint detection */

#endif	/* TYPES_H_ */
