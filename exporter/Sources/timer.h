#ifndef _TIMER_H
#define _TIMER_H


class Timer
{
	unsigned __int64	m_ulStartTime;

	unsigned __int64	m_ulOldTime;

public:
	/// \returns Time since timer was started
	float			GetTime();

	/// \returns Delta time
	float			GetDeltaTime();

	/// Returns delta time and resets the counter
	/// \returns Delta time
	float			GetDeltaTimeAndReset();

	/// Reset timer.
	void			Reset();

	/// Reset start time.
	void			ResetStartTime();
};


#endif // _TIMER_H
