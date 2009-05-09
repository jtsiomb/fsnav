#ifndef VMATH_MATRIX_H_
#define VMATH_MATRIX_H_

#include <stdio.h>
#include "vmath_types.h"

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

/* C matrix 3x3 functions */
static inline void m3_identity(mat3_t m);
static inline void m3_cons(mat3_t m,
		scalar_t m11, scalar_t m12, scalar_t m13,
		scalar_t m21, scalar_t m22, scalar_t m23,
		scalar_t m31, scalar_t m32, scalar_t m33);
static inline void m3_copy(mat3_t dest, mat3_t src);
void m3_to_m4(mat4_t dest, mat3_t src);

void m3_print(FILE *fp, mat3_t m);

/* C matrix 4x4 functions */
static inline void m4_identity(mat4_t m);
static inline void m4_cons(mat4_t m,
		scalar_t m11, scalar_t m12, scalar_t m13, scalar_t m14,
		scalar_t m21, scalar_t m22, scalar_t m23, scalar_t m24,
		scalar_t m31, scalar_t m32, scalar_t m33, scalar_t m34,
		scalar_t m41, scalar_t m42, scalar_t m43, scalar_t m44);
static inline void m4_copy(mat4_t dest, mat4_t src);
void m4_to_m3(mat3_t dest, mat4_t src);

static inline void m4_mult(mat4_t res, mat4_t m1, mat4_t m2);

void m4_translate(mat4_t m, scalar_t x, scalar_t y, scalar_t z);
void m4_rotate(mat4_t m, scalar_t x, scalar_t y, scalar_t z);
void m4_rotate_x(mat4_t m, scalar_t angle);
void m4_rotate_y(mat4_t m, scalar_t angle);
void m4_rotate_z(mat4_t m, scalar_t angle);
void m4_rotate_axis(mat4_t m, scalar_t angle, scalar_t x, scalar_t y, scalar_t z);
void m4_rotate_quat(mat4_t m, quat_t q);
void m4_scale(mat4_t m, scalar_t x, scalar_t y, scalar_t z);
static inline void m4_set_column(mat4_t m, vec4_t v, int idx);
static inline void m4_set_row(mat4_t m, vec4_t v, int idx);

void m4_transpose(mat4_t res, mat4_t m);
scalar_t m4_determinant(mat4_t m);
void m4_adjoint(mat4_t res, mat4_t m);
void m4_inverse(mat4_t res, mat4_t m);

void m4_print(FILE *fp, mat4_t m);

#ifdef __cplusplus
}

/* when included from C++ source files, also define the matrix classes */
#include <iostream>

/** 3x3 matrix */
class Matrix3x3 {
private:
	scalar_t m[3][3];

public:
	
	static Matrix3x3 identity;

	Matrix3x3();
	Matrix3x3(	scalar_t m11, scalar_t m12, scalar_t m13,
				scalar_t m21, scalar_t m22, scalar_t m23,
				scalar_t m31, scalar_t m32, scalar_t m33);
	Matrix3x3(const mat3_t cmat);
	
	Matrix3x3(const Matrix4x4 &mat4x4);
	
	/* binary operations matrix (op) matrix */
	friend Matrix3x3 operator +(const Matrix3x3 &m1, const Matrix3x3 &m2);
	friend Matrix3x3 operator -(const Matrix3x3 &m1, const Matrix3x3 &m2);
	friend Matrix3x3 operator *(const Matrix3x3 &m1, const Matrix3x3 &m2);
	
	friend void operator +=(Matrix3x3 &m1, const Matrix3x3 &m2);
	friend void operator -=(Matrix3x3 &m1, const Matrix3x3 &m2);
	friend void operator *=(Matrix3x3 &m1, const Matrix3x3 &m2);
	
	/* binary operations matrix (op) scalar and scalar (op) matrix */
	friend Matrix3x3 operator *(const Matrix3x3 &mat, scalar_t scalar);
	friend Matrix3x3 operator *(scalar_t scalar, const Matrix3x3 &mat);
	
	friend void operator *=(Matrix3x3 &mat, scalar_t scalar);
	
	inline scalar_t *operator [](int index);
	inline const scalar_t *operator [](int index) const;
	
