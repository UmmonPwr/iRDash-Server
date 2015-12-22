
// iRDash ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iRDash Server.h"
#include "iRDash ServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Global variables
sDashboard DashData;	// Variable for communication with dashboard thread
extern CiRDashServerDlg *piRDashDlg;

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CiRDashServerDlg dialog
CiRDashServerDlg::CiRDashServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CiRDashServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiRDashServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PORTNUMBER, Port);
}

BEGIN_MESSAGE_MAP(CiRDashServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ABOUT, &CiRDashServerDlg::OnClickedAbout)
	ON_BN_CLICKED(IDC_CONNECT, &CiRDashServerDlg::OnClickedConnect)
	ON_EN_CHANGE(IDC_PORTNUMBER, &CiRDashServerDlg::OnEnChangePortnumber)
END_MESSAGE_MAP()

// CiRDashServerDlg message handlers
BOOL CiRDashServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Initialize Dashboard data structure
	DashData.ThreadIsRunning = false;
	DashData.RequestThreadShutdown = false;
	Port.LimitText(3);	// Limit the length of the port number edit control to 3 character
	ReadSetting();		// Read port number from registry and set up the editbox

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CiRDashServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		// Display the About box
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
void CiRDashServerDlg::OnPaint()
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
HCURSOR CiRDashServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CiRDashServerDlg::OnClickedAbout()
{
	// Display About box
	CAboutDlg ADlg;
	ADlg.DoModal();
}


