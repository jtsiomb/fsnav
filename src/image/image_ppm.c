/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
#include "image.h"
#include <stdio.h>

#ifdef IMGLIB_USE_PPM

#include <stdlib.h>
#include <ctype.h>
#include "color_bits.h"
#include "util.h"


int check_ppm(FILE *fp) {
	fseek(fp, 0, SEEK_SET);
	if(fgetc(fp) == 'P' && fgetc(fp) == '6') {
		return 1;
	}
	return 0;
}

static int read_to_wspace(FILE *fp, char *buf, int bsize) {
	int c, count = 0;
	
	while((c = fgetc(fp)) != -1 && !isspace(c) && count < bsize - 1) {
		if(c == '#') {
			while((c = fgetc(fp)) != -1 && c != '\n' && c != '\r');
			c = fgetc(fp);
			if(c == '\n' || c == '\r') continue;
		}
		*buf++ = c;
		count++;
	}
	*buf = 0;
	
	while((c = fgetc(fp)) != -1 && isspace(c));
	ungetc(c, fp);
	return count;
}

void *load_ppm(FILE *fp, int *xsz, int *ysz) {
	char buf[64];
	int bytes, raw;
	unsigned int w, h, i, sz;
	uint32_t *pixels;
	
	fseek(fp, 0, SEEK_SET);
	
	bytes = read_to_wspace(fp, buf, 64);
	raw = buf[1] == '6';

	if((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		return 0;
	}
	if(!isdigit(*buf)) {
		fprintf(stderr, "load_ppm: invalid width: %s\n", buf);
		return 0;
	}
	w = atoi(buf);

	if((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		return 0;
	}
	if(!isdigit(*buf)) {
		fprintf(stderr, "load_ppm: invalid height: %s\n", buf);
		return 0;
	}
	h = atoi(buf);

	if((bytes = read_to_wspace(fp, buf, 64)) == 0) {
		return 0;
	}
	if(!isdigit(*buf) || atoi(buf) != 255) {
		fprintf(stderr, "load_ppm: invalid or unsupported max value: %s\n", buf);
		return 0;
	}

	if(!(pixels = malloc(w * h * sizeof *pixels))) {
		fprintf(stderr, "load_ppm: malloc failed\n");
		return 0;
	}

	sz = h * w;
	for(i=0; i<sz; i++) {
		int r = fgetc(fp);
		int g = fgetc(fp);
		int b = fgetc(fp);

		if(r == -1 || g == -1 || b == -1) {
			free(pixels);
			fprintf(stderr, "load_ppm: EOF while reading pixel data\n");
			return 0;
		}
		pixels[i] = PACK_COLOR24(r, g, b);
	}

	if(xsz) *xsz = w;
	if(ysz) *ysz = h;

	if(get_image_option(IMG_OPT_FLOAT)) {
		float *fpix;
		
		if(!(fpix = malloc(w * h * 4 * sizeof *fpix))) {
			free(pixels);
			return 0;
		}

		conv_32bpp_to_float(pixels, fpix, w, h);
		free(pixels);
		return fpix;
	}

	return pixels;
}

int save_ppm(FILE *fp, void *pixels, int xsz, int ysz) {
	int i, j;
	uint32_t *ptr;
	int save_text = get_image_option(IMG_OPT_TEXT);
	int save_inv = get_image_option(IMG_OPT_INVERT);
	int src_fmt_float = get_image_option(IMG_OPT_FLOAT);

	if(src_fmt_float) {
		void *rgba32 = malloc(xsz * ysz * sizeof(uint32_t));
		conv_float_to_32bpp(pixels, rgba32, xsz, ysz);
		pixels = rgba32;
	}

	fprintf(fp, "P%d\n%d %d\n255\n", save_text ? 3 : 6, xsz, ysz);

	ptr = save_inv ? (uint32_t*)pixels + (ysz - 1) * xsz : pixels;
	for(i=0; i<ysz; i++) {
		for(j=0; j<xsz; j++) {
			int r = UNP_RED32(ptr[j]);
			int g = UNP_GREEN32(ptr[j]);
			int b = UNP_BLUE32(ptr[j]);

			if(save_text) {
				fprintf(fp, "%d %d %d\n", r, g, b);
			} else {
				fputc(r, fp);
				fputc(g, fp);
				if(fputc(b, fp) == EOF) {
					fprintf(stderr, "save_ppm: failed to write to file\n");
					if(src_fmt_float) {
						free(pixels);
					}
					return -1;
				}
			}
		}

		ptr += save_inv ? -xsz : xsz;
	}

	if(src_fmt_float) {
		free(pixels);
	}
	return 0;
}

#endif	/* IMGLIB_USE_PPM */
