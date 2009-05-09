#include "vector.h"
#include "sphvec.h"

// ---------- Vector2 -----------

Vector2::Vector2(scalar_t x, scalar_t y)
{
	this->x = x;
	this->y = y;
}

Vector2::Vector2(const vec2_t &vec)
{
	x = vec.x;
	y = vec.y;
}

Vector2::Vector2(const Vector3 &vec)
{
	x = vec.x;
	y = vec.y;
}

Vector2::Vector2(const Vector4 &vec)
{
	x = vec.x;
	y = vec.y;
}

void Vector2::normalize()
{
	scalar_t len = length();
	x /= len;
	y /= len;
}

Vector2 Vector2::normalized() const
{	
	scalar_t len = length();
	return Vector2(x / len, y / len);
}

void Vector2::transform(const Matrix3x3 &mat)
{
	scalar_t nx = mat[0][0] * x + mat[0][1] * y + mat[0][2];
	y = mat[1][0] * x + mat[1][1] * y + mat[1][2];
	x = nx;
}

Vector2 Vector2::transformed(const Matrix3x3 &mat) const
{
	Vector2 vec;
	vec.x = mat[0][0] * x + mat[0][1] * y + mat[0][2];
	vec.y = mat[1][0] * x + mat[1][1] * y + mat[1][2];
	return vec;
}

void Vector2::rotate(scalar_t angle)
{
	*this = Vector2(cos(angle) * x - sin(angle) * y, sin(angle) * x + cos(angle) * y);
}

Vector2 Vector2::rotated(scalar_t angle) const
{
	return Vector2(cos(angle) * x - sin(angle) * y, sin(angle) * x + cos(angle) * y);
}
	
Vector2 Vector2::reflection(const Vector2 &normal) const
{
	return 2.0 * dot_product(*this, normal) * normal - *this;
}

Vector2 Vector2::refraction(const Vector2 &normal, scalar_t src_ior, scalar_t dst_ior) const
{
	// quick and dirty implementation :)
	Vector3 v3refr = Vector3(this->x, this->y, 1.0).refraction(Vector3(this->x, this->y, 1), src_ior, dst_ior);
	return Vector2(v3refr.x, v3refr.y);
}

std::ostream &operator <<(std::ostream &out, const Vector2 &vec)
{
	out << "[" << vec.x << " " << vec.y << "]";
	return out;
}



// --------- Vector3 ----------

