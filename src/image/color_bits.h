/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/

/* Macros for color packing/unpacking in a endian-independent manner. */
#ifndef _COLOR_BITS_H_
#define _COLOR_BITS_H_

#include "byteorder.h"

int _imago_rshift, _imago_gshift, _imago_bshift, _imago_ashift;

/* 32bit color shift values */
#ifdef LITTLE_ENDIAN
#define ALPHA_SHIFT32	24
#define RED_SHIFT32		16
#define GREEN_SHIFT32	8
#define BLUE_SHIFT32	0
#else	/* BIG_ENDIAN */
#define ALPHA_SHIFT32	0
#define RED_SHIFT32		8
#define GREEN_SHIFT32	16
#define BLUE_SHIFT32	24
#endif	/* LITTLE_ENDIAN */

/* these are used to initialize _imago_[rgba]shift */
#define DEFAULT_RSHIFT	RED_SHIFT32
#define DEFAULT_GSHIFT	GREEN_SHIFT32
#define DEFAULT_BSHIFT	BLUE_SHIFT32
#define DEFAULT_ASHIFT	ALPHA_SHIFT32

/* 32bit color mask values */
#define ALPHA_MASK32	(0xff << ALPHA_SHIFT32)
#define RED_MASK32		(0xff << RED_SHIFT32)
#define GREEN_MASK32	(0xff << GREEN_SHIFT32)
#define BLUE_MASK32		(0xff << BLUE_SHIFT32)

/* 16bit color shift values */
#ifdef LITTLE_ENDIAN
#define RED_SHIFT16		11
#define GREEN_SHIFT16	5
#define BLUE_SHIFT16	0
#else	/* BIG_ENDIAN */
#define RED_SHIFT16		0
#define GREEN_SHIFT16	5
#define BLUE_SHIFT16	11
#endif	/* LITTLE_ENDIAN */

/* 16bit color mask values */
#define RED_MASK16		(0x1f << RED_SHIFT16)
#define GREEN_MASK16	(0x3f << GREEN_SHIFT16)
#define BLUE_MASK16		(0x1f << BLUE_SHIFT16)

/* 15bit color shift values */
#ifdef LITTLE_ENDIAN
#define RED_SHIFT15		10
#define GREEN_SHIFT15	5
#define BLUE_SHIFT15	0
#else	/* BIG_ENDIAN */
#define RED_SHIFT15		0
#define GREEN_SHIFT15	5
#define BLUE_SHIFT15	10
#endif	/* LITTLE_ENDIAN */

/* 15bit color mask values */
#define RED_MASK15		(0x1f << RED_SHIFT15)
#define GREEN_MASK15	(0x1f << GREEN_SHIFT15)
#define BLUE_MASK15		(0x1f << BLUE_SHIFT15)


/* color packing macros */
#define PACK_COLOR32(a,r,g,b) \
	((((a) & 0xff) << _imago_ashift) | \
	 (((r) & 0xff) << _imago_rshift) | \
	 (((g) & 0xff) << _imago_gshift) | \
	 (((b) & 0xff) << _imago_bshift))

#define PACK_COLOR24(r,g,b)		PACK_COLOR32(0xff,r,g,b)

#define PACK_COLOR16(r,g,b) \
	(((uint16_t)(r) << RED_SHIFT16) & RED_MASK16) | \
	(((uint16_t)(g) << GREEN_SHIFT16) & GREEN_MASK16) | \
	(((uint16_t)(b) << BLUE_SHIFT16) & BLUE_MASK16)

#define PACK_COLOR15(r,g,b) \
	(((uint16_t)(r) << RED_SHIFT15) & RED_MASK15) | \
	(((uint16_t)(g) << GREEN_SHIFT15) & GREEN_MASK15) | \
	(((uint16_t)(b) << BLUE_SHIFT15) & BLUE_MASK15)

/* color unpacking macros */
#define UNP_RED32(p) \
	(((p) >> _imago_rshift) & 0xff)

#define UNP_GREEN32(p) \
	(((p) >> _imago_gshift) & 0xff)

#define UNP_BLUE32(p) \
	(((p) >> _imago_bshift) & 0xff)

#define UNP_ALPHA32(p) \
	(((p) >> _imago_ashift) & 0xff)

#endif	/* _COLOR_BITS_H_ */
