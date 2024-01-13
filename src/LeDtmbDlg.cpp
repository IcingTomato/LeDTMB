
// LeDtmbDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "LeDtmb.h"
#include "LeDtmbDlg.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLeDtmbDlg dialog



CLeDtmbDlg::CLeDtmbDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LEDTMB_DIALOG, pParent)
	, m_LogValue(_T(""))
	, m_PortValue(4567)
	, m_VlcPathValue(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLeDtmbDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RATELIST, m_RatesListCtrl);
	DDX_Control(pDX, IDC_EDIT_Log, m_LogCtrl);
	DDX_Text(pDX, IDC_EDIT_Log, m_LogValue);
	DDX_Control(pDX, IDC_AvailableRateList, m_AvailableRateListCtrl);
	DDX_Control(pDX, IDC_IPADDRESS_Ip, m_IpCtrl);
	DDX_Text(pDX, IDC_EDIT_Port, m_PortValue);
	DDV_MinMaxInt(pDX, m_PortValue, 0, 65535);
	DDX_Text(pDX, IDC_EDIT_VLCPath, m_VlcPathValue);

	DDX_Control(pDX, IDC_PROGRESS1, m_ProGressCtrl);

}

BEGIN_MESSAGE_MAP(CLeDtmbDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO1, &CLeDtmbDlg::OnBnClickedRadioAllSelect)
	ON_BN_CLICKED(IDC_RADIO2, &CLeDtmbDlg::OnBnClickedRadioInvertSelect)
	ON_NOTIFY(NM_DBLCLK, IDC_RATELIST, &CLeDtmbDlg::OnNMDblclkRatelist)
	ON_BN_CLICKED(IDC_BUTTON_Select, &CLeDtmbDlg::OnBnClickedButtonSelect)
	//ON_NOTIFY(NM_CLICK, IDC_AvailableRateList, &CLeDtmbDlg::OnNMClickAvailableRatelist)
	ON_BN_CLICKED(IDC_BUTTON_Search, &CLeDtmbDlg::OnBnClickedButtonSearch)
	ON_BN_CLICKED(IDC_BUTTON_Connect, &CLeDtmbDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_Refresh, &CLeDtmbDlg::OnBnClickedButtonRefresh)
	ON_BN_CLICKED(IDC_BUTTON_VLC, &CLeDtmbDlg::OnBnClickedButtonVlc)
	ON_BN_CLICKED(IDC_BUTTON_Stream, &CLeDtmbDlg::OnBnClickedButtonStream)
	ON_NOTIFY(LVN_GETEMPTYMARKUP, IDC_AvailableRateList, &CLeDtmbDlg::OnLvnGetEmptyMarkupAvailableratelist)
	ON_NOTIFY(LVN_INSERTITEM, IDC_AvailableRateList, &CLeDtmbDlg::OnLvnInsertitemAvailableratelist)
	ON_BN_CLICKED(IDC_BUTTON1, &CLeDtmbDlg::OnBnClickedButtonStop)

	ON_MESSAGE(WM_UPDATEDATA, OnUpdateData)

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_AvailableRateList, &CLeDtmbDlg::OnLvnItemchangedAvailableratelist)
	ON_NOTIFY(NM_DBLCLK, IDC_AvailableRateList, &CLeDtmbDlg::OnNMDblclkAvailableratelist)
END_MESSAGE_MAP()


// CLeDtmbDlg message handlers

