#include "EnginePrediction.h"
#include "hooks.hpp"
#include "valve_sdk/sdk.hpp"
#include "options.hpp"

void C_EnginePrediction::Start(CUserCmd* cmd)
{
	static bool predictionRandomSeedInit = false;
	if (!predictionRandomSeedInit)
	{
		predictionRandomSeed = *(int**)(Utils::FindPattern( "client_panorama.dll", (BYTE*)"\x8B\x0D\x00\x00\x00\x00\xBA\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x04", "xx????x????x????xxx") + 2);
		predictionRandomSeedInit = true;
	}

	*predictionRandomSeed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;

	oldCurtime = g_GlobalVars->curtime;
	oldFrametime = g_GlobalVars->frametime;

	g_GlobalVars->curtime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(g_LocalPlayer);

	memset(&moveData, 0, sizeof(moveData));
	g_MoveHelper->SetHost(g_LocalPlayer);
	g_Prediction->SetupMove(g_LocalPlayer, cmd, g_MoveHelper, &moveData);
	g_GameMovement->ProcessMovement(g_LocalPlayer, &moveData);
	g_Prediction->FinishMove(g_LocalPlayer, cmd, &moveData);
}

void C_EnginePrediction::End()
{
	g_GameMovement->FinishTrackPredictionErrors(g_LocalPlayer);
	g_MoveHelper->SetHost(0);

	*predictionRandomSeed = -1;

	g_GlobalVars->curtime = oldCurtime;
	g_GlobalVars->frametime = oldFrametime;
}