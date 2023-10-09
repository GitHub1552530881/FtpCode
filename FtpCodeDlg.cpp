
// FtpCodeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "FtpCode.h"
#include "FtpCodeDlg.h"
#include "afxdialogex.h"
#include "CNewDlg.h"

#include <iostream>
#include <io.h>
#include <direct.h>

#include <thread>
#include <vector>
#include <stdio.h>
#include <Windows.h>
#include <WinInet.h>
#pragma comment(lib, "WinInet.lib")
using namespace std;

#include<Windows.h>
#include<stdio.h>
#include <thread>
#include <future>
#include <mutex>


char *ftpip = "10.90.66.154";
char *username = "autodispatch";
char *passwd = "kjfdifm#diwt7o9ioi@";
CString up_local_path;//上传时，本地的路径名
HINTERNET Internet, Connect = NULL;
vector<CString> v_ftp_prj_name;		//存放ftp服务器中/FW/AOI/0DMH/目录下的文件名
vector<CString> v_ftp_ini_name;		//存放ftp服务器中/BARCODE/0AKE/目录下的文件名
CString edit_str;//编辑框edit中的内容

//cstring转为char*
char * CStringToChar(CString cstring);
//清理目录下的所有文件
void ClearDirectory(string dir);
//获取当前系统时间
CString LocalTime();
//获取ftp指定目录下的文件名，存放到vector
vector<CString> getFtpVector(CString str);
//ftp下载文件
BOOL FTP_Download(char *szHostName, char *szUserName, char *szPassword,
	char *szUrlPath, char *SavePath);
//ftp保存文件
BOOL Ftp_SaveToFile(char *pszFileName, BYTE *pData, DWORD dwDataSize);
//ftp上传文件
BOOL FTP_Upload(char *szHostName, char *szUserName, char *szPassword,
	char *szUrlPath, char *FilePath);
//在ftp服务器创建目录
BOOL FTP_MKDIR(CString path);
//连接ftp
HINTERNET ConnectFtp();
//将FTP指定文件的目录加载到容器中
vector<CString> ListFtpFiles(HINTERNET hConnect, CString ftppath);


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
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


// CFtpCodeDlg 对话框



CFtpCodeDlg::CFtpCodeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_FTPCODE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFtpCodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Upload_list, m_uplist);
	DDX_Control(pDX, download_list, m_loadlist);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_LIST3, m_list1);
	DDX_Control(pDX, IDC_LIST4, m_list2);
	DDX_Control(pDX, SHOW_LIST, m_show_list);
	DDX_Control(pDX, IDC_PROGRESS1, m_pro);
}

BEGIN_MESSAGE_MAP(CFtpCodeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Choose_file_btn, &CFtpCodeDlg::OnBnClickedfilebtn)
	ON_BN_CLICKED(Ftp_uplown_btn, &CFtpCodeDlg::OnBnClickeduplownbtn)
	ON_BN_CLICKED(Choose_prj_btn, &CFtpCodeDlg::OnBnClickedprjbtn)
	ON_BN_CLICKED(Choose_ini_btn, &CFtpCodeDlg::OnBnClickedinibtn)
	ON_BN_CLICKED(Ftp_download_btn, &CFtpCodeDlg::OnBnClickeddownloadbtn)
	ON_BN_CLICKED(IDC_BUTTON4, &CFtpCodeDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON2, &CFtpCodeDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CFtpCodeDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CFtpCodeDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFtpCodeDlg 消息处理程序

BOOL CFtpCodeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFtpCodeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFtpCodeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFtpCodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//cstring转为char*
char * CStringToChar(CString cstring)
{
	CString str = cstring;
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char *ptxtTemp = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptxtTemp, len, NULL, NULL);

	return ptxtTemp;
}

