#include "StdAfx.h"
#include "OpenedFiles.h"
#include "Main.h"
#include <Tlhelp32.h>
#include <Psapi.h>
#include "Utils.h"

void ShowOpenedFiles( LPCWSTR lpPath )
{
	Main( L"c:\\A" );
    OleUninitialize();
}

void EnumerateLoadedModules( CString& csPath, OF_CALLBACK CallBackProc, UINT_PTR pUserContext );
void EnumerateOpenedFiles( CString& csPath, OF_CALLBACK CallBackProc, UINT_PTR pUserContext, HANDLE hDriver, GetFinalPathNameByHandleDef pGetFinalPathNameByHandle );

extern "C" __declspec(dllexport) void GetOpenedFiles( LPCWSTR lpPath, OF_TYPE Filter, OF_CALLBACK CallBackProc,
													  UINT_PTR pUserContext )
{
	CString csPath = lpPath;
	csPath.MakeLower();
	EnableTokenPrivilege( SE_DEBUG_NAME );
	if( Filter& MODULES_ONLY )
	{
		EnumerateLoadedModules( csPath, CallBackProc, pUserContext );
	}

	if( Filter& FILES_ONLY )
	{
		

		// Extract the driver from the resource and install it.
		HANDLE hDriver = ExtractAndInstallDrv();
		GetFinalPathNameByHandleDef pGetFinalPathNameByHandle = 0;
		if(  !hDriver )
		{
			pGetFinalPathNameByHandle = (GetFinalPathNameByHandleDef)GetProcAddress( GetModuleHandle(_T("kernel32.dll")), "GetFinalPathNameByHandleW" );
		}		
		// Now walk all handles
		EnumerateOpenedFiles( csPath, CallBackProc, pUserContext, hDriver, pGetFinalPathNameByHandle );
		if( hDriver )
		{	// Time to wind up		
			StopAndUninstallDrv( hDriver );
		}
	}
}

UINT g_CurrentIndex = 0;
struct THREAD_PARAMS
{
	PSYSTEM_HANDLE_INFORMATION pSysHandleInformation;
	GetFinalPathNameByHandleDef pGetFinalPathNameByHandle;
	LPTSTR lpPath;
	int nFileType;
	HANDLE hStartEvent;
	HANDLE hFinishedEvent;
	bool bStatus;
};

DWORD WINAPI ThreadProc( LPVOID lParam )
{
	THREAD_PARAMS* pThreadParam = (THREAD_PARAMS*)lParam;
	
	GetFinalPathNameByHandleDef pGetFinalPathNameByHandle = pThreadParam->pGetFinalPathNameByHandle;
	for( g_CurrentIndex; g_CurrentIndex < pThreadParam->pSysHandleInformation->dwCount;  )
	{

		WaitForSingleObject( pThreadParam->hStartEvent, INFINITE );
		ResetEvent( pThreadParam->hStartEvent );
		pThreadParam->bStatus = false;
		SYSTEM_HANDLE& sh = pThreadParam->pSysHandleInformation->Handles[g_CurrentIndex];
		g_CurrentIndex++;
		HANDLE hDup = (HANDLE)sh.wValue;
		HANDLE hProcess = OpenProcess( PROCESS_DUP_HANDLE , FALSE, sh.dwProcessId );
		if( hProcess )
		{
			BOOL b = DuplicateHandle( hProcess, (HANDLE)sh.wValue, GetCurrentProcess(), &hDup, 0, FALSE, DUPLICATE_SAME_ACCESS );
			if( !b )
			{
				hDup = (HANDLE)sh.wValue;
			}
			CloseHandle( hProcess );
		}
		DWORD dwRet = pGetFinalPathNameByHandle( hDup, pThreadParam->lpPath, MAX_PATH, 0 );
		if( hDup && (hDup != (HANDLE)sh.wValue))
		{
			CloseHandle( hDup );
		}
		if(dwRet)
		{
			pThreadParam->bStatus = true;
		}
		SetEvent( pThreadParam->hFinishedEvent );
		
	}
	return 0;
}

