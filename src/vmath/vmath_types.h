#ifndef VMATH_TYPES_H_
#define VMATH_TYPES_H_

#include "vmath_config.h"

#ifdef SINGLE_PRECISION_MATH
typedef float scalar_t;
#else
typedef double scalar_t;
#endif	/* floating point precision */

/* vectors */
typedef struct { scalar_t x, y; } vec2_t;
typedef struct { scalar_t x, y, z; } vec3_t;
typedef struct { scalar_t x, y, z, w; } vec4_t;

/* quaternions */
typedef vec4_t quat_t;

/* matrices */
typedef scalar_t mat3_t[3][3];
typedef scalar_t mat4_t[4][4];


#ifdef __cplusplus
class Vector2;
class Vector3;
class Vector4;
class Quaternion;
class Matrix3x3;
class Matrix4x4;
class SphVector;
#endif	/* __cplusplus */

#endif	/* VMATH_TYPES_H_ */
