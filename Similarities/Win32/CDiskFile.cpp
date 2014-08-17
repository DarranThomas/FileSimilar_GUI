///////////////////////////////////////////////////////////////////////////////////

#include "pch.h"

///////////////////////////////////////////////////////////////////////////////////

#include "macro.h"
#include "THandle.h"
#include "CFile.h"

///////////////////////////////////////////////////////////////////////////////////

#include "CDiskFile.h"

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CDiskFile::GetTempPathA
	(
		LPSTR pszBuffer,
		DWORD dwSizeofBuffer,
		DWORD *pdwSizeofBufferUsed
	)
	{
		return ActualGetTempPath( false, pszBuffer, dwSizeofBuffer, pdwSizeofBufferUsed );
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CDiskFile::GetTempPathW
	(
		LPWSTR pszBuffer,
		DWORD dwSizeofBuffer,
		DWORD *pdwSizeofBufferUsed
	)
	{
		return ActualGetTempPath( true, pszBuffer, dwSizeofBuffer, pdwSizeofBufferUsed );
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CDiskFile::GetTempPath
	(
		LPTSTR pszBuffer,
		DWORD dwSizeofBuffer,
		DWORD *pdwSizeofBufferUsed
	)
	{
#if defined( _UNICODE )
		return ActualGetTempPath( true, pszBuffer, dwSizeofBuffer, pdwSizeofBufferUsed );
#else
		return ActualGetTempPath( false, pszBuffer, dwSizeofBuffer, pdwSizeofBufferUsed );
#endif
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	__cdecl
	CDiskFile::GetTempFilePathW
	(
		LPWSTR pszBuffer,
		DWORD dwSizeofBuffer,
		LPCWSTR pszFileNameFormat,
		...
	)
	{
		HRESULT hr = S_OK;

		if( IS_NULL( pszFileNameFormat ) )
		{
			hr = E_INVALIDARG;
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			DWORD dwSizeofBufferUsed = 0;

			hr = GetTempPathW( pszBuffer, dwSizeofBuffer, &dwSizeofBufferUsed );
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				va_list args;

				va_start( args, pszFileNameFormat );

				WCHAR szFileName[ MAX_PATH ];

				hr = ::StringCchVPrintfW( szFileName, sizeof( szFileName ) / sizeof( WCHAR ), pszFileNameFormat, args );
				ASSERT_SUCCEEDED( hr );

				if( SUCCEEDED( hr ) )
				{
					hr = ::StringCchCatW( pszBuffer, ( dwSizeofBuffer - dwSizeofBufferUsed ) / sizeof( WCHAR ), szFileName );
					ASSERT_SUCCEEDED( hr );
				}

				va_end( args );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CDiskFile::CopyW
	(
		LPCWSTR pszFilePathSrc,
		LPCWSTR pszFilePathDst,
		BOOL bFailIfExist
	)
	{
		HRESULT hr = S_OK;

		BOOL br = ::CopyFileW( pszFilePathSrc, pszFilePathDst, bFailIfExist );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CDiskFile::MoveW
	(
		LPCWSTR pszFilePathSrc,
		LPCWSTR pszFilePathDst
	)
	{
		HRESULT hr = S_OK;

		DWORD dwFlags = 0;
		dwFlags |= MOVEFILE_COPY_ALLOWED;
		dwFlags |= MOVEFILE_REPLACE_EXISTING;
		dwFlags |= MOVEFILE_WRITE_THROUGH;

		BOOL br = ::MoveFileExW( pszFilePathSrc, pszFilePathDst, dwFlags );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	STDAPICALLTYPE
	CDiskFile::DeleteW
	(
		LPCWSTR pszFilePath
	)
	{
		HRESULT hr = S_OK;

		BOOL br = ::DeleteFileW( pszFilePath );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CDiskFile::GetSize
	(
		LONGLONG &refllSize
	)
	{
		HRESULT hr = S_OK;

		DWORD dwHighPart = 0;

		DWORD dwLowPart = ::GetFileSize( m_h, &dwHighPart );

		if( INVALID_FILE_SIZE == dwLowPart )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		if( SUCCEEDED( hr ) )
		{
			refllSize = ( static_cast< LONGLONG >( dwHighPart ) << 32 ) | dwLowPart;
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CDiskFile::Flush()
	{
		HRESULT hr = S_OK;

		BOOL br = ::FlushFileBuffers( m_h );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CDiskFile::GetCurrentPosition
	(
		LONGLONG &refllCurrentPosition
	)
	{
		HRESULT hr = S_OK;

		LONG DistanceLow = 0;
		LONG DistanceHigh = 0;

		DWORD dw = ::SetFilePointer( m_h, DistanceLow, &DistanceHigh, FILE_CURRENT );

		if( INVALID_SET_FILE_POINTER == dw )
		{
			DWORD dwError = ::GetLastError();

			if( ERROR_SUCCESS != dwError )
			{
				hr = HRESULT_FROM_WIN32( dwError );
				ASSERT_SUCCEEDED( hr );
			}
		}

		if( SUCCEEDED( hr ) )
		{
			refllCurrentPosition = ( static_cast< LONGLONG >( DistanceHigh ) << 32 ) | dw;
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CDiskFile::Seek
	(
		LONGLONG llDistanceToMove,
		DWORD dwMoveMethod
	)
	{
		HRESULT hr = S_OK;

		LONG lDistanceLow = static_cast< LONG >( llDistanceToMove );
		LONG lDistanceHigh = static_cast< LONG >( llDistanceToMove >> 32 );

		DWORD dw = ::SetFilePointer( m_h, lDistanceLow, &lDistanceHigh, dwMoveMethod );

		if( INVALID_SET_FILE_POINTER == dw )
		{
			DWORD dwError = ::GetLastError();

			if( ERROR_SUCCESS != dwError )
			{
				hr = HRESULT_FROM_WIN32( dwError );
				ASSERT_SUCCEEDED( hr );
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	CDiskFile::SetEOF()
	{
		HRESULT hr = S_OK;

		BOOL br = ::SetEndOfFile( m_h );

		if( !br )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	//static
	HRESULT
	CDiskFile::ActualGetTempPath
	(
		bool bUnicode,
		LPVOID lpvBuffer,
		DWORD dwSizeofBuffer,
		DWORD *pdwSizeofBufferUsed
	)
	{
		HRESULT hr = S_OK;

		DWORD dw;

		if( bUnicode )
		{
			LPWSTR pszBuffer = reinterpret_cast< LPWSTR >( lpvBuffer );

			dw = ::GetTempPathW( dwSizeofBuffer / sizeof( WCHAR ), pszBuffer );
		}
		else
		{
			LPSTR pszBuffer = reinterpret_cast< LPSTR >( lpvBuffer );

			dw = ::GetTempPathA( dwSizeofBuffer / sizeof( CHAR ), pszBuffer );
		}

		if( 0 == dw )
		{
			hr = GetLastError();
			ASSERT_SUCCEEDED( hr );
		}
		else
		{
			if( IS_NOT_NULL( pdwSizeofBufferUsed ) )
			{
				if( IS_BAD_WRITE( pdwSizeofBufferUsed, sizeof( DWORD ) ) )
				{
					hr = E_POINTER;
					ASSERT_SUCCEEDED( hr );
				}
				else
				{
					if( bUnicode )
					{
						( *pdwSizeofBufferUsed ) = ( dw + 1 ) * sizeof( WCHAR );
					}
					else
					{
						( *pdwSizeofBufferUsed ) = ( dw + 1 ) * sizeof( CHAR );
					}
				}
			}
		}

		return hr;
	}

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////