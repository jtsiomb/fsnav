#ifndef VMATH_QUATERNION_H_
#define VMATH_QUATERNION_H_

#include <stdio.h>
#include "vmath_types.h"
#include "vector.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

#define quat_cons(s, x, y, z)	v4_cons(x, y, z, s)
#define quat_vec(q)				v3_cons((q).x, (q).y, (q).z)
#define quat_s(q)				((q).w)
#define quat_identity()			quat_cons(1.0, 0.0, 0.0, 0.0)
void quat_print(FILE *fp, quat_t q);

#define quat_add		v4_add
#define quat_sub		v4_sub
#define quat_neg		v4_neg

static inline quat_t quat_mul(quat_t q1, quat_t q2);

static inline quat_t quat_conjugate(quat_t q);

#define quat_length		v4_length
#define quat_length_sq	v4_length_sq

#define quat_normalize	v4_normalize
static inline quat_t quat_inverse(quat_t q);

quat_t quat_rotate(quat_t q, scalar_t angle, scalar_t x, scalar_t y, scalar_t z);
quat_t quat_rotate_quat(quat_t q, quat_t rotq);

static inline void quat_to_mat3(mat3_t res, quat_t q);
static inline void quat_to_mat4(mat4_t res, quat_t q);

#define quat_lerp quat_slerp
quat_t quat_slerp(quat_t q1, quat_t q2, scalar_t t);


#ifdef __cplusplus
}	/* extern "C" */

#include <iostream>

/* Quaternion */
class Quaternion {
public:
	scalar_t s;
	Vector3 v;

	Quaternion();
	Quaternion(scalar_t s, const Vector3 &v);
	Quaternion(scalar_t s, scalar_t x, scalar_t y, scalar_t z);
	Quaternion(const Vector3 &axis, scalar_t angle);
	Quaternion(const quat_t &quat);

	Quaternion operator +(const Quaternion &quat) const;
	Quaternion operator -(const Quaternion &quat) const;
	Quaternion operator -() const;
	Quaternion operator *(const Quaternion &quat) const;
	
	void operator +=(const Quaternion &quat);
	void operator -=(const Quaternion &quat);
	void operator *=(const Quaternion &quat);

	void reset_identity();

	Quaternion conjugate() const;

	scalar_t length() const;
	scalar_t length_sq() const;
	
	void normalize();
	Quaternion normalized() const;

	Quaternion inverse() const;

	void set_rotation(const Vector3 &axis, scalar_t angle);
	void rotate(const Vector3 &axis, scalar_t angle);
	/* note: this is a totally different operation from the above
	 * this treats the quaternion as signifying direction and rotates
	 * it by a rotation quaternion by rot * q * rot'
	 */
	void rotate(const Quaternion &q);

	Matrix3x3 get_rotation_matrix() const;
	
	friend Quaternion slerp(const Quaternion &q1, const Quaternion &q2, scalar_t t);
	
	friend std::ostream &operator <<(std::ostream &out, const Quaternion &q);
};

Quaternion slerp(const Quaternion &q1, const Quaternion &q2, scalar_t t);
inline Quaternion lerp(const Quaternion &q1, const Quaternion &q2, scalar_t t);
#endif	/* __cplusplus */

#include "quat.inl"

#endif	/* VMATH_QUATERNION_H_ */
