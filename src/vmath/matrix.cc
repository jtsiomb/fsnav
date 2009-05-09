#include <cstdio>
#include <cmath>
#include "matrix.h"
#include "vector.h"
#include "quat.h"

using namespace std;

// ----------- Matrix3x3 --------------

Matrix3x3 Matrix3x3::identity = Matrix3x3(1, 0, 0, 0, 1, 0, 0, 0, 1);

Matrix3x3::Matrix3x3()
{
	*this = identity;
}

Matrix3x3::Matrix3x3(	scalar_t m11, scalar_t m12, scalar_t m13,
						scalar_t m21, scalar_t m22, scalar_t m23,
						scalar_t m31, scalar_t m32, scalar_t m33)
{
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13;
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23;
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33;
}

Matrix3x3::Matrix3x3(const mat3_t cmat)
{
	memcpy(m, cmat, sizeof(mat3_t));
}

Matrix3x3::Matrix3x3(const Matrix4x4 &mat4x4)
{
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			m[i][j] = mat4x4[i][j];
		}
	}
}

Matrix3x3 operator +(const Matrix3x3 &m1, const Matrix3x3 &m2)
{
	Matrix3x3 res;
	const scalar_t *op1 = m1.m[0], *op2 = m2.m[0];
	scalar_t *dest = res.m[0];
	
	for(int i=0; i<9; i++) {
		*dest++ = *op1++ + *op2++;
	}
	return res;
}

Matrix3x3 operator -(const Matrix3x3 &m1, const Matrix3x3 &m2)
{
	Matrix3x3 res;
	const scalar_t *op1 = m1.m[0], *op2 = m2.m[0];
	scalar_t *dest = res.m[0];
	
	for(int i=0; i<9; i++) {
		*dest++ = *op1++ - *op2++;
	}
	return res;
}

Matrix3x3 operator *(const Matrix3x3 &m1, const Matrix3x3 &m2)
{
	Matrix3x3 res;
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			res.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j];
		}
	}
	return res;
}

void operator +=(Matrix3x3 &m1, const Matrix3x3 &m2)
{
	scalar_t *op1 = m1.m[0];
	const scalar_t *op2 = m2.m[0];
	
	for(int i=0; i<9; i++) {
		*op1++ += *op2++;
	}
}

void operator -=(Matrix3x3 &m1, const Matrix3x3 &m2)
{
	scalar_t *op1 = m1.m[0];
	const scalar_t *op2 = m2.m[0];
	
	for(int i=0; i<9; i++) {
		*op1++ -= *op2++;
	}
}

void operator *=(Matrix3x3 &m1, const Matrix3x3 &m2)
{
	Matrix3x3 res;
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			res.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j];
		}
	}
	memcpy(m1.m, res.m, 9 * sizeof(scalar_t));
}

Matrix3x3 operator *(const Matrix3x3 &mat, scalar_t scalar)
{
	Matrix3x3 res;
	const scalar_t *mptr = mat.m[0];
	scalar_t *dptr = res.m[0];
	
	for(int i=0; i<9; i++) {
		*dptr++ = *mptr++ * scalar;
	}
	return res;
}

Matrix3x3 operator *(scalar_t scalar, const Matrix3x3 &mat)
{
	Matrix3x3 res;
	const scalar_t *mptr = mat.m[0];
	scalar_t *dptr = res.m[0];
	
	for(int i=0; i<9; i++) {
		*dptr++ = *mptr++ * scalar;
	}
	return res;
}

void operator *=(Matrix3x3 &mat, scalar_t scalar)
{
	scalar_t *mptr = mat.m[0];
	
	for(int i=0; i<9; i++) {
		*mptr++ *= scalar;
	}
}

void Matrix3x3::translate(const Vector2 &trans)
{
	Matrix3x3 tmat(1, 0, trans.x, 0, 1, trans.y, 0, 0, 1);
	*this *= tmat;
}

