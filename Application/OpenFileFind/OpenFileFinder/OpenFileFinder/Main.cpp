// Main.cpp : implementation file
//
#include "stdafx.h"
#include "Main.h"
#include "Utils.h"
#include <Tlhelp32.h>
#include <Psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// Main 

Main::Main(CString csPath)
{
	this->socket = CreateTcpClient("127.0.0.1", 8000);
	m_csPath = csPath;
	m_csPath.Replace( L"\"", L"" );
	clock_t t ;
    char mes[1000];
	HANDLE hDriver = GetDriver();
	while (1)
	{
		//t = clock();
		try
		{       
			GetOpenedFiles( m_csPath, FILES_ONLY, CallBackFunc, (ULONG_PTR)this, hDriver );
		}
		catch(...)
		{
			//Send("Exception occured in Main", this->socket);
			OutputDebugString( _T("Exception occured in Main"));
		}
		//sprintf(mes, "Time taken: %.4fs", (float)(clock() - t)/CLOCKS_PER_SEC);
		//Send(mes, this->socket);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Main message handlers



/*
void Main::OnMainClosehandle()
{	
	DWORD dwPid = _ttoi( csPid );
	HANDLE hProcess = OpenProcess( PROCESS_DUP_HANDLE , FALSE, dwPid );
	if( !hProcess )
	{
		AfxMessageBox( L"Failed to open the process" );
		return;
	}
	HANDLE hDup = 0;
	BOOL b = DuplicateHandle( hProcess, hFile, GetCurrentProcess(), &hDup, DUPLICATE_SAME_ACCESS , FALSE, DUPLICATE_CLOSE_SOURCE );
	if( hDup )
	{
		CloseHandle( hDup );
	}    
	CloseHandle( hProcess );
	//b = DeleteFile( csFileName );
	Populate();
}
*/


void CALLBACK Main::CallBackFunc( OF_INFO_t OpenedFileInfo, UINT_PTR pUserContext )
{
	((Main*)pUserContext)->OnFileFound(OpenedFileInfo);
}

void Main::SendToGui(OF_INFO_t OpenedFileInfo, PROCESS_INFO_t& stInfo)
{
	char *FName = new char[256 + 1];
	memset(FName, 0, 256 + 1);
	wcstombs(FName, OpenedFileInfo.lpFile, 256);
	char *PName = new char[256 + 1];
	memset(PName, 0, 256 + 1);
	wcstombs(PName, stInfo.csProcess, 256);
	char mes[1000];
	sprintf(mes, "%d#%s#%s#%d", OpenedFileInfo.dwPID, PName, FName, stInfo.count);
	Send(mes, this->socket);
}



void Main::OnFileFound(OF_INFO_t OpenedFileInfo )
{
	PROCESS_INFO_t stInfo = {0};
	if( !m_stProcessInfo.Lookup( OpenedFileInfo.dwPID, stInfo))
	{
		TCHAR tcFileName[MAX_PATH];
		CString csModule;
		HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, TRUE, OpenedFileInfo.dwPID );
		if( !hProcess || !GetProcessImageFileName( hProcess, tcFileName, MAX_PATH ))
		{
			if( hProcess )
			{
				CloseHandle( hProcess );
			}
			if( OpenedFileInfo.dwPID == 4 )// system process
			{
				stInfo.csProcess = L"System";
			}
			else
			{
				stInfo.csProcess = L"Unknown Process";
			}
		}
		else
		{
			GetDrive( tcFileName, csModule, false );
			CloseHandle( hProcess );		
			PathStripPath( tcFileName );
			stInfo.csProcess = tcFileName;	
			stInfo.count = 1;
		}
		m_stProcessInfo[OpenedFileInfo.dwPID] = stInfo;
		SendToGui(OpenedFileInfo, stInfo);
	}
	else
	{
		m_stProcessInfo[OpenedFileInfo.dwPID].count++;
		SendToGui(OpenedFileInfo, stInfo);
	}
}