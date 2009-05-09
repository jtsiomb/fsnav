#ifndef VMATH_VECTOR_H_
#define VMATH_VECTOR_H_

#include <stdio.h>
#include "vmath_types.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* C 2D vector functions */
static inline vec2_t v2_cons(scalar_t x, scalar_t y);
static inline void v2_print(FILE *fp, vec2_t v);

static inline vec2_t v2_add(vec2_t v1, vec2_t v2);
static inline vec2_t v2_sub(vec2_t v1, vec2_t v2);
static inline vec2_t v2_scale(vec2_t v, scalar_t s);
static inline scalar_t v2_dot(vec2_t v1, vec2_t v2);
static inline scalar_t v2_length(vec2_t v);
static inline scalar_t v2_length_sq(vec2_t v);
static inline vec2_t v2_normalize(vec2_t v);

/* C 3D vector functions */
static inline vec3_t v3_cons(scalar_t x, scalar_t y, scalar_t z);
static inline void v3_print(FILE *fp, vec3_t v);

static inline vec3_t v3_add(vec3_t v1, vec3_t v2);
static inline vec3_t v3_sub(vec3_t v1, vec3_t v2);
static inline vec3_t v3_neg(vec3_t v);
static inline vec3_t v3_mul(vec3_t v1, vec3_t v2);
static inline vec3_t v3_scale(vec3_t v1, scalar_t s);
static inline scalar_t v3_dot(vec3_t v1, vec3_t v2);
static inline vec3_t v3_cross(vec3_t v1, vec3_t v2);
static inline scalar_t v3_length(vec3_t v);
static inline scalar_t v3_length_sq(vec3_t v);
static inline vec3_t v3_normalize(vec3_t v);
static inline vec3_t v3_transform(vec3_t v, mat4_t m);

static inline vec3_t v3_rotate(vec3_t v, scalar_t x, scalar_t y, scalar_t z);
static inline vec3_t v3_rotate_axis(vec3_t v, scalar_t angle, scalar_t x, scalar_t y, scalar_t z);
static inline vec3_t v3_rotate_quat(vec3_t v, quat_t q);

static inline vec3_t v3_reflect(vec3_t v, vec3_t n);

static inline vec3_t v3_lerp(vec3_t v1, vec3_t v2, scalar_t t);

/* C 4D vector functions */
static inline vec4_t v4_cons(scalar_t x, scalar_t y, scalar_t z, scalar_t w);
static inline void v4_print(FILE *fp, vec4_t v);

static inline vec4_t v4_add(vec4_t v1, vec4_t v2);
static inline vec4_t v4_sub(vec4_t v1, vec4_t v2);
static inline vec4_t v4_neg(vec4_t v);
static inline vec4_t v4_mul(vec4_t v1, vec4_t v2);
static inline vec4_t v4_scale(vec4_t v, scalar_t s);
static inline scalar_t v4_dot(vec4_t v1, vec4_t v2);
static inline scalar_t v4_length(vec4_t v);
static inline scalar_t v4_length_sq(vec4_t v);
static inline vec4_t v4_normalize(vec4_t v);
static inline vec4_t v4_transform(vec4_t v, mat4_t m);

#ifdef __cplusplus
}	/* extern "C" */

/* when included from C++ source files, also define the vector classes */
#include <iostream>

/** 2D Vector */
class Vector2 {
public:
	scalar_t x, y;

	Vector2(scalar_t x = 0.0, scalar_t y = 0.0);
	Vector2(const vec2_t &vec);
	Vector2(const Vector3 &vec);
	Vector2(const Vector4 &vec);
	
	inline scalar_t &operator [](int elem);	
	
	/* unary operations */
	friend inline Vector2 operator -(const Vector2 &vec);
	
	/* binary vector (op) vector operations */
	friend inline scalar_t dot_product(const Vector2 &v1, const Vector2 &v2);
	
	friend inline Vector2 operator +(const Vector2 &v1, const Vector2 &v2);
	friend inline Vector2 operator -(const Vector2 &v1, const Vector2 &v2);	
	friend inline Vector2 operator *(const Vector2 &v1, const Vector2 &v2);
	friend inline Vector2 operator /(const Vector2 &v1, const Vector2 &v2);
	friend inline bool operator ==(const Vector2 &v1, const Vector2 &v2);
	
	friend inline void operator +=(Vector2 &v1, const Vector2 &v2);
	friend inline void operator -=(Vector2 &v1, const Vector2 &v2);
	friend inline void operator *=(Vector2 &v1, const Vector2 &v2);
	friend inline void operator /=(Vector2 &v1, const Vector2 &v2);
	