BOOL CLeDtmbDlg::OnInitDialog()
{

	CDialogEx::OnInitDialog();
	if (!theUsb.initOpen()) {
		::MessageBox(0, _T("请确保已插入设备且安装驱动！"), _T("打开设备失败！"), MB_OK);
		return FALSE;
	}
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_RatesListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);
	m_RatesListCtrl.InsertColumn(0, _T("rate"), LVCFMT_LEFT, 100, 0);
	for (int i = 0; i < sizeof(rates) / sizeof(int); ++i) {
		m_RatesListCtrl.InsertItem(i, _T(""));
		m_RatesListCtrl.SetItemText(i, 0, std::to_wstring(rates[i]).c_str());
	}
	m_AvailableRateListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES);
	m_AvailableRateListCtrl.InsertColumn(0, _T("rate"), LVCFMT_LEFT, 100, 0);
	m_AvailableRateListCtrl.InsertColumn(1, _T("strength"), LVCFMT_LEFT, 100, 0);
	m_AvailableRateListCtrl.InsertColumn(2, _T("quality"), LVCFMT_LEFT, 100, 0);


	//this->GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(FALSE);
	//this->GetDlgItem(IDC_BUTTON_Refresh)->EnableWindow(FALSE);
	//this->GetDlgItem(IDC_BUTTON_VLC)->EnableWindow(FALSE);


	m_IpCtrl.SetWindowTextW(_T("127.0.0.1"));

	m_ProGressCtrl.ShowWindow(SW_HIDE);

	log(CString("init"));

	//if (!theUsb.initOpen()) {
	//	::MessageBox(0, _T("请确保已插入设备且安装驱动！"), _T("打开设备失败！"), MB_OK);
	//	return FALSE;
	//}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CLeDtmbDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLeDtmbDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLeDtmbDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void CLeDtmbDlg::OnBnClickedRadioAllSelect()
{
	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_RatesListCtrl.GetItemCount(); i++) {
		m_RatesListCtrl.SetCheck(i, TRUE);
		m_RatesListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}

}


void CLeDtmbDlg::OnBnClickedRadioInvertSelect()
{
	// TODO: Add your control notification handler code here
	BOOL state;
	for (int i = 0; i < m_RatesListCtrl.GetItemCount(); i++) {
		state = m_RatesListCtrl.GetCheck(i);
		if (state == FALSE) {
			m_RatesListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
			m_RatesListCtrl.SetCheck(i, TRUE);
		}
		else {
			m_RatesListCtrl.SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
			m_RatesListCtrl.SetCheck(i, FALSE);
		}
	}
}





void CLeDtmbDlg::OnNMDblclkRatelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int i = pNMItemActivate->iItem;
	BOOL state = m_RatesListCtrl.GetCheck(i);
	if (state == FALSE) {
		m_RatesListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		m_RatesListCtrl.SetCheck(i, TRUE);
	}
	else {
		m_RatesListCtrl.SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
		m_RatesListCtrl.SetCheck(i, FALSE);
	}
}

LRESULT CLeDtmbDlg::OnUpdateData(WPARAM wParam, LPARAM lParam)
{
	UpdateData(wParam);
	return 0;
}

BOOL CLeDtmbDlg::log(CString& str)
{
	int LineCnt = m_LogCtrl.GetLineCount();
	if (LineCnt > 100)
	{
		m_LogCtrl.SetWindowText(_T(""));
		m_LogValue.Empty();
	}

	CString tmpStr;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	tmpStr.Format(_T("[%4d-%02d-%02d %02d:%02d:%02d]>>>"), sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
	m_LogValue += tmpStr;	// 添加log显示时间
	m_LogValue += str;	// 需要显示的信息
	m_LogValue += _T("\r\n");	// 换行
	// 将m_InfoStr的值显示到界面
	//UpdateData(FALSE);
	SendMessage(WM_UPDATEDATA, FALSE);
	// 将滚轮调到最后
	m_LogCtrl.LineScroll(m_LogCtrl.GetLineCount());
	return 0;
}



void CLeDtmbDlg::OnBnClickedButtonSelect()
{
	// TODO: Add your control notification handler code here

	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.exe)|*.exe|All Files (*.*)|*.*||"), NULL);

	if (dlgFile.DoModal())
	{
		m_VlcPathValue = dlgFile.GetPathName();
		UpdateData(FALSE);
	}
}





