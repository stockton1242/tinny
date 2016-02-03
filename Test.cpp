
// GroupProcessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GroupProcess.h"
#include "GroupProcessDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


bool sortPositionX (Position* i,Position* j) { return ((i->x)<(j->x)); }
bool sortPositionY (Position* i,Position* j) { return ((i->y)<(j->y)); }

// CGroupProcessDlg dialog




CGroupProcessDlg::CGroupProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGroupProcessDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pDataSet = new double*[ROW_NUM];
	for (int i = 0; i < ROW_NUM; i++)
	{
		m_pDataSet[i] = new double[COL_NUM];
	}
	m_szImportFilePath = "";
	m_szExportFilePath = "";
	m_nDataRows = 0;
	m_nDataColumns = 0;
	m_dMinDataValue = 0;
	m_nMinPopulationValue = 20;
	m_nGroupIndex = 0;
	m_nBlockRows = 10;
	m_nBlockColumns = 31;
	m_nTotalX = 0;
	m_nTotalY = 0;
	m_bRememberSettings = FALSE;
	m_bDefaultOutPath = FALSE;
	m_bOpenFileAfterDone = FALSE;
	m_bBatch = FALSE;
}

CGroupProcessDlg::~CGroupProcessDlg()
{
	if (m_pDataSet)
	{
		for (int i = 0; i < ROW_NUM; i++)
		{
			delete[] m_pDataSet[i];
		}
		delete[] m_pDataSet;
		m_pDataSet = NULL;
	}
	POSITION pos = m_listGroups.GetHeadPosition();
	for (int i = 0; i < m_listGroups.GetCount(); i++)
	{
		CGroupList* pList = m_listGroups.GetNext(pos);
		delete pList;
	}
	m_listGroups.RemoveAll();

	std::vector<Position*>::iterator iter;
	for (iter = m_vecPositions.begin(); iter != m_vecPositions.end(); iter++)
	{
		delete *iter;
	}
	m_vecPositions.clear();
}

void CGroupProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_IMPORT_PATH, m_szImportFilePath);
	DDX_Text(pDX, IDC_EDIT_EXPORT_PATH, m_szExportFilePath);
	DDX_Text(pDX, IDC_EDIT_MIN_VAL, m_dMinDataValue);
	DDX_Text(pDX, IDC_EDIT_MIN_POPULATION, m_nMinPopulationValue);
	DDX_Text(pDX, IDC_EDIT_ROWS, m_nBlockRows);
	DDX_Text(pDX, IDC_EDIT_COLUMNS, m_nBlockColumns);
	DDX_Check(pDX, IDC_CHECK_REMEMBER_SETTINGS, m_bRememberSettings);
	DDX_Check(pDX, IDC_CHECK_DEFAULT_DES, m_bDefaultOutPath);
	DDX_Check(pDX, IDC_CHECK_OPENFILE_AFTER_DONE, m_bOpenFileAfterDone);
	DDX_Check(pDX, IDC_CHECK_BATCH, m_bBatch);

	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CGroupProcessDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_IMPORT, &CGroupProcessDlg::OnBnClickedButtonImport)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CGroupProcessDlg::OnBnClickedButtonExport)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CGroupProcessDlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_CHECK_DEFAULT_DES, &CGroupProcessDlg::OnBnClickedCheckDefaultDes)
	ON_EN_CHANGE(IDC_EDIT_IMPORT_PATH, &CGroupProcessDlg::OnEnChangeEditImportPath)
	ON_BN_CLICKED(IDC_CHECK_REMEMBER_SETTINGS, &CGroupProcessDlg::OnBnClickedCheckRememberSettings)
	ON_BN_CLICKED(IDC_CHECK_OPENFILE_AFTER_DONE, &CGroupProcessDlg::OnBnClickedCheckOpenfileAfterDone)
	ON_EN_CHANGE(IDC_EDIT_MIN_VAL, &CGroupProcessDlg::OnEnChangeEditMinVal)
	ON_EN_CHANGE(IDC_EDIT_MIN_POPULATION, &CGroupProcessDlg::OnEnChangeEditMinPopulation)
	ON_EN_CHANGE(IDC_EDIT_ROWS, &CGroupProcessDlg::OnEnChangeEditRows)
	ON_EN_CHANGE(IDC_EDIT_COLUMNS, &CGroupProcessDlg::OnEnChangeEditColumns)
	ON_BN_CLICKED(IDC_CHECK_BATCH, &CGroupProcessDlg::OnBnClickedCheckBatch)
