#include "Timer/PerformanceTimer.h"

/*
这里记录一下性能计时器的原理：
有一个CPU高精度计时器，固定频率递增计数
有两个API：
QueryPerformanceFrequency是获取计时器的频率，也就是每秒计数多少次
QueryPerformanceCounter是获取当前的计数值

*/


namespace PerformanceTimer {

	static double s_secondsPerCount = 0.0;
	static _int64 s_frequency = 0;

	//下面是基础功能，返回的是时间double

	//初始化性能计时器
	void Initialize()
	{
		static bool initialized = false;
		if (initialized)
		{
			return;
		}


		LARGE_INTEGER countsPerSec;
		QueryPerformanceFrequency(&countsPerSec);
		s_frequency = countsPerSec.QuadPart;
		s_secondsPerCount = 1.0 / static_cast<double>(countsPerSec.QuadPart);

		initialized = true;
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








	_int64 GetCount()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return currentTime.QuadPart;
	}



	_int64 GetDeltaCount(_int64 lastCount, _int64 currCount)
	{
		return currCount - lastCount;
	}

	double CountsToSeconds(_int64 counts)
	{
		return static_cast<double>(counts) * s_secondsPerCount;
	}

	_int64 SecondsToCounts(double seconds)
	{
		return static_cast<_int64>(seconds / s_secondsPerCount);
	}

	_int64 GetFrequencyCounts()
	{
		return s_frequency;
	}

	
	double GetSecondsPerCount()
	{
		return s_secondsPerCount;
	}



}
