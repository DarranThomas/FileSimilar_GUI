#include "stdafx.h"
#include "FileCompare.h"

#include "CRC32.h"
#include "FoundInData.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////////

// Removing spaces, tabs, semicolons.
bool g_bRemoveDelimiters = true;

// Removing carriage-returns and new-lines.
bool g_bRemoveNewLines = true;

// Estimate execution progress.
bool g_bShowProgress = true;
double g_dProgress = 0.0;

///////////////////////////////////////////////////////////////////////////////////

void helpstring()
{
	_tprintf( TEXT( "Usage : console.exe <INIFilePath>\n" ) );
}

///////////////////////////////////////////////////////////////////////////////////

TCharacterPosition
CharacterPositionFromHashOffset
(
	const TMap_HashOffset_CharacterPosition &crefMap_HashOffset_CharacterPosition,
	SIZE_T HashOffset
)
{
	SIZE_T CharacterOffset = 0;

	TMap_HashOffset_CharacterPosition::const_iterator cit = crefMap_HashOffset_CharacterPosition.lower_bound( HashOffset );

	if( cit == crefMap_HashOffset_CharacterPosition.end() )
	{
		if( cit == crefMap_HashOffset_CharacterPosition.begin() )
		{
			CharacterOffset += HashOffset;
		}
		else
		{
			-- cit;

			CharacterOffset = cit->second;

			CharacterOffset += ( HashOffset - cit->first );
		}
	}
	else
	{
		if( HashOffset == cit->first )
		{
			CharacterOffset = cit->second;
		}
		else
		{
			if( cit == crefMap_HashOffset_CharacterPosition.begin() )
			{
				CharacterOffset += HashOffset;
			}
			else
			{
				-- cit;

				CharacterOffset = cit->second;

				CharacterOffset += ( HashOffset - cit->first );
			}
		}
	}

	return TCharacterPosition( CharacterOffset / 80, CharacterOffset % 80 );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
PrepareSourceFile
(
	TSourceFile &refSourceFile,
	SIZE_T SearchLength
)
{
	HRESULT hr = S_OK;

	_tprintf( TEXT( "processing \"%s\"...\n" ), refSourceFile.pszFilePath );

	Win32::CDiskFile DiskFile;

	DWORD dwFlags = 0;

	dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;

	hr = refSourceFile.DiskFile.Open( refSourceFile.pszFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, dwFlags );
	ASSERT_SUCCEEDED( hr );

	if( SUCCEEDED( hr ) )
	{
		LONGLONG llSizeofDiskFile = 0;

		hr = refSourceFile.DiskFile.GetSize( llSizeofDiskFile );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
#pragma remind( "REMEMBER ! Maximum size of source file is limited by 0x3FFFFFFF = 0.99GB" )
			if( 0x3FFFFFFF < llSizeofDiskFile )
			{
				_tprintf( TEXT( "too large file! program does not works with such large files.\n" ) );

				hr = E_NOTIMPL;
				ASSERT_SUCCEEDED( hr );
			}
		}

		if( SUCCEEDED( hr ) )
		{
			SIZE_T SizeofDiskFile = static_cast< SIZE_T >( llSizeofDiskFile );

			refSourceFile.SizeofTempFile = SizeofDiskFile;

			hr = refSourceFile.DiskFileMapping.Create( refSourceFile.DiskFile, NULL, PAGE_READONLY );
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				hr = refSourceFile.DiskFileView.Init( refSourceFile.DiskFileMapping, FILE_MAP_READ );
				ASSERT_SUCCEEDED( hr );

				if( SUCCEEDED( hr ) )
				{
					SIZE_T SizeofDiskFileView = 0;

					hr = refSourceFile.DiskFileView.GetSize( SizeofDiskFileView );
					ASSERT_SUCCEEDED( hr );

					if( SUCCEEDED( hr ) )
					{
						if( SizeofDiskFile > SizeofDiskFileView )
						{
							_tprintf( TEXT( "too large file! not enough virtual memory to load it.\n" ) );

							hr = E_NOTIMPL;
							ASSERT_SUCCEEDED( hr );
						}
					}

					if( SUCCEEDED( hr ) )
					{
						hr = refSourceFile.TempFileMapping.Create( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, SizeofDiskFileView );
						ASSERT_SUCCEEDED( hr );

						if( SUCCEEDED( hr ) )
						{
							hr = refSourceFile.TempFileView.Init( refSourceFile.TempFileMapping, FILE_MAP_WRITE );
							ASSERT_SUCCEEDED( hr );

							if( SUCCEEDED( hr ) )
							{
								const
									char *cp_src_begin = reinterpret_cast< const char * >( refSourceFile.DiskFileView.cpv() );

								const
									char *cp_src = cp_src_begin;

								const
									char *cp_src_end = cp_src + SizeofDiskFile;

								char *p_dst_begin = reinterpret_cast< char * >( refSourceFile.TempFileView.cpv() );

								char *p_dst = p_dst_begin;

								SIZE_T CharacterOffset = 0;

								while( cp_src != cp_src_end )
								{
									const
										char &cref = ( *cp_src );

									SIZE_T HashOffset = p_dst - p_dst_begin;

									bool bUnwantedCharacter = false;

									if(g_bRemoveDelimiters)
									{
										bUnwantedCharacter = bUnwantedCharacter || ( ' ' == cref ) || ( '\t' == cref ) || ( ';' == cref );
									}

									if(g_bRemoveNewLines)
									{
										bUnwantedCharacter = bUnwantedCharacter || ( '\r' == cref ) || ( '\n' == cref );
									}

									bool bPrintable = ( 0x20 <= cref ) && ( cref < 0x80 );

									if( bUnwantedCharacter )
									{
										TMap_HashOffset_CharacterPosition::iterator it = refSourceFile.Map_HashOffset_CharacterPosition.find( HashOffset );

										if( bPrintable )
										{
											++ CharacterOffset;
										}

										if( it != refSourceFile.Map_HashOffset_CharacterPosition.end() )
										{
											it->second = CharacterOffset;
										}
										else
										{
											refSourceFile.Map_HashOffset_CharacterPosition.insert( TMap_HashOffset_CharacterPosition::value_type( HashOffset, CharacterOffset ) );
										}
									}
									else
									{
										( *p_dst ++ ) = ( ( 0x41 <= cref ) && ( cref < 0x5B ) ) ? ( cref + 0x20 ) : ( cref );

										if( bPrintable )
										{
											++ CharacterOffset;
										}
									}

									++ cp_src;
								}

								refSourceFile.SizeofTempFile = p_dst - p_dst_begin;
							}
						}
					}
				}
			}

			if( SUCCEEDED( hr ) )
			{
				SIZE_T NumberofCRCs = refSourceFile.SizeofTempFile - SearchLength + 1;

				hr = refSourceFile.HashFileMapping.Create( INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, NumberofCRCs * sizeof( DWORD ) );
				ASSERT_SUCCEEDED( hr );

				if( SUCCEEDED( hr ) )
				{
					hr = refSourceFile.HashFileView.Init( refSourceFile.HashFileMapping, FILE_MAP_READ | FILE_MAP_WRITE );
					ASSERT_SUCCEEDED( hr );

					if( SUCCEEDED( hr ) )
					{
						DWORD *pdwCRCs = reinterpret_cast< DWORD * >( refSourceFile.HashFileView.pv() );

#pragma remind( "REMEMBER ! Following is a loop of calculation of CRC32 for a string of given SearchLength. It should be paralleled." )

						const
							BYTE *cpb = refSourceFile.TempFileView.cpb();

#pragma remind( "ATTENTION ! Hash file cannot exceed 4GB." )
						const
							BYTE *cpb_end = cpb + NumberofCRCs;

						while( cpb != cpb_end )
						{
							( *pdwCRCs ++ ) = CRC32( cpb ++, SearchLength );
						}

						refSourceFile.HashTableLength = NumberofCRCs;
					}
				}
			}
		}
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
ThreadProc_PrepareSourceFile
(
	LPVOID lpvContext
)
{
	TContext_ThreadProc_PrepareSourceFile *pContext = reinterpret_cast< TContext_ThreadProc_PrepareSourceFile * >( lpvContext );

	return PrepareSourceFile( *pContext->pSourceFile, pContext->SearchLength );
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
CompareSourceFiles
(
	TSourceFile &refSourceFile1,
	TSourceFile &refSourceFile2,
	SIZE_T CPUoffset,
	SIZE_T CPUstride,
	TVector_HashOffsetPair &refVector_HashOffsetPair
)
{
	HRESULT hr = S_OK;

	const
		DWORD *cpdwCRC1_begin = reinterpret_cast< const DWORD * >( refSourceFile1.HashFileView.cpv() );

	const
		DWORD *cpdwCRC1_end = cpdwCRC1_begin + refSourceFile1.HashTableLength;

	const
		DWORD *cpdwCRC2_begin = reinterpret_cast< const DWORD * >( refSourceFile2.HashFileView.cpv() );

	const
		DWORD *cpdwCRC2_end = cpdwCRC2_begin + refSourceFile2.HashTableLength;

	float fltProgress = 0.0f;
	float fltProgressNew = 0.0f;

	if(g_bShowProgress)
	{
		_tprintf( TEXT( "\rcomparing... % 6.2f%% completed." ), fltProgress );

		g_dProgress = fltProgress;
	}

	float fltProgressRatio = CPUstride * 100.0f / refSourceFile1.HashTableLength;

	const
		DWORD *cpdwCRC1 = cpdwCRC1_begin + CPUoffset;

	while( cpdwCRC1 < cpdwCRC1_end )
	{
		SIZE_T Offset1 = cpdwCRC1 - cpdwCRC1_begin;

		const
			DWORD *cpdwCRC2 = cpdwCRC2_begin;

		while( cpdwCRC2 < cpdwCRC2_end )
		{
			if( ( *cpdwCRC1 ) == ( *cpdwCRC2 ) )
			{
				refVector_HashOffsetPair.push_back( THashOffsetPair( Offset1, cpdwCRC2 - cpdwCRC2_begin ) );
			}

			++ cpdwCRC2;
		}

		cpdwCRC1 += CPUstride;

		if(g_bShowProgress)
		{
			fltProgressNew += fltProgressRatio;

			if( fltProgress + 0.01f <= fltProgressNew )
			{
				fltProgress = fltProgressNew;

				_tprintf( TEXT( "\rcomparing... % 6.2f%% completed." ), fltProgress );
			}

			g_dProgress = fltProgress;
		}
	}

	if(g_bShowProgress)
	{
		fltProgress = 100.0f;

		_tprintf( TEXT( "\rcomparing... % 6.2f%% completed." ), fltProgress );

		g_dProgress = fltProgress;
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

#if defined( USE_SSE2 )

HRESULT
CompareSourceFiles_SSE
(
	TSourceFile &refSourceFile1,
	TSourceFile &refSourceFile2,
	SIZE_T CPUoffset,
	SIZE_T CPUstride,
	TVector_HashOffsetPair &refVector_HashOffsetPair
)
{
	HRESULT hr = S_OK;

	const
		DWORD *cpdwCRC1_begin = reinterpret_cast< const DWORD * >( refSourceFile1.HashFileView.cpv() );

	const
		DWORD *cpdwCRC1_end = cpdwCRC1_begin + refSourceFile1.HashTableLength;

	SIZE_T CPUstride4x = ( CPUstride << 2 );

	const
		DWORD *cpdwCRC2_begin = reinterpret_cast< const DWORD * >( refSourceFile2.HashFileView.cpv() );

	const
		DWORD *cpdwCRC2_end = cpdwCRC2_begin + refSourceFile2.HashTableLength;

#if defined( SHOW_PROGRESS )
	float fltProgress = 0.0f;
	float fltProgressNew = 0.0f;

	_tprintf( TEXT( "\rcomparing... % 5.1f%% completed." ), fltProgress );

	float fltProgressRatio = CPUstride * ( sizeof( __m128i ) / sizeof( DWORD ) ) * 100.0f / refSourceFile1.HashTableLength;
#endif

	DWORD __declspec( align( 16 ) ) pdwResultsOfComparison[ 16 ];

	const
		DWORD *cpdwCRC1 = cpdwCRC1_begin + ( CPUoffset << 2 );

	while( cpdwCRC1 < cpdwCRC1_end )
	{
		__m128i xmm0 = _mm_load_si128( reinterpret_cast< __m128i * >( const_cast< DWORD * >( cpdwCRC1 ) ) );

		__m128i xmm1 = _mm_shuffle_epi32( xmm0, _MM_SHUFFLE(1,1,1,1) );
		__m128i xmm2 = _mm_shuffle_epi32( xmm0, _MM_SHUFFLE(2,2,2,2) );
		__m128i xmm3 = _mm_shuffle_epi32( xmm0, _MM_SHUFFLE(3,3,3,3) );
		xmm0 = _mm_shuffle_epi32( xmm0, _MM_SHUFFLE(0,0,0,0) );

		const
			DWORD *cpdwCRC2 = cpdwCRC2_begin;

		while( cpdwCRC2 < cpdwCRC2_end )
		{
			__m128i xmm4 = _mm_load_si128( reinterpret_cast< __m128i * >( const_cast< DWORD * >( cpdwCRC2 ) ) );

			DWORD *pdwResultOfComparison = pdwResultsOfComparison;

			_mm_store_si128( reinterpret_cast< __m128i * >( pdwResultOfComparison ), _mm_sub_epi32( xmm4, xmm0 ) );

			pdwResultOfComparison += 4;

			_mm_store_si128( reinterpret_cast< __m128i * >( pdwResultOfComparison ), _mm_sub_epi32( xmm4, xmm1 ) );

			pdwResultOfComparison += 4;

			_mm_store_si128( reinterpret_cast< __m128i * >( pdwResultOfComparison ), _mm_sub_epi32( xmm4, xmm2 ) );

			pdwResultOfComparison += 4;

			_mm_store_si128( reinterpret_cast< __m128i * >( pdwResultOfComparison ), _mm_sub_epi32( xmm4, xmm3 ) );

			pdwResultOfComparison -= 12;

			SIZE_T Offset1 = cpdwCRC1 - cpdwCRC1_begin;
			SIZE_T Offset2 = cpdwCRC2 - cpdwCRC2_begin;

			for( char i = 0; i < 16; ++ i )
			{
				if( 0 == ( *pdwResultOfComparison ++ ) )
				{
					refVector_HashOffsetPair.push_back( THashOffsetPair( Offset1 + ( i >> 2 ), Offset2 + ( 3 & i ) ) );
				}
			}

			cpdwCRC2 += 4;
		}

		cpdwCRC1 += CPUstride4x;

#if defined( SHOW_PROGRESS )
		fltProgressNew += fltProgressRatio;

		if( fltProgress + 0.01f <= fltProgressNew )
		{
			fltProgress = fltProgressNew;

			_tprintf( TEXT( "\rcomparing... % 5.1f%% completed." ), fltProgress );
		}
#endif
	}

#if defined( SHOW_PROGRESS )
	fltProgress = 100.0f;

	_tprintf( TEXT( "\rcomparing... % 5.1f%% completed." ), fltProgress );
#endif

	return hr;
}

#endif

///////////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
ThreadProc_CompareSourceFiles
(
	LPVOID lpvContext
)
{
	TContext_ThreadProc_CompareSourceFiles *pContext = reinterpret_cast< TContext_ThreadProc_CompareSourceFiles * >( lpvContext );

#if defined( USE_SSE2 )
	return ( bHaveSSE2() ? CompareSourceFiles_SSE : CompareSourceFiles )
#else
	return CompareSourceFiles
#endif
		(
			*pContext->pSourceFile1,
			*pContext->pSourceFile2,
			pContext->CPUoffset,
			pContext->CPUstride,
			pContext->Vector_HashOffsetPair
		);
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
AssignCurrentProcessOnAllCPUs()
{
	HRESULT hr = S_OK;

	Win32::CProcess processCurrent;

	DWORD dwDesiredAccess = 0;

	dwDesiredAccess |= READ_CONTROL;
	dwDesiredAccess |= PROCESS_CREATE_THREAD;
	dwDesiredAccess |= PROCESS_QUERY_INFORMATION;
	dwDesiredAccess |= PROCESS_SET_INFORMATION;

	hr = processCurrent.Open( dwDesiredAccess, FALSE, ::GetCurrentProcessId() );
	ASSERT_SUCCEEDED( hr );

	if( SUCCEEDED( hr ) )
	{
		DWORD_PTR dwptrProcessAffinityMask = 0;
		DWORD_PTR dwptrSystemAffinityMask = 0;

		hr = processCurrent.GetAffinityMask( dwptrProcessAffinityMask, dwptrSystemAffinityMask );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
			if( dwptrProcessAffinityMask != dwptrSystemAffinityMask )
			{
				hr = processCurrent.SetAffinityMask( dwptrSystemAffinityMask );
				ASSERT_SUCCEEDED( hr );
			}
		}

		HRESULT hr2 = processCurrent.Free();
		ASSERT_SUCCEEDED( hr2 );

		REPLACE_IF_FAILED( hr, hr2 );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
CreateThreadSpecially
(
	Win32::CThread &refThread,
	LPTHREAD_START_ROUTINE pfThreadProc,
	LPVOID lpvContext,
	int Priority,
	DWORD IndexofProcessor = static_cast< DWORD >( -1 )
)
{
	HRESULT hr = S_OK;

	hr = refThread.Create( NULL, 0, pfThreadProc, lpvContext, Priority, TRUE );
	ASSERT_SUCCEEDED( hr );

	if( SUCCEEDED( hr ) )
	{
		if( static_cast< DWORD >( -1 ) == IndexofProcessor )
		{
			DWORD_PTR dwptrSystemAffinityMask = 0;

			hr = Win32::CProcess::GetSystemAffinityMask( dwptrSystemAffinityMask );
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				hr = refThread.SetAffinityMask( dwptrSystemAffinityMask );
				ASSERT_SUCCEEDED( hr );
			}
		}
		else
		{
			hr = refThread.SetIdealProcessor( IndexofProcessor );
			ASSERT_SUCCEEDED( hr );
		}

		if( SUCCEEDED( hr ) )
		{
			hr = refThread.Resume();
			ASSERT_SUCCEEDED( hr );
		}
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
Proceed
(
	LPCTSTR pszFilePath_Src1,
	LPCTSTR pszFilePath_Src2,
	SIZE_T SearchLength,
//	LPCTSTR pszFilePath_Dst,
	SIZE_T MaxNumberofThreads,
	map<_tstring, unsigned int>& mapCounts,
	map<_tstring, map<_tstring, vector<_tstring>>>& mapPositions
)
{
	HRESULT hr = S_OK;

#if 0
	const char sz1[] = "isinitialized()";
	const char sz2[] = "t);messagebox.s";
	DWORD dw = 0x896860b6;

	DWORD dw1 = CRC32( ( const BYTE * )sz1, sizeof( sz1 )-1 );
	DWORD dw2 = CRC32( ( const BYTE * )sz2, sizeof( sz2 )-1 );

	ASSERT( ( dw != dw1 ) || ( dw != dw2 ) );
#endif

	hr = AssignCurrentProcessOnAllCPUs();
	ASSERT_SUCCEEDED( hr );

	if( SUCCEEDED( hr ) )
	{
		SYSTEM_INFO SystemInfo;

		::GetSystemInfo( &SystemInfo );

		SIZE_T NumberofProcessors = SystemInfo.dwNumberOfProcessors;

		if( 0 != MaxNumberofThreads )
		{
			NumberofProcessors = min( MaxNumberofThreads, NumberofProcessors );
		}

		// Attention! All priorities higher than THREAD_PRIORITY_NORMAL may hung machine, even unrecoverably!

		//int ThreadPriority = THREAD_PRIORITY_TIME_CRITICAL;
		//int ThreadPriority = THREAD_PRIORITY_ABOVE_NORMAL;
		//int ThreadPriority = THREAD_PRIORITY_BELOW_NORMAL;
		int ThreadPriority = THREAD_PRIORITY_NORMAL;

		TSourceFile SourceFile1;
		SourceFile1.pszFilePath = pszFilePath_Src1;

		TSourceFile SourceFile2;
		SourceFile2.pszFilePath = pszFilePath_Src2;

		TSet_HashOffsetPair Set_HashOffsetPair;

		if( 1 == NumberofProcessors )
		{
			hr = PrepareSourceFile( SourceFile1, SearchLength );
			ASSERT_SUCCEEDED( hr );

			if( SUCCEEDED( hr ) )
			{
				hr = PrepareSourceFile( SourceFile2, SearchLength );
				ASSERT_SUCCEEDED( hr );

				if( SUCCEEDED( hr ) )
				{
					TVector_HashOffsetPair Vector_HashOffsetPair;

#if defined( USE_SSE2 )
					hr = ( bHaveSSE2() ? CompareSourceFiles_SSE : CompareSourceFiles )
#else
					hr = CompareSourceFiles
#endif
						(
						SourceFile1,
						SourceFile2,
						0, 1,
						Vector_HashOffsetPair
						);
					ASSERT_SUCCEEDED( hr );

					if( SUCCEEDED( hr ) )
					{
						_tprintf( TEXT( "\n" ) );

						Set_HashOffsetPair.insert( Vector_HashOffsetPair.begin(), Vector_HashOffsetPair.end() );
					}
				}
			}
		}
		else
			if( 2 <= NumberofProcessors )
			{
				Win32::CThread threadPrepareSourceFile;

				TContext_ThreadProc_PrepareSourceFile Context_ThreadProc_PrepareSourceFile;

				Context_ThreadProc_PrepareSourceFile.pSourceFile = &SourceFile1;
				Context_ThreadProc_PrepareSourceFile.SearchLength = SearchLength;

				hr = CreateThreadSpecially( threadPrepareSourceFile, ThreadProc_PrepareSourceFile, &Context_ThreadProc_PrepareSourceFile, ThreadPriority );
				ASSERT_SUCCEEDED( hr );

				if( SUCCEEDED( hr ) )
				{
					hr = threadPrepareSourceFile.Wait();
					ASSERT_SUCCEEDED( hr );

					if( SUCCEEDED( hr ) )
					{
						threadPrepareSourceFile.Free();

						Context_ThreadProc_PrepareSourceFile.pSourceFile = &SourceFile2;
						Context_ThreadProc_PrepareSourceFile.SearchLength = SearchLength;

						hr = CreateThreadSpecially( threadPrepareSourceFile, ThreadProc_PrepareSourceFile, &Context_ThreadProc_PrepareSourceFile, ThreadPriority );
						ASSERT_SUCCEEDED( hr );

						if( SUCCEEDED( hr ) )
						{
							hr = threadPrepareSourceFile.Wait();
							ASSERT_SUCCEEDED( hr );
						}
					}
				}

				if( SUCCEEDED( hr ) )
				{
					struct
						TSimultaneousThread
					{
						TContext_ThreadProc_CompareSourceFiles Context;
						Win32::CThread thread;
					}
					SimultaneousThreads[ MAX_NUMBER_OF_CPU_UTILIZED_SIMULTANEOUSLY ];

					if( NumberofProcessors > MAX_NUMBER_OF_CPU_UTILIZED_SIMULTANEOUSLY )
					{
						NumberofProcessors = MAX_NUMBER_OF_CPU_UTILIZED_SIMULTANEOUSLY;
					}

					HANDLE pHandles[ MAX_NUMBER_OF_CPU_UTILIZED_SIMULTANEOUSLY ] = { NULL };

					for( unsigned int IndexofProcessor = 0; IndexofProcessor < NumberofProcessors; ++ IndexofProcessor )
					{
						TSimultaneousThread &refSimultaneousThread = SimultaneousThreads[ IndexofProcessor ];

						refSimultaneousThread.Context.pSourceFile1 = &SourceFile1;
						refSimultaneousThread.Context.pSourceFile2 = &SourceFile2;
						refSimultaneousThread.Context.SearchLength = SearchLength;
						refSimultaneousThread.Context.CPUoffset = IndexofProcessor;
						refSimultaneousThread.Context.CPUstride = NumberofProcessors;

						hr = CreateThreadSpecially( refSimultaneousThread.thread, ThreadProc_CompareSourceFiles, &refSimultaneousThread.Context, ThreadPriority, IndexofProcessor );
						ASSERT_SUCCEEDED( hr );

						if( SUCCEEDED( hr ) )
						{
							pHandles[ IndexofProcessor ] = refSimultaneousThread.thread;
						}

						if( FAILED( hr ) )
						{
							break;
						}
					}

					if( SUCCEEDED( hr ) )
					{
						hr = Win32::CSynchronizationObject::WaitForMultipleObjects( static_cast< DWORD >( NumberofProcessors ), pHandles, NULL, TRUE, INFINITE );
						ASSERT_SUCCEEDED( hr );

						if( SUCCEEDED( hr ) )
						{
							_tprintf( TEXT( "\n" ) );

							for( unsigned int IndexofProcessor = 0; IndexofProcessor < NumberofProcessors; ++ IndexofProcessor )
							{
								const
									TSimultaneousThread &crefSimultaneousThread = SimultaneousThreads[ IndexofProcessor ];

								Set_HashOffsetPair.insert( crefSimultaneousThread.Context.Vector_HashOffsetPair.begin(), crefSimultaneousThread.Context.Vector_HashOffsetPair.end() );
							}
						}
					}
				}
			}

			if( SUCCEEDED( hr ) )
			{
				_tprintf( TEXT( "filter out wrong matches..." ) );

				TSet_HashOffsetPair::const_iterator cit = Set_HashOffsetPair.begin();

				while( cit != Set_HashOffsetPair.end() )
				{
					const
						THashOffsetPair &crefHashOffsetPair = ( *cit );

					if( ( SourceFile1.SizeofTempFile < crefHashOffsetPair.first + SearchLength ) || ( SourceFile2.SizeofTempFile < crefHashOffsetPair.second + SearchLength ) )
					{
#pragma remind( "ignore accidentally positive result of comparison of last one to three DWORDs which are outside file!" )
						cit = Set_HashOffsetPair.erase( cit );

						continue;
					}
					else
					{
						const
							BYTE *cpb1 = SourceFile1.TempFileView.cpb() + crefHashOffsetPair.first;

						const
							BYTE *cpb2 = SourceFile2.TempFileView.cpb() + crefHashOffsetPair.second;

						if( 0 != memcmp( cpb1, cpb2, SearchLength ) )
						{
#pragma remind( "two absolutely different strings produces identical CRC32 values !" )
							cit = Set_HashOffsetPair.erase( cit );

							continue;
						}
					}

					++ cit;
				}
			}

			// Shup - Buggy code that removes everything!
// 			if( SUCCEEDED( hr ) )
// 			{
// 				_tprintf( TEXT( "\nfilter out similar strings longer than %d..." ), SearchLength );
// 
// 				for( TSet_HashOffsetPair::const_iterator cit = Set_HashOffsetPair.begin(); cit != Set_HashOffsetPair.end(); ++ cit )
// 				{
// 					const
// 						THashOffsetPair &crefHashOffsetPair = ( *cit );
// 
// 					TSet_HashOffsetPair::const_iterator cit2 = cit;
// 
// 					++ cit2;
// 
// 					SIZE_T N = 1;
// 
// 					SIZE_T Nmax = SearchLength;
// 
// 					while( cit2 != Set_HashOffsetPair.end() )
// 					{
// 						const
// 							THashOffsetPair &crefHashOffsetPair2 = ( *cit2 );
// 
// 						if( crefHashOffsetPair.first == crefHashOffsetPair2.first )
// 						{
// 							if( SearchLength >= crefHashOffsetPair2.second - crefHashOffsetPair.second )
// 							{
// 								cit2 = Set_HashOffsetPair.erase( cit2 );
// 
// 								++ N;
// 								++ Nmax;
// 
// 								continue;
// 							}
// 						}
// 						else
// 							if( crefHashOffsetPair.first + N == crefHashOffsetPair2.first )
// 							{
// 								if( crefHashOffsetPair.second + N == crefHashOffsetPair2.second )
// 								{
// 									cit2 = Set_HashOffsetPair.erase( cit2 );
// 
// 									++ N;
// 									++ Nmax;
// 
// 									continue;
// 								}
// 							}
// 							else
// 								if( crefHashOffsetPair.first + Nmax < crefHashOffsetPair2.first )
// 								{
// 									break;
// 								}
// 
// 								++ cit2;
// 					}
// 				}
// 			}

			if( SUCCEEDED( hr ) )
			{
				_tprintf( TEXT( "\nwriting matches..." ) );

//				FILE *pf = _tfopen( pszFilePath_Dst, TEXT( "w" ) );

				long lHashOffsetPairIndex = 0;

				for( TSet_HashOffsetPair::const_iterator cit = Set_HashOffsetPair.begin(); cit != Set_HashOffsetPair.end(); ++ cit )
				{
					lHashOffsetPairIndex++;

					if((((lHashOffsetPairIndex - 1) % SearchLength) != 0) && (lHashOffsetPairIndex < (long) Set_HashOffsetPair.size()))
					{
						continue;
					}

					const
						THashOffsetPair &crefHashOffsetPair = ( *cit );

					TCharacterPosition CharacterPosition1 = CharacterPositionFromHashOffset( SourceFile1.Map_HashOffset_CharacterPosition, crefHashOffsetPair.first );
					TCharacterPosition CharacterPosition2 = CharacterPositionFromHashOffset( SourceFile2.Map_HashOffset_CharacterPosition, crefHashOffsetPair.second );

// 					fprintf
// 					(
// 						pf,
// 						"%d(%d)\t%*.*s\t%d(%d)\t%*.*s\n",
// 						CharacterPosition1.first, CharacterPosition1.second,
// 						SearchLength,SearchLength, SourceFile1.TempFileView.cpb() + crefHashOffsetPair.first,
// 						CharacterPosition2.first, CharacterPosition2.second,
// 						SearchLength,SearchLength, SourceFile2.TempFileView.cpb() + crefHashOffsetPair.second
// 					);

					char* pstrKey = (char*) malloc(sizeof(char) * SearchLength + 1);
					sprintf_s(pstrKey, SearchLength + 1, "%*.*s", SearchLength, SearchLength, SourceFile1.TempFileView.cpb() + crefHashOffsetPair.first);

					char* pstrPostion1 = (char*) malloc(sizeof(char) * 32);
					sprintf_s(pstrPostion1, 32, "%.4d(%.4d)", CharacterPosition1.first, CharacterPosition1.second);

					char* pstrPostion2 = (char*) malloc(sizeof(char) * 32);
					sprintf_s(pstrPostion2, 32, "%.4d(%.4d)", CharacterPosition2.first, CharacterPosition2.second);

					_tstring strKey = _T("");
					_tstring strPosition1 = _T("");
					_tstring strPosition2 = _T("");

#ifdef UNICODE
					strKey = CUtil::GetStringW(pstrKey);
					strPosition1 = CUtil::GetStringW(pstrPostion1);
					strPosition2 = CUtil::GetStringW(pstrPostion2);
#else
					strKey = pstrKey;
					strPosition1 = pstrPostion1;
					strPosition2 = pstrPostion2;
#endif
					free(pstrKey);
					free(pstrPostion1);
					free(pstrPostion2);

					mapCounts[strKey]++;
					mapPositions[strKey][strPosition1].push_back(strPosition2);
				}

//				fclose( pf );
			}

			_tprintf( TEXT( "\ndone.\n" ) );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
unsafeFileCanBeRead
(
	LPCTSTR pszFilePath
)
{
	HRESULT hr = S_OK;

	if( IS_NULL( pszFilePath ) )
	{
		hr = E_POINTER;
		ASSERT_SUCCEEDED( hr );
	}
	else
	{
		Win32::CDiskFile DiskFile;

		hr = DiskFile.Open( pszFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL );
		ASSERT_SUCCEEDED( hr );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
safeFileCanBeRead
(
	LPCTSTR pszFilePath
)
{
	__try
	{
		return unsafeFileCanBeRead( pszFilePath );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return E_ABORT;
	}
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
unsafeFileCanBeWritten
(
	LPCTSTR pszFilePath
)
{
	HRESULT hr = S_OK;

	if( IS_NULL( pszFilePath ) )
	{
		hr = E_POINTER;
		ASSERT_SUCCEEDED( hr );
	}
	else
	{
		Win32::CDiskFile DiskFile;

		hr = DiskFile.Open( pszFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL );
		ASSERT_SUCCEEDED( hr );
	}

	return hr;
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT
safeFileCanBeWritten
(
	LPCTSTR pszFilePath
)
{
	__try
	{
		return unsafeFileCanBeWritten( pszFilePath );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return E_ABORT;
	}
}

///////////////////////////////////////////////////////////////////////////////////

HRESULT safeStringToInteger
(
	LPCTSTR psz,
	int &refNumber
)
{
	__try
	{
		refNumber = _ttoi( psz );

		return S_OK;
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return E_ABORT;
	}
}

///////////////////////////////////////////////////////////////////////////////////
// CFileComapre thread
///////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI StartFileCompareThread(LPVOID param)
{
	CFileCompare* pFC = (CFileCompare*) param;
	pFC->CompareFiles();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// CFileCompare class definitions
///////////////////////////////////////////////////////////////////////////////////
CFileCompare::CFileCompare(void)
{
	Initialize();
}

CFileCompare::~CFileCompare(void)
{
	Initialize();
}

void CFileCompare::Initialize()
{
	m_strFilePath1 = _T("");
	m_strFilePath2 = _T("");

	m_lSearchLength = 0;

	m_strOutputFilePath = _T("");

	m_lThreadCount = 0;

	m_bIsRunning = false;

	m_pdProgress = &g_dProgress;
	m_dTime = 0.0;

	m_dSimilarityF1 = 0.0;
	m_dSimilarityF2 = 0.0;

	m_mapAllCounts.clear();
	m_mapAllPositions.clear();

	m_mapExcludeList.clear();
	m_mapKeywordList.clear();

	m_mapResultantCounts.clear();
	m_mapResultantPositions.clear();

	m_strAllOutput;
	m_strResultantOutput;
}

void CFileCompare::Start()
{
	if(IsRunning())
	{
		return;
	}

	m_bIsRunning = true;

	DWORD dwThreadId;
	CreateThread(NULL, 0, StartFileCompareThread, (void*) this, 0, &dwThreadId);
}

void CFileCompare::CompareFiles()
{
	if(!IsReady())
	{
		return;
	}

	if(CUtil::CompareNoCase(m_strFilePath1, m_strFilePath2) == 0)
	{
		return;
	}

	DWORD dwStart = ::GetTickCount();

	_tstring strFilePath1 = _T("");
	_tstring strFilePath2 = _T("");

	if(m_mapKeywordList.empty())
	{
		strFilePath1 = m_strFilePath1;
		strFilePath2 = m_strFilePath2;
	}
	else
	{
		_tstring strFile1Data = _T("");
		_tstring strFile2Data = _T("");

		strFilePath1 = m_strFilePath1;
		strFilePath1 += TEMP_FILE_EXT;

		strFilePath2 = m_strFilePath2;
		strFilePath2 += TEMP_FILE_EXT;

		bool bF1 = false;
		bool bF2 = false;

		if(!CUtil::IsFile(strFilePath1))
		{
			CUtil::ReadFile(m_strFilePath1, strFile1Data);

			bF1 = true;
		}

		if(!CUtil::IsFile(strFilePath2))
		{
			CUtil::ReadFile(m_strFilePath2, strFile2Data);

			bF2 = true;
		}

		for(map<_tstring, unsigned int>::iterator iMapIndex = m_mapKeywordList.begin(); iMapIndex != m_mapKeywordList.end(); ++iMapIndex)
		{
			_tstring strKeyword = (*iMapIndex).first;

			if(bF1)
			{
				CUtil::FindReplace(strFile1Data, strKeyword, _T(""));
			}

			if(bF2)
			{
				CUtil::FindReplace(strFile2Data, strKeyword, _T(""));
			}
		}

		if(bF1)
		{
			CUtil::WriteFile(strFilePath1, strFile1Data);
		}

		if(bF2)
		{
			CUtil::WriteFile(strFilePath2, strFile2Data);
		}
	}

	CompareFiles(strFilePath1, strFilePath2, m_lSearchLength, m_strOutputFilePath, m_lThreadCount);

	CreateResultantCounts();
	CreateResultantPositions();

	CalculateSimilarityF1();
	CalculateSimilarityF2();

	CreateAllOutput();
	CreateResultantOutput();

	DWORD dwEnd = ::GetTickCount();

	m_dTime = (double) (dwEnd - dwStart) / 1000.0;

	m_bIsRunning = false;
}

bool CFileCompare::IsReady()
{
	if(!CUtil::IsFile(m_strFilePath1))
	{
		return false;
	}

	if(!CUtil::IsFile(m_strFilePath2))
	{
		return false;
	}

	if(m_lSearchLength <= 0)
	{
		return false;
	}

	if(m_strOutputFilePath.empty())
	{
		return false;
	}

	if(m_lThreadCount < 0)
	{
		return false;
	}

	return true;
}

void CFileCompare::RemoveDelimiters(bool bEnable /* = true*/)
{
	g_bRemoveDelimiters = bEnable;
}

void CFileCompare::RemoveNewLines(bool bEnable /* = true*/)
{
	g_bRemoveNewLines = bEnable;
}

void CFileCompare::ShowProgress(bool bEnable /* = true*/)
{
	g_bShowProgress = bEnable;
}

bool CFileCompare::LoadList(map<_tstring, unsigned int>& mapList, const _tstring& strListFilePath)
{
	_tstring strList = _T("");

	if(CUtil::CompareNoCase(strListFilePath, _T("")) == 0)
	{
		return false;
	}

	if(!CUtil::ReadFile(strListFilePath, strList))
	{
		return false;
	}

	CUtil::FindReplace(strList, _T("\r\n"), _T("\n"));
	CUtil::FindReplace(strList, _T("\n\n"), _T("\n"));

	mapList.clear();

	long lIndex = 0;
	long lCount = CUtil::GetTokenCount(strList, _T("\n"));

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		_tstring strToken = CUtil::GetToken(strList, _T("\n"), lIndex);
		CUtil::Trim(strToken);
		CUtil::RemoveWhiteSpaces(strToken);

		if(!strToken.empty())
		{
			mapList[strToken]++;
		}
	}

	return (!mapList.empty());
}

bool CFileCompare::LoadExcludeList(const _tstring& strExcludeListFilePath)
{
	return LoadList(m_mapExcludeList, strExcludeListFilePath);
}

bool CFileCompare::LoadKeywordList(const _tstring& strKeywordListFilePath)
{
	return LoadList(m_mapKeywordList, strKeywordListFilePath);
}

void CFileCompare::CreateResultantCounts()
{
	m_mapResultantCounts.clear();

	for(map<_tstring, unsigned int>::iterator iMapIndex = m_mapAllCounts.begin(); iMapIndex != m_mapAllCounts.end(); ++iMapIndex)
	{
		_tstring strKey = (*iMapIndex).first;

		if(m_mapExcludeList.find(strKey) == m_mapExcludeList.end())
		{
			m_mapResultantCounts[strKey] = (*iMapIndex).second;
		}
	}
}

void CFileCompare::CreateResultantPositions()
{
	m_mapResultantPositions.clear();

	for(map<_tstring, map<_tstring, vector<_tstring>>>::iterator iMapIndex = m_mapAllPositions.begin(); iMapIndex != m_mapAllPositions.end(); ++iMapIndex)
	{
		_tstring strKey = (*iMapIndex).first;

		if(m_mapExcludeList.find(strKey) == m_mapExcludeList.end())
		{
			m_mapResultantPositions[strKey] = (*iMapIndex).second;
		}
	}
}

long CFileCompare::GetCount(map<_tstring, unsigned int>& mapCounts)
{
	long lCount = 0;

	for(map<_tstring, unsigned int>::iterator iMapIndex = mapCounts.begin(); iMapIndex != mapCounts.end(); ++iMapIndex)
	{
		lCount += (*iMapIndex).second;
	}

	return lCount;
}

void CFileCompare::CalculateSimilarityF1()
{
	double dTotalPhraseCount = 0.0;

	if(m_lSearchLength != 0)
	{
		dTotalPhraseCount = (double) CUtil::GetFileSize(m_strFilePath1) / (double) m_lSearchLength;
	}

	if(dTotalPhraseCount == 0.0)
	{
		return;
	}

	double dTotalMatchingPhraseCount = (double) m_mapResultantCounts.size()/* / (double) m_lSearchLength*/;

	m_dSimilarityF1 = dTotalMatchingPhraseCount / dTotalPhraseCount;

	if(m_dSimilarityF1 >= 1.0)
	{
		m_dSimilarityF1 = 1.0;
	}
}

void CFileCompare::CalculateSimilarityF2()
{
	double dTotalPhraseCount = 0.0;

	if(m_lSearchLength != 0)
	{
		dTotalPhraseCount = (double) CUtil::GetFileSize(m_strFilePath2) / (double) m_lSearchLength;
	}

	if(dTotalPhraseCount == 0.0)
	{
		return;
	}

	double dTotalMatchingPhraseCount = (double) GetCount(m_mapResultantCounts)/* / (double) m_lSearchLength*/;

	m_dSimilarityF2 = dTotalMatchingPhraseCount / dTotalPhraseCount;

	if(m_dSimilarityF2 >= 1.0)
	{
		m_dSimilarityF2 = 1.0;
	}
}

_tstring CFileCompare::CreateHeaderFileComparision()
{
	/*
	------------------------------------------------------------------------------------------------
	                                          number of
	                  total                   matches
	                  matches     source      found for   destination
	string            found       line        that line   lines
	------------------------------------------------------------------------------------------------
	"xxxxxxxxxxxx"    [000002]    1812(63)    [000001] -> 97(77)
	                              1813(59)    [000001] -> 97(77)
	*/

	long lHeaderStringLength = m_lSearchLength + (long) _tstring(TAB).length() + 2;

	_tstring strHeader = _T("");

	strHeader += HEADER_BREAK;
	strHeader += NEW_LINE;

	strHeader += CUtil::CreatePadding(lHeaderStringLength, SPACE);
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE, SPACE);
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE, SPACE);
	strHeader += _T("number of");
	strHeader += NEW_LINE;

	strHeader += CUtil::CreatePadding(lHeaderStringLength, SPACE);
	strHeader += _T("total");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("total")).length(), SPACE);
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE, SPACE);
	strHeader += _T("matches");
	strHeader += NEW_LINE;

	strHeader += CUtil::CreatePadding(lHeaderStringLength, SPACE);
	strHeader += _T("matches");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("matches")).length(), SPACE);
	strHeader += _T("source");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("source")).length(), SPACE);
	strHeader += _T("found for");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("found for")).length(), SPACE);
	strHeader += _T("destination");
	strHeader += NEW_LINE;

	strHeader += _T("string");
	strHeader += CUtil::CreatePadding(lHeaderStringLength - (unsigned long) _tstring(_T("string")).length(), SPACE);
	strHeader += _T("found");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("found")).length(), SPACE);
	strHeader += _T("line");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("line")).length(), SPACE);
	strHeader += _T("that line");
	strHeader += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) _tstring(_T("that line")).length(), SPACE);
	strHeader += _T("lines");
	strHeader += NEW_LINE;

	strHeader += HEADER_BREAK;
	strHeader += NEW_LINE;

	return strHeader;
}

