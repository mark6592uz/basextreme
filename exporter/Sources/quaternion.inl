
#include "constants.h"
#include "Vector3f.h"
#include "Matrix.h"


void Quaternionx::Set(float fX, float fY, float fZ, float fW)
{
	x = fX;
	y = fY;
	z = fZ;
	w = fW;
}


void Quaternionx::ToMatrixx(Matrixx *pkMatrixx) const
{
	float fTX  = 2.0f * x;
	float fTY  = 2.0f * y;
	float fTZ  = 2.0f * z;
	float fTWX = fTX * w;
	float fTWY = fTY * w;
	float fTWZ = fTZ * w;
	float fTXX = fTX * x;
	float fTXY = fTY * x;
	float fTXZ = fTZ * x;
	float fTYY = fTY * y;
	float fTYZ = fTZ * y;
	float fTZZ = fTZ * z;

	float *pfMat = &pkMatrixx->m[0][0];
	pfMat[0]  = 1.0f - ( fTYY + fTZZ );
	pfMat[4]  = fTXY - fTWZ;
	pfMat[8]  = fTXZ + fTWY;

	pfMat[1]  = fTXY + fTWZ;
	pfMat[5]  = 1.0f - ( fTXX + fTZZ );
	pfMat[9]  = fTYZ - fTWX;
	
	pfMat[2]  = fTXZ - fTWY;
	pfMat[6]  = fTYZ + fTWX;
	pfMat[10] = 1.0f - ( fTXX + fTYY );
}


Quaternionx& Quaternionx::FromEulerAngles(float fAngleX, float fAngleY, float fAngleZ)
{
	float  fAngle;
	float  sr, sp, sy, cr, cp, cy;

	fAngle = fAngleZ * 0.5f;
	sy = sinf(fAngle);
	cy = cosf(fAngle);

	fAngle = fAngleY * 0.5f;
	sp = sinf(fAngle);
	cp = cosf(fAngle);

	fAngle = fAngleX * 0.5f;
	sr = sinf(fAngle);
	cr = cosf(fAngle);

	float cpcy = cp * cy;
	float spcy = sp * cy;
	float cpsy = cp * sy;
	float spsy = sp * sy;

	x = sr * cpcy - cr * spsy;
	y = cr * spcy + sr * cpsy;
	z = cr * cpsy - sr * spcy;
	w = cr * cpcy + sr * spsy;

	return Normalize();
}


Quaternionx& Quaternionx::Normalize()
{
	float fNorm = x * x + y * y + z * z + w * w;

	if( fNorm < HM_EPSILON ) {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	} else if( fabs( fNorm - 1.0f ) > HM_EPSILON ) {
		float fScale = 1.0f / sqrtf(fNorm);
	
		x *= fScale;
		y *= fScale;
		z *= fScale;
		w *= fScale;
	}

	return( *this ); 
}


Quaternionx& Quaternionx::Inverse()
{
	Set(-x, -y, -z, w);
	return *this;
}


float& Quaternionx::operator[](int i)
{
	assert((i >= 0) && (i <= 3));
	return (&x)[i]; 
}


inline Quaternionx Quaternionx::operator+(const Quaternionx &kOther) const
{
	return Quaternionx(
		x + kOther.x,
		y + kOther.y,
		z + kOther.z,
		w + kOther.w
	);
}


Quaternionx& Quaternionx::operator*=(const Quaternionx &kOther)
{
	Vector3x kVecOne(x, y, z);
	Vector3x kVecTwo(kOther.x, kOther.y, kOther.z);
	Vector3x kVecResult = (kVecOne % kVecTwo) + kVecTwo * w + kVecOne * kOther.w;

	x = kVecResult.x;
	y = kVecResult.y;
	z = kVecResult.z;
	w = w * kOther.w - kVecOne * kVecTwo;

	return *this;
}


Quaternionx Quaternionx::operator*(const Quaternionx &kOther) const
{
	Quaternionx kResult;

	Vector3x kVecOne(x, y, z);
	Vector3x kVecTwo(kOther.x, kOther.y, kOther.z);
	Vector3x kVecResult = (kVecOne % kVecTwo) + kVecTwo * w + kVecOne * kOther.w;

	kResult.x = kVecResult.x;
	kResult.y = kVecResult.y;
	kResult.z = kVecResult.z;
	kResult.w = w * kOther.w - kVecOne * kVecTwo;

	return kResult;
}


inline Quaternionx Quaternionx::operator*(const float fScale) const
{
	return Quaternionx(x * fScale, y * fScale, z * fScale, w * fScale);
}


Vector3x Quaternionx::operator *(const Vector3x &rkVector) const
{
	Vector3x kVec0(x, y, z);
	Vector3x kVec1(kVec0 % rkVector);

	kVec1 += rkVector * w;

	Vector3x kVec2 = kVec1 % kVec0;

	kVec0 *= rkVector * kVec0;
	kVec0 += kVec1 * w;

	return Vector3x(kVec0 - kVec2);
}


inline Quaternionx& Quaternionx::operator=(const Matrixx &rkMatrixx)
{
	float fTrace = rkMatrixx[0][0] + rkMatrixx[1][1] + rkMatrixx[2][2];
	float fRoot;

	if( fTrace > 0.0 ) {
		fRoot = sqrtf( fTrace + 1.0f );

		w = 0.5f * fRoot;

		fRoot = 0.5f / fRoot;

		x = ( rkMatrixx[1][2] - rkMatrixx[2][1] ) * fRoot;
		y = ( rkMatrixx[2][0] - rkMatrixx[0][2] ) * fRoot;
		z = ( rkMatrixx[0][1] - rkMatrixx[1][0] ) * fRoot;
	} else {
		int iNext[3] = { 1, 2, 0 };

		int i = 0;
		if( rkMatrixx[1][1] > rkMatrixx[0][0] )
			i = 1;

		if( rkMatrixx[2][2] > rkMatrixx[i][i] )
			i = 2;

		int j = iNext[i];
		int k = iNext[j];

		fRoot = sqrtf( rkMatrixx[i][i] - rkMatrixx[j][j] - rkMatrixx[k][k] + 1.0f );

		float *apfQuat[3] = { &x, &y, &z };

		*(apfQuat[i]) = 0.5f * fRoot;

		fRoot = 0.5f / fRoot;

		w = ( rkMatrixx[j][k] - rkMatrixx[k][j] ) * fRoot;

		*(apfQuat[j]) = ( rkMatrixx[i][j] + rkMatrixx[j][i] ) * fRoot;
		*(apfQuat[k]) = ( rkMatrixx[i][k] + rkMatrixx[k][i] ) * fRoot;
	}

	return Normalize();
}


inline Quaternionx::Quaternionx(float fX, float fY, float fZ)
{
	FromEulerAngles(fX, fY, fZ);
}
