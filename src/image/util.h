/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
#ifndef UTIL_H_
#define UTIL_H_

#include "int_types.h"

void conv_32bpp_to_float(uint32_t *src, float *dst, int xsz, int ysz);
void conv_float_to_32bpp(float *src, uint32_t *dst, int xsz, int ysz);

#endif	/* UTIL_H_ */
