// InterFaceCls.h : Declaration of the CInterFaceCls

#pragma once
#include <shlobj.h>
#include <comdef.h>
#include "OpenFileFinder.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif



// CInterFaceCls

class ATL_NO_VTABLE CInterFaceCls :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CInterFaceCls, &CLSID_InterFaceCls>,
	//public IInterFaceCls
	public IShellExtInit,
	public IContextMenu
{
public:
	CInterFaceCls()
	{
	}

DECLARE_NOT_AGGREGATABLE(CInterFaceCls)

BEGIN_COM_MAP(CInterFaceCls)
	//COM_INTERFACE_ENTRY(IInterFaceCls)
	COM_INTERFACE_ENTRY(IShellExtInit)
	COM_INTERFACE_ENTRY(IContextMenu)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:

	STDMETHODIMP Initialize( LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hProgID );
	
	STDMETHODIMP GetCommandString( UINT_PTR idCmd, UINT uFlags, UINT* pwReserved, LPSTR pszName, UINT cchMax );

	STDMETHODIMP InvokeCommand( LPCMINVOKECOMMANDINFO pCmdInfo );

	STDMETHODIMP QueryContextMenu( HMENU hmenu, UINT uMenuIndex, UINT uidFirstCmd, UINT uidLastCmd, UINT uFlags );

private:

	CString m_csPath;
};

