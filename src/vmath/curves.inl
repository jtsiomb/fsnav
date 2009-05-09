static inline int curve_value(curve_t *c, float t, float val)
{
	vec4_t vec = {0, 0, 0, 0};
	vec.x = val;
	return curve_value4(c, t, vec);
}

static inline int curve_value3(curve_t *c, float t, vec3_t val)
{
	vec4_t vec;
	vec.x = val.x;
	vec.y = val.y;
	vec.z = val.z;
	vec.w = 0.0f;
	return curve_value4(c, t, vec);
}

static inline float curve_eval(curve_t *c, float t)
{
	return c->eval(c, t).x;
}

static inline vec3_t curve_eval3(curve_t *c, float t)
{
	vec4_t res = c->eval(c, t);
	return *(vec3_t*)&res;
}

static inline vec4_t curve_eval4(curve_t *c, float t)
{
	return c->eval(c, t);
}
