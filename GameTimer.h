#pragma once
#include <windows.h>
#include "PerformanceTimer.h"

class GameTimer
{
public:
	GameTimer();
	~GameTimer()=default;

	//还是得禁用拷贝
	GameTimer(const GameTimer&) = delete;
	GameTimer& operator=(const GameTimer&) = delete;


	float TotalTime() const;
	float DeltaTime() const;


	void Reset();
	void Start();
	void Stop();
	void Pause();
	void Resume();

	void Tick();

	//检查的
	bool IsPaused() const { return m_isPaused; }
	bool IsStopped() const { return m_isStopped; }


private:
	double m_secondsPerCount = 0.0;//构造里去调用性能计时器查询
	double m_deltaTime = -1.0;

	//下面主要是实现stop和pause的相关计数
	double m_baseTime = 0;//基准的时间，在reset里
	double m_pausedTime = 0;//累积暂停的时间
	double m_stopTime = 0;//记录暂停时或者停止时的计数
	double m_prevTime = 0;//上一帧
	double m_currTime = 0;//当前

	bool m_isPaused = false;
	bool m_isStopped = false;

};