_tstring CFileCompare::CreateHeaderGroupSummary()
{
	/*
	------------------------------------------------------------------------------------------------
	                  total matches
	                  across all
	string            destination files
	------------------------------------------------------------------------------------------------
	"xxxxxxxxxxxx"    1
	"yyyyyyyyyyyy"    8
	*/

	long lHeaderStringLength = m_lSearchLength + (unsigned long) _tstring(TAB).length() + 2;

	_tstring strHeader = _T("");

	strHeader += HEADER_BREAK;
	strHeader += NEW_LINE;

	strHeader += CUtil::CreatePadding(lHeaderStringLength, SPACE);
	strHeader += _T("total matches");
	strHeader += NEW_LINE;

	strHeader += CUtil::CreatePadding(lHeaderStringLength, SPACE);
	strHeader += _T("across all");
	strHeader += NEW_LINE;

	strHeader += _T("string");
	strHeader += CUtil::CreatePadding(lHeaderStringLength - (unsigned long) _tstring(_T("string")).length(), SPACE);
	strHeader += _T("destination files");
	strHeader += NEW_LINE;

	strHeader += HEADER_BREAK;
	strHeader += NEW_LINE;

	return strHeader;
}

_tstring CFileCompare::GetOutput(map<_tstring, unsigned int>& mapCounts, map<_tstring, map<_tstring, vector<_tstring>>>& mapPositions)
{
	vector<CFoundInData> vecFIB;

	for(map<_tstring, unsigned int>::iterator iMapCIndex = mapCounts.begin(); iMapCIndex != mapCounts.end(); ++iMapCIndex)
	{
		CFoundInData FIB;
		_tstring strFIBEntry = _T("");

		_tstring strKey = (*iMapCIndex).first;

		_tstring strKeyData = _T("");

		strKeyData += _T("\"");
		strKeyData += strKey;
		strKeyData += _T("\"");
		strKeyData += TAB;

		strKeyData += _T("[");
		strKeyData += CUtil::Long2String((*iMapCIndex).second, PRECISION);
		strKeyData += _T("]");
		strKeyData += TAB;

		strFIBEntry += strKeyData;

		_tstring strPadding = CUtil::CreatePadding((unsigned long) strKeyData.length(), SPACE);

		for(map<_tstring, vector<_tstring>>::iterator iMapPIndex = mapPositions[strKey].begin(); iMapPIndex != mapPositions[strKey].end(); ++iMapPIndex)
		{
			_tstring strPositions = _T("");

			_tstring strFile1Position = (*iMapPIndex).first;

			if(FIB.m_strKey.empty())
			{
				FIB.m_strKey = strFile1Position;
			}

			strPositions += strFile1Position;
			strPositions += CUtil::CreatePadding(HEADER_LABEL_DISTANCE - (unsigned long) strFile1Position.length(), SPACE);

			_tstring strFile2Positions = _T("");

			sort((*iMapPIndex).second.begin(), (*iMapPIndex).second.end());

			long lIndex = 0;
			long lCount = (long) (*iMapPIndex).second.size();

			strPositions += _T("[");
			strPositions += CUtil::Long2String(lCount, PRECISION);
			strPositions += _T("]");
			strPositions += _T(" -> ");

			for(lIndex = 0; lIndex < lCount; lIndex++)
			{
				strFile2Positions += (*iMapPIndex).second.at(lIndex);
				strFile2Positions += _T(", ");
			}

			strFile2Positions.erase(strFile2Positions.length() - 2);

			strPositions += strFile2Positions;
			strPositions += NEW_LINE;
			
			strPositions += strPadding;

			strFIBEntry += strPositions;
		}

		strFIBEntry.erase(strFIBEntry.length() - strPadding.length());
		strFIBEntry += NEW_LINE;

		FIB.m_strValue = strFIBEntry;
		vecFIB.push_back(FIB);
	}

	sort(vecFIB.begin(), vecFIB.end());

	_tstring strOutput = CreateHeaderFileComparision();

	long lFIBIndex = 0;
	long lFIBCount = (long) vecFIB.size();

	for(lFIBIndex = 0; lFIBIndex < lFIBCount; lFIBIndex++)
	{
		strOutput += vecFIB[lFIBIndex].m_strValue;
	}

	return strOutput;
}