//清理目录下的所有文件
void ClearDirectory(string dir)
{
	//在目录后面加上"\\*.*"进行第一次搜索
	string newDir = dir + "\\*.*";
	//用于查找的句柄
	intptr_t handle;
	struct _finddata_t fileinfo;
	//第一次查找
	handle = _findfirst(newDir.c_str(), &fileinfo);

	if (handle == -1) {
		//cout << "无文件" << endl;
		system("pause");
		return;
	}

	do
	{
		if (fileinfo.attrib & _A_SUBDIR) {//如果为文件夹，加上文件夹路径，再次遍历
			if (strcmp(fileinfo.name, ".") == 0 || strcmp(fileinfo.name, "..") == 0)
				continue;

			// 在目录后面加上"\\"和搜索到的目录名进行下一次搜索
			newDir = dir + "\\" + fileinfo.name;
			ClearDirectory(newDir.c_str());//先遍历删除文件夹下的文件，再删除空的文件夹
			//cout << newDir.c_str() << endl;
			if (_rmdir(newDir.c_str()) == 0) {//删除空文件夹
				//cout << "delete empty dir success" << endl;
			}
			else {
				//cout << "delete empty dir error" << endl;
			}
		}
		else {
			string file_path = dir + "\\" + fileinfo.name;
			//cout << file_path.c_str() << endl;
			if (remove(file_path.c_str()) == 0) {//删除文件
				//cout << "delete file success" << endl;
			}
			else {
				//cout << "delete file error" << endl;
			}
		}
	} while (!_findnext(handle, &fileinfo));

	_findclose(handle);
	return;
}

