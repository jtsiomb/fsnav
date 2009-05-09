/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
/* png support */
#include "image.h"
#include <stdio.h>

#ifdef IMGLIB_USE_PNG

#include <stdlib.h>
#include <png.h>
#include "color_bits.h"
#include "util.h"

#define FILE_SIG_BYTES	8

int check_png(FILE *fp)
{
	unsigned char sig[FILE_SIG_BYTES];

	fread(sig, 1, FILE_SIG_BYTES, fp);

	return png_sig_cmp(sig, 0, FILE_SIG_BYTES) == 0 ? 1 : 0;
}

void *load_png(FILE *fp, int *xsz, int *ysz)
{
	png_struct *png_ptr;
	png_info *info_ptr;
	int i;
	uint32_t **lineptr, *pixels;
	int channel_bits, color_type, ilace_type, compression, filtering;
	unsigned long img_x, img_y;
	
	if(!(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0))) {
		return 0;
	}
	
	if(!(info_ptr = png_create_info_struct(png_ptr))) {
		png_destroy_read_struct(&png_ptr, 0, 0);
		return 0;
	}
	
	if(setjmp(png_jmpbuf(png_ptr))) {		
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return 0;
	}
	
	png_init_io(png_ptr, fp);	
	png_set_sig_bytes(png_ptr, FILE_SIG_BYTES);
	
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_BGR, 0);
		
	png_get_IHDR(png_ptr, info_ptr, &img_x, &img_y, &channel_bits, &color_type, &ilace_type, &compression, &filtering);
	*xsz = img_x;
	*ysz = img_y;
	pixels = malloc(*xsz * *ysz * sizeof(uint32_t));
	
	lineptr = (uint32_t**)png_get_rows(png_ptr, info_ptr);
	
	for(i=0; i<*ysz; i++) {
		
		switch(color_type) {
		case PNG_COLOR_TYPE_RGB:
			{
				int j;
				unsigned char *ptr = (unsigned char*)lineptr[i];
				for(j=0; j<*xsz; j++) {
			
					uint32_t pixel;
					pixel = PACK_COLOR24(*(ptr+2), *(ptr+1), *ptr);
					ptr+=3;
					pixels[i * *xsz + j] = pixel;			
				}
			}
			break;
			
		case PNG_COLOR_TYPE_RGB_ALPHA:
			/* TODO BUG make sure the user-settable shifts are obeyed */
			memcpy(&pixels[i * *xsz], lineptr[i], *xsz * sizeof(uint32_t));
			break;
			
		default:
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			return 0;
		}
				
	}
	
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	if(get_image_option(IMG_OPT_FLOAT)) {
		float *fpix;
		
		if(!(fpix = malloc(img_x * img_y * 4 * sizeof *fpix))) {
			free(pixels);
			return 0;
		}

		conv_32bpp_to_float(pixels, fpix, img_x, img_y);
		free(pixels);
		return fpix;
	}

	return pixels;
}

int save_png(FILE *fp, void *pixels, int xsz, int ysz)
{
	png_struct *png;
	png_info *info;
	png_text txt;
	int i, j, coltype;
	uint32_t *pixptr;
	unsigned char **rows;
	int save_alpha = get_image_option(IMG_OPT_ALPHA);
	int save_invert = get_image_option(IMG_OPT_INVERT);
	int src_fmt_float = get_image_option(IMG_OPT_FLOAT);

	if(src_fmt_float) {
		void *rgba32 = malloc(xsz * ysz * sizeof(uint32_t));
		conv_float_to_32bpp(pixels, rgba32, xsz, ysz);
		pixels = rgba32;
	}

	txt.compression = PNG_TEXT_COMPRESSION_NONE;
	txt.key = "Software";
	txt.text = "libimago";
	txt.text_length = 0;

	if(!(png = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0))) {
		if(src_fmt_float) {
			free(pixels);
		}
		return -1;
	}
	
	if(!(info = png_create_info_struct(png))) {
		png_destroy_write_struct(&png, 0);
		if(src_fmt_float) {
			free(pixels);
		}
		return -1;
	}

	if(setjmp(png_jmpbuf(png))) {
		png_destroy_write_struct(&png, &info);
		if(src_fmt_float) {
			free(pixels);
		}
		return -1;
	}

	png_init_io(png, fp);

	coltype = save_alpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
	png_set_IHDR(png, info, xsz, ysz, 8, coltype, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_set_text(png, info, &txt, 1);

	if(!(rows = malloc(ysz * sizeof *rows))) {
		png_destroy_write_struct(&png, &info);
		if(src_fmt_float) {
			free(pixels);
		}
		return -1;
	}

	pixptr = save_invert ? (uint32_t*)pixels + (ysz - 1) * xsz : pixels;
	for(i=0; i<ysz; i++) {
		if(coltype == PNG_COLOR_TYPE_RGB) {
			rows[i] = malloc(xsz * 3);
			for(j=0; j<xsz; j++) {
				rows[i][j * 3] = UNP_BLUE32(pixptr[j]);
				rows[i][j * 3 + 1] = UNP_GREEN32(pixptr[j]);
				rows[i][j * 3 + 2] = UNP_RED32(pixptr[j]);
			}
		} else {
			/* TODO BUG, make sure the user-settable shifts are obeyed */
			rows[i] = (unsigned char*)pixptr;
		}
		pixptr += save_invert ? -xsz : xsz;
	}

	png_set_rows(png, info, rows);
	png_write_png(png, info, PNG_TRANSFORM_BGR, 0);

	if(coltype == PNG_COLOR_TYPE_RGB) {
		for(i=0; i<ysz; i++) {
			free(rows[i]);
		}
	}
	free(rows);

	png_write_end(png, info);
	png_destroy_write_struct(&png, &info);

	if(src_fmt_float) {
		free(pixels);
	}
	return 0;
}

#endif	/* IMGLIB_USE_PNG */
