// iRDash ServerDlg.h : header file
// Declarations of data types used for communication with iRacing and dashboard feeding thread
#pragma once
#include "afxwin.h"

// Data structure for communication with Dashboard feeding thread
struct sDashboard
{
	bool ThreadIsRunning;
	bool RequestThreadShutdown;
	HANDLE PortHandle;
};

// Structure for iRacing live data storage
struct siRLiveData
{
	float Brake;					// 0 - 0=brake released to 1=max pedal force
	float Clutch;					// 1 - 0=disengaged to 1=fully engaged
	float DriverCarSLBlinkRPM;		// 2 -
	irsdk_EngineWarnings EngineWarnings;	// 3 - Bitfield for warning lights (8 bit value)
	float FuelLevel;				// 4 - Liters of fuel remaining
	float FuelPress;				// 5 - Engine fuel pressure
	int Gear;						// 6 - -1=reverse  0=neutral  1..n=current gear
	bool IsInGarage;				// 7- 1=Car in garage physics running
	bool IsOnTrack;					// 8 - 1=Car on track physics running
	float ManifoldPress;			// 9 - Engine manifold pressure
	float OilLevel;					// 10 - Engine oil level
	float OilPress;					// 11 - Engine oil pressure
	float OilTemp;					// 12 - Engine oil temperature
	float RPM;						// 13 - Engine rpm
	float ShiftGrindRPM;			// 14 - RPM of shifter grinding noise
	irsdk_Flags SessionFlags;		// 15 - Session flags (32 bit value)
	float ShiftIndicatorPct;		// 16
	float Speed;					// 17 - GPS vehicle speed (m/s)
	float Throttle;					// 18 - 0=off throttle to 1=full throttle
	float Voltage;					// 19 - Engine voltage
	float WaterLevel;				// 20 - Engine coolant level
	float WaterTemp;				// 21 - Engine coolant temp
};

// Structure for iRacing live data offset in memory mapped file
struct siRLiveDataOffset
{
	int Brake;
	int Clutch;
	int DriverCarSLBlinkRPM;
	int EngineWarnings;
	int FuelLevel;
	int FuelPress;
	int Gear;
	int IsInGarage;
	int IsOnTrack;
	int ManifoldPress;
	int OilLevel;
	int OilPress;
	int OilTemp;
	int RPM;
	int ShiftGrindRPM;
	int SessionFlags;
	int ShiftIndicatorPct;
	int Speed;
	int Throttle;
	int Voltage;
	int WaterLevel;
	int WaterTemp;
};

// Name of iRacing live data variables
const char iRLiveDataNames[][20] ={
	"Brake",				// 0
	"Clutch",				// 1
	"DriverCarSLBlinkRPM",	// 2
	"EngineWarnings",		// 3
	"FuelLevel",			// 4
	"FuelPress",			// 5
	"Gear",					// 6
	"IsInGarage",			// 7
	"IsOnTrack",			// 8
	"ManifoldPress",		// 9
	"OilLevel",				// 10
	"OilPress",				// 11
	"OilTemp",				// 12
	"RPM",					// 13
	"ShiftGrindRPM",		// 14
	"SessionFlags",			// 15
	"ShiftIndicatorPct",	// 16
	"Speed",				// 17
	"Throttle",				// 18
	"Voltage",				// 19
	"WaterLevel",			// 20
	"WaterTemp"				// 21
};

// ID numbers of iRacing live variables in the internal data structure
#define IDBrake				0
#define IDClutch			1
#define IDDriverCarSLBlinkRPM 2
#define IDEngineWarnings	3
#define IDFuelLevel			4
#define IDFuelPress			5
#define IDGear				6
#define IDIsInGarage		7
#define IDIsOnTrack			8
#define IDManifoldPress		9
#define IDOilLevel			10
#define IDOilPress			11
#define IDOilTemp			12
#define IDRPM				13
#define IDShiftGrindRPM		14
#define IDSessionFlags		15
#define IDShiftIndicatorPct	16
#define IDSpeed				17
#define IDThrottle			18
#define IDVoltage			19
#define IDWaterLevel		20
#define IDWaterTemp			21

#define DEFAULTPORT 4	// use this port number if no registry setting is existing
#define ERROR_NO 0
#define ERROR_SETTINGS_READ 1
#define ERROR_SETTINGS_WRITE 2

// CiRDashServerDlg dialog
class CiRDashServerDlg : public CDialogEx
{
// Construction
public:
	CiRDashServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IRDASHSERVER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedAbout();
	afx_msg void OnClickedConnect();
	afx_msg void OnEnChangePortnumber();
	int ReadSetting();
	int WriteSetting();
	CEdit Port;	// Port number edit control handler
	void ShowConnect();
	void ShowDisconnect();
};

// Handler of dashboard feeding thread
UINT Dash_Thread(LPVOID pParam);