END_MESSAGE_MAP()


BOOL CGroupProcessDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) { return TRUE; }

	return CDialog::PreTranslateMessage(pMsg);
}

// CGroupProcessDlg message handlers

BOOL CGroupProcessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_ctrlProgress.SetRange(0, 100);
	//for remember settings
	if ((m_bRememberSettings = AfxGetApp()->GetProfileInt("GroupProcess", "RemeberSettings", -1)) == -1)
	{
		m_bRememberSettings = FALSE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "RemeberSettings",m_bRememberSettings);
	}

	if (m_bRememberSettings)
	{
		CString szTemp;
		if ((szTemp = AfxGetApp()->GetProfileString("GroupProcess", "Criteria1", "")) == "")
		{
			szTemp.Format("%f",m_dMinDataValue);
			AfxGetApp()->WriteProfileString("GroupProcess", "Criteria1",szTemp);
		}
		else
		{
			m_dMinDataValue = atof(szTemp);
		}

		if ((m_nMinPopulationValue = AfxGetApp()->GetProfileInt("GroupProcess", "Criteria2", -1)) == -1)
		{
			m_nMinPopulationValue = 20;
			AfxGetApp()->WriteProfileInt("GroupProcess", "Criteria2",m_nMinPopulationValue);
		}

		if ((m_nBlockRows = AfxGetApp()->GetProfileInt("GroupProcess", "BlockRows", -1)) == -1)
		{
			m_nBlockRows = 10;
			AfxGetApp()->WriteProfileInt("GroupProcess", "BlockRows",m_nBlockRows);
		}

		if ((m_nBlockColumns = AfxGetApp()->GetProfileInt("GroupProcess", "BlockColumns", -1)) == -1)
		{
			m_nBlockColumns = 21;
			AfxGetApp()->WriteProfileInt("GroupProcess", "BlockColumns",m_nBlockColumns);
		}

		if ((m_bDefaultOutPath = AfxGetApp()->GetProfileInt("GroupProcess", "DefaultOutPath", -1)) == -1)
		{
			m_bDefaultOutPath = FALSE;
			AfxGetApp()->WriteProfileInt("GroupProcess", "DefaultOutPath",m_bDefaultOutPath);
		}

		if ((m_bOpenFileAfterDone = AfxGetApp()->GetProfileInt("GroupProcess", "OpenFile", -1)) == -1)
		{
			m_bOpenFileAfterDone = FALSE;
			AfxGetApp()->WriteProfileInt("GroupProcess", "OpenFile",m_bOpenFileAfterDone);
		}

		if ((m_bBatch = AfxGetApp()->GetProfileInt("GroupProcess", "Batch", -1)) == -1)
		{
			m_bBatch = FALSE;
			AfxGetApp()->WriteProfileInt("GroupProcess", "Batch",m_bBatch);
		}
	}
	UpdateData(FALSE);
	OnBnClickedCheckDefaultDes();
	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGroupProcessDlg::OnPaint()
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
HCURSOR CGroupProcessDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGroupProcessDlg::OnBnClickedButtonImport()
{
	// TODO: Add your control notification handler code here

	if (m_bBatch)
	{
		BROWSEINFO   bi; 
		ZeroMemory(&bi,   sizeof(bi)); 
		TCHAR   szDisplayName[MAX_PATH]; 
		szDisplayName[0]    =   0;  

		bi.hwndOwner        =   NULL; 
		bi.pidlRoot         =   NULL; 
		bi.pszDisplayName   =   szDisplayName; 
		bi.lpszTitle        =   _T("Please select a folder:"); 
		bi.ulFlags          =   BIF_RETURNONLYFSDIRS;
		bi.lParam           =   NULL; 
		bi.iImage           =   0;  

		LPITEMIDLIST   pidl   =   SHBrowseForFolder(&bi);
		TCHAR   szPathName[MAX_PATH]; 
		if   (NULL   !=   pidl)
		{
			 BOOL bRet = SHGetPathFromIDList(pidl,szPathName);
			 if(FALSE == bRet)
				  return;
			 m_szImportFilePath = szPathName;
		}
	}
	else
	{
		CFileDialog dlgFile(
			TRUE,
			NULL,
			NULL,
			OFN_HIDEREADONLY,
			_T("Data File (*.txt)|*.txt|All Files (*.*)|*.*||"),
			NULL);
		dlgFile.m_ofn.lpstrInitialDir = ".\\";
		if (dlgFile.DoModal() == IDOK)
		{
			m_szImportFilePath	= dlgFile.GetPathName();
		}
	}

	UpdateData(FALSE);
	OnEnChangeEditImportPath();
}


