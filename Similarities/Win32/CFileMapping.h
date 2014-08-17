///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_FILE_MAPPING__
#define __WIN32__C_FILE_MAPPING__

///////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////

	class
	CFileMapping
	:
		public CHandle
	{
	public :

		STDMETHODIMP
		CreateA
		(
			HANDLE hFile,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwProtection,
			LONGLONG llMaximumSize = 0,
			LPCSTR pszName = NULL
		);

		STDMETHODIMP
		CreateW
		(
			HANDLE hFile,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwProtection,
			LONGLONG llMaximumSize = 0,
			LPCWSTR pszName = NULL
		);

		STDMETHODIMP
		Create
		(
			HANDLE hFile,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwProtection,
			LONGLONG llMaximumSize = 0,
			LPCTSTR pszName = NULL
		);

	private :

		HRESULT
		ActualCreate
		(
			bool bUnicode,
			HANDLE hFile,
			LPSECURITY_ATTRIBUTES lpSecurityAttributes,
			DWORD dwProtection,
			LONGLONG llMaximumSize,
			LPCVOID lpcvName
		);
	};

	///////////////////////////////////////////////////////////////////////////////

	class
	CFileView
	:
		private THandle< LPVOID >
	{
	public :

		STDMETHODIMP
		Init
		(
			HANDLE hFileMapping,
			DWORD dwDesiredAccess,
			LONGLONG llFileOffset = 0,
			SIZE_T NumberOfBytesToMap = 0
		);

		const
		LPVOID
		cpv()
		const;

		LPVOID
		pv();

		const
		BYTE *
		cpb()
		const;

		BYTE *
		pb();

		STDMETHODIMP
		GetSize
		(
			SIZE_T &refNumberOfBytes
		);

		~CFileView();

	protected :

		//virtual
		HRESULT
		CloseHandle();
	};

	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////