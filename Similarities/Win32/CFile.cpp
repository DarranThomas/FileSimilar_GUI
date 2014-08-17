///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"
#include "THandle.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CFile.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::OpenA
	(
		LPCSTR pszFilePath,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlags,
		HANDLE hTemplate
	)
	{
		return ActualOpen( false, pszFilePath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlags, hTemplate );
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::OpenW
	(
		LPCWSTR pszFilePath,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlags,
		HANDLE hTemplate
	)
	{
		return ActualOpen( true, pszFilePath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlags, hTemplate );
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::Open
	(
		LPCTSTR pszFilePath,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlags,
		HANDLE hTemplate
	)
	{
#if defined( _UNICODE )
		return OpenW( pszFilePath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlags, hTemplate );
#else
		return OpenA( pszFilePath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlags, hTemplate );
#endif
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::Read
	(
		VOID *pvBuffer,
		DWORD dwNumberofBytesToRead,
		DWORD *pdwNumberofBytesRead,
		LPOVERLAPPED lpOverlapped
	)
	{
		HRESULT hr = S_OK;

		DWORD dwNumberofBytesRead = 0;

		BOOL br = ::ReadFile( m_h, pvBuffer, dwNumberofBytesToRead, &dwNumberofBytesRead, lpOverlapped );

		if( !br )
		{
			DWORD dw = ::GetLastError();

			if( ERROR_IO_PENDING == dw )
			{
				hr = MAKE_HRESULT_WIN32_SUCCEEDED( ERROR_IO_PENDING );
			}
			else
			{
				hr = HRESULT_FROM_WIN32( dw );
				ASSERT_SUCCEEDED( hr );
			}
		}
		else
		{
			if( 0 == dwNumberofBytesRead )
			{
				hr = HRESULT_FROM_WIN32( ERROR_HANDLE_EOF );
				ASSERT_SUCCEEDED( hr );
			}
			else
			if( dwNumberofBytesToRead != dwNumberofBytesRead )
			{
				hr = HRESULT_FROM_WIN32( ERROR_PARTIAL_COPY );
				ASSERT_SUCCEEDED( hr );
			}
		}

		if( IS_NOT_NULL( pdwNumberofBytesRead ) )
		{
			if( IS_BAD_WRITE( pdwNumberofBytesRead, sizeof( DWORD ) ) )
			{
				HRESULT hr2 = E_POINTER;
				ASSERT_SUCCEEDED( hr2 );

				REPLACE_IF_FAILED( hr, hr2 );
			}
			else
			{
				( *pdwNumberofBytesRead ) = dwNumberofBytesRead;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::Write
	(
		const
		VOID *cpvBuffer,
		DWORD dwNumberofBytesToWrite,
		DWORD *pdwNumberofBytesWritten,
		LPOVERLAPPED lpOverlapped
	)
	{
		HRESULT hr = S_OK;

		DWORD dwNumberofBytesWritten = 0;

		BOOL br = ::WriteFile( m_h, cpvBuffer, dwNumberofBytesToWrite, &dwNumberofBytesWritten, lpOverlapped );

		if( !br )
		{
			DWORD dw = ::GetLastError();

			if( ERROR_IO_PENDING == dw )
			{
				hr = MAKE_HRESULT_WIN32_SUCCEEDED( ERROR_IO_PENDING );
			}
			else
			// following one is pipe-related
			if( ERROR_NO_DATA == dw )
			{
				hr = MAKE_HRESULT_WIN32_SUCCEEDED( ERROR_NO_DATA );
			}
			else
			{
				hr = HRESULT_FROM_WIN32( dw );
				ASSERT_SUCCEEDED( hr );
			}
		}
		else
		{
			if( dwNumberofBytesToWrite != dwNumberofBytesWritten )
			{
				hr = HRESULT_FROM_WIN32( ERROR_PARTIAL_COPY );
				ASSERT_SUCCEEDED( hr );
			}
		}

		if( IS_NOT_NULL( pdwNumberofBytesWritten ) )
		{
			if( IS_BAD_WRITE( pdwNumberofBytesWritten, sizeof( DWORD ) ) )
			{
				HRESULT hr2 = E_POINTER;
				ASSERT_SUCCEEDED( hr2 );

				REPLACE_IF_FAILED( hr, hr2 );
			}
			else
			{
				( *pdwNumberofBytesWritten ) = dwNumberofBytesWritten;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::GetOverlappedResult
	(
		LPOVERLAPPED lpOverlapped,
		DWORD *pdwNumberofBytesTransferred,
		BOOL bWait
	)
	{
		HRESULT hr = S_OK;

		DWORD dwNumberofBytesTransferred = 0;

		BOOL br = ::GetOverlappedResult( m_h, lpOverlapped, &dwNumberofBytesTransferred, bWait );

		if( !br )
		{
			DWORD dw = ::GetLastError();

			// following one is pipe-related
			if( ERROR_PIPE_NOT_CONNECTED == dw )
			{
				hr = MAKE_HRESULT_WIN32_SUCCEEDED( ERROR_PIPE_NOT_CONNECTED );
			}
			else
			{
				hr = HRESULT_FROM_WIN32( dw );
				ASSERT_SUCCEEDED( hr );
			}
		}

		if( IS_NOT_NULL( pdwNumberofBytesTransferred ) )
		{
			if( IS_BAD_WRITE( pdwNumberofBytesTransferred, sizeof( DWORD ) ) )
			{
				HRESULT hr2 = E_POINTER;
				ASSERT_SUCCEEDED( hr2 );

				REPLACE_IF_FAILED( hr, hr2 );
			}
			else
			{
				( *pdwNumberofBytesTransferred ) = dwNumberofBytesTransferred;
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CFile::CancelIO()
	{
		HRESULT hr = S_OK;

		BOOL br = ::CancelIo( m_h );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	HRESULT
	CFile::ActualOpen
	(
		bool bUnicode,
		LPCVOID lpcvFilePath,
		DWORD dwDesiredAccess,
		DWORD dwShareMode,
		LPSECURITY_ATTRIBUTES lpSecurityAttributes,
		DWORD dwCreationDisposition,
		DWORD dwFlags,
		HANDLE hTemplate
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
			if( bUnicode )
			{
				LPCWSTR pszFilePath = reinterpret_cast< LPCWSTR >( lpcvFilePath );

				m_h = ::CreateFileW( pszFilePath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlags, hTemplate );
			}
			else
			{
				LPCSTR pszFilePath = reinterpret_cast< LPCSTR >( lpcvFilePath );

				m_h = ::CreateFileA( pszFilePath, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlags, hTemplate );
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

}

///////////////////////////////////////////////////////////////////////////////////