void CGroupProcessDlg::OnBnClickedButtonExport()
{
	// TODO: Add your control notification handler code here
	//CFileFind finder;
	//finder.FindFile(".");
	//finder.FindNextFile();
	if (m_bBatch)
	{
		BROWSEINFO   bi; 
		ZeroMemory(&bi,   sizeof(bi)); 
		TCHAR   szDisplayName[MAX_PATH]; 
		szDisplayName[0]    =   0;  

		bi.hwndOwner        =   NULL; 
		bi.pidlRoot         =   NULL; 
		bi.pszDisplayName   =   szDisplayName; 
		bi.lpszTitle        =   _T("Please select a folder:"); 
		bi.ulFlags          =   BIF_RETURNONLYFSDIRS;
		bi.lParam           =   NULL; 
		bi.iImage           =   0;  

		LPITEMIDLIST   pidl   =   SHBrowseForFolder(&bi);
		TCHAR   szPathName[MAX_PATH]; 
		if   (NULL   !=   pidl)
		{
			 BOOL bRet = SHGetPathFromIDList(pidl,szPathName);
			 if(FALSE == bRet)
				  return;
			 m_szExportFilePath = szPathName;
		}
	}
	else
	{
		CFileDialog dlgFile(
			TRUE,
			NULL,
			NULL,
			OFN_HIDEREADONLY,
			_T("Data File (*.txt)|*.txt|All Files (*.*)|*.*||"),
			NULL);
		dlgFile.m_ofn.lpstrInitialDir = ".\\";
		if (dlgFile.DoModal() == IDOK)
		{
			m_szExportFilePath	= dlgFile.GetPathName();//.GetFileName();
		}
	}

	UpdateData(FALSE);
}


