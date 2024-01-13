
// LeDtmbDlg.h : header file
//

#pragma once
#define   WM_UPDATEDATA   WM_USER + 5 

// CLeDtmbDlg dialog
class CLeDtmbDlg : public CDialogEx
{
	// Construction
public:
	CLeDtmbDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LEDTMB_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioAllSelect();
	afx_msg void OnBnClickedRadioInvertSelect();
	afx_msg void OnNMDblclkRatelist(NMHDR* pNMHDR, LRESULT* pResult);
	//afx_msg void OnLvnItemchangedAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult);
	LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);

	//private:
	BOOL log(CString& str);

public:
	int m_nPreSel = -1;
	CEdit m_LogCtrl;
	CString m_LogValue;
	// all rates
	CListCtrl m_RatesListCtrl;
	// AvailableRateList
	CListCtrl m_AvailableRateListCtrl;
	CIPAddressCtrl m_IpCtrl;
	int m_PortValue;
	CString m_VlcPathValue;
	int m_nSelItemIndex;

	CProgressCtrl m_ProGressCtrl;

	afx_msg void OnBnClickedButtonSelect();

	afx_msg void OnNMClickAvailableRatelist(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnBnClickedButtonSearch();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonVlc();
	afx_msg void OnBnClickedButtonStream();
	afx_msg void OnLvnGetEmptyMarkupAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnInsertitemAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedButtonStop();
	void doSearch();
	afx_msg void OnLvnItemchangedAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclkAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult);
};
