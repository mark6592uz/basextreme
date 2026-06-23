
#pragma once

#include "Vector3f.h"
#include "Quaternion.h"


inline void Matrixx::Set(const Vector3x &rkTranslation, const Quaternionx &rkRotation)
{
	rkRotation.ToMatrixx(this);
	m[3][0] = rkTranslation.x;
	m[3][1] = rkTranslation.y;
	m[3][2] = rkTranslation.z;
}


inline void Matrixx::SetTranslation(const Vector3x &rkTranslation)
{
	m[3][0] = rkTranslation.x;
	m[3][1] = rkTranslation.y;
	m[3][2] = rkTranslation.z;
}


inline Vector3x Matrixx::GetTranslation() const
{
	return Vector3x(m[3][0], m[3][1], m[3][2]);
}


inline Quaternionx Matrixx::GetRotation() const
{
	return Quaternionx(*this);
}


inline void Matrixx::SetScaling(const Vector3x &rkScaling)
{
	m[0][0] = rkScaling.x;
	m[1][1] = rkScaling.y;
	m[2][2] = rkScaling.z;
}


inline const Matrixx& Matrixx::Transpose()
{
	float fTemp;

	fTemp = m[0][1];
	m[0][1] = m[1][0];
	m[1][0] = fTemp;

	fTemp = m[0][2];
	m[0][2] = m[2][0];
	m[2][0] = fTemp;

	fTemp = m[0][3];
	m[0][3] = m[3][0];
	m[3][0] = fTemp;

	fTemp = m[1][2];
	m[1][2] = m[2][1];
	m[2][1] = fTemp;

	fTemp = m[1][3];
	m[1][3] = m[3][1];
	m[3][1] = fTemp;

	fTemp = m[2][3];
	m[2][3] = m[3][2];
	m[3][2] = fTemp;

	return *this;
}


inline void Matrixx::TransposeTo(Matrixx *pkMatrixx) const
{
	float *pfDst = &(pkMatrixx->m[0][0]);
	const float *pfSrc = &m[0][0];

	*pfDst++ = pfSrc[0];
	*pfDst++ = pfSrc[4];
	*pfDst++ = pfSrc[8];
	*pfDst++ = pfSrc[12];
	*pfDst++ = pfSrc[1];
	*pfDst++ = pfSrc[5];
	*pfDst++ = pfSrc[9];
	*pfDst++ = pfSrc[13];
	*pfDst++ = pfSrc[2];
	*pfDst++ = pfSrc[6];
	*pfDst++ = pfSrc[10];
	*pfDst++ = pfSrc[14];
	*pfDst++ = pfSrc[3];
	*pfDst++ = pfSrc[7];
	*pfDst++ = pfSrc[11];
	*pfDst   = pfSrc[15];
}


inline void Matrixx::MakeIdentity()
{
	m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
	m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
	m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
	m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
}

/*
inline void m4_submat(const Matrixx &kMat, float *mat, int uiCol, int uiRow) {
	int uiColD, uiRowD, uiColS, uiRowS;
	// loop through 3x3 subMatrixx
	for(uiColD = 3; uiColD--;) {
		for(uiRowD = 3; uiRowD--;) {
			// map 3x3 element (destination) to 4x4 element (source)
			uiColS = uiColD + ((uiColD >= uiCol) ? 1 : 0);
			uiRowS = uiRowD + ((uiRowD >= uiRow) ? 1 : 0);
			// copy element
			mat[uiColD * 3 + uiRowD] = kMat[uiColS][uiRowS];
		}
	}
}


inline float m3_det(float *mat)
{
	return    mat[0] * (mat[4] * mat[8] - mat[7] * mat[5])
		- mat[1] * (mat[3] * mat[8] - mat[6] * mat[5])
		+ mat[2] * (mat[3] * mat[7] - mat[6] * mat[4]);
}
*/


