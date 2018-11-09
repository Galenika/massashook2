#include "hooks.hpp"
#include <intrin.h>  

#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/misc.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "features/Skinchanger.h"
#include "features/legitbot.h"
#include "features/GrenadePredict.h"
#include "helpers/math.hpp"
#include "GameEvents.h"
#include "EnginePrediction.h"
#include "features\Ragebot.h"
#include "SpoofedConvar.h"
#include "Resolver.h"
#include "obs_visuals.h"
#include "steam_sdk/isteamfriends.h"
#pragma intrinsic(_ReturnAddress)  
vgui::HFont font;

std::unique_ptr<C_EventListener>          g_EventListener = nullptr;

void ConColorMsg( Color const &color , const char* buf , ... )
{
	using ConColFn = void( __stdcall* )( Color const & , const char* , ... );
	auto ConCol = reinterpret_cast< ConColFn >( ( GetProcAddress( GetModuleHandle( L"tier0.dll" ) , "?ConColorMsg@@YAXABVColor@@PBDZZ" ) ) );
	ConCol( color , buf );
}

typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void ConMsg( const char* msg , ... )
{
	if ( msg == nullptr )
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = ( MsgFn ) GetProcAddress( GetModuleHandleA( "tier0.dll" ) , "Msg" ); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer[989];
	va_list list; //Normal varargs stuff http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments
	va_start( list , msg );
	vsprintf( buffer , msg , list );
	va_end( list );
	fn( buffer , list ); //Calls the function, we got the address above.
}

namespace Hooks
{
	vfunc_hook hlclient_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook clientmode_hook;
	vfunc_hook sv_cheats;
	vfunc_hook steamgamecoordinator;
	vfunc_hook engine_sound;
	vfunc_hook render_view;
	vfunc_hook sound_hook;

	void Initialize( )
	{
		hlclient_hook.setup( g_CHLClient, "client_panorama.dll" );
		direct3d_hook.setup( g_D3DDevice9, "shaderapidx9.dll" );
		vguipanel_hook.setup( g_VGuiPanel );
		vguisurf_hook.setup( g_VGuiSurface );
		mdlrender_hook.setup( g_MdlRender, "engine.dll" );
		clientmode_hook.setup( g_ClientMode, "client_panorama.dll" );
		ConVar* sv_cheats_con = g_CVar->FindVar( "sv_cheats" );
		sv_cheats.setup( sv_cheats_con );
		render_view.setup( g_RenderView );
		sound_hook.setup( g_EngineSound );


		hlclient_hook.hook_index( 37, hkFrameStageNotify );
		if ( !g_Options.light ) {
			hlclient_hook.hook_index( 22, hkCreateMove_Proxy );
			vguipanel_hook.hook_index( 41, hkPaintTraverse );
			mdlrender_hook.hook_index( index::DrawModelExecute, hkDrawModelExecute );
			clientmode_hook.hook_index( index::DoPostScreenSpaceEffects, hkDoPostScreenEffects );
			clientmode_hook.hook_index( index::OverrideView, hkOverrideView );
			clientmode_hook.hook_index( 35, hkGetViewModelFOV );
			render_view.hook_index( 9, hkSceneEnd );
		}

		vguisurf_hook.hook_index( 67, hkLockCursor );

		sound_hook.hook_index( 5, hkEmitSound1 );

		sv_cheats.hook_index( index::SvCheatsGetBool, hkSvCheatsGetBool );

		Visuals::CreateFonts( );

		direct3d_hook.hook_index( index::EndScene, hkEndScene );
		direct3d_hook.hook_index( index::Reset, hkReset );

		g_EventListener = std::make_unique<C_EventListener>( );
		std::cout << "Done!" << std::endl;

	}
	//-------------------------------------------------------------------------------

	void Shutdown( )
	{
		hlclient_hook.unhook_all( );
		direct3d_hook.unhook_all( );
		vguipanel_hook.unhook_all( );
		vguisurf_hook.unhook_all( );
		mdlrender_hook.unhook_all( );
		clientmode_hook.unhook_all( );
		render_view.unhook_all( );
		sound_hook.unhook_all( );

		Glow::Get( ).Shutdown( );

		Visuals::DestroyFonts( );
	}