	inline void reset_identity();
	
	void translate(const Vector2 &trans);
	void set_translation(const Vector2 &trans);
	
	void rotate(scalar_t angle);						/* 2d rotation */
	void rotate(const Vector3 &euler_angles);			/* 3d rotation with euler angles */
	void rotate(const Vector3 &axis, scalar_t angle);	/* 3d axis/angle rotation */
	void set_rotation(scalar_t angle);
	void set_rotation(const Vector3 &euler_angles);
	void set_rotation(const Vector3 &axis, scalar_t angle);
	
	void scale(const Vector3 &scale_vec);
	void set_scaling(const Vector3 &scale_vec);
	
	void set_column_vector(const Vector3 &vec, unsigned int col_index);
	void set_row_vector(const Vector3 &vec, unsigned int row_index);
	Vector3 get_column_vector(unsigned int col_index) const;
	Vector3 get_row_vector(unsigned int row_index) const;

	void transpose();
	Matrix3x3 transposed() const;	
	scalar_t determinant() const;
	Matrix3x3 inverse() const;
	
	friend std::ostream &operator <<(std::ostream &out, const Matrix3x3 &mat);
};

/** 4x4 matrix */
class Matrix4x4 {
private:
	scalar_t m[4][4];

public:
	
	static Matrix4x4 identity;

	Matrix4x4();
	Matrix4x4(	scalar_t m11, scalar_t m12, scalar_t m13, scalar_t m14,
				scalar_t m21, scalar_t m22, scalar_t m23, scalar_t m24,
				scalar_t m31, scalar_t m32, scalar_t m33, scalar_t m34,
				scalar_t m41, scalar_t m42, scalar_t m43, scalar_t m44);
	Matrix4x4(const mat4_t cmat);
	
	Matrix4x4(const Matrix3x3 &mat3x3);
	
	/* binary operations matrix (op) matrix */
	friend Matrix4x4 operator +(const Matrix4x4 &m1, const Matrix4x4 &m2);
	friend Matrix4x4 operator -(const Matrix4x4 &m1, const Matrix4x4 &m2);
	friend Matrix4x4 operator *(const Matrix4x4 &m1, const Matrix4x4 &m2);
	
	friend void operator +=(Matrix4x4 &m1, const Matrix4x4 &m2);
	friend void operator -=(Matrix4x4 &m1, const Matrix4x4 &m2);
	friend inline void operator *=(Matrix4x4 &m1, const Matrix4x4 &m2);
	
	/* binary operations matrix (op) scalar and scalar (op) matrix */
	friend Matrix4x4 operator *(const Matrix4x4 &mat, scalar_t scalar);
	friend Matrix4x4 operator *(scalar_t scalar, const Matrix4x4 &mat);
	
	friend void operator *=(Matrix4x4 &mat, scalar_t scalar);
	
	inline scalar_t *operator [](int index);
	inline const scalar_t *operator [](int index) const;
	
	inline void reset_identity();
	
	void translate(const Vector3 &trans);
	void set_translation(const Vector3 &trans);
	
	void rotate(const Vector3 &euler_angles);			/* 3d rotation with euler angles */
	void rotate(const Vector3 &axis, scalar_t angle);	/* 3d axis/angle rotation */
	void set_rotation(const Vector3 &euler_angles);
	void set_rotation(const Vector3 &axis, scalar_t angle);
	
	void scale(const Vector4 &scale_vec);
	void set_scaling(const Vector4 &scale_vec);
	
	void set_column_vector(const Vector4 &vec, unsigned int col_index);
	void set_row_vector(const Vector4 &vec, unsigned int row_index);
	Vector4 get_column_vector(unsigned int col_index) const;
	Vector4 get_row_vector(unsigned int row_index) const;
	
	void transpose();
	Matrix4x4 transposed() const;
	scalar_t determinant() const;
	Matrix4x4 adjoint() const;
	Matrix4x4 inverse() const;
	
	friend std::ostream &operator <<(std::ostream &out, const Matrix4x4 &mat);
};
#endif	/* __cplusplus */

#include "matrix.inl"

#endif	/* VMATH_MATRIX_H_ */
