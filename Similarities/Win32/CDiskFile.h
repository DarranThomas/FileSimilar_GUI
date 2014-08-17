///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_DISK_FILE__
#define __WIN32__C_DISK_FILE__

///////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_FILE__
	#error #include <Win32/CFile.h>
#endif

///////////////////////////////////////////////////////////////////////////////////

#if defined( GetTempPath )
	#undef GetTempPath
#endif

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////

	class
	CDiskFile
	:
		public CFile
	{
	public :

		static
		HRESULT
		STDAPICALLTYPE
		GetTempPathA
		(
			LPSTR pszBuffer,
			DWORD dwSizeofBuffer,
			DWORD *pdwSizeofBufferUsed = NULL
		);

		static
		HRESULT
		STDAPICALLTYPE
		GetTempPathW
		(
			LPWSTR pszBuffer,
			DWORD dwSizeofBuffer,
			DWORD *pdwSizeofBufferUsed = NULL
		);

		static
		HRESULT
		STDAPICALLTYPE
		GetTempPath
		(
			LPTSTR pszBuffer,
			DWORD dwSizeofBuffer,
			DWORD *pdwSizeofBufferUsed = NULL
		);

		static
		HRESULT
		__cdecl
		GetTempFilePathW
		(
			LPWSTR pszBuffer,
			DWORD dwSizeofBuffer,
			LPCWSTR pszFileNameFormat = NULL,
			...
		);

		static
		HRESULT
		STDAPICALLTYPE
		CopyW
		(
			LPCWSTR pszFilePathSrc,
			LPCWSTR pszFilePathDst,
			BOOL bFailIfExist = FALSE
		);

		static
		HRESULT
		STDAPICALLTYPE
		MoveW
		(
			LPCWSTR pszFilePathSrc,
			LPCWSTR pszFilePathDst
		);

		static
		HRESULT
		STDAPICALLTYPE
		DeleteW
		(
			LPCWSTR pszFilePath
		);

	public :

		STDMETHODIMP
		GetSize
		(
			LONGLONG &refllSize
		);

		STDMETHODIMP
		Flush();

		STDMETHODIMP
		GetCurrentPosition
		(
			LONGLONG &refllCurrentPosition
		);

		STDMETHODIMP
		Seek
		(
			LONGLONG llDistanceToMove,
			DWORD dwMoveMethod = FILE_CURRENT
		);

		STDMETHODIMP
		SetEOF();

	private :

		static
		HRESULT
		ActualGetTempPath
		(
			bool bUnicode,
			LPVOID lpvBuffer,
			DWORD dwSizeofBuffer,
			DWORD *pdwSizeofBufferUsed = NULL
		);
	};

	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////