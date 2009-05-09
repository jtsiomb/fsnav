#ifndef VMATH_CONFIG_H_
#define VMATH_CONFIG_H_

#if (__STDC_VERSION__ < 199999)
#if defined(__GNUC__) || defined(_MSC_VER)
#define inline __inline
#else
#define inline

#ifdef VECTOR_H_
#warning "compiling vector operations without inline, performance might suffer"
#endif	/* VECTOR_H_ */

#endif	/* gcc/msvc */
#endif	/* not C99 */

#define SINGLE_PRECISION_MATH

#endif	/* VMATH_CONFIG_H_ */
