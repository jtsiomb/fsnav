#include <math.h>
#include "vmath.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* C 2D vector functions */
static inline vec2_t v2_cons(scalar_t x, scalar_t y)
{
	vec2_t v;
	v.x = x;
	v.y = y;
	return v;
}

static inline void v2_print(FILE *fp, vec2_t v)
{
	fprintf(fp, "[ %.4f %.4f ]", v.x, v.y);
}

static inline vec2_t v2_add(vec2_t v1, vec2_t v2)
{
	vec2_t res;
	res.x = v1.x + v2.x;
	res.y = v1.y + v2.y;
	return res;
}

static inline vec2_t v2_sub(vec2_t v1, vec2_t v2)
{
	vec2_t res;
	res.x = v1.x - v2.x;
	res.y = v1.y - v2.y;
	return res;
}

static inline vec2_t v2_scale(vec2_t v, scalar_t s)
{
	vec2_t res;
	res.x = v.x * s;
	res.y = v.y * s;
	return res;
}

static inline scalar_t v2_dot(vec2_t v1, vec2_t v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

static inline scalar_t v2_length(vec2_t v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}

static inline scalar_t v2_length_sq(vec2_t v)
{
	return v.x * v.x + v.y * v.y;
}

static inline vec2_t v2_normalize(vec2_t v)
{
	scalar_t len = (scalar_t)sqrt(v.x * v.x + v.y * v.y);
	v.x /= len;
	v.y /= len;
	return v;
}


/* C 3D vector functions */
static inline vec3_t v3_cons(scalar_t x, scalar_t y, scalar_t z)
{
	vec3_t v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

static inline void v3_print(FILE *fp, vec3_t v)
{
	fprintf(fp, "[ %.4f %.4f %.4f ]", v.x, v.y, v.z);
}

static inline vec3_t v3_add(vec3_t v1, vec3_t v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

static inline vec3_t v3_sub(vec3_t v1, vec3_t v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	return v1;
}

static inline vec3_t v3_neg(vec3_t v)
{
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	return v;
}

static inline vec3_t v3_mul(vec3_t v1, vec3_t v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	return v1;
}

static inline vec3_t v3_scale(vec3_t v1, scalar_t s)
{
	v1.x *= s;
	v1.y *= s;
	v1.z *= s;
	return v1;
}

static inline scalar_t v3_dot(vec3_t v1, vec3_t v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static inline vec3_t v3_cross(vec3_t v1, vec3_t v2)
{
	vec3_t v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
	return v;
}

static inline scalar_t v3_length(vec3_t v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

static inline scalar_t v3_length_sq(vec3_t v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline vec3_t v3_normalize(vec3_t v)
{
	scalar_t len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x /= len;
	v.y /= len;
	v.z /= len;
	return v;
}

static inline vec3_t v3_transform(vec3_t v, mat4_t m)
{
	vec3_t res;
	res.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
	res.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
	res.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];
	return res;
}

static inline vec3_t v3_rotate(vec3_t v, scalar_t x, scalar_t y, scalar_t z)
{
	void m4_rotate(mat4_t, scalar_t, scalar_t, scalar_t);

	mat4_t m = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
	m4_rotate(m, x, y, z);
	return v3_transform(v, m);
}

static inline vec3_t v3_rotate_axis(vec3_t v, scalar_t angle, scalar_t x, scalar_t y, scalar_t z)
{
	void m4_rotate_axis(mat4_t, scalar_t, scalar_t, scalar_t, scalar_t);

	mat4_t m = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
	m4_rotate_axis(m, angle, x, y, z);
	return v3_transform(v, m);
}

static inline vec3_t v3_rotate_quat(vec3_t v, quat_t q)
{
	quat_t quat_rotate_quat(quat_t, quat_t);

	quat_t vq = v4_cons(v.x, v.y, v.z, 0.0);
	quat_t res = quat_rotate_quat(vq, q);
	return v3_cons(res.x, res.y, res.z);
}

static inline vec3_t v3_reflect(vec3_t v, vec3_t n)
{
	scalar_t dot = v3_dot(v, n);
	return v3_neg(v3_sub(v3_scale(n, dot * 2.0), v));
}

static inline vec3_t v3_lerp(vec3_t v1, vec3_t v2, scalar_t t)
{
	v1.x += (v2.x - v1.x) * t;
	v1.y += (v2.y - v1.y) * t;
	v1.z += (v2.z - v1.z) * t;
	return v1;
}

/* C 4D vector functions */
static inline vec4_t v4_cons(scalar_t x, scalar_t y, scalar_t z, scalar_t w)
{
	vec4_t v;
	v.x = x;
	v.y = y;
	v.z = z;
	v.w = w;
	return v;
}

static inline void v4_print(FILE *fp, vec4_t v)
{
	fprintf(fp, "[ %.4f %.4f %.4f %.4f ]", v.x, v.y, v.z, v.w);
}

static inline vec4_t v4_add(vec4_t v1, vec4_t v2)
{
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
	return v1;
}

static inline vec4_t v4_sub(vec4_t v1, vec4_t v2)
{
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
	return v1;
}

static inline vec4_t v4_neg(vec4_t v)
{
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.z;
	v.w = -v.w;
	return v;
}

static inline vec4_t v4_mul(vec4_t v1, vec4_t v2)
{
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
	return v1;
}

static inline vec4_t v4_scale(vec4_t v, scalar_t s)
{
	v.x *= s;
	v.y *= s;
	v.z *= s;
	v.w *= s;
	return v;
}

static inline scalar_t v4_dot(vec4_t v1, vec4_t v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

static inline scalar_t v4_length(vec4_t v)
{
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

static inline scalar_t v4_length_sq(vec4_t v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
}

static inline vec4_t v4_normalize(vec4_t v)
{
	scalar_t len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	v.x /= len;
	v.y /= len;
	v.z /= len;
	v.w /= len;
	return v;
}

static inline vec4_t v4_transform(vec4_t v, mat4_t m)
{
	vec4_t res;
	res.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
	res.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
	res.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
	res.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
	return res;
}

#ifdef __cplusplus
}	/* extern "C" */


/* --------------- C++ part -------------- */

inline scalar_t &Vector2::operator [](int elem) {
	return elem ? y : x;
}

inline Vector2 operator -(const Vector2 &vec) {
	return Vector2(-vec.x, -vec.y);
}

inline scalar_t dot_product(const Vector2 &v1, const Vector2 &v2) {
	return v1.x * v2.x + v1.y * v2.y;
}

inline Vector2 operator +(const Vector2 &v1, const Vector2 &v2) {
	return Vector2(v1.x + v2.x, v1.y + v2.y);
}

inline Vector2 operator -(const Vector2 &v1, const Vector2 &v2) {
	return Vector2(v1.x - v2.x, v1.y - v2.y);
}

inline Vector2 operator *(const Vector2 &v1, const Vector2 &v2) {
	return Vector2(v1.x * v2.x, v1.y * v2.y);
}

inline Vector2 operator /(const Vector2 &v1, const Vector2 &v2) {
	return Vector2(v1.x / v2.x, v1.y / v2.y);
}

inline bool operator ==(const Vector2 &v1, const Vector2 &v2) {
	return (fabs(v1.x - v2.x) < XSMALL_NUMBER) && (fabs(v1.y - v2.x) < XSMALL_NUMBER);
}

inline void operator +=(Vector2 &v1, const Vector2 &v2) {
	v1.x += v2.x;
	v1.y += v2.y;
}

inline void operator -=(Vector2 &v1, const Vector2 &v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
}

inline void operator *=(Vector2 &v1, const Vector2 &v2) {
	v1.x *= v2.x;
	v1.y *= v2.y;
}

inline void operator /=(Vector2 &v1, const Vector2 &v2) {
	v1.x /= v2.x;
	v1.y /= v2.y;
}

inline Vector2 operator +(const Vector2 &vec, scalar_t scalar) {
	return Vector2(vec.x + scalar, vec.y + scalar);
}

inline Vector2 operator +(scalar_t scalar, const Vector2 &vec) {
	return Vector2(vec.x + scalar, vec.y + scalar);
}

inline Vector2 operator -(const Vector2 &vec, scalar_t scalar) {
	return Vector2(vec.x - scalar, vec.y - scalar);
}

inline Vector2 operator -(scalar_t scalar, const Vector2 &vec) {
	return Vector2(vec.x - scalar, vec.y - scalar);
}

inline Vector2 operator *(const Vector2 &vec, scalar_t scalar) {
	return Vector2(vec.x * scalar, vec.y * scalar);
}

inline Vector2 operator *(scalar_t scalar, const Vector2 &vec) {
	return Vector2(vec.x * scalar, vec.y * scalar);
}

inline Vector2 operator /(const Vector2 &vec, scalar_t scalar) {
	return Vector2(vec.x / scalar, vec.y / scalar);
}

inline Vector2 operator /(scalar_t scalar, const Vector2 &vec) {
	return Vector2(vec.x / scalar, vec.y / scalar);
}

inline void operator +=(Vector2 &vec, scalar_t scalar) {
	vec.x += scalar;
	vec.y += scalar;
}

inline void operator -=(Vector2 &vec, scalar_t scalar) {
	vec.x -= scalar;
	vec.y -= scalar;
}

inline void operator *=(Vector2 &vec, scalar_t scalar) {
	vec.x *= scalar;
	vec.y *= scalar;
}

inline void operator /=(Vector2 &vec, scalar_t scalar) {
	vec.x /= scalar;
	vec.y /= scalar;
}

inline scalar_t Vector2::length() const {
	return sqrt(x*x + y*y);
}

inline scalar_t Vector2::length_sq() const {
	return x*x + y*y;
}

inline Vector2 lerp(const Vector2 &a, const Vector2 &b, scalar_t t)
{
	return a + (b - a) * t;
}

inline Vector2 catmull_rom_spline(const Vector2 &v0, const Vector2 &v1,
		const Vector2 &v2, const Vector2 &v3, scalar_t t)
{
	scalar_t x = catmull_rom_spline(v0.x, v1.x, v2.x, v3.x, t);
	scalar_t y = catmull_rom_spline(v0.y, v1.y, v2.y, v3.y, t);
	return Vector2(x, y);
}


/* ------------- Vector3 -------------- */

inline scalar_t &Vector3::operator [](int elem) {
	return elem ? (elem == 1 ? y : z) : x;
}

/* unary operations */
inline Vector3 operator -(const Vector3 &vec) {
	return Vector3(-vec.x, -vec.y, -vec.z);
}

/* binary vector (op) vector operations */
inline scalar_t dot_product(const Vector3 &v1, const Vector3 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector3 cross_product(const Vector3 &v1, const Vector3 &v2) {
	return Vector3(v1.y * v2.z - v1.z * v2.y,  v1.z * v2.x - v1.x * v2.z,  v1.x * v2.y - v1.y * v2.x);
}


inline Vector3 operator +(const Vector3 &v1, const Vector3 &v2) {
	return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline Vector3 operator -(const Vector3 &v1, const Vector3 &v2) {
	return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline Vector3 operator *(const Vector3 &v1, const Vector3 &v2) {
	return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}

inline Vector3 operator /(const Vector3 &v1, const Vector3 &v2) {
	return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
}

inline bool operator ==(const Vector3 &v1, const Vector3 &v2) {
	return (fabs(v1.x - v2.x) < XSMALL_NUMBER) && (fabs(v1.y - v2.y) < XSMALL_NUMBER) && (fabs(v1.z - v2.z) < XSMALL_NUMBER);
}

inline void operator +=(Vector3 &v1, const Vector3 &v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
}

inline void operator -=(Vector3 &v1, const Vector3 &v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
}

inline void operator *=(Vector3 &v1, const Vector3 &v2) {
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
}

inline void operator /=(Vector3 &v1, const Vector3 &v2) {
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
}
/* binary vector (op) scalar and scalar (op) vector operations */
inline Vector3 operator +(const Vector3 &vec, scalar_t scalar) {
	return Vector3(vec.x + scalar, vec.y + scalar, vec.z + scalar);
}

inline Vector3 operator +(scalar_t scalar, const Vector3 &vec) {
	return Vector3(vec.x + scalar, vec.y + scalar, vec.z + scalar);
}

inline Vector3 operator -(const Vector3 &vec, scalar_t scalar) {
	return Vector3(vec.x - scalar, vec.y - scalar, vec.z - scalar);
}

inline Vector3 operator -(scalar_t scalar, const Vector3 &vec) {
	return Vector3(vec.x - scalar, vec.y - scalar, vec.z - scalar);
}

inline Vector3 operator *(const Vector3 &vec, scalar_t scalar) {
	return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

inline Vector3 operator *(scalar_t scalar, const Vector3 &vec) {
	return Vector3(vec.x * scalar, vec.y * scalar, vec.z * scalar);
}

inline Vector3 operator /(const Vector3 &vec, scalar_t scalar) {
	return Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}

inline Vector3 operator /(scalar_t scalar, const Vector3 &vec) {
	return Vector3(vec.x / scalar, vec.y / scalar, vec.z / scalar);
}

inline void operator +=(Vector3 &vec, scalar_t scalar) {
	vec.x += scalar;
	vec.y += scalar;
	vec.z += scalar;
}

inline void operator -=(Vector3 &vec, scalar_t scalar) {
	vec.x -= scalar;
	vec.y -= scalar;
	vec.z -= scalar;
}

inline void operator *=(Vector3 &vec, scalar_t scalar) {
	vec.x *= scalar;
	vec.y *= scalar;
	vec.z *= scalar;
}

inline void operator /=(Vector3 &vec, scalar_t scalar) {
	vec.x /= scalar;
	vec.y /= scalar;
	vec.z /= scalar;
}

inline scalar_t Vector3::length() const {
	return sqrt(x*x + y*y + z*z);
}
inline scalar_t Vector3::length_sq() const {
	return x*x + y*y + z*z;
}

inline Vector3 lerp(const Vector3 &a, const Vector3 &b, scalar_t t) {
	return a + (b - a) * t;
}

inline Vector3 catmull_rom_spline(const Vector3 &v0, const Vector3 &v1,
		const Vector3 &v2, const Vector3 &v3, scalar_t t)
{
	scalar_t x = catmull_rom_spline(v0.x, v1.x, v2.x, v3.x, t);
	scalar_t y = catmull_rom_spline(v0.y, v1.y, v2.y, v3.y, t);
	scalar_t z = catmull_rom_spline(v0.z, v1.z, v2.z, v3.z, t);
	return Vector3(x, y, z);
}

/* ----------- Vector4 ----------------- */

inline scalar_t &Vector4::operator [](int elem) {
	return elem ? (elem == 1 ? y : (elem == 2 ? z : w)) : x;
}

inline Vector4 operator -(const Vector4 &vec) {
	return Vector4(-vec.x, -vec.y, -vec.z, -vec.w);
}

inline scalar_t dot_product(const Vector4 &v1, const Vector4 &v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline Vector4 cross_product(const Vector4 &v1, const Vector4 &v2, const Vector4 &v3) {
	float  a, b, c, d, e, f;       /* Intermediate Values */
    Vector4 result;

    /* Calculate intermediate values. */
    a = (v2.x * v3.y) - (v2.y * v3.x);
    b = (v2.x * v3.z) - (v2.z * v3.x);
    c = (v2.x * v3.w) - (v2.w * v3.x);
    d = (v2.y * v3.z) - (v2.z * v3.y);
    e = (v2.y * v3.w) - (v2.w * v3.y);
    f = (v2.z * v3.w) - (v2.w * v3.z);

    /* Calculate the result-vector components. */
    result.x =   (v1.y * f) - (v1.z * e) + (v1.w * d);
    result.y = - (v1.x * f) + (v1.z * c) - (v1.w * b);
    result.z =   (v1.x * e) - (v1.y * c) + (v1.w * a);
    result.w = - (v1.x * d) + (v1.y * b) - (v1.z * a);
    return result;
}

inline Vector4 operator +(const Vector4 &v1, const Vector4 &v2) {
	return Vector4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

inline Vector4 operator -(const Vector4 &v1, const Vector4 &v2) {
	return Vector4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

inline Vector4 operator *(const Vector4 &v1, const Vector4 &v2) {
	return Vector4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}

inline Vector4 operator /(const Vector4 &v1, const Vector4 &v2) {
	return Vector4(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z, v1.w / v2.w);
}

inline bool operator ==(const Vector4 &v1, const Vector4 &v2) {
	return 	(fabs(v1.x - v2.x) < XSMALL_NUMBER) && 
			(fabs(v1.y - v2.y) < XSMALL_NUMBER) && 
			(fabs(v1.z - v2.z) < XSMALL_NUMBER) &&
			(fabs(v1.w - v2.w) < XSMALL_NUMBER);
}

inline void operator +=(Vector4 &v1, const Vector4 &v2) {
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	v1.w += v2.w;
}

inline void operator -=(Vector4 &v1, const Vector4 &v2) {
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
}

inline void operator *=(Vector4 &v1, const Vector4 &v2) {
	v1.x *= v2.x;
	v1.y *= v2.y;
	v1.z *= v2.z;
	v1.w *= v2.w;
}

inline void operator /=(Vector4 &v1, const Vector4 &v2) {
	v1.x /= v2.x;
	v1.y /= v2.y;
	v1.z /= v2.z;
	v1.w /= v2.w;
}

/* binary vector (op) scalar and scalar (op) vector operations */
inline Vector4 operator +(const Vector4 &vec, scalar_t scalar) {
	return Vector4(vec.x + scalar, vec.y + scalar, vec.z + scalar, vec.w + scalar);
}

inline Vector4 operator +(scalar_t scalar, const Vector4 &vec) {
	return Vector4(vec.x + scalar, vec.y + scalar, vec.z + scalar, vec.w + scalar);
}

inline Vector4 operator -(const Vector4 &vec, scalar_t scalar) {
	return Vector4(vec.x - scalar, vec.y - scalar, vec.z - scalar, vec.w - scalar);
}

inline Vector4 operator -(scalar_t scalar, const Vector4 &vec) {
	return Vector4(vec.x - scalar, vec.y - scalar, vec.z - scalar, vec.w - scalar);
}

inline Vector4 operator *(const Vector4 &vec, scalar_t scalar) {
	return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
}

inline Vector4 operator *(scalar_t scalar, const Vector4 &vec) {
	return Vector4(vec.x * scalar, vec.y * scalar, vec.z * scalar, vec.w * scalar);
}

inline Vector4 operator /(const Vector4 &vec, scalar_t scalar) {
	return Vector4(vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar);
}

inline Vector4 operator /(scalar_t scalar, const Vector4 &vec) {
	return Vector4(vec.x / scalar, vec.y / scalar, vec.z / scalar, vec.w / scalar);
}

inline void operator +=(Vector4 &vec, scalar_t scalar) {
	vec.x += scalar;
	vec.y += scalar;
	vec.z += scalar;
	vec.w += scalar;
}

inline void operator -=(Vector4 &vec, scalar_t scalar) {
	vec.x -= scalar;
	vec.y -= scalar;
	vec.z -= scalar;
	vec.w -= scalar;
}

inline void operator *=(Vector4 &vec, scalar_t scalar) {
	vec.x *= scalar;
	vec.y *= scalar;
	vec.z *= scalar;
	vec.w *= scalar;
}

inline void operator /=(Vector4 &vec, scalar_t scalar) {
	vec.x /= scalar;
	vec.y /= scalar;
	vec.z /= scalar;
	vec.w /= scalar;
}

inline scalar_t Vector4::length() const {
	return sqrt(x*x + y*y + z*z + w*w);
}
inline scalar_t Vector4::length_sq() const {
	return x*x + y*y + z*z + w*w;
}

inline Vector4 lerp(const Vector4 &v0, const Vector4 &v1, scalar_t t)
{
	return v0 + (v1 - v0) * t;
}

inline Vector4 catmull_rom_spline(const Vector4 &v0, const Vector4 &v1,
		const Vector4 &v2, const Vector4 &v3, scalar_t t)
{
	scalar_t x = catmull_rom_spline(v0.x, v1.x, v2.x, v3.x, t);
	scalar_t y = catmull_rom_spline(v0.y, v1.y, v2.y, v3.y, t);
	scalar_t z = catmull_rom_spline(v0.z, v1.z, v2.z, v3.z, t);
	scalar_t w = catmull_rom_spline(v0.w, v1.w, v2.w, v3.w, t);
	return Vector4(x, y, z, w);
}

#endif	/* __cplusplus */