void Matrix3x3::set_translation(const Vector2 &trans)
{
	*this = Matrix3x3(1, 0, trans.x, 0, 1, trans.y, 0, 0, 1);
}

void Matrix3x3::rotate(scalar_t angle)
{
	scalar_t cos_a = cos(angle);
	scalar_t sin_a = sin(angle);
	Matrix3x3 rmat(	cos_a, 	-sin_a, 	0,
					sin_a, 	cos_a, 		0,
					0,		0,			1);
	*this *= rmat;
}

void Matrix3x3::set_rotation(scalar_t angle)
{
	scalar_t cos_a = cos(angle);
	scalar_t sin_a = sin(angle);
	*this = Matrix3x3(cos_a, -sin_a, 0, sin_a, cos_a, 0, 0, 0, 1);
}

void Matrix3x3::rotate(const Vector3 &euler_angles)
{
	Matrix3x3 xrot, yrot, zrot;
	
	xrot = Matrix3x3(	1,			0,					0,
						0,	cos(euler_angles.x),	-sin(euler_angles.x),
						0,	sin(euler_angles.x),	cos(euler_angles.x));
	
	yrot = Matrix3x3(	cos(euler_angles.y),	0,	sin(euler_angles.y),
								0,				1,				0,
						-sin(euler_angles.y),	0,	cos(euler_angles.y));
	
	zrot = Matrix3x3(	cos(euler_angles.z),	-sin(euler_angles.z),	0,
						sin(euler_angles.z),	cos(euler_angles.z),	0,
								0,						0,				1);
	
	*this *= xrot * yrot * zrot;
}

void Matrix3x3::set_rotation(const Vector3 &euler_angles)
{
	Matrix3x3 xrot, yrot, zrot;
	
	xrot = Matrix3x3(	1,			0,					0,
						0,	cos(euler_angles.x),	-sin(euler_angles.x),
						0,	sin(euler_angles.x),	cos(euler_angles.x));
	
	yrot = Matrix3x3(	cos(euler_angles.y),	0,	sin(euler_angles.y),
								0,				1,				0,
						-sin(euler_angles.y),	0,	cos(euler_angles.y));
	
	zrot = Matrix3x3(	cos(euler_angles.z),	-sin(euler_angles.z),	0,
						sin(euler_angles.z),	cos(euler_angles.z),	0,
								0,						0,				1);
	
	*this = xrot * yrot * zrot;
}