	void __stdcall hkEmitSound1( IRecipientFilter& filter , int iEntIndex , int iChannel , const char* pSoundEntry , unsigned int nSoundEntryHash , const char *pSample , float flVolume , int nSeed , float flAttenuation , int iFlags , int iPitch , const Vector* pOrigin , const Vector* pDirection , void* pUtlVecOrigins , bool bUpdatePositions , float soundtime , int speakerentity , int unk )
	{
		static auto ofunc = sound_hook.get_original<EmitSound1>( 5 );


		if ( !strcmp( pSoundEntry , "UIPanorama.popup_accept_match_beep" ) )
		{
			if ( g_Options.misc_autoaccept )
			{
				static auto fnAccept = reinterpret_cast< bool( __stdcall* )( const char* ) >( Utils::PatternScan( GetModuleHandleA( "client_panorama.dll" ) , "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" ) );

				if ( fnAccept )
				{

					fnAccept( "" );

					//This will flash the CSGO window on the taskbar
					//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
					FLASHWINFO fi;
					fi.cbSize = sizeof( FLASHWINFO );
					fi.hwnd = InputSys::Get( ).GetMainWindow( );
					fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
					fi.uCount = 0;
					fi.dwTimeout = 0;
					FlashWindowEx( &fi );
				}
			}
		}

		ofunc( g_EngineSound , filter , iEntIndex , iChannel , pSoundEntry , nSoundEntryHash , pSample , flVolume , nSeed , flAttenuation , iFlags , iPitch , pOrigin , pDirection , pUtlVecOrigins , bUpdatePositions , soundtime , speakerentity , unk );

	}

	typedef void( __thiscall* LockCursor )( void* );
	LockCursor oLockCursor;

	void __stdcall hkLockCursor( )
	{
		oLockCursor = Hooks::vguisurf_hook.get_original<LockCursor>( 67 );

		if ( Menu::Get( ).IsVisible( ) )
		{
			g_VGuiSurface->UnlockCursor( );
			return;
		}

		oLockCursor( g_VGuiSurface );
	}

	void __fastcall hkSceneEnd( void* thisptr , void* edx )
	{
		auto ofunc = render_view.get_original<SceneEndFn>( 9 );

		bool update = false;

		if ( !g_LocalPlayer || !g_EngineClient->IsConnected( ) || !g_EngineClient->IsInGame( ) )
		{
			update = true;
			return ofunc( thisptr );
		}

		if ( g_Options.misc_thirdperson && g_Options.misc_legit_aa )
		{
			static IMaterial* mat = g_MatSystem->FindMaterial( "debug/debugdrawflat" , TEXTURE_GROUP_MODEL );
			if ( !mat ) 
				return ofunc( thisptr );

			for ( int i = 1; i < g_GlobalVars->maxClients; ++i )
			{
				auto ent = static_cast<C_BasePlayer*>( g_EntityList->GetClientEntity( i ) );
				if ( ent && ent->IsAlive( ) && !ent->IsDormant( ) )
				{
					if ( ent == g_LocalPlayer )
					{
						QAngle oAngle = ent->m_angEyeAngles( );

						ent->SetAngle2( QAngle( oAngle.pitch ,
												  g_Options.Real_yaw ,
												  oAngle.roll ) );

						g_RenderView->SetColorModulation( new const float[3] { 0.9f, 0.9f, 0.9f } );

						mat->IncrementReferenceCount( );
						mat->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ , false );	

						g_MdlRender->ForcedMaterialOverride( mat );
						ent->DrawModel( 0x1 , 255 );
						g_MdlRender->ForcedMaterialOverride( nullptr );
						ent->SetAngle2( oAngle );
					}
				}
			}
		}

