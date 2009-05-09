#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else

#if defined(WIN32) || defined(__WIN32__)
#include <windows.h>
#else
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include "text.h"

#ifndef GL_BGRA
#define GL_BGRA                           0x80E1
#endif

#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
#endif

#ifndef GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF
#endif

#define MAX_CHARS		128
#define MAX_IMG_WIDTH	1024
#define SIZE_PIXELS(x)	((x) / 64)

typedef struct {
	float r, g, b, a;
} color_t;

struct font {
	unsigned int tex_id;
	float scale;	/* this compensates if a higher res font is loaded in its stead, with a new CreateFont */
	float line_adv;	/* vertical advance to go to the next line */
	struct {
		vec2_t pos, size;		/* glyph position (from origin) and size in normalized coords [0, 1] */
		float advance;			/* advance in normalized coords */
		vec2_t tc_pos, tc_sz;	/* tex coord box pos/size */
	} glyphs[MAX_CHARS];

	char *name;
	int ptsize;
};

static void blit_font_glyph(struct font *fnt, int x, int y, FT_GlyphSlot glyph, unsigned int *img, int xsz, int ysz);
static void clean_up(void);

static FT_Library ft;
static vec2_t text_pos;
static float text_size = 1.0;
static color_t text_color;
static struct font *act_fnt;
static unsigned int tmode = TEXT_MODE_2D;

#define MAX_FONTS	128
static struct font *fonts[MAX_FONTS];


static float my_log2(float x) {
	float res = 0.0;
	while(x > 1.0) {
		x /= 2.0;
		res += 1.0;
	}
	return res;
}

static int next_pow2(int x)
{
	float lg2 = my_log2((float)x);
	return (int)pow(2.0f, (int)ceil(lg2));
}


unsigned int create_font(const char *fname, int font_size)
{
	struct font *fnt;
	int i, max_width, gx, gy;
	int foo_xsz, foo_ysz, tex_xsz, tex_ysz;
	int vport_xsz, vport_ysz;
	int max_glyph_x, max_glyph_y;
	int idx;
	int xsz, ysz;
	FT_Face face;
	unsigned int *img;
	float max_aniso = 0.0;

	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	xsz = vp[2];
	ysz = vp[3];

	if(strstr((char*)glGetString(GL_EXTENSIONS), "EXT_texture_filter_anisotropic")) {
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	}

	if(!ft) {
		if(FT_Init_FreeType(&ft) != 0) {
			fprintf(stderr, "failed to initialize freetype\n");
			return 0;
		}
		atexit(clean_up);
	}

	idx = 0;
	for(i=1; i<MAX_FONTS; i++) {
		if(!fonts[i]) {
			idx = i;
		}
	}
	if(!idx) {
		fprintf(stderr, "too many fonts loaded\n");
		return 0;
	}

	if(FT_New_Face(ft, fname, 0, &face) != 0) {
		fprintf(stderr, "failed to load font: %s\n", fname);
		return 0;
	}

	FT_Set_Pixel_Sizes(face, 0, font_size);

	if(!(fnt = malloc(sizeof *fnt))) {
		perror("malloc failed");
		return 0;
	}

	max_width = MAX_CHARS * SIZE_PIXELS(face->bbox.xMax - face->bbox.xMin);
	foo_xsz = MAX_IMG_WIDTH;
	foo_ysz = SIZE_PIXELS(face->bbox.yMax - face->bbox.yMin) * max_width / foo_xsz;

	tex_xsz = next_pow2(foo_xsz);
	tex_ysz = next_pow2(foo_ysz);

	if(!(img = malloc(tex_xsz * tex_ysz * sizeof *img))) {
		perror("malloc failed");
		return 0;
	}
	memset(img, 0, tex_xsz * tex_ysz * sizeof *img);

	vport_xsz = xsz;
	vport_ysz = ysz;

	max_glyph_y = max_glyph_x = 0;
	for(i=0; i<MAX_CHARS; i++) {
		int width, height;

		FT_Load_Char(face, i, 0);
		width = SIZE_PIXELS(face->glyph->metrics.width);
		height = SIZE_PIXELS(face->glyph->metrics.height);
		
		if(height > max_glyph_y) {
			max_glyph_y = height;
		}

		if(width > max_glyph_x) {
			max_glyph_x = width;
		}
	}

	gx = gy = 0;
	for(i=0; i<MAX_CHARS; i++) {
		int gwidth, gheight;
		FT_GlyphSlot g;

		FT_Load_Char(face, i, FT_LOAD_RENDER);
		g = face->glyph;

		gwidth = SIZE_PIXELS(g->metrics.width);
		gheight = SIZE_PIXELS(g->metrics.height);

		if(gx > MAX_IMG_WIDTH - gwidth) {
			gx = 0;
			gy += max_glyph_y;
		}

		blit_font_glyph(fnt, gx, gy, g, img, tex_xsz, tex_ysz);
		fnt->scale = 1.0;
		fnt->line_adv = (float)SIZE_PIXELS(g->metrics.vertAdvance) / (float)vport_ysz;
		fnt->glyphs[i].tc_pos.x = (float)gx / (float)tex_xsz;
		fnt->glyphs[i].tc_pos.y = (float)gy / (float)tex_ysz;
		fnt->glyphs[i].tc_sz.x = (float)gwidth / (float)tex_xsz;
		fnt->glyphs[i].tc_sz.y = (float)gheight / (float)tex_ysz;
		fnt->glyphs[i].size.x = (float)gwidth / (float)vport_xsz;
		fnt->glyphs[i].size.y = (float)gheight / (float)vport_ysz;
		fnt->glyphs[i].pos.x = (float)SIZE_PIXELS(g->metrics.horiBearingX) / (float)vport_xsz;
		fnt->glyphs[i].pos.y = -(float)SIZE_PIXELS(g->metrics.horiBearingY) / (float)vport_ysz;
		fnt->glyphs[i].advance = (float)SIZE_PIXELS(g->metrics.horiAdvance) / (float)vport_xsz;

		gx += gwidth;
	}

	FT_Done_Face(face);

	glGenTextures(1, &fnt->tex_id);
	glBindTexture(GL_TEXTURE_2D, fnt->tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if(max_aniso > 0.0) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	}

	/*glTexImage2D(GL_TEXTURE_2D, 0, 4, tex_xsz, tex_ysz, 0, GL_BGRA, GL_UNSIGNED_BYTE, img);*/
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, tex_xsz, tex_ysz, GL_BGRA, GL_UNSIGNED_BYTE, img);
	free(img);

	fonts[idx] = fnt;
	act_fnt = fnt;
	return idx;
}

