#ifndef  REDJAGUAR_JS_TIMER_H_
#define  REDJAGUAR_JS_TIMER_H_

#include <windows.h>  // For QueryPerformanceCounter()
#include <limits>

class RJ_Timer
{
	public:
		RJ_Timer();
		~RJ_Timer();

		void Start();
		void Stop();
		void Update(); 

		bool IsStopped() { return m_timerStopped; }
		double GetRunningTime() { return m_runningTime; }
		double GetElapsedTime() { return m_timerStopped ? 0.0f : (m_timeElapsed >= FLT_EPSILON ? m_timeElapsed : FLT_EPSILON); } 

	private:
		LARGE_INTEGER m_ticksPerSecond;
		LARGE_INTEGER m_currentTime;
		LARGE_INTEGER m_lastTime;
		double m_runningTime;
		double m_timeElapsed;
		bool m_timerStopped;
};

#endif // end of REDJAGUAR_JS_TIMER_H_