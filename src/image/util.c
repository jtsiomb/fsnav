/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
#include "image.h"
#include "util.h"
#include "color_bits.h"

#define CLAMP(x, a, b)	((x) < (a) ? (a) : ((x) > (b) ? (b) : (x)))

void conv_32bpp_to_float(uint32_t *src, float *dst, int xsz, int ysz)
{
	int i, sz = xsz * ysz;
	for(i=0; i<sz; i++) {
		dst[i * 4] = UNP_RED32(src[i]) / 255.0f;
		dst[i * 4 + 1] = UNP_GREEN32(src[i]) / 255.0f;
		dst[i * 4 + 2] = UNP_BLUE32(src[i]) / 255.0f;
		dst[i * 4 + 3] = UNP_ALPHA32(src[i]) / 255.0f;
	}
}

void conv_float_to_32bpp(float *src, uint32_t *dst, int xsz, int ysz)
{
	int i, sz = xsz * ysz;
	for(i=0; i<sz; i++) {
		int r = src[i * 4] * 255.0f;
		int g = src[i * 4 + 1] * 255.0f;
		int b = src[i * 4 + 2] * 255.0f;
		int a = src[i * 4 + 3] * 255.0f;
		dst[i] = PACK_COLOR32(CLAMP(a, 0, 255), CLAMP(r, 0, 255), CLAMP(g, 0, 255), CLAMP(b, 0, 255));
	}
}
