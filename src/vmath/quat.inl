#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

static inline quat_t quat_mul(quat_t q1, quat_t q2)
{
	quat_t res;
	vec3_t v1 = quat_vec(q1);
	vec3_t v2 = quat_vec(q2);

	res.w = q1.w * q2.w - v3_dot(v1, v2);
	/* resvec = v2 * q1 + v1 * q2 + cross(v1, v2) */
	res.x = v2.x * q1.w + v1.x * q2.w + (v1.y * v2.z - v1.z * v2.y);
	res.y = v2.y * q1.w + v1.y * q2.w + (v1.z * v2.x - v1.x * v2.z);
	res.z = v2.z * q1.w + v1.z * q2.w + (v1.x * v2.y - v1.y * v2.x);
	return res;
}

static inline quat_t quat_conjugate(quat_t q)
{
	q.x = -q.x;
	q.y = -q.y;
	q.z = -q.z;
	return q;
}

static inline quat_t quat_inverse(quat_t q)
{
	scalar_t lensq = quat_length_sq(q);
	q = quat_conjugate(q);
	q.x /= lensq;
	q.y /= lensq;
	q.z /= lensq;
	q.w /= lensq;
	return q;
}

static inline void quat_to_mat3(mat3_t res, quat_t q)
{
	m3_cons(res, 1.0 - 2.0 * q.y*q.y - 2.0 * q.z*q.z, 2.0 * q.x * q.y + 2.0 * q.w * q.z,   2.0 * q.z * q.x - 2.0 * q.w * q.y,
				 2.0 * q.x * q.y - 2.0 * q.w * q.z,   1.0 - 2.0 * q.x*q.x - 2.0 * q.z*q.z, 2.0 * q.y * q.z + 2.0 * q.w * q.x,
				 2.0 * q.z * q.x + 2.0 * q.w * q.y,   2.0 * q.y * q.z - 2.0 * q.w * q.x,   1.0 - 2.0 * q.x*q.x - 2.0 * q.y*q.y);
}

static inline void quat_to_mat4(mat4_t res, quat_t q)
{
	m4_cons(res, 1.0 - 2.0 * q.y*q.y - 2.0 * q.z*q.z, 2.0 * q.x * q.y + 2.0 * q.w * q.z,   2.0 * q.z * q.x - 2.0 * q.w * q.y,   0,
				 2.0 * q.x * q.y - 2.0 * q.w * q.z,   1.0 - 2.0 * q.x*q.x - 2.0 * q.z*q.z, 2.0 * q.y * q.z + 2.0 * q.w * q.x,   0,
				 2.0 * q.z * q.x + 2.0 * q.w * q.y,   2.0 * q.y * q.z - 2.0 * q.w * q.x,   1.0 - 2.0 * q.x*q.x - 2.0 * q.y*q.y, 0,
				 0, 0, 0, 1);
}

#ifdef __cplusplus
}	/* extern "C" */

inline Quaternion lerp(const Quaternion &a, const Quaternion &b, scalar_t t)
{
	return slerp(a, b, t);
}
#endif	/* __cplusplus */
