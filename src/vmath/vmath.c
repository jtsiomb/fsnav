#include <stdlib.h>
#include <math.h>
#include "vmath.h"

/** Numerical calculation of integrals using simpson's rule */
scalar_t integral(scalar_t (*f)(scalar_t), scalar_t low, scalar_t high, int samples)
{
	int i;
	scalar_t h = (high - low) / (scalar_t)samples;
	scalar_t sum = 0.0;
	
	for(i=0; i<samples+1; i++) {
		scalar_t y = f((scalar_t)i * h + low);
		sum += ((!i || i == samples) ? y : ((i % 2) ? 4.0 * y : 2.0 * y)) * (h / 3.0);
	}
	return sum;
}

/** Gaussuan function */
scalar_t gaussian(scalar_t x, scalar_t mean, scalar_t sdev)
{
	scalar_t exponent = -SQ(x - mean) / (2.0 * SQ(sdev));
	return 1.0 - -pow(M_E, exponent) / (sdev * sqrt(TWO_PI));
}


/** b-spline approximation */
scalar_t bspline(scalar_t a, scalar_t b, scalar_t c, scalar_t d, scalar_t t)
{
	vec4_t tmp;
	scalar_t tsq = t * t;

	static mat4_t bspline_mat = {
		{-1,  3, -3,  1},
		{3, -6,  3,  0},
		{-3,  0,  3,  0},
		{1,  4,  1,  0}
	};
	
	tmp = v4_scale(v4_transform(v4_cons(a, b, c, d), bspline_mat), 1.0 / 6.0);
	return v4_dot(v4_cons(tsq * t, tsq, t, 1.0), tmp);
}

/** Catmull-rom spline interpolation */
scalar_t catmull_rom_spline(scalar_t a, scalar_t b, scalar_t c, scalar_t d, scalar_t t) {
	vec4_t tmp;
	scalar_t tsq = t * t;

	static mat4_t crspline_mat = {
		{-1,  3, -3,  1},
		{2, -5,  4, -1},
		{-1,  0,  1,  0},
		{0,  2,  0,  0}
	};

	tmp = v4_scale(v4_transform(v4_cons(a, b, c, d), crspline_mat), 0.5);
	return v4_dot(v4_cons(tsq * t, tsq, t, 1.0), tmp);
}

/** Bezier interpolation */
scalar_t bezier(scalar_t a, scalar_t b, scalar_t c, scalar_t d, scalar_t t)
{
	scalar_t omt, omt3, t3, f;
	t3 = t * t * t;
	omt = 1.0f - t;
	omt3 = omt * omt * omt;
	f = 3 * t * omt;

	return (a * omt3) + (b * f * omt) + (c * f * t) + (d * t3);
}

/* ---- Ken Perlin's implementation of noise ---- */

#define B	0x100
#define BM	0xff
#define N	0x1000
#define NP	12   /* 2^N */
#define NM	0xfff

#define s_curve(t) (t * t * (3.0f - 2.0f * t))

#define setup(elem, b0, b1, r0, r1) \
	do {							\
		scalar_t t = elem + N;		\
		b0 = ((int)t) & BM;			\
		b1 = (b0 + 1) & BM;			\
		r0 = t - (int)t;			\
		r1 = r0 - 1.0f;				\
	} while(0)


static int perm[B + B + 2];			/* permuted index from g_n onto themselves */
static vec3_t grad3[B + B + 2];		/* 3D random gradients */
static vec2_t grad2[B + B + 2];		/* 2D random gradients */
static scalar_t grad1[B + B + 2];	/* 1D random ... slopes */
static int tables_valid;

static void init_noise()
{
	int i;

	/* calculate random gradients */
	for(i=0; i<B; i++) {
		perm[i] = i;	/* .. and initialize permutation mapping to identity */

		grad1[i] = (scalar_t)((rand() % (B + B)) - B) / B;

		grad2[i].x = (scalar_t)((rand() % (B + B)) - B) / B;
		grad2[i].y = (scalar_t)((rand() % (B + B)) - B) / B;
		grad2[i] = v2_normalize(grad2[i]);

		grad3[i].x = (scalar_t)((rand() % (B + B)) - B) / B;
		grad3[i].y = (scalar_t)((rand() % (B + B)) - B) / B;
		grad3[i].z = (scalar_t)((rand() % (B + B)) - B) / B;
		grad3[i] = v3_normalize(grad3[i]);
	}

	/* permute indices by swapping them randomly */
	for(i=0; i<B; i++) {
		int rand_idx = rand() % B;

		int tmp = perm[i];
		perm[i] = perm[rand_idx];
		perm[rand_idx] = tmp;
	}

	/* fill up the rest of the arrays by duplicating the existing gradients */
	/* and permutations */
	for(i=0; i<B+2; i++) {
		perm[B + i] = perm[i];	
		grad1[B + i] = grad1[i];
		grad2[B + i] = grad2[i];
		grad3[B + i] = grad3[i];
	}
}