void CFileCompare::CreateAllOutput()
{
	m_strAllOutput = GetOutput(m_mapAllCounts, m_mapAllPositions);
}

void CFileCompare::CreateResultantOutput()
{
	m_strResultantOutput = GetOutput(m_mapResultantCounts, m_mapResultantPositions);
}

HRESULT CFileCompare::CompareFiles(const _tstring& strInputFilePath1, const _tstring& strInputFilePath2, long lSearchLength, _tstring strOutputFilePath, long lThreadCount)
{
	HRESULT hr = S_OK;

	LPCTSTR pszFilePath_Src1 = NULL;
	LPCTSTR pszFilePath_Src2 = NULL;
	int SearchLength = 0;
	LPCTSTR pszFilePath_Dst = NULL;
	int MaxNumberofThreads = 0;

	{
		pszFilePath_Src1 = strInputFilePath1.c_str();

		hr = safeFileCanBeRead( pszFilePath_Src1 );
		ASSERT_SUCCEEDED( hr );

		if( FAILED( hr ) )
		{
			_tprintf( TEXT( "An error occured while accessing \"%s\".\n\n" ), pszFilePath_Src1 );

			helpstring();
		}
		else
		{
			pszFilePath_Src2 = strInputFilePath2.c_str();

			hr = safeFileCanBeRead( pszFilePath_Src2 );
			ASSERT_SUCCEEDED( hr );

			if( FAILED( hr ) )
			{
				_tprintf( TEXT( "An error occured while accessing \"%s\".\n\n" ), pszFilePath_Src2 );

				helpstring();
			}
			else
			{
				SearchLength = lSearchLength;

				if( 2 > SearchLength )
				{
					_tprintf( TEXT( "Invalid search length specified.\n\n" ) );

					helpstring();

					hr = E_INVALIDARG;
					ASSERT_SUCCEEDED( hr );
				}

				if( SUCCEEDED( hr ) )
				{
					pszFilePath_Dst = strOutputFilePath.c_str();

					hr = safeFileCanBeWritten( pszFilePath_Dst );
					ASSERT_SUCCEEDED( hr );

					if( FAILED( hr ) )
					{
						_tprintf( TEXT( "An error occured while accessing \"%s\".\n\n" ), pszFilePath_Dst );

						helpstring();
					}
					else
					{
						MaxNumberofThreads = lThreadCount;

						if( 0 > MaxNumberofThreads )
						{
							_tprintf( TEXT( "Negative or zero number of threads specified!\n" ) );

							hr = E_INVALIDARG;
							ASSERT_SUCCEEDED( hr );
						}
					}
				}
			}
		}
	}

	if( SUCCEEDED( hr ) )
	{
		DWORD dw1 = ::GetTickCount();

		hr = Proceed( pszFilePath_Src1, pszFilePath_Src2, SearchLength, /* pszFilePath_Dst,*/ MaxNumberofThreads, m_mapAllCounts, m_mapAllPositions );
		ASSERT_SUCCEEDED( hr );

		if( SUCCEEDED( hr ) )
		{
			DWORD msec = ::GetTickCount() - dw1;

			_tprintf( TEXT( "Calculation time = %d.%03d sec.\n" ), msec / 1000, msec % 1000 );
		}
	}

	if( ::IsDebuggerPresent() )
	{
		_tprintf( TEXT( "Press any key to exit... " ) );

		while( !_kbhit() );

		while( _kbhit() )
		{
			_getch();
		}
	}

	return hr;
}