void CGroupProcessDlg::OnBnClickedButtonProcess()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(FALSE);
	BOOL bAllSuccess = TRUE;
	if (m_bBatch)
	{
		UpdateData(TRUE);
		CFileFind finder;
		CString szTempPath = m_szImportFilePath;
		if (szTempPath[szTempPath.GetLength()-1] == '\\')
		{
			szTempPath = szTempPath.Left(szTempPath.GetLength()-1);
		}
		if(finder.FindFile(szTempPath))
		{
			szTempPath.Format(_T("%s\\*.txt"), szTempPath);
		}
		else
		{
			MessageBox("Load Import File Folder FAIL!");
			return;
		}
		if (!finder.FindFile(m_szExportFilePath))
		{
			if (!CreateFileDirectory(m_szExportFilePath))
			{
				MessageBox("Create Export Directory FAIL!");
				return;
			}
		}
		INT nResult = -1;
		if ((nResult = finder.FindFile(szTempPath)) != 0)
		{
			CFile file;
			CString szResultFile, szContent;
			szResultFile.Format("%s\\Log.txt", m_szExportFilePath);
			file.Open(szResultFile, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate);
			while (nResult)
			{
				nResult = finder.FindNextFile();
				CString szProcessFileName = finder.GetFilePath();
				if (ProcessFile(szProcessFileName))
				{
					szContent.Format("%s process successfully\r\n", szProcessFileName);
					file.Write(szContent, szContent.GetLength());
				}
				else
				{
					bAllSuccess = FALSE;
					szContent.Format("%s process FAILED!\r\n", szProcessFileName);
					file.Write(szContent, szContent.GetLength());
					continue;
				}
				
				CString szSaveFileName = finder.GetFileName();
				INT nIndex = szSaveFileName.ReverseFind(_T('.'));
				szSaveFileName.Insert(nIndex, _T("_GrpnPrcs"));
				CString szSavePath;
				szSavePath.Format("%s\\%s", m_szExportFilePath, szSaveFileName);
				SaveStatisticsIntoFile(szSavePath);
			}
			file.Close();
		}
	}
	else
	{
		if (!ProcessFile(m_szImportFilePath))
		{
			m_ctrlProgress.SetPos(0);
			GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
			return;
		}
		if (!SaveStatisticsIntoFile(m_szExportFilePath))
		{
			m_ctrlProgress.SetPos(0);
			GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
			return;
		}
	}

	m_ctrlProgress.SetPos(100);

	if (m_bOpenFileAfterDone)
	{
		if (!m_bBatch)
		{
			CString szCmd = "notepad " + m_szExportFilePath;
			STARTUPINFO			startupInfo;
			PROCESS_INFORMATION	processInfo;
			memset(&startupInfo, 0, sizeof(STARTUPINFO));
			startupInfo.cb		= sizeof(STARTUPINFO);
			CreateProcess(NULL, szCmd.GetBuffer(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS,
				NULL, NULL, &startupInfo, &processInfo);
			WaitForSingleObject( processInfo.hProcess, INFINITE );
		}
		else
		{
			ShellExecute(NULL, NULL, m_szExportFilePath, NULL, NULL, SW_SHOWNORMAL);
		}
	}
	else
	{
		CString szTemp;
		if (m_bBatch)
		{
			if (bAllSuccess)
				szTemp.Format("All files are processed successfully");
			else
				szTemp.Format("Finished. Some of the files are not properly processed");
		}
		else
		{
			szTemp.Format("Success! Total %d Groups.", m_listGroups.GetCount());
		}
		MessageBox(szTemp);
	}
	m_ctrlProgress.SetPos(0);
	GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
}

BOOL CGroupProcessDlg::ProcessFile(CString szPath)
{
	if (!LoadImportFile(szPath))
	{
		GetDlgItem(IDC_BUTTON_PROCESS)->EnableWindow(TRUE);
		return FALSE;
	}
	for (int i = 0; i < m_nDataRows; i++)
	{
		for (int j = 0; j < m_nDataColumns; j++)
		{
			if (IsDataValid(i, j) && !IsDataItemChecked(i, j))
			{
				GroupData(i, j);

				//m_ctrlProgress.SetPos((i*m_nDataRows+j)/(m_nDataRows*m_nDataColumns));

			}
		}
		m_ctrlProgress.SetPos(i*100/m_nDataRows-1);
		OnPaint();
	}

	if (m_listGroups.GetCount() == m_nBlockRows*m_nBlockColumns)
	{
		std::sort(m_vecPositions.begin(), m_vecPositions.end(), sortPositionX);
		for (int i =0; i < m_nBlockRows; i++)
		{
			std::sort(m_vecPositions.begin()+i*m_nBlockColumns,
						m_vecPositions.begin()+(i+1)*m_nBlockColumns,
						sortPositionY);
		}
	}
	else
	{
		CString szTemp;
		szTemp.Format("DISCREPANCY! Total %d Groups. %s", m_listGroups.GetCount(), szPath);
		MessageBox(szTemp);
		m_ctrlProgress.SetPos(0);
		return FALSE;
	}

	CalculateStatistics();
	m_ctrlProgress.SetPos(95);
	OnPaint();
	
	return TRUE;
}

BOOL CGroupProcessDlg::LoadImportFile(CString szPath)
{
	UpdateData(TRUE);
	CFileFind finder;
	if(finder.FindFile(szPath))
	{
		finder.FindNextFile();
		ResetDataSet();
		ReadDataFromFile(szPath);

		return TRUE;
	}
	MessageBox("Load Import File FAIL!");
	return FALSE;
}