void delete_font(unsigned int fid)
{
	if(fid > 0 && fid < MAX_FONTS) {
		free(fonts[fid]);
		fonts[fid] = 0;
	}
}

int bind_font(unsigned int fid)
{
	if(fid > 0 && fid < MAX_FONTS && fonts[fid]) {
		act_fnt = fonts[fid];
		return fid;
	}
	return 0;
}

void set_text_mode(unsigned int text_mode)
{
	tmode = text_mode;
}

unsigned int get_text_mode()
{
	return tmode;
}

void set_text_pos(float x, float y)
{
	text_pos.x = x;
	text_pos.y = y;
}

vec2_t get_text_pos(void)
{
	return text_pos;
}

void text_line_advance(int adv)
{
	text_pos.y += (float)adv * act_fnt->line_adv;
}

void text_cret(void)
{
	text_pos.x = 0.0;
}

void set_text_size(float sz)
{
	text_size = sz;
}

float get_text_size(void)
{
	return text_size;
}

void set_text_color(float r, float g, float b, float a)
{
	text_color.r = r;
	text_color.g = g;
	text_color.b = b;
	text_color.a = a;
}

vec4_t get_text_color(void)
{
	return *(vec4_t*)&text_color;
}

/*
static void im_overlay(vec2_t v1, vec2_t v2, color_t col, unsigned int tex)
{
	float l = v1.x * 2.0f - 1.0f;
	float r = v2.x * 2.0f - 1.0f;
	float u = -v1.y * 2.0f + 1.0f;
	float d = -v2.y * 2.0f + 1.0f;

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBindTexture(GL_TEXTURE_2D, tex);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);

	glBegin(GL_QUADS);
	glColor4f(col.r, col.g, col.b, col.a);
	glTexCoord2f(0, 0);
	glVertex2f(l, u);
	glTexCoord2f(1, 0);
	glVertex2f(r, u);
	glTexCoord2f(1, 1);
	glVertex2f(r, d);
	glTexCoord2f(0, 1);
	glVertex2f(l, d);
	glEnd();

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
*/

