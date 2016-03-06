#include <Windows.h>
#include <stdio.h>

extern "C" __declspec(dllimport) void ShowOpenedFiles( LPCWSTR lpPath );
int __stdcall wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	ShowOpenedFiles( lpCmdLine );
	return 0;
}

//void EntryFunc()
//{
//	LPWSTR lpCmdLine = GetCommandLine();
//	int nArgCount = 0;
//	LPWSTR* pArgs = CommandLineToArgvW( lpCmdLine, &nArgCount );
//	ShowOpenedFiles( pArgs[1] );
//}