// TODO: optimise.. remove kResult
inline const Matrixx& Matrixx::Inverse()
{
	float fDet = this->Determinant();
	if (fabs(fDet) != 0.0f) {
		Matrixx kResult;

		kResult.m[0][0] = m[1][2]*m[2][3]*m[3][1] - m[1][3]*m[2][2]*m[3][1] + m[1][3]*m[2][1]*m[3][2] - m[1][1]*m[2][3]*m[3][2] - m[1][2]*m[2][1]*m[3][3] + m[1][1]*m[2][2]*m[3][3];
		kResult.m[0][1] = m[0][3]*m[2][2]*m[3][1] - m[0][2]*m[2][3]*m[3][1] - m[0][3]*m[2][1]*m[3][2] + m[0][1]*m[2][3]*m[3][2] + m[0][2]*m[2][1]*m[3][3] - m[0][1]*m[2][2]*m[3][3];
		kResult.m[0][2] = m[0][2]*m[1][3]*m[3][1] - m[0][3]*m[1][2]*m[3][1] + m[0][3]*m[1][1]*m[3][2] - m[0][1]*m[1][3]*m[3][2] - m[0][2]*m[1][1]*m[3][3] + m[0][1]*m[1][2]*m[3][3];
		kResult.m[0][3] = m[0][3]*m[1][2]*m[2][1] - m[0][2]*m[1][3]*m[2][1] - m[0][3]*m[1][1]*m[2][2] + m[0][1]*m[1][3]*m[2][2] + m[0][2]*m[1][1]*m[2][3] - m[0][1]*m[1][2]*m[2][3];
		kResult.m[1][0] = m[1][3]*m[2][2]*m[3][0] - m[1][2]*m[2][3]*m[3][0] - m[1][3]*m[2][0]*m[3][2] + m[1][0]*m[2][3]*m[3][2] + m[1][2]*m[2][0]*m[3][3] - m[1][0]*m[2][2]*m[3][3];
		kResult.m[1][1] = m[0][2]*m[2][3]*m[3][0] - m[0][3]*m[2][2]*m[3][0] + m[0][3]*m[2][0]*m[3][2] - m[0][0]*m[2][3]*m[3][2] - m[0][2]*m[2][0]*m[3][3] + m[0][0]*m[2][2]*m[3][3];
		kResult.m[1][2] = m[0][3]*m[1][2]*m[3][0] - m[0][2]*m[1][3]*m[3][0] - m[0][3]*m[1][0]*m[3][2] + m[0][0]*m[1][3]*m[3][2] + m[0][2]*m[1][0]*m[3][3] - m[0][0]*m[1][2]*m[3][3];
		kResult.m[1][3] = m[0][2]*m[1][3]*m[2][0] - m[0][3]*m[1][2]*m[2][0] + m[0][3]*m[1][0]*m[2][2] - m[0][0]*m[1][3]*m[2][2] - m[0][2]*m[1][0]*m[2][3] + m[0][0]*m[1][2]*m[2][3];
		kResult.m[2][0] = m[1][1]*m[2][3]*m[3][0] - m[1][3]*m[2][1]*m[3][0] + m[1][3]*m[2][0]*m[3][1] - m[1][0]*m[2][3]*m[3][1] - m[1][1]*m[2][0]*m[3][3] + m[1][0]*m[2][1]*m[3][3];
		kResult.m[2][1] = m[0][3]*m[2][1]*m[3][0] - m[0][1]*m[2][3]*m[3][0] - m[0][3]*m[2][0]*m[3][1] + m[0][0]*m[2][3]*m[3][1] + m[0][1]*m[2][0]*m[3][3] - m[0][0]*m[2][1]*m[3][3];
		kResult.m[2][2] = m[0][1]*m[1][3]*m[3][0] - m[0][3]*m[1][1]*m[3][0] + m[0][3]*m[1][0]*m[3][1] - m[0][0]*m[1][3]*m[3][1] - m[0][1]*m[1][0]*m[3][3] + m[0][0]*m[1][1]*m[3][3];
		kResult.m[2][3] = m[0][3]*m[1][1]*m[2][0] - m[0][1]*m[1][3]*m[2][0] - m[0][3]*m[1][0]*m[2][1] + m[0][0]*m[1][3]*m[2][1] + m[0][1]*m[1][0]*m[2][3] - m[0][0]*m[1][1]*m[2][3];
		kResult.m[3][0] = m[1][2]*m[2][1]*m[3][0] - m[1][1]*m[2][2]*m[3][0] - m[1][2]*m[2][0]*m[3][1] + m[1][0]*m[2][2]*m[3][1] + m[1][1]*m[2][0]*m[3][2] - m[1][0]*m[2][1]*m[3][2];
		kResult.m[3][1] = m[0][1]*m[2][2]*m[3][0] - m[0][2]*m[2][1]*m[3][0] + m[0][2]*m[2][0]*m[3][1] - m[0][0]*m[2][2]*m[3][1] - m[0][1]*m[2][0]*m[3][2] + m[0][0]*m[2][1]*m[3][2];
		kResult.m[3][2] = m[0][2]*m[1][1]*m[3][0] - m[0][1]*m[1][2]*m[3][0] - m[0][2]*m[1][0]*m[3][1] + m[0][0]*m[1][2]*m[3][1] + m[0][1]*m[1][0]*m[3][2] - m[0][0]*m[1][1]*m[3][2];
		kResult.m[3][3] = m[0][1]*m[1][2]*m[2][0] - m[0][2]*m[1][1]*m[2][0] + m[0][2]*m[1][0]*m[2][1] - m[0][0]*m[1][2]*m[2][1] - m[0][1]*m[1][0]*m[2][2] + m[0][0]*m[1][1]*m[2][2];

		kResult *= (1.0f / fDet);
		(*this) = kResult;
		return *this;
	}

	MakeIdentity();
	return *this;
}


