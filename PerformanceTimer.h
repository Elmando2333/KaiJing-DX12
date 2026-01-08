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

}

