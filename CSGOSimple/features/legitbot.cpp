#include "legitbot.h"
#include "../options.hpp"
#include "LagComp.h"
#include "../helpers/math.hpp"
#include <random>

#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10

using LineGoesThroughSmokeFn = bool( __cdecl* )( Vector , Vector , int16_t );
LineGoesThroughSmokeFn LineGoesThroughSmoke;

bool isBehindSmoke( Vector src , Vector rem )
{
	static auto func = Utils::FindPattern( "client_panorama.dll" , ( PBYTE )"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0" , "xxxxxxxx????xxx" );

	LineGoesThroughSmoke = ( LineGoesThroughSmokeFn ) ( func );

	return LineGoesThroughSmoke( src , rem , true );
}

void SinCos2( float a , float* s , float*c )
{
	*s = sin( a );
	*c = cos( a );
}

void AngleVectors(const QAngle& angles, Vector* forward)
{
	float sp, sy, cp, cy;

	DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
	DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
}

inline void clamp_angles(QAngle& angles)
{
	if (angles.pitch > 89.0f) angles.pitch = 89.0f;
	else if (angles.pitch < -89.0f) angles.pitch = -89.0f;

	if (angles.yaw > 180.0f) angles.yaw = 180.0f;
	else if (angles.yaw < -180.0f) angles.yaw = -180.0f;

	angles.roll = 0;
}
inline void normalize_angles(QAngle& angles)
{
	for (auto i = 0; i < 3; i++) {
		while (angles[i] < -180.0f) angles[i] += 360.0f;
		while (angles[i] >  180.0f) angles[i] -= 360.0f;
	}
}

inline bool sanitize_angles(QAngle &angles)
{
	QAngle temp = angles;
	normalize_angles(temp);
	clamp_angles(temp);

	if (!isfinite(temp.pitch) ||
		!isfinite(temp.yaw) ||
		!isfinite(temp.roll))
		return false;

	angles = temp;

	return true;
}

void AngleVectors7( const QAngle &angles , Vector& forward , Vector& right , Vector& up )
{
	float sr , sp , sy , cr , cp , cy;

	SinCos2( DEG2RAD( angles[1] ) , &sy , &cy );
	SinCos2( DEG2RAD( angles[0] ) , &sp , &cp );
	SinCos2( DEG2RAD( angles[2] ) , &sr , &cr );

	forward.x = ( cp * cy );
	forward.y = ( cp * sy );
	forward.z = ( -sp );
	right.x = ( -1 * sr * sp * cy + -1 * cr * -sy );
	right.y = ( -1 * sr * sp * sy + -1 * cr *  cy );
	right.z = ( -1 * sr * cp );
	up.x = ( cr * sp * cy + -sr * -sy );
	up.y = ( cr * sp * sy + -sr * cy );
	up.z = ( cr * cp );
}

#define PI 3.14159265358979323846f
#define PI_F	((float)(PI)) 

inline void RandomSeed2( int seed )
{
	static auto fn = ( decltype( &RandomSeed2 ) ) ( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ) , "RandomSeed" ) );

	return fn( seed );
}
Vector CrossProduct2( const Vector &a , const Vector &b )
{
	return Vector( a.y*b.z - a.z*b.y , a.z*b.x - a.x*b.z , a.x*b.y - a.y*b.x );
}
inline float RandomFloat2( float min , float max )
{
	static auto fn = ( decltype( &RandomFloat2 ) ) ( GetProcAddress( GetModuleHandleA( "vstdlib.dll" ) , "RandomFloat" ) );
	return fn( min , max );
}

void VectorAngles7( const Vector& forward , Vector& up , QAngle& angles )
{
	Vector left = CrossProduct2( up , forward );
	left.NormalizeInPlace( );

	float forwardDist = forward.Length2D( );

	if ( forwardDist > 0.001f )
	{
		angles.pitch = atan2f( -forward.z , forwardDist ) * 180 / PI_F;
		angles.yaw = atan2f( forward.y , forward.x ) * 180 / PI_F;

		float upZ = ( left.y * forward.x ) - ( left.x * forward.y );
		angles.roll = atan2f( left.z , upZ ) * 180 / PI_F;
	}
	else
	{
		angles.pitch = atan2f( -forward.z , forwardDist ) * 180 / PI_F;
		angles.yaw = atan2f( -left.x , left.y ) * 180 / PI_F;
		angles.roll = 0;
	}
}

