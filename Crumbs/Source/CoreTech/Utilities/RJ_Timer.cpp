#include "RJ_Timer.h"

RJ_Timer::RJ_Timer()
{
	QueryPerformanceFrequency(&m_ticksPerSecond);

	m_runningTime = m_timeElapsed = 0.0;
	m_timerStopped = true;
}

RJ_Timer::~RJ_Timer()
{

}

void RJ_Timer::Start()
{
	if(!m_timerStopped)
		return;

	QueryPerformanceCounter(&m_lastTime);
	m_timerStopped = false;
	m_timeElapsed = 0.0;
	m_runningTime = 0.0;
}

void RJ_Timer::Stop()
{
	if(m_timerStopped)
		return;

	LARGE_INTEGER stopTime;
	QueryPerformanceCounter(&stopTime);
	m_runningTime += (double)(stopTime.QuadPart - m_lastTime.QuadPart) / m_ticksPerSecond.QuadPart;
	m_timerStopped = true;
}

void RJ_Timer::Update()
{
	if(m_timerStopped)
		return;
	
	QueryPerformanceCounter(&m_currentTime);

	m_timeElapsed = (double)(m_currentTime.QuadPart - m_lastTime.QuadPart) / m_ticksPerSecond.QuadPart;
	m_runningTime += m_timeElapsed;

	m_lastTime = m_currentTime;
}