CString LocalTime()
{
	SYSTEMTIME time;
	GetLocalTime(&time);

	char buffer[256];
	sprintf_s(buffer, "%04d/%02d/%02d %02d:%02d:%02d\n", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	CString strTime = (CString)buffer;

	return strTime;
}

//连接ftp
//返回值：句柄
HINTERNET ConnectFtp()
{
	DWORD dwBytesReturn = 0;
	DWORD UploadDataSize = 0;
	BYTE *pUploadData = NULL;
	//DWORD dwRet, bRet = 0;

	// 建立会话
	Internet = InternetOpen(_T("WinInet Ftp"), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	// 建立连接
	Connect = InternetConnect(Internet, (CString)ftpip,
		INTERNET_INVALID_PORT_NUMBER, (CString)username, (CString)passwd,
		INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (Connect == NULL)
	{
		return NULL;
	}

	return Connect;
}

//将FTP指定文件的目录加载到容器中
//hConnect：句柄
//ftppath：ftp上的路径。例如：/FW/AOI/0DMH/
//返回值：vector容器，里面放的是cstring
vector<CString> ListFtpFiles(HINTERNET hConnect, CString ftppath)
{
	WIN32_FIND_DATA fd;
	vector<CString> vector;
	HINTERNET hFind = FtpFindFirstFile(hConnect, ftppath, &fd, INTERNET_FLAG_RELOAD, 0);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		BOOL bFind = TRUE;

		while (bFind)
		{
			vector.push_back(fd.cFileName);
			//AfxMessageBox(fd.cFileName);
			bFind = InternetFindNextFile(hFind, &fd);
		}
	}
	InternetCloseHandle(hFind);

	return vector;
}


//在ftp服务器创建目录
BOOL FTP_MKDIR(CString path)
{
	HINTERNET hInternet, hConnect, hFTPFile = NULL;
	DWORD dwBytesReturn = 0;
	DWORD UploadDataSize = 0;
	BYTE *pUploadData = NULL;
	//DWORD dwRet, bRet = 0;

	// 建立会话
	hInternet = InternetOpen(_T("WinInet Ftp"), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	// 建立连接
	hConnect = InternetConnect(hInternet, (CString)ftpip,
		INTERNET_INVALID_PORT_NUMBER, (CString)username, (CString)passwd,
		INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (hConnect == NULL)
	{
		AfxMessageBox(_T("ftp连接失败"));
		return false;
	}

	//创建目录。
	//AfxMessageBox(path);
	FtpCreateDirectoryW(hConnect, path);
	
	InternetCloseHandle(hConnect);
	return TRUE;
}

//ftp上传文件
BOOL FTP_Upload(char *szHostName, char *szUserName, char *szPassword,
	char *szUrlPath, char *FilePath)
{
	HINTERNET hInternet, hConnect, hFTPFile = NULL;
	DWORD dwBytesReturn = 0;
	DWORD UploadDataSize = 0;
	BYTE *pUploadData = NULL;
	DWORD dwRet, bRet = 0;

	// 建立会话
	hInternet = InternetOpen(_T("WinInet Ftp"), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);
	// 建立连接
	hConnect = InternetConnect(hInternet, (CString)szHostName,
		INTERNET_INVALID_PORT_NUMBER, (CString)szUserName, (CString)szPassword,
		INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (hConnect == NULL)
	{
		AfxMessageBox(_T("ftp连接失败"));
		return false;
	}
	// 打开FTP文件, 文件操作和本地操作相似
	hFTPFile = FtpOpenFile(hConnect, (CString)szUrlPath, GENERIC_WRITE,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	if (hFTPFile == NULL)
	{
		AfxMessageBox(_T("ftp文件打开失败"));
		return false;
	}

	// 打开文件
	HANDLE hFile = CreateFile((CString)FilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		AfxMessageBox(_T("文件打开失败"));
		return false;
	}
	
	// 获取文件大小
	UploadDataSize = GetFileSize(hFile, NULL);
	// 读取文件数据
	pUploadData = new BYTE[UploadDataSize];
	ReadFile(hFile, pUploadData, UploadDataSize, &dwRet, NULL);
	UploadDataSize = dwRet;

	// 上传数据
	bRet = InternetWriteFile(hFTPFile, pUploadData, UploadDataSize, &dwBytesReturn);
	if (FALSE == bRet)
	{
		AfxMessageBox(_T("上传数据失败"));
		delete[]pUploadData;
		return FALSE;
	}

	InternetCloseHandle(hFTPFile);

	delete[]pUploadData;
	return TRUE;
}


//ftp保存文件
BOOL Ftp_SaveToFile(char *pszFileName, BYTE *pData, DWORD dwDataSize)
{
	HANDLE hFile = CreateFile((CString)pszFileName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwRet = 0;
	WriteFile(hFile, pData, dwDataSize, &dwRet, NULL);
	CloseHandle(hFile);
	return TRUE;
}

//ftp下载文件
BOOL FTP_Download(char *szHostName, char *szUserName, char *szPassword,
	char *szUrlPath, char *SavePath)
{
	HINTERNET hInternet, hConnect, hFTPFile = NULL;
	BYTE *pDownloadData = NULL;
	DWORD dwDownloadDataSize = 0;
	DWORD dwBufferSize = 4096;
	BYTE *pBuf = NULL;
	DWORD dwBytesReturn = 0;
	DWORD dwOffset = 0;
	BOOL bRet = FALSE;

	// 建立会话
	hInternet = InternetOpen(_T("WinInet Ftp"), INTERNET_OPEN_TYPE_PRECONFIG,
		NULL, NULL, 0);

	// 建立连接
	hConnect = InternetConnect(hInternet, (CString)szHostName,
		INTERNET_INVALID_PORT_NUMBER, (CString)szUserName,
		(CString)szPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);
	if (hConnect == NULL)
	{
		AfxMessageBox(_T("连接ftp失败！"));
		return false;
	}

	// 打开FTP文件, 文件操作和本地操作相似
	hFTPFile = FtpOpenFile(hConnect, (CString)szUrlPath, GENERIC_READ,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD, NULL);
	if (hFTPFile == NULL)
	{
		AfxMessageBox(_T("ftp文件打开失败！"));
		return false;
	}
	// 获取文件大小
	dwDownloadDataSize = FtpGetFileSize(hFTPFile, NULL);
	// 申请动态内存
	pDownloadData = new BYTE[dwDownloadDataSize];

	RtlZeroMemory(pDownloadData, dwDownloadDataSize);
	pBuf = new BYTE[dwBufferSize];
	RtlZeroMemory(pBuf, dwBufferSize);
	// 接收数据
	do
	{
		bRet = InternetReadFile(hFTPFile, pBuf, dwBufferSize, &dwBytesReturn);
		if (FALSE == bRet)
			break;
		RtlCopyMemory((pDownloadData + dwOffset), pBuf, dwBytesReturn);
		dwOffset = dwOffset + dwBytesReturn;
	} while (dwDownloadDataSize > dwOffset);

	// 将数据保存为文件
	Ftp_SaveToFile(SavePath, pDownloadData, dwDownloadDataSize);
	// 释放内存
	delete[]pDownloadData;
	pDownloadData = NULL;
	return TRUE;
}

//获取ftp指定目录下的文件名，存放到vector
vector<CString> getFtpVector(CString str)
{
	vector<CString> vector;
	//连接ftp
	HINTERNET hConnect = ConnectFtp();
	if (hConnect == NULL)
	{
		AfxMessageBox(_T("ftp连接失败"));
	}

	//将ftp上的指定目录的文件名存放到容器中
	vector = ListFtpFiles(hConnect, str);

	return vector;
}


void CFtpCodeDlg::OnBnClickedfilebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_uplist.ResetContent();
	m_list1.ResetContent();

	//CFileDialog dlgFile(TRUE, _T("*"), _T("Please select file"),
		//NULL, _T("ALL Files(*.*)|*.*||Files(*.prj)"), NULL);
	CFileDialog dlgFile(TRUE, _T("PRJ FILE(*.prj) | *.prj"), _T("Please select file"),
		NULL, _T("PRJ FILE(*.prj) | *.prj"), NULL);

	dlgFile.m_ofn.lpstrTitle = _T("上传路径名");//文件打开对话框标题
	if (IDOK == dlgFile.DoModal())//判断是否成功选中文件  
	{
		CString strLocalFilePath_Temp;
		strLocalFilePath_Temp = dlgFile.GetPathName();
		m_list1.AddString(dlgFile.GetFolderPath());
		char chCurdir[256];
		CString strCurDir;
		GetCurrentDirectory(256, (LPTSTR)chCurdir);
		strCurDir.Format(_T("%s"), chCurdir);

		//获取到路径名和文件夹名
		int iFlag1 = 0, iFlag2 = 0, iFlag3 = 0;
		CString strtemp, strtemp2;
		strtemp = strLocalFilePath_Temp;
		iFlag2 = strtemp.GetLength();
		iFlag1 = strtemp.ReverseFind('\\');
		strtemp2 = strtemp.Left(iFlag1);
		up_local_path.Format(_T("%s"), strtemp2);
		SetDlgItemText(Upload_list, up_local_path);
		//m_uplist.AddString(up_local_path);

		//显示文件夹内容
		HANDLE hFile;
		CString str1;
		CString str2;
		WIN32_FIND_DATA fileinfo;
		DWORD errorcode = 0;
		str1.Format(_T("%s\\*.*"), up_local_path);
		hFile = FindFirstFile(str1, &fileinfo);
		while (hFile != INVALID_HANDLE_VALUE && errorcode != ERROR_NO_MORE_FILES)
		{
			str2 = fileinfo.cFileName;
			FindNextFile(hFile, &fileinfo);
			errorcode = GetLastError();
			if ((str2 == ".") || (str2 == ".."))
			{
				continue;
			}
			if (str2.Right(3) == "prj")
			{
				m_uplist.AddString(str2);
			}
		}
		FindClose(hFile);
	}
	return;
	
}

void CFtpCodeDlg::OnBnClickeduplownbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = m_uplist.GetSelCount();

	//没有选择文件，弹框报错
	if (nCount < 1)
	{
		AfxMessageBox(_T("请选择文件"));
		return;
	}

	//获取到文件名,将路径名+文件名。拼接成完整的路径，用于ftp上传
	int* SelIndex = new int[nCount];
	CString up_ftp_path;
	vector<CString> v_path;//用于ftp服务器上的路径。例如：/FW/AOI/0DMH
	vector<CString> v_up_path;//用于存放要上传文件的全路径名：例如：C:\sercomm\barcode\0DMH0500W65_1.0.prj
	vector<CString> v_ftp_path;//用于ftp上传文件的全路径名。例如：/FW/AOI/0DMH/0DMH0500W65_1.0.prj
	vector<CString> v_temp;//ftp服务器上所有的文件名

	CString str;//从listbox读取到的文件名
	m_uplist.GetSelItems(nCount, SelIndex);//selindex数组里面就是行的索引
	
	//清空容器
	v_up_path.clear();
	v_ftp_path.clear();
	v_temp.clear();

	for (int i = 0; i < nCount; i++)
	{
		m_uplist.GetText(SelIndex[i], str);

		//拼接要上传ftp的路径名
		char *ftp = "/FW/AOI/";
		CString path = (CString)ftp + str.Left(4);//拼接成：/FW/AOI/0DMH

		//获取ftp服务器目录下的文件名
		vector<CString> temp= getFtpVector(path);
		v_temp.insert(v_temp.end(), temp.begin(), temp.end());

		up_ftp_path = path + '/' + str;//拼接成：/FW/AOI/0DMH/0DMH0500W65_1.0.prj

		//拼接本地要上传ftp文件的绝对路径名
		str = up_local_path + '\\' + str;
		//AfxMessageBox(str);
		v_path.push_back(path);
		v_ftp_path.push_back(up_ftp_path);
		v_up_path.push_back(str);

		//AfxMessageBox((CString)str);
		//AfxMessageBox((CString)up_ftp_path);
	}

	//进度条
	m_pro.SetRange(0, nCount);
	m_pro.SetStep(1);
	m_pro.SetPos(0);

	bool update = true;//判断是否更新文件,false不更新，true更新
	for (int i = 0; i < nCount; i++)
	{
		//cstring转为char*
		char *ftp_path = CStringToChar(v_ftp_path[i]);
		char *local_path = CStringToChar(v_up_path[i]);

		m_uplist.GetText(SelIndex[i], str);

		//判断文件名中释放包含空格
		for (int s = 0;s < str.GetLength();s++)
		{
			if (str[s] == ' ')
			{
				AfxMessageBox(str + _T("文件名中包含空格，请修改！"));
				return;
			}
		}

		update = true;//默认更新
		//判断文件是否存在。若存在，弹框询问是否覆盖；若不存在，直接上传
		for (int j = 0;j < v_temp.size();j++)
		{
			//上传同名文件
			if (str == v_temp[j])
			{
				//弹框询问是否覆盖
				CString show = _T("文件") + str + _T("已经存在，是否覆盖？");
				int result = AfxMessageBox(show, MB_YESNO);

				//覆盖掉ftp服务器的文件
				if (result == IDYES)
				{
					update = true;
				}
				else if(result == IDNO)//不覆盖
				{
					update = false;
					return;
				}
				else
				{
					update = false;
				}
				break;
			}
		}
		//AfxMessageBox((CString)ftp_path);
		//AfxMessageBox((CString)local_path);
	
		
		//将文件上传到ftp服务器
		if (update == true)
		{
			if (v_up_path[i].Right(3) == "prj")
			{
				//AfxMessageBox((CString)ftp_path);
				//AfxMessageBox((CString)local_path);
				//创建目录
				FTP_MKDIR(v_path[i]);

				BOOL bRET = FTP_Upload(ftpip, username, passwd, ftp_path, local_path);
				if (bRET == FALSE)
				{
					//AfxMessageBox((CString)"ftp上传失败");
					//在m_show_list显示出来
					CString strTime = LocalTime();
					m_show_list.AddString(strTime + ":" + v_up_path[i] + _T("ftp上传失败"));
					return;
				}
				//CFtpCodeDlg dlg;
				
				m_pro.StepIt();

				//在m_show_list显示出来
				CString strTime = LocalTime();
				m_show_list.AddString(strTime + ":"+ v_up_path[i] + _T("上传ftp成功！"));
				
				//AfxMessageBox(v_up_path[i] + "上传成功");
				delete ftp_path;
				delete local_path;
			}
			else
			{
				//AfxMessageBox(v_up_path[i]);
				AfxMessageBox(_T("请选择prj格式的文件！"));
				return;
			}
		}

	}
	
	//AfxMessageBox((CString)"上传ftp成功！");
	delete[] SelIndex;
}


void CFtpCodeDlg::OnBnClickedprjbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_loadlist.ResetContent();//清空listbox
	m_list2.ResetContent();

	//获取编辑框中的内容
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->GetWindowText(edit_str);

	//编辑框中为空，提示
	if (edit_str.IsEmpty())
	{
		AfxMessageBox(_T("请输入料号！"));
		return;
	}
	
	//AfxMessageBox(edit_str);
	//连接ftp
	HINTERNET hConnect = ConnectFtp();
	if (hConnect == NULL)
	{
		AfxMessageBox(_T("ftp连接失败"));
		return;
	}

	//将ftp上的指定目录的文件名存放到容器中
	CString path = (CString)"/FW/AOI/" + edit_str.Left(4);//拼接路径。例如：/FW/AOI/0DMH/
	m_list2.AddString(path);
	v_ftp_prj_name = ListFtpFiles(hConnect, path);

	//将文件名添加到listbox中
	for (int i = 0; i < v_ftp_prj_name.size(); i++)
	{
		if (v_ftp_prj_name[i].Right(3) == "prj")
		{
			int ilen = edit_str.GetLength();
			char *str1 = CStringToChar(v_ftp_prj_name[i]);
			char *str2 = CStringToChar(edit_str);
			if (strncmp(str1, str2, ilen) == 0)//查找到
			{
				m_loadlist.AddString(v_ftp_prj_name[i]);
			}
		}
	}
}


void CFtpCodeDlg::OnBnClickedinibtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_loadlist.ResetContent();//清空listbox
	m_list2.ResetContent();

	//获取编辑框中的内容
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	pEdit->GetWindowText(edit_str);

	//编辑框中为空，提示
	if (edit_str.IsEmpty())
	{
		AfxMessageBox(_T("请输入料号！"));
		return;
	}

	//连接ftp
	HINTERNET hConnect = ConnectFtp();
	if (hConnect == NULL)
	{
		AfxMessageBox(_T("ftp连接失败"));
		return;
	}

	//将ftp上的指定目录的文件名存放到容器中
	CString path = (CString)"/BARCODE/" + edit_str.Left(4);//拼接路径。例如：/BARCODE/0AKE/
	m_list2.AddString(path);
	v_ftp_ini_name = ListFtpFiles(hConnect, path);

	for (int i = 0; i < v_ftp_ini_name.size(); i++)
	{
		if (v_ftp_ini_name[i].Right(3) == "ini")
		{
			int ilen = edit_str.GetLength();
			char *str1 = CStringToChar(v_ftp_ini_name[i]);
			char *str2 = CStringToChar(edit_str);
			if (strncmp(str1, str2, ilen) == 0)//查找到
			{
				m_loadlist.AddString(v_ftp_ini_name[i]);
			}
		}
	}
}


void CFtpCodeDlg::OnBnClickeddownloadbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = m_loadlist.GetSelCount();
	//没有选择文件，弹框报错
	if (nCount < 1)
	{
		AfxMessageBox(_T("请选择文件"));
		return;
	}

	//拼接ftp上全路径名
	int* SelIndex = new int[nCount];
	CString str;//从listbox读取到的文件名
	CString local_path;//本地文件路径
	vector<CString> v_ftp_path_name;//要下载ftp服务器上的ini全路径名
	vector<CString> v_local_path_name;//存放在本地的文件全路径名
	m_loadlist.GetSelItems(nCount, SelIndex);//selindex数组里面就是行的索引
	for (int i = 0; i < nCount; i++)
	{
		m_loadlist.GetText(SelIndex[i], str);
		//AfxMessageBox((CString)str);

		//拼接要下载到本地的全路径名
		//拼接全路径。例如：C:\\sercomm\\project\\0DMH\\0DMH0500W65_test2.prj
		local_path = (CString)"C:\\sercomm\\project\\" + str.Left(4) + (CString)"\\" + str;
		v_local_path_name.push_back(local_path);

		//拼接ftp服务器上的文件名。如果选中的是prj文件，拼接指定的文件名。
		//例如：/FW/AOI/0DMH/******.prj
		if (str.Right(3) == "prj")
		{
			//拼接路径名。例如：/FW/AOI/0DMH/0DMH0500W65_test2.prj
			str = (CString)"/FW/AOI/" + str.Left(4) + (CString)"/" + str; 
			v_ftp_path_name.push_back(str);
		}

		//拼接ftp服务器上的文件名。如果选中的是ini文件，拼接指定的文件名。
		//例如：/BARCODE/0AKE/******.prj
		else if (str.Right(3) == "ini")
		{
			//拼接路径名。例如：/BARCODE/0AKE/0AKE0500PK_1.0_R.TEST.BARCODE.ini
			str = (CString)"/BARCODE/" + str.Left(4) + (CString)"/" + str;
			v_ftp_path_name.push_back(str);
		}
		else
		{
			AfxMessageBox(_T("请选择格式为：.prj或.ini文件！"));
			return;
		}

		//AfxMessageBox(v_ftp_path_name[i]);
		//AfxMessageBox(v_local_path_name[i]);
	}

	//创建目录，确保目录存在
	CString lc_path = (CString)"C:\\sercomm\\project\\" + edit_str.Left(4);
	CreateDirectory(_T("C:\\sercomm"), NULL);
	CreateDirectory(_T("C:\\sercomm\\project"), NULL);
	CreateDirectory(lc_path , NULL);//例如：C:\\sercomm\\project\\0DMH\\

	//清理目录下的所有文件
	string sz2 = CT2A(lc_path.GetBuffer());
	ClearDirectory(sz2);

	//进度条
	m_pro.SetRange(0, nCount);
	m_pro.SetStep(1);
	m_pro.SetPos(0);

	//把ftp服务器上的文件下载到本地，其中ftp上的路径名在v_ftp_path_name容器中
	for (int i = 0; i < nCount; i++)
	{
		//cstring转为char*
		char *ftp_path = CStringToChar(v_ftp_path_name[i]);
		char *local_path = CStringToChar(v_local_path_name[i]);
		//AfxMessageBox((CString)ftp_path);

		//下载ftp上的文件
		BOOL bRET = FTP_Download(ftpip, username, passwd, ftp_path, local_path);
		if (bRET == FALSE)
		{
			//AfxMessageBox((CString)"下载失败");
			//在m_show_list显示出来
			CString strTime = LocalTime();
			m_show_list.AddString(strTime + ":" + ftp_path + _T("下载失败"));
			return;
		}

		m_pro.StepIt();

		//AfxMessageBox((CString)"ftp下载成功！");
		//在m_show_list显示出来
		CString strTime = LocalTime();
		m_show_list.AddString(strTime + ":" + ftp_path + _T("ftp下载成功！"));
		delete ftp_path;
		delete local_path;
	}

	delete[] SelIndex;
}

//全选m_uplist
void CFtpCodeDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = m_uplist.GetCount();
	//没有选择文件，弹框报错
	if (nCount < 1)
	{
		AfxMessageBox(_T("列表框为空！"));
		return;
	}
	m_uplist.SetSel(-1, true);//全选
	
}

//清空m_uplist
void CFtpCodeDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_uplist.ResetContent();//清空
	m_uplist.SetSel(-1, FALSE);//全不选
}

//全选m_loadlist
void CFtpCodeDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	int nCount = m_loadlist.GetCount();
	//没有选择文件，弹框报错
	if (nCount < 1)
	{
		AfxMessageBox(_T("列表框为空！"));
		return;
	}
	m_loadlist.SetSel(-1, true);//全选

}

//清空m_loadlist
void CFtpCodeDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	//m_loadlist.ResetContent();//清空
	m_loadlist.SetSel(-1, FALSE);//全不选
}




BOOL CFtpCodeDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
	{
		return TRUE;
	}
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE))
	{
		return TRUE;
	}
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_TAB))
	{
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}
