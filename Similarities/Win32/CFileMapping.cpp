///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"
#include "THandle.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CFileMapping.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFileMapping::CreateA
	(
		HANDLE hFile,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwProtection,
		LONGLONG llMaximumSize,
		LPCSTR pszName
	)
	{
		return ActualCreate( false, hFile, lpSecurityAttributes, dwProtection, llMaximumSize, pszName );
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFileMapping::CreateW
	(
		HANDLE hFile,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwProtection,
		LONGLONG llMaximumSize,
		LPCWSTR pszName
	)
	{
		return ActualCreate( true, hFile, lpSecurityAttributes, dwProtection, llMaximumSize, pszName );
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFileMapping::Create
	(
		HANDLE hFile,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwProtection,
		LONGLONG llMaximumSize,
		LPCTSTR pszName
	)
	{
#if defined( _UNICODE )
		return CreateW( hFile, lpSecurityAttributes, dwProtection, llMaximumSize, pszName );
#else
		return CreateA( hFile, lpSecurityAttributes, dwProtection, llMaximumSize, pszName );
#endif
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	HRESULT
	CFileMapping::ActualCreate
	(
		bool bUnicode,
		HANDLE hFile,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwProtection,
		LONGLONG llMaximumSize,
		LPCVOID lpcvName
	)
	{
		HRESULT hr = S_OK;

		if( IsValid() )
		{
			hr = E_UNEXPECTED;
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			DWORD dwMaximumSizeHigh = static_cast< DWORD >( llMaximumSize >> 32 );
			DWORD dwMaximumSizeLow = static_cast< DWORD >( llMaximumSize );

			if( bUnicode )
			{
				LPCWSTR pszName = reinterpret_cast< LPCWSTR >( lpcvName );

				m_h = ::CreateFileMappingW( hFile, lpSecurityAttributes, dwProtection, dwMaximumSizeHigh, dwMaximumSizeLow, pszName );
			}
			else
			{
				LPCSTR pszName = reinterpret_cast< LPCSTR >( lpcvName );

				m_h = ::CreateFileMappingA( hFile, lpSecurityAttributes, dwProtection, dwMaximumSizeHigh, dwMaximumSizeLow, pszName );
			}

			if( IsInvalid() )
			{
				// we may get ERROR_FILE_NOT_FOUND or ERROR_PIPE_BUSY
				// when opening pipe at client side - we don't want to assert them

				DWORD dw = ::GetLastError();

				if( ERROR_FILE_NOT_FOUND == dw )
				{
					hr = MAKE_HRESULT_WIN32_FAILED( ERROR_FILE_NOT_FOUND );
				}
				else
				if( ERROR_PIPE_BUSY == dw )
				{
					hr = MAKE_HRESULT_WIN32_FAILED( ERROR_PIPE_BUSY );
				}
				else
				{
					hr = HRESULT_FROM_WIN32( dw );
					ASSERT_SUCCEEDED( hr );
				}
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFileView::Init
	(
		HANDLE hFileMapping,
		DWORD dwDesiredAccess,
		LONGLONG llFileOffset,
		SIZE_T NumberOfBytesToMap
	)
	{
		HRESULT hr = S_OK;

		if( IsValid() )
		{
			hr = E_UNEXPECTED;
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			DWORD dwFileOffsetHigh = static_cast< DWORD >( llFileOffset >> 32 );
			DWORD dwFileOffsetLow = static_cast< DWORD >( llFileOffset );

			m_h = ::MapViewOfFile( hFileMapping, dwDesiredAccess, dwFileOffsetHigh, dwFileOffsetLow, NumberOfBytesToMap );

			if( IsInvalid() )
			{
				hr = GetLastError();
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	const
	LPVOID
	CFileView::cpv()
	const
	{
		return m_h;
	}

	///////////////////////////////////////////////////////////////////////////////

	LPVOID
	CFileView::pv()
	{
		return m_h;
	}

	///////////////////////////////////////////////////////////////////////////////

	const
	BYTE *
	CFileView::cpb()
	const
	{
		return reinterpret_cast< LPBYTE >( m_h );
	}

	///////////////////////////////////////////////////////////////////////////////

	BYTE *
	CFileView::pb()
	{
		return reinterpret_cast< LPBYTE >( m_h );
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFileView::GetSize
	(
		SIZE_T &refNumberOfBytes
	)
	{
		HRESULT hr = S_OK;

		MEMORY_BASIC_INFORMATION MemoryBasicInformation;

		MemoryBasicInformation.RegionSize = 0;

		SIZE_T NumberofBytesReturned = ::VirtualQuery( m_h, &MemoryBasicInformation, sizeof( MEMORY_BASIC_INFORMATION ) );

		if( 0 == NumberofBytesReturned )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			refNumberOfBytes = MemoryBasicInformation.RegionSize;
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	CFileView::~CFileView()
	{
		HRESULT hr = Free();
		ASSERT_SUCCEEDED( hr );
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//virtual
	HRESULT
	CFileView::CloseHandle()
	{
		HRESULT hr = S_OK;

		BOOL br = ::UnmapViewOfFile( m_h );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////