void Matrix3x3::rotate(const Vector3 &axis, scalar_t angle)
{
	scalar_t sina = (scalar_t)sin(angle);
	scalar_t cosa = (scalar_t)cos(angle);
	scalar_t invcosa = 1-cosa;
	scalar_t nxsq = axis.x * axis.x;
	scalar_t nysq = axis.y * axis.y;
	scalar_t nzsq = axis.z * axis.z;

	Matrix3x3 xform;
	xform.m[0][0] = nxsq + (1-nxsq) * cosa;
	xform.m[0][1] = axis.x * axis.y * invcosa - axis.z * sina;
	xform.m[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	xform.m[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	xform.m[1][1] = nysq + (1-nysq) * cosa;
	xform.m[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	xform.m[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	xform.m[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	xform.m[2][2] = nzsq + (1-nzsq) * cosa;

	*this *= xform;
}

void Matrix3x3::set_rotation(const Vector3 &axis, scalar_t angle)
{
	scalar_t sina = (scalar_t)sin(angle);
	scalar_t cosa = (scalar_t)cos(angle);
	scalar_t invcosa = 1-cosa;
	scalar_t nxsq = axis.x * axis.x;
	scalar_t nysq = axis.y * axis.y;
	scalar_t nzsq = axis.z * axis.z;

	reset_identity();
	m[0][0] = nxsq + (1-nxsq) * cosa;
	m[0][1] = axis.x * axis.y * invcosa - axis.z * sina;
	m[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	m[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	m[1][1] = nysq + (1-nysq) * cosa;
	m[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	m[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	m[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	m[2][2] = nzsq + (1-nzsq) * cosa;
}

void Matrix3x3::scale(const Vector3 &scale_vec)
{
	Matrix3x3 smat(	scale_vec.x, 0, 0,
					0, scale_vec.y, 0,
					0, 0, scale_vec.z);
	*this *= smat;
}

void Matrix3x3::set_scaling(const Vector3 &scale_vec)
{
	*this = Matrix3x3(	scale_vec.x, 0, 0,
						0, scale_vec.y, 0,
						0, 0, scale_vec.z);
}

void Matrix3x3::set_column_vector(const Vector3 &vec, unsigned int col_index)
{
	m[0][col_index] = vec.x;
	m[1][col_index] = vec.y;
	m[2][col_index] = vec.z;
}

void Matrix3x3::set_row_vector(const Vector3 &vec, unsigned int row_index)
{
	m[row_index][0] = vec.x;
	m[row_index][1] = vec.y;
	m[row_index][2] = vec.z;
}

Vector3 Matrix3x3::get_column_vector(unsigned int col_index) const
{
	return Vector3(m[0][col_index], m[1][col_index], m[2][col_index]);
}

Vector3 Matrix3x3::get_row_vector(unsigned int row_index) const
{
	return Vector3(m[row_index][0], m[row_index][1], m[row_index][2]);
}

void Matrix3x3::transpose()
{
	Matrix3x3 tmp = *this;
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			m[i][j] = tmp[j][i];
		}
	}
}

Matrix3x3 Matrix3x3::transposed() const
{
	Matrix3x3 res;
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			res[i][j] = m[j][i];
		}
	}
	return res;
}

scalar_t Matrix3x3::determinant() const
{
	return 	m[0][0] * (m[1][1]*m[2][2] - m[1][2]*m[2][1]) -
			m[0][1] * (m[1][0]*m[2][2] - m[1][2]*m[2][0]) +
			m[0][2] * (m[1][0]*m[2][1] - m[1][1]*m[2][0]);
}

Matrix3x3 Matrix3x3::inverse() const
{
	// TODO: implement 3x3 inverse
	return *this;
}

ostream &operator <<(ostream &out, const Matrix3x3 &mat)
{
	for(int i=0; i<3; i++) {
		char str[100];
		sprintf(str, "[ %12.5f %12.5f %12.5f ]\n", (float)mat.m[i][0], (float)mat.m[i][1], (float)mat.m[i][2]);
		out << str;
	}
	return out;
}



/* ----------------- Matrix4x4 implementation --------------- */

Matrix4x4 Matrix4x4::identity = Matrix4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

Matrix4x4::Matrix4x4()
{
	*this = identity;
}

Matrix4x4::Matrix4x4(	scalar_t m11, scalar_t m12, scalar_t m13, scalar_t m14,
						scalar_t m21, scalar_t m22, scalar_t m23, scalar_t m24,
						scalar_t m31, scalar_t m32, scalar_t m33, scalar_t m34,
						scalar_t m41, scalar_t m42, scalar_t m43, scalar_t m44)
{
	m[0][0] = m11; m[0][1] = m12; m[0][2] = m13; m[0][3] = m14;
	m[1][0] = m21; m[1][1] = m22; m[1][2] = m23; m[1][3] = m24;
	m[2][0] = m31; m[2][1] = m32; m[2][2] = m33; m[2][3] = m34;
	m[3][0] = m41; m[3][1] = m42; m[3][2] = m43; m[3][3] = m44;
}

Matrix4x4::Matrix4x4(const mat4_t cmat)
{
	memcpy(m, cmat, sizeof(mat4_t));
}

Matrix4x4::Matrix4x4(const Matrix3x3 &mat3x3)
{
	reset_identity();
	for(int i=0; i<3; i++) {
		for(int j=0; j<3; j++) {
			m[i][j] = mat3x3[i][j];
		}
	}
}

Matrix4x4 operator +(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
	Matrix4x4 res;
	const scalar_t *op1 = m1.m[0], *op2 = m2.m[0];
	scalar_t *dest = res.m[0];
	
	for(int i=0; i<16; i++) {
		*dest++ = *op1++ + *op2++;
	}
	return res;
}

Matrix4x4 operator -(const Matrix4x4 &m1, const Matrix4x4 &m2)
{
	Matrix4x4 res;
	const scalar_t *op1 = m1.m[0], *op2 = m2.m[0];
	scalar_t *dest = res.m[0];
	
	for(int i=0; i<16; i++) {
		*dest++ = *op1++ - *op2++;
	}
	return res;
}

/*
Matrix4x4 operator *(const Matrix4x4 &m1, const Matrix4x4 &m2) {
	Matrix4x4 res;
	
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			res.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}

	return res;
}
*/

void operator +=(Matrix4x4 &m1, const Matrix4x4 &m2)
{
	scalar_t *op1 = m1.m[0];
	const scalar_t *op2 = m2.m[0];
	
	for(int i=0; i<16; i++) {
		*op1++ += *op2++;
	}
}

void operator -=(Matrix4x4 &m1, const Matrix4x4 &m2)
{
	scalar_t *op1 = m1.m[0];
	const scalar_t *op2 = m2.m[0];
	
	for(int i=0; i<16; i++) {
		*op1++ -= *op2++;
	}
}

void operator *=(Matrix4x4 &m1, const Matrix4x4 &m2)
{
	Matrix4x4 res;
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			res.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] + m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
		}
	}
	memcpy(m1.m, res.m, 16 * sizeof(scalar_t));
}

Matrix4x4 operator *(const Matrix4x4 &mat, scalar_t scalar)
{
	Matrix4x4 res;
	const scalar_t *mptr = mat.m[0];
	scalar_t *dptr = res.m[0];
	
	for(int i=0; i<16; i++) {
		*dptr++ = *mptr++ * scalar;
	}
	return res;
}

Matrix4x4 operator *(scalar_t scalar, const Matrix4x4 &mat)
{
	Matrix4x4 res;
	const scalar_t *mptr = mat.m[0];
	scalar_t *dptr = res.m[0];
	
	for(int i=0; i<16; i++) {
		*dptr++ = *mptr++ * scalar;
	}
	return res;
}

void operator *=(Matrix4x4 &mat, scalar_t scalar)
{
	scalar_t *mptr = mat.m[0];
	
	for(int i=0; i<16; i++) {
		*mptr++ *= scalar;
	}
}

void Matrix4x4::translate(const Vector3 &trans)
{
	Matrix4x4 tmat(1, 0, 0, trans.x, 0, 1, 0, trans.y, 0, 0, 1, trans.z, 0, 0, 0, 1);
	*this *= tmat;
}

void Matrix4x4::set_translation(const Vector3 &trans)
{
	*this = Matrix4x4(1, 0, 0, trans.x, 0, 1, 0, trans.y, 0, 0, 1, trans.z, 0, 0, 0, 1);
}

void Matrix4x4::rotate(const Vector3 &euler_angles)
{
	Matrix3x3 xrot, yrot, zrot;
	
	xrot = Matrix3x3(	1,			0,					0,
						0,	cos(euler_angles.x),	-sin(euler_angles.x),
						0,	sin(euler_angles.x),	cos(euler_angles.x));
	
	yrot = Matrix3x3(	cos(euler_angles.y),	0,	sin(euler_angles.y),
								0,				1,				0,
						-sin(euler_angles.y),	0,	cos(euler_angles.y));
	
	zrot = Matrix3x3(	cos(euler_angles.z),	-sin(euler_angles.z),	0,
						sin(euler_angles.z),	cos(euler_angles.z),	0,
								0,						0,				1);
	
	*this *= Matrix4x4(xrot * yrot * zrot);
}

void Matrix4x4::set_rotation(const Vector3 &euler_angles)
{
	Matrix3x3 xrot, yrot, zrot;
	
	xrot = Matrix3x3(	1,			0,					0,
						0,	cos(euler_angles.x),	-sin(euler_angles.x),
						0,	sin(euler_angles.x),	cos(euler_angles.x));
	
	yrot = Matrix3x3(	cos(euler_angles.y),	0,	sin(euler_angles.y),
								0,				1,				0,
						-sin(euler_angles.y),	0,	cos(euler_angles.y));
	
	zrot = Matrix3x3(	cos(euler_angles.z),	-sin(euler_angles.z),	0,
						sin(euler_angles.z),	cos(euler_angles.z),	0,
								0,						0,				1);
	
	*this = Matrix4x4(xrot * yrot * zrot);
}

void Matrix4x4::rotate(const Vector3 &axis, scalar_t angle)
{
	scalar_t sina = (scalar_t)sin(angle);
	scalar_t cosa = (scalar_t)cos(angle);
	scalar_t invcosa = 1-cosa;
	scalar_t nxsq = axis.x * axis.x;
	scalar_t nysq = axis.y * axis.y;
	scalar_t nzsq = axis.z * axis.z;

	Matrix3x3 xform;
	xform[0][0] = nxsq + (1-nxsq) * cosa;
	xform[0][1] = axis.x * axis.y * invcosa - axis.z * sina;
	xform[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	xform[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	xform[1][1] = nysq + (1-nysq) * cosa;
	xform[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	xform[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	xform[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	xform[2][2] = nzsq + (1-nzsq) * cosa;

	*this *= Matrix4x4(xform);
}

void Matrix4x4::set_rotation(const Vector3 &axis, scalar_t angle)
{
	scalar_t sina = (scalar_t)sin(angle);
	scalar_t cosa = (scalar_t)cos(angle);
	scalar_t invcosa = 1-cosa;
	scalar_t nxsq = axis.x * axis.x;
	scalar_t nysq = axis.y * axis.y;
	scalar_t nzsq = axis.z * axis.z;

	reset_identity();
	m[0][0] = nxsq + (1-nxsq) * cosa;
	m[0][1] = axis.x * axis.y * invcosa - axis.z * sina;
	m[0][2] = axis.x * axis.z * invcosa + axis.y * sina;
	m[1][0] = axis.x * axis.y * invcosa + axis.z * sina;
	m[1][1] = nysq + (1-nysq) * cosa;
	m[1][2] = axis.y * axis.z * invcosa - axis.x * sina;
	m[2][0] = axis.x * axis.z * invcosa - axis.y * sina;
	m[2][1] = axis.y * axis.z * invcosa + axis.x * sina;
	m[2][2] = nzsq + (1-nzsq) * cosa;
}

void Matrix4x4::scale(const Vector4 &scale_vec)
{
	Matrix4x4 smat(	scale_vec.x, 0, 0, 0,
					0, scale_vec.y, 0, 0,
					0, 0, scale_vec.z, 0,
					0, 0, 0, scale_vec.w);
	*this *= smat;
}

void Matrix4x4::set_scaling(const Vector4 &scale_vec)
{
	*this = Matrix4x4(	scale_vec.x, 0, 0, 0,
						0, scale_vec.y, 0, 0,
						0, 0, scale_vec.z, 0,
						0, 0, 0, scale_vec.w);
}

void Matrix4x4::set_column_vector(const Vector4 &vec, unsigned int col_index)
{
	m[0][col_index] = vec.x;
	m[1][col_index] = vec.y;
	m[2][col_index] = vec.z;
	m[3][col_index] = vec.w;
}

void Matrix4x4::set_row_vector(const Vector4 &vec, unsigned int row_index)
{
	m[row_index][0] = vec.x;
	m[row_index][1] = vec.y;
	m[row_index][2] = vec.z;
	m[row_index][3] = vec.w;
}

Vector4 Matrix4x4::get_column_vector(unsigned int col_index) const
{
	return Vector4(m[0][col_index], m[1][col_index], m[2][col_index], m[3][col_index]);
}

Vector4 Matrix4x4::get_row_vector(unsigned int row_index) const
{
	return Vector4(m[row_index][0], m[row_index][1], m[row_index][2], m[row_index][3]);
}

void Matrix4x4::transpose()
{
	Matrix4x4 tmp = *this;
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			m[i][j] = tmp[j][i];
		}
	}
}

Matrix4x4 Matrix4x4::transposed() const
{
	Matrix4x4 res;
	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			res[i][j] = m[j][i];
		}
	}
	return res;
}

scalar_t Matrix4x4::determinant() const
{
	scalar_t det11 =	(m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
						(m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
						(m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));

	scalar_t det12 =	(m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
						(m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
						(m[1][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));

	scalar_t det13 =	(m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
						(m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
						(m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	scalar_t det14 =	(m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
						(m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
						(m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	return m[0][0] * det11 - m[0][1] * det12 + m[0][2] * det13 - m[0][3] * det14;
}


Matrix4x4 Matrix4x4::adjoint() const
{
	Matrix4x4 coef;

	coef.m[0][0] =	(m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	coef.m[0][1] =	(m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
	coef.m[0][2] =	(m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
	coef.m[0][3] =	(m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	coef.m[1][0] =	(m[0][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[0][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[0][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	coef.m[1][1] =	(m[0][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[0][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[0][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
	coef.m[1][2] =	(m[0][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[0][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[0][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
	coef.m[1][3] =	(m[0][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[0][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[0][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	coef.m[2][0] =	(m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3])) -
					(m[0][2] * (m[1][1] * m[3][3] - m[3][1] * m[1][3])) +
					(m[0][3] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]));
	coef.m[2][1] =	(m[0][0] * (m[1][2] * m[3][3] - m[3][2] * m[1][3])) -
					(m[0][2] * (m[1][0] * m[3][3] - m[3][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[3][2] - m[3][0] * m[1][2]));
	coef.m[2][2] =	(m[0][0] * (m[1][1] * m[3][3] - m[3][1] * m[1][3])) -
					(m[0][1] * (m[1][0] * m[3][3] - m[3][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]));
	coef.m[2][3] =	(m[0][0] * (m[1][1] * m[3][2] - m[3][1] * m[1][2])) -
					(m[0][1] * (m[1][0] * m[3][2] - m[3][0] * m[1][2])) +
					(m[0][2] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]));

	coef.m[3][0] =	(m[0][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3])) -
					(m[0][2] * (m[1][1] * m[2][3] - m[2][1] * m[1][3])) +
					(m[0][3] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]));
	coef.m[3][1] =	(m[0][0] * (m[1][2] * m[2][3] - m[2][2] * m[1][3])) -
					(m[0][2] * (m[1][0] * m[2][3] - m[2][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]));
	coef.m[3][2] =	(m[0][0] * (m[1][1] * m[2][3] - m[2][1] * m[1][3])) -
					(m[0][1] * (m[1][0] * m[2][3] - m[2][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));
	coef.m[3][3] =	(m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])) -
					(m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2])) +
					(m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));

	coef.transpose();

	for(int i=0; i<4; i++) {
		for(int j=0; j<4; j++) {
			coef.m[i][j] = j%2 ? -coef.m[i][j] : coef.m[i][j];
			if(i%2) coef.m[i][j] = -coef.m[i][j];
		}
	}

	return coef;
}

Matrix4x4 Matrix4x4::inverse() const
{
	Matrix4x4 AdjMat = adjoint();

	return AdjMat * (1.0f / determinant());
}

ostream &operator <<(ostream &out, const Matrix4x4 &mat)
{
	for(int i=0; i<4; i++) {
		char str[100];
		sprintf(str, "[ %12.5f %12.5f %12.5f %12.5f ]\n", (float)mat.m[i][0], (float)mat.m[i][1], (float)mat.m[i][2], (float)mat.m[i][3]);
		out << str;
	}
	return out;
}
