#include "GameTimer.h"


GameTimer::GameTimer()
{
	PerformanceTimer::Initialize();
	m_secondsPerCount = PerformanceTimer::GetSecondsPerCount();
}

void GameTimer::Reset()
{
	m_baseCounts = PerformanceTimer::GetCount();
	m_prevCounts = m_baseCounts;
	m_currCounts = m_baseCounts;
	m_stopCounts = 0;
	m_pausedCounts = 0;
	m_isStopped = false;
	m_isPaused = false;
}

void GameTimer::Start()
{
	_int64 startCounts = PerformanceTimer::GetCount();

	if (m_isStopped)
	{
		m_pausedCounts += (startCounts - m_stopCounts);
		m_prevCounts = startCounts;
		m_stopCounts = 0;
		m_isPaused = false;
		m_isStopped = false;
	}
}


void GameTimer::Stop()
{
	if (!m_isStopped)
	{
		m_stopCounts = PerformanceTimer::GetCount();
		m_isStopped = true;
	}
}

void GameTimer::Pause()
{
	if (!m_isPaused && !m_isStopped)
	{
		m_stopCounts = PerformanceTimer::GetCount();
		m_isPaused = true;
	}
}

void GameTimer::Resume()
{
	if (m_isPaused && !m_isStopped)
	{
		_int64 currentCounts = PerformanceTimer::GetCount();

		m_pausedCounts += (currentCounts - m_stopCounts);
		m_prevCounts = currentCounts;
		m_stopCounts = 0;
		m_isPaused = false;
	}
}


void GameTimer::Tick()
{
	if (m_isStopped || m_isPaused)
	{
		m_deltaCounts = 0;
		return;
	}

	m_currCounts = PerformanceTimer::GetCount();
	m_deltaCounts = PerformanceTimer::GetDeltaCount(m_prevCounts, m_currCounts);

	m_prevCounts = m_currCounts;

	if (m_deltaCounts < 0) m_deltaCounts = 0;
}

float GameTimer::TotalTimeSeconds() const
{
	_int64 totalCounts = TotalTimeCounts();
	return static_cast<float>(PerformanceTimer::CountsToSeconds(totalCounts));
}

float GameTimer::DeltaTimeSeconds() const
{
	return static_cast<float>(PerformanceTimer::CountsToSeconds(m_deltaCounts));
}
float GameTimer::DeltaTime() const
{
	return static_cast<float>(PerformanceTimer::CountsToSeconds(m_deltaCounts));
}


_int64 GameTimer::TotalTimeCounts() const
{
	if (m_isStopped)
	{
		return m_stopCounts - m_baseCounts - m_pausedCounts;
	}

	if (m_isPaused)
	{
		return m_stopCounts - m_baseCounts - m_pausedCounts;
	}

	return m_currCounts - m_baseCounts - m_pausedCounts;
}

_int64 GameTimer::DeltaTimeCounts() const
{
	return m_deltaCounts;
}

float GameTimer::GetBaseTime() const
{
	return static_cast<float>(PerformanceTimer::CountsToSeconds(m_baseCounts));
}

float GameTimer::GetCurrentTime() const
{
	return static_cast<float>(PerformanceTimer::CountsToSeconds(m_currCounts));
}

float GameTimer::GetPausedTime() const
{
	return static_cast<float>(PerformanceTimer::CountsToSeconds(m_pausedCounts));
}