inline float Matrixx::Determinant()
{
	return
	m[0][3] * m[1][2] * m[2][1] * m[3][0]-m[0][2] * m[1][3] * m[2][1] * m[3][0]-m[0][3] * m[1][1] * m[2][2] * m[3][0]+m[0][1] * m[1][3] * m[2][2] * m[3][0]+
	m[0][2] * m[1][1] * m[2][3] * m[3][0]-m[0][1] * m[1][2] * m[2][3] * m[3][0]-m[0][3] * m[1][2] * m[2][0] * m[3][1]+m[0][2] * m[1][3] * m[2][0] * m[3][1]+
	m[0][3] * m[1][0] * m[2][2] * m[3][1]-m[0][0] * m[1][3] * m[2][2] * m[3][1]-m[0][2] * m[1][0] * m[2][3] * m[3][1]+m[0][0] * m[1][2] * m[2][3] * m[3][1]+
	m[0][3] * m[1][1] * m[2][0] * m[3][2]-m[0][1] * m[1][3] * m[2][0] * m[3][2]-m[0][3] * m[1][0] * m[2][1] * m[3][2]+m[0][0] * m[1][3] * m[2][1] * m[3][2]+
	m[0][1] * m[1][0] * m[2][3] * m[3][2]-m[0][0] * m[1][1] * m[2][3] * m[3][2]-m[0][2] * m[1][1] * m[2][0] * m[3][3]+m[0][1] * m[1][2] * m[2][0] * m[3][3]+
	m[0][2] * m[1][0] * m[2][1] * m[3][3]-m[0][0] * m[1][2] * m[2][1] * m[3][3]-m[0][1] * m[1][0] * m[2][2] * m[3][3]+m[0][0] * m[1][1] * m[2][2] * m[3][3];
}


float* Matrixx::operator[](unsigned int uiColumn)
{
	return (float*)&m[uiColumn][0];
}


const float* Matrixx::operator[](unsigned int uiColumn) const
{
	return (float*)&m[uiColumn][0];
}


inline Matrixx Matrixx::operator+ (const Matrixx &rkMatrixx) const
{
	Matrixx kResult;

	unsigned int uiCol, uiRow;
	for(uiCol = 4; uiCol--;)
		for(uiRow = 4; uiRow--;)
			kResult.m[uiCol][uiRow] = this->m[uiCol][uiRow] * rkMatrixx.m[uiCol][uiRow];

	return kResult;
}