VOID CGroupProcessDlg::ResetDataSet()
{
	for (int i = 0; i < ROW_NUM; i++)
	{
		for (int j = 0; j < COL_NUM; j++)
		{
			m_pDataSet[i][j] = -1.0;
		}
	}
	m_nDataRows = 0;
	m_nDataColumns = 0;
	m_nGroupIndex = 0;
	m_listChecked.clear();
	POSITION pos = m_listGroups.GetHeadPosition();
	for (int i = 0; i < m_listGroups.GetCount(); i++)
	{
		CGroupList* pList = m_listGroups.GetNext(pos);
		delete pList;
	}
	m_listGroups.RemoveAll();
	m_mapRow.clear();
	m_mapColumn.clear();
	std::vector<Position*>::iterator iter;
	for (iter = m_vecPositions.begin(); iter != m_vecPositions.end(); iter++)
	{
		delete *iter;
	}
	m_vecPositions.clear();
	m_listSum.clear();
	m_listAve.clear();
	m_listStdev.clear();
	m_listMin.clear();
	m_listMax.clear();
	m_listPopulation.clear();
}

VOID CGroupProcessDlg::ReadDataFromFile(CString& szFileName)
{
	CStdioFile file;
	CString szNumber;
	BOOL openstatus = file.Open(szFileName, CFile::modeRead | CFile::shareDenyNone);

	char c, cLastChar = '\0';
	for (int i = 0; i< 2; i++) //skip first 2 rows
	{
		while (TRUE)
		{
			file.Read(&c, sizeof(char));
			if (c == '\r')
				break;
		}
	}

	int i = 0, j = 0;
	while (file.Read(&c, sizeof(char)))
	{
		if (c == '\r')
		{
			m_pDataSet[i][j] = atof(szNumber);
			i++;
			j = 0;
			szNumber = "";
		}
		else if (c == '\t')
		{
			m_pDataSet[i][j] = atof(szNumber);
			j++;
			szNumber = "";
		}
		szNumber.Format("%s%c",szNumber, c);
	}
	for (i = 0; i < ROW_NUM; i++)
	{
		if (m_pDataSet[i][0] == -1.0)
		{
			m_nDataRows = i;
		}
	}
	for (i = 0; i < COL_NUM; i++)
	{
		if (m_pDataSet[0][i] == -1.0)
		{
			m_nDataColumns = i;
		}
	}
	file.Close();
}

BOOL CGroupProcessDlg::IsDataValid(double& value)
{
	if (value > m_dMinDataValue)
		return TRUE;
	else
		return FALSE;
}

BOOL CGroupProcessDlg::IsDataValid(int& i, int& j)
{
	if (m_pDataSet[i][j] > m_dMinDataValue)
		return TRUE;
	else
		return FALSE;
}

BOOL CGroupProcessDlg::IsDataItemChecked(int& x, int& y)
{
	/*POSITION pos = m_listGroups.GetHeadPosition();
	for (int i = 0; i < m_listGroups.GetCount(); i++)
	{
		CGroupList* pList = m_listGroups.GetNext(pos);
		if (pList->Find(&m_pDataSet[x][y]))
		{
			return TRUE;
		}
	}
	return FALSE;*/
	if (m_listChecked.find(&m_pDataSet[x][y]) != m_listChecked.end())
		return TRUE;
	else
		return FALSE;
}

VOID CGroupProcessDlg::GroupData(int& x, int& y)
{
	CGroupList* pList = new CGroupList;
	CGroupList& rList = *pList;
	m_nTotalX = 0;
	m_nTotalY = 0;
	FindSameGroupElement(x, y, rList);

	POSITION pos = rList.GetHeadPosition();
	for (int i = 0; i < rList.GetCount(); i++)
	{
		double* pElement = rList.GetNext(pos);
		m_listChecked.insert(pElement);
	}

	if (rList.GetCount() > m_nMinPopulationValue)
	{
		m_nGroupIndex++;
		m_listGroups.AddTail(pList);
		Position* pPos = new Position;
		pPos->groupIndex = m_nGroupIndex;
		pPos->x = m_nTotalX*1.0 / rList.GetCount();
		pPos->y = m_nTotalY*1.0 / rList.GetCount();
		m_vecPositions.push_back(pPos);
	}
	else
	{
		delete pList;
	}
}

