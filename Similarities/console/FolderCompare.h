#pragma once

#include "Util.h"

#include <map>

using std::map;

class CFolderCompare
{
private:
	_tstring m_strFolder1;
	_tstring m_strFolder2;

	long m_lSearchLength;

	_tstring m_strOutputFolder;

	long m_lThreadCount;

	double m_dThreshold;

	_tstring m_strWildcardFolder1;
	_tstring m_strWildcardFolder2;

	bool m_bRemoveDelimiters;
	bool m_bRemoveNewLines;
	bool m_bShowProgress;

	bool m_bIgnore0Similarities;

	_tstring m_strExcludeListFilePath;
	_tstring m_strKeywordListFilePath;

private:
	vector<_tstring> m_vecFolder1FilePaths;
	vector<_tstring> m_vecFolder2FilePaths;

	unsigned long m_ulTotalFileCombinations;
	unsigned long m_ulProcessedFileCombinations;

private:
	vector<map<_tstring, unsigned int>> m_vecmapFolder1FileOccurrences;
	vector<double> m_vecdSimilaritiesF1;
	vector<double> m_vecdSimilaritiesF2;

	vector<_tstring> m_vecstrComparisionFilePaths;
	vector<_tstring> m_vecstrGeneralSummaryFilePaths;

public:
	long GetComparisionFilePathsLength() { return (long) m_vecstrComparisionFilePaths.size(); };
	_tstring GetComparisionFilePath(long lIndex) { return m_vecstrComparisionFilePaths.at(lIndex); };
	vector<_tstring>* GetComparisionFilePaths() { return &m_vecstrComparisionFilePaths; };

	long GetGeneralSummaryFilePathsLength() { return (long) m_vecstrGeneralSummaryFilePaths.size(); };
	_tstring GetGeneralSummaryFilePath(long lIndex) { return m_vecstrGeneralSummaryFilePaths.at(lIndex); };
	vector<_tstring>* GetGeneralSummaryFilePaths() { return &m_vecstrGeneralSummaryFilePaths; };

private:
	bool m_bIsRunning;

	bool m_bStop;

public:
	bool IsRunning() { return m_bIsRunning; };

	void Stop() { m_bStop = true; };

private:
	double m_dCurrentFileProgress;
	double m_dTotalProgress;

	double m_dCurrentFileTime;
	double m_dCumulativeTime;
	double m_dTotalTime;

	_tstring m_strSummary;

	_tstring m_strLog;

public:
	CFolderCompare(void);
	~CFolderCompare(void);

public:
	void Initialize();

public:
	void Start();
	void CompareFolders();

public:
	void SetData(	const _tstring& strFolder1,
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
					bool bIgnore0Similarities);

	bool IsReady();

private:
	void CompareFolders(vector<_tstring>& vecFolder1FilePaths, vector<_tstring>& vecFolder2FilePaths);

	_tstring CreateHeaderGroupSummary();

public:
	_tstring GetOutputFolder() { return m_strOutputFolder; };
	_tstring GetSummary() { return m_strSummary; };
	_tstring GetLog() { return m_strLog; };

	double GetCurrentFileProgress() { return m_dCurrentFileProgress; };
	double GetTotalProgress() { return m_dTotalProgress; };

	double GetCurrentFileTime() { return m_dCurrentFileTime; };
	double GetCumulativeTime() { return m_dCumulativeTime; };
	double GetTotalTime() { return m_dTotalTime; };

	double GetEstimatedCompletionTime();
	double GetTimeToCompletion();

	_tstring GetFileOccurrences(map<_tstring, unsigned int>& mapFileOccurrences);

private:
	void CleanUp();
};
