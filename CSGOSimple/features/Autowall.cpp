#include "..\hooks.hpp"
#include "..\valve_sdk\sdk.hpp"
#include "..\helpers\math.hpp"

#include "Autowall.h"

#define    HITGROUP_GENERIC    0
#define    HITGROUP_HEAD        1
#define    HITGROUP_CHEST        2
#define    HITGROUP_STOMACH    3
#define		HITGROUP_LEFTARM    4    
#define		HITGROUP_RIGHTARM    5
#define		HITGROUP_LEFTLEG    6
#define		HITGROUP_RIGHTLEG    7
#define	 HITGROUP_GEAR        10

inline bool CGameTrace::DidHitWorld() const
{
	return hit_entity->EntIndex() == 0;
}

inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return hit_entity != NULL && !DidHitWorld();
}

bool HandleBulletPenetration(CCSWeaponInfo *wpn_data);

float GetHitgroupDamageMult(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 1.f;
	case HITGROUP_HEAD:
		return 4.f;
	case HITGROUP_CHEST:
		return 1.f;
	case HITGROUP_STOMACH:
		return 1.25f;
	case HITGROUP_LEFTARM:
		return 1.f;
	case HITGROUP_RIGHTARM:
		return 1.f;
	case HITGROUP_LEFTLEG:
		return 0.75f;
	case HITGROUP_RIGHTLEG:
		return 0.75f;
	case HITGROUP_GEAR:
		return 1.f;
	default:
		break;
	}

	return 1.f;
}

bool IsArmored(C_BasePlayer* Entity, int ArmorValue, int Hitgroup)
{
	bool result = false;

	if (ArmorValue > 0)
	{
		switch (Hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			result = true;
			break;
		case HITGROUP_HEAD:
			result = Entity->m_bHasHelmet(); // DT_CSPlayer -> m_bHasHelmet
			break;
		}
	}

	return result;
}

void ScaleDamage(int Hitgroup, C_BasePlayer* Entity, float WeaponArmorRatio, float &Damage)
{
	// NOTE: the Guardian/Coop Missions/Gamemode have bots with heavy armor which has a less damage modifier
	auto HeavyArmor = Entity->m_bHasHeavyArmor(); // DT_CSPlayer -> m_bHasHeavyArmor
	auto ArmorValue = Entity->m_ArmorValue(); // DT_CSPlayer -> m_ArmorValue

	switch (Hitgroup)
	{
	case HITGROUP_HEAD:
		if (HeavyArmor)
			Damage = (Damage * 4.f) * 0.5f;
		else
			Damage *= 4.f;
		break;
	case HITGROUP_STOMACH:
		Damage *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		Damage *= 0.75f;
		break;
	}

	if (IsArmored(Entity, ArmorValue, Hitgroup))
	{
		float v47 = 1.f, ArmorBonusRatio = 0.5f, ArmorRatio = WeaponArmorRatio * 0.5f;

		if (HeavyArmor)
		{
			ArmorBonusRatio = 0.33f;
			ArmorRatio = (WeaponArmorRatio * 0.5f) * 0.5f;
			v47 = 0.33f;
		}

		auto NewDamage = Damage * ArmorRatio;

		if (HeavyArmor)
			NewDamage *= 0.85f;

		if (((Damage - (Damage * ArmorRatio)) * (v47 * ArmorBonusRatio)) > ArmorValue)
			NewDamage = Damage - (ArmorValue / ArmorBonusRatio);

		Damage = NewDamage;
	}
}

struct
{

	Vector           src;
	trace_t          enter_trace;
	Vector           direction;
	CTraceFilter    filter;
	float           trace_length;
	float           trace_length_remaining;
	float           current_damage;
	int             penetrate_count;
}FireBulletData;

void UTIL_TraceLine(const Vector& vecAbsStart, const Vector& vecAbsEnd, unsigned int mask, C_BasePlayer *ignore, int collisionGroup, trace_t *ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilter traceFilter;
	traceFilter.pSkip = ignore;
	g_EngineTrace->TraceRay(ray, mask, &traceFilter, ptr);
}

