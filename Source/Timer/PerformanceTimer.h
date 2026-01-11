#pragma once
//性能计时器书上就一点，写个工具函数，就不写成类了
#include <windows.h>

namespace PerformanceTimer
{
	//初始化，只有程序启动时调用
	void Initialize();

	//获取当前的时间
	double GetTime();

	//获取一下时间差
	double GetDeltaTime(double lastTime);

	//获取频率
	double GetFrequency();




	//为了GameTimer的扩展：
	_int64 GetCount();
	_int64 GetDeltaCount(_int64 lastCount, _int64 currCount);


	//转换
	double CountsToSeconds(_int64 counts);

	_int64 SecondsToCounts(double seconds);

	_int64 GetFrequencyCounts(double seconds);

	// 获取转换系数
	double GetSecondsPerCount();

}

