	#include "Ragebot.h"
#include "Autowall.h"
#include "..\helpers\utils.hpp"
#include "..\helpers\math.hpp"
#include "..\options.hpp"


void AngleVectors4(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

QAngle m_oldangle;
float m_oldforward;
float m_oldsidemove;

void StartFix(CUserCmd* cmd)
{
	m_oldangle = cmd->viewangles;
	m_oldforward = cmd->forwardmove;
	m_oldsidemove = cmd->sidemove;
}

void EndFix(CUserCmd* cmd)
{
	float yaw_delta = cmd->viewangles.yaw - m_oldangle.yaw;
	float f1;
	float f2;

	if (m_oldangle.yaw < 0.f)
		f1 = 360.0f + m_oldangle.yaw;
	else
		f1 = m_oldangle.yaw;

	if (cmd->viewangles.yaw < 0.0f)
		f2 = 360.0f + cmd->viewangles.yaw;
	else
		f2 = cmd->viewangles.yaw;

	if (f2 < f1)
		yaw_delta = abs(f2 - f1);
	else
		yaw_delta = 360.0f - abs(f1 - f2);
	yaw_delta = 360.0f - yaw_delta;

	cmd->forwardmove = cos(DEG2RAD(yaw_delta)) * m_oldforward + cos(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
	cmd->sidemove = sin(DEG2RAD(yaw_delta)) * m_oldforward + sin(DEG2RAD(yaw_delta + 90.f)) * m_oldsidemove;
}

void C_Ragebot::OnCreateMove(CUserCmd* cmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive() || !g_Options.rage_enable)
		return;

	StartFix(cmd);

	if (g_Options.rage_aimbot_enable)
		Ragebot(cmd);

	EndFix(cmd);
}


void C_Ragebot::sync_config(C_BaseCombatWeapon* w)
{
	CCSWeaponInfo* info = w->GetCSWeaponData();

	if (w->m_Item().m_iItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER)
	{
		valid_wep = true;
		min_damage = 130.f;
		hit_chance = 0.f;
	}
	else if (w->IsSniper(info))
	{
		valid_wep = true;
		min_damage = g_Options.rage_aimbot_sniper_mindamage;
		hit_chance = g_Options.rage_aimbot_sniper_hitchance;
	}
	else if (w->IsRifle(info))
	{
		valid_wep = true;
		min_damage = g_Options.rage_aimbot_rifle_mindamage;
		hit_chance = g_Options.rage_aimbot_rifle_hitchance;
	}
	else if (w->IsPistol(info))
	{
		valid_wep = true;
		min_damage = g_Options.rage_aimbot_pistol_mindamage;
		hit_chance = g_Options.rage_aimbot_pistol_hitchance;
	}
	else
	{
		valid_wep = false;
	}
}

void SinCos(float a, float* s, float*c)
{
	*s = sin(a);
	*c = cos(a);
}

void AngleVectors5(const QAngle &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr * -sy);
		up->y = (cr*sp*sy + -sr * cy);
		up->z = cr * cp;
	}
}

void VectorAngles4(Vector forward, QAngle &angles)
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

inline void RandomSeed(int seed)
{
	static auto fn = (decltype(&RandomSeed))(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));

	return fn(seed);
}

inline float RandomFloat(float min, float max)
{
	static auto fn = (decltype(&RandomFloat))(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
	return fn(min, max);
}


#define PI 3.14159265358979323846f
#define PI_F	((float)(PI)) 

Vector CrossProduct(const Vector &a, const Vector &b)
{
	return Vector(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

void VectorAngles6(const Vector& forward, Vector& up, QAngle& angles)
{
	Vector left = CrossProduct(up, forward);
	left.NormalizeInPlace();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
		angles.yaw = atan2f(forward.y, forward.x) * 180 / PI_F;

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.roll = atan2f(left.z, upZ) * 180 / PI_F;
	}
	else
	{
		angles.pitch = atan2f(-forward.z, forwardDist) * 180 / PI_F;
		angles.yaw = atan2f(-left.x, left.y) * 180 / PI_F;
		angles.roll = 0;
	}
}

void AngleVectors6(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

	forward.x = (cp * cy);
	forward.y = (cp * sy);
	forward.z = (-sp);
	right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
	right.z = (-1 * sr * cp);
	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = (cr * cp);
}

bool HitChance(QAngle angles, C_BasePlayer *ent, float chance)
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return false;

	Vector forward, right, up;
	Vector src = g_LocalPlayer->GetEyePos();
	AngleVectors6(angles, forward, right, up);

	int cHits = 0;
	int cNeededHits = static_cast<int>(150.f * (chance / 100.f));

	weapon->UpdateAccuracyPenalty();
	float weap_spread = weapon->GetSpread();
	float weap_inaccuracy = weapon->GetInaccuracy();

	for (int i = 0; i < 150; i++)
	{
		float a = RandomFloat(0.f, 1.f);
		float b = RandomFloat(0.f, 2.f * PI_F);
		float c = RandomFloat(0.f, 1.f);
		float d = RandomFloat(0.f, 2.f * PI_F);

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if (weapon->m_Item().m_iItemDefinitionIndex() == 64)
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalized();

		QAngle viewAnglesSpread;
		VectorAngles6(direction, up, viewAnglesSpread);
		Math::ClampAngles(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * weapon->GetCSWeaponData()->flRange);

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		g_EngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, ent, &tr);

		if (tr.hit_entity == ent)
			++cHits;

		if (static_cast<int>((static_cast<float>(cHits) / 150.f) * 100.f) >= chance)
			return true;

		if ((150 - i + cHits) < cNeededHits)
			return false;
	}
	return false;
}

void DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	if (g_LocalPlayer != nullptr)
	{
		QAngle AimPunch = g_LocalPlayer->m_aimPunchAngle();
		if (AimPunch.Length() > 0 && AimPunch.Length() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			Math::ClampAngles(pCmd->viewangles);
		}
	}
}

QAngle viewangle;

int C_Ragebot::Get_Target()
{
	return best_target;
}

void C_Ragebot::Ragebot(CUserCmd* cmd)
{

	best_target = 0;

	if (g_Options.rage_aimbot_key && !GetAsyncKeyState(g_Options.rage_aimbot_key))
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	viewangle = cmd->viewangles;

	sync_config(weapon);

	CCSWeaponInfo* data = weapon->GetCSWeaponData();

	if (!valid_wep)
		return;

	if (!weapon->CanFire())
		return;

	int BestTarget = GetBestTarget();

	if (BestTarget == -1) return;

	best_target = BestTarget;

	C_BasePlayer* ent = (C_BasePlayer*)g_EntityList->GetClientEntity(BestTarget);

	if (!ent) return;
	if (ent->IsDormant()) return;

	Vector point;

	int hitbox = GetBestHitbox(ent);
	if (hitbox == -1) return;

	if (g_Options.rage_aimbot_baim == 1)
		baim = true;
	else
		baim = false;

	GetHitboxPos(ent, hitbox, point);

	if (point.Length() == 0) return;

	QAngle angles;

	Vector src = g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset();

	VectorAngles4(point - src, angles);

	Math::ClampAngles(angles);

	if (min_damage > ent->m_iHealth())
		min_damage = ent->m_iHealth();

	float damage = 0;
	CanHit(ent, point, &damage);

	if (!g_Options.rage_aimbot_autofire)
	{
		if (!g_Options.rage_aimbot_silent)
			g_EngineClient->SetViewAngles(angles);
		cmd->viewangles = angles;
	}

	if (damage >= min_damage && g_Options.rage_aimbot_autofire)
	{
		cmd->viewangles = angles;
		if (!g_Options.rage_aimbot_silent)
			g_EngineClient->SetViewAngles(angles);

		if (weapon->IsSniper(data) && !g_LocalPlayer->m_bIsScoped())
			cmd->buttons |= IN_ATTACK2;

		if (!HitChance(angles, ent, hit_chance))
			return;
		cmd->buttons |= IN_ATTACK;


		DoNoRecoil(cmd);
	}

}

bool TargetMeetsRequirements(C_BasePlayer* pEntity)
{
	// Is a valid player
	if (pEntity && pEntity->IsDormant() == false && pEntity->IsAlive() && pEntity->EntIndex() != g_LocalPlayer->EntIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)ClassId::ClassId_CCSPlayer && g_EngineClient->GetPlayerInfo(pEntity->EntIndex(), &pinfo))
		{
			// Team Check
		/*	if ( !g_Options.rage_frindlyfire && pEntity->m_iTeamNum( ) != g_LocalPlayer->m_iTeamNum( ) )
			{
				if ( !pEntity->m_bGunGameImmunity( ) )
					return true;
			}*/
				
			if ( g_Options.rage_frindlyfire )
			{
				if ( !pEntity->m_bGunGameImmunity( ) )
					return true;
			}
			else
			{
				if ( pEntity->m_iTeamNum( ) != g_LocalPlayer->m_iTeamNum( ) )
				{
					if ( !pEntity->m_bGunGameImmunity( ) )
						return true;
				}
			}

		}
	}

	// They must have failed a requirement
	return false;
}

FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