	/* binary vector (op) scalar and scalar (op) vector operations */
	friend inline Vector2 operator +(const Vector2 &vec, scalar_t scalar);
	friend inline Vector2 operator +(scalar_t scalar, const Vector2 &vec);
	friend inline Vector2 operator -(const Vector2 &vec, scalar_t scalar);
	friend inline Vector2 operator -(scalar_t scalar, const Vector2 &vec);
	friend inline Vector2 operator *(const Vector2 &vec, scalar_t scalar);
	friend inline Vector2 operator *(scalar_t scalar, const Vector2 &vec);
	friend inline Vector2 operator /(const Vector2 &vec, scalar_t scalar);
	friend inline Vector2 operator /(scalar_t scalar, const Vector2 &vec);
	
	friend inline void operator +=(Vector2 &vec, scalar_t scalar);
	friend inline void operator -=(Vector2 &vec, scalar_t scalar);
	friend inline void operator *=(Vector2 &vec, scalar_t scalar);
	friend inline void operator /=(Vector2 &vec, scalar_t scalar);
	
	inline scalar_t length() const;
	inline scalar_t length_sq() const;
	void normalize();
	Vector2 normalized() const;

	void transform(const Matrix3x3 &mat);
	Vector2 transformed(const Matrix3x3 &mat) const;

	void rotate(scalar_t angle);
	Vector2 rotated(scalar_t angle) const;
			
	Vector2 reflection(const Vector2 &normal) const;
	Vector2 refraction(const Vector2 &normal, scalar_t src_ior, scalar_t dst_ior) const;
	
	friend std::ostream &operator <<(std::ostream &out, const Vector2 &vec);
};

inline Vector2 lerp(const Vector2 &a, const Vector2 &b, scalar_t t);
inline Vector2 catmull_rom_spline(const Vector2 &v0, const Vector2 &v1,
		const Vector2 &v2, const Vector2 &v3, scalar_t t);

/* 3D Vector */
class Vector3 {
public:
	scalar_t x, y, z;

	Vector3(scalar_t x = 0.0, scalar_t y = 0.0, scalar_t z = 0.0);
	Vector3(const vec3_t &vec);
	Vector3(const Vector2 &vec);
	Vector3(const Vector4 &vec);
	Vector3(const SphVector &sph);

	Vector3 &operator =(const SphVector &sph);
	
	inline scalar_t &operator [](int elem);	
	
	/* unary operations */
	friend inline Vector3 operator -(const Vector3 &vec);
	
	/* binary vector (op) vector operations */
	friend inline scalar_t dot_product(const Vector3 &v1, const Vector3 &v2);
	friend inline Vector3 cross_product(const Vector3 &v1, const Vector3 &v2);
	
	friend inline Vector3 operator +(const Vector3 &v1, const Vector3 &v2);
	friend inline Vector3 operator -(const Vector3 &v1, const Vector3 &v2);	
	friend inline Vector3 operator *(const Vector3 &v1, const Vector3 &v2);	
	friend inline Vector3 operator /(const Vector3 &v1, const Vector3 &v2);	
	friend inline bool operator ==(const Vector3 &v1, const Vector3 &v2);
	
	friend inline void operator +=(Vector3 &v1, const Vector3 &v2);
	friend inline void operator -=(Vector3 &v1, const Vector3 &v2);
	friend inline void operator *=(Vector3 &v1, const Vector3 &v2);
	friend inline void operator /=(Vector3 &v1, const Vector3 &v2);
	
	/* binary vector (op) scalar and scalar (op) vector operations */
	friend inline Vector3 operator +(const Vector3 &vec, scalar_t scalar);
	friend inline Vector3 operator +(scalar_t scalar, const Vector3 &vec);
	friend inline Vector3 operator -(const Vector3 &vec, scalar_t scalar);
	friend inline Vector3 operator -(scalar_t scalar, const Vector3 &vec);
	friend inline Vector3 operator *(const Vector3 &vec, scalar_t scalar);
	friend inline Vector3 operator *(scalar_t scalar, const Vector3 &vec);
	friend inline Vector3 operator /(const Vector3 &vec, scalar_t scalar);
	friend inline Vector3 operator /(scalar_t scalar, const Vector3 &vec);
	
	friend inline void operator +=(Vector3 &vec, scalar_t scalar);
	friend inline void operator -=(Vector3 &vec, scalar_t scalar);
	friend inline void operator *=(Vector3 &vec, scalar_t scalar);
	friend inline void operator /=(Vector3 &vec, scalar_t scalar);
	
