#pragma once
#include "valve_sdk/sdk.hpp"

struct obs_vis_player_t
{
	RECT bBox;
	bool valid_ent = false;

	Vector bonePos1;
	Vector bonePos2;
};

namespace OBS
{

	void OnPreset( );
	void OnPaintTraverse( );


	void DrawBox( C_BasePlayer* ent );
}

extern obs_vis_player_t pl[64];