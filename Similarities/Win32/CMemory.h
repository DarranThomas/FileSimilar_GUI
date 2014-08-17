///////////////////////////////////////////////////////////////////////////////////

#ifndef __WIN32__C_MEMORY__
#define __WIN32__C_MEMORY__

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
	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeAllocateMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeAllocateAlignedMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes,
		size_t Alignment = 16
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeReallocateMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeReallocateAlignedMemory
	(
		LPVOID &lpv,
		size_t NumberofBytes,
		size_t Alignment = 16
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeZeroMemory
	(
		LPVOID lpv,
		size_t NumberofBytes
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeCopyMemory
	(
		LPVOID lpvDst,
		LPCVOID lpcvSrc,
		size_t NumberofBytes
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeFreeMemory
	(
		LPVOID &lpv
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////

	STDMETHODIMP
	safeFreeAlignedMemory
	(
		LPVOID &lpv
	)
	throw();

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	interface
	IMemoryAllocator
	{
	public :

		virtual
		STDMETHODIMP
		AllocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		= 0;

		virtual
		STDMETHODIMP
		ReallocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		= 0;

		virtual
		STDMETHODIMP
		FreeMemory
		(
			LPVOID &reflpv
		)
		= 0;
	};

	///////////////////////////////////////////////////////////////////////////////

	class
	CMemoryAllocator
	:
		public IMemoryAllocator
	{
	public :

		//virtual
		STDMETHODIMP
		AllocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		);

		//virtual
		STDMETHODIMP
		ReallocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		);

		//virtual
		STDMETHODIMP
		FreeMemory
		(
			LPVOID &reflpv
		);
	};

	///////////////////////////////////////////////////////////////////////////////

	template
	<
		int TAlignment = 16
	>
	class
	CAlignedMemoryAllocator
	:
		public IMemoryAllocator
	{
	public :

		//virtual
		STDMETHODIMP
		AllocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		{
			return safeAllocateAlignedMemory( reflpv, NumberofBytes, TAlignment );
		}

		//virtual
		STDMETHODIMP
		ReallocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		{
			return safeReallocateAlignedMemory( reflpv, NumberofBytes, TAlignment );
		}

		//virtual
		STDMETHODIMP
		FreeMemory
		(
			LPVOID &reflpv
		)
		{
			return safeFreeAlignedMemory( reflpv );
		}
	};

	///////////////////////////////////////////////////////////////////////////////

	class
	CMemoryHandler
	{
	public :

		CMemoryHandler();

		CMemoryHandler
		(
			const
			CMemoryHandler &cref
		);

		const
		CMemoryHandler &
		operator =
		(
			const
			CMemoryHandler &cref
		);

		STDMETHODIMP
		Allocate
		(
			SIZE_T NumberofBytes,
			bool bZero = false
		);

		STDMETHODIMP
		Reallocate
		(
			SIZE_T NumberofBytes,
			bool bZero = false
		);

		bool
		IsValid()
		const;

		bool
		IsInvalid()
		const;

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

		SIZE_T
		NumberofBytes()
		const;

		STDMETHODIMP
		Zero();

		STDMETHODIMP
		CopyFrom
		(
			LPCVOID lpcv,
			SIZE_T NumberofBytes
		);

		STDMETHODIMP
		CopyTo
		(
			LPVOID lpv,
			SIZE_T NumberofBytes
		)
		const;

		STDMETHODIMP
		Free();

		virtual
		~CMemoryHandler();

	protected :

		LPVOID m_lpv;

		SIZE_T m_NumberofBytes;

		virtual
		HRESULT
		ActualAllocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		= 0;

		virtual
		HRESULT
		ActualReallocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		= 0;

		virtual
		HRESULT
		ActualFreeMemory
		(
			LPVOID &reflpv
		)
		= 0;
	};

	///////////////////////////////////////////////////////////////////////////////

	template
	<
		typename TMemoryAllocator
	>
	class
	TMemoryHandler
	:
		public CMemoryHandler,
		private TMemoryAllocator
	{
	public :

		//virtual
		~TMemoryHandler()
		{
			HRESULT hr = Free();
			ASSERT_SUCCEEDED( hr );
		}

	protected :

		//virtual
		HRESULT
		ActualAllocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		{
			return static_cast< IMemoryAllocator * >( static_cast< TMemoryAllocator * >( this ) )->AllocateMemory( reflpv, NumberofBytes );
		}

		//virtual
		HRESULT
		ActualReallocateMemory
		(
			LPVOID &reflpv,
			size_t NumberofBytes
		)
		{
			return static_cast< IMemoryAllocator * >( static_cast< TMemoryAllocator * >( this ) )->ReallocateMemory( reflpv, NumberofBytes );
		}

		//virtual
		HRESULT
		ActualFreeMemory
		(
			LPVOID &reflpv
		)
		{
			return static_cast< IMemoryAllocator * >( static_cast< TMemoryAllocator * >( this ) )->FreeMemory( reflpv );
		}
	};

	///////////////////////////////////////////////////////////////////////////////

	typedef
	TMemoryHandler< CMemoryAllocator >
	CMemory;

	///////////////////////////////////////////////////////////////////////////////

	typedef
	TMemoryHandler< CAlignedMemoryAllocator<  > >
	CAlignedMemory;

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////////

#define SafeAllocateMemory( pointer, size ) \
	Win32::safeAllocateMemory( reinterpret_cast< LPVOID & >( pointer ), size )

#define SafeAllocateAlignedMemory( pointer, size, alignment ) \
	Win32::safeAllocateAlignedMemory( reinterpret_cast< LPVOID & >( pointer ), size, alignment )

#define SafeReallocateMemory( pointer, size ) \
	Win32::safeReallocateMemory( reinterpret_cast< LPVOID & >( pointer ), size )

#define SafeReallocateAlignedMemory( pointer, size, alignment ) \
	Win32::safeReallocateAlignedMemory( reinterpret_cast< LPVOID & >( pointer ), size, alignment )

#define SafeZeroMemory( pointer, size ) \
	Win32::safeZeroMemory( pointer, size )

#define SafeCopyMemory( dst, src, size ) \
	Win32::safeCopyMemory( dst, src, size )

#define SafeFreeMemory( pointer ) \
	Win32::safeFreeMemory( reinterpret_cast< LPVOID & >( pointer ) )

#define SafeFreeAlignedMemory( pointer ) \
	Win32::safeFreeAlignedMemory( reinterpret_cast< LPVOID & >( pointer ) )

///////////////////////////////////////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////////////////////////////////////