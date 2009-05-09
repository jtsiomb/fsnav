#include "ray.h"

ray_t ray_transform(ray_t r, mat4_t xform)
{
	mat4_t upper;
	vec3_t dir;

	m4_copy(upper, xform);
	upper[0][3] = upper[1][3] = upper[2][3] = upper[3][0] = upper[3][1] = upper[3][2] = 0.0;
	upper[3][3] = 1.0;
	
	dir = v3_sub(r.dir, r.origin);
	dir = v3_transform(dir, upper);
	r.origin = v3_transform(r.origin, xform);
	r.dir = v3_add(dir, r.origin);
	return r;
}
