#ifndef _MATH_VECTOR_H
#define _MATH_VECTOR_H
#pragma once

#include <assert.h>
#include <math.h>

//
class Vector3x
{
public:
	enum VECTOR_AXIS {
		X = 0,
		Y = 1,
		Z = 2
	};

	float	x, y, z;

	inline float		Length() const;

	inline void		Normalize();

	inline float&		operator[](int i) {
					assert((i >= 0) && (i <= 2));
					return (&x)[i]; 
				};

	inline const float&	operator[](int i) const {
					assert((i >= 0) && (i <= 2));
					return (&x)[i]; 
				};

	inline Vector3x		operator- (const Vector3x& v) const {
					Vector3x res;
					res.x = this->x - v.x;
					res.y = this->y - v.y;
					res.z = this->z - v.z;
					return res;
				};

	inline Vector3x		operator- () const {
					return Vector3x(-x, -y, -z);
				};

	inline Vector3x		operator+ (const Vector3x& v) const {
					Vector3x res;
					res.x = this->x + v.x;
					res.y = this->y + v.y;
					res.z = this->z + v.z;
					return res;
				};

	inline Vector3x&	operator+= (const Vector3x& rkOther);

	inline Vector3x&	operator-= (const Vector3x& rkOther);

	inline Vector3x		operator* (const float f) const {
					Vector3x res;
					res.x = this->x * f;
					res.y = this->y * f;
					res.z = this->z * f;
					return res;
				};

	inline void		operator*= (const float f);

	/// Dot product
	inline float		operator* (const Vector3x& kOther) const;

	inline bool		operator== (const Vector3x& v) const {
					return ((this->x == v.x) && (this->y == v.y) && (this->z == v.z));
				};

	inline bool		operator!= (const Vector3x& v) const {
					return ((this->x != v.x) || (this->y != v.y) || (this->z != v.z));
				};

	inline Vector3x		operator= (const Vector3x& v) {
					x = v.x;
					y = v.y;
					z = v.z;
					return *this;
				};

	/// Cross product
	inline Vector3x		operator% (const Vector3x& kOther) const;

	inline Vector3x&		Set(float fX, float fY, float fZ) { x = fX; y = fY; z = fZ; return *this; };

	inline Vector3x&		Clear();

	inline				Vector3x(): x(0.0f), y(0.0f), z(0.0f) {};

	inline				Vector3x(float fX, float fY, float fZ) { x = fX; y = fY; z = fZ; };

	inline				Vector3x(const float afValues[]);
};



#include "Vector3f.inl"

#endif // _MATH_VECTOR_H
