#include "obs_visuals.h"
#include "hooks.hpp"
#include "imgui/imgui.h"
#include "options.hpp"
#include "helpers/math.hpp"

obs_vis_player_t pl[64];

RECT GetBBox2( C_BaseEntity* ent )
{
	RECT rect {};
	auto collideable = ent->GetCollideable( );

	if ( !collideable )
		return rect;

	auto min = collideable->OBBMins( );
	auto max = collideable->OBBMaxs( );

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame( );

	Vector points[] = {
		Vector( min.x, min.y, min.z ),
		Vector( min.x, max.y, min.z ),
		Vector( max.x, max.y, min.z ),
		Vector( max.x, min.y, min.z ),
		Vector( max.x, max.y, max.z ),
		Vector( min.x, max.y, max.z ),
		Vector( min.x, min.y, max.z ),
		Vector( max.x, min.y, max.z )
	};

	Vector pointsTransformed[8];
	for ( int i = 0; i < 8; i++ ) {
		Math::VectorTransform( points[i], trans, pointsTransformed[i] );
	}

	Vector screen_points[8] = {};

	for ( int i = 0; i < 8; i++ ) {
		//if ( !Math::WorldToScreen(pointsTransformed[i], screen_points[i] ) )
		//	return rect;
		g_DebugOverlay->ScreenPosition( pointsTransformed[i], screen_points[i] );
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for ( int i = 1; i < 8; i++ ) {
		if ( left > screen_points[i].x )
			left = screen_points[i].x;
		if ( top < screen_points[i].y )
			top = screen_points[i].y;
		if ( right < screen_points[i].x )
			right = screen_points[i].x;
		if ( bottom > screen_points[i].y )
			bottom = screen_points[i].y;
	}
	return RECT { ( long ) left, ( long ) top, ( long ) right, ( long ) bottom };
}

void OBS::OnPaintTraverse( )
{
	for ( int i = 0; i <= g_EntityList->GetHighestEntityIndex( ); i++ ) {

		auto ent = C_BasePlayer::GetPlayerByIndex( i );

		pl[i].valid_ent = false;

		if ( !ent )
			continue;
		if ( !ent->IsPlayer( ) )
			continue;
		if ( !ent->IsAlive( ) )
			continue;
		if ( ent->IsDormant( ) )
			continue;
		if ( ent->m_iTeamNum( ) == g_LocalPlayer->m_iTeamNum( ) )
			continue;

		pl[i].bBox = GetBBox2( C_BasePlayer::GetPlayerByIndex( i ) );
		pl[i].valid_ent = true;
	}
}

void OBS::OnPreset( )
{
	if ( g_EngineClient->IsConnected( ) || g_EngineClient->IsInGame( ) )
	{
		for ( int i = 0; i <= g_EntityList->GetHighestEntityIndex( ); i++ )
		{

			if ( !pl[i].valid_ent )
				continue;

			if ( g_Options.obs_box || g_Options.obs_name )
				OBS::DrawBox( C_BasePlayer::GetPlayerByIndex(i) );

		}
	}
}

void OBS::DrawBox( C_BasePlayer* ent )
{
	if ( !ent )
		return;

	RECT bBox = pl[ent->EntIndex( )].bBox;

	float
		length_horizontal = ( bBox.right - bBox.left ) * 0.2f,
		length_vertical = ( bBox.bottom - bBox.top ) * 0.2f;

	if ( g_Options.obs_box )
	{
		ImGui::GetWindowDrawList( )->AddRect( ImVec2( bBox.left, bBox.bottom ), ImVec2( bBox.right, bBox.top ), ImColor( 240, 30, 30 ), 4, 15, 1 );
		ImGui::GetWindowDrawList( )->AddRect( ImVec2( bBox.left - 1, bBox.bottom - 1 ), ImVec2( bBox.right + 1, bBox.top + 1 ), ImColor( 30, 30, 30 ), 4, 15, 1 );
		ImGui::GetWindowDrawList( )->AddRect( ImVec2( bBox.left + 1, bBox.bottom + 1 ), ImVec2( bBox.right - 1, bBox.top - 1 ), ImColor( 30, 30, 30 ), 4, 15, 1 );
	}

	if ( g_Options.obs_name )
	{
		auto name = ent->GetPlayerInfo( ).szName;

		ImGui::GetWindowDrawList( )->AddText( ImVec2(bBox.left, bBox.bottom - 16 ), ImColor( 230, 230, 230 ), name );
	}

	if ( g_Options.obs_health )
	{
		int health = ent->m_iHealth( );
		if ( health > 100 )
			health = 100;

		float box_h = ( float ) fabs( bBox.bottom - bBox.top );
		float off = 8;

		auto height = box_h - ( ( ( box_h * health ) / 100 ) );

		int x = bBox.left - off;
		int y = bBox.bottom;
		int w = 4;
		int h = box_h;

		ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( x, y ), ImVec2( x + w, y + h ), ImColor( 30, 30, 30, 140 ), 3, 15 );
		ImGui::GetWindowDrawList( )->AddRectFilled( ImVec2( x + 1, y + height + 1 ), ImVec2( x + w - 1, y + h - 1 ), ImColor( ( 255 - health * int( 2.55f ) ), ( health* int( 2.55f ) ), 0, 240 ), 3, 15 );
	}


}