bool HitChance2( QAngle angles , C_BasePlayer *ent , float chance )
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon( ).Get( );

	if ( !weapon )
		return false;

	Vector forward , right , up;
	Vector src = g_LocalPlayer->GetEyePos( );
	AngleVectors7( angles , forward , right , up );

	int cHits = 0;
	int cNeededHits = static_cast<int>( 150.f * ( chance / 100.f ) );

	weapon->UpdateAccuracyPenalty( );
	float weap_spread = weapon->GetSpread( );
	float weap_inaccuracy = weapon->GetInaccuracy( );

	for ( int i = 0; i < 150; i++ )
	{
		float a = RandomFloat2( 0.f , 1.f );
		float b = RandomFloat2( 0.f , 2.f * PI_F );
		float c = RandomFloat2( 0.f , 1.f );
		float d = RandomFloat2( 0.f , 2.f * PI_F );

		float inaccuracy = a * weap_inaccuracy;
		float spread = c * weap_spread;

		if ( weapon->m_Item( ).m_iItemDefinitionIndex( ) == 64 )
		{
			a = 1.f - a * a;
			a = 1.f - c * c;
		}

		Vector spreadView( ( cos( b ) * inaccuracy ) + ( cos( d ) * spread ) , ( sin( b ) * inaccuracy ) + ( sin( d ) * spread ) , 0 ) , direction;

		direction.x = forward.x + ( spreadView.x * right.x ) + ( spreadView.y * up.x );
		direction.y = forward.y + ( spreadView.x * right.y ) + ( spreadView.y * up.y );
		direction.z = forward.z + ( spreadView.x * right.z ) + ( spreadView.y * up.z );
		direction.Normalized( );

		QAngle viewAnglesSpread;
		VectorAngles7( direction , up , viewAnglesSpread );
		Math::ClampAngles( viewAnglesSpread );

		Vector viewForward;
		Math::AngleVectors( viewAnglesSpread , viewForward );
		viewForward.NormalizeInPlace( );

		viewForward = src + ( viewForward * weapon->GetCSWeaponData( )->flRange );

		trace_t tr;
		Ray_t ray;

		ray.Init( src , viewForward );
		g_EngineTrace->ClipRayToEntity( ray , MASK_SHOT | CONTENTS_GRATE , ent , &tr );

		if ( tr.hit_entity == ent )
			++cHits;

		if ( static_cast<int>( ( static_cast<float>( cHits ) / 150.f ) * 100.f ) >= chance )
			return true;

		if ( ( 150 - i + cHits ) < cNeededHits )
			return false;
	}
	return false;
}

void C_Legitbot::OnCreateMove(CUserCmd* cmd)
{
	if (!g_Options.legit_enable)
		return;

	if (g_Options.legit_backtrack)
		Backtrack(cmd);

	if (g_LocalPlayer)
	{
		if (!g_LocalPlayer->IsAlive()) return;

		auto active_weapon = g_LocalPlayer->m_hActiveWeapon();
		if (!active_weapon) return;

		C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(active_weapon);
		if (!weapon) return;

		SyncSettings(weapon);

		if (bInvalidWeapon) return;

		if (weapon->m_iClip1() == 0) return;

		view_angle = cmd->viewangles;

		if (GetAsyncKeyState(iKey) || !iKey)
			Aimbot(cmd);


		if ( bTrigger && ( GetAsyncKeyState( trigger_key ) || !trigger_key ) )
		{
			Triggerbot( cmd );
		}
		else
			iDelay = 0;
	}
}

float random_number_range(float min, float max)
{
	std::random_device device;
	std::mt19937 engine(device());
	std::uniform_real_distribution<> distribution(min, max);
	return static_cast< float >(distribution(engine));
}

QAngle C_Legitbot::get_randomized_recoil(C_BasePlayer *local)
{
	QAngle compensatedAngles = (local->m_aimPunchAngle() * 2.0f) * (fRCS / 100.0f);
	sanitize_angles(compensatedAngles);

	return (local->m_iShotsFired() > 1 ? compensatedAngles : QAngle(0.0f, 0.0f, 0.0f));
}

QAngle C_Legitbot::get_randomized_angles(C_BasePlayer *local)
{
	QAngle randomizedValue = QAngle(0.0f, 0.0f, 0.0f);

	float randomRate = random_number_range(-1, 1);
	float randomDeviation = random_number_range(-1, 1);

	switch (rand() % 2)
	{
	case 0:
		randomizedValue.pitch = (randomRate * cos(randomDeviation));
		randomizedValue.yaw = (randomRate * cos(randomDeviation));
		randomizedValue.roll = (randomRate * cos(randomDeviation));
		break;
	case 1:
		randomizedValue.pitch = (randomRate * sin(randomDeviation));
		randomizedValue.yaw = (randomRate * sin(randomDeviation));
		randomizedValue.roll = (randomRate * sin(randomDeviation));
		break;
	}

	sanitize_angles(randomizedValue);

	return (local->m_iShotsFired() > 1 ? randomizedValue : QAngle(0.0f, 0.0f, 0.0f));
}

