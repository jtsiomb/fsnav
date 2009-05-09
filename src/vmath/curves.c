#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "curves.h"
#include "vector.h"

#undef DBG_SEARCH_LINEAR

#define INIT_TRACK_SIZE		32

static int find_key(struct key *track, int beg, int end, float t);

static vec4_t eval_linear(curve_t *c, float t);
static vec4_t eval_cosine(curve_t *c, float t);
static vec4_t eval_spline(curve_t *c, float t);

int curve_cons(curve_t *c)
{
	memset(c, 0, sizeof *c);

	if(!(c->track = malloc(INIT_TRACK_SIZE * sizeof *c->track))) {
		return -1;
	}
	c->tsize = INIT_TRACK_SIZE;
	c->key_count = 0;
	c->eval = eval_spline;
	return 0;
}

void curve_free(curve_t *c)
{
	free(c->track);
	memset(c, 0, sizeof *c);
}

void curve_mode(curve_t *c, int mode)
{
	switch(mode) {
	case CURVE_LINEAR:
		c->eval = eval_linear;
		break;
	case CURVE_COS:
		c->eval = eval_cosine;
		break;
	case CURVE_SPLINE:
		c->eval = eval_spline;
		break;
	default:
		break;
	}
}

int curve_value4(curve_t *c, float t, vec4_t val)
{
	int kidx = find_key(c->track, 0, c->key_count, t);

	if(c->track[kidx].t == t) {
		c->track[kidx].val = val;
		return 0;
	}

	if(c->tsize == c->key_count) {
		struct key *tmp = realloc(c->track, c->tsize * 2 * sizeof *c->track);
		if(!tmp) {
			return -1;
		}
		c->track = tmp;
		c->tsize *= 2;
	}

	memmove(c->track + kidx + 1, c->track + kidx, (c->key_count - kidx) * sizeof *c->track);
	c->track[kidx].t = t;
	c->track[kidx].val = val;
	c->key_count++;
	return 0;
}

#ifndef DBG_SEARCH_LINEAR
static int find_key(struct key *track, int beg, int end, float t)
{
	int mid = (beg + end) / 2;

	if(beg == end) {
		return beg;
	}

	if(track[mid].t < t) {
		return find_key(track, mid + 1, end, t);
	}
	return find_key(track, beg, mid, t);
}
#else
static int find_key(struct key *track, int beg, int end, float t)
{
	int i;

	for(i=beg; i<end; i++) {
		if(track[i].t >= t) {
			return i;
		}
	}
	return end;
}
#endif

#define LERP(a, b, t)	((a) + ((b) - (a)) * (t))

#define PAR_RNG_CHECK(t) \
	do {\
		int last = c->key_count - 1; \
		if(t <= c->track[0].t) return c->track[0].val; \
		if(t >= c->track[last].t) return c->track[last].val; \
	} while(0)

static inline int find_segm(curve_t *c, float *t)
{
	int seg = find_key(c->track, 0, c->key_count, *t);
	if(*t != c->track[seg].t) {
		seg--;
	}
	*t = (*t - c->track[seg].t) / (c->track[seg + 1].t - c->track[seg].t);
	return seg;
}

static vec4_t eval_linear(curve_t *c, float t)
{
	int seg;
	vec4_t res;

	PAR_RNG_CHECK(t);

	seg = find_segm(c, &t);

	res.x = LERP(c->track[seg].val.x, c->track[seg + 1].val.x, t);
	res.y = LERP(c->track[seg].val.y, c->track[seg + 1].val.y, t);
	res.z = LERP(c->track[seg].val.z, c->track[seg + 1].val.z, t);
	res.w = LERP(c->track[seg].val.w, c->track[seg + 1].val.w, t);
	return res;
}

static vec4_t eval_cosine(curve_t *c, float t)
{
	return v4_cons(0, 0, 0, 0);	/* TODO */
}

static vec4_t eval_spline(curve_t *c, float t)
{
	int seg;
	vec4_t cp[4], res = {0, 0, 0, 0};

	switch(c->key_count) {
	case 0:
		return res;
	case 1:
		return c->track[0].val;
	case 2:
		return eval_linear(c, t);
	default:
		break;
	}

	PAR_RNG_CHECK(t);

	seg = find_segm(c, &t);

	cp[1] = c->track[seg].val;
	cp[2] = c->track[seg + 1].val;
	cp[0] = seg ? c->track[seg - 1].val : cp[1];
	cp[3] = seg < c->key_count - 2 ? c->track[seg + 2].val : cp[2];

	res.x = catmull_rom_spline(cp[0].x, cp[1].x, cp[2].x, cp[3].x, t);
	res.y = catmull_rom_spline(cp[0].y, cp[1].y, cp[2].y, cp[3].y, t);
	res.z = catmull_rom_spline(cp[0].z, cp[1].z, cp[2].z, cp[3].z, t);
	res.w = catmull_rom_spline(cp[0].w, cp[1].w, cp[2].w, cp[3].w, t);

	return res;
}