//
void CLeDtmbDlg::OnNMClickAvailableRatelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	for (int i = 0; i < m_AvailableRateListCtrl.GetItemCount(); i++)
	{
		if (m_AvailableRateListCtrl.GetCheck(i) == FALSE)  //未被选中的
		{
			if (i == pNMItemActivate->iItem) {
				m_AvailableRateListCtrl.SetCheck(i, TRUE);
				m_AvailableRateListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

				GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(TRUE);
			}
		}
		else  //选中的
		{
			if (i == pNMItemActivate->iItem) {
				m_AvailableRateListCtrl.SetCheck(i, FALSE);

			}
			m_AvailableRateListCtrl.SetCheck(i, FALSE);
		}

	}
}


void CLeDtmbDlg::OnBnClickedButtonSearch()
{
	// TODO: Add your control notification handler code here
	//theUsb.dlg->GetDlgItem(idc_butt)
	GetDlgItem(IDC_BUTTON_Search)->EnableWindow(FALSE);

	m_AvailableRateListCtrl.DeleteAllItems();

	log(CString("start search~"));

	std::thread t(&CLeDtmbDlg::doSearch, this);
	t.detach();
}


void CLeDtmbDlg::OnBnClickedButtonConnect()
{
	log(CString(_T("连接中~")));
	GetDlgItem(IDC_BUTTON_Stream)->EnableWindow(FALSE);

	// TODO: Add your control notification handler code here
	//int i = m_AvailableRateListCtrl.GetSelectionMark();
	int i = -1;
	for (int n = 0; n < m_AvailableRateListCtrl.GetItemCount(); ++n) {
		if (m_AvailableRateListCtrl.GetCheck(n)) {
			i=n;
			break;
		}
	}
	//判断是否已选择某项
	if (-1 == i)
	{
		AfxMessageBox(_T("请选择一项"));
		return;
	}
	//m_AvailableRateListCtrl.EnableWindow(FALSE);
	CString cs = m_AvailableRateListCtrl.GetItemText(i, 1);
	theUsb.setRate(_ttoi(cs.GetBuffer()));
	log(theUsb.getSignal());
	log(CString(_T("连接成功！")));
	GetDlgItem(IDC_BUTTON_Stream)->EnableWindow(TRUE);

}


void CLeDtmbDlg::OnBnClickedButtonRefresh()
{
	log(CString("开始刷新"));

	// TODO: Add your control notification handler code here
	for (int i = 0; i < m_AvailableRateListCtrl.GetItemCount(); ++i) {
		//theUsb.setExit(TRUE);
		if (!m_AvailableRateListCtrl.GetCheck(i))
			continue;
		theUsb.refreshSignal(i, _ttoi(m_AvailableRateListCtrl.GetItemText(i, 0).GetBuffer()));
		break;

	}
	log(CString("刷新完成"));
}


void CLeDtmbDlg::OnBnClickedButtonVlc()
{
	// TODO: Add your control notification handler code here
	log(CString("开始启动vlc"));
	UpdateData(TRUE);

	if (m_VlcPathValue.IsEmpty()) {
		log(CString("vlc路径为空"));
		return;
	}

	CString cs;
	CString csIP;
	m_IpCtrl.GetWindowText(csIP);
	cs.Format(_T("udp://@%s:%d"), csIP, m_PortValue);
	//GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(TRUE);

	log(CString("启动vlc完成"));
	ShellExecute(NULL, _T("open"), m_VlcPathValue, cs, NULL, SW_SHOWNORMAL);
}


void CLeDtmbDlg::OnBnClickedButtonStream()
{
	log(CString("开始启动串流"));

	CString cs;
	// TODO: Add your control notification handler code here
	//int i = m_AvailableRateListCtrl.GetSelectionMark();
	int i = -1;
	for (int n = 0; n < m_AvailableRateListCtrl.GetItemCount(); ++n) {
		if (m_AvailableRateListCtrl.GetCheck(n)) {
			i = n;
			break;
		}
	}
	//判断是否已选择某项
	if (-1 == i)
	{
		AfxMessageBox(_T("请选择一项"));
		return;
	}
	UpdateData(TRUE);
	DWORD dwip = 0;
	m_IpCtrl.GetAddress(dwip);
	if (dwip && m_PortValue) {
		CString csIP;
		m_IpCtrl.GetWindowText(csIP);
		cs.Format(_T("udp://@%s:%d"), csIP, m_PortValue);
		log(cs);
		//GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(TRUE);
		theUsb.setIp(htonl(dwip));
		theUsb.setPort(m_PortValue);
		theUsb.setRate(_ttoi(m_AvailableRateListCtrl.GetItemText(i, 0).GetBuffer()));
		std::thread t(&DtmbUsb::dtmbRead, &theUsb);
		t.detach();
		GetDlgItem(IDC_BUTTON_Stream)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(FALSE);
		m_AvailableRateListCtrl.EnableWindow(FALSE);
	}
	else
	{
		AfxMessageBox(_T("ip或端口为错误"));
		return;
	}
}



