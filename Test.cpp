
// LargeDataGroupingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LargeDataGrouping.h"
#include "LargeDataGroupingDlg.h"
#include "afxdialogex.h"
#include<direct.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLargeDataGroupingDlg dialog




CLargeDataGroupingDlg::CLargeDataGroupingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLargeDataGroupingDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDataset = new double*[NUM_ROWS];
	for (int i = 0; i < NUM_ROWS; i++)
	{
		m_pDataset[i] = new double[NUM_COLS];
	}
	//Init data
	for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLS; j++)
		{
			m_pDataset[i][j] =0;
		}
	}
	m_pDataset[100][100] = 5;
	m_pDataset[100][101] = 5;
	m_pDataset[101][101] = 5;
	m_pDataset[102][99] = 5;
	m_pDataset[102][100] = 5;
	m_pDataset[103][100] = 5;
	m_pDataset[104][100] = 5;
	m_pDataset[105][100] = 5;
	m_pDataset[106][100] = 5;
	m_pDataset[107][100] = 5;
	m_pDataset[108][100] = 5;
	m_pDataset[109][100] = 5;
	m_pDataset[110][100] = 5;
	m_pDataset[110][101] = 5;

	m_pDataset[100][105] = 5;
	m_pDataset[100][106] = 5;
	m_pDataset[101][106] = 5;
	m_pDataset[102][104] = 5;
	m_pDataset[102][105] = 5;
	m_pDataset[103][105] = 5;
	m_pDataset[104][105] = 5;
}

CLargeDataGroupingDlg::~CLargeDataGroupingDlg()
{
	for (int i = 0; i < NUM_ROWS; i++)
	{
		delete[] m_pDataset[i];
	}
	delete[] m_pDataset;
	m_pDataset = NULL;
}

void CLargeDataGroupingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLargeDataGroupingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDC_BUTTON_PROCESS, OnProcessBtn)
END_MESSAGE_MAP()


// CLargeDataGroupingDlg message handlers

BOOL CLargeDataGroupingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLargeDataGroupingDlg::OnPaint()
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
HCURSOR CLargeDataGroupingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CLargeDataGroupingDlg::OnProcessBtn()
{
	CFile file;
	BOOL bResult;
	CString szFileName = "D:\\abc\\aa";
	CFileFind finder;
	bResult = finder.FindFile(szFileName);
	if (!bResult)
	{
		bResult = _mkdir(szFileName);
	}
	bResult = finder.IsDirectory();
	bResult = file.Open("D:\\abc\\t.txt", CFile::modeCreate | CFile::modeWrite);
	file.Close();
	/*for (int i = 0; i < NUM_ROWS; i++)
	{
		for (int j = 0; j < NUM_COLS; j++)
		{
			if (IsDataValid(m_pDataset[i][j]) && !IsDataItemChecked(i, j))
			{
				GroupData(i, j);
			}
		}
	}
	CString szMsg;
	szMsg.Format("%d groups", m_listGroups.GetCount());
	POSITION pos = m_listGroups.GetHeadPosition();
	for (int i = 0; i < m_listGroups.GetCount(); i++)
	{
		CGroupList* pList = m_listGroups.GetNext(pos);
		szMsg.Format("%s, %d", szMsg, pList->GetCount());
	}
	MessageBox(szMsg);*/
}

BOOL CLargeDataGroupingDlg::IsDataValid(double& value)
{
	if (value > 0)
		return TRUE;
	else
		return FALSE;
}

BOOL CLargeDataGroupingDlg::IsDataItemChecked(int& x, int &y)
{
	POSITION pos = m_listGroups.GetHeadPosition();
	for (int i = 0; i < m_listGroups.GetCount(); i++)
	{
		CGroupList* pList = m_listGroups.GetNext(pos);
		if (pList->Find(&m_pDataset[x][y]))
			return TRUE;
	}
	return FALSE;
}

VOID CLargeDataGroupingDlg::GroupData(int& x, int &y)
{
	CGroupList* pList = new CGroupList;
	m_listGroups.AddTail(pList);
	CGroupList& rList = *pList;
	rList.AddTail(&m_pDataset[x][y]);
	FindSameGroupElement(x, y, rList);
}

VOID CLargeDataGroupingDlg::FindSameGroupElement(int x, int y, CGroupList& rList)
{
	//left down
	if (IsDataValid(m_pDataset[x+1][y-1]) && !rList.Find(&m_pDataset[x+1][y-1]))
	{
		rList.AddTail(&m_pDataset[x+1][y-1]);
		FindSameGroupElement(x+1, y-1,rList);
	}
	//down
	if (IsDataValid(m_pDataset[x+1][y]) && !rList.Find(&m_pDataset[x+1][y]))
	{
		rList.AddTail(&m_pDataset[x+1][y]);
		FindSameGroupElement(x+1, y,rList);
	}
	// right down
	if (IsDataValid(m_pDataset[x+1][y+1]) && !rList.Find(&m_pDataset[x+1][y+1]))
	{
		rList.AddTail(&m_pDataset[x+1][y+1]);
		FindSameGroupElement(x+1, y+1,rList);
	}
	//right
	if (IsDataValid(m_pDataset[x][y+1]) && !rList.Find(&m_pDataset[x][y+1]))
	{
		rList.AddTail(&m_pDataset[x][y+1]);
		FindSameGroupElement(x, y+1,rList);
	}
	//left extra
	if (IsDataValid(m_pDataset[x][y-1]) && !rList.Find(&m_pDataset[x][y-1]))
	{
		rList.AddTail(&m_pDataset[x][y-1]);
		FindSameGroupElement(x, y-1,rList);
	}
}