void EnumerateOpenedFiles( CString& csPath, OF_CALLBACK CallBackProc, UINT_PTR pUserContext, HANDLE hDriver,
						   GetFinalPathNameByHandleDef pGetFinalPathNameByHandle ) 
{
	int nFileType = XP_FILETYPE;
	OSVERSIONINFO info = { 0 }; 
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&info); 
	if( info.dwMajorVersion == 6 &&
		info.dwMinorVersion == 0 )
	{
		nFileType = VISTA_FILETYPE;
	}
	LPCTSTR lpPath = csPath;
	CString csShortName;
	GetShortPathName( csPath, csShortName.GetBuffer( MAX_PATH), MAX_PATH );
	csShortName.ReleaseBuffer();
	csShortName.MakeLower();
	bool bShortPath = false;
	LPCTSTR lpShortPath = csShortName;
	if(  csShortName != csPath && FALSE == csShortName.IsEmpty())
	{
		bShortPath = true;
	}

	HMODULE hModule = GetModuleHandle(_T("ntdll.dll"));		
	PNtQuerySystemInformation NtQuerySystemInformation = (PNtQuerySystemInformation)GetProcAddress( hModule, "NtQuerySystemInformation" );
	if( 0 == NtQuerySystemInformation )
	{
		OutputDebugString( L"Getting proc of NtQuerySystemInformation failed" );
		return;
	}

	// Get the list of all handles in the system
	PSYSTEM_HANDLE_INFORMATION pSysHandleInformation = new SYSTEM_HANDLE_INFORMATION;
	DWORD size = sizeof(SYSTEM_HANDLE_INFORMATION);
	DWORD needed = 0;
	NTSTATUS status = NtQuerySystemInformation( SystemHandleInformation, pSysHandleInformation, size, &needed );
	if( !NT_SUCCESS(status))
	{
		if( 0 == needed )
		{
			return;// some other error
		}
		// The previously supplied buffer wasn't enough.
		delete pSysHandleInformation;
		size = needed + 1024;
		pSysHandleInformation = (PSYSTEM_HANDLE_INFORMATION)new BYTE[size];
		status = NtQuerySystemInformation( SystemHandleInformation, pSysHandleInformation, size, &needed );
		if( !NT_SUCCESS(status))
		{
			// some other error so quit.
			delete pSysHandleInformation;
			return;
		}
	}

	if( pGetFinalPathNameByHandle )// there is no driver, we have do it ugly way
	{
		g_CurrentIndex = 0;
		TCHAR tcFileName[MAX_PATH+1];
		THREAD_PARAMS ThreadParams;
		ThreadParams.lpPath = tcFileName;
		ThreadParams.nFileType = nFileType;
		ThreadParams.pGetFinalPathNameByHandle = pGetFinalPathNameByHandle;
		ThreadParams.pSysHandleInformation = pSysHandleInformation;
		ThreadParams.hStartEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
		ThreadParams.hFinishedEvent = ::CreateEvent( 0, TRUE, FALSE, 0 );
		HANDLE ThreadHandle = 0;
		while( g_CurrentIndex < pSysHandleInformation->dwCount )
		{
			if( !ThreadHandle )
			{
				ThreadHandle = CreateThread( 0, 0, ThreadProc, &ThreadParams, 0, 0 );	
			}
			ResetEvent( ThreadParams.hFinishedEvent );
			SetEvent( ThreadParams.hStartEvent );
			if( WAIT_TIMEOUT == WaitForSingleObject( ThreadParams.hFinishedEvent, 100 ))
			{
				CString csError;
				csError.Format(L"Query hang for handle %d", (int)pSysHandleInformation->Handles[g_CurrentIndex - 1].wValue);
				OutputDebugString(csError );
				TerminateThread( ThreadHandle, 0 );
				CloseHandle( ThreadHandle );
				ThreadHandle = 0;
				continue;
			}
			if( !ThreadParams.bStatus )
			{
				continue;
			}
			int nCmpStart = 4;
			CString csFileName( &ThreadParams.lpPath[nCmpStart] );
			csFileName.MakeLower();
			if( 0 != _tcsncmp( lpPath, csFileName , csPath.GetLength()))
			{
				continue;
			}
			OF_INFO_t stOFInfo;
			stOFInfo.dwPID = pSysHandleInformation->Handles[g_CurrentIndex - 1].dwProcessId;
			stOFInfo.lpFile = csFileName;
			stOFInfo.hFile  = (HANDLE)pSysHandleInformation->Handles[g_CurrentIndex - 1].wValue;
			CallBackProc( stOFInfo, pUserContext );
		}
		if( ThreadHandle )
		{
			if( WAIT_TIMEOUT == WaitForSingleObject( ThreadHandle, 1000 ))
			{
				TerminateThread( ThreadHandle, 0 );
			}			
			CloseHandle( ThreadHandle );
		}
		CloseHandle( ThreadParams.hStartEvent );
		CloseHandle( ThreadParams.hFinishedEvent );
		return;
	}

	// Walk through the handle list
	for ( DWORD i = 0; i < pSysHandleInformation->dwCount; i++ )
	{
		SYSTEM_HANDLE& sh = pSysHandleInformation->Handles[i];
		if( sh.bObjectType != nFileType )// Under windows XP file handle is of type 28
		{
			continue;
		}

		CString csFileName;
		CString csDir;
		if( hDriver )
		{
			HANDLE_INFO stHandle = {0};
			ADDRESS_INFO stAddress;
			stAddress.pAddress = sh.pAddress;
			DWORD dwReturn = 0;
			BOOL bSuccess = DeviceIoControl( hDriver, IOCTL_LISTDRV_BUFFERED_IO, &stAddress, sizeof(ADDRESS_INFO), 
				&stHandle, sizeof(HANDLE_INFO), &dwReturn, NULL );

			
			if( bSuccess && stHandle.tcFileName[0] != 0 && 
				stHandle.uType != FILE_DEVICE_SOUND && 
				stHandle.uType != FILE_DEVICE_NAMED_PIPE )
			{

				if( stHandle.uType != FILE_DEVICE_NETWORK_FILE_SYSTEM  )
				{
					// Get the drive name from the dos device name
					if( !GetDrive( (LPCTSTR)stHandle.tcDeviceName, csFileName, true ))
					{
						OutputDebugString( L"GetDrive failed" );
					}
					csFileName += (LPCTSTR)stHandle.tcFileName;
				}
				else
				{
					csFileName = _T("\\");
					csFileName += (LPCTSTR)stHandle.tcFileName;
				}
			}
            else
            {
                continue;
            }
		}		
		else
		{
			return;
		}


		csFileName.MakeLower();
		// Check whether the file belongs to the specified folder
// 		if( -1 == csFileName.Find( csPath ))
// 		{
// 			if( bShortPath )
// 			{
// 				// Some times the file name may be in short path form.
// 				if( -1 == csFileName.Find( csShortName ))
// 				{
// 					continue;
// 				}
// 			}
// 			else
// 			{
// 				continue;
// 			}
// 		}

		if( 0 != _tcsncmp( lpPath, csFileName, csPath.GetLength()))
		{
			if( bShortPath )
			{
				// Some times the file name may be in short path form.
				if( 0 != _tcsncmp( lpShortPath, csFileName, csShortName.GetLength()))
				{
					continue;
				}
			}
			else
			{
				continue;
			}
		}
		OF_INFO_t stOFInfo;
		stOFInfo.dwPID = sh.dwProcessId;
		stOFInfo.lpFile = csFileName;
		stOFInfo.hFile  = (HANDLE)sh.wValue;
		CallBackProc( stOFInfo, pUserContext );
	}
	delete pSysHandleInformation;

}