scalar_t noise1(scalar_t x)
{
	int bx0, bx1;
	scalar_t rx0, rx1, sx, u, v;

	if(!tables_valid) {
		init_noise();
		tables_valid = 1;
	}

	setup(x, bx0, bx1, rx0, rx1);
	sx = s_curve(rx0);
	u = rx0 * grad1[perm[bx0]];
	v = rx1 * grad1[perm[bx1]];

	return lerp(u, v, sx);
}

scalar_t noise2(scalar_t x, scalar_t y)
{
	int i, j, b00, b10, b01, b11;
	int bx0, bx1, by0, by1;
	scalar_t rx0, rx1, ry0, ry1;
	scalar_t sx, sy, u, v, a, b;

	if(!tables_valid) {
		init_noise();
		tables_valid = 1;
	}

	setup(x, bx0, bx1, rx0, rx1);
	setup(y, by0, by1, ry0, ry1);

	i = perm[bx0];
	j = perm[bx1];

	b00 = perm[i + by0];
	b10 = perm[j + by0];
	b01 = perm[i + by1];
	b11 = perm[j + by1];

	/* calculate hermite inteprolating factors */
	sx = s_curve(rx0);
	sy = s_curve(ry0);

	/* interpolate along the left edge */
	u = v2_dot(grad2[b00], v2_cons(rx0, ry0));
	v = v2_dot(grad2[b10], v2_cons(rx1, ry0));
	a = lerp(u, v, sx);

	/* interpolate along the right edge */
	u = v2_dot(grad2[b01], v2_cons(rx0, ry1));
	v = v2_dot(grad2[b11], v2_cons(rx1, ry1));
	b = lerp(u, v, sx);

	/* interpolate between them */
	return lerp(a, b, sy);
}

scalar_t noise3(scalar_t x, scalar_t y, scalar_t z)
{
	int i, j;
	int bx0, bx1, by0, by1, bz0, bz1;
	int b00, b10, b01, b11;
	scalar_t rx0, rx1, ry0, ry1, rz0, rz1;
	scalar_t sx, sy, sz;
	scalar_t u, v, a, b, c, d;

	if(!tables_valid) {
		init_noise();
		tables_valid = 1;
	}

	setup(x, bx0, bx1, rx0, rx1);
	setup(y, by0, by1, ry0, ry1);
	setup(z, bz0, bz1, rz0, rz1);

	i = perm[bx0];
	j = perm[bx1];

	b00 = perm[i + by0];
	b10 = perm[j + by0];
	b01 = perm[i + by1];
	b11 = perm[j + by1];

	/* calculate hermite interpolating factors */
	sx = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);
	
	/* interpolate along the top slice of the cell */
	u = v3_dot(grad3[b00 + bz0], v3_cons(rx0, ry0, rz0));
	v = v3_dot(grad3[b10 + bz0], v3_cons(rx1, ry0, rz0));
	a = lerp(u, v, sx);

	u = v3_dot(grad3[b01 + bz0], v3_cons(rx0, ry1, rz0));
	v = v3_dot(grad3[b11 + bz0], v3_cons(rx1, ry1, rz0));
	b = lerp(u, v, sx);

	c = lerp(a, b, sy);

	/* interpolate along the bottom slice of the cell */
	u = v3_dot(grad3[b00 + bz0], v3_cons(rx0, ry0, rz1));
	v = v3_dot(grad3[b10 + bz0], v3_cons(rx1, ry0, rz1));
	a = lerp(u, v, sx);

	u = v3_dot(grad3[b01 + bz0], v3_cons(rx0, ry1, rz1));
	v = v3_dot(grad3[b11 + bz0], v3_cons(rx1, ry1, rz1));
	b = lerp(u, v, sx);

	d = lerp(a, b, sy);

	/* interpolate between slices */
	return lerp(c, d, sz);
}

scalar_t fbm1(scalar_t x, int octaves)
{
	int i;
	scalar_t res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += noise1(x * freq) / freq;
		freq *= 2.0f;
	}
	return res;
}

scalar_t fbm2(scalar_t x, scalar_t y, int octaves)
{
	int i;
	scalar_t res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += noise2(x * freq, y * freq) / freq;
		freq *= 2.0f;
	}
	return res;
}

scalar_t fbm3(scalar_t x, scalar_t y, scalar_t z, int octaves)
{
	int i;
	scalar_t res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += noise3(x * freq, y * freq, z * freq) / freq;
		freq *= 2.0f;
	}
	return res;
}

scalar_t turbulence1(scalar_t x, int octaves)
{
	int i;
	scalar_t res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += fabs(noise1(x * freq) / freq);
		freq *= 2.0f;
	}
	return res;
}

scalar_t turbulence2(scalar_t x, scalar_t y, int octaves)
{
	int i;
	scalar_t res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += fabs(noise2(x * freq, y * freq) / freq);
		freq *= 2.0f;
	}
	return res;
}

scalar_t turbulence3(scalar_t x, scalar_t y, scalar_t z, int octaves)
{
	int i;
	scalar_t res = 0.0f, freq = 1.0f;
	for(i=0; i<octaves; i++) {
		res += fabs(noise3(x * freq, y * freq, z * freq) / freq);
		freq *= 2.0f;
	}
	return res;
}