#define M_RADPI 57.295779513082f
inline void compute_angle(const Vector &source, const Vector &destination, QAngle& angles)
{
	Vector delta = source - destination;
	angles.pitch = static_cast< float >(asin(delta.z / delta.Length()) * M_RADPI);
	angles.yaw = static_cast< float >(atan(delta.y / delta.x) * M_RADPI);
	angles.roll = 0.0f;

	if (delta.x >= 0.0f)
		angles.yaw += 180.0f;
}
inline QAngle compute_angle(const Vector &source, const Vector &destination)
{
	QAngle angles;

	Vector delta = source - destination;
	angles.pitch = static_cast< float >(asin(delta.z / delta.Length()) * M_RADPI);
	angles.yaw = static_cast< float >(atan(delta.y / delta.x) * M_RADPI);
	angles.roll = 0.0f;

	if (delta.x >= 0.0f)
		angles.yaw += 180.0f;
	
	return angles;
}

void C_Legitbot::Aimbot(CUserCmd* cmd)
{
	Vector destination;
	int best_target = GetTarget(cmd, destination);

	if (best_target == -1) return;

	C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(best_target);
	if (!entity)
		return;

	QAngle hitboxpos; 
	compute_angle(g_LocalPlayer->GetEyePos(), destination, hitboxpos);

	if ( g_Options.legit_smoke_check && isBehindSmoke( g_LocalPlayer->GetEyePos( ) , destination ) )
		return;

	sanitize_angles(hitboxpos);

	hitboxpos -= get_randomized_recoil(g_LocalPlayer);
	hitboxpos += get_randomized_angles(g_LocalPlayer);

	QAngle delta_angle = view_angle - hitboxpos;
	sanitize_angles(delta_angle);

	float randomSmoothing = 1.0f;

	if (randomSmoothing > 1.0f)
		randomSmoothing = random_number_range(randomSmoothing / 10.0f, 1.0f);

	QAngle final_angle = view_angle - delta_angle / fSmooth * randomSmoothing;
	sanitize_angles(final_angle);

	cmd->viewangles = final_angle;
	g_EngineClient->SetViewAngles(cmd->viewangles);
	bIsLocked = true;

}

bool get_hitbox_pos(C_BasePlayer* entity, int hitbox, Vector &output)
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

