// OpenFileFinder.cpp : Implementation of DLL Exports.


#include "stdafx.h"
#include "OpenFileFinder.h"


class COpenFileFinderModule : public CAtlDllModuleT< COpenFileFinderModule >
{
public :
	DECLARE_LIBID(LIBID_OpenFileFinderLib)
};

COpenFileFinderModule _AtlModule;

class COpenFileFinderApp : public CWinApp
{
public:

// Overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(COpenFileFinderApp, CWinApp)
END_MESSAGE_MAP()

COpenFileFinderApp theApp;

BOOL COpenFileFinderApp::InitInstance()
{
    return CWinApp::InitInstance();
}

int COpenFileFinderApp::ExitInstance()
{
    return CWinApp::ExitInstance();
}


// Used to determine whether the DLL can be unloaded by OLE
STDAPI DllCanUnloadNow(void)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return (AfxDllCanUnloadNow()==S_OK && _AtlModule.GetLockCount()==0) ? S_OK : S_FALSE;
}


// Returns a class factory to create an object of the requested type
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
    return _AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
  
    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
        {
            // handle error
        }
    }
    return bIsWow64;
}


// DllRegisterServer - Adds entries to the system registry
STDAPI DllRegisterServer(void)
{
    if( IsWow64() )
    {
        AfxMessageBox( L"Cannot register 32bit of this dll, use 64 bit version instead" );
		return E_FAIL;
    }
	OSVERSIONINFO info = { 0 }; 
	info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&info); 
	if( info.dwMajorVersion != 5 && info.dwMajorVersion != 6 )
	{
		// since I have build the driver only for widows xp and vista, this application cannot run on other os versions.
		AfxMessageBox( L"Sorry this can only be installed only in windows XP/2003 and Vista" );
		return E_FAIL;
	}
    // registers object, typelib and all interfaces in typelib
    HRESULT hr = _AtlModule.DllRegisterServer(FALSE);
	return hr;
}


// DllUnregisterServer - Removes entries from the system registry
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = _AtlModule.DllUnregisterServer(FALSE);
	return hr;
}

