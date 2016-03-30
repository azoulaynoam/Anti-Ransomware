#if !defined(AFX_UTILS_H__DEF02A65_CB32_46B8_9C74_5E7432392A9F__INCLUDED_)
#define AFX_UTILS_H__DEF02A65_CB32_46B8_9C74_5E7432392A9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <Winsvc.h>
#include "WinIoCtl.h"

#define IOCTL_LISTDRV_BUFFERED_IO		\
        CTL_CODE(FILE_DEVICE_UNKNOWN,	\
        0x802,							\
        METHOD_BUFFERED,	            \
        FILE_READ_DATA | FILE_WRITE_DATA)

#define VISTA_FILETYPE  25
#define XP_FILETYPE 28

typedef struct _HANDLE_INFO
{	
	USHORT tcDeviceName[260];
	USHORT tcFileName[260];
    ULONG uType;
}HANDLE_INFO;

typedef struct _ADDRESS_INFO
{
	PVOID pAddress;
}ADDRESS_INFO;

typedef LONG NTSTATUS;

#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)

typedef struct _SYSTEM_HANDLE
{
    DWORD       dwProcessId;
    BYTE		bObjectType;
    BYTE		bFlags;
    WORD		wValue;
	PVOID       pAddress;
	DWORD GrantedAccess;
}SYSTEM_HANDLE;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    DWORD         dwCount;
    SYSTEM_HANDLE Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION, **PPSYSTEM_HANDLE_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
    SystemHandleInformation = 0X10,
} SYSTEM_INFORMATION_CLASS;

typedef NTSTATUS ( WINAPI *PNtQuerySystemInformation)
				 ( IN	SYSTEM_INFORMATION_CLASS SystemInformationClass,
				   OUT	PVOID					 SystemInformation,
				   IN	ULONG					 SystemInformationLength,
				   OUT	PULONG					 ReturnLength OPTIONAL );



// 
// typedef enum OF_FILE_INFO_BY_HANDLE_CLASS
// {
// 	OF_FileNameInfo  = 2,
// 	OF_FileStreamInfo = 7
// };
// 
// struct OF_FILE_NAME_INFO
// {
// 	DWORD  FileNameLength;
// 	WCHAR FileName[MAX_PATH];
// };
// 
// struct OF_FILE_STREAM_INFO
// {  
// 	DWORD NextEntryOffset;
// 	DWORD StreamNameLength;
// 	LARGE_INTEGER StreamSize;
// 	LARGE_INTEGER StreamAllocationSize; 
// 	WCHAR StreamName[MAX_PATH];
// };
// 
// 
// typedef BOOL (WINAPI *PGetFileInformationByHandleEx)(
// 	HANDLE hFile,
// 	OF_FILE_INFO_BY_HANDLE_CLASS FileInformationClass,
// 	LPVOID lpFileInformation,
// 	DWORD dwBufferSize);

typedef DWORD (WINAPI* GetFinalPathNameByHandleDef)(
	HANDLE hFile,
	LPWSTR lpszFilePath,
	DWORD cchFilePath,
	DWORD dwFlags	);
	


BOOL StopAndUninstallDrv( HANDLE hDrvHandle );

HANDLE ExtractAndInstallDrv();

BOOL GetDrive( LPCTSTR lpDeviceFileName, CString& fsFileName, bool bDriveLetterOnly );

BOOL EnableTokenPrivilege(LPCTSTR pszPrivilege);
#endif