		if ( g_Options.update_world || update && !g_Options.bypass_obs)
		{

			static auto fog_enableskybox = g_CVar->FindVar( "fog_enableskybox" );
			static auto fog_override = g_CVar->FindVar( "fog_override" );
			static auto fog_enable = g_CVar->FindVar( "fog_enable" );
			static auto draw_specific_static_prop = g_CVar->FindVar( "r_DrawSpecificStaticProp" );

			fog_enableskybox->m_nFlags &= ~FCVAR_CHEAT;
			fog_override->m_nFlags &= ~FCVAR_CHEAT;
			fog_enable->m_nFlags &= ~FCVAR_CHEAT;
			draw_specific_static_prop->m_nFlags &= ~FCVAR_CHEAT;

			// set values
			fog_enable->SetValue( 0.0f );
			fog_enableskybox->SetValue( 0.0f );
			fog_override->SetValue( 1.0f );
			draw_specific_static_prop->SetValue( 1.0f );

			for ( MaterialHandle_t i = g_MatSystem->FirstMaterial( ); i != g_MatSystem->InvalidMaterial( ); i = g_MatSystem->NextMaterial( i ) )
			{
				IMaterial *pMaterial = g_MatSystem->GetMaterial( i );
				if ( !pMaterial || pMaterial->IsErrorMaterial( ) )
					continue;

				if ( g_Options.misc_col_sky )
				{
					if ( strstr( pMaterial->GetTextureGroupName( ) , TEXTURE_GROUP_SKYBOX ) )
					{
						pMaterial->ColorModulate( g_Options.color_sky.r( ) / 255.f ,
												  g_Options.color_sky.g( ) / 255.f ,
												  g_Options.color_sky.b( ) / 255.f );
					}

				}
				else
				{
					if ( !g_Options.misc_nightmode )
					{
						if ( strstr( pMaterial->GetTextureGroupName( ) , TEXTURE_GROUP_SKYBOX ) )
						{
							pMaterial->ColorModulate( 1 , 1 , 1 );
						}
					}
				}
				if ( g_Options.misc_nightmode )
				{
					if ( !g_Options.misc_col_sky )
					{
						if ( strstr( pMaterial->GetTextureGroupName( ) , TEXTURE_GROUP_SKYBOX ) )
						{
							pMaterial->ColorModulate( g_Options.color_nightmode.r( ) / 255.f ,
													  g_Options.color_nightmode.g( ) / 255.f ,
													  g_Options.color_nightmode.b( ) / 255.f );
						}
					}

					if ( strstr( pMaterial->GetTextureGroupName( ) , "World textures" ) )
					{
						pMaterial->ColorModulate( g_Options.color_nightmode.r( ) / 255.f ,
												  g_Options.color_nightmode.g( ) / 255.f ,
												  g_Options.color_nightmode.b( ) / 255.f );
					}
					if ( strstr( pMaterial->GetTextureGroupName( ) , "StaticProp" ) )
					{
						pMaterial->ColorModulate( g_Options.color_nightmode.r( ) / 255.f ,
												  g_Options.color_nightmode.g( ) / 255.f ,
												  g_Options.color_nightmode.b( ) / 255.f );
					}
					if ( strstr( pMaterial->GetTextureGroupName( ) , "models/props/de_dust/palace_bigdome" ) )
					{
						pMaterial->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW , true );
					}
					if ( strstr( pMaterial->GetTextureGroupName( ) , "models/props/de_dust/palace_pillars" ) )
					{
						pMaterial->ColorModulate( g_Options.color_nightmode.r( ) / 255.f ,
												  g_Options.color_nightmode.g( ) / 255.f ,
												  g_Options.color_nightmode.b( ) / 255.f );
					}

					if ( strstr( pMaterial->GetTextureGroupName( ) , "Particle textures" ) )
					{
						pMaterial->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW , true );
					}

				}
				else
				{
					if ( !g_Options.misc_col_sky )
					{
						if ( strstr( pMaterial->GetTextureGroupName( ) , TEXTURE_GROUP_SKYBOX ) )
						{
							pMaterial->ColorModulate( 1 , 1 , 1 );
						}
					}

					if ( strstr( pMaterial->GetTextureGroupName( ) , "World textures" ) )
					{
						pMaterial->ColorModulate( 1 , 1 , 1 );
					}
					if ( strstr( pMaterial->GetTextureGroupName( ) , "StaticProp" ) )
					{
						pMaterial->ColorModulate( 1 , 1 , 1 );
					}
					if ( strstr( pMaterial->GetTextureGroupName( ) , "models/props/de_dust/palace_bigdome" ) )
					{
						pMaterial->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW , false );
					}
					if ( strstr( pMaterial->GetTextureGroupName( ) , "models/props/de_dust/palace_pillars" ) )
					{
						pMaterial->ColorModulate( 1 , 1 , 1 );
					}

