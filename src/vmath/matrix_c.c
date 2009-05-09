#include <stdio.h>
#include "matrix.h"
#include "vector.h"

void m3_to_m4(mat4_t dest, mat3_t src)
{
	int i, j;
	
	memset(dest, 0, sizeof(mat4_t));
	for(i=0; i<3; i++) {
		for(j=0; j<3; j++) {
			dest[i][j] = src[i][j];
		}
	}
	dest[3][3] = 1.0;
}

void m3_print(FILE *fp, mat3_t m)
{
	int i;
	for(i=0; i<3; i++) {
		fprintf(fp, "[ %12.5f %12.5f %12.5f ]\n", (float)m[i][0], (float)m[i][1], (float)m[i][2]);
	}
}

/* C matrix 4x4 functions */
void m4_to_m3(mat3_t dest, mat4_t src)
{
	int i, j;
	for(i=0; i<3; i++) {
		for(j=0; j<3; j++) {
			dest[i][j] = src[i][j];
		}
	}
}

void m4_translate(mat4_t m, scalar_t x, scalar_t y, scalar_t z)
{
	mat4_t tm;
	m4_identity(tm);
	tm[0][3] = x;
	tm[1][3] = y;
	tm[2][3] = z;
	m4_mult(m, m, tm);
}

void m4_rotate(mat4_t m, scalar_t x, scalar_t y, scalar_t z)
{
	m4_rotate_x(m, x);
	m4_rotate_y(m, y);
	m4_rotate_z(m, z);
}

void m4_rotate_x(mat4_t m, scalar_t angle)
{
	mat4_t rm;
	m4_identity(rm);
	rm[1][1] = cos(angle); rm[1][2] = -sin(angle);
	rm[2][1] = sin(angle); rm[2][2] = cos(angle);
	m4_mult(m, m, rm);
}

void m4_rotate_y(mat4_t m, scalar_t angle)
{
	mat4_t rm;
	m4_identity(rm);
	rm[0][0] = cos(angle); rm[0][2] = sin(angle);
	rm[2][0] = -sin(angle); rm[2][2] = cos(angle);
	m4_mult(m, m, rm);
}

void m4_rotate_z(mat4_t m, scalar_t angle)
{
	mat4_t rm;
	m4_identity(rm);
	rm[0][0] = cos(angle); rm[0][1] = -sin(angle);
	rm[1][0] = sin(angle); rm[1][1] = cos(angle);
	m4_mult(m, m, rm);
}

void m4_rotate_axis(mat4_t m, scalar_t angle, scalar_t x, scalar_t y, scalar_t z)
{
	mat4_t xform;
	scalar_t sina = sin(angle);
	scalar_t cosa = cos(angle);
	scalar_t one_minus_cosa = 1.0 - cosa;
	scalar_t nxsq = x * x;
	scalar_t nysq = y * y;
	scalar_t nzsq = z * z;

	m4_identity(xform);
	xform[0][0] = nxsq + (1.0 - nxsq) * cosa;
	xform[0][1] = x * y * one_minus_cosa - z * sina;
	xform[0][2] = x * z * one_minus_cosa + y * sina;
	xform[1][0] = x * y * one_minus_cosa + z * sina;
	xform[1][1] = nysq + (1.0 - nysq) * cosa;
	xform[1][2] = y * z * one_minus_cosa - x * sina;
	xform[2][0] = x * z * one_minus_cosa - y * sina;
	xform[2][1] = y * z * one_minus_cosa + x * sina;
	xform[2][2] = nzsq + (1.0 - nzsq) * cosa;

	m4_mult(m, m, xform);
}

void m4_rotate_quat(mat4_t m, quat_t q)
{
	mat4_t rm;
	quat_to_mat4(rm, q);
	m4_mult(m, m, rm);
}

void m4_scale(mat4_t m, scalar_t x, scalar_t y, scalar_t z)
{
	mat4_t sm;
	m4_identity(sm);
	sm[0][0] = x;
	sm[1][1] = y;
	sm[2][2] = z;
	m4_mult(m, m, sm);
}

void m4_transpose(mat4_t res, mat4_t m)
{
	int i, j;
	mat4_t tmp;
	m4_copy(tmp, m);

	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			res[i][j] = tmp[j][i];
		}
	}
}