float get_fov(const QAngle &viewAngles, const QAngle &aimAngles)
{
	Vector ang, aim;
	AngleVectors(viewAngles, &aim);
	AngleVectors(aimAngles, &ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}


bool C_Legitbot::GetHitbox(C_BasePlayer* entity, Vector& destination)
{
	bestHitbox = -1;
	float best_fov = fFov;

	static const std::vector<int> hitboxes = { (int)Hitboxes::HITBOX_HEAD, (int)Hitboxes::HITBOX_CHEST, (int)Hitboxes::HITBOX_STOMACH };

	for (auto hitbox : hitboxes)
	{
		Vector temp;
		if (!get_hitbox_pos(entity, hitbox, temp))
			continue;

		float fov = get_fov(view_angle, compute_angle(g_LocalPlayer->GetEyePos(), temp));
		if (fov < best_fov)
		{
			best_fov = fov;
			switch (iBone)
			{
			case 0:
				bestHitbox = hitbox;
				break;
			case 1: 
				bestHitbox = Hitboxes::HITBOX_HEAD;
				break;
			case 2:
				bestHitbox = Hitboxes::HITBOX_CHEST;
				break;
			}
		}
	}

	if (bestHitbox != -1)
	{
		if (get_hitbox_pos(entity, bestHitbox, destination))
			return true;
	}

	return false;

}

int C_Legitbot::GetTarget(CUserCmd* cmd, Vector& destination)
{
	int best_target = -1;
	float best_fov = fFov;

	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		C_BasePlayer* entity = (C_BasePlayer*)g_EntityList->GetClientEntity(i);
		if (!entity
			|| entity == g_LocalPlayer
			|| entity->IsDormant()
			|| entity->m_lifeState() != LIFE_ALIVE
			|| entity->GetClientClass()->m_ClassID != (int)ClassId::ClassId_CCSPlayer
			|| ( !g_Options.legit_aimbot_friendlyfire && entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			|| !(entity->m_fFlags() & FL_ONGROUND))
			continue;

		Vector hitbox;
		if (!GetHitbox(entity, hitbox))
			continue;

		float fov = Math::GetFOV(view_angle + (g_LocalPlayer->m_aimPunchAngle() * 2.0f), compute_angle(g_LocalPlayer->GetEyePos(), hitbox));
		if (fov < best_fov)
		{
			if (g_LocalPlayer->CanSeePlayer(entity, bestHitbox))
			{
				best_fov = fov;
				destination = hitbox;
				best_target = i;
			}
		}
	}
	return best_target;
}

void C_Legitbot::Triggerbot(CUserCmd* cmd)
{
	Vector src, dst, forward;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	AngleVectors(cmd->viewangles, &forward);

	forward *= g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData()->flRange;
	filter.pSkip = g_LocalPlayer;
	src = g_LocalPlayer->GetEyePos();
	dst = src + forward;

	ray.Init(src, dst);

	g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

	if (!tr.hit_entity)
		return;

	if ( !g_Options.legit_aimbot_friendlyfire && tr.hit_entity->GetBaseEntity()->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		return;

	int hitgroup = tr.hitgroup;
	bool didHit = false;

	
	if (g_Options.legit_trigger_head)
	{
		if (hitgroup == HITGROUP_HEAD) didHit = true;
	}
	if (g_Options.legit_trigger_body)
	{
		if (hitgroup == HITGROUP_CHEST || hitgroup == HITGROUP_STOMACH) didHit = true;
	}
	if (g_Options.legit_trigger_arms)
	{
		if (hitgroup == HITGROUP_LEFTARM || hitgroup == HITGROUP_RIGHTARM) didHit = true;
	}
	if (g_Options.legit_trigger_legs)
	{
		if (hitgroup == HITGROUP_LEFTLEG || hitgroup == HITGROUP_RIGHTLEG) didHit = true;
	}

	
	if (iDelay >= trigger_delay && didHit)
	{
		if ( !HitChance2( cmd->viewangles , ( C_BasePlayer* ) tr.hit_entity , hitchance ) )
			return;

		if ( g_Options.legit_smoke_check && isBehindSmoke( g_LocalPlayer->GetEyePos() , reinterpret_cast<C_BasePlayer*>(tr.hit_entity)->GetBonePos(8) ) )
			return;

		iDelay = 0;
		cmd->buttons |= IN_ATTACK;
	}
	iDelay++;

}

void C_Legitbot::SyncSettings(C_BaseCombatWeapon* weapon)
{

	CCSWeaponInfo* data = weapon->GetCSWeaponData();

	if (g_Options.legit_mode == 1)
	{
		if (weapon->m_Item().m_iItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_TASER || weapon->IsKnife(data))
			bInvalidWeapon = true;
		else
		{
			bInvalidWeapon = false;
			bTrigger = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].trigger;
			iBone = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].hitbox;
			fFov = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].fov;
			fRCS = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].rcs;
			fSmooth = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].smooth;
			iKey = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].key;
			trigger_key = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].trigger_key;
			trigger_delay = g_Options.advanced_legit[weapon->m_Item().m_iItemDefinitionIndex()].trigger_delay;
			hitchance = g_Options.advanced_legit[weapon->m_Item( ).m_iItemDefinitionIndex( )].trigger_hc;
		}
	}
	else if (g_Options.legit_mode == 0)
	{
		if (weapon->IsSniper(data))
		{
			bTrigger = g_Options.legit_trigger_sniper;
			iBone = g_Options.legit_sniper_bone;
			fFov = g_Options.legit_sniper_fov;
			fRCS = g_Options.legit_sniper_rcs;
			fSmooth = g_Options.legit_sniper_smooth;
			iKey = g_Options.legit_sniper_key;
			fRandS = g_Options.legit_sniper_rand_smooth;
			trigger_delay = g_Options.legit_trigger_delay;
			trigger_key = g_Options.legit_trigger_key;
			hitchance = g_Options.trigger_hc;
			bInvalidWeapon = false;
		}
		else if (weapon->IsRifle(data))
		{
			bTrigger = g_Options.legit_trigger_rifle;
			iBone = g_Options.legit_rifle_bone;
			fFov = g_Options.legit_rifle_fov;
			fRCS = g_Options.legit_rifle_rcs;
			fSmooth = g_Options.legit_rifle_smooth;
			iKey = g_Options.legit_rifle_key;
			fRandS = g_Options.legit_rifle_rand_smooth;
			trigger_delay = g_Options.legit_trigger_delay;
			trigger_key = g_Options.legit_trigger_key;
			hitchance = g_Options.trigger_hc;
			bInvalidWeapon = false;
		}
		else if (weapon->IsPistol(data))
		{
			bTrigger = g_Options.legit_trigger_pistol;
			iBone = g_Options.legit_pistol_bone;
			fFov = g_Options.legit_pistol_fov;
			fRCS = g_Options.legit_pistol_rcs;
			fSmooth = g_Options.legit_pistol_smooth;
			iKey = g_Options.legit_pistol_key;
			fRandS = g_Options.legit_pistol_rand_smooth;
			trigger_delay = g_Options.legit_trigger_delay;
			trigger_key = g_Options.legit_trigger_key;
			hitchance = g_Options.trigger_hc;
			bInvalidWeapon = false;
		}
		else
		{
			bInvalidWeapon = true;
		}
	}
}

void C_Legitbot::Backtrack(CUserCmd* cmd)
{
	C_LagComp::Get().LegitBacktrack(cmd);
}