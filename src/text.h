#ifndef TEXT_H
#define TEXT_H

#include <vmath.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned int create_font(const char *fname, int font_size);
void delete_font(unsigned int fid);
int bind_font(unsigned int fid);

unsigned int get_font_texture(void);

enum {
	TEXT_MODE_2D,
	TEXT_MODE_3D
};
void set_text_mode(unsigned int text_mode);
unsigned int get_text_mode();

void set_text_pos(float x, float y);
vec2_t get_text_pos(void);

void text_line_advance(int adv);
void text_cret(void);

void set_text_size(float sz);
float get_text_size(void);

void set_text_color(float r, float g, float b, float a);
vec4_t get_text_color(void);

float print_string(const char *text);
void print_string_lines(const char **str, int lines);

float get_max_descent(void);
float get_line_advance(void);
float get_text_width(const char *str);

#ifdef __cplusplus
}
#endif

#endif	/* TEXT_H */
