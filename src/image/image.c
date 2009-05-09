/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"
#include "color_bits.h"

#define FMT_DECL(x) \
	int check_##x(FILE *fp); \
	void *load_##x(FILE *fp, int *xsz, int *ysz); \
	int save_##x(FILE *fp, void *pixels, int xsz, int ysz)

#define FMT_FUNC(x) \
	{ check_##x, load_##x, save_##x }


#ifdef IMGLIB_USE_PNG
FMT_DECL(png);
#endif
#ifdef IMGLIB_USE_JPEG
FMT_DECL(jpeg);
#endif
#ifdef IMGLIB_USE_TGA
FMT_DECL(tga);
#endif
#ifdef IMGLIB_USE_PPM
FMT_DECL(ppm);
#endif
#ifdef IMGLIB_USE_RGBE
FMT_DECL(rgbe);
#endif

struct {
	int (*check)(FILE*);
	void *(*load)(FILE*, int*, int*);
	int (*save)(FILE*, void*, int, int);
} fmt_func[] = {
	{0, 0, 0},	/* this corresponds to the IMG_FMT_AUTO enum */
#ifdef IMGLIB_USE_PNG
	FMT_FUNC(png),
#endif
#ifdef IMGLIB_USE_JPEG
	FMT_FUNC(jpeg),
#endif
#ifdef IMGLIB_USE_TGA
	FMT_FUNC(tga),
#endif
#ifdef IMGLIB_USE_PPM
	FMT_FUNC(ppm),
#endif
#ifdef IMGLIB_USE_RGBE
	FMT_FUNC(rgbe),
#endif
	{0, 0, 0}	/* last */
};

static unsigned int options;
int _imago_rshift = DEFAULT_RSHIFT;
int _imago_gshift = DEFAULT_GSHIFT;
int _imago_bshift = DEFAULT_BSHIFT;
int _imago_ashift = DEFAULT_ASHIFT;

void *load_image(const char *fname, int *xsz, int *ysz) {
	int i;
	FILE *file;
	void *pix = 0;

	if(!(file = fopen(fname, "rb"))) {
		fprintf(stderr, "Image loading error: could not open file %s\n", fname);
		return 0;
	}

	for(i=1; fmt_func[i].check; i++) {
		if(fmt_func[i].check(file)) {
			pix = fmt_func[i].load(file, xsz, ysz);
			break;
		}
	}
	
	fclose(file);
	return pix;
}

void free_image(void *img) {
	free(img);
}

static const char *suffix_list[] = {
	0,
	".png",
	".jpg",
	".tga",
	".ppm",
	".rgbe",
	0
};

int save_image(const char *fname, void *pixels, int xsz, int ysz, unsigned int fmt) {
	FILE *fp;
	int res;

	if(fmt == IMG_FMT_AUTO) {
		int i;
		char *suffix = strrchr(fname, '.');

		if(suffix) {
			for(i=1; suffix_list[i]; i++) {
				if(strcmp(suffix, suffix_list[i]) == 0) {
					fmt = i;
					break;
				}
			}
		}

		if(!suffix || !suffix_list[i]) {
			fprintf(stderr, "Image saving error: failed to infer filetype (unknown suffix in %s)\n", fname);
			return -1;
		}
	}

	if(fmt >= IMG_FMT_LAST) {
		fprintf(stderr, "Image saving error: error saving %s, invalid format specification\n", fname);
		return -1;
	}

	if(!(fp = fopen(fname, "wb"))) {
		fprintf(stderr, "Image saving error: could not open file %s for writing\n", fname);
		return -1;
	}

	res = fmt_func[fmt].save(fp, pixels, xsz, ysz);
	fclose(fp);
	return res;
}

int set_image_option(unsigned int opt, int val)
{
	unsigned int mask = 1 << opt;
	int prev_val = options & mask ? 1 : 0;

	options = val ? (options | mask) : (options & ~mask);
	return prev_val;
}

int get_image_option(unsigned int opt)
{
	unsigned int mask = 1 << opt;
	return options & mask ? 1 : 0;
}

void set_pixel_format(int rshift, int gshift, int bshift, int ashift)
{
	_imago_rshift = rshift;
	_imago_gshift = gshift;
	_imago_bshift = bshift;
	_imago_ashift = ashift;
}

void get_pixel_format(int *rshift, int *gshift, int *bshift, int *ashift)
{
	*rshift = _imago_rshift;
	*gshift = _imago_gshift;
	*bshift = _imago_bshift;
	*ashift = _imago_ashift;
}
