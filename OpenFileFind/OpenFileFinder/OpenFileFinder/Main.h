#if !defined(AFX_MAINDLG_H__4D07C7BC_0870_45EB_9823_9A4A081D76B5__INCLUDED_)
#define AFX_MAINDLG_H__4D07C7BC_0870_45EB_9823_9A4A081D76B5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainDlg.h : header file
//

#include <afxctl.h>
#include <afxcmn.h>
#include <afxtempl.h>
#include "OpenedFiles.h"

struct PROCESS_INFO_t
{
    CString csProcess;
    DWORD dwImageListIndex;
};

// MainDlg dialog
class Main 
{
// Construction
public:
	
	Main( CString csPath );
    	
	bool InstallDriver();
    
	void EnumerateOpenedFiles( HANDLE hDriver );
    
	void EnumerateLoadedModules();

	static void CALLBACK CallBackFunc(OF_INFO_t OpenedFileInf0, UINT_PTR pUserContext );
	
	void OnFileFound(OF_INFO_t OpenedFileInf0 );

   void Populate();

private:

    CString		m_csPath;
    CMap<int,int,PROCESS_INFO_t,PROCESS_INFO_t&> m_stProcessInfo;
	DWORD	   m_nCount;
public:
//	afx_msg void OnMainClosehandle();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINDLG_H__4D07C7BC_0870_45EB_9823_9A4A081D76B5__INCLUDED_)
