#ifndef VMATH_SPHVEC_H_
#define VMATH_SPHVEC_H_

#include "vmath_types.h"

#ifdef __cplusplus
/* Vector in spherical coordinates */
class SphVector {
public:
	scalar_t theta, phi, r;

	SphVector(scalar_t theta = 0.0, scalar_t phi = 0.0, scalar_t r = 1.0);
	SphVector(const Vector3 &cvec);
	SphVector &operator =(const Vector3 &cvec);
};
#endif	/* __cplusplus */

#endif	/* VMATH_SPHVEC_H_ */
