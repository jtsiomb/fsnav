/*
This file is part of imago, a multi file format image I/O library.
Copyright (C) 2004 - 2009 John Tsiombikas <nuclear@member.fsf.org>

Redistribution and use in source and binary forms, with or without
modification, are permitted under the terms of the modified 3-clause
BSD license. See COPYING for more details.
*/
/* targa support
 *
 * original author: (JT)
 */

#include "image.h"
#include <stdio.h>

#ifdef IMGLIB_USE_TGA

#include <stdlib.h>
#include <string.h>
#include "color_bits.h"
#include "byteorder.h"
#include "util.h"

enum {
	IMG_NONE,
	IMG_CMAP,
	IMG_RGBA,
	IMG_BW,

	IMG_RLE_CMAP = 9,
	IMG_RLE_RGBA,
	IMG_RLE_BW
};

#define IS_RLE(x)	((x) >= IMG_RLE_CMAP)
#define IS_RGBA(x)	((x) == IMG_RGBA || (x) == IMG_RLE_RGBA)


struct tga_header {
	uint8_t idlen;			/* id field length */
	uint8_t cmap_type;		/* color map type (0:no color map, 1:color map present) */
	uint8_t img_type;		/* image type: 
							 * 0: no image data
							 *	1: uncomp. color-mapped		 9: RLE color-mapped
							 *	2: uncomp. true color		10: RLE true color
							 *	3: uncomp. black/white		11: RLE black/white */	
	uint16_t cmap_first;	/* color map first entry index */
	uint16_t cmap_len;		/* color map length */
	uint8_t cmap_entry_sz;	/* color map entry size */
	uint16_t img_x;			/* X-origin of the image */
	uint16_t img_y;			/* Y-origin of the image */
	uint16_t img_width;		/* image width */
	uint16_t img_height;	/* image height */
	uint8_t img_bpp;		/* bits per pixel */
	uint8_t img_desc;		/* descriptor: 
							 * bits 0 - 3: alpha or overlay bits
							 * bits 5 & 4: origin (0 = bottom/left, 1 = top/right)
							 * bits 7 & 6: data interleaving */	
};

struct tga_footer {
	uint32_t ext_off;		/* extension area offset */
	uint32_t devdir_off;	/* developer directory offset */
	char sig[18];				/* signature with . and \0 */
};

/*static void print_tga_info(struct tga_header *hdr);*/

int check_tga(FILE *fp)
{
	struct tga_footer foot;
	
	fseek(fp, -18, SEEK_END);
	fread(foot.sig, 1, 18, fp);

	foot.sig[17] = 0;
	return strcmp(foot.sig, "TRUEVISION-XFILE.") == 0 ? 1 : 0;
}

static uint32_t read_pixel(FILE *fp, int rdalpha)
{
	int r, g, b, a;
	b = getc(fp);
	g = getc(fp);
	r = getc(fp);
	a = rdalpha ? getc(fp) : 0xff;
	return PACK_COLOR32(r, g, b, a);
}

void *load_tga(FILE *fp, int *xsz, int *ysz)
{
	struct tga_header hdr;
	unsigned long x, y, sz;
	int i;
	uint32_t *pix, ppixel = 0;
	int rle_mode = 0, rle_pix_left = 0;
	int rdalpha;

	/* read header */
	fseek(fp, 0, SEEK_SET);
	hdr.idlen = fgetc(fp);
	hdr.cmap_type = fgetc(fp);
	hdr.img_type = fgetc(fp);
	hdr.cmap_first = read_int16_le(fp);
	hdr.cmap_len = read_int16_le(fp);
	hdr.cmap_entry_sz = fgetc(fp);
	hdr.img_x = read_int16_le(fp);
	hdr.img_y = read_int16_le(fp);
	hdr.img_width = read_int16_le(fp);
	hdr.img_height = read_int16_le(fp);
	hdr.img_bpp = fgetc(fp);
	hdr.img_desc = fgetc(fp);

	if(feof(fp)) {
		return 0;
	}
	
	/* only read true color images */
	if(!IS_RGBA(hdr.img_type)) {
		fprintf(stderr, "only true color tga images supported\n");
		return 0;
	}

	fseek(fp, hdr.idlen, SEEK_CUR); /* skip the image ID */

	/* skip the color map if it exists */
	if(hdr.cmap_type == 1) {
		fseek(fp, hdr.cmap_len * hdr.cmap_entry_sz / 8, SEEK_CUR);
	}

	x = hdr.img_width;
	y = hdr.img_height;
	sz = x * y;
	if(!(pix = malloc(sz * 4))) {
		return 0;
	}

	rdalpha = hdr.img_desc & 0xf;

	for(i=0; i<y; i++) {
		uint32_t *ptr;
		int j;

		ptr = pix + ((hdr.img_desc & 0x20) ? i : y-(i+1)) * x;

		for(j=0; j<x; j++) {
			/* if the image is raw, then just read the next pixel */
			if(!IS_RLE(hdr.img_type)) {
				ppixel = read_pixel(fp, rdalpha);
			} else {
                /* otherwise, for RLE... */

				/* if we have pixels left in the packet ... */
				if(rle_pix_left) {
					/* if it's a raw packet, read the next pixel, otherwise keep the same */
					if(!rle_mode) {
						ppixel = read_pixel(fp, rdalpha);
					}
					rle_pix_left--;
				} else {
					/* read the RLE packet header */
					unsigned char packet_hdr = getc(fp);
					rle_mode = (packet_hdr & 128);		/* last bit shows the mode for this packet (1: rle, 0: raw) */
					rle_pix_left = (packet_hdr & ~128);	/* the rest gives the count of pixels minus one (we also read one here, so no +1) */
					ppixel = read_pixel(fp, rdalpha);	/* and read the first pixel of the packet */
				}
			}

			*ptr++ = ppixel;

			if(feof(fp)) break;
		}
	}

	*xsz = x;
	*ysz = y;

	if(get_image_option(IMG_OPT_FLOAT)) {
		float *fpix;
		
		if(!(fpix = malloc(x * y * 4 * sizeof *fpix))) {
			free(pix);
			return 0;
		}

		conv_32bpp_to_float(pix, fpix, x, y);
		free(pix);
		return fpix;
	}

	return pix;
}

