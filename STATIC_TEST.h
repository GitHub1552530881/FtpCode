#pragma once


// STATIC_TEST 对话框

class STATIC_TEST : public CDialogEx
{
	DECLARE_DYNAMIC(STATIC_TEST)

public:
	STATIC_TEST(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~STATIC_TEST();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
