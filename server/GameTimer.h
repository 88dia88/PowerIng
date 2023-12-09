#pragma once

#include <Windows.h>

class GameTimer
{
public:

	GameTimer();

	float TotalTime()const;	// 초 단위
	float DeltaTime()const;	// 초 단위

	void Reset();			// 메세지루프 이전에 호출
	void Start();			// 타이머 시작 또는 재개
	void Stop();			// 타이머 정지
	void Tick();			// 매 프레임마다 호출

private:

	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;		// 타이머가 시작된 시간
	__int64 mPausedTime;	// 일시정지된 시간
	__int64 mStopTime;		// 일시정지가 시작된 시간
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

