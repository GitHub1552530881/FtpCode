
// FtpCodeDlg.h: 头文件
//

#pragma once


// CFtpCodeDlg 对话框
class CFtpCodeDlg : public CDialogEx
{
// 构造
public:
	CFtpCodeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FTPCODE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_uplist;
	CListBox m_loadlist;
	afx_msg void OnBnClickedfilebtn();
	afx_msg void OnBnClickeduplownbtn();
	afx_msg void OnBnClickedprjbtn();
	afx_msg void OnBnClickedinibtn();
	afx_msg void OnBnClickeddownloadbtn();
	CEdit m_edit;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();
	CListBox m_list1;
	CListBox m_list2;
	CListBox m_show_list;

	//线程上传ftp
	//void ftpUp(void *arg);
	CProgressCtrl m_pro;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
