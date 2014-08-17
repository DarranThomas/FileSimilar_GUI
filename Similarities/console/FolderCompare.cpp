#include "stdafx.h"
#include "FolderCompare.h"

#include "FileCompare.h"

///////////////////////////////////////////////////////////////////////////////////
// CFolderCompare thread
///////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI StartFolderCompareThread(LPVOID param)
{
	CFolderCompare* pFC = (CFolderCompare*) param;
	pFC->CompareFolders();
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////
// CFolderCompare class definitions
///////////////////////////////////////////////////////////////////////////////////
CFolderCompare::CFolderCompare(void)
{
	Initialize();
}

CFolderCompare::~CFolderCompare(void)
{
	Initialize();
}

void CFolderCompare::Initialize()
{
	m_strFolder1 = _T("");
	m_strFolder2 = _T("");

	m_lSearchLength = 0;

	m_strOutputFolder = _T("");

	m_lThreadCount = 0;

	m_dThreshold = 0.0;

	m_strWildcardFolder1 = _T("*.*");
	m_strWildcardFolder2 = _T("*.*");

	m_bRemoveDelimiters = true;
	m_bRemoveNewLines = true;
	m_bShowProgress = true;

	m_bIgnore0Similarities = true;

	m_strExcludeListFilePath = _T("");
	m_strKeywordListFilePath = _T("");

	m_vecFolder1FilePaths.clear();
	m_vecFolder2FilePaths.clear();

	m_ulTotalFileCombinations = 0;
	m_ulProcessedFileCombinations = 0;

	m_vecmapFolder1FileOccurrences.clear();
	m_vecdSimilaritiesF1.clear();
	m_vecdSimilaritiesF2.clear();

	m_vecstrComparisionFilePaths.clear();
	m_vecstrGeneralSummaryFilePaths.clear();

	m_bIsRunning = false;

	m_bStop = false;

	m_dCurrentFileProgress = 0.0;
	m_dTotalProgress = 0.0;

	m_dCurrentFileTime = 0.0;
	m_dCumulativeTime = 0.0;
	m_dTotalTime = 0.0;

	m_strSummary = _T("");

	m_strLog = _T("");
}

void CFolderCompare::Start()
{
	if(IsRunning())
	{
		return;
	}

	m_bIsRunning = true;
	m_bStop = false;

	DWORD dwThreadId;
	CreateThread(NULL, 0, StartFolderCompareThread, (void*) this, 0, &dwThreadId);
}

void CFolderCompare::CompareFolders()
{
	if(!IsReady())
	{
		return;
	}

	DWORD dwStart = ::GetTickCount();

	CompareFolders(m_vecFolder1FilePaths, m_vecFolder2FilePaths);

	DWORD dwEnd = ::GetTickCount();

	m_dTotalTime = (double) (dwEnd - dwStart) / 1000.0;

	m_bIsRunning = false;
	m_bStop = false;
}

void CFolderCompare::SetData(	const _tstring& strFolder1,
								const _tstring& strFolder2,

								long lSearchLength,

								const _tstring& strOutputFolder,

								long lThreadCount,

								const _tstring& strWildcardFolder1,
								const _tstring& strWildcardFolder2,

								bool bRemoveDelimiters,
								bool bRemoveNewLines,
								bool bShowProgress,

								const _tstring& strExcludeListFilePath,
								const _tstring& strKeywordListFilePath,

								long lThreshold,
								bool bIgnore0Similarities)
{
	if(IsRunning())
	{
		return;
	}

	if(CUtil::CompareNoCase(strFolder1, strFolder2) == 0)
	{
		return;
	}

	Initialize();

	m_strFolder1 = strFolder1;
	m_strFolder2 = strFolder2;

	m_lSearchLength = lSearchLength;

	m_strOutputFolder = strOutputFolder;

	if(!CUtil::IsDirectory(m_strOutputFolder))
	{
		CUtil::MakeDirectory(m_strOutputFolder);
	}

	m_lThreadCount = lThreadCount;

	m_dThreshold = (double) lThreshold / 100.0;

	m_strWildcardFolder1 = strWildcardFolder1;
	m_strWildcardFolder2 = strWildcardFolder2;

	CUtil::GetFileList(m_strFolder1, m_strWildcardFolder1, false, m_vecFolder1FilePaths);
	CUtil::GetFileList(m_strFolder2, m_strWildcardFolder2, false, m_vecFolder2FilePaths);

	m_ulTotalFileCombinations = (unsigned long) (m_vecFolder1FilePaths.size() * m_vecFolder2FilePaths.size());

	m_bRemoveDelimiters = bRemoveDelimiters;
	m_bRemoveNewLines = bRemoveNewLines;
	m_bShowProgress = bShowProgress;

	m_bIgnore0Similarities = bIgnore0Similarities;

	m_strExcludeListFilePath = strExcludeListFilePath;
	m_strKeywordListFilePath = strKeywordListFilePath;
}

bool CFolderCompare::IsReady()
{
	if(!CUtil::IsDirectory(m_strFolder1))
	{
		return false;
	}

	if(!CUtil::IsDirectory(m_strFolder2))
	{
		return false;
	}

	if(m_lSearchLength <= 0)
	{
		return false;
	}

	if(!CUtil::IsDirectory(m_strOutputFolder))
	{
		return false;
	}

	if(m_lThreadCount < 0)
	{
		return false;
	}

	if(m_strWildcardFolder1.empty())
	{
		return false;
	}

	if(m_strWildcardFolder2.empty())
	{
		return false;
	}

	if(m_vecFolder1FilePaths.empty())
	{
		return false;
	}

	if(m_vecFolder2FilePaths.empty())
	{
		return false;
	}

	if(m_ulTotalFileCombinations <= 0)
	{
		return false;
	}

	return true;
}

void CFolderCompare::CompareFolders(vector<_tstring>& vecFolder1FilePaths, vector<_tstring>& vecFolder2FilePaths)
{
	_tstring strGroupSummaryFolder = CUtil::AddDirectoryEnding(m_strOutputFolder);
	strGroupSummaryFolder += _T("GroupSummary");

	CUtil::MakeDirectory(strGroupSummaryFolder);

	m_strSummary = _T("");

	unsigned long ulCombinationsProcessed = 0;

	long lF1Index = 0;
	long lF1Count = (long) vecFolder1FilePaths.size();

	for(lF1Index = 0; lF1Index < lF1Count; lF1Index++)
	{
		map<_tstring, unsigned int> mapFileOccurrences;

		long lF2Index = 0;
		long lF2Count = (long) vecFolder2FilePaths.size();

		for(lF2Index = 0; lF2Index < lF2Count; lF2Index++)
		{
			_tstring strFile1Path = vecFolder1FilePaths[lF1Index];
			_tstring strFile2Path = vecFolder2FilePaths[lF2Index];

			m_strLog = _T("Processing Files: ");
			m_strLog += CUtil::GetFileName(strFile1Path, true);
			m_strLog += _T(" <=> ");
			m_strLog += CUtil::GetFileName(strFile2Path, true);

			long lSearchLength = m_lSearchLength;

			_tstring strOutputFilePath = CUtil::AddDirectoryEnding(m_strOutputFolder);
			strOutputFilePath += CUtil::GetFileName(strFile1Path, true);
			strOutputFilePath += _T("_FOUND-IN_");
			strOutputFilePath += CUtil::GetFileName(strFile2Path, true);
			strOutputFilePath += _T(".txt");

			long lThreadCount = m_lThreadCount;

			CFileCompare FC;

			FC.SetData(strFile1Path, strFile2Path, lSearchLength, strOutputFilePath, lThreadCount);

			FC.LoadExcludeList(m_strExcludeListFilePath);
			FC.LoadKeywordList(m_strKeywordListFilePath);

			FC.RemoveDelimiters(m_bRemoveDelimiters);
			FC.RemoveNewLines(m_bRemoveNewLines);
			FC.ShowProgress(m_bShowProgress);

			FC.Start();

			while(FC.IsRunning())
			{
				Sleep(250);

				m_dCurrentFileProgress = FC.GetProgress();
			}

			if(m_bIgnore0Similarities && (FC.GetSimilarityF1() <= 0.01) && (FC.GetSimilarityF2() <= 0.01))
			{
				// Ignore! Do not write file, or add it to list!

				_tunlink(strOutputFilePath.c_str());
			}
			else
			{
				FC.WriteOutput();

				if(FC.GetSimilarityF1() > m_dThreshold && m_dThreshold >= 0.01 && m_dThreshold <= 0.99)
				{
					_tstring strNewOutputFilePath = CUtil::GetFileDirectory(strOutputFilePath);
					strNewOutputFilePath = CUtil::AddDirectoryEnding(strNewOutputFilePath);
					strNewOutputFilePath += _T("_");
					strNewOutputFilePath += CUtil::Long2String((long) (m_dThreshold * 100.0));
					strNewOutputFilePath += _T("%_");
					strNewOutputFilePath += CUtil::GetFileName(strOutputFilePath, true);

					_trename(strOutputFilePath.c_str(), strNewOutputFilePath.c_str());

					m_vecstrComparisionFilePaths.push_back(strNewOutputFilePath);
				}
				else
				{
					m_vecstrComparisionFilePaths.push_back(strOutputFilePath);
				}
			}

			ulCombinationsProcessed++;

			m_dCurrentFileTime = FC.GetTime();
			m_dCumulativeTime += m_dCurrentFileTime;

			m_dTotalProgress = ((double) ulCombinationsProcessed / m_ulTotalFileCombinations) * 100.0;
			m_ulProcessedFileCombinations = ulCombinationsProcessed;

// 			m_vecdSimilaritiesF1.push_back(FC.GetSimilarityF1());
// 			m_vecdSimilaritiesF2.push_back(FC.GetSimilarityF2());

			FC.AddFileOccurrences(mapFileOccurrences);

			m_strSummary += FC.GetSummary();

			if(m_bStop)
			{
				break;
			}
		}

		m_vecmapFolder1FileOccurrences.push_back(mapFileOccurrences);

		_tstring strFolder1FileOccurrencesPath = CUtil::AddDirectoryEnding(strGroupSummaryFolder);
		strFolder1FileOccurrencesPath += CUtil::GetFileName(vecFolder1FilePaths[lF1Index]);
		strFolder1FileOccurrencesPath += _T(".");
		strFolder1FileOccurrencesPath += CUtil::GetFileExtension(vecFolder1FilePaths[lF1Index]);
		strFolder1FileOccurrencesPath += _T(".txt");

		CUtil::WriteFile(strFolder1FileOccurrencesPath, GetFileOccurrences(mapFileOccurrences));

		m_vecstrGeneralSummaryFilePaths.push_back(strFolder1FileOccurrencesPath);

		if(m_bStop)
		{
			break;
		}
	}

	CleanUp();

	_tstring strSummaryFilePath = CUtil::AddDirectoryEnding(strGroupSummaryFolder);
	strSummaryFilePath += _T("_Summary.txt");

	CUtil::WriteFile(strSummaryFilePath, m_strSummary);
}

_tstring CFolderCompare::CreateHeaderGroupSummary()
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

	long lHeaderStringLength = m_lSearchLength + (long) _tstring(TAB).length() + 2;

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

double CFolderCompare::GetEstimatedCompletionTime()
{
	double dExtimatedCompletionTime = 0.0;

	if(IsRunning())
	{
		dExtimatedCompletionTime = (m_dCumulativeTime / m_ulProcessedFileCombinations) * m_ulTotalFileCombinations;
	}
	else
	{
		dExtimatedCompletionTime = 30.0 * m_ulTotalFileCombinations;
	}

	return dExtimatedCompletionTime;
}

double CFolderCompare::GetTimeToCompletion()
{
	double dTimeToCompletion = 0.0;

	if(m_ulProcessedFileCombinations > 0)
	{
		dTimeToCompletion = (m_dCumulativeTime / m_ulProcessedFileCombinations) * (m_ulTotalFileCombinations - m_ulProcessedFileCombinations);
	}
	else
	{
		dTimeToCompletion = 30.0 * (m_ulTotalFileCombinations - m_ulProcessedFileCombinations);
	}

	return dTimeToCompletion;
}

_tstring CFolderCompare::GetFileOccurrences(map<_tstring, unsigned int>& mapFileOccurrences)
{
	_tstring strFileOccurrences = CreateHeaderGroupSummary();

	for(map<_tstring, unsigned int>::iterator iMapIndex = mapFileOccurrences.begin(); iMapIndex != mapFileOccurrences.end(); ++iMapIndex)
	{
		strFileOccurrences += _T("\"");
		strFileOccurrences += (*iMapIndex).first;
		strFileOccurrences += _T("\"");
		strFileOccurrences += TAB;
		strFileOccurrences += CUtil::Long2String((*iMapIndex).second);
		strFileOccurrences += NEW_LINE;
	}

	return strFileOccurrences;
}

void CFolderCompare::CleanUp()
{
	if(!IsReady())
	{
		return;
	}

	_tstring strWildcard = _T("*");
	strWildcard += TEMP_FILE_EXT;

	CUtil::DeleteFiles(m_strFolder1, strWildcard, false);
	CUtil::DeleteFiles(m_strFolder2, strWildcard, false);
}
