#pragma once

#include "Util.h"

#include <map>

using std::map;

///////////////////////////////////////////////////////////////////////////////////

#define TAB				_T("    ")
#define SPACE			_T(" ")
#define NEW_LINE		_T("\r\n")
#define HEADER_BREAK	_T("------------------------------------------------------------------------------------------------")
#define PAGE_BREAK		_T("################################################################################################")

#define HEADER_LABEL_DISTANCE		12

#define PRECISION					6

#define TEMP_FILE_EXT		_T(".sim_tmp")

///////////////////////////////////////////////////////////////////////////////////

inline
bool
operator <
(
	const std::pair< SIZE_T, SIZE_T > &crefL,
	const std::pair< SIZE_T, SIZE_T > &crefR
)
{
	return ( ( crefL.first < crefR.first ) || ( ( crefL.first == crefR.first ) && ( crefL.second < crefR.second ) ) );
}

///////////////////////////////////////////////////////////////////////////////////

typedef
std::pair< SIZE_T, SIZE_T >
TCharacterPosition;

///////////////////////////////////////////////////////////////////////////////////

typedef
std::map< SIZE_T, SIZE_T >
TMap_HashOffset_CharacterPosition;

///////////////////////////////////////////////////////////////////////////////////

struct TSourceFile
{
	LPCTSTR pszFilePath;

	Win32::CDiskFile DiskFile;

	Win32::CFileMapping DiskFileMapping;

	Win32::CFileView DiskFileView;

	Win32::CFileMapping TempFileMapping;

	Win32::CFileView TempFileView;

	SIZE_T SizeofTempFile;

	Win32::CFileMapping HashFileMapping;

	Win32::CFileView HashFileView;

	SIZE_T HashTableLength;

	TMap_HashOffset_CharacterPosition Map_HashOffset_CharacterPosition;
};

///////////////////////////////////////////////////////////////////////////////////

struct TContext_ThreadProc_PrepareSourceFile
{
	TSourceFile *pSourceFile;
	SIZE_T SearchLength;
};

///////////////////////////////////////////////////////////////////////////////////

typedef
std::pair
<
SIZE_T,
SIZE_T
>
THashOffsetPair;

///////////////////////////////////////////////////////////////////////////////////

typedef
std::vector
<
THashOffsetPair
>
TVector_HashOffsetPair;

///////////////////////////////////////////////////////////////////////////////////

typedef
std::set
<
THashOffsetPair
>
TSet_HashOffsetPair;

///////////////////////////////////////////////////////////////////////////////////

struct TContext_ThreadProc_CompareSourceFiles
{
	TSourceFile *pSourceFile1;
	TSourceFile *pSourceFile2;
	SIZE_T SearchLength;
	SIZE_T CPUoffset;
	SIZE_T CPUstride;
	TVector_HashOffsetPair Vector_HashOffsetPair;
};

///////////////////////////////////////////////////////////////////////////////////

class CFileCompare
{
private:
	_tstring m_strFilePath1;
	_tstring m_strFilePath2;

	long m_lSearchLength;

	_tstring m_strOutputFilePath;

	long m_lThreadCount;

private:
	bool m_bIsRunning;

public:
	bool IsRunning() { return m_bIsRunning; };

private:
	double* m_pdProgress;
	double m_dTime;

	double m_dSimilarityF1;
	double m_dSimilarityF2;

private:
	map<_tstring, unsigned int> m_mapAllCounts;
	map<_tstring, map<_tstring, vector<_tstring>>> m_mapAllPositions;

	map<_tstring, unsigned int> m_mapExcludeList;
	map<_tstring, unsigned int> m_mapKeywordList;

	map<_tstring, unsigned int> m_mapResultantCounts;
	map<_tstring, map<_tstring, vector<_tstring>>> m_mapResultantPositions;

private:
	_tstring m_strAllOutput;
	_tstring m_strResultantOutput;

public:
	CFileCompare(void);
	~CFileCompare(void);

public:
	void Initialize();

public:
	void Start();
	void CompareFiles();

public:
	void SetData(_tstring strInputFilePath1, _tstring strInputFilePath2, long lSearchLength, _tstring strOutputFilePath, long lThreadCount)
	{
		if(IsRunning())
		{
			return;
		}

		Initialize();

		m_strFilePath1 = strInputFilePath1;
		m_strFilePath2 = strInputFilePath2;

		m_lSearchLength = lSearchLength;

		m_strOutputFilePath = strOutputFilePath;

		m_lThreadCount = lThreadCount;
	};

	bool IsReady();

public:
	void RemoveDelimiters(bool bEnable = true);
	void RemoveNewLines(bool bEnable = true);
	void ShowProgress(bool bEnable = true);

public:
	bool LoadList(map<_tstring, unsigned int>& mapList, const _tstring& strExcludeListFilePath);

	bool LoadExcludeList(const _tstring& strExcludeListFilePath);
	bool LoadKeywordList(const _tstring& strKeywordListFilePath);

private:
	void CreateResultantCounts();
	void CreateResultantPositions();

	long GetCount(map<_tstring, unsigned int>& mapCounts);

	void CalculateSimilarityF1();
	void CalculateSimilarityF2();

	_tstring CreateHeaderFileComparision();
	_tstring CreateHeaderGroupSummary();
	_tstring GetOutputHeader();

	_tstring GetOutput(map<_tstring, unsigned int>& mapCounts, map<_tstring, map<_tstring, vector<_tstring>>>& mapPositions);

	void CreateAllOutput();
	void CreateResultantOutput();

private:
	HRESULT CompareFiles(const _tstring& strInputFilePath1, const _tstring& strInputFilePath2, long lSearchLength, _tstring strOutputFilePath, long lThreadCount);

public:
	double GetProgress() { return *m_pdProgress; };
	double GetTime() { return m_dTime; };

	double GetSimilarityF1() { return m_dSimilarityF1; };
	double GetSimilarityF2() { return m_dSimilarityF2; };

	_tstring GetOutputAll() { return m_strAllOutput; };
	_tstring GetOutputResultant() { return m_strResultantOutput; };

	void WriteOutputAll();
	void WriteOutputResultant();

	void WriteOutput() { WriteOutputResultant(); };

public:
	void AddFileOccurrences(map<_tstring, unsigned int>& mapFileOccurrences);
	_tstring GetSummary();
};
