#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"
#include "helpers/Config.h"

#include "hooks.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "Seq.h"
#include "GameEvents.h"
#include "imgui/imgui.h"
#include "NetVarHooks.h"
#include "steam_sdk/steam_api.h"

#include "ReflectiveLoader.h"
#include <wininet.h>
#include <string.h>
# include <ctime>
#pragma comment(lib, "wininet")


std::string GetUrlData(std::string url)
{
	std::string request_data = "";

	HINTERNET hIntSession = InternetOpenA("", 1, NULL, NULL, 0);

	if (!hIntSession)
	{
		return request_data;
	}

	HINTERNET hHttpSession = InternetConnectA(hIntSession, "www.massas.pw", 80, 0, 0, 3, 0, NULL);

	if (!hHttpSession)
	{
		return request_data;
	}

	HINTERNET hHttpRequest = HttpOpenRequestA(hHttpSession, "GET", url.c_str()
		, 0, 0, 0, 0x80000000, 0);

	if (!hHttpSession)
	{
		return request_data;
	}

	char* szHeaders = "Content-Type: text/html\r\nUser-Agent: License";
	char szRequest[1024] = { 0 };

	if (!HttpSendRequestA(hHttpRequest, szHeaders, strlen(szHeaders), szRequest, strlen(szRequest)))
	{
		return request_data;
	}

	CHAR szBuffer[1024] = { 0 };
	DWORD dwRead = 0;

	while (InternetReadFile(hHttpRequest, szBuffer, sizeof(szBuffer) - 1, &dwRead) && dwRead)
	{
		request_data.append(szBuffer, dwRead);
	}

	InternetCloseHandle(hHttpRequest);
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hIntSession);

	return request_data;
}

DWORD GetHwid()
{
	DWORD serial;
	GetVolumeInformationA("C:\\", 0, 0, &serial, 0, 0, 0, 0);

	return serial;
}

typedef uint32_t SteamPipeHandle;
typedef uint32_t SteamUserHandle;


