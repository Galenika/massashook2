#pragma once
#include "..\valve_sdk\sdk.hpp"
#include "..\hooks.hpp"

bool CanHit(C_BasePlayer* entity, const Vector &point, float *damage_given);
float CanWallbang();