int C_Ragebot::GetBestTarget()
{
	int target = -1;
	float min_dist = 560.f;
	float minFoV = g_Options.rage_aimbot_fov;

	Vector angle;
	Vector src = g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset();

	QAngle new_ang;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (weapon->m_Item().m_iItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER)
	{
		for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			C_BasePlayer *pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

			if (!pEntity)
				continue;

			if (!pEntity->IsPlayer())
				continue;

			if (g_Options.Players[pEntity->EntIndex()].is_friendly)
				continue;

			if (TargetMeetsRequirements(pEntity))
			{
				int NewHitBox = 6;
				if (NewHitBox >= 0)
				{
					GetHitboxPos(pEntity, NewHitBox, angle);
					VectorAngles4(angle - src, new_ang);

					float dist = Math::VectorDistance(g_LocalPlayer->GetEyePos(), pEntity->GetHitboxPos(Hitboxes::HITBOX_PELVIS));
					if (dist < min_dist)
					{
						min_dist = dist;
						target = i;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < g_EntityList->GetHighestEntityIndex(); i++)
		{
			C_BasePlayer *pEntity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);


			if (!pEntity)
				continue;

			if (!pEntity->IsPlayer())
				continue;

			if (g_Options.Players[pEntity->EntIndex()].is_friendly)
				continue;

			if (TargetMeetsRequirements(pEntity))
			{
				int NewHitBox = GetBestHitbox(pEntity);
				if (NewHitBox >= 0)
				{
					GetHitboxPos(pEntity, NewHitBox, angle);
					VectorAngles4(angle - src, new_ang);

					float damage = 0;
					CanHit(pEntity, angle, &damage);
					if (damage > min_damage)
					{
						float fov = Math::GetFOV(viewangle, new_ang);
						if (fov < minFoV)
						{
							minFoV = fov;
							target = i;
						}
					}
				}
			}
		}
	}
	return target;
}

bool C_Ragebot::GetHitboxPos(C_BasePlayer* entity, int hitbox, Vector &output)
{
	if (hitbox >= 20)
		return false;

	const model_t *model = entity->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudioModel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[128];
	if (!entity->SetupBones(matrix, 128, 0x100, entity->m_flSimulationTime()))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

int C_Ragebot::GetBestHitbox(C_BasePlayer* ent)
{
	int hitbox = -1;
	static float best_damage = 1;

	std::vector<int> hitboxes;

	if (g_Options.rage_aimbot_head)
		hitboxes.push_back(Hitboxes::HITBOX_HEAD);
	if (g_Options.rage_aimbot_chest)
		hitboxes.push_back(Hitboxes::HITBOX_CHEST);
	if (g_Options.rage_aimbot_arms)
	{
		hitboxes.push_back(Hitboxes::HITBOX_LEFT_HAND);
		hitboxes.push_back(Hitboxes::HITBOX_RIGHT_HAND);
	}
	if (g_Options.rage_aimbot_pelvis)
		hitboxes.push_back(Hitboxes::HITBOX_STOMACH);
	if (g_Options.rage_aimbot_legs)
	{
		hitboxes.push_back(Hitboxes::HITBOX_LEFT_CALF);
		hitboxes.push_back(Hitboxes::HITBOX_RIGHT_CALF);
	}
	if (g_Options.rage_aimbot_feet)
	{
		hitboxes.push_back(Hitboxes::HITBOX_LEFT_FOOT);
		hitboxes.push_back(Hitboxes::HITBOX_RIGHT_FOOT);
	}		
	bool chest = false;

	if (baim)
	{

		Vector Hitbox;
		if (GetHitboxPos(ent, Hitboxes::HITBOX_STOMACH, Hitbox))
		{
			float damage = 0.f;

			if (CanHit(ent, Hitbox, &damage))
			{
				if (damage > min_damage)
					return Hitboxes::HITBOX_STOMACH;
				else
					chest = true;

			}
			else
				chest = true;
		}
	}
	if (chest)
	{
		Vector Hitbox;
		if (GetHitboxPos(ent, Hitboxes::HITBOX_CHEST, Hitbox))
		{
			float damage = 0.f;

			if (CanHit(ent, Hitbox, &damage))
			{
				if (damage > min_damage)
					return Hitboxes::HITBOX_CHEST;
			}

		}
	}

	chest = false;

	for (auto i : hitboxes)
	{
		Vector Hitbox;
		if (GetHitboxPos(ent, i, Hitbox))
		{
			float damage = 0; 

			CanHit(ent, ent->GetHitboxPos(i), &damage);

			if (damage > best_damage)
			{
				best_damage = damage;
				hitbox = i;
			}
		}
	}
	best_damage = 0;

	hitboxes.clear();

	if (g_LocalPlayer->m_hActiveWeapon().Get()->m_Item().m_iItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER)
		hitbox = Hitboxes::HITBOX_PELVIS;

	return hitbox;
}