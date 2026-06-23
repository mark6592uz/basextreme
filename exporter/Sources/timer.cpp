
#include "timer.h"


#define WIN32_LEAN_AND_MEAN
#include <windows.h>


float Timer::GetTime()
{
	unsigned __int64 ulCurrentTime;
	unsigned __int64 ulFrequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&ulCurrentTime);
	QueryPerformanceFrequency((LARGE_INTEGER*)&ulFrequency);

	return float(double((__int64)(ulCurrentTime - m_ulStartTime))/double((__int64)ulFrequency));
}


float Timer::GetDeltaTime()
{
	float fDeltaTime = 0.0f;

	unsigned __int64 ulCurrentTime;
	unsigned __int64 ulFrequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&ulCurrentTime);
	QueryPerformanceFrequency((LARGE_INTEGER*)&ulFrequency);

	return float(double((__int64)(ulCurrentTime - m_ulOldTime))/double((__int64)ulFrequency));
}


float Timer::GetDeltaTimeAndReset()
{
	float fDeltaTime = 0.0f;

	unsigned __int64 ulCurrentTime;
	unsigned __int64 ulFrequency;

	QueryPerformanceCounter((LARGE_INTEGER*)&ulCurrentTime);
	QueryPerformanceFrequency((LARGE_INTEGER*)&ulFrequency);

	fDeltaTime = float(double((__int64)(ulCurrentTime - m_ulOldTime))/double((__int64)ulFrequency));
	m_ulOldTime = ulCurrentTime;

	return fDeltaTime;
}


void Timer::Reset()
{
//	QueryPerformanceCounter((LARGE_INTEGER*)&m_ulOldTime);

	LARGE_INTEGER l;
	QueryPerformanceCounter(&l);
	m_ulOldTime = l.QuadPart;
}


void Timer::ResetStartTime()
{
//	QueryPerformanceCounter((LARGE_INTEGER*)&m_ulStartTime);

	LARGE_INTEGER l;
	QueryPerformanceCounter(&l);
	m_ulStartTime = l.QuadPart;
}