_tstring CFileCompare::GetOutputHeader()
{
	_tstring strOutputHeader = _T("");

	strOutputHeader += _T("Comparing: ");
	strOutputHeader += CUtil::GetFileName(m_strFilePath1, true);
	strOutputHeader += _T(" <=> ");
	strOutputHeader += CUtil::GetFileName(m_strFilePath2, true);
	strOutputHeader += NEW_LINE;

	strOutputHeader += _T("Search Length: ");
	strOutputHeader += CUtil::Long2String(m_lSearchLength);
	strOutputHeader += NEW_LINE;

	strOutputHeader += _T("Time: ");
	strOutputHeader += CUtil::Double2String(GetTime());
	strOutputHeader += _T(" seconds");
	strOutputHeader += NEW_LINE;
	strOutputHeader += NEW_LINE;

	strOutputHeader += _T("Similarity ");
	strOutputHeader += NEW_LINE;
	strOutputHeader += _T("Original: ");
	strOutputHeader += CUtil::Double2String(GetSimilarityF1() * 100.0);
	strOutputHeader += _T(" %");
	strOutputHeader += NEW_LINE;
	strOutputHeader += _T("Suspect: ");
	strOutputHeader += CUtil::Double2String(GetSimilarityF2() * 100.0);
	strOutputHeader += _T(" %");
	strOutputHeader += NEW_LINE;
	strOutputHeader += NEW_LINE;

	return strOutputHeader;
}

