#include "sphvec.h"
#include "vector.h"

/* theta: 0 <= theta <= 2pi, the angle around Y axis.
 * phi: 0 <= phi <= pi, the angle from Y axis.
 * r: radius.
 */
SphVector::SphVector(scalar_t theta, scalar_t phi, scalar_t r) {
	this->theta = theta;
	this->phi = phi;
	this->r = r;
}

/* Constructs a spherical coordinate vector from a cartesian vector */
SphVector::SphVector(const Vector3 &cvec) {
	*this = cvec;
}

/* Assignment operator that converts cartesian to spherical coords */
SphVector &SphVector::operator =(const Vector3 &cvec) {
	r = cvec.length();
	//theta = atan2(cvec.y, cvec.x);
	theta = atan2(cvec.z, cvec.x);
	//phi = acos(cvec.z / r);
	phi = acos(cvec.y / r);
	return *this;
}
