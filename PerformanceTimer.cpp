#include "PerformanceTimer.h"

/*
这里记录一下性能计时器的原理：
有一个CPU高精度计时器，固定频率递增计数
有两个API：
QueryPerformanceFrequency是获取计时器的频率，也就是每秒计数多少次
QueryPerformanceCounter是获取当前的计数值

*/


namespace PerformanceTimer {

	static double s_secondsPerCount = 0.0;

	void Initialize()
	{
		LARGE_INTEGER countsPerSec;
		QueryPerformanceFrequency(&countsPerSec);
		s_secondsPerCount = 1.0 / static_cast<double>(countsPerSec.QuadPart);
	}


	double GetTime()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return static_cast<double>(currentTime.QuadPart)*s_secondsPerCount;
	}


	double GetDeltaTime(double lastTime)
	{
		double currentTime = GetTime();
		return currentTime - lastTime;
	}


	double GetFrequency()
	{
		return s_secondsPerCount;
	}
}
