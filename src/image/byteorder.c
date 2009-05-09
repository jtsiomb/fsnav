#include <stdio.h>
#include "int_types.h"


int8_t read_int8(FILE *fp) {
	int8_t v;
	fread(&v, 1, 1, fp);
	return v;
}

int16_t read_int16(FILE *fp) {
	int16_t v;
	fread(&v, 2, 1, fp);
	return v;
}

int16_t read_int16_inv(FILE *fp) {
	int16_t v;
	fread(&v, 2, 1, fp);
	return v >> 8 | v << 8;
}

int32_t read_int32(FILE *fp) {
	int32_t v;
	fread(&v, 4, 1, fp);
	return v;
}

int32_t read_int32_inv(FILE *fp) {
	int32_t v;
	fread(&v, 4, 1, fp);
	return v >> 24 | (v & 0x00ff0000) >> 8 | (v & 0x0000ff00) << 8 | v << 24;
}

float read_float(FILE *fp) {
	int32_t tmp = read_int32(fp);
	return *((float*)&tmp);
}

float read_float_inv(FILE *fp) {
	int32_t tmp = read_int32_inv(fp);
	return *((float*)&tmp);
}

void write_int8(FILE *fp, int8_t v) {
	fwrite(&v, 1, 1, fp);
}

void write_int16(FILE *fp, int16_t v) {
	fwrite(&v, 2, 1, fp);
}

void write_int16_inv(FILE *fp, int16_t v) {
	int16_t tmp = v >> 8 | v << 8;
	fwrite(&tmp, 2, 1, fp);
}

void write_int32(FILE *fp, int32_t v) {
	fwrite(&v, 4, 1, fp);
}

void write_int32_inv(FILE *fp, int32_t v) {
	int32_t tmp = v >> 24 | (v & 0x00ff0000) >> 8 | (v & 0x0000ff00) << 8 | v << 24;
	fwrite(&tmp, 4, 1, fp);
}

void write_float(FILE *fp, float v) {
	write_int32(fp, *((int32_t*)&v));
}
void write_float_inv(FILE *fp, float v) {
	write_int32_inv(fp, *((int32_t*)&v));
}
