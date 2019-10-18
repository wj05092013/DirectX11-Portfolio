#include "stdafx.h"

ba::Timer::Timer() :
	seconds_per_count_(0.0),
	delta_time_(0.0),

	base_count_(0),
	prev_count_(0),
	curr_count_(0),

	paused_count_(0),
	total_paused_count_(0),

	paused_(false)
{
	__int64 counts_per_second;
	QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&counts_per_second));
	seconds_per_count_ = 1.0 / counts_per_second;
}

void ba::Timer::Reset()
{
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&base_count_));

	prev_count_ = base_count_;
	curr_count_ = base_count_;

	total_paused_count_ = 0;
}

void ba::Timer::Tick()
{
	if (paused_)
	{
		delta_time_ = 0.0;
		return;
	}

	prev_count_ = curr_count_;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&curr_count_));
	delta_time_ = (curr_count_ - prev_count_) * seconds_per_count_;

	if (delta_time_ < 0.0)
		delta_time_ = 0.0;
}

void ba::Timer::Pause()
{
	if (paused_)
		return;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&paused_count_));
	paused_ = true;
}

void ba::Timer::Unpause()
{
	if (!paused_)
		return;

	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&curr_count_));
	total_paused_count_ += paused_count_ - curr_count_;

	paused_ = false;
}

double ba::Timer::get_total_time()
{
	if (paused_)
		return (paused_count_ - base_count_ - total_paused_count_) * seconds_per_count_;

	return (curr_count_ - base_count_ - total_paused_count_) * seconds_per_count_;
}

double ba::Timer::get_delta_time()
{
	return delta_time_;
}

bool ba::Timer::query_paused()
{
	return paused_;
}
