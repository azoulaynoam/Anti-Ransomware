#pragma once

enum OF_TYPE
{
	FILES_ONLY = 1,
	MODULES_ONLY = 2,
	ALL_TYPES = 3
};

struct OF_INFO_t
{
	DWORD dwPID;
	LPCWSTR lpFile;
	HANDLE hFile;
};

typedef void (CALLBACK* OF_CALLBACK)(OF_INFO_t OpenedFileInf0, UINT_PTR uUserContext );


extern "C" __declspec(dllexport) void ShowOpenedFiles( LPCWSTR lpPath );
extern "C" __declspec(dllexport) void GetOpenedFiles( LPCWSTR lpPath, 
													  OF_TYPE Filter,
													  OF_CALLBACK CallBackProc,
													  UINT_PTR pUserContext );
