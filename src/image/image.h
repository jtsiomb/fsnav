/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/

/* image file handling library */
#ifndef IMAGE_H_
#define IMAGE_H_

/* always compile support for image formats that
 * don't create any library dependencies.
 */
#define IMGLIB_USE_PPM
#define IMGLIB_USE_TGA
#define IMGLIB_USE_RGBE

/* for jpeg and png, make the default be to compile them in,
 * unless explicitly disabled.
 */
#ifndef IMGLIB_NO_JPEG
#define IMGLIB_USE_JPEG
#endif	/* IMGLIB_NO_JPEG */

#ifndef IMGLIB_NO_PNG
#define IMGLIB_USE_PNG
#endif	/* IMGLIB_NO_PNG */

/* supported image file formats */
enum {
	IMG_FMT_AUTO,	/* autodetect based on filename suffix */

	IMG_FMT_PNG,	/* PNG compressed, 32/24bit (libpng/zlib) */
	IMG_FMT_JPEG,	/* JPEG compressed (lossy), 24bit (ijg libjpeg) */
	IMG_FMT_TGA,	/* Targa uncompressed/rle 32/24bit */
	IMG_FMT_PPM,	/* Portable PixMap uncompressed 24bit binary/text */
	IMG_FMT_RGBE,	/* Ward's RGBE, compact floating point rgb images */

	IMG_FMT_LAST
};

/* various save/load options */
enum {
	IMG_OPT_ALPHA,		/* S: if the format allows 32bit images with alpha */
	IMG_OPT_COMPRESS,	/* S: if the format supports both un/compressed */
	IMG_OPT_INVERT,		/* S: invert scanlines (top-bottom) */
	IMG_OPT_TEXT,		/* S: if the format supports text-based output */
	IMG_OPT_FLOAT		/* L: return float buffer, don't convert to 32bpp (or convert to float) */
};

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* load_image() loads the specified image from file, returns the pixel data
 * in 32bit mode, and changes xsz and ysz to the size of the image
 */
void *load_image(const char *fname, int *xsz, int *ysz);

/* deallocate the image data with this function
 * note: provided for consistency, simply calls free()
 */
void free_image(void *img);

/* save the supplied image data in a file of the specified format */
int save_image(const char *fname, void *pixels, int xsz, int ysz, unsigned int fmt);

/* set/get image save/load options */
int set_image_option(unsigned int opt, int val);
int get_image_option(unsigned int opt);

/* set/get packed pixel format */
void set_pixel_format(int rshift, int gshift, int bshift, int ashift);
void get_pixel_format(int *rshift, int *gshift, int *bshift, int *ashift);

#ifdef __cplusplus
}
#endif	/* __cplusplus */
	
#endif	/* IMAGE_H_ */