Vector3x Matrixx::operator *(const Vector3x &rkVector) const
{
	Vector3x kResult;

//	kResult.x = m[0][0] * rkVector.x + m[0][1] * rkVector.y + m[0][2] * rkVector.z + m[0][3];
//	kResult.y = m[1][0] * rkVector.x + m[1][1] * rkVector.y + m[1][2] * rkVector.z + m[1][3];
//	kResult.z = m[2][0] * rkVector.x + m[2][1] * rkVector.y + m[2][2] * rkVector.z + m[2][3];
	kResult.x = m[0][0] * rkVector.x + m[1][0] * rkVector.y + m[2][0] * rkVector.z + m[3][0];
	kResult.y = m[0][1] * rkVector.x + m[1][1] * rkVector.y + m[2][1] * rkVector.z + m[3][1];
	kResult.z = m[0][2] * rkVector.x + m[1][2] * rkVector.y + m[2][2] * rkVector.z + m[3][2];

	return kResult;
}


inline Matrixx Matrixx::operator* (const Matrixx &rkMatrixx) const
{
	Matrixx kResult;
	unsigned int uiRow, uiCol;

	for(uiCol = 4; uiCol--;) {
		for(uiRow = 4; uiRow--;) {
			kResult.m[uiCol][uiRow] =
				m[uiCol][0] * rkMatrixx.m[0][uiRow] +
				m[uiCol][1] * rkMatrixx.m[1][uiRow] +
				m[uiCol][2] * rkMatrixx.m[2][uiRow] +
				m[uiCol][3] * rkMatrixx.m[3][uiRow]
			;
		}
	}

	return kResult;
}


inline const Matrixx& Matrixx::operator*= (float fScale)
{
	unsigned int uiRow, uiCol;
	for (uiCol = 4; uiCol--;)
		for (uiRow = 4; uiRow--;)
			m[uiCol][uiRow] *= fScale;
	return *this;
}


inline Matrixx Matrixx::operator* (float fScale) const
{
	Matrixx kResult;
	unsigned int uiRow, uiCol;
	for (uiCol = 4; uiCol--;)
		for (uiRow = 4; uiRow--;)
			kResult.m[uiCol][uiRow] = m[uiCol][uiRow] * fScale;
	return kResult;
}


inline const Matrixx& Matrixx::operator= (const Matrixx &rkOther)
{
	unsigned int uiRow, uiCol;

	for(uiRow = 4; uiRow--;) {
		for(uiCol = 4; uiCol--;) {
			m[uiCol][uiRow] = rkOther.m[uiCol][uiRow];
		}
	}
        
	return *this;
}


inline Matrixx::Matrixx()
{
	MakeIdentity();
}


inline Matrixx::Matrixx(const Quaternionx &rkRotation)
{
	rkRotation.ToMatrixx(this);
	m[0][3] = 0;
	m[1][3] = 0;
	m[2][3] = 0;
	m[3][3] = 1;
	m[3][0] = 0;
	m[3][1] = 0;
	m[3][2] = 0;
}

inline Matrixx::Matrixx(const Vector3x &rkVector)
{
	m[0][0] = 0.0f;
	m[1][0] = -rkVector.z;
	m[2][0] = rkVector.y;
	m[3][0] = 0.0f;

	m[0][1] = rkVector.z;
	m[1][1] = 0.0f;
	m[2][1] = -rkVector.x;
	m[3][1] = 0.0f;

	m[0][2] = -rkVector.y;
	m[1][2] = rkVector.x;
	m[2][2] = 0.0f;
	m[3][2] = 0.0f;

	m[0][3] = 0.0f;
	m[1][3] = 0.0f;
	m[2][3] = 0.0f;
	m[3][3] = 1.0f;

/*
	m[0][0] = 1.0f;
	m[0][1] = 0.0f;
	m[0][2] = 0.0f;
	m[0][3] = 0.0f;

	m[1][0] = 0.0f;
	m[1][1] = 1.0f;
	m[1][2] = 0.0f;
	m[1][3] = 0.0f;

	m[2][0] = 0.0f;
	m[2][1] = 0.0f;
	m[2][2] = 1.0f;
	m[2][3] = 0.0f;

	m[3][0] = rkVector.x;
	m[3][1] = rkVector.y;
	m[3][2] = rkVector.z;
	m[3][3] = 1.0f;
*/
}


// TODO: optimize.. remove makeidentity
inline Matrixx::Matrixx(const Vector3x &rkTranslation, const Quaternionx &rkRotation)
{
	MakeIdentity();
	Set(rkTranslation, rkRotation);
}