void CFileCompare::WriteOutputAll()
{
	if(IsRunning())
	{
		return;
	}

	if(!IsReady())
	{
		return;
	}

	_tstring strOutputFileData = GetOutputHeader();

	strOutputFileData += GetOutputAll();

	strOutputFileData += NEW_LINE;
	strOutputFileData += PAGE_BREAK;
	strOutputFileData += NEW_LINE;

	CUtil::WriteFile(m_strOutputFilePath, strOutputFileData);
}

void CFileCompare::WriteOutputResultant()
{
	if(IsRunning())
	{
		return;
	}

	if(!IsReady())
	{
		return;
	}

	_tstring strOutputFileData = GetOutputHeader();

	strOutputFileData += GetOutputResultant();

	strOutputFileData += NEW_LINE;
	strOutputFileData += PAGE_BREAK;
	strOutputFileData += NEW_LINE;

	CUtil::WriteFile(m_strOutputFilePath, strOutputFileData);
}

void CFileCompare::AddFileOccurrences(map<_tstring, unsigned int>& mapFileOccurrences)
{
	for(map<_tstring, unsigned int>::iterator iMapIndex = m_mapResultantCounts.begin(); iMapIndex != m_mapResultantCounts.end(); ++iMapIndex)
	{
		mapFileOccurrences[(*iMapIndex).first] += (*iMapIndex).second;
	}
}