					if ( strstr( pMaterial->GetTextureGroupName( ) , "Particle textures" ) )
					{
						pMaterial->SetMaterialVarFlag( MATERIAL_VAR_NO_DRAW , false );
					}
				}

			}
			update = false;
			g_Options.update_world = false;
		}

		ofunc( thisptr );
	}

	void __fastcall hkDrawWorldLists( IVRenderView * ecx , uintptr_t edx , void * mat_render_context , void * render_list , unsigned long flags , float water_adjust )
	{
		auto ofunc = render_view.get_original<DrawWorldList>( 12 );

		ofunc( g_RenderView , edx , mat_render_context , render_list , flags , water_adjust );
	}

	long __stdcall hkEndScene( IDirect3DDevice9* device )
	{
		auto oEndScene = direct3d_hook.get_original<EndScene>( index::EndScene );

		static uintptr_t gameoverlay_return_address = 0;

		if ( !gameoverlay_return_address ) {
			MEMORY_BASIC_INFORMATION info;
			VirtualQuery( _ReturnAddress( ), &info, sizeof( MEMORY_BASIC_INFORMATION ) );

			char mod[MAX_PATH];
			GetModuleFileNameA( ( HMODULE ) info.AllocationBase, mod, MAX_PATH );

			if ( strstr( mod, "gameoverlay" ) )
				gameoverlay_return_address = ( uintptr_t ) ( _ReturnAddress( ) );
		}

		if ( gameoverlay_return_address != ( uintptr_t ) ( _ReturnAddress( ) ) && g_Options.bypass_obs )
			return oEndScene( device );

		DWORD colorwrite , srgbwrite;
		device->GetRenderState( D3DRS_COLORWRITEENABLE , &colorwrite );
		device->GetRenderState( D3DRS_SRGBWRITEENABLE , &srgbwrite );

		//fix drawing without calling engine functons/cl_showpos
		device->SetRenderState( D3DRS_COLORWRITEENABLE , 0xffffffff );
		//removes the source engine color correction
		device->SetRenderState( D3DRS_SRGBWRITEENABLE , false );

		IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
		device->GetVertexDeclaration( &vertDec );
		device->GetVertexShader( &vertShader );

		Menu::Get( ).Render( );

		device->SetRenderState( D3DRS_COLORWRITEENABLE , colorwrite );
		device->SetRenderState( D3DRS_SRGBWRITEENABLE , srgbwrite );
		device->SetVertexDeclaration( vertDec );
		device->SetVertexShader( vertShader );

		g_SteamFriends->SetRichPresence( "MassasHook - CS:GO", "MassasHook - CS:GO" );

		return oEndScene( device );
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset( IDirect3DDevice9* device , D3DPRESENT_PARAMETERS* pPresentationParameters )
	{
		auto oReset = direct3d_hook.get_original<Reset>( index::Reset );

		Visuals::DestroyFonts( );
		Menu::Get( ).OnDeviceLost( );

		auto hr = oReset( device , pPresentationParameters );

		if ( hr >= 0 )
		{
			Menu::Get( ).OnDeviceReset( );
			Visuals::CreateFonts( );
		}

		return hr;
	}
	//--------------------------------------------------------------------------------
	QAngle m_oldangle;
	float m_oldforward;
	float m_oldsidemove;

	void StartFix( CUserCmd* cmd )
	{
		m_oldangle = cmd->viewangles;
		m_oldforward = cmd->forwardmove;
		m_oldsidemove = cmd->sidemove;
	}

	void EndFix( CUserCmd* cmd )
	{
		float yaw_delta = cmd->viewangles.yaw - m_oldangle.yaw;
		float f1;
		float f2;

		if ( m_oldangle.yaw < 0.f )
			f1 = 360.0f + m_oldangle.yaw;
		else
			f1 = m_oldangle.yaw;

		if ( cmd->viewangles.yaw < 0.0f )
			f2 = 360.0f + cmd->viewangles.yaw;
		else
			f2 = cmd->viewangles.yaw;

		if ( f2 < f1 )
			yaw_delta = abs( f2 - f1 );
		else
			yaw_delta = 360.0f - abs( f1 - f2 );
		yaw_delta = 360.0f - yaw_delta;

		cmd->forwardmove = cos( DEG2RAD( yaw_delta ) ) * m_oldforward + cos( DEG2RAD( yaw_delta + 90.f ) ) * m_oldsidemove;
		cmd->sidemove = sin( DEG2RAD( yaw_delta ) ) * m_oldforward + sin( DEG2RAD( yaw_delta + 90.f ) ) * m_oldsidemove;
	}

	void do_legitaa( CUserCmd* cmd, C_BaseCombatWeapon* weapon )
	{
		StartFix( cmd );
		QAngle oldAngle = cmd->viewangles;
		float oldForward = cmd->forwardmove;
		float oldSideMove = cmd->sidemove;

		if ( g_LocalPlayer->m_nMoveType( ) == MOVETYPE_LADDER )
			return;
		if ( cmd->buttons & IN_ATTACK )
			return;

		if ( weapon )
		{
			if ( weapon->IsGrenade() )
				return;
		}

		static int ChokedPackets = -1;
		ChokedPackets++;
		static bool yFlip;
		if ( ChokedPackets < 1 )
		{
			G::bSendPacket = true;
		}
		else
		{
			G::bSendPacket = false;
			yFlip ? cmd->viewangles.yaw += 90 : cmd->viewangles.yaw -= 90;
			ChokedPackets = -1;
		}
		yFlip = !yFlip;
		EndFix( cmd );
	}

	void __stdcall hkCreateMove( int sequence_number , float input_sample_frametime , bool active , bool& bSendPacket )
	{
		auto oCreateMove = hlclient_hook.get_original<CreateMove>( 22 );

		oCreateMove( g_CHLClient , sequence_number , input_sample_frametime , active );

		auto cmd = g_Input->GetUserCmd( sequence_number );
		auto verified = g_Input->GetVerifiedCmd( sequence_number );

		if ( !cmd || !cmd->command_number )
			return;

		G::view_angle = cmd->viewangles;
		G::bSendPacket = true;

		if ( g_LocalPlayer && !g_Options.PlayeName )
			g_Options.PlayeName = g_LocalPlayer->GetPlayerInfo( ).szName;

		if ( g_LocalPlayer )
		{
			if ( InputSys::Get( ).IsKeyDown( VK_TAB ) && g_Options.misc_rankreveal )
				Utils::RankRevealAll( );

			if ( g_Options.misc_legit_aa )
			{
				do_legitaa( cmd, g_LocalPlayer->m_hActiveWeapon( ).Get( ) );
			}

			if ( g_Options.esp_grenadeprediction )
				C_GrenadePrediction::Get( ).Tick( cmd->buttons );

			C_Misc::Get( ).OnCreateMove( cmd );

			C_EnginePrediction::Get( ).Start( cmd );
			{
				if ( g_Options.legit_enable )
					C_Legitbot::Get( ).OnCreateMove( cmd );

				if ( g_Options.rage_enable )
					C_Ragebot::Get( ).OnCreateMove( cmd );
			}
			C_EnginePrediction::Get( ).End( );


			if ( !G::bSendPacket )
				g_Options.Fake_yaw = cmd->viewangles.yaw;
			else
				g_Options.Real_yaw = cmd->viewangles.yaw;
		}

		auto view = cmd->viewangles;

		view.Normalize( );
		Math::ClampAngles( view );

		if ( view.pitch > 89.f || view.pitch < -89.f )
			return;
		if ( view.yaw > 180.f || view.yaw < -180.f )
			return;
		if ( view.roll != 0 )
			return;

		cmd->viewangles = view;


		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum( );

		bSendPacket = G::bSendPacket;

	}
	//--------------------------------------------------------------------------------
	__declspec( naked ) void __stdcall hkCreateMove_Proxy( int sequence_number , float input_sample_frametime , bool active )
	{
		__asm
		{
			push ebp
			mov  ebp , esp
			push ebx
			lea  ecx , [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkPaintTraverse( vgui::VPANEL panel , bool forceRepaint , bool allowForce )
	{
		static auto panelId = vgui::VPANEL { 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>( 41 );

		oPaintTraverse( g_VGuiPanel , panel , forceRepaint , allowForce );

		if ( !panelId )
		{
			const auto panelName = g_VGuiPanel->GetName( panel );
			if ( !strcmp( panelName , "FocusOverlayPanel" ) )
			{
				panelId = panel;
			}
		}
		else if ( panelId == panel )
		{


			if ( GetAsyncKeyState( g_Options.misc_tp_key ) & 1 )
				g_Options.misc_thirdperson = !g_Options.misc_thirdperson;

			if ( !g_EngineClient->IsInGame( ) )
				g_Options.CurrentPlayer = 0;

			static ConVar* post_process = nullptr;
			if ( g_Options.misc_disable_pp )
			{
				post_process = g_CVar->FindVar( "mat_postprocess_enable" );
				post_process->RemoveFlags( FCVAR_CHEAT );
				post_process->SetValue( 0 );
			}
			else
			{
				if ( post_process )
				{
					post_process->SetValue( 1 );
					post_process = nullptr;
				}
			}

			g_InputSystem->EnableInput( !Menu::Get( ).IsVisible( ) );

			if ( g_EngineClient->IsInGame( ) && !g_EngineClient->IsTakingScreenshot( ) )
			{

				if ( !g_LocalPlayer )
					return;

				if ( g_Options.bypass_obs && ( g_Options.obs_name || g_Options.obs_box ) )
					OBS::OnPaintTraverse( );

				if ( g_Options.bypass_obs )
					return;

				static auto cross = g_CVar->FindVar("weapon_debug_spread_show");
				cross->SetValue(g_Options.misc_sniper_crosshair && !g_LocalPlayer->m_bIsScoped() ? 3 : 0);

				static auto linegoesthrusmoke = Utils::FindPattern( "client_panorama.dll" , ( PBYTE )"\x55\x8B\xEC\x83\xEC\x08\x8B\x15\x00\x00\x00\x00\x0F\x57\xC0" , "xxxxxxxx????xxx" );
				static auto smokecout = *( DWORD* ) ( linegoesthrusmoke + 0x8 );
				if ( g_Options.misc_nosmoke ) *( int* ) ( smokecout ) = 0;

				if ( g_Options.misc_hitmark )
					Visuals::Misc::RenderHitmark( );

				if ( g_Options.esp_grenadeprediction )
					C_GrenadePrediction::Get( ).Paint( );

				if ( g_Options.esp_autowall )
					Visuals::Misc::RenderDamage( );

				if ( g_Options.misc_noflash )
				{
					g_LocalPlayer->m_flFlashMaxAlpha( ) = 0;
				}

				if ( g_Options.esp_enabled || g_Options.misc_radar )
				{
					for ( auto i = 0; i <= g_EntityList->GetHighestEntityIndex( ); ++i )
					{
						C_BasePlayer* entity = ( C_BasePlayer* ) g_EntityList->GetClientEntity( i );

						if ( !entity )
							continue;

						if ( entity == g_LocalPlayer )
							continue;

						if ( !entity->IsDormant( ) )
						{

							if ( g_Options.misc_radar && entity->IsAlive( ) )
							{
								static auto m_bSpotted = NetvarSys::Get( ).GetOffset( "DT_BaseEntity" , "m_bSpotted" );

								*( char* ) ( ( DWORD ) ( entity ) +m_bSpotted ) = 1;
							}

							if ( g_Options.esp_planted_c4 && entity->GetClientClass( )->m_ClassID == ClassId::ClassId_CPlantedC4 )
								Visuals::Misc::RenderPlantedC4( entity );

							if ( !entity->IsAlive( ) ) continue;

							if ( g_Options.esp_enabled )
							{
								if ( entity->IsPlayer( ) )
								{

									bool can_draw = true;

									if ( g_Options.esp_dead_only && g_LocalPlayer->IsAlive( ) )
										continue;

									if ( Visuals::Player::Begin( entity ) )
									{

										if (!g_Options.Players[entity->EntIndex()].is_forced_esp && g_Options.esp_playerlist)
											can_draw = false;

										if (can_draw)
										{
											if (g_Options.esp_player_snaplines) Visuals::Player::RenderSnapline();
											if (g_Options.esp_player_boxes)     Visuals::Player::RenderBox();
											if (g_Options.esp_player_weapons)   Visuals::Player::RenderWeapon();
											if (g_Options.esp_player_names)     Visuals::Player::RenderName();
											if (g_Options.esp_player_health)    Visuals::Player::RenderHealth();
											if (g_Options.esp_player_armour)    Visuals::Player::RenderArmour();
											if (g_Options.esp_history)			Visuals::Player::RenderHistory();
											if (g_Options.esp_skeleton)			Visuals::Player::RenderSkeleton();
											if (g_Options.rage_aimbot_resolver) Visuals::Player::RenderResolver();
										}

									}
								}

								if ( g_Options.esp_dropped_weapons && entity->IsWeapon( ) )
								{
									Visuals::Misc::RenderWeapon( ( C_BaseCombatWeapon* ) entity );
								}
								if ( g_Options.esp_defuse_kit && entity->IsDefuseKit( ) )
								{
									Visuals::Misc::RenderDefuseKit( entity );
								}
								if (g_Options.esp_nade_esp)
									Visuals::Misc::RenderNadeESP((C_BaseCombatWeapon*)entity);
							}
						}
					}
				}

			}
		}
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkPlaySound( const char* name )
	{
		static auto oPlaySound = vguisurf_hook.get_original<PlaySound>( index::PlaySound );

		if ( g_Options.misc_autoaccept )
		{
			if ( strstr( name , "UI/competitive_accept_beep.wav" ) )
			{
				static auto fnAccept =
					( void( *)( ) )Utils::PatternScan( GetModuleHandleA( "client.dll" ) , "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE" );

				fnAccept( );

				//This will flash the CSGO window on the taskbar
				//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
				FLASHWINFO fi;
				fi.cbSize = sizeof( FLASHWINFO );
				fi.hwnd = InputSys::Get( ).GetMainWindow( );
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx( &fi );
			}
		}

		oPlaySound( g_VGuiSurface , name );
	}
	//--------------------------------------------------------------------------------
	typedef float( __thiscall *get_fov_t )( void* );
	float __fastcall hkGetViewModelFOV( void* ecx , void* edx )
	{

		static auto ofunc = clientmode_hook.get_original<get_fov_t>( 35 );
		float viewmodelFOV = ofunc( ecx );

		if ( g_Options.bypass_obs )
			return 68;

		if ( !g_Options.misc_viewmodel_fov || !g_LocalPlayer )
			return 68;
		else
		{
			if ( !g_LocalPlayer->m_bIsScoped( ) )
				return g_Options.misc_viewmodel_fov;
			else
				return 68;
		}

		return 68;
	}

	int __stdcall hkDoPostScreenEffects( int a1 )
	{

		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>( index::DoPostScreenSpaceEffects );

		if ( g_LocalPlayer && g_Options.glow_enabled && g_Options.esp_enabled && !g_Options.bypass_obs)
			Glow::Get( ).Run( );

		return oDoPostScreenEffects( g_ClientMode , a1 );
	}
	//--------------------------------------------------------------------------------

	void __stdcall hkFrameStageNotify( ClientFrameStage_t stage )
	{

		static auto ofunc = hlclient_hook.get_original<FrameStageNotify>( 37 );

		if ( stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START )
		{
			auto local_id = g_EngineClient->GetLocalPlayer( );
			if ( local_id )
			{
				C_BasePlayer* localplayer = ( C_BasePlayer* ) g_EntityList->GetClientEntity( local_id );

				if ( localplayer && localplayer->IsAlive( ) )
				{
					C_Skins::Get( ).Skinchanger( );
					C_Skins::Get( ).Glovechanger( );
				}
			}


			if ( g_LocalPlayer )
			{
				if ( g_Options.rage_aimbot_resolver && g_Options.rage_aimbot_enable && g_Options.rage_enable )
				{
					for ( int i = 0; i < g_EntityList->GetHighestEntityIndex( ); i++ )
					{
						C_BasePlayer* ent = ( C_BasePlayer* ) g_EntityList->GetClientEntity( i );
						if ( !ent ) continue;
						if ( !ent->IsPlayer( ) ) continue;
						if ( ent->IsDormant( ) ) continue;
						if ( !ent->IsAlive( ) ) continue;
						if ( ent->m_iTeamNum( ) == g_LocalPlayer->m_iTeamNum( ) ) continue;

						C_Resolver::Get( ).resolve( ent );
					}
				}

			}
		}

		if ( stage == FRAME_RENDER_START )
		{
			for ( int i = 1; i <= g_GlobalVars->maxClients; i++ )
			{
				if ( i == g_EngineClient->GetLocalPlayer( ) ) continue;

				IClientEntity* pCurEntity = g_EntityList->GetClientEntity( i );
				if ( !pCurEntity ) continue;

				*( int* ) ( ( uintptr_t ) pCurEntity + 0xA30 ) = g_GlobalVars->framecount; //we'll skip occlusion checks now
				*( int* ) ( ( uintptr_t ) pCurEntity + 0xA28 ) = 0;//clear occlusion flags
			}
		}

		ofunc( g_CHLClient , stage );
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkOverrideView( CViewSetup* vsView )
	{

		static auto ofunc = clientmode_hook.get_original<OverrideView>( index::OverrideView );

		if ( g_EngineClient->IsInGame( ) && vsView )
		{
			Visuals::Misc::ThirdPerson( );

			if ( g_LocalPlayer && g_Options.esp_grenadeprediction && !g_Options.bypass_obs)
				C_GrenadePrediction::Get( ).View( vsView );

		}
		if ( g_LocalPlayer )
		{
			if ( !g_LocalPlayer->m_bIsScoped( ) && !g_Options.bypass_obs)
			{
				static auto viewmodel = g_CVar->FindVar( "viewmodel_fov" );

				if ( g_Options.misc_viewmodel_fov )
					viewmodel->SetValue( g_Options.misc_viewmodel_fov );
				else
					viewmodel->SetValue( viewmodel->GetDefault( ) );

				if ( g_Options.misc_fov )
					vsView->fov = g_Options.misc_fov;
			}


		}

		ofunc( g_ClientMode , vsView );
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkDrawModelExecute( IMatRenderContext* ctx , const DrawModelState_t& state , const ModelRenderInfo_t& pInfo , matrix3x4_t* pCustomBoneToWorld )
	{
		static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>( index::DrawModelExecute );

		if ( !g_Options.bypass_obs )
			Chams::Get( ).OnDrawModelExecute( ctx , state , pInfo , pCustomBoneToWorld );
		ofunc( g_MdlRender , ctx , state , pInfo , pCustomBoneToWorld );

		g_MdlRender->ForcedMaterialOverride( nullptr );
	}

	auto dwCAM_Think = Utils::PatternScan( GetModuleHandleW( L"client_panorama.dll") , "85 C0 75 30 38 86" );
	typedef bool( __thiscall *svc_get_bool_t )( PVOID );
	bool __fastcall hkSvCheatsGetBool( PVOID pConVar , void* edx )
	{
		static auto ofunc = sv_cheats.get_original<svc_get_bool_t>( 13 );
		if ( !ofunc )
			return false;

		if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == reinterpret_cast< DWORD >( dwCAM_Think ) )
			return true;
		return ofunc( pConVar );
	}
}