scalar_t m4_determinant(mat4_t m)
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

void m4_adjoint(mat4_t res, mat4_t m)
{
	int i, j;
	mat4_t coef;

	coef[0][0] =	(m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[1][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	coef[0][1] =	(m[1][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[1][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
	coef[0][2] =	(m[1][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[1][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[1][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
	coef[0][3] =	(m[1][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[1][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[1][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	coef[1][0] =	(m[0][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[0][2] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) +
					(m[0][3] * (m[2][1] * m[3][2] - m[3][1] * m[2][2]));
	coef[1][1] =	(m[0][0] * (m[2][2] * m[3][3] - m[3][2] * m[2][3])) -
					(m[0][2] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[0][3] * (m[2][0] * m[3][2] - m[3][0] * m[2][2]));
	coef[1][2] =	(m[0][0] * (m[2][1] * m[3][3] - m[3][1] * m[2][3])) -
					(m[0][1] * (m[2][0] * m[3][3] - m[3][0] * m[2][3])) +
					(m[0][3] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));
	coef[1][3] =	(m[0][0] * (m[2][1] * m[3][2] - m[3][1] * m[2][2])) -
					(m[0][1] * (m[2][0] * m[3][2] - m[3][0] * m[2][2])) +
					(m[0][2] * (m[2][0] * m[3][1] - m[3][0] * m[2][1]));

	coef[2][0] =	(m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3])) -
					(m[0][2] * (m[1][1] * m[3][3] - m[3][1] * m[1][3])) +
					(m[0][3] * (m[1][1] * m[3][2] - m[3][1] * m[1][2]));
	coef[2][1] =	(m[0][0] * (m[1][2] * m[3][3] - m[3][2] * m[1][3])) -
					(m[0][2] * (m[1][0] * m[3][3] - m[3][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[3][2] - m[3][0] * m[1][2]));
	coef[2][2] =	(m[0][0] * (m[1][1] * m[3][3] - m[3][1] * m[1][3])) -
					(m[0][1] * (m[1][0] * m[3][3] - m[3][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]));
	coef[2][3] =	(m[0][0] * (m[1][1] * m[3][2] - m[3][1] * m[1][2])) -
					(m[0][1] * (m[1][0] * m[3][2] - m[3][0] * m[1][2])) +
					(m[0][2] * (m[1][0] * m[3][1] - m[3][0] * m[1][1]));

	coef[3][0] =	(m[0][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3])) -
					(m[0][2] * (m[1][1] * m[2][3] - m[2][1] * m[1][3])) +
					(m[0][3] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]));
	coef[3][1] =	(m[0][0] * (m[1][2] * m[2][3] - m[2][2] * m[1][3])) -
					(m[0][2] * (m[1][0] * m[2][3] - m[2][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[2][2] - m[2][0] * m[1][2]));
	coef[3][2] =	(m[0][0] * (m[1][1] * m[2][3] - m[2][1] * m[1][3])) -
					(m[0][1] * (m[1][0] * m[2][3] - m[2][0] * m[1][3])) +
					(m[0][3] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));
	coef[3][3] =	(m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])) -
					(m[0][1] * (m[1][0] * m[2][2] - m[2][0] * m[1][2])) +
					(m[0][2] * (m[1][0] * m[2][1] - m[2][0] * m[1][1]));

	m4_transpose(res, coef);

	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			res[i][j] = j % 2 ? -res[i][j] : res[i][j];
			if(i % 2) res[i][j] = -res[i][j];
		}
	}
}

void m4_inverse(mat4_t res, mat4_t m)
{
	int i, j;
	mat4_t adj;
	scalar_t det;

	m4_adjoint(adj, m);
	det = m4_determinant(m);
	
	for(i=0; i<4; i++) {
		for(j=0; j<4; j++) {
			res[i][j] = adj[i][j] / det;
		}
	}
}

void m4_print(FILE *fp, mat4_t m)
{
	int i;
	for(i=0; i<4; i++) {
		fprintf(fp, "[ %12.5f %12.5f %12.5f %12.5f ]\n", (float)m[i][0], (float)m[i][1], (float)m[i][2], (float)m[i][3]);
	}
}
