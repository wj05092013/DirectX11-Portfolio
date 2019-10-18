#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Uncopiable
	{
	public:
		Uncopiable() {}
		virtual ~Uncopiable() = 0 {};

	private:
		Uncopiable(const Uncopiable&) = delete;
		Uncopiable& operator=(const Uncopiable&) = delete;
	};
}