void UTIL_ClipTraceToPlayers(C_BasePlayer* pEntity, Vector start, Vector end, unsigned int mask, ITraceFilter* filter, trace_t* tr)
{
	trace_t playerTrace;
	Ray_t ray;
	float smallestFraction = tr->fraction;

	ray.Init(start, end);

	if (!pEntity || !pEntity->IsAlive() || pEntity->IsDormant())
		return;

	if (filter && filter->ShouldHitEntity(pEntity, mask) == false)
		return;

	g_EngineTrace->ClipRayToEntity(ray, mask | CONTENTS_HITBOX, pEntity, &playerTrace);
	if (playerTrace.fraction < smallestFraction)
	{
		// we shortened the ray - save off the trace
		*tr = playerTrace;
		smallestFraction = playerTrace.fraction;
	}
}

bool SimulateFireBullet(C_BasePlayer* entity, C_BasePlayer *local, C_BaseCombatWeapon *weapon)
{
	//Utils::ToLog("SimulateFireBullet");
	FireBulletData.penetrate_count = 4;
	FireBulletData.trace_length = 0.0f;
	auto *wpn_data = weapon->GetCSWeaponData();

	FireBulletData.current_damage = static_cast<float>(wpn_data->iDamage);

	while ((FireBulletData.penetrate_count > 0) && (FireBulletData.current_damage >= 1.0f))
	{
		FireBulletData.trace_length_remaining = wpn_data->flRange - FireBulletData.trace_length;

		Vector end = FireBulletData.src + FireBulletData.direction * FireBulletData.trace_length_remaining;

		UTIL_TraceLine(FireBulletData.src, end, 0x4600400B, local, 0, &FireBulletData.enter_trace);
		UTIL_ClipTraceToPlayers(entity, FireBulletData.src, end + FireBulletData.direction * 40.f, 0x4600400B, &FireBulletData.filter, &FireBulletData.enter_trace);

		if (FireBulletData.enter_trace.fraction == 1.0f)
			break;

		C_BasePlayer* hit_ent = (C_BasePlayer*)FireBulletData.enter_trace.hit_entity;

		if ((FireBulletData.enter_trace.hitgroup <= 7)
			&& (FireBulletData.enter_trace.hitgroup > 0))
		{
			FireBulletData.trace_length += (float)(FireBulletData.enter_trace.fraction * FireBulletData.trace_length_remaining);
			FireBulletData.current_damage *= (float)(pow(wpn_data->flRangeModifier, FireBulletData.trace_length * 0.002));
			ScaleDamage(FireBulletData.enter_trace.hitgroup, hit_ent, wpn_data->flArmorRatio, FireBulletData.current_damage);

			return true;
		}

		if (!HandleBulletPenetration(wpn_data))
			break;
	}

	return false;
}

#include "..\options.hpp"
bool TraceToExit(Vector& end, trace_t& tr, Vector start, Vector vEnd, trace_t* trace)
{
	typedef bool(__fastcall* TraceToExitFn)(Vector&, trace_t&, float, float, float, float, float, float, trace_t*);
	static DWORD TraceToExit = Utils::FindPattern( "client_panorama.dll", (BYTE*)"\x55\x8B\xEC\x83\xEC\x30\xF3\x0F\x10\x75", "xxxxxxxxxx");

	if (!TraceToExit)
		return false;

	float start_y = start.y, start_z = start.z, start_x = start.x, dir_y = vEnd.y, dir_x = vEnd.x, dir_z = vEnd.z;

	_asm
	{
		push trace
		push dir_z
		push dir_y
		push dir_x
		push start_z
		push start_y
		push start_x
		mov edx, tr
		mov ecx, end
		call TraceToExit
		add esp, 0x1C
	}
}

