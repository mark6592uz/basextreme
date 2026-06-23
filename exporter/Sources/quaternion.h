#ifndef _MATH_Quaternionx_H
#define _MATH_Quaternionx_H
#pragma once


#include <assert.h>
#include <math.h>
#include "constants.h"

class Matrixx;
class Vector3x;

class Quaternionx
{
public:
	enum Quaternionx_AXIS {
		X = 0,
		Y = 1,
		Z = 2,
		W = 3
	};

	float x, y, z, w;

	inline void		Set(float fX, float fY, float fZ, float fW);

	inline void		ToMatrixx(Matrixx *pkMatrixx) const;

	inline Quaternionx&	FromEulerAngles(float fAngleX, float fAngleY, float fAngleZ);

	inline Quaternionx&	Normalize();
	
	inline Quaternionx&	Inverse();

	inline float&		operator[](const int i);

	inline Quaternionx	operator+(const Quaternionx &kOther) const;

	inline Quaternionx&	operator*=(const Quaternionx &kOther);

	inline Quaternionx	operator*(const Quaternionx &kOther) const;

	inline Quaternionx	operator*(const float fScale) const;

	inline Vector3x		operator*(const Vector3x &rkVector) const;

	inline Quaternionx&	operator=(const Matrixx &rkMatrixx);

	inline				Quaternionx(): x(0.0f), y(0.0f), z(0.0f), w(1.0f) {};

	inline				Quaternionx(float fX, float fY, float fZ, float fW): x(fX), y(fY), z(fZ), w(fW) {};

	inline				Quaternionx(float fX, float fY, float fZ);

	inline				Quaternionx::Quaternionx(const Matrixx &rkMatrixx) { *this = rkMatrixx; };
};


#include "Quaternion.inl"

#endif // _MATH_Quaternionx_H
