#pragma once
#include "../valve_sdk/sdk.hpp"
#include "../hooks.hpp"

class C_Legitbot : public Singleton<C_Legitbot>
{
public:
	void OnCreateMove(CUserCmd* cmd);
	bool bIsLocked = false;

private:
	void Backtrack(CUserCmd* cmd);
	void Triggerbot(CUserCmd* cmd);
	void Aimbot(CUserCmd* cmd);

	int bestHitbox = -1;
	QAngle view_angle = QAngle(0, 0, 0);
	int GetTarget(CUserCmd* cmd, Vector& destination);
	bool GetHitbox(C_BasePlayer* entity, Vector& destination);
	QAngle get_randomized_recoil(C_BasePlayer *local);
	QAngle get_randomized_angles(C_BasePlayer *local);

	void SyncSettings(C_BaseCombatWeapon* weapon);

	bool bTrigger = false;
	int iKey = 0;
	int trigger_key = 0;
	int trigger_delay = 0;
	float hitchance;
	int iDelay = 0;
	bool bInvalidWeapon = false;
	float fFov = 0;
	float fSmooth = 0;
	float fRandS = 0;
	float fRCS = 0;
	int iBone = 0;
};