inline vec_t VectorLength(const Vector& v)
{
	return (vec_t)sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

bool HandleBulletPenetration(CCSWeaponInfo *wpn_data)
{
	surfacedata_t *enter_surface_data = g_PhysSurface->GetSurfaceData(FireBulletData.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;


	FireBulletData.trace_length += FireBulletData.enter_trace.fraction * FireBulletData.trace_length_remaining;
	FireBulletData.current_damage *= (float)(pow(wpn_data->flRangeModifier, (FireBulletData.trace_length * 0.002)));

	if ((FireBulletData.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		FireBulletData.penetrate_count = 0;

	if (FireBulletData.penetrate_count <= 0)
		return false;

	Vector dummy;
	trace_t trace_exit;
	if (!TraceToExit(dummy, FireBulletData.enter_trace, FireBulletData.enter_trace.endpos, FireBulletData.direction, &trace_exit))
		return false;

	surfacedata_t *exit_surface_data = g_PhysSurface->GetSurfaceData(trace_exit.surface.surfaceProps);
	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((FireBulletData.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f;
		final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (FireBulletData.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->flPenetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - FireBulletData.enter_trace.endpos);

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;


	float lost_damage = fmaxf(0.0f, v35 + thickness);

	if (lost_damage > FireBulletData.current_damage)
		return false;

	if (lost_damage >= 0.0f)
		FireBulletData.current_damage -= lost_damage;

	if (FireBulletData.current_damage < 1.0f)
		return false;

	FireBulletData.src = trace_exit.endpos;
	FireBulletData.penetrate_count--;

	return true;
}

void AngleVectors5(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

void CalcAngle(Vector src, Vector dst, QAngle &angles)
{
	Vector delta = src - dst;
	double hyp = delta.Length2D();
	angles.yaw = (atan(delta.y / delta.x) * 57.295779513082f);
	angles.pitch = (vec_t)(atan(delta.z / hyp) * 57.295779513082f);
	angles[2] = 0.00;

	if (delta.x >= 0.0)
		angles.yaw += 180.0f;
}

void VectorAngles5(Vector forward, QAngle &angles)
{
	float tmp, yaw, pitch;

	yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
	tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
	pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);


	while (yaw <= -180) yaw += 360;
	while (yaw > 180) yaw -= 360;
	while (pitch <= -180) pitch += 360;
	while (pitch > 180) pitch -= 360;


	if (pitch > 89.0f)
		pitch = 89.0f;
	else if (pitch < -89.0f)
		pitch = -89.0f;

	if (yaw > 179.99999f)
		yaw = 179.99999f;
	else if (yaw < -179.99999f)
		yaw = -179.99999f;

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

bool CanHit(C_BasePlayer* entity, const Vector &point, float *damage_given)
{
	//Utils::ToLog("CANHIT");
	C_BasePlayer *local = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!local) return false;

	//auto data = FireBulletData((const Vector)(local->m_vecOrigin() + local->m_vecViewOffset()));
	FireBulletData.src = local->m_vecOrigin() + local->m_vecViewOffset();
	FireBulletData.filter = CTraceFilter();
	FireBulletData.filter.pSkip = local;

	QAngle angles;
	//	CalcAngle(data.src, point, angles);
	VectorAngles5(point - FireBulletData.src, angles);
	AngleVectors5(angles, &FireBulletData.direction);
	FireBulletData.direction.Normalize();

	if (SimulateFireBullet(entity, local, reinterpret_cast<C_BaseCombatWeapon*>(g_EntityList->GetClientEntityFromHandle((local->m_hActiveWeapon())))))
	{
		*damage_given = FireBulletData.current_damage;
		//Utils::ToLog("CANHIT END");
		return true;
	}

	return false;
}

float CanWallbang()
{

	C_BasePlayer *local = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!local) return 0;

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(local->m_hActiveWeapon());
	if (!local)
		return false;

	FireBulletData.filter = CTraceFilter();
	FireBulletData.filter.pSkip = local;
	auto flRange = weapon->GetCSWeaponData()->flRange;
	QAngle EyeAng;
	g_EngineClient->GetViewAngles(EyeAng);

	Vector dst, forward;

	AngleVectors5(EyeAng, &forward);
	dst = FireBulletData.src + (forward * 8000.f);

	QAngle angles;
	CalcAngle(FireBulletData.src, dst, angles);
	AngleVectors5(angles, &FireBulletData.direction);
	FireBulletData.direction.Normalize();



	if (!weapon)
		return false;

	FireBulletData.penetrate_count = 1;

	CCSWeaponInfo *weaponData = weapon->GetCSWeaponData();

	if (!weaponData)
		return false;

	FireBulletData.current_damage = (int)weaponData->iDamage;

	FireBulletData.trace_length_remaining = weaponData->iDamage;

	Vector end = FireBulletData.src + FireBulletData.direction * FireBulletData.trace_length_remaining;

	UTIL_TraceLine(FireBulletData.src, end, MASK_SHOT | CONTENTS_GRATE, local, 0, &FireBulletData.enter_trace);

	if (FireBulletData.enter_trace.fraction == 1.f)
		return false;

	if (HandleBulletPenetration(weaponData))
	{
		return FireBulletData.current_damage;;
	}

	return 0;
}
