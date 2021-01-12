#pragma once
#pragma once
#include <chrono>
#include <thread>
#include <cmath>

#include "Singleton.h"

// Abbreviated from TimeManager class from my 2D Engine - https://matthiasseys.me/Projects/protoengine.html
// Helps capping FPS, specifically in this case to limit my code to the gameboy to allow the key buttons to go through correctly

#define TimePoint std::chrono::steady_clock::time_point  // NOLINT(cppcoreguidelines-macro-usage)

class TimeManager final : public Singleton<TimeManager>
{
	float SecondsToMicroSeconds(float seconds)
	{
		return std::roundf(seconds * 1'000'000);
	}
	float MicroSecondsToSeconds(float microSeconds)
	{
		return microSeconds / 1'000'000;
	}
	
public:
	void SetFPSLimit(float fpsLimit) { m_FPSLimit = fpsLimit; }
	void SetStartTime(const TimePoint& startTime) { m_StartTime = startTime; }
	void SetCurrTime(const TimePoint& currTime) { m_CurrTime = currTime; }
	TimePoint GetCurrTime() const { return m_CurrTime; }

	void UpdateTime()
	{
		DeltaTime == 0.f ? FPS = 0 : FPS = int(1.f / DeltaTime);
		FPS_Unscaled = int(1.f / DeltaTime_Unscaled);

		DeltaTime = std::chrono::duration<float>(m_CurrTime - m_StartTime).count();

		const int targetFPS_microSeconds{ int(SecondsToMicroSeconds(1.f / m_FPSLimit)) };
		const int sleepTime_microSeconds{ targetFPS_microSeconds - int(SecondsToMicroSeconds(DeltaTime)) };

		std::this_thread::sleep_for(std::chrono::microseconds(sleepTime_microSeconds));

		DeltaTime += MicroSecondsToSeconds(float(sleepTime_microSeconds));

		DeltaTime_Unscaled = DeltaTime;
		DeltaTime *= TimeScale;
	}

	const float FixedDeltaTime{ 0.02f };
	float DeltaTime{};
	unsigned int FPS{};
	float TimeScale{ 1 };

	float DeltaTime_Unscaled;
	unsigned int FPS_Unscaled;

private:
	TimePoint m_StartTime;
	TimePoint m_CurrTime;

	float m_FPSLimit{ 60 };
};