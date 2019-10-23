#include "stdafx.h"
#include <iostream>

ba::Character::Character()
{
}

ba::Character::~Character()
{
}

void ba::Character::UpdateOnKeyInput(bool key_pressed[256], bool key_switch[256])
{
	if (key_pressed[VK_SPACE])
		std::cout << "no" << std::endl;
}
