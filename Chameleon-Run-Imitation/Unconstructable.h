#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Unconstructable : private Uncopiable
	{
	public:
		virtual ~Unconstructable() = 0 {};

	private:
		Unconstructable() = delete;
	};
}
