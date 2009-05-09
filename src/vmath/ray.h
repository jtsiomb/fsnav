#ifndef VMATH_RAY_H_
#define VMATH_RAY_H_

#include "vector.h"
#include "matrix.h"

typedef struct {
	vec3_t origin, dir;
} ray_t;

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

static inline ray_t ray_cons(vec3_t origin, vec3_t dir);
ray_t ray_transform(ray_t r, mat4_t m);

#ifdef __cplusplus
}	/* __cplusplus */

#include <stack>

class Ray {
private:
	std::stack<scalar_t> ior_stack;

public:
	/* enviornmental index of refraction, normally 1.0 */
	static scalar_t env_ior;
	
	Vector3 origin, dir;
	scalar_t energy;
	int iter;
	scalar_t ior;
	long time;

	Ray();
	Ray(const Vector3 &origin, const Vector3 &dir);

	void transform(const Matrix4x4 &xform);
	Ray transformed(const Matrix4x4 &xform) const;

	void enter(scalar_t new_ior);
	void leave();
};
#endif	/* __cplusplus */

#include "ray.inl"

#endif	/* VMATH_RAY_H_ */
