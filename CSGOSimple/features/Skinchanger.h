#pragma once

#include "../hooks.hpp"
#include "../valve_sdk/sdk.hpp"

class C_Skins : public Singleton<C_Skins>
{
public:
	void Skinchanger();
	void Glovechanger();

	void Fullupdate();

};