//可用频率列表为空时，置灰连接按钮
void CLeDtmbDlg::OnLvnGetEmptyMarkupAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	log(CString("当前无可用频率，请搜索！"));
	NMLVEMPTYMARKUP* pStateChanged = reinterpret_cast<NMLVEMPTYMARKUP*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Stream)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Refresh)->EnableWindow(FALSE);

}

//可用频率存在时，激活连接按钮，判断串流按钮
void CLeDtmbDlg::OnLvnInsertitemAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	//if (!GetDlgItem(IDC_BUTTON_Connect)->IsWindowEnabled())
		GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(TRUE);

	//GetDlgItem(IDC_BUTTON_Stream)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Refresh)->EnableWindow(TRUE);
}


void CLeDtmbDlg::OnBnClickedButtonStop()
{
	log(CString("stop"));

	// TODO: Add your control notification handler code here
	theUsb.setExit(true);
	//GetDlgItem(IDC_BUTTON_Stop)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_Stream)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(TRUE);
	m_AvailableRateListCtrl.EnableWindow(TRUE);


}

void CLeDtmbDlg::doSearch()
{
	std::vector<int> vs;
	std::vector<int> s;
	std::vector<float> q;
	for (int i = 0; i < m_RatesListCtrl.GetItemCount(); ++i) {
		if (m_RatesListCtrl.GetCheck(i)) {
			vs.push_back(_ttoi(m_RatesListCtrl.GetItemText(i, 0)));
		}
	}
	if (vs.size() == 0) {
		log(CString("no rate item select"));
		return;
	}
	m_ProGressCtrl.ShowWindow(SW_SHOW);
	vs = theUsb.getAvailableRates(vs, s, q);
	m_ProGressCtrl.ShowWindow(SW_HIDE);

	if (vs.size() == 0) {
		GetDlgItem(IDC_BUTTON_Connect)->EnableWindow(FALSE);
	}
	else
	{
		for (int i = 0; i < vs.size(); ++i) {
			m_AvailableRateListCtrl.InsertItem(i, _T(""));
			m_AvailableRateListCtrl.SetItemText(i, 0, std::to_wstring(vs[i]).c_str());
			m_AvailableRateListCtrl.SetItemText(i, 1, std::to_wstring(s[i]).c_str());
			m_AvailableRateListCtrl.SetItemText(i, 2, std::to_wstring(q[i]).c_str());
		}
	}
	GetDlgItem(IDC_BUTTON_Search)->EnableWindow(TRUE);

}


void CLeDtmbDlg::OnLvnItemchangedAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (m_nPreSel != -1 && m_nPreSel != pNMLV->iItem)
	{
		m_AvailableRateListCtrl.SetCheck(m_nPreSel, FALSE);	//设置上次选中状态复选框为未选中状态
	}

	m_nPreSel = pNMLV->iItem;

}


void CLeDtmbDlg::OnNMDblclkAvailableratelist(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	int i = pNMItemActivate->iItem;
	BOOL state = m_AvailableRateListCtrl.GetCheck(i);
	if (state == FALSE) {
		m_AvailableRateListCtrl.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
		m_AvailableRateListCtrl.SetCheck(i, TRUE);
	}
	else {
		m_AvailableRateListCtrl.SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
		m_AvailableRateListCtrl.SetCheck(i, FALSE);
	}
}