	inline scalar_t length() const;
	inline scalar_t length_sq() const;
	void normalize();
	Vector3 normalized() const;
	
	void transform(const Matrix3x3 &mat);
	Vector3 transformed(const Matrix3x3 &mat) const;
	void transform(const Matrix4x4 &mat);
	Vector3 transformed(const Matrix4x4 &mat) const;
	void transform(const Quaternion &quat);
	Vector3 transformed(const Quaternion &quat) const;

	void rotate(const Vector3 &euler);
	Vector3 rotated(const Vector3 &euler) const;
	
	Vector3 reflection(const Vector3 &normal) const;
	Vector3 refraction(const Vector3 &normal, scalar_t src_ior, scalar_t dst_ior) const;
	
	friend std::ostream &operator <<(std::ostream &out, const Vector3 &vec);
};

inline Vector3 lerp(const Vector3 &a, const Vector3 &b, scalar_t t);
inline Vector3 catmull_rom_spline(const Vector3 &v0, const Vector3 &v1,
		const Vector3 &v2, const Vector3 &v3, scalar_t t);

/* 4D Vector */
class Vector4 {
public:
	scalar_t x, y, z, w;

	Vector4(scalar_t x = 0.0, scalar_t y = 0.0, scalar_t z = 0.0, scalar_t w = 0.0);
	Vector4(const vec4_t &vec);
	Vector4(const Vector2 &vec);
	Vector4(const Vector3 &vec);
	
	inline scalar_t &operator [](int elem);	
	
	/* unary operations */
	friend inline Vector4 operator -(const Vector4 &vec);
		
	/* binary vector (op) vector operations */
	friend inline scalar_t dot_product(const Vector4 &v1, const Vector4 &v2);
	friend inline Vector4 cross_product(const Vector4 &v1, const Vector4 &v2, const Vector4 &v3);
	
	friend inline Vector4 operator +(const Vector4 &v1, const Vector4 &v2);
	friend inline Vector4 operator -(const Vector4 &v1, const Vector4 &v2);	
	friend inline Vector4 operator *(const Vector4 &v1, const Vector4 &v2);	
	friend inline Vector4 operator /(const Vector4 &v1, const Vector4 &v2);	
	friend inline bool operator ==(const Vector4 &v1, const Vector4 &v2);
	
	friend inline void operator +=(Vector4 &v1, const Vector4 &v2);
	friend inline void operator -=(Vector4 &v1, const Vector4 &v2);
	friend inline void operator *=(Vector4 &v1, const Vector4 &v2);
	friend inline void operator /=(Vector4 &v1, const Vector4 &v2);
	
	/* binary vector (op) scalar and scalar (op) vector operations */
	friend inline Vector4 operator +(const Vector4 &vec, scalar_t scalar);
	friend inline Vector4 operator +(scalar_t scalar, const Vector4 &vec);
	friend inline Vector4 operator -(const Vector4 &vec, scalar_t scalar);
	friend inline Vector4 operator -(scalar_t scalar, const Vector4 &vec);
	friend inline Vector4 operator *(const Vector4 &vec, scalar_t scalar);
	friend inline Vector4 operator *(scalar_t scalar, const Vector4 &vec);
	friend inline Vector4 operator /(const Vector4 &vec, scalar_t scalar);
	friend inline Vector4 operator /(scalar_t scalar, const Vector4 &vec);
	
	friend inline void operator +=(Vector4 &vec, scalar_t scalar);
	friend inline void operator -=(Vector4 &vec, scalar_t scalar);
	friend inline void operator *=(Vector4 &vec, scalar_t scalar);
	friend inline void operator /=(Vector4 &vec, scalar_t scalar);
	
	inline scalar_t length() const;
	inline scalar_t length_sq() const;
	void normalize();
	Vector4 normalized() const;

	void transform(const Matrix4x4 &mat);
	Vector4 transformed(const Matrix4x4 &mat) const;
		
	Vector4 reflection(const Vector4 &normal) const;
	Vector4 refraction(const Vector4 &normal, scalar_t src_ior, scalar_t dst_ior) const;
	
	friend std::ostream &operator <<(std::ostream &out, const Vector4 &vec);
};

inline Vector4 lerp(const Vector4 &v0, const Vector4 &v1, scalar_t t);
inline Vector4 catmull_rom_spline(const Vector4 &v0, const Vector4 &v1,
		const Vector4 &v2, const Vector4 &v3, scalar_t t);

#endif	/* __cplusplus */

#include "vector.inl"

#endif	/* VMATH_VECTOR_H_ */
