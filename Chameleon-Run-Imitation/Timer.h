#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Timer
	{
	public:
		Timer();

		void Reset();
		void Tick();
		void Pause();
		void Unpause();

		double get_total_time();
		double get_delta_time();

		bool query_paused();

	private:
		double seconds_per_count_;
		double delta_time_;

		__int64 base_count_;
		__int64 prev_count_;
		__int64 curr_count_;

		__int64 paused_count_;
		__int64 total_paused_count_;

		bool paused_;
	};
}
