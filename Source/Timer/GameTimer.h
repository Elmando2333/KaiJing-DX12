#pragma once
#include <windows.h>
#include "Timer/PerformanceTimer.h"

class GameTimer
{
public:
	GameTimer();
	~GameTimer()=default;

	//还是得禁用拷贝
	GameTimer(const GameTimer&) = delete;
	GameTimer& operator=(const GameTimer&) = delete;


	float TotalTimeSeconds() const;
	float DeltaTimeSeconds() const;
	float DeltaTime() const;

	//返回一下计数值版本
	_int64 TotalTimeCounts() const;
	_int64 DeltaTimeCounts() const;

	
		 

	void Reset();
	void Start();
	void Stop();
	void Pause();
	void Resume();

	void Tick();

	//检查的
	bool IsPaused() const { return m_isPaused; }
	bool IsStopped() const { return m_isStopped; }




	float GetBaseTime() const;
	float GetCurrentTime() const;
	float GetPausedTime() const;

	_int64 GetBaseTimeCounts() const { return m_baseCounts; }
	_int64 GetCurrentTimeCounts() const { return m_currCounts; }
	_int64 GetPausedTimeCounts() const { return m_pausedCounts; }


private:
	double m_secondsPerCount = 0.0;

	_int64 m_deltaCounts = -1;

	_int64 m_baseCounts = 0;
	_int64 m_pausedCounts = 0;
	_int64 m_stopCounts = 0;
	_int64 m_prevCounts = 0;
	_int64 m_currCounts = 0;

	bool m_isPaused = false;
	bool m_isStopped = false;

};

