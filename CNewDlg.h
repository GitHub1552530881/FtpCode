#pragma once


// CNewDlg 对话框

class CNewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CNewDlg)

public:
	CNewDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CNewDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//CStatic m_static;
	CButton m_static;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CButton m_text;
};