/* this function contains the preamble of all block text drawing functions */
static void pre_draw()
{
	if(tmode == TEXT_MODE_2D) {
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
	}
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glPushAttrib(GL_ENABLE_BIT);

	glDisable(GL_LIGHTING);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, act_fnt->tex_id);

	glBegin(GL_QUADS);
	glColor4f(text_color.r, text_color.g, text_color.b, text_color.a);
}

static void post_draw()
{
	glEnd();

	glPopAttrib();

	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	if(tmode == TEXT_MODE_2D) {
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}
}

static float print_string_internal(const char *str, int standalone)
{
	float start_x;
	if(standalone) pre_draw();
	
	start_x = text_pos.x;
	while(*str) {
		float tx, ty, sx, sy;
		float l, r, u, d;
		vec2_t tc1, tc2, v1, v2;

		int c = *str++;

		if(!isprint(c)) {
			c = ' ';
		}

		tx = act_fnt->glyphs[c].tc_pos.x;
		ty = act_fnt->glyphs[c].tc_pos.y;
		sx = act_fnt->glyphs[c].tc_sz.x;
		sy = act_fnt->glyphs[c].tc_sz.y;

		tc1.x = tx;
		tc1.y = ty;
		tc2.x = tx + sx;
		tc2.y = ty + sy;

		v1.x = text_pos.x + act_fnt->glyphs[c].pos.x * act_fnt->scale * text_size;
		v1.y = text_pos.y + act_fnt->glyphs[c].pos.y * act_fnt->scale * text_size;

		v2.x = v1.x + act_fnt->glyphs[c].size.x * act_fnt->scale * text_size;
		v2.y = v1.y + act_fnt->glyphs[c].size.y * act_fnt->scale * text_size;
		
		l = v1.x * 2.0f - 1.0f;
		r = v2.x * 2.0f - 1.0f;
		u = -v1.y * 2.0f + 1.0f;
		d = -v2.y * 2.0f + 1.0f;

		glTexCoord2f(tc1.x, tc2.y);
		glVertex2f(l, d);
		glTexCoord2f(tc2.x, tc2.y);
		glVertex2f(r, d);
		glTexCoord2f(tc2.x, tc1.y);
		glVertex2f(r, u);
		glTexCoord2f(tc1.x, tc1.y);
		glVertex2f(l, u);

		text_pos.x += act_fnt->glyphs[c].advance * act_fnt->scale * text_size;
	}

	if(standalone) post_draw();
	return text_pos.x - start_x;
}

float print_string(const char *str)
{
	return print_string_internal(str, 1);
}

void print_string_lines(const char **str, int lines)
{
	pre_draw();

	while(lines-- > 0) {
		print_string_internal(*str++, 0);
		text_line_advance(1);
		text_cret();
	}

	post_draw();
}

static void blit_font_glyph(struct font *fnt, int x, int y, FT_GlyphSlot glyph, unsigned int *img, int xsz, int ysz)
{
	int i, j;
	unsigned int *dest;
	unsigned char *src;

	if(glyph->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY) {
		fprintf(stderr, "blit_font_glyph: unsupported pixel mode: %d\n", glyph->bitmap.pixel_mode);
		return;
	}

	dest = img + y * xsz + x;
	src = glyph->bitmap.buffer;

	for(j=0; j<glyph->bitmap.rows; j++) {
		for(i=0; i<glyph->bitmap.width; i++) {
			dest[i] = 0x00ffffff | ((unsigned int)src[i] << 24);
		}
		dest += xsz;
		src += glyph->bitmap.pitch;
	}
}

static void clean_up(void)
{
	FT_Done_FreeType(ft);
}

float get_max_descent()
{
	int i;
	struct font *fnt = act_fnt;

	float max_descent = 0.0f;
	for(i=0; i<MAX_CHARS; i++) {
		float des = fnt->glyphs[i].pos.y + fnt->glyphs[i].size.y;
		if(isprint(i) && des > max_descent) {
			max_descent = des;
		}
	}

	return max_descent;
}

float get_line_advance()
{
	return act_fnt->line_adv;
}

float get_text_width(const char *str)
{
	float width = 0;
	while(*str) {
		width += act_fnt->glyphs[(int)*str++].advance * act_fnt->scale * text_size;
	}
	return width;
}
