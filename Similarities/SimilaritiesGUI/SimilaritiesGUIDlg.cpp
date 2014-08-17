// SimilaritiesGUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SimilaritiesGUI.h"
#include "SimilaritiesGUIDlg.h"

#include "BrowseForFolder.H"

#define SIMILARITY_TIMER				1
#define SIMILARITY_TIMER_TIMEOUT		250

#define LIST_TYPE_KEYWORDS		_T("Keywords")
#define EXCLUDE_FILE_NAME		_T("Exclude.txt")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSimilaritiesGUIDlg dialog

CSimilaritiesGUIDlg::CSimilaritiesGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimilaritiesGUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimilaritiesGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FOLDER1, m_edtFolder1);
	DDX_Control(pDX, IDC_EDIT_FOLDER2, m_edtFolder2);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_FOLDER, m_edtOutputFolder);
	DDX_Control(pDX, IDC_EDIT_SEARCH_LENGTH, m_edtSearchLength);
	DDX_Control(pDX, IDC_COMBO_KEYWORD_LIST, m_cboKeywordList);
	DDX_Control(pDX, IDC_CHECK_REMOVE_DELIMITERS, m_chkRemoveDelimiters);
	DDX_Control(pDX, IDC_CHECK_REMOVE_NEW_LINES, m_chkRemoveNewLines);
	DDX_Control(pDX, IDC_CHECK_SHOW_PROGRESS, m_chkShowProgress);
	DDX_Control(pDX, IDC_PROGRESS_CURRENT, m_prgsCurrent);
	DDX_Control(pDX, IDC_PROGRESS_TOTAL, m_prgsTotal);
	DDX_Control(pDX, IDC_EDIT_LOG, m_edtLog);
	DDX_Control(pDX, IDC_STATIC_LOG, m_stLog);
	DDX_Control(pDX, IDC_COMBO_COMPARISION_RESULTS, m_cboComparisionResults);
	DDX_Control(pDX, IDC_COMBO_GENERAL_SUMMARY, m_cboGeneralSummary);
	DDX_Control(pDX, IDC_EDIT_WILDCARD_FOLDER1, m_edtWildcardFolder1);
	DDX_Control(pDX, IDC_EDIT_WILDCARD_FOLDER2, m_edtWildcardFolder2);
	DDX_Control(pDX, IDC_COMBO_THREADS, m_cboThreads);
	DDX_Control(pDX, IDC_LIST_EXCLUDE, m_lstExclude);
	DDX_Control(pDX, IDC_STATIC_ESTIMATED_TIME, m_stEstimatedTime);
	DDX_Control(pDX, IDC_EDIT_THRESHOLD, m_edtThreshold);
	DDX_Control(pDX, IDC_CHECK_IGNORE_0_SIMILARITIES, m_chkIgnore0Similarities);
}

BEGIN_MESSAGE_MAP(CSimilaritiesGUIDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_FOLDER1, &CSimilaritiesGUIDlg::OnBnClickedButtonBrowseFolder1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_FOLDER2, &CSimilaritiesGUIDlg::OnBnClickedButtonBrowseFolder2)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_OUTPUT_FOLDER, &CSimilaritiesGUIDlg::OnBnClickedButtonBrowseOutputFolder)
	ON_BN_CLICKED(IDC_BUTTON_START, &CSimilaritiesGUIDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CSimilaritiesGUIDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO_COMPARISION_RESULTS, &CSimilaritiesGUIDlg::OnCbnSelchangeComboComparisionResults)
	ON_CBN_SELCHANGE(IDC_COMBO_GENERAL_SUMMARY, &CSimilaritiesGUIDlg::OnCbnSelchangeComboGeneralSummary)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CSimilaritiesGUIDlg::OnBnClickedButtonReload)
END_MESSAGE_MAP()

void CSimilaritiesGUIDlg::LoadCBO(vector<_tstring>& vecstrList, CComboBox& cboList, bool bHasNone)
{
	long lIndex = 0;
	long lCount = (long) vecstrList.size();

	if(lCount <= 0)
	{
		return;
	}

	cboList.ResetContent();

	if(bHasNone)
	{
		cboList.AddString(_T("NONE"));
	}

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		cboList.AddString(CUtil::GetFileName(vecstrList[lIndex]).c_str());
	}
}

