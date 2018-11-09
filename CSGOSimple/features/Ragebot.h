#pragma once
#include "..\hooks.hpp"
#include "..\valve_sdk\sdk.hpp"

class C_Ragebot : public Singleton<C_Ragebot>
{
public:
	void OnCreateMove( CUserCmd* cmd );
	int Get_Target( );

	bool can_aa = false;

private:
	void Ragebot( CUserCmd* cmd );
	int GetBestTarget( );
	bool GetHitboxPos( C_BasePlayer* entity , int hitbox , Vector &output );
	int GetBestHitbox( C_BasePlayer* ent );

	int best_target = 0;
	bool baim;
	void sync_config( C_BaseCombatWeapon* w );
	float hit_chance = 0;
	float min_damage = 0;
	bool valid_wep = 0;


	void Antiaim( CUserCmd* cmd );


};
