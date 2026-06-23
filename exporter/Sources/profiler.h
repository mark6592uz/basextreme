
#include "timer.h"

extern Timer g_cTimer;


class ProfilePoint
{
	float m_fTime;

	const char* m_szName;

public:
	ProfilePoint(const char *szName)
	{
		m_fTime = g_cTimer.GetTime();
		m_szName = szName;
	}

	~ProfilePoint()
	{
		//printf("PROFILE %s %f\n", m_szName, g_cTimer.GetTime() - m_fTime);
	}
};


#define PROFILE(name) ProfilePoint cPoint(#name);

