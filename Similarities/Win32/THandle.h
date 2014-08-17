///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__T_HANDLE__
#define __WIN32__T_HANDLE__

///////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__MACRO__
	#error #include <Win32/macro.h>
#endif

///////////////////////////////////////////////////////////////////////////////////

namespace Win32
{

	///////////////////////////////////////////////////////////////////////////////

	template
	<
		typename T = HANDLE,
		typename T InvalidValue = NULL
	>
	class
	THandle
	{
	public :

		static
		HRESULT
		STDAPICALLTYPE
		DuplicateHandle
		(
			HANDLE hSourceProcess,
			HANDLE hSource,
			HANDLE hTargetProcess,
			HANDLE *phTarget,
			DWORD dwDesiredAccess,
			BOOL bInheritHandle,
			DWORD dwOptions
		)
		{
			HRESULT hr = S_OK;

			BOOL br = ::DuplicateHandle( hSourceProcess, hSource, hTargetProcess, phTarget, dwDesiredAccess, bInheritHandle, dwOptions );

			if( !br )
			{
				hr = Win32::GetLastError();
				ASSERT_SUCCEEDED( hr );
			}

			return hr;
		}

	public :

		THandle()
		:
			m_h( InvalidValue )
		{}

		operator
		const
		typename T &()
		const
		{
			return m_h;
		}

		inline
		BOOL
		IsInvalid()
		const
		{
			return ( InvalidValue == m_h );
		}

		inline
		BOOL
		IsValid()
		const
		{
			return ( InvalidValue != m_h );
		}

		STDMETHODIMP
		Free()
		{
			HRESULT hr = S_OK;

			if( IsInvalid() )
			{
				hr = S_FALSE;
			}
			else
			{
				hr = CloseHandle();
				ASSERT_SUCCEEDED( hr );

				if( SUCCEEDED( hr ) )
				{
					Invalidate();
				}
			}

			return hr;
		}

		virtual
		~THandle()
		{
			HRESULT hr = Free();
			ASSERT_SUCCEEDED( hr );
		}

	private :

		THandle
		(
			const
			THandle &
		);

	protected :

		typename T m_h;

		inline
		void
		Invalidate()
		{
			m_h = InvalidValue;
		}

		virtual
		HRESULT
		CloseHandle()
		{
			HRESULT hr = S_OK;

			BOOL br = ::CloseHandle( reinterpret_cast< HANDLE >( m_h ) );

			if( !br )
			{
				hr = GetLastError();
				ASSERT_SUCCEEDED( hr );
			}

			return hr;
		}
	};

	///////////////////////////////////////////////////////////////////////////////

	typedef
	THandle<>
	CHandle;

	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////