Vector3::Vector3(scalar_t x, scalar_t y, scalar_t z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3(const vec3_t &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

Vector3::Vector3(const Vector2 &vec)
{
	x = vec.x;
	y = vec.y;
	z = 1;
}

Vector3::Vector3(const Vector4 &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

Vector3::Vector3(const SphVector &sph)
{
	*this = sph;
}

Vector3 &Vector3::operator =(const SphVector &sph)
{
	x = sph.r * cos(sph.theta) * sin(sph.phi);
	z = sph.r * sin(sph.theta) * sin(sph.phi);
	y = sph.r * cos(sph.phi);
	return *this;
}

void Vector3::normalize()
{
	scalar_t len = length();
	x /= len;
	y /= len;
	z /= len;	
}

Vector3 Vector3::normalized() const
{
	scalar_t len = length();
	return Vector3(x / len, y / len, z / len);
}

Vector3 Vector3::reflection(const Vector3 &normal) const
{
	return -(2.0 * dot_product(*this, normal) * normal - *this);
}

Vector3 Vector3::refraction(const Vector3 &normal, scalar_t src_ior, scalar_t dst_ior) const
{
	scalar_t cos_inc = dot_product(*this, -normal);
	scalar_t ior = src_ior / dst_ior;

	scalar_t radical = 1.0 + SQ(ior) * (SQ(cos_inc) - 1.0);

	if(radical < 0.0) {		// total internal reflection
		return reflection(normal);
	}

	scalar_t beta = ior * cos_inc - sqrt(radical);

	return *this * ior + normal * beta;
}

void Vector3::transform(const Matrix3x3 &mat)
{
	scalar_t nx = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z;
	scalar_t ny = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z;
	z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z;
	x = nx;
	y = ny;
}

Vector3 Vector3::transformed(const Matrix3x3 &mat) const
{
	Vector3 vec;
	vec.x = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z;
	vec.y = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z;
	vec.z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z;
	return vec;
}

void Vector3::transform(const Matrix4x4 &mat)
{
	scalar_t nx = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z + mat[0][3];
	scalar_t ny = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z + mat[1][3];
	z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z + mat[2][3];
	x = nx;
	y = ny;
}

Vector3 Vector3::transformed(const Matrix4x4 &mat) const
{
	Vector3 vec;
	vec.x = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z + mat[0][3];
	vec.y = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z + mat[1][3];
	vec.z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z + mat[2][3];
	return vec;
}

void Vector3::transform(const Quaternion &quat)
{
	Quaternion vq(0.0f, *this);
	vq = quat * vq * quat.inverse();
	*this = vq.v;
}

Vector3 Vector3::transformed(const Quaternion &quat) const
{
	Quaternion vq(0.0f, *this);
	vq = quat * vq * quat.inverse();
	return vq.v;
}

void Vector3::rotate(const Vector3 &euler)
{
	Matrix4x4 rot;
	rot.set_rotation(euler);
	transform(rot);
}

Vector3 Vector3::rotated(const Vector3 &euler) const
{
	Matrix4x4 rot;
	rot.set_rotation(euler);
	return transformed(rot);
}


std::ostream &operator <<(std::ostream &out, const Vector3 &vec)
{
	out << "[" << vec.x << " " << vec.y << " " << vec.z << "]";
	return out;
}



// -------------- Vector4 --------------
Vector4::Vector4(scalar_t x, scalar_t y, scalar_t z, scalar_t w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Vector4::Vector4(const vec4_t &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = vec.w;
}

Vector4::Vector4(const Vector2 &vec)
{
	x = vec.x;
	y = vec.y;
	z = 1;
	w = 1;
}

Vector4::Vector4(const Vector3 &vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
	w = 1;
}

void Vector4::normalize()
{
	scalar_t len = (scalar_t)sqrt(x*x + y*y + z*z + w*w);
	x /= len;
	y /= len;
	z /= len;
	w /= len;
}

Vector4 Vector4::normalized() const
{
	scalar_t len = (scalar_t)sqrt(x*x + y*y + z*z + w*w);
	return Vector4(x / len, y / len, z / len, w / len);
}

void Vector4::transform(const Matrix4x4 &mat)
{
	scalar_t nx = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z + mat[0][3] * w;
	scalar_t ny = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z + mat[1][3] * w;
	scalar_t nz = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z + mat[2][3] * w;
	w = mat[3][0] * x + mat[3][1] * y + mat[3][2] * z + mat[3][3] * w;
	x = nx;
	y = ny;
	z = nz;
}

Vector4 Vector4::transformed(const Matrix4x4 &mat) const
{
	Vector4 vec;
	vec.x = mat[0][0] * x + mat[0][1] * y + mat[0][2] * z + mat[0][3] * w;
	vec.y = mat[1][0] * x + mat[1][1] * y + mat[1][2] * z + mat[1][3] * w;
	vec.z = mat[2][0] * x + mat[2][1] * y + mat[2][2] * z + mat[2][3] * w;
	vec.w = mat[3][0] * x + mat[3][1] * y + mat[3][2] * z + mat[3][3] * w;
	return vec;
}

// TODO: implement 4D vector reflection
Vector4 Vector4::reflection(const Vector4 &normal) const
{
	return *this;
}

// TODO: implement 4D vector refraction
Vector4 Vector4::refraction(const Vector4 &normal, scalar_t src_ior, scalar_t dst_ior) const
{
	return *this;
}

std::ostream &operator <<(std::ostream &out, const Vector4 &vec)
{
	out << "[" << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << "]";
	return out;
}
