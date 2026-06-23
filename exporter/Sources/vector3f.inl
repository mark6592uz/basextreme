

inline float Vector3x::Length() const
{
	return sqrtf(x*x + y*y + z*z);
}


void Vector3x::Normalize()
{
	float fSqrSize = 1.0f / sqrtf(x*x + y*y + z*z);

	x *= fSqrSize;
	y *= fSqrSize;
	z *= fSqrSize;
}


Vector3x& Vector3x::operator+= (const Vector3x& rkOther)
{
	x += rkOther.x;
	y += rkOther.y;
	z += rkOther.z;

	return *this;
}


Vector3x& Vector3x::operator-= (const Vector3x& rkOther)
{
	x -= rkOther.x;
	y -= rkOther.y;
	z -= rkOther.z;

	return *this;
}


float Vector3x::operator* (const Vector3x& kOther) const
{
	return x * kOther.x 
		+ y * kOther.y 
		+ z * kOther.z;
}


void Vector3x::operator*= (const float f)
{
	x *= f;
	y *= f;
	z *= f;
}


Vector3x Vector3x::operator% (const Vector3x& kOther) const
{
	return Vector3x(y * kOther.z - z * kOther.y, z * kOther.x - x * kOther.z, x * kOther.y - y * kOther.x);
}


inline Vector3x& Vector3x::Clear()
{
	x = 0.0f;
	y = 0.0f;
	z = 0.0f;
	return *this;
}


inline Vector3x::Vector3x(const float afValues[])
{
	x = afValues[0];
	y = afValues[1];
	z = afValues[2];
}
