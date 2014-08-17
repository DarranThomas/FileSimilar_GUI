// SimilaritiesGUIDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "../console/FolderCompare.h"


// CSimilaritiesGUIDlg dialog
class CSimilaritiesGUIDlg : public CDialog
{
private:
	CFolderCompare m_FC;

private:
	void LoadCBO(vector<_tstring>& vecstrList, CComboBox& cboList, bool bHasNone);
	void LoadCBO(const _tstring& strType, CComboBox& cboList);

	bool LoadList(const _tstring& strListFilePath, CListBox& ListBox);

private:
	void LoadKeywordsList();
	void LoadExcludeList();

private:
	void InitializeGUI();

// Construction
public:
	CSimilaritiesGUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SIMILARITIESGUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtFolder1;
	CEdit m_edtFolder2;
	CEdit m_edtOutputFolder;
	CEdit m_edtSearchLength;
	CComboBox m_cboKeywordList;
	CButton m_chkRemoveDelimiters;
	CButton m_chkRemoveNewLines;
	CButton m_chkShowProgress;
	CProgressCtrl m_prgsCurrent;
	CProgressCtrl m_prgsTotal;
	CEdit m_edtLog;
	afx_msg void OnBnClickedButtonBrowseFolder1();
	afx_msg void OnBnClickedButtonBrowseFolder2();
	afx_msg void OnBnClickedButtonBrowseOutputFolder();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_stLog;
	CComboBox m_cboComparisionResults;
	CComboBox m_cboGeneralSummary;
	afx_msg void OnCbnSelchangeComboComparisionResults();
	afx_msg void OnCbnSelchangeComboGeneralSummary();
	CEdit m_edtWildcardFolder1;
	CEdit m_edtWildcardFolder2;
	CComboBox m_cboThreads;
	CListBox m_lstExclude;
	afx_msg void OnBnClickedButtonReload();
	CStatic m_stEstimatedTime;
	CEdit m_edtThreshold;
	CButton m_chkIgnore0Similarities;
};