void CSimilaritiesGUIDlg::LoadCBO(const _tstring& strType, CComboBox& cboList)
{
	vector<_tstring> vecstrFileList;
	vecstrFileList.clear();

	_tstring strFolder = CUtil::AddDirectoryEnding(CUtil::GetProgramDirectory());
	strFolder += strType;

	CUtil::GetFileList(strFolder, _T("*.txt"), false, vecstrFileList);

	LoadCBO(vecstrFileList, cboList, true);
}

bool CSimilaritiesGUIDlg::LoadList(const _tstring& strListFilePath, CListBox& ListBox)
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

	ListBox.ResetContent();

	long lIndex = 0;
	long lCount = CUtil::GetTokenCount(strList, _T("\n"));

	for(lIndex = 0; lIndex < lCount; lIndex++)
	{
		_tstring strToken = CUtil::GetToken(strList, _T("\n"), lIndex);
		CUtil::Trim(strToken);
		CUtil::RemoveWhiteSpaces(strToken);

		if(!strToken.empty())
		{
			ListBox.AddString(strToken.c_str());
		}
	}

	return (ListBox.GetCount() > 0);
}

void CSimilaritiesGUIDlg::LoadKeywordsList()
{
	LoadCBO(LIST_TYPE_KEYWORDS, m_cboKeywordList);
}

void CSimilaritiesGUIDlg::LoadExcludeList()
{
	_tstring strFolder = CUtil::AddDirectoryEnding(CUtil::GetProgramDirectory());
	strFolder += EXCLUDE_FILE_NAME;

	LoadList(strFolder, m_lstExclude);
}

void CSimilaritiesGUIDlg::InitializeGUI()
{
// 	m_edtFolder1.SetWindowText(_T("C:\\_Test\\Test01"));
// 	m_edtWildcardFolder1.SetWindowText(_T("*.txt"));
// 
// 	m_edtFolder2.SetWindowText(_T("C:\\_Test\\Test02"));
// 	m_edtWildcardFolder2.SetWindowText(_T("*.txt"));
// 
// 	m_edtOutputFolder.SetWindowText(_T("C:\\_Test\\Output01"));
// 
// 	m_edtSearchLength.SetWindowText(_T("4"));

	m_edtFolder1.SetWindowText(_T(""));
	m_edtWildcardFolder1.SetWindowText(_T("*.*"));

	m_edtFolder2.SetWindowText(_T(""));
	m_edtWildcardFolder2.SetWindowText(_T("*.*"));

	m_edtOutputFolder.SetWindowText(_T(""));

	m_edtSearchLength.SetWindowText(_T("16"));

	LoadKeywordsList();
	m_cboKeywordList.SetCurSel(0);

	LoadExcludeList();

	m_chkRemoveDelimiters.SetCheck(BST_CHECKED);
	m_chkRemoveNewLines.SetCheck(BST_CHECKED);
	m_chkShowProgress.SetCheck(BST_CHECKED);

	m_chkIgnore0Similarities.SetCheck(BST_CHECKED);

	long lThreadIndex = 0;
	long lThreadCount = CUtil::GetProcessorCount();

	for(lThreadIndex = 0; lThreadIndex < lThreadCount; lThreadIndex++)
	{
		m_cboThreads.AddString(CUtil::Long2String(lThreadIndex + 1).c_str());
	}

	m_cboThreads.SetCurSel(m_cboThreads.GetCount() - 1);

	m_edtThreshold.SetWindowText(_T("30"));

	m_prgsCurrent.SetPos(0);
	m_prgsTotal.SetPos(0);

	m_stLog.SetWindowText(_T(""));
	m_stEstimatedTime.SetWindowText(_T(""));

	m_edtLog.SetWindowText(_T(""));

	m_cboComparisionResults.ResetContent();
	m_cboGeneralSummary.ResetContent();
}

