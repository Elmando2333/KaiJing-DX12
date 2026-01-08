#include "GameTimer.h"


GameTimer::GameTimer()
{
	PerformanceTimer::Initialize();
}

void GameTimer::Reset()
{
	m_baseTime= PerformanceTimer::GetTime();
	m_prevTime = m_baseTime;
	m_currTime = m_baseTime;
	m_stopTime = 0.0;
	m_pausedTime = 0.0;
	m_isStopped = false;
	m_isPaused = false;
}

//这里注意一下，被stop的timer只能被start开启。被paused的timer只能被resume恢复
void GameTimer::Start()
{
	double startTime = PerformanceTimer::GetTime();

	if (m_isStopped)//之前是停止状态
	{
		m_pausedTime += (startTime - m_stopTime);//算一下总共的暂停时间
		m_prevTime = startTime;
		m_stopTime = 0.0;
		m_isPaused = false;
		m_isStopped = false;
	}

	
}


void GameTimer::Stop()
{
	if (!m_isStopped)
	{
		m_stopTime = PerformanceTimer::GetTime();
		m_isStopped = true;
	}
}

void GameTimer::Pause()
{
	if (!m_isPaused&&!m_isStopped)
	{
		m_stopTime = PerformanceTimer::GetTime();
		m_isPaused = true;
	}
}

void GameTimer::Resume()
{
	if (m_isPaused && !m_isStopped)
	{
		double currentTime = PerformanceTimer::GetTime();

		m_pausedTime += (currentTime - m_stopTime);
		m_prevTime = currentTime;
		m_stopTime = 0.0;
		m_isPaused = false;
	}
}


void GameTimer::Tick()
{
	if (m_isStopped||m_isPaused)
	{
		m_deltaTime = 0.0;
		return;
	}

	m_currTime = PerformanceTimer::GetTime();
	m_deltaTime = PerformanceTimer::GetDeltaTime(m_prevTime);

	m_prevTime = m_currTime;

	if (m_deltaTime < 0.0) m_deltaTime = 0.0;
}

//游戏时间不包括暂停时间
float GameTimer::TotalTime() const
{
	


	if (m_isStopped)
	{
		return static_cast<float>(m_stopTime - m_baseTime - m_pausedTime);
	}


	if (m_isPaused)
	{
		return static_cast<float>(m_stopTime - m_baseTime - m_pausedTime);
	}


	

	return m_currTime - m_baseTime - m_pausedTime;

}


float GameTimer::DeltaTime() const
{
	return static_cast<float>(m_deltaTime);
}


