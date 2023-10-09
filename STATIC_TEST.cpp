// STATIC_TEST.cpp: 实现文件
//

#include "pch.h"
#include "FtpCode.h"
#include "STATIC_TEST.h"
#include "afxdialogex.h"


// STATIC_TEST 对话框

IMPLEMENT_DYNAMIC(STATIC_TEST, CDialogEx)

STATIC_TEST::STATIC_TEST(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

STATIC_TEST::~STATIC_TEST()
{
}

void STATIC_TEST::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(STATIC_TEST, CDialogEx)
END_MESSAGE_MAP()


// STATIC_TEST 消息处理程序