VOID CGroupProcessDlg::FindSameGroupElement(int& x, int& y, CGroupList& rList)
{
	m_nTotalX += x;
	m_nTotalY += y;
	rList.AddTail(&m_pDataSet[x][y]);
	int i, j;
	//left down
	i = x+1;
	j = y-1;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
	//down
	i = x+1;
	j = y;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
	//right down
	i = x+1;
	j = y+1;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}//left down
	i = x;
	j = y+1;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
	//left down
	i = x;
	j = y-1;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
	//left down
	i = x-1;
	j = y-1;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
	//left down
	i = x-1;
	j = y;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
	//left down
	i = x-1;
	j = y+1;
	if (IsDataValid(i, j) && !rList.Find(&m_pDataSet[i][j]))
	{
		FindSameGroupElement(i, j, rList);
	}
}

VOID CGroupProcessDlg::CalculateStatistics()
{
	INT nIndex = -1;
	std::vector<Position*>::iterator iter;
	for (iter = m_vecPositions.begin(); iter != m_vecPositions.end(); iter++)
	{
		nIndex = (*iter)->groupIndex - 1;
		POSITION pos = m_listGroups.FindIndex(nIndex);
		CGroupList* pList = m_listGroups.GetAt(pos);
		CGroupList& rList = *pList;
		POSITION posList = rList.GetHeadPosition();
		double dSum = 0.0, dMin = 9999999999.99, dMax = -9999999999.99, dAve = 0.0;
		INT nCount;
		double* pdValue;
		for (int i = 0; i < rList.GetCount(); i++)
		{
			pdValue = rList.GetNext(posList);
			dSum += (*pdValue);
			if (*pdValue > dMax)
				dMax = *pdValue;
			if (*pdValue < dMin)
				dMin = *pdValue;
		}
		dAve = dSum / rList.GetCount();
		double dTotalForce = dSum*0.125*0.125;
		nCount = rList.GetCount();
		m_listSum.push_back(dTotalForce);
		m_listAve.push_back(dAve);
		m_listMin.push_back(dMin);
		m_listMax.push_back(dMax);
		double dArea = nCount*0.125*0.125;
		m_listPopulation.push_back(dArea);

		posList = rList.GetHeadPosition();
		double dSquaredSum = 0.0, dSquaredAve = 0.0;
		for (int i = 0; i < rList.GetCount(); i++)
		{
			pdValue = rList.GetNext(posList);
			dSquaredSum += pow((*pdValue)-dAve, 2);
		}
		dSquaredAve = dSquaredSum / rList.GetCount();
		double dStdev = sqrt(dSquaredAve);
		m_listStdev.push_back(dStdev);
	}
}

