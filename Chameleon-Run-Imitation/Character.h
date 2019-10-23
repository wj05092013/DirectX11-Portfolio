#pragma once

/*
PCH: Yes
*/

namespace ba
{
	class Character : public Model
	{
	public:
		Character();
		~Character() override;

		void UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256]);
	};
}
