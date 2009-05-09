#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

static inline ray_t ray_cons(vec3_t origin, vec3_t dir)
{
	ray_t r;
	r.origin = origin;
	r.dir = dir;
	return r;
}

#ifdef __cplusplus
}
#endif	/* __cplusplus */