BOOL CGroupProcessDlg::SaveStatisticsIntoFile(CString szPath)
{
	UpdateData(TRUE);
	CFile file;
	if (!file.Open(szPath, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate))
	{
		CString szMsg = "Open Export File Failed!";
		MessageBox(szMsg);
		return FALSE;
	}
	CString szContent;
	//szContent.Format("total in a %d * %d matrix\r\n\r\nSummation:\r\n", m_nBlockRows, m_nBlockColumns);
	szContent.Format("total in a %d * %d matrix\r\n\r\nTotal Force (N):\r\n", m_nBlockRows, m_nBlockColumns);
	file.Write(szContent, szContent.GetLength());
	//sum
	std::list<double>::iterator iter;
	iter = m_listSum.begin();
	for (int i = 0; i <m_nBlockRows; i++)
	{
		if (iter != m_listSum.end())
		{
			szContent.Format("%f",*iter);
			iter++;
			for (int j =1; j < m_nBlockColumns; j++)
			{
				if (iter != m_listSum.end())
				{
					szContent.Format("%s\t%f", szContent, *iter);
					iter++;
				}
			}
			szContent.Format("%s\r\n", szContent);
		}
		file.Write(szContent, szContent.GetLength());
	}
	//population
	szContent.Format("\r\nArea (mm2):\r\n");
	file.Write(szContent, szContent.GetLength());
	std::list<double>::iterator iterInt = m_listPopulation.begin();
	for (int i = 0; i <m_nBlockRows; i++)
	{
		if (iterInt != m_listPopulation.end())
		{
			szContent.Format("%f",*iterInt);
			iterInt++;
			for (int j =1; j < m_nBlockColumns; j++)
			{
				if (iterInt != m_listPopulation.end())
				{
					szContent.Format("%s\t%f", szContent, *iterInt);
					iterInt++;
				}
			}
			szContent.Format("%s\r\n", szContent);
		}
		file.Write(szContent, szContent.GetLength());
	}
	//ave
	szContent.Format("\r\nAverage:\r\n");
	file.Write(szContent, szContent.GetLength());
	iter = m_listAve.begin();
	for (int i = 0; i <m_nBlockRows; i++)
	{
		if (iter != m_listAve.end())
		{
			szContent.Format("%f",*iter);
			iter++;
			for (int j =1; j < m_nBlockColumns; j++)
			{
				if (iter != m_listAve.end())
				{
					szContent.Format("%s\t%f", szContent, *iter);
					iter++;
				}
			}
			szContent.Format("%s\r\n", szContent);
		}
		file.Write(szContent, szContent.GetLength());
	}
	//stdev
	szContent.Format("\r\nStandard Deviation:\r\n");
	file.Write(szContent, szContent.GetLength());
	iter = m_listStdev.begin();
	for (int i = 0; i <m_nBlockRows; i++)
	{
		if (iter != m_listStdev.end())
		{
			szContent.Format("%f",*iter);
			iter++;
			for (int j =1; j < m_nBlockColumns; j++)
			{
				if (iter != m_listStdev.end())
				{
					szContent.Format("%s\t%f", szContent, *iter);
					iter++;
				}
			}
			szContent.Format("%s\r\n", szContent);
		}
		file.Write(szContent, szContent.GetLength());
	}
	//min
	szContent.Format("\r\nMinimum:\r\n");
	file.Write(szContent, szContent.GetLength());
	iter = m_listMin.begin();
	for (int i = 0; i <m_nBlockRows; i++)
	{
		if (iter != m_listMin.end())
		{
			szContent.Format("%f",*iter);
			iter++;
			for (int j =1; j < m_nBlockColumns; j++)
			{
				if (iter != m_listMin.end())
				{
					szContent.Format("%s\t%f", szContent, *iter);
					iter++;
				}
			}
			szContent.Format("%s\r\n", szContent);
		}
		file.Write(szContent, szContent.GetLength());
	}
	//max
	szContent.Format("\r\nMaximum:\r\n");
	file.Write(szContent, szContent.GetLength());
	iter = m_listMax.begin();
	for (int i = 0; i <m_nBlockRows; i++)
	{
		if (iter != m_listMax.end())
		{
			szContent.Format("%f",*iter);
			iter++;
			for (int j =1; j < m_nBlockColumns; j++)
			{
				if (iter != m_listMax.end())
				{
					szContent.Format("%s\t%f", szContent, *iter);
					iter++;
				}
			}
			szContent.Format("%s\r\n", szContent);
		}
		file.Write(szContent, szContent.GetLength());
	}
	
	file.Close();
	return TRUE;
}

void CGroupProcessDlg::OnBnClickedCheckDefaultDes()
{
	CButton* pBtn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_DEFAULT_DES));
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_EXPORT_PATH)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(FALSE);
		if (m_szImportFilePath != "")
		{
			UpdateData(TRUE);
			if (m_bBatch)
			{
				CString szTempPath = m_szImportFilePath;
				if (szTempPath[szTempPath.GetLength()-1] == '\\')
				{
					szTempPath = szTempPath.Left(szTempPath.GetLength()-1);
				}
				m_szExportFilePath.Format("%s\\Result", szTempPath);
			}
			else
			{
				m_szExportFilePath = m_szImportFilePath;
				INT nIndex = m_szExportFilePath.ReverseFind(_T('.'));
				m_szExportFilePath.Insert(nIndex, _T("_GrpnPrcs"));
			}
			UpdateData(FALSE);
		}

		m_bDefaultOutPath = TRUE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "DefaultOutPath",m_bDefaultOutPath);
	}
	else
	{
		GetDlgItem(IDC_EDIT_EXPORT_PATH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_EXPORT)->EnableWindow(TRUE);

		m_bDefaultOutPath = FALSE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "DefaultOutPath",m_bDefaultOutPath);
	}
}


