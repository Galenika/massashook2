# include "NetVarHooks.h"
# include "valve_sdk/sdk.hpp"
# include "hooks.hpp"
# include "options.hpp"

typedef void( *RecvVarProxyFn )( const CRecvProxyData* pData, void* pStruct, void* pOut );
RecvVarProxyFn fnSequenceProxyFn = nullptr;
RecvVarProxyFn oRecvnModelIndex;

void Hooked_RecvProxy_Viewmodel( CRecvProxyData *pData, void *pStruct, void *pOut )
{
	if ( !g_Options.skins_knife_model )
		return oRecvnModelIndex( pData, pStruct, pOut );

	int default_t = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_default_t.mdl" );
	int default_ct = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_default_ct.mdl" );
	int iBayonet = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_bayonet.mdl" );
	int iButterfly = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_butterfly.mdl" );
	int iFlip = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_flip.mdl" );
	int iGut = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_gut.mdl" );
	int iKarambit = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_karam.mdl" );
	int iM9Bayonet = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_m9_bay.mdl" );
	int iHuntsman = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_tactical.mdl" );
	int iFalchion = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_falchion_advanced.mdl" );
	int iDagger = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_push.mdl" );
	int iBowie = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_survival_bowie.mdl" );
	int iGunGame = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_gg.mdl" );
	int iNavja = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_gypsy_jackknife.mdl" );
	int iUrsus = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_ursus.mdl" );
	int iTalon = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_widowmaker.mdl" );
	int iStiletto = g_MdlInfo->GetModelIndex( "models/weapons/v_knife_stiletto.mdl" );

	if ( g_Options.skins_enable && g_LocalPlayer )
	{
		if ( g_LocalPlayer->IsAlive( ) && ( pData->m_Value.m_Int == default_ct
			|| pData->m_Value.m_Int == default_t
			|| pData->m_Value.m_Int == iGut
			|| pData->m_Value.m_Int == iKarambit
			|| pData->m_Value.m_Int == iM9Bayonet
			|| pData->m_Value.m_Int == iHuntsman
			|| pData->m_Value.m_Int == iFalchion
			|| pData->m_Value.m_Int == iDagger
			|| pData->m_Value.m_Int == iBowie
			|| pData->m_Value.m_Int == iButterfly
			|| pData->m_Value.m_Int == iFlip
			|| pData->m_Value.m_Int == iBayonet ) )
		{
			switch ( g_Options.skins_knife_model )
			{
			case MenuKnife::Bayonet:
				pData->m_Value.m_Int = iBayonet;
				break;
			case MenuKnife::Bowei:
				pData->m_Value.m_Int = iBowie;
				break;
			case MenuKnife::Butter:
				pData->m_Value.m_Int = iButterfly;
				break;
			case MenuKnife::Flachion:
				pData->m_Value.m_Int = iFalchion;
				break;
			case MenuKnife::Flip:
				pData->m_Value.m_Int = iFlip;
				break;
			case MenuKnife::Navja:
				pData->m_Value.m_Int = iNavja;
				break;
			case MenuKnife::Gut:
				pData->m_Value.m_Int = iGut;
				break;
			case MenuKnife::Hunts:
				pData->m_Value.m_Int = iHuntsman;
				break;
			case MenuKnife::Karam:
				pData->m_Value.m_Int = iKarambit;
				break;
			case MenuKnife::M9:
				pData->m_Value.m_Int = iM9Bayonet;
				break;
			case MenuKnife::Shadow:
				pData->m_Value.m_Int = iDagger;
				break;
			case MenuKnife::Ursus:
				pData->m_Value.m_Int = iUrsus;
				break;
			case MenuKnife::Stiletto:
				pData->m_Value.m_Int = iStiletto;
				break;
			case MenuKnife::Talon:
				pData->m_Value.m_Int = iTalon;
				break;

			}
		}
	}

	oRecvnModelIndex( pData, pStruct, pOut );
}

void SetViewModelSequence2( const CRecvProxyData *pDataConst, void *pStruct, void *pOut )
{

	static int seq = 0;
	if ( GetAsyncKeyState( VK_NUMPAD0 ) & 1 ) seq++;
	if ( GetAsyncKeyState( VK_NUMPAD5 ) & 1 ) seq = 0;
	if ( GetAsyncKeyState( VK_F7 ) & 1 ) MessageBoxA( 0, std::to_string( seq ).c_str( ), 0, 0 );


	// Make the incoming data editable.
	CRecvProxyData* pData = const_cast< CRecvProxyData* >( pDataConst );

	// Confirm that we are replacing our view model and not someone elses.
	CBaseViewModel* pViewModel = static_cast< CBaseViewModel* >( pStruct );

	if ( pViewModel )
	{
		C_BasePlayer* pOwner = static_cast< C_BasePlayer* >( g_EntityList->GetClientEntity( pViewModel->GetOwner( ) & 0xFFF ) );

		// Compare the owner entity of this view model to the local player entity.
		if ( pOwner && pOwner->EntIndex( ) == g_EngineClient->GetLocalPlayer( ) )
		{
			// Get the filename of the current view model.
			auto pModel = g_MdlInfo->GetModel( pViewModel->GetModelIndex( ) );
			std::string szModel = g_MdlInfo->GetModelName( pModel );

			// Store the current sequence.
			int m_nSequence = pData->m_Value.m_Int;
			if ( szModel == "models/weapons/v_knife_butterfly.mdl" )
			{
				// Fix animations for the Butterfly Knife.
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt( SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03 );
					break;
				default:
					m_nSequence++;
				}
			}
			else if ( szModel == "models/weapons/v_knife_falchion_advanced.mdl" )
			{
				// Fix animations for the Falchion Knife.
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt( SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP );
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt( SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02 );
					break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence--;
				}
			}
			else if ( szModel == "models/weapons/v_knife_push.mdl" )
			{
				// Fix animations for the Shadow Daggers.
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
				case SEQUENCE_DEFAULT_LIGHT_MISS1:
				case SEQUENCE_DEFAULT_LIGHT_MISS2:
					m_nSequence = RandomInt( SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5 );
					break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt( SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1 );
					break;
				case SEQUENCE_DEFAULT_HEAVY_HIT1:
				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence += 3; break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence += 2;
				}
			}
			else if ( szModel == "models/weapons/v_knife_survival_bowie.mdl" )
			{
				// Fix animations for the Bowie Knife.
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_BOWIE_IDLE1;
					break;
				default:
					m_nSequence--;
				}
			}
			else if ( szModel == "models/weapons/v_knife_stiletto.mdl" )
			{
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt( 12, 13 );
					break;
				}
			}
			else if ( szModel == "models/weapons/v_knife_ursus.mdl" )
			{
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt( SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2 );
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt( SEQUENCE_BUTTERFLY_LOOKAT01, 14 );
					break;
				default:
					m_nSequence++;
					break;
				}
			}
			else if ( szModel == "models/weapons/v_knife_widowmaker.mdl" )
			{
				switch ( m_nSequence )
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt( 14, 15 );
					break;
				}
			}

			// Set the fixed sequence.
			pData->m_Value.m_Int = m_nSequence;
		}
	}

	// Call original function with the modified data.

	fnSequenceProxyFn( pData, pStruct, pOut );
}

