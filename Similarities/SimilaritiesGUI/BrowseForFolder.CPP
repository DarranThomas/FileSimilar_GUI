#include "stdafx.h"
#include "BrowseForFolder.H"

struct	BrowseData
{
	LPCTSTR				pszTitle;
	LPCTSTR				pszInitDir;
	UINT				ExtFlags;
};

BOOL CheckNotReadOnly(LPCTSTR pszDir)
{
//	Return TRUE if ReadOnly
//	DWORD	Attributes = GetFileAttributes(pszDir);
//	return Attributes & FILE_ATTRIBUTE_READONLY;
	
	CString	Path = pszDir;
	
	if(!Path.IsEmpty() && (Path.Right(1) != '\\'))
	{
		Path += '\\';
	}

	Path += "XXXTemp";

	if(CreateDirectory(Path, NULL))
	{
		RemoveDirectory(Path);
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	BrowseData	*pbd = (BrowseData *) lpData;
	
	switch(uMsg)
	{
		case BFFM_INITIALIZED:
			::SendMessage(hwnd, BFFM_SETSELECTION, (WPARAM) TRUE, (LPARAM) pbd->pszInitDir);
			break;

		case BFFM_SELCHANGED:
		{
			TCHAR pszBuffer[MAX_PATH];
		
			if (::SHGetPathFromIDList((LPCITEMIDLIST) lParam, pszBuffer))
			{
				if (pbd->ExtFlags & BFF_CHECK_NOT_READONLY)
				{
					if (CheckNotReadOnly(pszBuffer))
					::SendMessage(hwnd, BFFM_ENABLEOK, (WPARAM) FALSE, (LPARAM) 0);
				}
			}
		}

		break;
	}

	return 0;
}

CString BrowseForFolder(CWnd *pParent, LPCTSTR pszTitle, LPCTSTR pszInitDir, UINT ExtFlags, UINT Flags)
{
	CString		Folder;

	LPMALLOC pMalloc;		/* Gets the Shell's default allocator */
	
	if(::SHGetMalloc(&pMalloc) == NOERROR)
	{
		TCHAR				pszBuffer[MAX_PATH];
		BrowseData			bd;
		bd.pszTitle			= pszTitle;
		bd.pszInitDir		= pszInitDir;
		bd.ExtFlags			= ExtFlags;

		BROWSEINFO			bi;
		bi.hwndOwner		= pParent->GetSafeHwnd();
		bi.pidlRoot			= NULL;
		bi.pszDisplayName	= pszBuffer;
		bi.lpszTitle		= pszTitle;
		bi.ulFlags			= Flags;
		bi.lpfn				= BrowseCallbackProc;
		bi.lParam			= (LPARAM) &bd;
		bi.iImage			= 0;

		LPITEMIDLIST	pidl;

		if((pidl = ::SHBrowseForFolder(&bi)) != NULL)
		{
			if(::SHGetPathFromIDList(pidl, pszBuffer))
				Folder = pszBuffer;

			// Free the PIDL allocated by SHBrowseForFolder.
			pMalloc->Free(pidl);
		}

		// Release the shell's allocator.
		pMalloc->Release();
	}

	return Folder;
}