void CGroupProcessDlg::OnEnChangeEditImportPath()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CButton* pBtn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_DEFAULT_DES));
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		UpdateData(TRUE);
		if (m_szImportFilePath != "")
		{
			if (m_bBatch)
			{
				CString szTempPath = m_szImportFilePath;
				if (szTempPath[szTempPath.GetLength()-1] == '\\')
				{
					szTempPath = szTempPath.Left(szTempPath.GetLength()-1);
				}
				m_szExportFilePath.Format("%s\\Result", szTempPath);
			}
			else
			{
				m_szExportFilePath = m_szImportFilePath;
				INT nIndex = m_szExportFilePath.ReverseFind(_T('.'));
				m_szExportFilePath.Insert(nIndex, _T("_GrpnPrcs"));
			}
		}
		else
		{
			m_szExportFilePath = "";
		}
		UpdateData(FALSE);
	}
}





void CGroupProcessDlg::OnBnClickedCheckRememberSettings()
{
	// TODO: Add your control notification handler code here
	CButton* pBtn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_REMEMBER_SETTINGS));
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		m_bRememberSettings = TRUE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "RemeberSettings",m_bRememberSettings);
	}
	else
	{
		m_bRememberSettings = FALSE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "RemeberSettings",m_bRememberSettings);
	}

	OnBnClickedCheckOpenfileAfterDone();
	OnEnChangeEditMinVal();
	OnEnChangeEditMinPopulation();
	OnEnChangeEditRows();
	OnEnChangeEditColumns();
}


void CGroupProcessDlg::OnBnClickedCheckOpenfileAfterDone()
{
	// TODO: Add your control notification handler code here
	CButton* pBtn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_OPENFILE_AFTER_DONE));
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		m_bRememberSettings = TRUE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "OpenFile",m_bRememberSettings);
	}
	else
	{
		m_bRememberSettings = FALSE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "OpenFile",m_bRememberSettings);
	}
}


void CGroupProcessDlg::OnEnChangeEditMinVal()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	CString szTemp;
	szTemp.Format("%f",m_dMinDataValue);
	AfxGetApp()->WriteProfileString("GroupProcess", "Criteria1",szTemp);
	UpdateData(FALSE);
}


void CGroupProcessDlg::OnEnChangeEditMinPopulation()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt("GroupProcess", "Criteria2",m_nMinPopulationValue);
	UpdateData(FALSE);
}


void CGroupProcessDlg::OnEnChangeEditRows()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt("GroupProcess", "BlockRows",m_nBlockRows);
	UpdateData(FALSE);
}


void CGroupProcessDlg::OnEnChangeEditColumns()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	AfxGetApp()->WriteProfileInt("GroupProcess", "BlockColumns",m_nBlockColumns);
	UpdateData(FALSE);
}


void CGroupProcessDlg::OnBnClickedCheckBatch()
{
	// TODO: Add your control notification handler code here
	CButton* pBtn = static_cast<CButton*>(GetDlgItem(IDC_CHECK_BATCH));
	if (pBtn->GetCheck() == BST_CHECKED)
	{
		m_bBatch = TRUE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "Batch",m_bBatch);
	}
	else
	{
		m_bBatch = FALSE;
		AfxGetApp()->WriteProfileInt("GroupProcess", "Batch",m_bBatch);
	}
}


BOOL CGroupProcessDlg::CreateFileDirectory(const CString& szPath)
{
	CFileFind				finder;
	CString					szTemp;

	int nLen				= 0;
	int nTotal				= szPath.GetLength();

	while (nLen < nTotal)
	{
		if ((szPath[nLen] == _TEXT('\\')) && (nLen != 0) &&
			(szPath[nLen-1] != _TEXT(':')))
		{
			szTemp			= szPath.Left(nLen);

			if (!finder.FindFile(szTemp))
			{
				if (!CreateDirectory(szTemp, NULL))
				{
					return FALSE;
				}
			}
		}

		nLen++;
	}

	if (szTemp != szPath)
	{
		if (!finder.FindFile(szPath))
		{
			if (!CreateDirectory(szPath, NULL))
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}
