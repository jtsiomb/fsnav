/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
/* jpeg support
 * author: Michael Georgoulopoulos
 */

#include "image.h"
#include <stdio.h>

#ifdef IMGLIB_USE_JPEG

#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#define HAVE_BOOLEAN
#endif

#include <jpeglib.h>
#include "color_bits.h"
#include "util.h"

typedef struct {
	unsigned char r,g,b;
} RGBTriplet;

/*jpeg signature*/
int check_jpeg(FILE *fp)
{
	unsigned char sig[10];

	fseek(fp, 0, SEEK_SET);
	fread(sig, 1, 10, fp);
	
    if(sig[0]!=0xff || sig[1]!=0xd8 || sig[2]!=0xff || sig[3]!=0xe0) {
		return 0;
	}

	if(/*sig[6]!='J' ||*/sig[7]!='F' || sig[8]!='I' || sig[9]!='F') {
		return 0;
	}
	
    return 1;
}

void *load_jpeg(FILE *fp, int *xsz, int *ysz)
{
	int i;
	RGBTriplet *buffer;
	uint32_t *image;
	
	JSAMPLE *tmp;
    
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	cinfo.err = jpeg_std_error(&jerr);
	
	fseek(fp, 0, SEEK_SET);

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, fp);
	
	jpeg_read_header(&cinfo, TRUE);
	
	/* force output to rgb */
	cinfo.out_color_space = JCS_RGB;
	
    /* allocate space */
    if(!(buffer = malloc(cinfo.image_width * sizeof(RGBTriplet)))) {
		return 0;
	}
    if(!(image = malloc(cinfo.image_width * (cinfo.image_height) * sizeof(uint32_t)))) {
		free(buffer);
		return 0;
	}
    
    tmp = (JSAMPLE*) buffer;

	/* return w and h */
	*xsz = cinfo.image_width;
	*ysz = cinfo.image_height;
    
	/* Decompress, pack and store */
    jpeg_start_decompress(&cinfo);
	while(cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, &tmp, 1);
		if(cinfo.output_scanline == 0) continue;
		
		for(i=0;i<cinfo.image_width;i++) {
			int offs = i + (cinfo.output_scanline-1) * cinfo.output_width;
			image[offs] = PACK_COLOR24(buffer[i].r, buffer[i].g, buffer[i].b);
		}
	}
    jpeg_finish_decompress(&cinfo);
	
	/*Done - cleanup*/
	jpeg_destroy_decompress(&cinfo);
	free(buffer);

	if(get_image_option(IMG_OPT_FLOAT)) {
		float *fpix;
		
		if(!(fpix = malloc(*xsz * *ysz * 4 * sizeof *fpix))) {
			free(image);
			return 0;
		}

		conv_32bpp_to_float(image, fpix, *xsz, *ysz);
		free(image);
		return fpix;
	}

	return image;
}

/* TODO: implement this */
int save_jpeg(FILE *fp, void *pixels, int xsz, int ysz)
{
	fprintf(stderr, "saving jpeg files is not implemented yet\n");
	return -1;
}

#endif	/* IMGLIB_USE_JPEG */
