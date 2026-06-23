#ifndef _MATH_Matrixx_H
#define _MATH_Matrixx_H

#pragma once


class Vector3x;
class Quaternionx;


/// Implements 4x4 Matrixx (OpenGL compatible)
/// Stores data in column major mode: [column][row]
///
/// Matrixx in mem:   [0][x] returns:
///			
/// | 0 4  8 12 |    | 1 |
/// | 1 5  9 13 |    | 2 |
/// | 2 6 10 14 |    | 3 |
/// | 3 7 11 15 |    | 4 |
///
/// Translation:  Rotation X:        Rotation Y:        Rotation Z:
///
/// | 1 0 0 x |   | 1   0    0 0 |   | cos  0 sin 0 |   | cos -sin 0 0 |
/// | 0 1 0 y |   | 0 cos -sin 0 |   | 0    1 0   0 |   | sin  cos 0 0 |
/// | 0 0 1 z |   | 0 sin  cos 0 |   | -sin 0 cos 0 |   | 0 0      1 0 |
/// | 0 0 0 1 |   | 0   0    0 1 |   | 0    0 0   1 |   | 0 0      0 1 |
///
class Matrixx
{
public:
	float m[4][4];

	inline void			Set(const Vector3x &rkTranslation, const Quaternionx &rkRotation);

	inline void			SetTranslation(const Vector3x &rkTranslation);

	inline Vector3x			GetTranslation() const;

	inline Quaternionx		GetRotation() const;

	inline void			SetScaling(const Vector3x &rkScaling);

	inline const Matrixx&		Transpose();

	inline void			TransposeTo(Matrixx *pkMatrixx) const;

	inline void			MakeIdentity();

	inline const Matrixx&		Inverse();

	inline float			Determinant();

	inline float*			operator[](unsigned int uiColumn);

	inline const float*		operator[](unsigned int uiColumn) const;

	inline Matrixx			operator+ (const Matrixx &rkMatrixx) const;

	inline Vector3x			operator* (const Vector3x &rkVector) const;

	inline Matrixx			operator* (const Matrixx &rkMatrixx) const;

	inline const Matrixx&		operator*= (float fScale);

	inline Matrixx			operator* (float fScale) const;

	inline const Matrixx&		operator= (const Matrixx &rkOther);

	/// Construct Matrixx as ident
	inline				Matrixx();

	/// Construct rotation Matrixx from Quaternionx
	inline				Matrixx(const Quaternionx &rkRotation);

	inline				Matrixx(const Vector3x &rkVector);

	inline				Matrixx(const Vector3x &rkTranslation, const Quaternionx &rkRotation);
};



#include "Matrix.inl"

#endif // _MATH_Matrixx_H
