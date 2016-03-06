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
	FILE* fp = fopen("1.txt", "at");
	fprintf(fp, "Start\n");
	fclose(fp);
    m_csPath = csPath;
	m_csPath.Replace( L"\"", L"" );
	while ( 1 )
	  Populate() ;
}


/////////////////////////////////////////////////////////////////////////////
// Main message handlers


//MODULES_ONLY   FILES_ONLY
void Main::Populate() 
{
    try
    {       
		GetOpenedFiles( m_csPath, FILES_ONLY, CallBackFunc, (ULONG_PTR)this );
	}
    catch(...)
    {
        OutputDebugString( _T("Exception occured in Main::Populate()"));
    }
}


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
	if ( OpenedFileInfo.dwPID != 5600 )
	{
		FILE* fp = fopen("1.txt", "at");
		char pid[20];
		sprintf(pid, "%d", OpenedFileInfo.dwPID);
		fprintf(fp, "%s  %s\n",pid, OpenedFileInfo.lpFile);
		fclose(fp);

		((Main*)pUserContext)->OnFileFound( OpenedFileInfo );
	}
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
		}
		m_stProcessInfo[OpenedFileInfo.dwPID] = stInfo;
	}	
	// Insert Process name, PID and file name
//	m_list.InsertItem( m_nCount, stInfo.csProcess, stInfo.dwImageListIndex );                    
	CString csPid;
	csPid.Format( _T("%d ( 0x%x )"), OpenedFileInfo.dwPID , OpenedFileInfo.dwPID );			
//	m_list.SetItemText( m_nCount, 1, csPid );
//	m_list.SetItemText( m_nCount, 2, OpenedFileInfo.lpFile );
//	m_list.SetItemData( m_nCount, (DWORD_PTR)OpenedFileInfo.hFile );
//	m_nCount++;
}