#include "basis.h"
#include "matrix.h"
#include "vector.h"

Basis::Basis()
{
	i = Vector3(1, 0, 0);
	j = Vector3(0, 1, 0);
	k = Vector3(0, 0, 1);
}

Basis::Basis(const Vector3 &i, const Vector3 &j, const Vector3 &k)
{
	this->i = i;
	this->j = j;
	this->k = k;
}

Basis::Basis(const Vector3 &dir, bool left_handed)
{
	k = dir;
	j = Vector3(0, 1, 0);
	i = cross_product(j, k);
	j = cross_product(k, i);
}

/** Rotate with euler angles */
void Basis::rotate(scalar_t x, scalar_t y, scalar_t z) {
	Matrix4x4 RotMat;
	RotMat.set_rotation(Vector3(x, y, z));
	i.transform(RotMat);
	j.transform(RotMat);
	k.transform(RotMat);
}

/** Rotate by axis-angle specification */
void Basis::rotate(const Vector3 &axis, scalar_t angle) {
	Quaternion q;
	q.set_rotation(axis, angle);
	i.transform(q);
	j.transform(q);
	k.transform(q);
}

/** Rotate with a 4x4 matrix */
void Basis::rotate(const Matrix4x4 &mat) {
	i.transform(mat);
	j.transform(mat);
	k.transform(mat);
}

/** Rotate with a quaternion */
void Basis::rotate(const Quaternion &quat) {
	i.transform(quat);
	j.transform(quat);
	k.transform(quat);
}

/** Extract a rotation matrix from the orthogonal basis */
Matrix3x3 Basis::create_rotation_matrix() const {
	return Matrix3x3(	i.x, j.x, k.x,
						i.y, j.y, k.y,
						i.z, j.z, k.z);
}