void EnumerateLoadedModules( CString& csPath, OF_CALLBACK CallBackProc, UINT_PTR pUserContext )
{
	CString csShortName;
	GetShortPathName( csPath, csShortName.GetBuffer( MAX_PATH), MAX_PATH );
	csShortName.ReleaseBuffer();
	csShortName.MakeLower();
	bool bShortPath = false;
	if(  csShortName != csPath && FALSE == csShortName.IsEmpty())
	{
		bShortPath = true;
	}

	DWORD dwsize = 300;
	PDWORD pDwId = (PDWORD)new BYTE[dwsize];
	DWORD dwRetruned = dwsize;
	// Enum all the process first
	while( 1 )
	{
		EnumProcesses( pDwId, dwsize, &dwRetruned );
		if( dwsize > dwRetruned  )
		{
			break;
		}
		delete pDwId;
		dwsize += 50;
		pDwId = (PDWORD)new BYTE[dwsize];
	}
	int nCount = dwRetruned / sizeof(DWORD);
	int nItemCount = -1;
	// Enumerate modules of the above process
	for( int nIdx = 0; nIdx < nCount;nIdx++ )
	{    
		if( 0 != pDwId[nIdx] )
		{
			HANDLE hModuleSnap = INVALID_HANDLE_VALUE; 
			MODULEENTRY32 me32; 
			// Take a snapshot of all modules in the specified process. 
			hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, pDwId[nIdx] ); 
			if( hModuleSnap == INVALID_HANDLE_VALUE ) 
			{     
				continue; 
			}
			me32.dwSize = sizeof( MODULEENTRY32 ); 
			if( !Module32First( hModuleSnap, &me32 ) ) 
			{     
				CloseHandle( hModuleSnap );
				continue; 
			}
			bool bFirst = true;			
			PROCESS_INFO_t stInfo;
			do 
			{ 
				CString csModule;
				if( bFirst )
				{
					// First module is always the exe name
					bFirst = false;					
					if( !PathFileExists( me32.szExePath ))
					{
						TCHAR tcFileName[MAX_PATH];
						HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, TRUE, pDwId[nIdx] );                        
						if( GetProcessImageFileName( hProcess, tcFileName, MAX_PATH ))
						{
							GetDrive( tcFileName, csModule, false );
						}
						CloseHandle( hProcess );
					}
					else
					{
						csModule = me32.szExePath;
					}					
					csModule.MakeLower();
				}
				else
				{
					csModule = me32.szExePath;
					csModule.MakeLower();
				}
				if( -1 == csModule.Find( csPath ))
				{
					if( bShortPath )
					{
						if( -1 == csModule.Find( csShortName ))
						{
							continue;
						}
					}
					else
					{
						continue;
					}
				}
				OF_INFO_t stOFInfo;
				stOFInfo.dwPID = pDwId[nIdx];
				stOFInfo.lpFile = csModule;
				CallBackProc( stOFInfo, pUserContext );
			}
			while( Module32Next( hModuleSnap, &me32 ) ); 
			CloseHandle( hModuleSnap ); 
		}
	}
	delete pDwId;
}