void AnimationFixHook( )
{
	for ( ClientClass* pClass = g_CHLClient->GetAllClasses( ); pClass; pClass = pClass->m_pNext ) {
		if ( !strcmp( pClass->m_pNetworkName, "CBaseViewModel" ) ) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for ( int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++ ) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if ( !pProp || strcmp( pProp->m_pVarName, "m_nSequence" ) )
					continue;

				// Store the original proxy function.
				fnSequenceProxyFn = static_cast< RecvVarProxyFn >( pProp->m_ProxyFn );

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = static_cast< RecvVarProxyFn >( SetViewModelSequence2 );

				break;
			}

			break;
		}
	}
}

void AnimationFixUnhook( )
{
	for ( ClientClass* pClass = g_CHLClient->GetAllClasses( ); pClass; pClass = pClass->m_pNext ) {
		if ( !strcmp( pClass->m_pNetworkName, "CBaseViewModel" ) ) {
			// Search for the 'm_nModelIndex' property.
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for ( int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++ ) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if ( !pProp || strcmp( pProp->m_pVarName, "m_nSequence" ) )
					continue;

				// Replace the proxy function with our sequence changer.
				pProp->m_ProxyFn = fnSequenceProxyFn;

				break;
			}

			break;
		}
	}
}

RecvVarProxyFn fnNoSmoke;
void NoSmoke( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	if ( g_Options.misc_nosmoke ) *( bool* ) ( ( DWORD ) pOut + 0x1 ) = true;
	fnNoSmoke( pData, pStruct, pOut );
}

void NetvarHook( )
{
	AnimationFixHook( );
	ClientClass *pClass = g_CHLClient->GetAllClasses( );
	while ( pClass )
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if ( !strcmp( pszName, "DT_SmokeGrenadeProjectile" ) )
		{
			for ( int i = 0; i < pClass->m_pRecvTable->m_nProps; i++ )
			{
				RecvProp *pProp = &( pClass->m_pRecvTable->m_pProps[i] );
				const char *name = pProp->m_pVarName;
				if ( !strcmp( name, "m_bDidSmokeEffect" ) )
				{
					fnNoSmoke = ( RecvVarProxyFn ) pProp->m_ProxyFn;
					pProp->m_ProxyFn = NoSmoke;
				}

			}
		}
		else if ( !strcmp( pszName, "DT_BaseViewModel" ) )
		{
			for ( int i = 0; i < pClass->m_pRecvTable->m_nProps; i++ )
			{
				RecvProp *pProp = &( pClass->m_pRecvTable->m_pProps[i] );
				const char *name = pProp->m_pVarName;
				if ( !strcmp( name, "m_nModelIndex" ) )
				{
					oRecvnModelIndex = ( RecvVarProxyFn ) pProp->m_ProxyFn;
					pProp->m_ProxyFn = ( RecvVarProxyFn ) Hooked_RecvProxy_Viewmodel;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}
void UnloadProxy( )
{
	AnimationFixUnhook( );
	ClientClass *pClass = g_CHLClient->GetAllClasses( );
	while ( pClass )
	{
		const char *pszName = pClass->m_pRecvTable->m_pNetTableName;
		if ( !strcmp( pszName, "DT_SmokeGrenadeProjectile" ) )
		{
			for ( int i = 0; i < pClass->m_pRecvTable->m_nProps; i++ )
			{
				RecvProp *pProp = &( pClass->m_pRecvTable->m_pProps[i] );
				const char *name = pProp->m_pVarName;
				if ( !strcmp( name, "m_bDidSmokeEffect" ) )
				{
					pProp->m_ProxyFn = fnNoSmoke;
				}

			}
		}
		else if ( !strcmp( pszName, "DT_BaseViewModel" ) )
		{
			for ( int i = 0; i < pClass->m_pRecvTable->m_nProps; i++ )
			{
				RecvProp *pProp = &( pClass->m_pRecvTable->m_pProps[i] );
				const char *name = pProp->m_pVarName;


				// Knives
				if ( !strcmp( name, "m_nModelIndex" ) )
				{
					pProp->m_ProxyFn = oRecvnModelIndex;
				}
			}
		}
		pClass = pClass->m_pNext;
	}
}
