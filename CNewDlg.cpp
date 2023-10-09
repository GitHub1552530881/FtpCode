// CNewDlg.cpp: 实现文件
//

#include "pch.h"
#include "FtpCode.h"
#include "CNewDlg.h"
#include "afxdialogex.h"


// CNewDlg 对话框

IMPLEMENT_DYNAMIC(CNewDlg, CDialogEx)

CNewDlg::CNewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CNewDlg::~CNewDlg()
{
}

void CNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDCANCEL, m_static);
	DDX_Control(pDX, IDOK, m_text);
}


BEGIN_MESSAGE_MAP(CNewDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CNewDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CNewDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CNewDlg 消息处理程序


void CNewDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}


void CNewDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();

}