DWORD WINAPI OnDllAttach( LPVOID base )
{
	while ( !GetModuleHandleW( L"server.dll" ) ) Sleep( 500 );

#ifdef _DEBUG
	Utils::AttachConsole( );
#endif

	try {
		Utils::ConsolePrint( "Initializing...\n" );

		time_t rawtime;
		struct tm * timeinfo;
		char timer[80];

		time( &rawtime );
		timeinfo = localtime( &rawtime );

		strftime( timer, sizeof( timer ), "%d-%m-%Y %H:%M:%S", timeinfo );
		std::string str( timer );

		SHGetFolderPathA( NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, ( LPSTR ) ConfigSys::Get( ).my_documents_folder );

		CreateDirectoryA( std::string( ConfigSys::Get( ).my_documents_folder + std::string( "\\massas.pw\\" ) ).c_str( ), NULL );


		using fnCommandLine = ICommandLine * ( __stdcall * )( );
		g_CommandLine = reinterpret_cast< fnCommandLine >( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "CommandLine" ) )( );

		static auto cmdline = g_CommandLine->GetCmdLine( );

		g_Options.beta = strstr( cmdline, "massashook_beta" );
		g_Options.light = strstr( cmdline, "massashook_light" );

		Interfaces::Initialize( );
		Interfaces::Dump( );

		SteamUserHandle hSteamUser = ( ( SteamUserHandle( __cdecl* )( void ) )GetProcAddress( GetModuleHandleA( "steam_api.dll" ), "SteamAPI_GetHSteamUser" ) )( );
		SteamPipeHandle hSteamPipe = ( ( SteamPipeHandle( __cdecl* )( void ) )GetProcAddress( GetModuleHandleA( "steam_api.dll" ), "SteamAPI_GetHSteamPipe" ) )( );

		auto SteamClient = ( ( ISteamClient*( __cdecl* )( void ) )GetProcAddress( GetModuleHandleA( "steam_api.dll" ), "SteamClient" ) )( );
		g_SteamClient = SteamClient;
		auto SteamHTTP = SteamClient->GetISteamHTTP( hSteamUser, hSteamPipe, "STEAMHTTP_INTERFACE_VERSION002" );
		g_SteamUser = ( ISteamUser* ) SteamClient->GetISteamUser( hSteamUser, hSteamPipe, "SteamUser019" );
		auto SteamFriends = SteamClient->GetISteamFriends( hSteamUser, hSteamPipe, "SteamFriends015" );
		g_SteamFriends = SteamFriends;
		auto SteamInventory = SteamClient->GetISteamInventory( hSteamUser, hSteamPipe, "STEAMINVENTORY_INTERFACE_V002" );
		g_SteamGameCoordinator = ( ISteamGameCoordinator* ) SteamClient->GetISteamGenericInterface( hSteamUser, hSteamPipe, "SteamGameCoordinator001" );

		auto acc_id = g_SteamUser->GetSteamID( ).GetAccountID( );
		auto name = SteamFriends->GetPersonaName( );
		auto level = g_SteamUser->GetPlayerSteamLevel( );

		std::string m = std::string( timer + std::string( " DLL Side confirmation: ( Level: " ) + std::to_string( level ) +
			" | Nickname: " + name + " | AccountID: " + std::to_string( acc_id ) + " )" );

		GetUrlData( std::string( "/loader2/log.php?message=" + m ) ); /*just to keep some track :v*/

		NetvarSys::Get( ).Initialize( );
		InputSys::Get( ).Initialize( );
		Menu::Get( ).Initialize( );

		Hooks::Initialize( );
		NetvarHook( );

		ConVar* developer = g_CVar->FindVar( "developer" );
		developer->SetValue( 1 );
		ConVar* con_filter_enable = g_CVar->FindVar( "con_filter_enable" );
		con_filter_enable->SetValue( 2 );
		ConVar* con_filter_text = g_CVar->FindVar( "con_filter_text" );
		con_filter_text->SetValue( "[MassasHook]" );
		ConVar* con_filter_text_out = g_CVar->FindVar( "con_filter_text_out" );
		con_filter_text_out->SetValue( "values are between 54.00 and 68.00. To remove the restriction set sv_competitive_minspec 0 on the server." );



		InputSys::Get( ).RegisterHotkey( VK_F9, [base] ( ) {
			if ( Menu::Get( ).IsVisible( ) )
				Menu::Get( ).Toggle( );
			g_Unload = true;
		} );

		// Menu Toggle
		InputSys::Get( ).RegisterHotkey( VK_INSERT, [base] ( ) {
			Menu::Get( ).Toggle( );
		} );

		Utils::ConsolePrint( "Finished.\n" );

		while ( !g_Unload )
			Sleep( 1000 );

		g_CVar->FindVar( "crosshair" )->SetValue( true );

#ifdef _DEBUG
		Utils::DetachConsole( );
#endif
		Hooks::Shutdown( );
		Menu::Get( ).Shutdown( );
		UnloadProxy( );

		FreeLibraryAndExitThread( static_cast< HMODULE >( base ), 1 );

	}
	catch ( const std::exception& ex ) {
		Utils::ConsolePrint( "An error occured during initialization:\n" );
		Utils::ConsolePrint( "%s\n", ex.what( ) );
		Utils::ConsolePrint( "Press any key to exit.\n" );
		Utils::ConsoleReadKey( );
		Utils::DetachConsole( );

		FreeLibraryAndExitThread( static_cast< HMODULE >( base ), 1 );
	}

	// unreachable
	//return TRUE;
}

BOOL WINAPI OnDllDetach()
{
#ifdef _DEBUG
	Utils::DetachConsole();
#endif
	Hooks::Shutdown();
	Menu::Get().Shutdown();
	UnloadProxy(); //is this for the skinchanger animnation meme? yes show me
    return TRUE;
}
HINSTANCE meme;
BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch(fdwReason) {
	    case DLL_QUERY_HMODULE:
		    if (lpvReserved != NULL)
			    *(HMODULE *)lpvReserved = meme;
		    break;
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        case DLL_PROCESS_DETACH:
			if (lpvReserved == nullptr)
				return OnDllDetach();
            return TRUE;
        default:
            return TRUE;
    }
}
