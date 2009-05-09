#ifndef VMATH_CURVES_H_
#define VMATH_CURVES_H_

#include "vmath_types.h"

enum {
	CURVE_LINEAR,
	CURVE_COS,
	CURVE_SPLINE
};

struct curve {
	struct key {
		float t;
		vec4_t val;
	} *track;
	int key_count, tsize;

	vec4_t (*eval)(struct curve *c, float t);
};

typedef struct curve curve_t;

#ifdef __cplusplus
extern "C" {
#endif

int curve_cons(curve_t *c);
void curve_free(curve_t *c);
void curve_mode(curve_t *c, int cmode);

static inline int curve_value(curve_t *c, float t, float val);
static inline int curve_value3(curve_t *c, float t, vec3_t val);
int curve_value4(curve_t *c, float t, vec4_t val);

static inline float curve_eval(curve_t *c, float t);
static inline vec3_t curve_eval3(curve_t *c, float t);
static inline vec4_t curve_eval4(curve_t *c, float t);

#include "curves.inl"

#ifdef __cplusplus
}
#endif

#endif	/* VMATH_CURVES_H_ */
