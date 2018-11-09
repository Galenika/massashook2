#pragma once
#include "hooks.hpp"
#include "valve_sdk/sdk.hpp"
#include "valve_sdk/interfaces/IMoveHelper.hpp"

class C_EnginePrediction : public Singleton<C_EnginePrediction>
{
public:
	void Start(CUserCmd* cmd); 
	void End();
private:

	float oldCurtime;
	float oldFrametime;

	CMoveData moveData;

	int *predictionRandomSeed;
};