int save_tga(FILE *fp, void *pixels, int xsz, int ysz) {
	struct tga_header hdr;
	struct tga_footer ftr;
	unsigned int pix_count = xsz * ysz;
	uint32_t *pptr;
	int i;
	int src_fmt_float = get_image_option(IMG_OPT_FLOAT);

	if(src_fmt_float) {
		void *rgba32 = malloc(xsz * ysz * sizeof(uint32_t));
		conv_float_to_32bpp(pixels, rgba32, xsz, ysz);
		pixels = rgba32;
	}

	pptr = pixels;

	memset(&hdr, 0, sizeof hdr);
	hdr.img_type = 2;
	hdr.img_width = xsz;
	hdr.img_height = ysz;

	if(get_image_option(IMG_OPT_ALPHA)) {
		hdr.img_bpp = 32;
		hdr.img_desc = 8 | 0x20;	/* 8 alpha bits, origin bottom-left */
	} else {
		hdr.img_bpp = 24;
		hdr.img_desc = 0x20;		/* no alpha bits, origin bottom-left */
	}

	if(get_image_option(IMG_OPT_INVERT)) {
		hdr.img_desc ^= 0x20;
	}

	ftr.ext_off = 0;
	ftr.devdir_off = 0;
	strcpy(ftr.sig, "TRUEVISION-XFILE.");

	/* write the header */
	
	fwrite(&hdr.idlen, 1, 1, fp);
	fwrite(&hdr.cmap_type, 1, 1, fp);
	fwrite(&hdr.img_type, 1, 1, fp);
	write_int16_le(fp, hdr.cmap_first);
	write_int16_le(fp, hdr.cmap_len);
	fwrite(&hdr.cmap_entry_sz, 1, 1, fp);
	write_int16_le(fp, hdr.img_x);
	write_int16_le(fp, hdr.img_y);
	write_int16_le(fp, hdr.img_width);
	write_int16_le(fp, hdr.img_height);
	fwrite(&hdr.img_bpp, 1, 1, fp);
	fwrite(&hdr.img_desc, 1, 1, fp);

	/* write the pixels */
	for(i=0; i<pix_count; i++) {
		fputc(UNP_BLUE32(*pptr), fp);
		fputc(UNP_GREEN32(*pptr), fp);
		fputc(UNP_RED32(*pptr), fp);
		
		if(get_image_option(IMG_OPT_ALPHA)) {
			fputc(UNP_ALPHA32(*pptr), fp);
		}
		
		pptr++;
	}

	/* write the footer */
	write_int32_le(fp, ftr.ext_off);
	write_int32_le(fp, ftr.devdir_off);
	fputs(ftr.sig, fp);
	fputc(0, fp);

	if(src_fmt_float) {
		free(pixels);
	}
	return 0;
}

/*
static void print_tga_info(struct tga_header *hdr) {
	static const char *img_type_str[] = {
		"no image data",
		"uncompressed color-mapped",
		"uncompressed true color",
		"uncompressed black & white",
		"", "", "", "", "",
		"RLE color-mapped",
		"RLE true color",
		"RLE black & white"
	};
	
	printf("id field length: %d\n", (int)hdr->idlen);
	printf("color map present: %s\n", hdr->cmap_type ? "yes" : "no");
	printf("image type: %s\n", img_type_str[hdr->img_type]);
	printf("color-map start: %d\n", (int)hdr->cmap_first);
	printf("color-map length: %d\n", (int)hdr->cmap_len);
	printf("color-map entry size: %d\n", (int)hdr->cmap_entry_sz);
	printf("image origin: %d, %d\n", (int)hdr->img_x, (int)hdr->img_y);
	printf("image size: %d, %d\n", (int)hdr->img_width, (int)hdr->img_height);
	printf("bpp: %d\n", (int)hdr->img_bpp);
	printf("attribute bits (alpha/overlay): %d\n", (int)(hdr->img_desc & 0xf));
	printf("origin: %s-", (hdr->img_desc & 0x20) ? "top" : "bottom");
	printf("%s\n", (hdr->img_desc & 0x10) ? "right" : "left");
}
*/

#endif	/* IMGLIB_USE_TGA */