_tstring CFileCompare::GetSummary()
{
	if(IsRunning())
	{
		return _T("");
	}

	if(!IsReady())
	{
		return _T("");
	}

	_tstring strSummary = _T("");

	strSummary += _T("Comparing: ");
	strSummary += CUtil::GetFileName(m_strFilePath1, true);
	strSummary += _T(" <=> ");
	strSummary += CUtil::GetFileName(m_strFilePath2, true);
	strSummary += NEW_LINE;

	strSummary += _T("Search Length: ");
	strSummary += CUtil::Long2String(m_lSearchLength);
	strSummary += NEW_LINE;

	strSummary += _T("Time: ");
	strSummary += CUtil::Double2String(GetTime());
	strSummary += _T(" seconds");
	strSummary += NEW_LINE;
	strSummary += NEW_LINE;

	if(GetSimilarityF1() >= 0.80 || GetSimilarityF2() >= 0.80)
	{
		strSummary += _T("Similarity - Match close to 100%, please examine manually!");
	}
	else
	{
		strSummary += _T("Similarity");
	}

	strSummary += NEW_LINE;
	strSummary += _T("Original: ");
	strSummary += CUtil::Double2String(GetSimilarityF1() * 100.0);
	strSummary += _T(" %");
	strSummary += NEW_LINE;
	strSummary += _T("Suspect: ");
	strSummary += CUtil::Double2String(GetSimilarityF2() * 100.0);
	strSummary += _T(" %");
	strSummary += NEW_LINE;
	strSummary += NEW_LINE;

	strSummary += PAGE_BREAK;
	strSummary += NEW_LINE;
	strSummary += NEW_LINE;

	return strSummary;
}