// Start or terminate the dashboard thread
void CiRDashServerDlg::OnClickedConnect()
{
	CString PortNumber;
	wchar_t PortName[20];
	HANDLE PortHandle;
	DCB PortConfig;
	COMMTIMEOUTS comTimeOut;

	WriteSetting(); // Write port number to registry

	// Check if dashboard thread is already running. If not then open the serial port and start the serial port feeding thread
	if (DashData.ThreadIsRunning == false)
	{
		DashData.RequestThreadShutdown = false;
		// Get port number from edit box and create the port name
		Port.GetWindowTextW(PortNumber);
		wcscpy_s(PortName, 20, L"\\\\.\\COM");
		wcsncat_s(PortName, PortNumber, 3);

		// Open the serial port
		PortHandle = CreateFile(PortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		// CreateFile() can be used to get a handle to a serial port.The "Win32 Programmer's Reference" entry for "CreateFile()" mentions that the share mode must be 0, the create parameter must be OPEN_EXISTING, and the template must be NULL.
		// CreateFile() is successful when you use "COM1" through "COM9" for the name of the file; however, the message INVALID_HANDLE_VALUE is returned if you use "COM10" or greater.

		//	If the name of the port is \\.\COM10, the correct way to specify the serial port in a call to CreateFile() is as follows :
		//CreateFile(
		//	"\\\\.\\COM10",     // address of name of the communications device
		//	fdwAccess,          // access (read-write) mode
		//	0,                  // share mode, must be opened with exclusive-access
		//	NULL,               // address of security descriptor
		//	OPEN_EXISTING,      // how to create, must use OPEN_EXISTING
		//	0,                  // file attributes, not overlapped I/O
		//	NULL                // handle of file with attributes to copy, hTemplate must be NULL for comm devices
		//	);
		// NOTES: This syntax also works for ports COM1 through COM9. Certain boards will let you choose the port names yourself. This syntax works for those names as well.

		if (PortHandle == INVALID_HANDLE_VALUE) return;	// Opening the serial port failed
		
		//  Initialize the DCB structure.
		SecureZeroMemory(&PortConfig, sizeof(DCB));
		PortConfig.DCBlength = sizeof(DCB);

		//  Build on the current configuration by first retrieving all current settings.
		if(!GetCommState(PortHandle, &PortConfig)) return;
		
		//  Serial port data transfer settings: 115,200 bps, 8 data bits, no parity, and 1 stop bit.
		PortConfig.BaudRate = CBR_115200;	//  baud rate
		PortConfig.ByteSize = 8;			//  data size, xmit and rcv
		PortConfig.Parity = NOPARITY;		//  parity bit
		PortConfig.StopBits = ONESTOPBIT;	//  stop bit
		SetCommState(PortHandle, &PortConfig);

		comTimeOut.ReadIntervalTimeout = 3;			// Specify time-out between character for receiving.
		comTimeOut.ReadTotalTimeoutMultiplier = 3;	// Specify value that is multiplied by the requested number of bytes to be read.
		comTimeOut.ReadTotalTimeoutConstant = 2;	// Specify value is added to the product of the ReadTotalTimeoutMultiplier member
		comTimeOut.WriteTotalTimeoutMultiplier = 3;	// Specify value that is multiplied by the requested number of bytes to be sent.
		comTimeOut.WriteTotalTimeoutConstant = 2;	// Specify value is added to the product of the WriteTotalTimeoutMultiplier member
		SetCommTimeouts(PortHandle, &comTimeOut);	// Set the time-out parameter into device control.

		// Change the button text to "Disconnect" and start the dashboard thread
		ShowDisconnect();
		DashData.ThreadIsRunning = true;
		DashData.PortHandle = PortHandle;
		AfxBeginThread(Dash_Thread, NULL);
	}
	else
	{
		// Dashboard thread is already running, request termination
		DashData.RequestThreadShutdown = true;
	}
}


// Write the settings to registry
int CiRDashServerDlg::WriteSetting()
{
	CString CSPortNumber;
	int iPortNumber;
	HKEY hKey;
	DWORD err;

	// Get port number from edit box
	Port.GetWindowTextW(CSPortNumber);
	iPortNumber = _wtoi(CSPortNumber);

	// Create the registry key
	err = RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\iRDash", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, 0, &hKey, NULL);
	if (err != ERROR_SUCCESS) return ERROR_SETTINGS_WRITE;

	// Write setting
	err = RegSetValueEx(hKey, L"PortNumber", 0, REG_DWORD, (LPBYTE)&iPortNumber, sizeof(DWORD));
	if (err != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return ERROR_SETTINGS_WRITE;
	}

	RegCloseKey(hKey);
	return ERROR_NO;
}


// Read the settings from registry
int CiRDashServerDlg::ReadSetting()
{
	CString CSPortNumber;
	int iPortNumber;
	HKEY hKey;
	DWORD cb, type, err;

	// Check if registry manipulating is working
	err = RegCreateKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\iRDash", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_SET_VALUE | KEY_CREATE_SUB_KEY, NULL, &hKey, NULL);
	if (err != ERROR_SUCCESS) return ERROR_SETTINGS_READ;

	// Read setting, if it is not existing then use default value
	cb = sizeof(int);
	err = RegQueryValueEx(hKey, L"PortNumber", 0, &type, (LPBYTE)&iPortNumber, &cb);
	if ((err != ERROR_SUCCESS) || (type != REG_DWORD))
	{
		// create the registry entry if it is missing or has a different type
		iPortNumber = DEFAULTPORT;
		RegDeleteValue(hKey, L"PortNumber");
		RegSetValueEx(hKey, L"PortNumber", 0, REG_DWORD, (LPBYTE)&iPortNumber, sizeof(DWORD));
	}

	RegCloseKey(hKey);

	// Convert int to CString and setup edit box
	CSPortNumber.Format(L"%d", iPortNumber);
	Port.SetWindowTextW(CSPortNumber);

	return ERROR_NO;
}


void CiRDashServerDlg::OnEnChangePortnumber()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


// Show "Connect" on main dialog button
void CiRDashServerDlg::ShowConnect()
{
	CWnd *pBConnect = GetDlgItem(IDC_CONNECT);

	pBConnect->SetWindowText(L"Connect");
}


// Show "Disconnect" on main dialog button
void CiRDashServerDlg::ShowDisconnect()
{
	CWnd *pBConnect = GetDlgItem(IDC_CONNECT);

	pBConnect->SetWindowText(L"Disconnect");
}


// Use separate thread to get the telemetry data from iRacing and send it out through the serial port
UINT Dash_Thread(LPVOID pParam)
{
	char *g_data = NULL;
	int g_nData = 0;
	DWORD length;
	siRLiveData LiveData;
	siRLiveDataOffset LiveDataOffset;
	char LiveDataHeader[4] = { 3, 12, 48 };	// Identification header of the telemetry data block
	char temp_char;

	// Set the "thread is running" flag to allow only one thread running
	DashData.ThreadIsRunning = true;

	// Bump priority up so we get time from the sim
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	while (!DashData.RequestThreadShutdown)
	{
		// Wait for new data and copy it into the g_data buffer, if g_data is not null
		if (irsdk_waitForDataReady(TIMEOUT, g_data))
		{
			const irsdk_header *pHeader = irsdk_getHeader();
			if (pHeader)
			{
				// if header changes size, assume a new connection
				if (!g_data || g_nData != pHeader->bufLen)
				{
					// Reallocate our g_data buffer to fit
					if (g_data) delete[] g_data;
					g_nData = pHeader->bufLen;
					g_data = new char[g_nData];

					// Grab the memory offset of the telemetry data variables
					LiveDataOffset.Brake = irsdk_varNameToOffset(iRLiveDataNames[IDBrake]);
					LiveDataOffset.Clutch = irsdk_varNameToOffset(iRLiveDataNames[IDClutch]);
					LiveDataOffset.EngineWarnings = irsdk_varNameToOffset(iRLiveDataNames[IDEngineWarnings]);
					LiveDataOffset.FuelLevel = irsdk_varNameToOffset(iRLiveDataNames[IDFuelLevel]);
					LiveDataOffset.FuelPress = irsdk_varNameToOffset(iRLiveDataNames[IDFuelPress]);
					LiveDataOffset.Gear = irsdk_varNameToOffset(iRLiveDataNames[IDGear]);
					LiveDataOffset.IsInGarage = irsdk_varNameToOffset(iRLiveDataNames[IDIsInGarage]);
					LiveDataOffset.IsOnTrack = irsdk_varNameToOffset(iRLiveDataNames[IDIsOnTrack]);
					LiveDataOffset.ManifoldPress = irsdk_varNameToOffset(iRLiveDataNames[IDManifoldPress]);
					LiveDataOffset.OilLevel = irsdk_varNameToOffset(iRLiveDataNames[IDOilLevel]);
					LiveDataOffset.OilPress = irsdk_varNameToOffset(iRLiveDataNames[IDOilPress]);
					LiveDataOffset.OilTemp = irsdk_varNameToOffset(iRLiveDataNames[IDOilTemp]);
					LiveDataOffset.RPM = irsdk_varNameToOffset(iRLiveDataNames[IDRPM]);
					LiveDataOffset.ShiftGrindRPM = irsdk_varNameToOffset(iRLiveDataNames[IDShiftGrindRPM]);
					LiveDataOffset.SessionFlags = irsdk_varNameToOffset(iRLiveDataNames[IDSessionFlags]);
					LiveDataOffset.ShiftIndicatorPct = irsdk_varNameToOffset(iRLiveDataNames[IDShiftIndicatorPct]);
					LiveDataOffset.Speed = irsdk_varNameToOffset(iRLiveDataNames[IDSpeed]);
					LiveDataOffset.Throttle = irsdk_varNameToOffset(iRLiveDataNames[IDThrottle]);
					LiveDataOffset.Voltage = irsdk_varNameToOffset(iRLiveDataNames[IDVoltage]);
					LiveDataOffset.WaterLevel = irsdk_varNameToOffset(iRLiveDataNames[IDWaterLevel]);
					LiveDataOffset.WaterTemp = irsdk_varNameToOffset(iRLiveDataNames[IDWaterTemp]);
				}
				else if (g_data)
				{
					// Load the telemetry data to our structure
					LiveData.Brake = *(float *)(g_data + LiveDataOffset.Brake);
					LiveData.Clutch = *(float *)(g_data + LiveDataOffset.Clutch);
					LiveData.EngineWarnings = *(irsdk_EngineWarnings *)(g_data + LiveDataOffset.EngineWarnings);
					LiveData.FuelLevel = *(float *)(g_data + LiveDataOffset.FuelLevel);
					LiveData.FuelPress = *(float *)(g_data + LiveDataOffset.FuelPress);
					LiveData.Gear = *(int *)(g_data + LiveDataOffset.Gear);
					LiveData.IsInGarage = *(bool *)(g_data + LiveDataOffset.IsInGarage);
					LiveData.IsOnTrack = *(bool *)(g_data + LiveDataOffset.IsOnTrack);
					LiveData.ManifoldPress = *(float *)(g_data + LiveDataOffset.ManifoldPress);
					LiveData.OilLevel = *(float *)(g_data + LiveDataOffset.OilLevel);
					LiveData.OilPress = *(float *)(g_data + LiveDataOffset.OilPress);
					LiveData.RPM = *(float *)(g_data + LiveDataOffset.RPM);
					LiveData.ShiftGrindRPM = *(float *)(g_data + LiveDataOffset.ShiftGrindRPM);
					LiveData.SessionFlags = *(irsdk_Flags *)(g_data + LiveDataOffset.SessionFlags);
					LiveData.ShiftIndicatorPct = *(float *)(g_data + LiveDataOffset.ShiftIndicatorPct);
					LiveData.Speed = *(float *)(g_data + LiveDataOffset.Speed);
					LiveData.Throttle = *(float *)(g_data + LiveDataOffset.Throttle);
					LiveData.Voltage = *(float *)(g_data + LiveDataOffset.Voltage);
					LiveData.WaterLevel = *(float *)(g_data + LiveDataOffset.WaterLevel);
					LiveData.WaterTemp = *(float *)(g_data + LiveDataOffset.WaterTemp);

					// Write our telemetry data to the serial port
					// Send identification header first
					if (WriteFile(DashData.PortHandle, &LiveDataHeader[0], 1, &length, NULL) == false)
					{
						DashData.RequestThreadShutdown = true;	// Request thread termination in case of failed data transfer
					}
					WriteFile(DashData.PortHandle, &LiveDataHeader[1], 1, &length, NULL);
					WriteFile(DashData.PortHandle, &LiveDataHeader[2], 1, &length, NULL);

					// Now we can send the data block
					//temp_char = (char)LiveData.EngineWarnings;
					//WriteFile(DashData.PortHandle, &temp_char, 1, &length, NULL);
					WriteFile(DashData.PortHandle, &LiveData.FuelLevel, 4, &length, NULL);
					temp_char = (char)LiveData.Gear;
					WriteFile(DashData.PortHandle, &temp_char, 1, &length, NULL);
					WriteFile(DashData.PortHandle, &LiveData.IsOnTrack, 1, &length, NULL);
					WriteFile(DashData.PortHandle, &LiveData.RPM, 4, &length, NULL);
					WriteFile(DashData.PortHandle, &LiveData.Speed, 4, &length, NULL);
					WriteFile(DashData.PortHandle, &LiveData.WaterTemp, 4, &length, NULL);
				}
			}
		}
		// Session ended
		else if (!irsdk_isConnected())
		{
			if (g_data) delete[] g_data;
			g_data = NULL;
		}
	}
	// Exited with a request for shutdown
	
	// Clean up before exiting
	if (g_data) delete[] g_data;
	irsdk_shutdown();

	CloseHandle(DashData.PortHandle);	// Close the serial port
	DashData.ThreadIsRunning = false;
	piRDashDlg->ShowConnect();			// Change back button text to "Connect"
	return 0;
}



void CAboutDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}