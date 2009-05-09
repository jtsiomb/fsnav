#ifndef VMATH_BASIS_H_
#define VMATH_BASIS_H_

#include "vector.h"
#include "matrix.h"

enum {
	LEFT_HANDED,
	RIGHT_HANDED
};

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

void basis_matrix(mat4_t res, vec3_t i, vec3_t j, vec3_t k);
void basis_matrix_dir(mat4_t res, vec3_t dir);

#ifdef __cplusplus
}	/* extern "C" */

class Basis {
public:
	Vector3 i, j, k;

	Basis();
	Basis(const Vector3 &i, const Vector3 &j, const Vector3 &k);
	Basis(const Vector3 &dir, bool left_handed = true);

	void rotate(scalar_t x, scalar_t y, scalar_t z);
	void rotate(const Vector3 &axis, scalar_t angle);
	void rotate(const Matrix4x4 &mat);
	void rotate(const Quaternion &quat);

	Matrix3x3 create_rotation_matrix() const;
};
#endif	/* __cplusplus */

#endif	/* VMATH_BASIS_H_ */