// CSimilaritiesGUIDlg message handlers

BOOL CSimilaritiesGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitializeGUI();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimilaritiesGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSimilaritiesGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSimilaritiesGUIDlg::OnBnClickedButtonBrowseFolder1()
{
	if(m_FC.IsRunning())
	{
		return;
	}

	CString strDirectory = BrowseForFolder(this, _T("Select a folder"), _T("C:\\"));

	m_edtFolder1.SetWindowText(strDirectory);
}

void CSimilaritiesGUIDlg::OnBnClickedButtonBrowseFolder2()
{
	if(m_FC.IsRunning())
	{
		return;
	}

	CString strDirectory = BrowseForFolder(this, _T("Select a folder"), _T("C:\\"));

	m_edtFolder2.SetWindowText(strDirectory);
}

void CSimilaritiesGUIDlg::OnBnClickedButtonBrowseOutputFolder()
{
	if(m_FC.IsRunning())
	{
		return;
	}

	CString strDirectory = BrowseForFolder(this, _T("Select an output folder"), _T("C:\\"));

	m_edtOutputFolder.SetWindowText(strDirectory);
}

void CSimilaritiesGUIDlg::OnBnClickedButtonStart()
{
	if(m_FC.IsRunning())
	{
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	CString strInputFolder1 = _T("");
	m_edtFolder1.GetWindowText(strInputFolder1);

	if(!CUtil::IsDirectory(_tstring(strInputFolder1)))
	{
		AfxMessageBox(_T("\"Original File Set\" folder is not a valid directory!"));
		return;
	}

	CString strInputFolder2 = _T("");
	m_edtFolder2.GetWindowText(strInputFolder2);

	if(!CUtil::IsDirectory(_tstring(strInputFolder2)))
	{
		AfxMessageBox(_T("\"Suspect File Set\" folder is not a valid directory!"));
		return;
	}

	CString strOutputFolder = _T("");
	m_edtOutputFolder.GetWindowText(strOutputFolder);

	if(strOutputFolder.CompareNoCase(_T("")) == 0)
	{
		AfxMessageBox(_T("\"Output\" folder is not a valid directory!"));
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	CString strSearchLength = _T("");
	m_edtSearchLength.GetWindowText(strSearchLength);
	long lSearchLength = _tstol(strSearchLength);

	if(lSearchLength <= 0)
	{
		AfxMessageBox(_T("\"Search Length\" must be a positive number!"));
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	CString strThreads = _T("");
	m_cboThreads.GetLBText(m_cboThreads.GetCurSel(), strThreads);
	long lThreadCount = _tstol(strThreads);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	CString strThreshold = _T("");
	m_edtThreshold.GetWindowText(strThreshold);
	long lThreshold = _tstol(strThreshold);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	CString strWildcardFolder1 = _T("");
	m_edtWildcardFolder1.GetWindowText(strWildcardFolder1);

	if(strWildcardFolder1.CompareNoCase(_T("")) == 0)
	{
		strWildcardFolder1 = _T("*.*");
	}

	CString strWildcardFolder2 = _T("");
	m_edtWildcardFolder2.GetWindowText(strWildcardFolder2);

	if(strWildcardFolder2.CompareNoCase(_T("")) == 0)
	{
		strWildcardFolder2 = _T("*.*");
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	bool bRemoveDelimiters = (m_chkRemoveDelimiters.GetCheck() == BST_CHECKED);
	bool bRemoveNewLines = (m_chkRemoveNewLines.GetCheck() == BST_CHECKED);
	bool bShowProgress = (m_chkShowProgress.GetCheck() == BST_CHECKED);

	bool bIgnore0Similarities = (m_chkIgnore0Similarities.GetCheck() == BST_CHECKED);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	_tstring strKeywordListFilePath = CUtil::AddDirectoryEnding(CUtil::GetProgramDirectory());
	strKeywordListFilePath += LIST_TYPE_KEYWORDS;
	strKeywordListFilePath = CUtil::AddDirectoryEnding(strKeywordListFilePath);

	CString strKeywordListName = _T("");
	m_cboKeywordList.GetLBText(m_cboKeywordList.GetCurSel(), strKeywordListName);

	strKeywordListFilePath += strKeywordListName;
	strKeywordListFilePath += _T(".txt");

	if((strKeywordListName.CompareNoCase(_T("")) == 0) || (strKeywordListName.CompareNoCase(_T("NONE")) == 0))
	{
		strKeywordListFilePath = _T("");
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	_tstring strExcludeListFilePath = CUtil::AddDirectoryEnding(CUtil::GetProgramDirectory());
	strExcludeListFilePath += EXCLUDE_FILE_NAME;
	//////////////////////////////////////////////////////////////////////////

	m_FC.SetData(	_tstring(strInputFolder1),
					_tstring(strInputFolder2),
					lSearchLength,
					_tstring(strOutputFolder),
					lThreadCount,
					_tstring(strWildcardFolder1),
					_tstring(strWildcardFolder2),
					bRemoveDelimiters,
					bRemoveNewLines,
					bShowProgress,
					_tstring(strExcludeListFilePath),
					_tstring(strKeywordListFilePath),
					lThreshold,
					bIgnore0Similarities);

	m_FC.Start();

	SetTimer(SIMILARITY_TIMER, SIMILARITY_TIMER_TIMEOUT, NULL);
}

void CSimilaritiesGUIDlg::OnBnClickedButtonStop()
{
	if(!m_FC.IsRunning())
	{
		return;
	}

	if(MessageBox(_T("Are you sure you want to stop processing?"), _T("Similarities"), MB_YESNO) != IDYES)
	{
		return;
	}

	m_FC.Stop();
}

void CSimilaritiesGUIDlg::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent == SIMILARITY_TIMER)
	{
		m_prgsCurrent.SetPos((int) m_FC.GetCurrentFileProgress());
		m_prgsTotal.SetPos((int) m_FC.GetTotalProgress());

		m_stLog.SetWindowText(m_FC.GetLog().c_str());

		_tstring strEstimatedTime = _T("Estimated time to completion: ");
		strEstimatedTime += CUtil::GetTime((long) m_FC.GetTimeToCompletion());
		m_stEstimatedTime.SetWindowText(strEstimatedTime.c_str());

		CString strLog = _T("");
		m_edtLog.GetWindowText(strLog);

		if(CUtil::CompareNoCase(m_FC.GetSummary(), _tstring(strLog)) != 0)
		{
			m_edtLog.SetWindowText(m_FC.GetSummary().c_str());
			m_edtLog.LineScroll(m_edtLog.GetLineCount() - 1);
		}

		if(m_FC.GetComparisionFilePathsLength() != m_cboComparisionResults.GetCount())
		{
			LoadCBO(*m_FC.GetComparisionFilePaths(), m_cboComparisionResults, false);
		}

		if(m_FC.GetGeneralSummaryFilePathsLength() != m_cboGeneralSummary.GetCount())
		{
			LoadCBO(*m_FC.GetGeneralSummaryFilePaths(), m_cboGeneralSummary, false);
		}

		if(!m_FC.IsRunning())
		{
			KillTimer(SIMILARITY_TIMER);

			m_prgsCurrent.SetPos(0);
			m_prgsTotal.SetPos(0);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CSimilaritiesGUIDlg::OnCbnSelchangeComboComparisionResults()
{
	long lIndex = m_cboComparisionResults.GetCurSel();

	CUtil::Execute(m_FC.GetComparisionFilePath(lIndex), _T(""), true, false);
}

void CSimilaritiesGUIDlg::OnCbnSelchangeComboGeneralSummary()
{
	long lIndex = m_cboGeneralSummary.GetCurSel();

	CUtil::Execute(m_FC.GetGeneralSummaryFilePath(lIndex), _T(""), true, false);
}

void CSimilaritiesGUIDlg::OnBnClickedButtonReload()
{
	LoadKeywordsList();
	m_cboKeywordList.SetCurSel(0);

	LoadExcludeList();
}
