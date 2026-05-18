#define SKYSTUFF

//#define DEBUG_MESSAGES

#define _WIN32_IE 0x0600

#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include <winsock.h>
#include <shlobj.h>
#include <wininet.h>

#ifdef PRO
#include <hasp_hl.h>
#endif PRO

#include "resource.h"

#include "setupapi.h"	// VC++ 5 one is out of date
#include "stdio.h"	
#include "initguid.h"
#include "winioctl.h"

#include "TSReader.h"
#include "MDInterface.h"
#include "bcdmux.h"
#include "Md\MultiDec\Globals.h"
#include "parser.h"
#include "formatter.h"

// Stuff for imglib
#include <_ISource.h>

#include "TSReader_Stradis/TSReader_Stradis.h"
#include "charting.h"
#include "util.h"
#include "EPGGrid.h"
#include "registry_list.h"

#ifdef PRO
 #include "mdi/mdi.h"
/* #ifndef ZANALYZER
  #ifndef DTVSENTINEL
  #endif DTVSENTINEL
 #endif ZANALYZER*/
#endif PRO

// Stuff in RokuTelnetInterface.c
DWORD WINAPI RokuTelnetControlThread(LPVOID lpv);

#ifndef LITE
// Stuff in ControlServer.c
BOOL StartControlServer();
BOOL TerminateControlServer();
#endif LITE

// Stuff in MPEG2Decoder.c
DWORD WINAPI MPEG2DecoderThread(LPVOID lpv);

#ifndef LITE
// Stuff in H264Decoder.c
DWORD WINAPI H264DecoderThread(LPVOID lpv);

// Stuff in MPEG4Decoder.c
DWORD WINAPI MPEG4DecoderThread(LPVOID lpv);
#endif LITE

// Audio decoders
DWORD WINAPI MPEGAudioDecoderThread(LPVOID lpv);
#ifndef LITE
DWORD WINAPI AC3AudioDecoderThread(LPVOID lpv);
DWORD WINAPI AACAudioDecoderThread(LPVOID lpv);
#endif LITE

// In export.c
void HTMLExport(HANDLE hHTMFile, int nExportSITables, char * szOutputFilename);
void XMLExport(HWND hDlg, HANDLE hXMLFile);
void SIParserExport(HWND hDlg);
void StartXMLExport(HWND hDlg, BOOL fXMLTVFormat);
void XMLTVExport(HWND hDlg, HANDLE hXMLFile);


#ifdef PRO
// Stuff in EITServer.c
BOOL StartEITServer();
BOOL TerminateEITServer();
int GetEITConnectionCount();

// Stuff in CCDecoder.c
void ClosedCaptionDecoderToggle(HWND hWnd);
void InputCCData(BYTE * pPESPacket, int nPESLength, int nChartIndex);

// Stuff in VC1Decoder.c
DWORD WINAPI VC1DecoderThread(LPVOID lpv);

// Stuff in archive.c
BOOL StartArchivePrograms(HWND hWnd);
void StopArchivePrograms(HWND hWnd);
void ArchiveProgramData(BYTE * pBuffer, int nLength);
void ViewArchivedFiles(HWND hWnd);
BOOL CALLBACK SaveEPGDataDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Stuff in Forwarder.c
BOOL StartUDPForwarder(HWND hWnd);
BOOL StopUDPForwarder(HWND hWnd);
void ForwardProgramData(BYTE * pBuffer, int nLength);
void ForwarderModuleStartStop(HWND hWnd, int nIndex);

// Stuff in StreamMonitor.c
void MonitorProgramData(BYTE * pBuffer, int nLength);

// Stuff in ProfileBrowser.c
BOOL ShowProfileBrowser();

// Stuff in mosaic.c
void ShowVideoMosaic(HWND hWnd);

// Stuff in StreamMonitor.c
BOOL CALLBACK StreamMonitorDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Stuff in GPSSignal.c
BOOL CALLBACK GPSSignalExportDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Stuff in TitleThumbnails.c
BOOL CALLBACK AudioThumbnailSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL DecodeAudioTitleData(BYTE * pPESPacket, int nPacketLength, int nES);
#endif PRO

// Stuff in CI-CAM.C
BOOL CALLBACK CAMMenuDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
int GenerateCAPMT(BYTE * pCAPMTBuffer, int nBufferLength, int nPMTIndex);
typedef BOOL (* td_SendCAPMT) (BYTE * pBuffer, int nLength);
BOOL (* SendCAPMT) (BYTE * pBuffer, int nLength);

// Stuff in sky_epg.c
#ifndef LITE
void ParseSkyEPG(BYTE * pPacket, int nLength, BOOL fPrimaryEPGPID);
void UpdateSkyEPGMap(int nBATID);
#endif LITE

// Stuff in settings.c
BOOL CALLBACK SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Completion routine for parser
#ifndef LITE
typedef void (* td_InputData) (BYTE * pPESPacket, int nPESLength, int nChartIndex);
#endif LITE

int nTSReaderReturnValue;
PVARIABLES v;
PTSPARSERVARIABLES tv;

///////////////////////// these should go into the v struct
#ifdef PRO
tMDISTREAM * mdi;	
#endif PRO
///

#define SOURCE_INFO_COLOR_1 RGB(41, 253, 136)
#define SOURCE_INFO_COLOR_2 RGB(7,141,44)

extern char szProValue[];
#ifdef NONOEM
 #ifdef LITE
 char gszAppName[] = TEXT("TSReader Lite");
 char szLiteWarning[] = {"This function is only available in registered versions of TSReader"};
 #else LITE
  #ifndef PRO
   char gszAppName[32] = TEXT("TSReaderPro");
  #else PRO
   #ifndef ZANALYZER
    #ifndef DTVSENTINEL
     char gszAppName[32] = TEXT("TSReader Professional");
    #else DTVSENTINEL
	 char gszAppName[32] = TEXT("DTV Sentinel");
    #endif DTVSENTINEL
   #else ZANALYZER
    char gszAppName[32] = TEXT("ZAnalyzer");
   #endif ZANALYZER
  #endif PRO
 #endif LITE
#else NONOEM
 #ifdef INEOQUEST
 char gszAppName[] = TEXT("TSReader IneoQuest OEM");
 #endif INEOQUEST
#endif NONOEM
char gszMainClass[256] = {TEXT("TSReaderMain")};
char gszEPGGridClass[] = {"TSReaderEPGClass"};
char gszChartClass[] = {"TSReaderChartClass"};
char gszRestartNeeded[128] = {"For this change to take full effect you should restart "};
char gszKeyName[64] = TEXT("Software\\TSReaderPro");
#ifdef PRO
char gszVideoMosaicClass[] = {"TSReaderVideoMosaicClass"};
#endif PRO

// Source DLL functions
BOOL (* Init) (PSOURCESTRUCT pss);
BOOL (* DeInit) ();
BOOL (* Start) ();
BOOL (* Stop) ();
BOOL (* TuneDialog) (HWND hWnd);
BOOL (* Tune) ();
BOOL (* GetDescription) (char * szDescription, char * szCommandLineParameters, BOOL * fCanBeStopped, int * nMaxPIDs, DWORD * dwCapabilities);
BOOL (* ParseSourceModuleCommandLine) (PSOURCESTRUCT pss, char * szCommandLine, BOOL fQuiet);
BOOL (* PIDManagement) (BOOL fAdd, int nPID, BOOL fTemporary);
BOOL (* IsPIDActive) (int nPID);
BOOL (* GetTunerString) (char * szString);
BOOL (* GetSignalString) (char * szString);
int  (* GetSyncLossCount) (BOOL fReset);
int  (* GetRetuneCount) (BOOL fReset);

// forward declarations
DWORD WINAPI ParseIncomingTSDataThread(LPVOID lpv);

// Some strings
char gszDecimalString[] = {"(decimal)"};
char gszHexString[] = {"(hex)"};

// Code
void StartIncomingTSDataThread()
{
	DWORD dwThreadID;
	int j;

	for (j = 0; j < MAX_TS_BUFFERS; j++)
		v->ss.tsb[j].nSize = 0;

	v->ss.fReadThreadTerminated = FALSE;
	v->fDataReceviedInParseIncomingDataThread = FALSE;
	v->hStreamProcessingThread = CreateThread(NULL, 0, ParseIncomingTSDataThread, (LPVOID)0, CREATE_SUSPENDED, &dwThreadID);
	SourceHelper_SetWorkerThreadPriorities(TRUE);
	ResumeThread(v->hStreamProcessingThread);
}

void SaveSettings_New(HKEY hkReg)
{
	int nOffset = 0;

	while (lstrlen(rl[nOffset].szDescription))
	{
		DWORD dwType = rl[nOffset].dwType;

		switch(rl[nOffset].dwType)
		{
		case REG_DWORD:
			RegSetValueEx(hkReg, rl[nOffset].szDescription, 0, REG_DWORD, (BYTE *)v + rl[nOffset].dwOffset, sizeof(DWORD));
			break;
		case REG_SZ:
			{
				BYTE * szString = (BYTE *)v + rl[nOffset].dwOffset;

				RegSetValueEx(hkReg, rl[nOffset].szDescription, 0, REG_SZ, (BYTE *)szString, lstrlen(szString) + 1);
			}
			break;
		}
		nOffset++;
	};
}

void SaveSettings()
{
	LONG lKey;
	HKEY hkMainReg;
	DWORD dwDisposition;
	char szKeyName[MAX_PATH];

	lstrcpy(szKeyName, gszKeyName);
	if (lstrlen(v->szProfileName))
	{
		lstrcat(szKeyName, "-");
		lstrcat(szKeyName, v->szProfileName);
	}

	lKey = RegCreateKeyEx(v->hRegistryRoot,
		                  szKeyName,
						  0,
						  gszAppName,
					      REG_OPTION_NON_VOLATILE,
						  KEY_ALL_ACCESS,
						  NULL,
						  &hkMainReg,
						  &dwDisposition);
	if (lKey == ERROR_SUCCESS)
	{
		int i;

		SaveSettings_New(hkMainReg);

		for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
		{
			char szKey[128];

			wsprintf(szKey, "VLCConfigDescription%d", i);
			RegSetValueEx(hkMainReg, szKey, 0, REG_SZ, (BYTE *)v->szVLCConfigDescription[i], lstrlen(v->szVLCConfigDescription[i]) + 1);
			wsprintf(szKey, "VLCConfigCommand%d", i);
			RegSetValueEx(hkMainReg, szKey, 0, REG_SZ, (BYTE *)v->szVLCConfigCommand[i], lstrlen(v->szVLCConfigCommand[i]) + 1);
		}

		RegSetValueEx(hkMainReg, "SplitRecord", 0, REG_DWORD, (BYTE *)&v->fSplitRecord, sizeof(DWORD));
		RegSetValueEx(hkMainReg, "SplitFileSize", 0, REG_DWORD, (BYTE *)&v->nSplitFileSize, sizeof(DWORD));
		RegSetValueEx(hkMainReg, "SplitSeconds", 0, REG_DWORD, (BYTE *)&v->fSplitSeconds, sizeof(DWORD));

		if (!v->fHideWhenMinimizedTemporary)
			RegSetValueEx(hkMainReg, "HideWhenMinimized", 0, REG_DWORD, (BYTE *)&v->fHideWhenMinimized, sizeof(DWORD));

#ifndef LITE		
		RegSetValueEx(hkMainReg, "IgnoredNetworks", 0, REG_BINARY, (BYTE *)&v->nIgnoredNetworks, sizeof(v->nIgnoredNetworks));
#ifdef PRO
		for (i = 0; i < MONITOR_COUNT; i++)
		{
			char szValue[64];
			wsprintf(szValue, "StreamMonitorMask%02d", i);
			RegSetValueEx(hkMainReg, szValue, 0, REG_DWORD, (BYTE *)&v->sm[i].fDisabled, sizeof(DWORD));		
		}

#endif PRO
#endif LITE
		if (v->fDisableStreamParsing == FALSE)
		{
			RegSetValueEx(hkMainReg, "ThumbnailProcessingThreadPriority", 0, REG_DWORD, (BYTE *)&v->nThumbnailProcessingThreadPriority, sizeof(DWORD));
			RegSetValueEx(hkMainReg, "ESParsingCounterReload", 0, REG_DWORD, (BYTE *)&v->nESParsingCounterReload, sizeof(DWORD));
		}

		// DiSEqC settings
		i = TRUE;
		RegSetValueEx(hkMainReg, "SwitchValues", 0, REG_DWORD, (BYTE *)&i, sizeof(DWORD));
		for (i = 0; i < MAX_SWITCH_PARAMETERS; i++)
		{
			char szKey[128];
			wsprintf(szKey, "Switch%d_LNBType", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nLNBType, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_LOFrequencyHigh", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nLOFrequencyHigh, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_LOFrequencyLow", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nLOFrequencyLow, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_SwitchFrequency", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nSwitchFrequency, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_Voltage", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nVoltage, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_22KHzTone", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].n22KHzTone, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_PositionerType", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nPositionerType, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_AutoSelect", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nAutoSelect, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_AutoSelectStart", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nAutoSelectFreqStart, sizeof(DWORD));
			wsprintf(szKey, "Switch%d_AutoSelectEnd", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nAutoSelectFreqEnd, sizeof(DWORD));		
			wsprintf(szKey, "Switch%d_AutoSelectOrbital", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nAutoSelectOrbital, sizeof(DWORD));		
			wsprintf(szKey, "Switch%d_AutoSelectNetwork", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nAutoSelectNetwork, sizeof(DWORD));		
			wsprintf(szKey, "Switch%d_AutoSelectPolarity", i + 1);
			RegSetValueEx(hkMainReg, szKey, 0, REG_DWORD, (BYTE *)&v->sp[i].nAutoSelectPolarity, sizeof(DWORD));		
		}

		RegCloseKey(hkMainReg);
	}
}

DWORD myRegQueryDWORDValue(HKEY hk, char * szValue)
{
	DWORD dwRetVal = 0;
	DWORD dwDataSize = sizeof(DWORD);
	DWORD dwType = 0;

	RegQueryValueEx(hk, szValue, NULL, &dwType, (BYTE *)&dwRetVal, &dwDataSize);
	return dwRetVal;
}

void LoadSettings_New(HKEY hkReg)
{
	int nOffset = 0;

	while (lstrlen(rl[nOffset].szDescription))
	{
		DWORD dwType = 0;
		DWORD dwDataSize = rl[nOffset].dwSize;

		RegQueryValueEx(hkReg, rl[nOffset].szDescription, NULL, &dwType, (BYTE *)v + rl[nOffset].dwOffset, &dwDataSize);
		if (dwType != rl[nOffset].dwType)
		{
			char szTemp[MAX_PATH];
			wsprintf(szTemp, "TSReader: Registry type requested %d, got %d for %s\n", rl[nOffset].dwType, dwType, rl[nOffset].szDescription);
			OutputDebugString(szTemp);
		}
		nOffset++;
	};
}

void LoadSettings()
{
	DWORD dwDisposition;
	DWORD dwDataSize;
	DWORD dwType;
	LONG lKey;
	HKEY hkMainReg, hkAllReg;
	char szTemp[128] = {0};
	char szKeyName[MAX_PATH];

	lKey = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		                gszKeyName,
						0,
						KEY_ALL_ACCESS,
						&hkAllReg);
	if (lKey == ERROR_SUCCESS)
	{
		DWORD dwGlobalSettings = 0;
		dwDataSize = sizeof(DWORD);
		RegQueryValueEx(hkAllReg, "GlobalSettings", NULL, &dwType, (BYTE *)&dwGlobalSettings, &dwDataSize);
		if (dwGlobalSettings)
			v->hRegistryRoot = HKEY_LOCAL_MACHINE;
		RegCloseKey(hkAllReg);
	}
	
	lstrcpy(szKeyName, gszKeyName);
	if (lstrlen(v->szProfileName))
	{
		lstrcat(szKeyName, "-");
		lstrcat(szKeyName, v->szProfileName);
	}
	lKey = RegCreateKeyEx(v->hRegistryRoot,
		                  szKeyName,
						  0,
						  gszAppName,
					      REG_OPTION_NON_VOLATILE,
						  KEY_ALL_ACCESS,
						  NULL,
						  &hkMainReg,
						  &dwDisposition);
	if (lKey == ERROR_SUCCESS)
	{
		if (dwDisposition != REG_CREATED_NEW_KEY)
		{
			int i;

v->nProcessPriority = NORMAL_PRIORITY_CLASS;

			LoadSettings_New(hkMainReg);
#ifdef PRO
			if (v->nMaximumThumbnailThreads <= 0)
				v->nMaximumThumbnailThreads = 4;
			if (v->nGPSSerialBaudRate == 0)
				v->nGPSSerialBaudRate = 4800;
			if (v->nGPSLogSeconds == 0)
				v->nGPSLogSeconds = 5;
#endif PRO
#ifndef LITE
			if (v->nMaximumH264Pictures <= 0)
				v->nMaximumH264Pictures = 4;
#endif LITE

			if (lstrlen(v->szSplitFormatString) == 0)
				lstrcpy(v->szSplitFormatString, "%f_%3n.%e");

			for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
			{
				char szKey[128];

				wsprintf(szKey, "VLCConfigDescription%d", i);
				dwDataSize = sizeof(v->szVLCConfigDescription[i]);
				RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)v->szVLCConfigDescription[i], &dwDataSize);
				wsprintf(szKey, "VLCConfigCommand%d", i);
				dwDataSize = sizeof(v->szVLCConfigCommand[i]);
				RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)v->szVLCConfigCommand[i], &dwDataSize);
			}
			if (lstrlen(v->szVLCConfigDescription[0]) == 0)
				lstrcpy(v->szVLCConfigDescription[0], "Play");
			if (lstrlen(v->szVLCConfigCommand[0]) == 0)
				lstrcpy(v->szVLCConfigCommand[0], "<IP>");
		
			if (!(v->nAutoRecord && v->fSplitRecord))
			{
				v->fSplitRecord = myRegQueryDWORDValue(hkMainReg, "SplitRecord");
				v->nSplitFileSize = myRegQueryDWORDValue(hkMainReg, "SplitFileSize");
				v->fSplitSeconds = myRegQueryDWORDValue(hkMainReg, "SplitSeconds");
			}
			if (v->nPIDDataRefreshRate <= 0)
				v->nPIDDataRefreshRate = 250;
			if (v->dwEPGEventColor == 0)			v->dwEPGEventColor = RGB(0, 192, 192);
			if (v->dwEPGSelectedColor == 0)			v->dwEPGSelectedColor = RGB(0, 192, 0);
			if (v->dwEPGChannelColor == 0)			v->dwEPGChannelColor = RGB(192, 192, 0);
			if (v->dwEPGMainTextColor == 0)			v->dwEPGMainTextColor = RGB(255, 255, 255);
			if (v->dwEPGSubTextColor == 0)			v->dwEPGSubTextColor = RGB(0, 0, 0);
			if (v->dwEPGTimeGridColor == 0)			v->dwEPGTimeGridColor = RGB(192, 0, 0);

			if (v->epg.nSearchFlag == 0)
				v->epg.nSearchFlag = 1;
			
#ifndef LITE
			memset(v->nIgnoredNetworks, 0, sizeof(v->nIgnoredNetworks));
			dwDataSize = sizeof(v->nIgnoredNetworks);
			RegQueryValueEx(hkMainReg, "IgnoredNetworks", NULL, &dwType, (BYTE *)&v->nIgnoredNetworks, &dwDataSize);

#endif LITE
			if (v->nVLCPort <= 0)
				v->nVLCPort = 1234;			
			if (v->nStreamingPipeSize <= 0)
				v->nStreamingPipeSize = 20;
			if (v->nThumbnailPipeSize <= 0)
				v->nThumbnailPipeSize = 10;
			dwDataSize = sizeof(v->fHideWhenMinimized);
			RegQueryValueEx(hkMainReg, "HideWhenMinimized", NULL, &dwType, (BYTE *)&v->fHideWhenMinimized, &dwDataSize);
			v->nMinimumPATs = 5;
			v->nMaximumMPEGPictures = 4;
			v->nMaximumDCIIPictures = 40;
			v->nTunerLoops = 1;
			v->nEPGHalfHourWidth = 2;
			v->nEPGChannelHeight = 2;
			v->nGraphRefreshRate = 1000;
			v->nGraphHistoricalPoints = 300;
#ifndef LITE
			dwDataSize = sizeof(v->nMinimumPATs);
			RegQueryValueEx(hkMainReg, "MinimumPATs", NULL, &dwType, (BYTE *)&v->nMinimumPATs, &dwDataSize);
			if (v->nMaximumMPEGPictures == 0)
				v->nMaximumMPEGPictures = 4;
			if (v->nMaximumDCIIPictures == 0)
				v->nMaximumDCIIPictures = 40;
			
			v->fAllowResizing = TRUE;
			if (v->nControlServerPort == 0)
				v->nControlServerPort = 1399;
			dwDataSize = sizeof(v->nTunerLoops);
			RegQueryValueEx(hkMainReg, "TunerLoops", NULL, &dwType, (BYTE *)&v->nTunerLoops, &dwDataSize);
			if (v->nTunerLoops == 0)
				v->nTunerLoops = 1;	
			if (v->nEPGChannelHeight < 2)
				v->nEPGChannelHeight = 2;

#ifdef PRO
			if (v->nEITServerPort == 0)
				v->nEITServerPort = 1401;			
			if (v->nStreamMonitorAlarmTimeout == 0)
				v->nStreamMonitorAlarmTimeout = 10;
			if (v->nGraphHistoricalPoints == 0)
				v->nGraphHistoricalPoints = 300;
		
			for (i = 0; i < MONITOR_COUNT; i++)
			{
				char szValue[64];
				wsprintf(szValue, "StreamMonitorMask%02d", i);
				v->sm[i].fDisabled = myRegQueryDWORDValue(hkMainReg, szValue);
			}
#endif PRO
#endif LITE
			dwDataSize = sizeof(v->nESParsingCounterReload);
			RegQueryValueEx(hkMainReg, "ESParsingCounterReload", NULL, &dwType, (BYTE *)&v->nESParsingCounterReload, &dwDataSize);
			if (v->nESParsingCounterReload < 0)
				v->nESParsingCounterReload = 5;
			dwDataSize = sizeof(v->nThumbnailProcessingThreadPriority);
			RegQueryValueEx(hkMainReg, "ThumbnailProcessingThreadPriority", NULL, &dwType, (BYTE *)&v->nThumbnailProcessingThreadPriority, &dwDataSize);
			v->nExtraSerialTuneDelay = 0;
			dwDataSize = sizeof(v->nExtraSerialTuneDelay);
			RegQueryValueEx(hkMainReg, "ExtraSerialTuneDelay", NULL, &dwType, (BYTE *)&v->nExtraSerialTuneDelay, &dwDataSize);
			dwDataSize = sizeof(v->fNoDSSSupport);
			RegQueryValueEx(hkMainReg, "NoDSSSupport", NULL, &dwType, (BYTE *)&v->fNoDSSSupport, &dwDataSize);
			if (v->dwScrambledPIDColor == 0 && v->dwUnscrambledPIDColor == 0)
			{
				v->dwScrambledPIDColor = RGB(0xff, 0x00, 0x00);
				v->dwUnscrambledPIDColor = RGB(0x00, 0xff, 0x00);
				v->dwScrambledInactivePIDColor = RGB(0x80, 0x00, 0x00);
				v->dwUnscrambledInactivePIDColor = RGB(0x00, 0x80, 0x00);
				v->dwHighlightedPIDColor = RGB(0x00, 0x00, 0xff);
			}

			// DiSEqC settings
			dwDataSize = sizeof(i);
			RegQueryValueEx(hkMainReg, "SwitchValues", NULL, &dwType, (BYTE *)&i, &dwDataSize);
			if (i == TRUE)
			{
				for (i = 0; i < MAX_SWITCH_PARAMETERS; i++)
				{
					char szKey[128];

					wsprintf(szKey, "Switch%d_LNBType", i + 1);
					dwDataSize = sizeof(v->sp[i].nLNBType);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nLNBType, &dwDataSize);						

					wsprintf(szKey, "Switch%d_LOFrequencyHigh", i + 1);
					dwDataSize = sizeof(v->sp[i].nLOFrequencyHigh);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nLOFrequencyHigh, &dwDataSize);	
					
					wsprintf(szKey, "Switch%d_LOFrequencyLow", i + 1);
					dwDataSize = sizeof(v->sp[i].nLOFrequencyLow);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nLOFrequencyLow, &dwDataSize);						
					wsprintf(szKey, "Switch%d_SwitchFrequency", i + 1);
					dwDataSize = sizeof(v->sp[i].nSwitchFrequency);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nSwitchFrequency, &dwDataSize);						
					wsprintf(szKey, "Switch%d_Voltage", i + 1);
					dwDataSize = sizeof(v->sp[i].nVoltage);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nVoltage, &dwDataSize);						
					wsprintf(szKey, "Switch%d_22KHzTone", i + 1);
					dwDataSize = sizeof(v->sp[i].n22KHzTone);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].n22KHzTone, &dwDataSize);						
					wsprintf(szKey, "Switch%d_PositionerType", i + 1);
					dwDataSize = sizeof(v->sp[i].nPositionerType);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nPositionerType, &dwDataSize);						


					wsprintf(szKey, "Switch%d_AutoSelect", i + 1);
					dwDataSize = sizeof(v->sp[i].nAutoSelect);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nAutoSelect, &dwDataSize);						
					wsprintf(szKey, "Switch%d_AutoSelectStart", i + 1);
					dwDataSize = sizeof(v->sp[i].nAutoSelectFreqStart);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nAutoSelectFreqStart, &dwDataSize);						
					wsprintf(szKey, "Switch%d_AutoSelectEnd", i + 1);
					dwDataSize = sizeof(v->sp[i].nAutoSelectFreqEnd);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nAutoSelectFreqEnd, &dwDataSize);						
					wsprintf(szKey, "Switch%d_AutoSelectOrbital", i + 1);
					dwDataSize = sizeof(v->sp[i].nAutoSelectOrbital);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nAutoSelectOrbital, &dwDataSize);						
					wsprintf(szKey, "Switch%d_AutoSelectNetwork", i + 1);
					dwDataSize = sizeof(v->sp[i].nAutoSelectNetwork);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nAutoSelectNetwork, &dwDataSize);						
					wsprintf(szKey, "Switch%d_AutoSelectPolarity", i + 1);
					dwDataSize = sizeof(v->sp[i].nAutoSelectPolarity);
					RegQueryValueEx(hkMainReg, szKey, NULL, &dwType, (BYTE *)&v->sp[i].nAutoSelectPolarity, &dwDataSize);						
				}
			}
			else
			{
				// Default initial LNB switch values
				v->sp[0].nLNBType = LNB_TYPE_DUAL;
				v->sp[0].nLOFrequencyHigh = 10600;
				v->sp[0].nLOFrequencyLow = 9750;
				v->sp[0].nSwitchFrequency = 11700;
				v->sp[0].nVoltage = LNB_VOLTAGE_POLARITY;
				v->sp[0].n22KHzTone = LNB_22KHZ_BAND;
				v->sp[0].nPositionerType = LNB_POSITIONER_NONE;
				v->sp[0].nAutoSelect = AUTO_SELECT_LNB_FREQ;
				v->sp[0].nAutoSelectFreqStart = 10700;
				v->sp[0].nAutoSelectFreqEnd = 12800;

				v->sp[1].nLNBType = LNB_TYPE_SINGLE;
				v->sp[1].nLOFrequencyHigh = 10750;
				v->sp[1].nLOFrequencyLow = 10750;
				v->sp[1].nSwitchFrequency = 0;
				v->sp[1].nVoltage = LNB_VOLTAGE_POLARITY;
				v->sp[1].n22KHzTone = LNB_22KHZ_OFF;
				v->sp[1].nPositionerType = LNB_POSITIONER_NONE;

				v->sp[2].nLNBType = LNB_TYPE_SINGLE;
				v->sp[2].nLOFrequencyHigh = 5150;
				v->sp[2].nLOFrequencyLow = 5150;
				v->sp[2].nSwitchFrequency = 0;
				v->sp[2].nVoltage = LNB_VOLTAGE_POLARITY;
				v->sp[2].n22KHzTone = LNB_22KHZ_OFF;
				v->sp[2].nPositionerType = LNB_POSITIONER_NONE;
				v->sp[2].nAutoSelect = AUTO_SELECT_LNB_FREQ;
				v->sp[2].nAutoSelectFreqStart = 3400;
				v->sp[2].nAutoSelectFreqEnd = 4200;

				v->sp[3].nLNBType = LNB_TYPE_SINGLE;
				v->sp[3].nLOFrequencyHigh = 11250;
				v->sp[3].nLOFrequencyLow = 11250;
				v->sp[3].nSwitchFrequency = 0;
				v->sp[3].nVoltage = LNB_VOLTAGE_POLARITY;
				v->sp[3].n22KHzTone = LNB_22KHZ_OFF;
				v->sp[3].nPositionerType = LNB_POSITIONER_NONE;
			
				v->sp[4].nLNBType = LNB_TYPE_DUAL;
				v->sp[4].nLOFrequencyHigh = 10600;
				v->sp[4].nLOFrequencyLow = 9750;
				v->sp[4].nSwitchFrequency = 11700;
				v->sp[4].nVoltage = LNB_VOLTAGE_POLARITY;
				v->sp[4].n22KHzTone = LNB_22KHZ_BAND;
				v->sp[4].nPositionerType = LNB_POSITIONER_NONE;

				v->sp[5].nLNBType = LNB_TYPE_SINGLE;
				v->sp[5].nLOFrequencyHigh = 5150;
				v->sp[5].nLOFrequencyLow = 5150;
				v->sp[5].nSwitchFrequency = 0;
				v->sp[5].nVoltage = LNB_VOLTAGE_POLARITY;
				v->sp[5].n22KHzTone = LNB_22KHZ_OFF;
				v->sp[5].nPositionerType = LNB_POSITIONER_NONE;
			}
		}
		else
		{
			// First time

			GetCurrentDirectory(sizeof(v->szRecordPIDFolder), v->szRecordPIDFolder);
			GetCurrentDirectory(sizeof(v->ss.szTransportStreamInitialDir), v->ss.szTransportStreamInitialDir);
			GetCurrentDirectory(sizeof(v->szHTMInitialDir), v->szHTMInitialDir);
			GetCurrentDirectory(sizeof(v->szXMLInitialDir), v->szXMLInitialDir);
			v->szRecordFile[0] = '\0';

			v->fRecordPIDNoTSHeader = FALSE;
			v->fDiscardNULLPIDs = FALSE;
			v->fSplitRecord = FALSE;
			v->nSplitFileSize = 4096;
			v->fSortChartByPID = FALSE;
			v->fSortChartDecending = FALSE;
			v->ss.fLastFileTS = FALSE;
			v->nThumbnailPipeSize = 10;
			v->nStreamingPipeSize = 20;
			if (lstrlen(v->szProfileName))
				v->fAgreedToLicense = TRUE;
			v->nTunerLoops = 1;
			v->dwScrambledPIDColor = RGB(0xff, 0x00, 0x00);
			v->dwUnscrambledPIDColor = RGB(0x00, 0xff, 0x00);
			v->dwScrambledInactivePIDColor = RGB(0x80, 0x00, 0x00);
			v->dwUnscrambledInactivePIDColor = RGB(0x00, 0x80, 0x00);
			v->dwHighlightedPIDColor = RGB(0x00, 0x00, 0xff);
			v->nVLCPort = 1234;
			v->dwEPGEventColor = RGB(0, 192, 192);
			v->dwEPGSelectedColor = RGB(0, 192, 0);
			v->dwEPGChannelColor = RGB(192, 192, 0);
			v->dwEPGMainTextColor = RGB(255, 255, 255);
			v->dwEPGSubTextColor = RGB(0, 0, 0);
			v->dwEPGTimeGridColor = RGB(192, 0, 0);
			v->epg.nSearchFlag = 1;
			v->nProcessPriority = NORMAL_PRIORITY_CLASS;		
#ifdef PRO
			v->nStreamMonitorAlarmTimeout = 10;
#endif PRO
		}
		RegCloseKey(hkMainReg);
		
		if (dwDisposition == REG_CREATED_NEW_KEY)
			SaveSettings();
	}
}

char * GetTSRVersion(char * szOutput)
{
	if (szOutput == NULL)
		szOutput = v->szVersionBuffer;

	if (VERSION_SUB_EDIT != '\0')
		wsprintf(szOutput, "%d.%d.%d%c", VERSION_MAJOR, VERSION_MINOR, VERSION_EDIT, VERSION_SUB_EDIT);
	else
		wsprintf(szOutput, "%d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_EDIT);
	return szOutput;
}

#ifdef LITE
DWORD WINAPI LiteMonitorThread(LPVOID lpv)
{
	int nCount = 1275;
	while (v->fParseThreadRunning != FALSE)
	{
		Sleep(50);
		nCount--;
		if (!nCount)
		{
			PostMessage(v->hWndMainWindow, WM_USER + 7, 0, 0);
			return 0;
		}
	}
	return 0;
}
#endif LITE

BOOL ContainsRC(int nPMTIndex)
{
	int nOffset = 0;
	int nProgramInfoLength = v->pat.pmt[nPMTIndex].nProgramInfoLength;
	while (nProgramInfoLength)
	{
		int nDescriptor = v->pat.pmt[nPMTIndex].pProgramInfo[nOffset];
		int nDescriptorLength = v->pat.pmt[nPMTIndex].pProgramInfo[nOffset + 1];
		
		if (nDescriptor == 0xaa)
			return TRUE;
		nOffset += nDescriptorLength + 2;
		nProgramInfoLength -= nDescriptorLength + 2;			
	}
	return FALSE;
}

int GetNextESPID(BOOL fReset, int nES)
{
	static int nPMTIndex = 0;
	static int nESIndex = 0;
	char szTemp[512];

	if (v->fDisableStreamParsing == TRUE)
		return 0;

	v->nDecodeNoPIDTrafficCounter[nES] = 0;
	v->nDecodeNoPESLengthCounter[nES] = 0;

	if (fReset == TRUE)
	{
		nPMTIndex = 0;
		nESIndex = 0;
		for (nES = 0; nES < MAX_ES_PARSERS; nES++)
		{
			v->nESParseType[nES] = PARSE_ES_TYPE_NONE;
			v->nESParsePID[nES] = 0x1fff;
		}
		v->fCompletedESParsing = FALSE;
		return 0;
	}

	v->nESParseType[nES] = PARSE_ES_TYPE_NONE;
	v->nESParsePID[nES] = 0x1fff;

	for (; nPMTIndex < MAX_PAT_ENTRIES; )
	{
		if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
		{
			v->fCompletedESParsing = TRUE;
			v->nESParsingCounter = v->nESParsingCounterReload;
			v->lnESParseStartTime[nES] = 0;
			break;
		}
		if (v->nSingleThumbnailChannel)
		{
			if (v->pat.pmt[nPMTIndex].nProgramNumber != v->nSingleThumbnailChannel)
			{
				nPMTIndex++;
				continue;
			}
		}
		if (v->fWaitForCAThumbnail && v->fRecording == FALSE)
		{
			if (v->nSelectedProgram != nPMTIndex)
			{
				HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);
				TreeView_SelectItem(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
				TreeView_SelectSetFirstVisible(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
				v->dwWaitForCACounter = GetTickCount() + (10 * 1000);
			}
		}
		for (; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
		{
			if (v->nESParseType[nES])
				break;
			if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
			{
				nPMTIndex++;
				nESIndex = 0;
				break;
			}
			if (v->fPIDActive[v->pat.pmt[nPMTIndex].es[nESIndex].nESPID] == FALSE)
			{
				char szTemp[256];
				wsprintf(szTemp, "TSReader: Ignore program %d (ES PID 0x%04x) - PID not active\n", 
					v->pat.pmt[nPMTIndex].nProgramNumber, v->pat.pmt[nPMTIndex].es[nESIndex].nESPID);
				continue;	// ignore empty streams
			}
			if (v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
				continue;
			
			PIDManagement(TRUE, v->pat.pmt[nPMTIndex].es[nESIndex].nESPID, TRUE);
			EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
			switch(v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType)
			{
			case 0x01: // MPEG-1 or MPEG-2 video
			case 0x02:
			case 0x80: // DCII video
				if (v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType == 0x80)
				{
					if (v->nNetworkPID == 0x0010)
					{
						LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
						continue;
					}
				}
				v->nESParseType[nES] = PARSE_ES_TYPE_MPEG2_VIDEO;
				v->fESParseDecodedHeader[nES] = FALSE;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				if (ContainsRC(nPMTIndex))
					v->pat.pmt[nPMTIndex].es[nESIndex].nTeletextServices |= VBI_SERVICE_RC;
				break;
			case 0x03: // MPEG-1 or MPEG-2 audio
			case 0x04:
				v->nESParseType[nES] = PARSE_ES_TYPE_MPEG_AUDIO;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				break;
			case 0x81: // Dolby AC3 audio
				v->nESParseType[nES] = PARSE_ES_TYPE_AC3_AUDIO;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				break;
			case 0x06:	// maybe AC3 or VBI data
				if (IsAC3AudioStream(nPMTIndex, nESIndex) == TRUE)
				{
					v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
					v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
					v->nESParseType[nES] = PARSE_ES_TYPE_AC3_AUDIO;
				}
				else if (IsTeleTextOrVBIStream(nPMTIndex, nESIndex) == TRUE)
					v->nESParseType[nES] = PARSE_ES_TYPE_TELETEXT;
				break;
#ifndef LITE
			case 0x0f:	// MPEG-2 AAC audio
				v->nESParseType[nES] = PARSE_ES_TYPE_MPEG2_AAC_AUDIO;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				break;
			case 0x11:	// MPEG-4 AAC audio
				v->nESParseType[nES] = PARSE_ES_TYPE_MPEG4_AAC_AUDIO;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				break;
			case 0x1b:	// H264 video
				v->nESParseType[nES] = PARSE_ES_TYPE_H264_VIDEO;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				v->fESParseDecodedHeader[nES] = FALSE;
				break;
			case 0x10:	// MPEG-4 video
				v->nESParseType[nES] = PARSE_ES_TYPE_MPEG4_VIDEO;
				v->fESParseDecodedHeader[nES] = FALSE;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				break;
#endif LITE
#ifdef PRO
			case 0xea:	// VC1 video
				v->nESParseType[nES] = PARSE_ES_TYPE_VC1_VIDEO;
				v->fESParseDecodedHeader[nES] = FALSE;
				v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
				v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
				break;
			default:
				if (v->fSongTitleParserEnabled)
				{
					if (v->nSongTitleParserPMT == v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType)
					{
						v->nESParseType[nES] = PARSE_ES_TYPE_AUDIO_TITLE;
						v->fESParseDecodedHeader[nES] = FALSE;
						v->fESParseDecoderStartedLibMPEG[nES] = FALSE;
						v->fESParseDecoderCompletedLibMPEG[nES] = FALSE;
						tv->fBufferSections[nES] = TRUE;
					}
				}
				break;
#endif PRO
			}
			if (v->nESParseType[nES])
			{
				v->nESParsePMTIndex[nES] = nPMTIndex;
				v->nESParseESIndex[nES] = nESIndex;
				v->nESParsePID[nES] = v->pat.pmt[nPMTIndex].es[nESIndex].nESPID;
				v->pat.pmt[nPMTIndex].es[nESIndex].nParseType = v->nESParseType[nES];

			}
			LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
		}
		if (v->nESParseType[nES])
			break;
	}

	switch(v->nESParseType[nES])
	{
	case PARSE_ES_TYPE_MPEG2_VIDEO:
		wsprintf(szTemp, "Parsing MPEG-2 video stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_MPEG_AUDIO:
		wsprintf(szTemp, "Parsing MPEG audio stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_AC3_AUDIO:
		wsprintf(szTemp, "Parsing AC-3 audio stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_TELETEXT:
		wsprintf(szTemp, "Parsing Teletext/VBI stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
#ifndef LITE
	case PARSE_ES_TYPE_MPEG2_AAC_AUDIO:
		wsprintf(szTemp, "Parsing MPEG-2 AAC audio stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_MPEG4_AAC_AUDIO:
		wsprintf(szTemp, "Parsing MPEG-4 AAC audio stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_H264_VIDEO:
		wsprintf(szTemp, "Parsing H.264 video stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_MPEG4_VIDEO:
		wsprintf(szTemp, "Parsing MPEG-4 video stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
#endif LITE
#ifdef PRO
	case PARSE_ES_TYPE_VC1_VIDEO:
		wsprintf(szTemp, "Parsing VC-1 video stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
	case PARSE_ES_TYPE_AUDIO_TITLE:
		wsprintf(szTemp, "Parsing audio title stream from program %d on PID %s", v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber, FormatTooltipPID(v->nESParsePID[nES]));
		break;
#endif PRO
	default:
		szTemp[0] = '\0';
		break;
	}
	if (lstrlen(szTemp))
	{
		char szTemp2[1024] = {"TSReader: "};

		if (v->pChannelData[v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber] != NULL)
		{
			lstrcat(szTemp, " (");
			lstrcat(szTemp, v->pChannelData[v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber]->szShortName);
			lstrcat(szTemp, ")");
		}
		UpdateMainStatusText(szTemp);

		// Debug Output allways very useful
		lstrcat(szTemp2, szTemp);
#ifdef PRO
		wsprintf(szTemp, " - ES thread %d", nES);
		lstrcat(szTemp2, szTemp);
#endif PRO
		lstrcat(szTemp2, "\n"); 
		OutputDebugString(szTemp2);
	}
	if (v->nESParseType[nES])
		v->lnESParseStartTime[nES] = v->lnMuxRatePCR;

#ifdef PROx
	if (v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pRGBVideoFrame != NULL)
	{
		BYTE * pThumbnail = v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pRGBVideoFrame;
		int nHeight = v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].nVideoHeight;
		int nWidth = v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].nVideoWidth;
		int nRow;
		int nColumn;

		for (nRow = 0; nRow < nHeight; nRow++)
		{
			for (nColumn = 0; nColumn < nWidth; nColumn++)
			{
				if (   (nColumn == 0 || nColumn == nWidth - 1)
					|| (nRow == 0)
					|| (nRow == nHeight - 1) )
				{
					*(pThumbnail++) = 0xff;
					*(pThumbnail++) = 0xff;
					*(pThumbnail++) = 0x00;
				}
				else
					pThumbnail += 3;		
			}
		}
		//PostMessage(v->hDlgSIParser, WM_USER + 3, 0, 1);
	}

#endif PRO
	return v->nESParsePID[nES];
}

#ifndef LITE
BOOL DecodeH264Video(BYTE * pPESPacket, int nPacketLength, int nES)
{
	int nOffset = 0;
	BOOL fRetVal = FALSE;

	if (v->fESParseDecodedHeader[nES] == FALSE)
	{
		for (nOffset = 0; nOffset < nPacketLength - 5; nOffset++)
		{
			if (    (pPESPacket[nOffset + 0] == 0x00)
				 && (pPESPacket[nOffset + 1] == 0x00)
				 && (pPESPacket[nOffset + 2] == 0x00)
				 && (pPESPacket[nOffset + 3] == 0x01)
				 && ((pPESPacket[nOffset + 4] & 0x0f) == 0x07) )
			{
				v->fESParseDecodedHeader[nES] = TRUE;
				break;
			}
		}
	}
	if (v->fESParseDecodedHeader[nES] == FALSE)
		return FALSE;

	// Decode the video for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, H264DecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;
		int nActualLength = nPacketLength - nOffset;

		WriteFile(v->hMPEGDecoderWritePipe[nES], &pPESPacket[nOffset], nActualLength, &dwWritten, NULL);
		if (dwWritten != (DWORD)nActualLength)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: DecodeH264: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
			OutputDebugString(szTemp);
		}
	}
	return fRetVal;
}


BOOL DecodeMPEG4Video(BYTE * pPESPacket, int nPacketLength, int nES)
{
	int nOffset = 0;
	BOOL fRetVal = FALSE;

	if (v->fESParseDecodedHeader[nES] == FALSE)
	{
		for (nOffset = 0; nOffset < nPacketLength - 5; nOffset++)
		{
			if (    (pPESPacket[nOffset + 0] == 0x00)
				 && (pPESPacket[nOffset + 1] == 0x00)
				 && (pPESPacket[nOffset + 2] == 0x01)
				 && (pPESPacket[nOffset + 3] == 0xb6) )
			{
				v->fESParseDecodedHeader[nES] = TRUE;
				break;
			}
		}
	}
	if (v->fESParseDecodedHeader[nES] == FALSE)
		return FALSE;

	// Decode the video for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, MPEG4DecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;
		int nActualLength = nPacketLength - nOffset;

		WriteFile(v->hMPEGDecoderWritePipe[nES], &pPESPacket[nOffset], nActualLength, &dwWritten, NULL);
		if (dwWritten != (DWORD)nActualLength)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: DecodeMPEG4: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
			OutputDebugString(szTemp);
		}
	}
	return fRetVal;
}
#endif LITE

#ifdef PRO
BOOL DecodeVC1Video(BYTE * pPESPacket, int nPacketLength, int nES)
{
	int nOffset = 0;
	BOOL fRetVal = FALSE;

	if (v->fESParseDecodedHeader[nES] == FALSE)
	{
		for (nOffset = 0; nOffset < nPacketLength - 5; nOffset++)
		{
			if (    (pPESPacket[nOffset + 0] == 0x00)
				 && (pPESPacket[nOffset + 1] == 0x00)
				 && (pPESPacket[nOffset + 2] == 0x01)
				 && (pPESPacket[nOffset + 3] == 0x0f) )
			{
				v->fESParseDecodedHeader[nES] = TRUE;
				break;
			}
		}
	}
	if (v->fESParseDecodedHeader[nES] == FALSE)
		return FALSE;

	// Decode the video for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, VC1DecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;
		int nActualLength = nPacketLength - nOffset;

		WriteFile(v->hMPEGDecoderWritePipe[nES], &pPESPacket[nOffset], nActualLength, &dwWritten, NULL);
		if (dwWritten != (DWORD)nActualLength)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: DecodeVC1: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
			OutputDebugString(szTemp);
		}
	}
	return fRetVal;
}
#endif PRO

BOOL DecodeMPEG2Video(BYTE * pPESPacket, int nPacketLength, int nES)
{
	int nOffset;
	BOOL fRetVal = FALSE;

	if (v->fESParseDecodedHeader[nES] == FALSE)
	{
		for (nOffset = 0; nOffset < nPacketLength - 4; nOffset++)
		{
			if (    (pPESPacket[nOffset + 0] == 0x00)
				 && (pPESPacket[nOffset + 1] == 0x00)
				 && (pPESPacket[nOffset + 2] == 0x01)
				 && (pPESPacket[nOffset + 3] == 0xB3) )
			{
				// Decode the sequence header
				BYTE * pSequenceHeader = &pPESPacket[nOffset + 0];
				PPARSEDMPEGVIDEO pMPEG;

				int horizontal_size_value = (((pSequenceHeader[4] << 8) | pSequenceHeader[5]) >> 4);
				int vertical_size_value = (((pSequenceHeader[5] & 0x0f) << 8) | pSequenceHeader[6]);
				int aspect_ratio_information = pSequenceHeader[7] >> 4;
				int frame_rate_code = pSequenceHeader[7] & 0x0f;
				int bit_rate_value = pSequenceHeader[8] << 10 | pSequenceHeader[9] << 2 | ((pSequenceHeader[10] >> 6) & 0x03);
				int marker_bit = (pSequenceHeader[10] >> 5) & 0x01;
				int vbv_buffer_size_value = ((pSequenceHeader[10] & 0x1f) << 5) | (pSequenceHeader[11] >> 3);
				int contrained_parameters_flag = (pSequenceHeader[11] >> 2) & 0x01;
				int load_intra_quantiser_matrix = (pSequenceHeader[11] >> 1) & 0x01;
				int load_non_intra_quantiser_matrix = pSequenceHeader[11 + (load_intra_quantiser_matrix * 64)] & 0x01;
				BYTE * pNextHeader = &pSequenceHeader[12 + (load_intra_quantiser_matrix * 64) + (load_non_intra_quantiser_matrix * 64)];
				{
					
					pMPEG = LocalAlloc(LPTR, sizeof(PARSEDMPEGVIDEO));
					if (v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData)
						LocalFree(v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData);
					v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData = (BYTE *)pMPEG;

					pMPEG->horizontal_size_value = horizontal_size_value;
					pMPEG->vertical_size_value = vertical_size_value;
					pMPEG->aspect_ratio_information = aspect_ratio_information;
					pMPEG->frame_rate_code = frame_rate_code;
					pMPEG->bit_rate_value = bit_rate_value;
					pMPEG->marker_bit1 = marker_bit;
					pMPEG->vbv_buffer_size_value = vbv_buffer_size_value;
					pMPEG->contrained_parameters_flag = contrained_parameters_flag;
					pMPEG->load_intra_quantiser_matrix = load_intra_quantiser_matrix;
					pMPEG->load_non_intra_quantiser_matrix = load_non_intra_quantiser_matrix;

					// See if we've got an MPEG-2 extension
					if (   (pNextHeader[0] == 0x00)
						&& (pNextHeader[1] == 0x00)
						&& (pNextHeader[2] == 0x01)
					    && (pNextHeader[3] == 0xb5) )
					{
						BYTE * pExtensionHeader = pNextHeader;

						pMPEG->extension_start_code_identifier = pExtensionHeader[4] >> 4;
						pMPEG->profile_and_level_indication = ((pExtensionHeader[4] & 0x0f) << 4) | (pExtensionHeader[5] >> 4);
						pMPEG->progressive_sequence = (pExtensionHeader[5] >> 3) & 0x01;
						pMPEG->chroma_format = (pExtensionHeader[5] >> 1) & 0x03;
						pMPEG->horizontal_size_extension = ((pExtensionHeader[5] & 0x01) << 1) | (pExtensionHeader[6] >> 7);
						pMPEG->vertical_size_extension = (pExtensionHeader[6] >> 5) & 0x03;
						pMPEG->bit_rate_extension = ((pExtensionHeader[6] & 0x1f) << 7) | (pExtensionHeader[7] >> 1);
						pMPEG->marker_bit2 = pExtensionHeader[7] & 0x01;
						pMPEG->vbv_buffer_size_extension = pExtensionHeader[8];
						pMPEG->low_delay = (pExtensionHeader[9] >> 7) & 0x01;
						pMPEG->frame_rate_extension_n = (pExtensionHeader[9] >> 5) & 0x03;
						pMPEG->frame_rate_extension_d = pExtensionHeader[9] & 0x1f;
					}
					v->fESParseDecodedHeader[nES] = TRUE;
				}
				break;
			}
		}
	}

	// Make sure it's a profile libmpeg2 can handle
	{
		PPARSEDMPEGVIDEO pMPEG = (PPARSEDMPEGVIDEO)v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData;
		if (pMPEG == NULL)
			return FALSE;
	}

	// Decode the video for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, MPEG2DecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;

		while (*pPESPacket == 0xff && nPacketLength)
		{
			pPESPacket++;
			nPacketLength--;
		}

		if (nPacketLength > 0)
		{
			WriteFile(v->hMPEGDecoderWritePipe[nES], &nPacketLength, sizeof(nPacketLength), &dwWritten, NULL);
			WriteFile(v->hMPEGDecoderWritePipe[nES], pPESPacket, nPacketLength, &dwWritten, NULL);
			if (dwWritten != (DWORD)nPacketLength)
			{
				char szTemp[128];
				wsprintf(szTemp, "TSReader: DecodeMPEG2Video: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
				OutputDebugString(szTemp);
			}
		}
	}
	return fRetVal;
}

BOOL DecodeAC3AudioHeader(BYTE * pPESPacket, int nPacketLength, int nES)
{
	int nOffset;
	BOOL fRetVal = FALSE;

	for (nOffset = 0; nOffset < nPacketLength - 10; nOffset++)
	{
		int syncword = pPESPacket[nOffset + 0] << 8 | pPESPacket[nOffset + 1];
		if (syncword == 0x0b77)
		{		
			PPARSEDAC3AUDIO pAC3 = LocalAlloc(LPTR, sizeof(PARSEDAC3AUDIO));

			set_buf(BM_PARSER_THREAD, &pPESPacket[nOffset], 0, FALSE);
			{
				int cmixlev = -1;
				int surmixlev = -1;
				int dsurmod = -1;
				int lfeon;
				int dialnorm;

				int syncword = get_bits(BM_PARSER_THREAD, 16);				// AC3 syncinfo
				int crc1 = get_bits(BM_PARSER_THREAD, 16);
				int fscod = get_bits(BM_PARSER_THREAD, 2);
				int frmsizecod = get_bits(BM_PARSER_THREAD, 6);
				
				int bsid = get_bits(BM_PARSER_THREAD, 5);						// AC3 bsi
				int bsmod = get_bits(BM_PARSER_THREAD, 3);
				int acmod = get_bits(BM_PARSER_THREAD, 3);
				if ((acmod & 0x1) && (acmod != 0x1))
					cmixlev = get_bits(BM_PARSER_THREAD, 2);
				if (acmod & 0x4)
					surmixlev = get_bits(BM_PARSER_THREAD, 2);
				if (acmod == 0x2)
					dsurmod = get_bits(BM_PARSER_THREAD, 2);
				lfeon = get_bits(BM_PARSER_THREAD, 1);
				dialnorm = get_bits(BM_PARSER_THREAD, 5);

				pAC3->fscod = fscod;
				pAC3->frmsizecod = frmsizecod;
				pAC3->bsid = bsid;
				pAC3->bsmod = bsmod;
				pAC3->acmod = acmod;
				pAC3->cmixlev = cmixlev;
				pAC3->surmixlev = surmixlev;
				pAC3->dsurmod = dsurmod;
				pAC3->lfeon = lfeon;
				pAC3->dialnorm = dialnorm;

				if (v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData)
					LocalFree(v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData);
				v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData = (BYTE *)pAC3;
				break;
			}			
		}
	}

#ifdef LITE
#define NOAC3DECODER
#endif
#ifdef INEOQUEST
#define NOAC3DECODER
#endif INEOQUEST

#ifndef NOAC3DECODER
	// Decode the audio for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3 || !v->fAudioThumbnails)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, AC3AudioDecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;

		WriteFile(v->hMPEGDecoderWritePipe[nES], &nPacketLength, sizeof(nPacketLength), &dwWritten, NULL);
		WriteFile(v->hMPEGDecoderWritePipe[nES], pPESPacket, nPacketLength, &dwWritten, NULL);
		if (dwWritten != (DWORD)nPacketLength)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: DecodeAC3AudioHeader: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
			OutputDebugString(szTemp);
		}
	}
#else NOAC3DECODER
	fRetVal = TRUE;
#endif NOAC3DECODER
	
	return fRetVal;
}

BOOL DecodeTeletextVBIHeader(BYTE * pPESPacket, int nPESPacketLength, int nES)
{
	int nServices = 0;
	int nESIndex;

	set_buf(BM_PARSER_THREAD, pPESPacket, 0, FALSE);
	{
		int data_identifier;

		// Handle stuffing at the start
		while (nPESPacketLength > 0)
		{
			data_identifier = get_bits(BM_PARSER_THREAD, 8);
			nPESPacketLength--;
			if (data_identifier != 0xff)
				break;
		}
		while (nPESPacketLength > 0)
		{
			int data_unit_length;
			int data_unit_id = get_bits(BM_PARSER_THREAD, 8);
			nPESPacketLength--;
			data_unit_length = get_bits(BM_PARSER_THREAD, 8);
			/*{
				char szTemp[128];
				wsprintf(szTemp, "TSReader: Teletext PMT Index %d type %02x length %d\n", v->nESParsePMTIndex, data_unit_id, data_unit_length);
				OutputDebugString(szTemp);
			}*/
			nPESPacketLength--;
			if (   data_unit_id == 0x02
				|| data_unit_id == 0x03
				|| data_unit_id == 0xC0
				|| data_unit_id == 0xC1)
			{
				// txt_data_field
				int i;
				int reserved_for_future_use = get_bits(BM_PARSER_THREAD, 2);
				int field_parity = get_bits(BM_PARSER_THREAD, 1);
				int line_offset = get_bits(BM_PARSER_THREAD, 5);
				int framing_code = get_bits(BM_PARSER_THREAD, 8);
				int txt_data_block;
				for (i = 0; i < 42; i++) // 336 in bits
					txt_data_block = get_bits(BM_PARSER_THREAD, 8);
				nPESPacketLength -= 44; data_unit_length -= 44;
				switch(data_unit_id)
				{
				case 0x02:
					nServices |= VBI_SERVICE_TXT;
					break;
				case 0x03:
					nServices |= VBI_SERVICE_SUB;
					break;
				case 0xc0:
				case 0xc1:
					nServices |= VBI_SERVICE_ITXT;
					break;
				}
			}
			else if (data_unit_id == 0xc3)
			{
				// vps_data_field
				int i;
				int reserved_for_future_use = get_bits(BM_PARSER_THREAD, 2);
				int field_parity = get_bits(BM_PARSER_THREAD, 1);
				int line_offset = get_bits(BM_PARSER_THREAD, 5);
				int vps_data_block;
				for (i = 0; i < 13; i++) // 104 in bits
					vps_data_block = get_bits(BM_PARSER_THREAD, 8);
				nPESPacketLength -= 14; data_unit_length -= 14;
				nServices |= VBI_SERVICE_VPS;
			}
			else if (data_unit_id == 0xc4)
			{
				// wss_data_field
				int reserved_for_future_use = get_bits(BM_PARSER_THREAD, 2);
				int field_parity = get_bits(BM_PARSER_THREAD, 1);
				int line_offset = get_bits(BM_PARSER_THREAD, 5);
				int wss_aspect_ratio = get_bits(BM_PARSER_THREAD, 4);
				int wss_enhanced_services = get_bits(BM_PARSER_THREAD, 4);
				int wss_subtitles = get_bits(BM_PARSER_THREAD, 3);
				int wss_others = get_bits(BM_PARSER_THREAD, 3);
				//int wss_data_block = get_bits(BM_PARSER_THREAD, 14);
				reserved_for_future_use = get_bits(BM_PARSER_THREAD, 2);
				nPESPacketLength -= 3; data_unit_length -= 3;
				nServices |= VBI_SERVICE_WSS;
				switch(wss_aspect_ratio)		// bits are LSB in the WSS spec
				{
								// 0123 bit number
				case 0x01:		// 0001 full format 4:3
					nServices |= VBI_SERVICE_WSS_43;
					break;
				case 0x08:		// 1000 box 14:9 Centre
					nServices |= VBI_SERVICE_WSS_149;
					break;
				case 0x04:		// 0100 box 14:9 Top
					nServices |= VBI_SERVICE_WSS_149;
					break;
				case 0x0d:		// 1101 box 16:9 Centre
					nServices |= VBI_SERVICE_WSS_169;
					break;
				case 0x02:		// 0010 box 16:9 Top
					nServices |= VBI_SERVICE_WSS_169;
					break;
				case 0x0b:		// 1011 box > 16:9 Centre
					nServices |= VBI_SERVICE_WSS_169;
					break;
				case 0x07:		// 0111 full format 4:3 (shoot and protect 14:9 Centre)
					nServices |= VBI_SERVICE_WSS_43;
					break;
				case 0x0e:		// 1110 full format 16:9
					nServices |= VBI_SERVICE_WSS_169;
					break;
				}
			}
			else if (data_unit_id == 0xc5)
			{
				// closed_captioning_data_field
				int reserved_for_future_use = get_bits(BM_PARSER_THREAD, 2);
				int field_parity = get_bits(BM_PARSER_THREAD, 1);
				int line_offset = get_bits(BM_PARSER_THREAD, 5);
				int closed_captioning_data_block = get_bits(BM_PARSER_THREAD, 16);
				nPESPacketLength -= 3; data_unit_length -= 3;
				nServices |= VBI_SERVICE_CC;
			}
			else if (data_unit_id == 0xc6)
			{
				// monochrome_data_field
				int i;
				int first_segment_flag = get_bits(BM_PARSER_THREAD, 1);
				int last_segment_flag = get_bits(BM_PARSER_THREAD, 1);
				int field_parity = get_bits(BM_PARSER_THREAD, 1);
				int line_offset = get_bits(BM_PARSER_THREAD, 5);
				int first_pixel_position = get_bits(BM_PARSER_THREAD, 16);
				int n_pixels = get_bits(BM_PARSER_THREAD, 8);
				int Y_value;
				for (i = 0; i < n_pixels; i++)
				{
					Y_value = get_bits(BM_PARSER_THREAD, 8);
					nPESPacketLength--; data_unit_length--;
				}
				nPESPacketLength -= 4; data_unit_length -= 4;
				nServices |= VBI_SERVICE_422;
			}
			else if (data_unit_id == 0xff)
			{
			}

			while (data_unit_length > 0)
			{
				int stuffing = get_bits(BM_PARSER_THREAD, 8);
				nPESPacketLength--; data_unit_length--;
			}
		}
	}

	// Teletext services flag goes with the video ES (same place
	// as the bitmap)
	for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
	{
		if (v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nESPID == 0)
			break;
		if (   v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nStreamType == 0x01
			|| v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nStreamType == 0x02
			|| v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nStreamType == 0x10
			|| v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nStreamType == 0x1b
			|| v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nStreamType == 0x80
			|| v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nStreamType == 0xea)
		{		
			v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nTeletextServices &= ~(VBI_SERVICE_WSS_43 | VBI_SERVICE_WSS_169 | VBI_SERVICE_WSS_149);
			v->pat.pmt[v->nESParsePMTIndex[nES]].es[nESIndex].nTeletextServices |= nServices;
			break;
		}
	}
	PostMessage(v->hDlgSIParser, WM_USER + 3, 0, 1);
	return TRUE;
}

#ifndef LITE
BOOL DecodeMPEGAACAudio(BYTE * pPESPacket, int nPacketLength, int nES)
{
	BOOL fRetVal = FALSE;
	int nOffset = 0;

	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		for (nOffset = 0; nOffset < nPacketLength - 4; nOffset++)
		{
			int syncword = (pPESPacket[nOffset + 0]  << 8 | pPESPacket[nOffset + 1]);
			if ( ((syncword & 0xfff8) == 0xfff8) && (syncword != 0xffff) )
			{
				break;
			}
		}
	}
	if (nOffset == nPacketLength - 4)
		return FALSE;		// didn't find sync

	// Decode the audio for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3 || !v->fAudioThumbnails)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, AACAudioDecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;

		WriteFile(v->hMPEGDecoderWritePipe[nES], &pPESPacket[nOffset], nPacketLength - nOffset, &dwWritten, NULL);
		if (dwWritten != (DWORD)nPacketLength - nOffset)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: DecodeAACAudioHeader: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
			OutputDebugString(szTemp);
		}
	}
	return fRetVal;
}
#endif LITE

BOOL DecodeMPEGAudioHeader(BYTE * pPESPacket, int nPacketLength, int nES)
{
	int syncword;
	int nOffset;
	BOOL fRetVal = FALSE;

	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		for (nOffset = 0; nOffset < nPacketLength - 4; nOffset++)
		{
			syncword = (pPESPacket[nOffset + 0]  << 8 | pPESPacket[nOffset + 1]);
			if ( ((syncword & 0xfff8) == 0xfff8) && (syncword != 0xffff) )
			{
				int layer = (pPESPacket[nOffset + 1] & 0x0006) >> 1;
				int ID = (pPESPacket[nOffset + 1] >> 7) & 0x01;
				int bitrate_index = (pPESPacket[nOffset + 2] & 0xf0) >> 4;
				int sampling_frequency = (pPESPacket[nOffset + 2] >> 2) & 0x03;
				int mode = (pPESPacket[nOffset + 3] & 0xc0) >> 6;
				
				if ( (layer != 0) && (sampling_frequency != 3) && (bitrate_index != 0) && (bitrate_index != 0x0f) && (ID == 1) )
				{
					PPARSEDMPEGAUDIO pMPEG = LocalAlloc(LPTR, sizeof(PARSEDMPEGAUDIO));
					if (pMPEG != NULL)
					{
						pMPEG->bitrate_index = bitrate_index;
						pMPEG->layer = layer;
						pMPEG->mode = mode;
						pMPEG->sampling_frequency = sampling_frequency;
						if (v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData)
							LocalFree(v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData);
						v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].pParsedData = (BYTE *)pMPEG;
					}
					break;
				}
			}
		}
	}

#ifndef LITE
	// Decode the audio for a thumbnail view
	if (v->nThumbnailProcessingThreadPriority == 3 || !v->fAudioThumbnails)
		return TRUE;	// disabled

	EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (v->fESParseDecoderStartedLibMPEG[nES] == FALSE)
	{
		DWORD dwThreadID;
		HANDLE hThread;
		
		v->fESParseDecoderStartedLibMPEG[nES] = TRUE;
		CreatePipe(&v->hMPEGDecoderReadPipe[nES], &v->hMPEGDecoderWritePipe[nES], NULL, v->nThumbnailPipeSize * 1024 * 1024);
		v->esparserinfo[nES].nProgramNumber = v->pat.pmt[v->nESParsePMTIndex[nES]].nProgramNumber;
		v->esparserinfo[nES].nES = nES;
		hThread = CreateThread(NULL, 0, MPEGAudioDecoderThread, (LPVOID)&v->esparserinfo[nES], 0, &dwThreadID);
		switch(v->nThumbnailProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(hThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(hThread, THREAD_PRIORITY_IDLE);
			break;
		}
		CloseHandle(hThread);
	}
	else
	{
		if (v->fESParseDecoderCompletedLibMPEG[nES] == TRUE)
			fRetVal = TRUE;
	}
	LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	if (!fRetVal)
	{
		DWORD dwWritten;

		WriteFile(v->hMPEGDecoderWritePipe[nES], pPESPacket, nPacketLength, &dwWritten, NULL);
		if (dwWritten != (DWORD)nPacketLength)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: DecodeMPEGAudioHeader: Bad data write to pipe (nPacketLength = %d dwWritten = %d)\n", nPacketLength, dwWritten);
			OutputDebugString(szTemp);
		}
	}
#else LITE
	fRetVal = TRUE;
#endif LITE
	
	return fRetVal;
}

void AutoRecordPIDs()
{
#ifndef LITE
	PostMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_RECORD_PID, 0);
#endif LITE

}

void AutoRecord()
{
#ifndef LITE
	int nPMTIndex;

	if (v->nAutoRecord == AUTO_RECORD_ALL)
	{
		SetTimer(v->hDlgSIParser, 3, 750, NULL);
		return;
	}

	for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
	{
		if (v->pat.pmt[nPMTIndex].nProgramNumber == v->nAutoRecordProgram)
		{
			HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);

			TreeView_SelectItem(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
			TreeView_SelectSetFirstVisible(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
			SetTimer(v->hDlgSIParser, 3, 750, NULL);
			return;
		}
	}
	OutputDebugString("TSReader: No auto record program found\n");
	v->nAutoRecord = AUTO_RECORD_NONE;
	v->nAutoRecordSeconds = 0;
	if (!v->fDontWarnAboutInccorectAutoRecordProgram)
		PostMessage(v->hWndMainWindow, WM_USER + 14, 0, 0);
#endif LITE
}

int __cdecl SortPMTCompareFunction(const void *elem1, const void *elem2)
{
	PPMT pPMT1 = (PPMT)elem1;
	PPMT pPMT2 = (PPMT)elem2;

	if (pPMT1->nProgramNumber < pPMT2->nProgramNumber)
		return -1;
	if (pPMT1->nProgramNumber > pPMT2->nProgramNumber)
		return 1;
	return 0;
}

void AddNumberFormat(char * szCFormatString, int nNumberFormat)
{
	lstrcat(szCFormatString, "%");
	if (nNumberFormat != -1)
	{
		char szTemp[16];
		wsprintf(szTemp, "0%d", nNumberFormat);
		lstrcat(szCFormatString, szTemp);
	}
	lstrcat(szCFormatString, "d");
}

void GetCurrentEITEvent(int nServiceID, char * szEITEventName)
{
	PEITEVENT pCurrent;
	SYSTEMTIME stSystemTime;

	EnterCriticalSection(&v->csEIT);
	pCurrent = v->pEvents[nServiceID];
	if (pCurrent != NULL)
	{
		do
		{
			FILETIME ftProgramStart, ftNow;
			DWORD64 lnProgramStart, lnProgramEnd, lnNow;
			DWORD64 lnMultiplier = 10000000;
			DWORD64 lnRunTime = ( (pCurrent->stRunTime.wHour * 60 * 60)
									   + (pCurrent->stRunTime.wMinute * 60)
									   + (pCurrent->stRunTime.wSecond) ) * lnMultiplier;

			GetSystemTime(&stSystemTime);
			SystemTimeToFileTime(&stSystemTime, &ftNow);
			memcpy(&lnNow, &ftNow, sizeof(DWORD64));

			SystemTimeToFileTime(&pCurrent->stStartTime, &ftProgramStart);
			memcpy(&lnProgramStart, &ftProgramStart, sizeof(DWORD64));				
			lnProgramEnd = lnProgramStart + lnRunTime;
			
			if (lnNow >= lnProgramStart && lnNow <= lnProgramEnd)
			{
				int i;

				lstrcpy(szEITEventName, pCurrent->szEventName);
				for (i = 0; i < lstrlen(szEITEventName); i++)
				{
					switch(szEITEventName[i])
					{
					case '/':
					case '\\':
					case ':':
					case '*':
					case '\"':
					case '<':
					case '>':
					case '|':
					case '?':
						szEITEventName[i] = ' ';
						break;
					}
				}
				LeaveCriticalSection(&v->csEIT);
				return;
			}

			pCurrent = (PEITEVENT)pCurrent->dwNextEvent;
		} while (pCurrent != NULL);
	}
	LeaveCriticalSection(&v->csEIT);
}

#define MAX_ARGUMENTS 20
void GenerateSplitFilename(char * szInputFileName, char * szOutputFileName, char * szFormatString, int nSplitFileNumber)
{
	int nArgumentIndex = 0;
	int nCurrentArgumentLength = 0;
	int nNumberFormat = -1;
	int i;
	int nTwoDigitYear = 0;
	BOOL fEscaped = FALSE;
	SYSTEMTIME stTime;
	char szCFormatString[MAX_PATH] = {0};
	char szFileName[MAX_PATH];
	char szExtension[128] = {0};
	char * szExtensionPtr;
	char szStringArguments[MAX_ARGUMENTS][128];
	char * szArgumentPtrs[MAX_ARGUMENTS];
	char szEITEventName[256] = {0};
	char szNull[] = {""};

#ifdef PRO
	EnterCriticalSection(&v->csActualRecordFilename);
#endif PRO
	lstrcpy(szFileName, szInputFileName);		
	szExtensionPtr = GetExtensionPtr(szFileName);
	if (szExtensionPtr != NULL)
	{
		*szExtensionPtr = 0;
		lstrcpy(szExtension, szExtensionPtr + 1);
	}

	for (i = 0; i < MAX_ARGUMENTS; i++)
		szArgumentPtrs[i] = szNull;
	
	// Generate mask and index variables
#ifndef PRO
	GetLocalTime(&stTime);
#else PRO
	if (v->fAdvancedRecordUTCTime)
		GetSystemTime(&stTime);
	else
		GetLocalTime(&stTime);
#endif PRO
	GetCurrentEITEvent(v->nRecordProgram, szEITEventName);
	do
	{
		if (fEscaped == FALSE)
		{
			if (*szFormatString == '%')
			{
				if (nCurrentArgumentLength)
				{
					szArgumentPtrs[nArgumentIndex] = szStringArguments[nArgumentIndex];
					szStringArguments[nArgumentIndex++][nCurrentArgumentLength++] = '\0';
					lstrcat(szCFormatString, "%s");
					nCurrentArgumentLength = 0;
				}
				fEscaped = TRUE;
			}
			else
				szStringArguments[nArgumentIndex][nCurrentArgumentLength++] = *szFormatString;
		}
		else
		{
			if (*szFormatString >= '1' && *szFormatString <= '9')
				nNumberFormat = *szFormatString - '0';
			else if (*szFormatString >= 'a' && *szFormatString <= 'z' || *szFormatString == '%')
			{
				char cArgument = *szFormatString;
				fEscaped = FALSE;

				switch(cArgument)
				{
				case 'f':		// filename
					szArgumentPtrs[nArgumentIndex] = szStringArguments[nArgumentIndex];
					lstrcpy(szStringArguments[nArgumentIndex++], szFileName);
					lstrcat(szCFormatString, "%s");
					break;
				case 'e':		// extension
					szArgumentPtrs[nArgumentIndex] = szStringArguments[nArgumentIndex];
					lstrcpy(szStringArguments[nArgumentIndex++], szExtension);
					lstrcat(szCFormatString, "%s");
					break;
				case 'n':		// file number
					szArgumentPtrs[nArgumentIndex++] = (char *)nSplitFileNumber;
					AddNumberFormat(szCFormatString, nNumberFormat);
					break;
				case 'y':		// year
					if (nNumberFormat == 2)
					{
						char szTemp[16];
						wsprintf(szTemp, "%04d", stTime.wYear);
						sscanf(&szTemp[2], "%d", &nTwoDigitYear);
						szArgumentPtrs[nArgumentIndex++] = (char *)nTwoDigitYear;
						AddNumberFormat(szCFormatString, nNumberFormat);
					}
					else
					{
						szArgumentPtrs[nArgumentIndex++] = (char *)stTime.wYear;
						AddNumberFormat(szCFormatString, nNumberFormat);
					}
					break;
				case 'm':		// month
					szArgumentPtrs[nArgumentIndex++] = (char *)stTime.wMonth;
					AddNumberFormat(szCFormatString, nNumberFormat);
					break;
				case 'd':		// day
					szArgumentPtrs[nArgumentIndex++] = (char *)stTime.wDay;
					AddNumberFormat(szCFormatString, nNumberFormat);
					break;
				case 'h':		// hour
					szArgumentPtrs[nArgumentIndex++] = (char *)stTime.wHour;
					AddNumberFormat(szCFormatString, nNumberFormat);
					break;
				case 'i':		// minute
					szArgumentPtrs[nArgumentIndex++] = (char *)stTime.wMinute;
					AddNumberFormat(szCFormatString, nNumberFormat);
					break;
				case 's':		// second
					szArgumentPtrs[nArgumentIndex++] = (char *)stTime.wSecond;
					AddNumberFormat(szCFormatString, nNumberFormat);
					break;
				case '%':		// percent sign
					szArgumentPtrs[nArgumentIndex] = szStringArguments[nArgumentIndex];
					lstrcpy(szStringArguments[nArgumentIndex++], "%");
					lstrcat(szCFormatString, "%s");
					break;
				case 'v':		// event name
					szArgumentPtrs[nArgumentIndex] = szStringArguments[nArgumentIndex];
					lstrcpy(szStringArguments[nArgumentIndex++], szEITEventName);
					lstrcat(szCFormatString, "%s");
					break;
				default:
					wsprintf(szOutputFileName, "Invalid argument '%c'", cArgument);
					return;
				}
				nNumberFormat = -1;
			}
			else
			{
				lstrcpy(szOutputFileName, "Invalid format mask");
				return;
			}
		}
	} while (*szFormatString++ != '\0' && nArgumentIndex < MAX_ARGUMENTS);

	// Now we can generate the filename
	wsprintf(szOutputFileName, szCFormatString,
		     szArgumentPtrs[0], szArgumentPtrs[1], szArgumentPtrs[2], szArgumentPtrs[3],
		     szArgumentPtrs[4], szArgumentPtrs[5], szArgumentPtrs[6], szArgumentPtrs[7],
		     szArgumentPtrs[8], szArgumentPtrs[9], szArgumentPtrs[10], szArgumentPtrs[11],
		     szArgumentPtrs[12], szArgumentPtrs[13], szArgumentPtrs[14], szArgumentPtrs[15],
			 szArgumentPtrs[16], szArgumentPtrs[17], szArgumentPtrs[18], szArgumentPtrs[19]);
#ifdef PRO
	LeaveCriticalSection(&v->csActualRecordFilename);
#endif PRO
}

void UpdateSDTContinuity()
{
	int nCurrentContinuity = v->out_sdt[3] & 0x0f;
	nCurrentContinuity++;
	nCurrentContinuity &= 0x0f;
	v->out_sdt[3] &= 0xf0;
	v->out_sdt[3] |= nCurrentContinuity;
}

void UpdatePATContinuity()
{
	int nCurrentContinuity = v->out_pat[3] & 0x0f;
	nCurrentContinuity++;
	nCurrentContinuity &= 0x0f;
	v->out_pat[3] &= 0xf0;
	v->out_pat[3] |= nCurrentContinuity;
}

void UpdatePMTContinuity()
{
	int nPacketCount;

	for (nPacketCount = 0; nPacketCount < v->nOutputPMTPackets; nPacketCount++)
	{
		int nCurrentContinuity = v->out_pmt[3 + (188 * nPacketCount)] & 0x0f;
		nCurrentContinuity++;
		nCurrentContinuity &= 0x0f;
		v->out_pmt[3 + (188 * nPacketCount)] &= 0xf0;
		v->out_pmt[3 + (188 * nPacketCount)] |= nCurrentContinuity;
	}
}

void WritePATandPMTandSDT()
{
	DWORD dwWritten;
	DWORD dwTotalWritten = 0;

	WriteFile(v->hRecordFile, v->out_pat, 188, &dwWritten, NULL);
	dwTotalWritten += dwWritten;
	v->dTotalRecorded += (double)dwWritten;
	v->dThisFileRecorded += (double)dwWritten;
	UpdatePATContinuity();
	
	WriteFile(v->hRecordFile, v->out_pmt, 188 * v->nOutputPMTPackets, &dwWritten, NULL);
	dwTotalWritten += dwWritten;
	v->dTotalRecorded += (double)dwWritten;
	v->dThisFileRecorded += (double)dwWritten;
	UpdatePMTContinuity();

	if (    ATSCPIDs() == FALSE
		 && v->nNetworkPID == 0x0010 
		 && v->nRecordVideoPID != 0x0010 
		 && v->nStreamTo != STREAM_TO_STRADIS)
	{
		WriteFile(v->hRecordFile, v->out_sdt, 188, &dwWritten, NULL);
		dwTotalWritten += dwWritten;
		v->dTotalRecorded += (double)dwWritten;
		v->dThisFileRecorded += (double)dwWritten;
		UpdateSDTContinuity();
	}

	if (v->fStradisActive == TRUE)
	{
		EnterCriticalSection(&v->csPipeBytes);
		v->nPipeBytes += dwTotalWritten;
		LeaveCriticalSection(&v->csPipeBytes);
	}
}

BOOL CALLBACK PSWarningDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		MessageBeep(0);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_NO_PS_WARNING:
			v->fDisablePSWarning = IsDlgButtonChecked(hDlg, IDC_NO_PS_WARNING);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

void UpdateFilenameFormatPreview(HWND hDlg)
{
	char szFormatString[MAX_PATH];
	char szPreview[MAX_PATH];

	GetDlgItemText(hDlg, IDC_FORMAT_STRING, szFormatString, sizeof(szFormatString));
	GenerateSplitFilename("recording.ts", szPreview, szFormatString, 1);
	SetDlgItemText(hDlg, IDC_FORMAT_SAMPLE, szPreview);
}

BOOL CALLBACK FileNameFormatDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szNewTitle[256];
			char szCurrentTitle[256];

			wsprintf(szNewTitle, "%s ", gszAppName);
			GetDlgItemText(hDlg, IDC_SPLIT_INFO_CAPTION, szCurrentTitle, sizeof(szCurrentTitle));
			lstrcat(szNewTitle, szCurrentTitle);
			SetDlgItemText(hDlg, IDC_FORMAT_STRING, v->szSplitFormatString);
			//UpdateFilenameFormatPreview(hDlg);
			SendDlgItemMessage(hDlg, IDC_FORMAT_STRING, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hDlg, IDC_FORMAT_STRING));
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case BN_CLICKED:
			switch(LOWORD(wParam))
			{
			case IDOK:
				GetDlgItemText(hDlg, IDC_FORMAT_STRING, v->szSplitFormatString, sizeof(v->szSplitFormatString));
				EndDialog(hDlg, TRUE);
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			}
			break;
		case EN_CHANGE:
			if (LOWORD(wParam) == IDC_FORMAT_STRING)
				UpdateFilenameFormatPreview(hDlg);
			break;
		}
		break;
	}

	return FALSE;
}

BOOL LoadPIDList(HWND hDlg, char * szInputFile)
{
	int nItemCount = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETCOUNT, 0, 0);
	HANDLE hInputFile = CreateFile(szInputFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hInputFile == INVALID_HANDLE_VALUE)
		return FALSE;

	SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_SETSEL, FALSE, (LPARAM)-1);
	do
	{
		int nSelectPID;
		int nItem;
		char szInputLine[128];

		if (SourceHelper_ReadLine(hInputFile, szInputLine, sizeof(szInputLine)) == 0)
			break;
		sscanf(szInputLine, "%x", &nSelectPID);
		for (nItem = 0; nItem < nItemCount; nItem++)
		{
			int nListPID;
			char szListPID[32];

			SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETTEXT, nItem, (LPARAM)szListPID);
			if (v->fDecimalPIDs == FALSE)
				sscanf(&szListPID[2], "%x", &nListPID);
			else
				sscanf(szListPID, "%d", &nListPID);
			if (nListPID == nSelectPID)
				SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_SETSEL, TRUE, nItem);
		}
	} while (TRUE);
	CloseHandle(hInputFile);

	return TRUE;
}

void LoadPIDListDialog(HWND hDlg)
{
	OPENFILENAME ofn;
	char szInputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szInputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("PID List Files (*.pdl)\0*.pdl\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Load PID List");
	ofn.lpstrDefExt = TEXT("pdl");
	ofn.Flags =  OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = v->szRecordPIDFolder;				
	if (SourceHelper_myGetOpenFileName(&ofn) == TRUE)
	{
		if (LoadPIDList(hDlg, szInputFile) == FALSE)
			MessageBox(hDlg, "Unable to open PID list file", gszAppName, MB_ICONSTOP);		
	}
}

BOOL SavePIDList(HWND hDlg, char * szOutputFile)
{
	int nItemList[8192];	// in theory, this is possible!
	int nRetVal;
	int i;
	HANDLE hOutputFile = CreateFile(szOutputFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);

	if (hOutputFile == INVALID_HANDLE_VALUE)
		return FALSE;

	nRetVal = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETSELITEMS, (WPARAM)sizeof(nItemList) / sizeof(int), (LPARAM)&nItemList[0]);
	for (i = 0; i < nRetVal; i++)
	{
		int nPID;
		DWORD dwWritten;
		char szPID[32];
		char szTemp[32];
		
		SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETTEXT, (WPARAM)nItemList[i], (LPARAM)szPID);
		if (v->fDecimalPIDs == FALSE)
			sscanf(&szPID[2], "%x", &nPID);
		else
			sscanf(szPID, "%d", &nPID);
		wsprintf(szTemp, "%x\r\n", nPID);
		WriteFile(hOutputFile, szTemp, lstrlen(szTemp), &dwWritten, NULL);
	}
	CloseHandle(hOutputFile);
	return TRUE;	
}

void SavePIDListDialog(HWND hDlg)
{
	OPENFILENAME ofn;
	char szOutputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szOutputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("PID List Files (*.pdl)\0*.pdl\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Save PID List");
	ofn.lpstrDefExt = TEXT("pdl");
	ofn.lpstrInitialDir = v->szRecordPIDFolder;
	ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	ofn.hInstance = v->hInstance;						
	if (myGetSaveFileName(&ofn) == TRUE)
	{
		if (SavePIDList(hDlg, szOutputFile) == FALSE)
			MessageBox(hDlg, "Unable to save PID list file", gszAppName, MB_ICONSTOP);		
	}
}

#ifdef PRO
BOOL LoadTableList(HWND hDlg, char * szInputFile)
{
	int nItemIndex;
	HANDLE hInputFile = CreateFile(szInputFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hInputFile == INVALID_HANDLE_VALUE)
		return FALSE;

	// Remove old items and clear out memory
	while (ListView_DeleteItem(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST), 0) == TRUE)
		;
	for (nItemIndex = 0; nItemIndex < MAX_RECORD_TABLES - 1; nItemIndex++)
		v->record_tables[nItemIndex].nPID = -1;
	nItemIndex = 0;

	do
	{
		BOOL fHex = FALSE;
		LV_ITEM lvi; 
		char * szTab1, * szTab2;
		char szInputLine[256];

		if (SourceHelper_ReadLine(hInputFile, szInputLine, sizeof(szInputLine)) == 0)
			break;
		szTab1 = strstr(szInputLine, "\t");
		if (szTab1 == NULL)
			continue;
		*szTab1 = '\0';
		szTab1++;
		szTab2 = strstr(szTab1, "\t");
		if (szTab2 == NULL)
			continue;
		*szTab2 = '\0';
		szTab2++;

		if (memcmp(szInputLine, "0x", 2) == 0)
			fHex = TRUE;
		if (fHex)
		{
			sscanf(&szInputLine[2], "%x", &v->record_tables[nItemIndex].nPID);
			sscanf(&szTab1[2], "%x", &v->record_tables[nItemIndex].nStartTable);
			sscanf(&szTab2[2], "%x", &v->record_tables[nItemIndex].nEndTable);
		}
		else
		{
			sscanf(szInputLine, "%d", &v->record_tables[nItemIndex].nPID);
			sscanf(szTab1, "%d", &v->record_tables[nItemIndex].nStartTable);
			sscanf(szTab2, "%d", &v->record_tables[nItemIndex].nEndTable);
		}

		memset(&lvi, 0, sizeof(lvi));
		lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
		lvi.iItem = nItemIndex; 
		ListView_InsertItem(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST), &lvi);
		nItemIndex++;
	} while (TRUE);
	CloseHandle(hInputFile);

	return TRUE;
}

void LoadTableListDialog(HWND hDlg)
{
	OPENFILENAME ofn;
	char szInputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szInputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Table List Files (*.tbl)\0*.tbl\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Load Table List");
	ofn.lpstrDefExt = TEXT("tbl");
	ofn.Flags =  OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = v->szRecordPIDFolder;				
	if (SourceHelper_myGetOpenFileName(&ofn) == TRUE)
	{
		if (LoadTableList(hDlg, szInputFile) == FALSE)
			MessageBox(hDlg, "Unable to open Table list file", gszAppName, MB_ICONSTOP);		
	}
}

BOOL SaveTableList(HWND hDlg, char * szOutputFile)
{
	HANDLE hOutputFile = CreateFile(szOutputFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	int nItemIndex;

	if (hOutputFile == INVALID_HANDLE_VALUE)
		return FALSE;

	for (nItemIndex = 0; nItemIndex < MAX_RECORD_TABLES; nItemIndex++)
	{
		DWORD dwWritten;
		char szTemp[256];

		if (v->record_tables[nItemIndex].nPID == -1)
			continue;

		if (v->fDecimalPIDs)
			wsprintf(szTemp, "%d\t%d\t%d\r\n", v->record_tables[nItemIndex].nPID, v->record_tables[nItemIndex].nStartTable, v->record_tables[nItemIndex].nEndTable);
		else
			wsprintf(szTemp, "0x%04x\t0x%02x\t0x%02x\r\n", v->record_tables[nItemIndex].nPID, v->record_tables[nItemIndex].nStartTable, v->record_tables[nItemIndex].nEndTable);
		WriteFile(hOutputFile, szTemp, lstrlen(szTemp), &dwWritten, NULL);
	}
	CloseHandle(hOutputFile);
	return TRUE;	
}

void SaveTableListDialog(HWND hDlg)
{
	OPENFILENAME ofn;
	char szOutputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szOutputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Table List Files (*.tbl)\0*.tbl\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Save Table List");
	ofn.lpstrDefExt = TEXT("tbl");
	ofn.lpstrInitialDir = v->szRecordPIDFolder;
	ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	ofn.hInstance = v->hInstance;						
	if (myGetSaveFileName(&ofn) == TRUE)
	{
		if (SaveTableList(hDlg, szOutputFile) == FALSE)
			MessageBox(hDlg, "Unable to save Table list file", gszAppName, MB_ICONSTOP);		
	}
}

BOOL CALLBACK RecordAdvancedFileDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_ADVANCED_RECORD_REMOVE_OLD, v->fAdvancedRecordRemoveOldEnabled);
		CheckDlgButton(hDlg, IDC_ADVANCED_RECORD_UTC, v->fAdvancedRecordUTCTime);
		SetDlgItemInt(hDlg, IDC_ADVANCED_RECORD_REMOVE_OLD_SIZE, v->nAdvancedRecordRemoveOldLimitGB, FALSE);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nNewLimit = GetDlgItemInt(hDlg, IDC_ADVANCED_RECORD_REMOVE_OLD_SIZE, NULL, FALSE);
				if (nNewLimit == 0)
				{
					MessageBox(hDlg, "Limit must be non-zero", gszAppName, MB_ICONWARNING);
					SetFocus(GetDlgItem(hDlg, IDC_ADVANCED_RECORD_REMOVE_OLD_SIZE));
					break;
				}
				v->nAdvancedRecordRemoveOldLimitGB = nNewLimit;
				v->fAdvancedRecordRemoveOldEnabled = IsDlgButtonChecked(hDlg, IDC_ADVANCED_RECORD_REMOVE_OLD);
				v->fAdvancedRecordUTCTime = IsDlgButtonChecked(hDlg, IDC_ADVANCED_RECORD_UTC);
				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK RecordAdvancedMuxDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int i;

			CheckDlgButton(hDlg, IDC_ADVANCED_MUX_PID_DROP_ENABLE, v->fAdvancedRecordDropPID);

			for (i = 0; i < 8191; i++)
			{
				if (v->pc[i].lnPackets)
				{
					int nIndex;
					int j = 0;
					char szPID[64];

					wsprintf(szPID, "%s", FormatTooltipPID(v->pc[i].nPID));			
					nIndex = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_ADDSTRING, 0, (LPARAM)szPID);
					if (v->bAdvancedDropPID[v->pc[i].nPID])
						SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_SETSEL, TRUE, nIndex);
				}
			}
			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nItemList[8192];	// in theory, this is possible!
				int nRetVal;
				int i;

				v->fAdvancedRecordDropPID = IsDlgButtonChecked(hDlg, IDC_ADVANCED_MUX_PID_DROP_ENABLE);
				for (i = 0; i < 8192; i++)
					v->bAdvancedDropPID[i] = FALSE;

				nRetVal = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETSELITEMS, (WPARAM)sizeof(nItemList) / sizeof(int), (LPARAM)&nItemList[0]);
				if (nRetVal)
				{
					for (i = 0; i < nRetVal; i++)
					{
						int nPID;
						char szPID[32];
						
						SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETTEXT, (WPARAM)nItemList[i], (LPARAM)szPID);
						if (v->fDecimalPIDs == FALSE)
							sscanf(&szPID[2], "%x", &nPID);
						else
							sscanf(szPID, "%d", &nPID);
						v->bAdvancedDropPID[nPID] = TRUE;
					}
				}
				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_RECORD_PID_LOAD:
			LoadPIDListDialog(hDlg);
			break;
		case IDC_RECORD_PID_SAVE:
			SavePIDListDialog(hDlg);
			break;
		}
		break;
	}
	return FALSE;
}
#endif PRO

BOOL CALLBACK RecordDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		v->fStradisAutostart = FALSE;
		break;
	case WM_INITDIALOG:
		{
			int i;
			int nVideoStreams = 0;

			v->nAudioStreams = 0;
			v->nOtherStreams = 0;

			if (v->fRecordDialogStreamOnly == FALSE)
				SetDlgItemText(hDlg, IDC_RECORD_FILENAME_SAMPLE, "");

			if (!v->fRecordDialogStreamOnly)
			{
				if (v->nAutoRecord != AUTO_RECORD_NONE && v->nAutoRecord != AUTO_RECORD_VLC && v->nAutoRecord != AUTO_RECORD_VLC_TIME_LIMITED)
					lstrcpy(v->szRecordFile, v->szAutoRecordFile);
				SetDlgItemText(hDlg, IDC_RECORD_FILE, v->szRecordFile);
				CheckDlgButton(hDlg, IDC_RECORD_NO_NULLS, v->fDiscardNULLPIDs);
				CheckDlgButton(hDlg, IDC_RECORD_SPLIT, v->fSplitRecord);
				if (v->fSplitSeconds)
					CheckDlgButton(hDlg, IDC_RECORD_SEC_LIMIT, TRUE);
				else
					CheckDlgButton(hDlg, IDC_RECORD_MB_LIMIT, TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_SPLIT_SIZE), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_MB_LIMIT), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_SEC_LIMIT), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_SPLIT_CAPTION), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_FILENAME_SAMPLE), v->fSplitRecord);
				SetDlgItemInt(hDlg, IDC_RECORD_SPLIT_SIZE, v->nSplitFileSize, FALSE);
				SendDlgItemMessage(hDlg, IDC_RECORD_TITLE, EM_LIMITTEXT, sizeof(v->szRecordTitle) - 1, 0);
				SetDlgItemText(hDlg, IDC_RECORD_TITLE, v->szRecordTitle);
				CheckDlgButton(hDlg, IDC_ATSC_MODE, v->fATSCRecordMode);
				if (v->fAudioPMTETSI)
					CheckDlgButton(hDlg, IDC_RECORD_PMT_ETSI, BST_CHECKED);
				else
					CheckDlgButton(hDlg, IDC_RECORD_PMT_DEFACTO, BST_CHECKED);
			}
#ifdef LITE
			EnableWindow(GetDlgItem(hDlg, IDC_RECORD_LIMIT), FALSE);
#endif LITE

#ifndef LITE
			if (v->fRecordLimit && !v->fRecordDialogStreamOnly)
			{
				CheckDlgButton(hDlg, IDC_RECORD_LIMIT, BST_CHECKED);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_LIMIT_SIZE), TRUE);
				SetDlgItemInt(hDlg, IDC_RECORD_LIMIT_SIZE, v->nAutoRecordSeconds, FALSE);
				if (v->nAutoRecordSeconds == 0 && v->nAutoRecordSecondsReload)
					SetDlgItemInt(hDlg, IDC_RECORD_LIMIT_SIZE, v->nAutoRecordSecondsReload, FALSE);
			}

			if (v->fRecordAllTS == TRUE)
			{
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_VIDEO_ES), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_AUDIO_ES), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_VIDEO_ES_CAPTION), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_AUDIO_ES_CAPTION), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_NO_NULLS), TRUE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TITLE), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TITLE_CAPTION), FALSE);				
				EnableWindow(GetDlgItem(hDlg, IDC_ATSC_MODE), FALSE);								
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_PMT_DEFACTO), FALSE);								
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_PMT_ETSI), FALSE);													
#ifdef PRO
				ShowWindow(GetDlgItem(hDlg, IDC_RECORD_ADVANCED_FILE), SW_SHOW);
				ShowWindow(GetDlgItem(hDlg, IDC_RECORD_ADVANCED_MUX), SW_SHOW);
#endif PRO
			}
			else
#endif LITE
			{
				if (v->fRecordDialogStreamOnly == FALSE)
				{
#ifndef LITE
					CheckDlgButton(hDlg, IDC_INCLUDE_CA, v->fIncludeCAData);
					if (v->fRecordProgramStream)
					{
						EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TITLE), FALSE);
						EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TITLE_CAPTION), FALSE);				
						CheckDlgButton(hDlg, IDC_RECORD_PS, BST_CHECKED);
					}
					else
#endif LITE
						CheckDlgButton(hDlg, IDC_RECORD_TS, BST_CHECKED);
					EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TS), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDC_RECORD_PS), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDC_RECORD_STREAM_CAPTION), TRUE);
#ifdef LITE
					ShowWindow(GetDlgItem(hDlg, IDC_INCLUDE_CA), SW_HIDE);
#endif LITE
				}
				if (v->pat.hPATTreeItem != NULL || GetTotalPMTChannels())
				{
					for (i = 0; i < MAX_ESLIST_ENTRIES; i++)
					{
						char szStreamTypeEnglish[64];
						char szTemp[200], szMask[200];
						int nItem;

						if (v->pat.pmt[v->nSelectedProgram].es[i].nESPID == 0)
							break;
						DecodeStreamType(v->pat.pmt[v->nSelectedProgram].es[i].nStreamType, szStreamTypeEnglish, v->nSelectedProgram, i);
						switch(v->pat.pmt[v->nSelectedProgram].es[i].nStreamType)
						{
						case 0x01:	// MPEG-1 video
						case 0x02:	// MPEG-2 video
						case 0x10:	// MPEG-4 video
						case 0x1b:	// H264 video
						case 0x80:	// DCII video
						case 0xea:	// WMV9 video
							if (v->pat.pmt[v->nSelectedProgram].es[i].nStreamType == 0x80
								&& v->nNetworkPID == 0x0010)
								goto Record_BuildESListDefault;
							wsprintf(szMask, "PID %s - %%s", v->szOutputPIDFlags);
							wsprintf(szTemp, szMask, v->pat.pmt[v->nSelectedProgram].es[i].nESPID, szStreamTypeEnglish);
							nItem = SendDlgItemMessage(hDlg, IDC_RECORD_VIDEO_ES, LB_ADDSTRING, 0, (LPARAM)szTemp);
							SendDlgItemMessage(hDlg, IDC_RECORD_VIDEO_ES, LB_SETITEMDATA, nItem, i);
							nVideoStreams++;
							break;
						case 0x03:	// MPEG-1 audio
						case 0x04:	// MPEG-2 audio
						case 0x06:	// maybe AC3
						case 0x0f:	// MPEG-2 AAC audio
						case 0x11:	// MPEG-4 AAC audio
						case 0x81:	// AC3 audio
						case 0x83:	// LPCM audio
						case 0x85:	// DTS audio
						case 0xe6:	// WMA9 audio
							{
								char szLanguage[16] = {0};

								if (v->pat.pmt[v->nSelectedProgram].es[i].nStreamType == 0x06)
								{
									if (   IsAC3AudioStream(v->nSelectedProgram, i) == FALSE
										&& IsPCMAudioStream(v->nSelectedProgram, i) == FALSE
										&& IsDTSAudioStream(v->nSelectedProgram, i) == FALSE)
										goto Record_BuildESListDefault;
								}
								if (v->fAllowStradisAC3 == FALSE && IsAC3AudioStream(v->nSelectedProgram, i) == TRUE)
								{
									if (   (v->fRecordDialogStreamOnly == TRUE)
										&& (v->nStreamTo != STREAM_TO_DVHS)
										&& (v->nStreamTo != STREAM_TO_VLC)
										&& (v->nStreamTo != STREAM_TO_XNS)
										&& (v->nStreamTo != STREAM_TO_ROKU)
										&& (v->pat.pmt[v->nSelectedProgram].es[i].nStreamType == 0x81) )
										goto Record_BuildESListDefault; // AC3 audio doesn't work on Stradis cards
								}
								wsprintf(szMask, "PID %s - %%s", v->szOutputPIDFlags);
								wsprintf(szTemp, szMask, v->pat.pmt[v->nSelectedProgram].es[i].nESPID, szStreamTypeEnglish);

								GetLanguageFromDescriptor(szLanguage, v->nSelectedProgram, i);
								if (lstrlen(szLanguage))
								{
									char szTemp2[16];
									wsprintf(szTemp2, " (%s)", szLanguage);
									lstrcat(szTemp, szTemp2);
								}
								nItem = SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_ADDSTRING, 0, (LPARAM)szTemp);
								SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETITEMDATA, nItem, i);
								if (v->nAutoRecord != AUTO_RECORD_NONE)
								{
									if (v->nAutoRecordAudioTrack == 0)
										SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETSEL, TRUE, nItem);
								}
								v->nAudioStreams++;
							}
							break;
						default:
Record_BuildESListDefault:
							wsprintf(szMask, "PID %s - %%s", v->szOutputPIDFlags);
							wsprintf(szTemp, szMask, v->pat.pmt[v->nSelectedProgram].es[i].nESPID, szStreamTypeEnglish);
							nItem = SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_ADDSTRING, 0, (LPARAM)szTemp);
							SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_SETITEMDATA, nItem, i);
							v->nOtherStreams++;
							if (v->fAutoRecordSubtitles == TRUE)
							{
								if (IsSubtitleStream(v->nSelectedProgram, i) == TRUE)
									SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_SETSEL, TRUE, nItem);
								else if (IsTeleTextOrVBIStream(v->nSelectedProgram, i) == TRUE)
									SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_SETSEL, TRUE, nItem);									
							}

							break;
						}
					}
				}
				else
				{
					MessageBox(hDlg, "No channels are defined. Use manual channels to define channels and then use record", gszAppName, MB_ICONSTOP);
					EndDialog(hDlg, FALSE);
					break;
				}
				SendDlgItemMessage(hDlg, IDC_RECORD_VIDEO_ES, LB_SETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETSEL, TRUE, 0);
			}
			v->fAutoRecordSubtitles = FALSE;
			if (v->nAutoRecord != AUTO_RECORD_NONE)
			{
				if (v->nAutoRecordAudioTrack == -1)
				{
					for (i = 0; i < v->nAudioStreams; i++)
						SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETSEL, TRUE, i);
					for (i = 0; i < v->nOtherStreams; i++)
						SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_SETSEL, TRUE, i);
				}
				else
				{
					if (v->nAutoRecordAudioTrack != 0)
					{
						for (i = 0; i < v->nAudioStreams; i++)
							SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETSEL, i == v->nAutoRecordAudioTrack - 1, i);
					}
				}
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				break;
			}
			if (v->fStradisAutostart == TRUE)
			{
				// Just changing channels
				for (i = 0; i < v->nAudioStreams; i++)
					SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETSEL, TRUE, i);
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				break;
			}
			if (v->fRecordDialogStreamOnly == TRUE && v->nOtherStreams == 0)
			{
				if ( (v->nAudioStreams == 1) && (nVideoStreams == 0) )
					PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				else if ( (v->nAudioStreams == 0) && (nVideoStreams == 1) )
					PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				else if ( (v->nAudioStreams == 1) && (nVideoStreams == 1) )
					PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				SetFocus(GetDlgItem(hDlg, IDOK));
			}	
			else
			{
				SendDlgItemMessage(hDlg, IDC_RECORD_FILE, EM_SETSEL, 0, -1);
				SetFocus(GetDlgItem(hDlg, IDOK));
			}
			break;
		}
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case EN_CHANGE:
			switch(LOWORD(wParam))
			{
			case IDC_RECORD_FILE:
				{
					char szPreview[MAX_PATH];
					char szFileName[MAX_PATH];

					GetDlgItemText(hDlg, IDC_RECORD_FILE, szFileName, sizeof(szFileName));
					GenerateSplitFilename(szFileName, szPreview, v->szSplitFormatString, 1);
					SetDlgItemText(hDlg, IDC_RECORD_FILENAME_SAMPLE, szPreview);
				}
				break;
			}
			break;
		case BN_CLICKED:			
			switch(LOWORD(wParam))
			{
#ifdef PRO
			case IDC_RECORD_ADVANCED_FILE:
				DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_RECORD_ADVANCED_FILE), hDlg, RecordAdvancedFileDlgProc);
				break;
			case IDC_RECORD_ADVANCED_MUX:
				DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_RECORD_ADVANCED_MUX), hDlg, RecordAdvancedMuxDlgProc);
				break;
#endif PRO
			case IDC_RECORD_ALL_AUDIO:
				{
					int i;
					for (i = 0; i < v->nAudioStreams; i++)
						SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_SETSEL, TRUE, i);
				}
				break;
			case IDC_RECORD_ALL_OTHER:
				{
					int i;
					for (i = 0; i < v->nOtherStreams; i++)
						SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_SETSEL, TRUE, i);
				}
				break;
#ifndef LITE
			case IDC_RECORD_LIMIT:
				v->fRecordLimit = IsDlgButtonChecked(hDlg, IDC_RECORD_LIMIT);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_LIMIT_SIZE), v->fRecordLimit);
				break;
#endif LITE
			case IDC_ATSC_MODE:
				v->fATSCRecordMode = IsDlgButtonChecked(hDlg, IDC_ATSC_MODE);
				break;
			case IDC_RECORD_MB_LIMIT:
			case IDC_RECORD_SEC_LIMIT:
				v->fSplitSeconds = IsDlgButtonChecked(hDlg, IDC_RECORD_SEC_LIMIT);
				break;
			case IDOK:
				{
					if (v->fRecordAllTS == FALSE)
					{
						int i;
						int nItemIndex;
						
						for (i = 0; i < MAX_AUDIO_STREAMS; i++)
							v->nRecordAudioESIndex[i] = -1;
						i = 0;
						for (nItemIndex = 0; nItemIndex < v->nAudioStreams; nItemIndex++)
						{
							int nChecked = SendDlgItemMessage(hDlg, IDC_RECORD_AUDIO_ES, LB_GETSEL, nItemIndex, 0);
							if (nChecked)
							{
								v->nRecordAudioESIndex[i++] = SendDlgItemMessage(hDlg,
																		IDC_RECORD_AUDIO_ES,
																		LB_GETITEMDATA,
																		nItemIndex,
																		0);
								if (i == MAX_AUDIO_STREAMS)
									break;
							}
						}

						for (i = 0; i < MAX_OTHER_STREAMS; i++)
							v->nRecordOtherESIndex[i] = -1;
						i = 0;
						for (nItemIndex = 0; nItemIndex < v->nOtherStreams; nItemIndex++)
						{
							int nChecked = SendDlgItemMessage(hDlg, IDC_RECORD_OTHER_ES, LB_GETSEL, nItemIndex, 0);
							if (nChecked)
							{
								v->nRecordOtherESIndex[i++] = SendDlgItemMessage(hDlg,
																		IDC_RECORD_OTHER_ES,
																		LB_GETITEMDATA,
																		nItemIndex,
																		0);
								if (i == MAX_OTHER_STREAMS)
									break;
							}
						}

						v->nRecordVideoESIndex = -1;
						v->nRecordVideoESIndex = SendDlgItemMessage(hDlg,
																IDC_RECORD_VIDEO_ES,
																LB_GETITEMDATA,
																SendDlgItemMessage(hDlg, IDC_RECORD_VIDEO_ES, LB_GETCURSEL, 0, 0),
																0);
						v->fAudioPMTETSI = IsDlgButtonChecked(hDlg, IDC_RECORD_PMT_ETSI);
					}
#ifndef LITE
					else
						v->fDiscardNULLPIDs = IsDlgButtonChecked(hDlg, IDC_RECORD_NO_NULLS);
					//if (v->fRecordLimit && v->nAutoRecord == AUTO_RECORD_NONE && !v->fRecordDialogStreamOnly)
					if (v->fRecordLimit && !v->fRecordDialogStreamOnly)
					{
						v->nAutoRecordSeconds = GetDlgItemInt(hDlg, IDC_RECORD_LIMIT_SIZE, NULL, FALSE);
						if (v->nAutoRecordSeconds <= 0)
						{
							MessageBox(hDlg, "Please provide a valid record time limit", gszAppName, MB_ICONSTOP);
							SetFocus(GetDlgItem(hDlg, IDC_RECORD_LIMIT_SIZE));
							break;
						}
					}
					else if (v->nAutoRecord == AUTO_RECORD_NONE)						
						v->nAutoRecordSeconds = 0;
					v->fIncludeCAData = IsDlgButtonChecked(hDlg, IDC_INCLUDE_CA);
#endif LITE

					if (!v->fRecordDialogStreamOnly)
					{
						GetDlgItemText(hDlg, IDC_RECORD_FILE, v->szRecordFile, sizeof(v->szRecordFile));
						lstrcpy(v->szActualRecordFile, v->szRecordFile);
						if (v->fSplitRecord == TRUE)
						{
							v->nSplitFileSize = GetDlgItemInt(hDlg, IDC_RECORD_SPLIT_SIZE, NULL, FALSE);
							if (v->nSplitFileSize == 0)
							{
								MessageBox(hDlg, "Invalid split file size - please correct before continuing", gszAppName, MB_ICONSTOP);
								SetFocus(GetDlgItem(hDlg, IDC_RECORD_SPLIT_SIZE));
								break;
							}
							v->nSplitFileNumber = 1;
							v->lnRecordSplitPCR = -1;
							GenerateSplitFilename(v->szRecordFile, v->szActualRecordFile, v->szSplitFormatString, v->nSplitFileNumber);
						}
						if (v->fWarnBeforeOverwritingRecordings)
						{
							HANDLE hRecordFile;
							hRecordFile = CreateFile(v->szActualRecordFile,
											  GENERIC_READ,
											  FILE_SHARE_READ,
											  (LPSECURITY_ATTRIBUTES) NULL,
											  OPEN_EXISTING,
											  FILE_ATTRIBUTE_NORMAL,
											  (HANDLE) NULL);
							if (hRecordFile != INVALID_HANDLE_VALUE)
							{
								CloseHandle(hRecordFile);
								if (MessageBox(hDlg, "Record file already exists. Sure you want to overwrite?", gszAppName, MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDNO)
									break;
							}
						}
						v->hRecordFile = CreateFile(v->szActualRecordFile,
											  GENERIC_WRITE,
											  FILE_SHARE_READ,
											  (LPSECURITY_ATTRIBUTES) NULL,
											  CREATE_ALWAYS,
											  FILE_ATTRIBUTE_NORMAL,
											  (HANDLE) NULL);
						if (v->hRecordFile == INVALID_HANDLE_VALUE)
						{
							char szTemp[128 + MAX_PATH];

							wsprintf(szTemp, "Unable to open file %s", v->szActualRecordFile);
							MessageBox(hDlg, szTemp, gszAppName, MB_OK | MB_ICONWARNING);
							break;
						}
						else
						{
							char szTemp[128 + MAX_PATH];
							wsprintf(szTemp, "Recording to %s", v->szActualRecordFile);
							UpdateSecondaryStatusText(szTemp);
						}
						GetDlgItemText(hDlg, IDC_RECORD_TITLE, v->szRecordTitle, sizeof(v->szRecordTitle));
						if (lstrlen(v->szRecordTitle) == 0)
							lstrcpy(v->szRecordTitle, " ");
					}
					v->nAutoRecordSecondCounter = 0;
					EndDialog(hDlg, TRUE);
				}
				break;
			case IDC_RECORD_PS:
			case IDC_RECORD_TS:
#ifdef LITE
				if (LOWORD(wParam) == IDC_RECORD_PS)
				{
					if (IsDlgButtonChecked(hDlg, IDC_RECORD_PS))
					{
						CheckDlgButton(hDlg, IDC_RECORD_PS, FALSE);
						CheckDlgButton(hDlg, IDC_RECORD_TS, TRUE);
						v->fRecordProgramStream = FALSE;
						MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
					}
				}
#else LITE
				if (LOWORD(wParam) == IDC_RECORD_PS && !v->fDisablePSWarning)
				{
					if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_PROGRAM_STREAM_WARNING), hDlg, PSWarningDlgProc) == FALSE)
					{
						CheckDlgButton(hDlg, IDC_RECORD_TS, BST_CHECKED);
						CheckDlgButton(hDlg, IDC_RECORD_PS, BST_UNCHECKED);
						break;
					}
				}
				v->fRecordProgramStream = IsDlgButtonChecked(hDlg, IDC_RECORD_PS);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TITLE), !v->fRecordProgramStream);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_TITLE_CAPTION), !v->fRecordProgramStream);				
#endif LITE
				break;
			case IDC_RECORD_BROWSE:
				{
					OPENFILENAME ofn;

					memset( &(ofn), 0, sizeof(ofn));
					ofn.lStructSize	= sizeof(ofn);
					ofn.hwndOwner = hDlg;
					ofn.lpstrFile = v->szRecordFile;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrFilter = TEXT("MPEG Files (*.mpg)\0*.mpg\0All Files (*.*)\0*.*\0\0");	
					ofn.lpstrTitle = TEXT("Record Stream");
					ofn.lpstrDefExt = TEXT("mpg");
					ofn.lpstrInitialDir = v->ss.szTransportStreamInitialDir;
					ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
					ofn.hInstance = v->hInstance;						
					if (myGetSaveFileName(&ofn) == TRUE)
						SetDlgItemText(hDlg, IDC_RECORD_FILE, v->szRecordFile);
					break;
				}
			case IDC_RECORD_SPLIT:
				v->fSplitRecord = IsDlgButtonChecked(hDlg, IDC_RECORD_SPLIT);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_SPLIT_SIZE), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_MB_LIMIT), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_SEC_LIMIT), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_SPLIT_CAPTION), v->fSplitRecord);
				EnableWindow(GetDlgItem(hDlg, IDC_RECORD_FILENAME_SAMPLE), v->fSplitRecord);
				SetFocus(GetDlgItem(hDlg, IDC_RECORD_SPLIT_SIZE));
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			case IDC_CHANGE_FILENAME_FORMAT:
				{
					char szFileName[MAX_PATH];
					char szPreview[MAX_PATH];

					DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_FILE_NAME_FORMAT), hDlg, FileNameFormatDlgProc);
					GetDlgItemText(hDlg, IDC_RECORD_FILE, szFileName, sizeof(szFileName));
					GenerateSplitFilename(szFileName, szPreview, v->szSplitFormatString, 1);
					SetDlgItemText(hDlg, IDC_RECORD_FILENAME_SAMPLE, szPreview);
				}
				break;				
			}
		case LBN_DBLCLK:
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_RECORD_AUDIO_ES))
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
			break;
		}
		break;
	}

	return FALSE;
}

BOOL VerifySelectedProgram(HWND hDlg)
{
	if (v->pat.hPATTreeItem != NULL || GetTotalPMTChannels())
	{
		if (v->nSelectedProgram == -1)
		{
			MessageBox(hDlg, "Please select a program by clicking on a PMT PID before recording", gszAppName, MB_OK | MB_ICONINFORMATION);
			return FALSE;
		}
	}
	return TRUE;
}

int BuildTSSection(BYTE * pInput, int nInputLength, BYTE * pOutput, int nMaxOutput, int nPID)
{
	int nPacket;
	int nPointer;
	int nWholePackets = (nInputLength + 1) / 184; // +1 for pointer byte on first packet
	int nRemainder = nInputLength - (nWholePackets * 184);
	if (nRemainder > 0)
		nWholePackets++;
	nPointer = 184 - nRemainder;;
	
	set_buf(BM_USER_THREAD, pOutput, nWholePackets * 188, TRUE);
	for (nPacket = 0; nPacket < nWholePackets; nPacket++)
	{
		int nBytesLeftInPacket;
		BYTE nPIDHigh = (nPID >> 8) & 0xff;
		BYTE nPIDLow = nPID & 0xff;

		set_bits(BM_USER_THREAD, 0x47, 8);		// sync
		if (nPacket == 0)
			nPIDHigh |= 0x40;					// indicate PES start
		set_bits(BM_USER_THREAD, nPIDHigh, 8);	// PES start/PID
		set_bits(BM_USER_THREAD, nPIDLow, 8);	// PID
		set_bits(BM_USER_THREAD, 0x10 | (nPacket % 7), 8);		// TSF, adaption flags & continuity
		if (nPacket == 0)
		{
			set_bits(BM_USER_THREAD, 0, 8);		// pointer
			nBytesLeftInPacket = 183;
		}
		else
			nBytesLeftInPacket = 184;
		while (nBytesLeftInPacket && nInputLength)
		{
			set_bits(BM_USER_THREAD, *pInput, 8);
			pInput++;
			nBytesLeftInPacket--;
			nInputLength--;
		}
		if (nInputLength == 0)
		{
			while (nBytesLeftInPacket)
			{
				set_bits(BM_USER_THREAD, 0xff, 8);
				nBytesLeftInPacket--;
			}
		}
	}
	
	return nWholePackets;
}

int GetECMPID(int nPMTIndex, BOOL fUsePreferedCAID, BOOL nPrefereredCAID)
{
	int nECMPID = -1;
	int nEMMPID = 0;
	int nCASystemID = -1;
	int i;

	// First find the EMM entry in the CAT so we can find the CA system number
	// what about multiple CA systems?
	for (i = 0; i < MAX_CAT_DESCRIPTORS; i++)
	{
		if (v->cat.pDescriptor[i] == NULL)
			break;

		if (v->cat.pDescriptor[i][0] == 9)
		{
			// CA descriptor
			nCASystemID = (v->cat.pDescriptor[i][2] << 8) + v->cat.pDescriptor[i][3];
			nEMMPID = ((v->cat.pDescriptor[i][4] << 8) + v->cat.pDescriptor[i][5]) & 0x1fff;
			if (fUsePreferedCAID && nCASystemID == nPrefereredCAID)
				break;
			else if (!fUsePreferedCAID)
				break;
		}
	}
	//if (nCASystemID != -1)
	{
		// Got the CA system so search for the CA descriptor
		// in the PMT
		int nESIndex;

		for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
		{
			if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
				break;

			if (v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors)
			{
				BYTE * pDescriptorData = v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors;
				int nDescriptorsLength = v->pat.pmt[nPMTIndex].es[nESIndex].nDescriptorsLength;
				int nCurrentIndex = 0;

				do
				{
					int nThisDescriptorLength = (BYTE)pDescriptorData[nCurrentIndex + 1];

					if (pDescriptorData[nCurrentIndex] == 9)
					{
						// CA descriptor
						int nCheckCASystemID = (pDescriptorData[nCurrentIndex + 2] << 8) + pDescriptorData[nCurrentIndex + 3];
						int nCheckECMPID = ((pDescriptorData[nCurrentIndex + 4] << 8) + pDescriptorData[nCurrentIndex + 5]) & 0x1fff;

						// Check for BEV -- they use type 0x1801 in the CAT but for the
						// ECM pointer they use 0x1234. Odd...
						if (nCheckCASystemID == 0x1234)
							nCASystemID = 0x1234;

						if (nCheckCASystemID == nCASystemID && nCheckECMPID != 0x1fff)
							return nCheckECMPID;

						if (nCASystemID == -1 && nCheckECMPID != 0x1fff)
							return nCheckECMPID;
					}

					nCurrentIndex += (BYTE)pDescriptorData[nCurrentIndex + 1]; // descriptor length
					nCurrentIndex += 2;	// descriptor tag and length
				} while (nCurrentIndex < nDescriptorsLength);
			}
		}

		// Still not found, so try the PMT
		if (v->pat.pmt[nPMTIndex].pProgramInfo != NULL)
		{
			BYTE * pDescriptorData = v->pat.pmt[nPMTIndex].pProgramInfo;
			int nDescriptorsLength = v->pat.pmt[nPMTIndex].nProgramInfoLength;
			int nCurrentIndex = 0;

			do
			{
				if (pDescriptorData[nCurrentIndex] == 9)
				{
					// CA descriptor
					int nCheckCASystemID = (pDescriptorData[nCurrentIndex + 2] << 8) + pDescriptorData[nCurrentIndex + 3];
					int nCheckECMPID = ((pDescriptorData[nCurrentIndex + 4] << 8) + pDescriptorData[nCurrentIndex + 5]) & 0x1fff;

					if (nCheckCASystemID == nCASystemID)
						return nCheckECMPID;
					if (nCASystemID == -1)
						return nCheckECMPID;
				}

				nCurrentIndex += (BYTE)pDescriptorData[nCurrentIndex + 1]; // descriptor length
				nCurrentIndex += 2;	// descriptor tag and length
			} while (nCurrentIndex < nDescriptorsLength);
		}

	}

	return nECMPID;
}

void SetupCAESInfo(int nPID, int nExtraLength)
{
	int nESIndex;
	BOOL fSetESInfoLength = FALSE;

	for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
	{
		BOOL fPIDMatch = FALSE;

		if (v->pat.pmt[v->nSelectedProgram].es[nESIndex].nESPID == 0)
			break;
		if (v->pat.pmt[v->nSelectedProgram].es[nESIndex].nESPID == nPID)
		{
			if (v->pat.pmt[v->nSelectedProgram].es[nESIndex].pDescriptors != NULL)
			{
				// Figure out how big the es_info will be because there might be multiple CA descriptors
				BYTE * pDescriptorData = v->pat.pmt[v->nSelectedProgram].es[nESIndex].pDescriptors;
				int nDescriptorsLength = v->pat.pmt[v->nSelectedProgram].es[nESIndex].nDescriptorsLength;
				int nCurrentIndex = 0;
				int nESInfoSize = 0;

				do
				{
					if (pDescriptorData[nCurrentIndex + 0] == 0x09)
					{
						BOOL fAddThisOne = TRUE;

						int nCASystemID = (pDescriptorData[nCurrentIndex + 2] << 8) + pDescriptorData[nCurrentIndex + 3];
						
						if (v->fUsePreferedCAID && nCASystemID != v->nPrefereredCAID)
							fAddThisOne = FALSE;

						if (fAddThisOne)
							nESInfoSize += pDescriptorData[nCurrentIndex + 1] + 2;
					}
					nCurrentIndex += (BYTE)pDescriptorData[nCurrentIndex + 1]; // descriptor length
					nCurrentIndex += 2;	// descriptor tag and length
				} while (nCurrentIndex < nDescriptorsLength);

				// Now got the size we can copy over the es_info CA descriptors
				set_bits(BM_USER_THREAD, nESInfoSize + nExtraLength, 12);	// ES info Length
				fSetESInfoLength = TRUE;
				pDescriptorData = v->pat.pmt[v->nSelectedProgram].es[nESIndex].pDescriptors;
				nDescriptorsLength = v->pat.pmt[v->nSelectedProgram].es[nESIndex].nDescriptorsLength;
				nCurrentIndex = 0;
				do
				{
					if (pDescriptorData[nCurrentIndex + 0] == 0x09)
					{
						int nByte;
						BOOL fAddThisOne = TRUE;

						int nCASystemID = (pDescriptorData[nCurrentIndex + 2] << 8) + pDescriptorData[nCurrentIndex + 3];
						
						if (v->fUsePreferedCAID && nCASystemID != v->nPrefereredCAID)
							fAddThisOne = FALSE;

						if (fAddThisOne)
						{
							for (nByte = 0; nByte < pDescriptorData[nCurrentIndex + 1] + 2; nByte++)
								set_bits(BM_USER_THREAD, pDescriptorData[nCurrentIndex + nByte], 8);
						}
					}
					nCurrentIndex += (BYTE)pDescriptorData[nCurrentIndex + 1]; // descriptor length
					nCurrentIndex += 2;	// descriptor tag and length
				} while (nCurrentIndex < nDescriptorsLength);				
			}
			break;
		}
	}
	if (!fSetESInfoLength)
		set_bits(BM_USER_THREAD, nExtraLength, 12);	// ES info Length
}

void SetupPriorToProgramRecording()
{
	int nSectionLength;
	DWORD dwCRC;
	int nPMTPID;
	BYTE pmt_section[3096];

	if (v->pat.hPATTreeItem != NULL || GetTotalPMTChannels())
	{
		int i;
		PPARSEDMPEGVIDEO pMPEG;
		
		v->nRecordPCRPID = v->pat.pmt[v->nSelectedProgram].nPCRPID;
		if (v->nRecordVideoESIndex != -1)
			v->nRecordVideoPID = v->pat.pmt[v->nSelectedProgram].es[v->nRecordVideoESIndex].nESPID;
		else
			v->nRecordVideoPID = 0x1fff;
		v->fStradisPAL = FALSE;
		if (v->nRecordVideoESIndex != -1)
		{
			pMPEG = (PPARSEDMPEGVIDEO)v->pat.pmt[v->nSelectedProgram].es[v->nRecordVideoESIndex].pParsedData;
			if (pMPEG != NULL)
			{
				if ( (pMPEG->frame_rate_code == 3) || (pMPEG->frame_rate_code == 6) )
					v->fStradisPAL = TRUE;
			}
		}
		for (i = 0; i < MAX_AUDIO_STREAMS; i++)
		{
			if (v->nRecordAudioESIndex[i] != -1)
			{
				v->nRecordAudioPID[i] = v->pat.pmt[v->nSelectedProgram].es[v->nRecordAudioESIndex[i]].nESPID;
				if (v->pat.pmt[v->nSelectedProgram].es[v->nRecordAudioESIndex[i]].nStreamType == 0x06)
				{
					if (v->fAudioPMTETSI == FALSE)
					{
						if (IsAC3AudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE)
							v->nRecordAudioType[i] = 0x81;
						else if (IsPCMAudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE)
							v->nRecordAudioType[i] = 0x83;
						else if (IsDTSAudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE)
							v->nRecordAudioType[i] = 0x85;
					}
					else
						v->nRecordAudioType[i] = 0x06;
				}
				else
					v->nRecordAudioType[i] = v->pat.pmt[v->nSelectedProgram].es[v->nRecordAudioESIndex[i]].nStreamType;
			}
			else
			{
				v->nRecordAudioType[i] = 0;
				v->nRecordAudioPID[i] = 0x1fff;
			}
		}

		for (i = 0; i < MAX_OTHER_STREAMS; i++)
		{
			if (v->nRecordOtherESIndex[i] != -1)
				v->nRecordOtherPID[i] = v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].nESPID;
			else
				v->nRecordOtherPID[i] = 0x1fff;
		}

		v->nRecordPMTPID = v->pat.pmt[v->nSelectedProgram].nPMTPID;

		if (ATSCPIDs())
			nPMTPID = 0x0030;
		else
		{
			BOOL fTerminate;
			nPMTPID = 0x0020;
			do
			{
				fTerminate = TRUE;
				if (nPMTPID == v->nRecordVideoPID)
				{
					nPMTPID++;
					fTerminate = FALSE;
				}
				else if (nPMTPID == v->nRecordPCRPID)
				{
					nPMTPID++;
					fTerminate = FALSE;
				}
				else
				{
					for (i = 0; i < MAX_AUDIO_STREAMS; i++)
					{
						if (nPMTPID == v->nRecordAudioPID[i])
						{
							nPMTPID++;
							fTerminate = FALSE;
						}
					}
					for (i = 0; i < MAX_OTHER_STREAMS; i++)
					{
						if (nPMTPID == v->nRecordOtherPID[i])
						{
							nPMTPID++;
							fTerminate = FALSE;
						}
					}
				}
			} while (!fTerminate && nPMTPID < 8190);
		}

		// Setup the PAT
		set_buf(BM_USER_THREAD, v->out_pat, 188, TRUE);
		set_bits(BM_USER_THREAD, 0x47, 8);		// sync
		set_bits(BM_USER_THREAD, 0x40, 8);		// PES start/PID
		set_bits(BM_USER_THREAD, 0x00, 8);		// PID
		set_bits(BM_USER_THREAD, 0x10, 8);		// TSF flags & continuity
		set_bits(BM_USER_THREAD, 0x00, 8);		// pointer
		set_bits(BM_USER_THREAD, 0x00, 8);		// table ID
		set_bits(BM_USER_THREAD, 0x1, 1);		// section syntax indicator
		set_bits(BM_USER_THREAD, 0x0, 1);		// '0'
		set_bits(BM_USER_THREAD, 0x3, 2);		// reserved
		set_bits(BM_USER_THREAD, 0x00d, 12);	// section length
		set_bits(BM_USER_THREAD, 0x0001, 16);	// transport stream ID
		set_bits(BM_USER_THREAD, 0x3, 2);		// reserved
		set_bits(BM_USER_THREAD, 0x01, 5);		// version number
		set_bits(BM_USER_THREAD, 0x1, 1);		// current/next indicator
		set_bits(BM_USER_THREAD, 0x00, 8);		// section number
		set_bits(BM_USER_THREAD, 0x00, 8);		// last section number
		if (ATSCPIDs())
			set_bits(BM_USER_THREAD, 0x0003, 16);	// program number
		else
			set_bits(BM_USER_THREAD, 0x0001, 16);	// program number
		set_bits(BM_USER_THREAD, 0x7, 3);		// reserved
		set_bits(BM_USER_THREAD, nPMTPID, 13);	// PMT PID
		dwCRC = SourceHelper_CRC_Calc(&v->out_pat[5], 21 - 5 - 4);	
		set_bits(BM_USER_THREAD, dwCRC, 32);
		while (get_byte_pos(BM_USER_THREAD) < 188)
			set_bits(BM_USER_THREAD, 0xff, 8);

		// Setup the PMT
		nSectionLength = 13;
		if (v->nRecordVideoPID != 0x1fff)
			nSectionLength += 5;
		
		// Bump the PMT section length because of the audio entries
		for (i = 0; i < MAX_AUDIO_STREAMS; i++)
		{
			if (v->nRecordAudioPID[i] != 0x1fff)
			{
				nSectionLength += 5;
				if (v->fAudioPMTETSI == FALSE)
				{
					if (v->nRecordAudioType[i] == 0x81 || v->nRecordAudioType[i] == 0x83)
						nSectionLength += 6;
					else if (v->nRecordAudioType[i] == 0x85)
						nSectionLength += 6 + GetDTSAudioDescriptor(v->nSelectedProgram, v->nRecordAudioESIndex[i], NULL);
				}
				else
				{
					if (v->nRecordAudioType[i] == 0x06)
					{
						if (IsAC3AudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE)
							nSectionLength += 6;
						else if (IsPCMAudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE)
							nSectionLength += 6;
						else if (IsDTSAudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE)
							nSectionLength += 6 + GetDTSAudioDescriptor(v->nSelectedProgram, v->nRecordAudioESIndex[i], NULL);
					}
				}
			}
		}

		// Bump up the PMT section length because of the other entries
		for (i = 0; i < MAX_OTHER_STREAMS; i++)
		{
			if (v->nRecordOtherPID[i] != 0x1fff)
			{
				nSectionLength += 5;
				nSectionLength += v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].nDescriptorsLength;
			}			
		}

		// Bump by CA descriptors at the ES level if we're including CA data
		if (v->fIncludeCAData)
		{
			int nESSize = 0;
			int nESIndex;

			for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
			{
				BOOL fPIDMatch = FALSE;

				if (v->pat.pmt[v->nSelectedProgram].es[nESIndex].nESPID == 0)
					break;
				
				// See if this PID is part of the recording
				if (v->nRecordVideoPID == v->pat.pmt[v->nSelectedProgram].es[nESIndex].nESPID)
					fPIDMatch = TRUE;
				else
				{
					int nAudioStream;

					for (nAudioStream = 0; nAudioStream < MAX_AUDIO_STREAMS; nAudioStream++)
					{
						if (v->nRecordAudioPID[nAudioStream] != 0x1fff)
						{
							if (v->nRecordAudioPID[nAudioStream] == v->pat.pmt[v->nSelectedProgram].es[nESIndex].nESPID)
								fPIDMatch = TRUE;
						}
					}
				}

				if (v->pat.pmt[v->nSelectedProgram].es[nESIndex].pDescriptors != NULL && fPIDMatch)
				{
					BYTE * pDescriptorData = v->pat.pmt[v->nSelectedProgram].es[nESIndex].pDescriptors;
					int nDescriptorsLength = v->pat.pmt[v->nSelectedProgram].es[nESIndex].nDescriptorsLength;
					int nCurrentIndex = 0;

					do
					{
						if (pDescriptorData[nCurrentIndex + 0] == 0x09)
						{
							BOOL fAddThisOne = TRUE;

							int nCASystemID = (pDescriptorData[nCurrentIndex + 2] << 8) + pDescriptorData[nCurrentIndex + 3];
							
							if (v->fUsePreferedCAID && nCASystemID != v->nPrefereredCAID)
								fAddThisOne = FALSE;

							if (fAddThisOne)
								nESSize += pDescriptorData[nCurrentIndex + 1] + 2;
						}
						nCurrentIndex += (BYTE)pDescriptorData[nCurrentIndex + 1]; // descriptor length
						nCurrentIndex += 2;	// descriptor tag and length
					} while (nCurrentIndex < nDescriptorsLength);
				}
			}
			nSectionLength += nESSize;
		}

		// Bump by the Program Info length
		nSectionLength += v->pat.pmt[v->nSelectedProgram].nProgramInfoLength;

		// Make the PMT
		set_buf(BM_USER_THREAD, pmt_section, sizeof(pmt_section), TRUE);
		set_bits(BM_USER_THREAD, 0x02, 8);		// table ID
		set_bits(BM_USER_THREAD, 0x1, 1);		// section syntax indicator
		set_bits(BM_USER_THREAD, 0x0, 1);		// '0'
		set_bits(BM_USER_THREAD, 0x3, 2);		// reserved
		set_bits(BM_USER_THREAD, nSectionLength, 12);	// section length
		if (ATSCPIDs())		
			set_bits(BM_USER_THREAD, 0x0003, 16);	// program number
		else
			set_bits(BM_USER_THREAD, 0x0001, 16);	// program number
		set_bits(BM_USER_THREAD, 0x3, 2);		// reserved
		set_bits(BM_USER_THREAD, 0x01, 5);		// version number
		set_bits(BM_USER_THREAD, 0x1, 1);		// current/next indicator
		set_bits(BM_USER_THREAD, 0x00, 8);		// section number
		set_bits(BM_USER_THREAD, 0x00, 8);		// last section number
		set_bits(BM_USER_THREAD, 0x7, 3);		// reserved
		if (ATSCPIDs())
			set_bits(BM_USER_THREAD, 0x0031, 13);
		else
			set_bits(BM_USER_THREAD, v->nRecordPCRPID, 13);
		set_bits(BM_USER_THREAD, 0xf, 4);		// reserved
		set_bits(BM_USER_THREAD, v->pat.pmt[v->nSelectedProgram].nProgramInfoLength, 12);	// program info length
		for (i = 0; i < v->pat.pmt[v->nSelectedProgram].nProgramInfoLength; i++)
			set_bits(BM_USER_THREAD, v->pat.pmt[v->nSelectedProgram].pProgramInfo[i], 8);

		if (v->nRecordVideoPID != 0x1fff)
		{
			set_bits(BM_USER_THREAD, v->pat.pmt[v->nSelectedProgram].es[v->nRecordVideoESIndex].nStreamType, 8);		// stream type
			set_bits(BM_USER_THREAD, 0x7, 3);		// reserved
			if (ATSCPIDs())
				set_bits(BM_USER_THREAD, 0x0031, 13);
			else
				set_bits(BM_USER_THREAD, v->nRecordVideoPID, 13);
			set_bits(BM_USER_THREAD, 0xf, 4);		// reserved
			if (!v->fIncludeCAData)
				set_bits(BM_USER_THREAD, 0x000, 12);	// ES info Length
			else 
				SetupCAESInfo(v->nRecordVideoPID, 0);
		}
		{
			int i;

			for (i = 0; i < MAX_AUDIO_STREAMS; i++)
			{
				if (v->nRecordAudioPID[i] != 0x1fff)
				{
					set_bits(BM_USER_THREAD, v->nRecordAudioType[i], 8);		// stream type
					set_bits(BM_USER_THREAD, 0x7, 3);		// reserved
					if (ATSCPIDs())
						set_bits(BM_USER_THREAD, 0x0034 + i, 13);
					else
						set_bits(BM_USER_THREAD, v->nRecordAudioPID[i], 13);
					set_bits(BM_USER_THREAD, 0xf, 4);		// reserved
					if (    v->nRecordAudioType[i] == 0x81 
						|| (v->nRecordAudioType[i] == 0x06 && IsAC3AudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE))
					{
						if (!v->fIncludeCAData)
							set_bits(BM_USER_THREAD, 0x006, 12);	// ES info Length
						else 
							SetupCAESInfo(v->nRecordAudioPID[i], 0x006);
						set_bits(BM_USER_THREAD, 0x6a, 8);		// registration descriptor
						set_bits(BM_USER_THREAD, 0x04, 8);		// length
						set_bits(BM_USER_THREAD, 0x41432d33,32);// AC3 descriptor (AC-3)
					}
					else if (v->nRecordAudioType[i] == 0x83 
						 || (v->nRecordAudioType[i] == 0x06 && IsPCMAudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE))
					{
						if (!v->fIncludeCAData)
							set_bits(BM_USER_THREAD, 0x006, 12);	// ES info Length
						else 
							SetupCAESInfo(v->nRecordAudioPID[i], 0x006);
						set_bits(BM_USER_THREAD, 0x05, 8);		// registration descriptor
						set_bits(BM_USER_THREAD, 0x04, 8);		// length
						set_bits(BM_USER_THREAD, 0x42535344,32);// LPCM descriptor (BSSD)
					}
					else if (v->nRecordAudioType[i] == 0x85
						 || (v->nRecordAudioType[i] == 0x06 && IsDTSAudioStream(v->nSelectedProgram, v->nRecordAudioESIndex[i]) == TRUE))
					{
						int j;
						int nDTSAudioDescriptorLength;
						BYTE bDTSDescriptorBuffer[512];

						nDTSAudioDescriptorLength = GetDTSAudioDescriptor(v->nSelectedProgram, v->nRecordAudioESIndex[i], bDTSDescriptorBuffer);

						if (!v->fIncludeCAData)
							set_bits(BM_USER_THREAD, 0x006 + nDTSAudioDescriptorLength, 12);	// ES info Length
						else 
							SetupCAESInfo(v->nRecordAudioPID[i], 0x006 + nDTSAudioDescriptorLength);
						set_bits(BM_USER_THREAD, 0x05, 8);		// registration descriptor
						set_bits(BM_USER_THREAD, 0x04, 8);		// length
						switch(GetDTSFrameSize(v->nSelectedProgram, v->nRecordAudioESIndex[i]))
						{
						case 512:
							set_bits(BM_USER_THREAD, 0x44545331,32);// DTS descriptor (DTS1)
							break;
						case 1024:
							set_bits(BM_USER_THREAD, 0x44545332,32);// DTS descriptor (DTS2)
							break;
						case 2048:
							set_bits(BM_USER_THREAD, 0x44545333,32);// DTS descriptor (DTS3)
							break;
						default:
							set_bits(BM_USER_THREAD, 0x44545300,32);// DTS descriptor (DTS null)
							break;
						}
						for (j = 0; j < nDTSAudioDescriptorLength; j++)
							set_bits(BM_USER_THREAD, bDTSDescriptorBuffer[j], 8);
					}
					else
					{
						if (!v->fIncludeCAData)
							set_bits(BM_USER_THREAD, 0x000, 12);	// ES info Length
						else 
							SetupCAESInfo(v->nRecordAudioPID[i], 0);
					}
				}
			}
		}
		{
			int i;

			for (i = 0; i < MAX_OTHER_STREAMS; i++)
			{
				if (v->nRecordOtherPID[i] != 0x1fff)
				{
					set_bits(BM_USER_THREAD, v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].nStreamType, 8);		// stream type
					set_bits(BM_USER_THREAD, 0x7, 3);		// reserved
					set_bits(BM_USER_THREAD, v->nRecordOtherPID[i], 13); // PID
					set_bits(BM_USER_THREAD, 0xf, 4);		// reserved
					set_bits(BM_USER_THREAD, v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].nDescriptorsLength, 12);	// ES info Length
					if (v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].nDescriptorsLength)
					{
						int j;
						for (j = 0; j < v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].nDescriptorsLength; j++)
							set_bits(BM_USER_THREAD, v->pat.pmt[v->nSelectedProgram].es[v->nRecordOtherESIndex[i]].pDescriptors[j], 8);
					}
				}
			}
		}

		dwCRC = SourceHelper_CRC_Calc(pmt_section, nSectionLength - 1);
		set_bits(BM_USER_THREAD, dwCRC, 32);
		v->nOutputPMTPackets = BuildTSSection(pmt_section, nSectionLength + 3, v->out_pmt, sizeof(v->out_pmt), nPMTPID);

		// Setup the SDT
		{
			int nProviderNameLength, nServiceNameLength;
			int nDescriptorsLength;
			int nDescriptorLength;
			int k;
			SYSTEMTIME st;
			char szTemp[128];

			GetLocalTime(&st);
			wsprintf(szTemp, "Generated by TSReader %s on %04d/%02d/%02d %02d:%02d",
					 GetTSRVersion(NULL),
					 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
			nProviderNameLength = lstrlen(szTemp);					
			nServiceNameLength = lstrlen(v->szRecordTitle);
			
			nDescriptorLength = nProviderNameLength + nServiceNameLength + 3;
			nDescriptorsLength = nDescriptorLength + 2;
			nSectionLength = 13		// length of the section header
						   + 4		// plus CRC
						   + nDescriptorsLength;

			set_buf(BM_USER_THREAD, v->out_sdt, 188, TRUE);
			set_bits(BM_USER_THREAD, 0x47, 8);		// sync
			set_bits(BM_USER_THREAD, 0x40, 8);		// PES start/PID
			set_bits(BM_USER_THREAD, 0x11, 8);		// PID
			set_bits(BM_USER_THREAD, 0x10, 8);		// TSF flags & continuity
			set_bits(BM_USER_THREAD, 0x00, 8);		// pointer
			set_bits(BM_USER_THREAD, 0x42, 8);		// table number
			set_bits(BM_USER_THREAD, 0x01, 1);		// section syntax indicator
			set_bits(BM_USER_THREAD, 0x01, 1);		// reserved future use
			set_bits(BM_USER_THREAD, 0x03, 2);		// reserved
			set_bits(BM_USER_THREAD, nSectionLength, 12);		
			set_bits(BM_USER_THREAD, 0x0001, 16);	// transport ID
			set_bits(BM_USER_THREAD, 0xe7, 8);		// version/current-next
			set_bits(BM_USER_THREAD, 0x00, 8);		// section
			set_bits(BM_USER_THREAD, 0x00, 8);		// last section
			set_bits(BM_USER_THREAD, 0x0001, 16);	// original network
			set_bits(BM_USER_THREAD, 0xff, 8);		// reserved
			set_bits(BM_USER_THREAD, 0x0001, 16);	// service number
			set_bits(BM_USER_THREAD, 0xfe, 8);		// reserved/EIT schedule
			set_bits(BM_USER_THREAD, 0x00, 3);		// running status
			set_bits(BM_USER_THREAD, 0x01, 1);		// free CA mode
			set_bits(BM_USER_THREAD, nDescriptorsLength, 12);
			set_bits(BM_USER_THREAD, 0x48, 8);		// descriptor tag
			set_bits(BM_USER_THREAD, nDescriptorLength, 8);
			set_bits(BM_USER_THREAD, 0x01, 8);		// service type
			set_bits(BM_USER_THREAD, nProviderNameLength, 8);
			for (k = 0; k < nProviderNameLength; k++)
				set_bits(BM_USER_THREAD, szTemp[k], 8);
			set_bits(BM_USER_THREAD, nServiceNameLength, 8);
			for (k = 0; k < nServiceNameLength; k++)
				set_bits(BM_USER_THREAD, v->szRecordTitle[k], 8);
			dwCRC = SourceHelper_CRC_Calc(&v->out_sdt[5], nSectionLength - 1);
			set_bits(BM_USER_THREAD, dwCRC, 32);
			while (get_byte_pos(BM_USER_THREAD) < 188)
				set_bits(BM_USER_THREAD, 0xff, 8);
		}
	}

#ifndef LITE
	// Setup CA PID list
	if (v->fIncludeCAData)
	{
		int nCAPIDIndex = 0;
		int i;

		v->nCAPIDs[nCAPIDIndex++] = 0x0001;		// include the CAT
		for (i = 0; i < MAX_CAT_DESCRIPTORS; i++)
		{
			BYTE * pDescriptorData = v->cat.pDescriptor[i];
			if (pDescriptorData == NULL)
				break;
			if (pDescriptorData[0] == 0x09)	// CA descriptor
			{
				BOOL fAddThisOne = TRUE;

				int nCASystemID = (pDescriptorData[2] << 8) + pDescriptorData[3];
				int nCAPID = ((pDescriptorData[4] << 8) + pDescriptorData[5]) & 0x1fff;
				
				if (v->fUsePreferedCAID && nCASystemID != v->nPrefereredCAID)
					fAddThisOne = FALSE;

				if (fAddThisOne)
				{
					v->nCAPIDs[nCAPIDIndex++] = nCAPID;
					v->nCAPIDs[nCAPIDIndex++] = GetECMPID(v->nSelectedProgram, TRUE, nCASystemID);
				}
			}
		}
		v->nCAPIDs[nCAPIDIndex] = 0x1fff;
	}
#endif LITE
}

void EnableDisableVLCMenuItems(HMENU hMenu, int nOptions)
{
	int i;

	for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
		EnableMenuItem(hMenu, i + ID_PLAYBACK_VLC_1, nOptions);
}

void EnableOrDisableStreamMenuItems(HWND hDlg, BOOL fEnable)
{
	HMENU hMenu = GetMenu(hDlg);
	UINT uOptions = MF_ENABLED | MF_BYCOMMAND;

	if (!fEnable)
		uOptions = MF_DISABLED | MF_GRAYED | MF_BYCOMMAND;

	if (v->fStradisInterface && v->nStreamTo != STREAM_TO_STRADIS)
		EnableMenuItem(hMenu, IDC_SI_PARSER_TO_STRADIS, uOptions);
	if (v->fDSInterface && v->nStreamTo != STREAM_TO_DIRECTSHOW)
		EnableMenuItem(hMenu, ID_PLAYBACK_DIRECTSHOW, uOptions);
	if (v->fXNSInterface && v->nStreamTo != STREAM_TO_XNS)
		EnableMenuItem(hMenu, ID_PLAYBACK_XNSSERVER, uOptions);
	if (v->fDVHSInterface && v->nStreamTo != STREAM_TO_DVHS)
		EnableMenuItem(hMenu, ID_RECORD_RECORDPROGRAMTODVHS, uOptions);
	if (v->fVLCInterface && v->nStreamTo != STREAM_TO_VLC)
		EnableDisableVLCMenuItems(hMenu, uOptions);
	if (v->fVLCInterface && v->nStreamTo != STREAM_TO_ROKU)
		EnableMenuItem(hMenu, ID_PLAYBACK_ROKUHD1000, uOptions);
}

BOOL CALLBACK VLCConnectionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTemp[256];
			wsprintf(szTemp, "Launch your Media application and point it at this machine's TCP port %d using HTTP", v->nVLCPort);
			SetDlgItemText(hDlg, IDC_STATUS, szTemp);
			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, TRUE);
			break;
		case IDC_DONT_SHOW:
			v->fDontShowVLCConnectionDialog = IsDlgButtonChecked(hDlg, IDC_DONT_SHOW);
			break;
		}
		break;
	}

	return FALSE;
}

DWORD WINAPI LaunchVLCThread(LPVOID lpv)
{
	if (WinExec((char *)lpv, SW_SHOW) <= 31)
		MessageBox(v->hDlgSIParser, "Unable to start VLC - check location in the Settings/VLC dialog", gszAppName, MB_ICONSTOP);
	return 0;
}

BOOL CALLBACK EITLanguageSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_EIT_LANGUAGE_FILTER_ENABLED, v->fEITLanguageFilterEnabled);
		SetDlgItemText(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE, v->szEITLanguageFilterLanguage);
		EnableWindow(GetDlgItem(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE), v->fEITLanguageFilterEnabled);
		EnableWindow(GetDlgItem(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE_CAPTION), v->fEITLanguageFilterEnabled);
		SendDlgItemMessage(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE, EM_LIMITTEXT, 3, 0);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fEITLanguageFilterEnabled = IsDlgButtonChecked(hDlg, IDC_EIT_LANGUAGE_FILTER_ENABLED);
			GetDlgItemText(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE, v->szEITLanguageFilterLanguage, sizeof(v->szEITLanguageFilterLanguage));
			strlwr(v->szEITLanguageFilterLanguage);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_EIT_LANGUAGE_FILTER_ENABLED:
			{
				BOOL fEnabled = IsDlgButtonChecked(hDlg, IDC_EIT_LANGUAGE_FILTER_ENABLED);
				EnableWindow(GetDlgItem(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE), fEnabled);
				EnableWindow(GetDlgItem(hDlg, IDC_EIT_LANGUAGE_FILTER_LANGUAGE_CAPTION), fEnabled);
			}
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK StradisSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_STRADIS_FORCE_PAL, v->fForceStradisPAL);
		switch(v->nStradisAPI)
		{
		case 1:
			CheckDlgButton(hDlg, IDC_STRADIS_API_12, BST_CHECKED);
			break;
		case 2:
			CheckDlgButton(hDlg, IDC_STRADIS_API_16, BST_CHECKED);
			break;
		}
		break;
		SetFocus(GetDlgItem(hDlg, IDOK));
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fForceStradisPAL = IsDlgButtonChecked(hDlg, IDC_STRADIS_FORCE_PAL);
			if (IsDlgButtonChecked(hDlg, IDC_STRADIS_API_12))
				v->nStradisAPI = 1;
			else if (IsDlgButtonChecked(hDlg, IDC_STRADIS_API_16))
				v->nStradisAPI = 2;
			else
			{
				MessageBox(hDlg, "Please select the Stradis API version to use", gszAppName, MB_ICONSTOP);
				SetFocus(GetDlgItem(hDlg, IDC_STRADIS_API_12));
				break;
			}
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

void StreamDecoder(HWND hWnd)
{
	if (v->fRecording == TRUE)
	{
		v->fRecordProgramStream = v->fSavedRecordProgramStream;
		EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_ALL, MF_ENABLED | MF_BYCOMMAND);
		EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD, MF_ENABLED | MF_BYCOMMAND);
		EnableOrDisableStreamMenuItems(hWnd, TRUE);
		switch(v->nStreamTo)
		{
		case STREAM_TO_DIRECTSHOW:
			CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_DIRECTSHOW, MF_UNCHECKED | MF_BYCOMMAND);
			break;
		case STREAM_TO_STRADIS:
			CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_TO_STRADIS, MF_UNCHECKED | MF_BYCOMMAND);
			break;
		case STREAM_TO_XNS:
			CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_XNSSERVER, MF_UNCHECKED | MF_BYCOMMAND);
			break;
		case STREAM_TO_DVHS:
			CheckMenuItem(GetMenu(hWnd), ID_RECORD_RECORDPROGRAMTODVHS, MF_UNCHECKED | MF_BYCOMMAND);
			break;
		case STREAM_TO_VLC:
			{
				int i;
				if (v->fVLCControl == TRUE)
				{
					HWND hWndVLC;
					char szClass[] = {"wxWindowClassNR"};

					hWndVLC = FindWindow(szClass, "VLC media player");
					if (hWndVLC == NULL)
						lstrcpy(szClass, "wxWindowClass");

					while ( (hWndVLC = FindWindow(szClass, "VLC media player")) != NULL)
					{
						PostMessage(hWndVLC, WM_CLOSE, 0, 0);
						Sleep(100);
					}
				}
				CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_VLC_1 + v->nVLCPlaybackConfig, MF_UNCHECKED | MF_BYCOMMAND); 
				for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
				{
					if (i != v->nVLCPlaybackConfig)
						EnableMenuItem(GetMenu(hWnd), ID_PLAYBACK_VLC_1 + i, MF_ENABLED | MF_BYCOMMAND);
				}
			}
			break;
		case STREAM_TO_ROKU:
			CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_ROKUHD1000, MF_UNCHECKED | MF_BYCOMMAND);
			v->fTerminateRokuThread = TRUE;
			break;
		}
		v->fRecording = FALSE;
		v->fStradisActive = FALSE;
		CloseHandle(v->hRecordFile);
		CloseHandle(v->hStradisReadPipe);
		ShutdownStradis();
		if (v->hStradisInterface != v->hXNSInterface)
			FreeLibrary(v->hStradisInterface);
		v->dTotalRecorded = 0.0;
		v->nStreamBuffers = 0;
		SendMessage(v->hWndST, SB_SETICON, 4, (LPARAM)v->hStatusBarIcons[0]);
		v->nAutoRecordProgram = 0;
		v->nAutoRecord = AUTO_RECORD_NONE;
	}
	else
	{
		if (v->nStreamTo == STREAM_TO_STRADIS && (v->nStradisAPI < 1 || v->nStradisAPI > 2))
		{
			MessageBox(hWnd, "You must first set the Stradis API version to use - please select on the next dialog", gszAppName, MB_ICONINFORMATION);
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_STRADIS_SETTINGS), hWnd, StradisSettingsDlgProc) == FALSE)
				return;
		}

		if (VerifySelectedProgram(hWnd) == FALSE)
			return;

		v->fRecordDialogStreamOnly = TRUE;
		if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_STRADIS), hWnd, RecordDlgProc) == TRUE)
		{
			switch(v->nStreamTo)
			{
			case STREAM_TO_DIRECTSHOW:
				v->hStradisInterface = LoadLibrary("TSReader_DS.dll");
				break;
			case STREAM_TO_STRADIS:
				switch(v->nStradisAPI)
				{
				case 1:
					v->hStradisInterface = LoadLibrary("TSReader_Stradis12.dll");
					break;
				case 2:
					v->hStradisInterface = LoadLibrary("TSReader_Stradis16.dll");
					break;
				}
				break;
			case STREAM_TO_XNS:
				v->hStradisInterface = v->hXNSInterface;
				break;
			case STREAM_TO_DVHS:
				v->hStradisInterface = LoadLibrary("TSReader_DVHS.dll");
				break;
			case STREAM_TO_VLC:
			case STREAM_TO_ROKU:
				v->hStradisInterface = LoadLibrary("TSReader_VLC.dll");
				break;

			}

			if (v->hStradisInterface == NULL)
			{
				MessageBox(hWnd, "Unable to start playback - output DLL not loaded", gszAppName, MB_ICONSTOP);
			}
			else
			{
				SetupStradis = (td_SetupStradis)GetProcAddress(v->hStradisInterface, "SetupStradis");
				ShutdownStradis = (td_ShutdownStradis)GetProcAddress(v->hStradisInterface, "ShutdownStradis");
				EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_ALL, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
				EnableOrDisableStreamMenuItems(hWnd, FALSE);
				switch(v->nStreamTo)
				{
				case STREAM_TO_DIRECTSHOW:
					CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_DIRECTSHOW, MF_CHECKED | MF_BYCOMMAND);
					break;
				case STREAM_TO_STRADIS:
					CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_TO_STRADIS, MF_CHECKED | MF_BYCOMMAND);
					break;
				case STREAM_TO_XNS:
					CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_XNSSERVER, MF_CHECKED | MF_BYCOMMAND);
					break;
				case STREAM_TO_DVHS:
					v->szDVHSRecordFailureReason[0] = 0;
					v->dTotalRecorded = 0;
					v->dThisFileRecorded = 0;
					CheckMenuItem(GetMenu(hWnd), ID_RECORD_RECORDPROGRAMTODVHS, MF_CHECKED | MF_BYCOMMAND);
					break;
				case STREAM_TO_VLC:
					{
						int i;
						
						CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_VLC_1 + v->nVLCPlaybackConfig, MF_CHECKED | MF_BYCOMMAND);
						for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
						{
							if (i != v->nVLCPlaybackConfig)
								EnableMenuItem(GetMenu(hWnd), ID_PLAYBACK_VLC_1 + i, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
						}
					}
					break;
				case STREAM_TO_ROKU:
					CheckMenuItem(GetMenu(hWnd), ID_PLAYBACK_ROKUHD1000, MF_CHECKED | MF_BYCOMMAND);
					break;
				}

				SetupPriorToProgramRecording();
				v->nPipeBytes = 0;
				CreatePipe(&v->hStradisReadPipe, &v->hRecordFile, NULL, v->nStreamingPipeSize * 1024 * 1024);
				v->fStradisActive = TRUE;
				SetupStradis(v);

				v->fSavedRecordProgramStream = v->fRecordProgramStream;
				v->fRecordProgramStream = FALSE;

				SendMessage(v->hWndST, SB_SETICON, 4, (LPARAM)v->hStatusBarIcons[6]);
				v->fRecordAllTS = FALSE;		
				if (v->nStreamTo != STREAM_TO_XNS && v->fRecordProgramStream == FALSE)
					WritePATandPMTandSDT();
				v->fLiteMonitorThreadActive = FALSE;
				v->dwRecordTickCounter = GetTickCount();
				v->fRecording = TRUE;

				switch(v->nStreamTo)
				{
				case STREAM_TO_VLC:
					{
						if (v->fVLCControl)
						{
							DWORD dwThreadID;
							HANDLE hThread;
							char * szIPPtr;
							char szIP[128];
							char szTemp[512];

							wsprintf(szIP, "http://127.0.0.1:%d", v->nVLCPort);					
							lstrcpy(szTemp, v->szVLCConfigCommand[v->nVLCPlaybackConfig]);
							szIPPtr = strstr(szTemp, "<IP>");
							if (szIPPtr != NULL)
							{
								*szIPPtr = '\0';
								lstrcpy(v->szVLCCommand, v->szVLCExeLocation);
								lstrcat(v->szVLCCommand, " ");
								lstrcat(v->szVLCCommand, szTemp);
								lstrcat(v->szVLCCommand, szIP);
								lstrcat(v->szVLCCommand, &szIPPtr[4]);
							}
							else
							{
								lstrcpy(v->szVLCCommand, v->szVLCExeLocation);
								lstrcat(v->szVLCCommand, " ");
								lstrcat(v->szVLCCommand, szIP);
							}
							hThread = CreateThread(NULL, 0, LaunchVLCThread, (LPVOID)v->szVLCCommand, 0, &dwThreadID);
							CloseHandle(hThread);
						}
						else
						{
							if (!v->fDontShowVLCConnectionDialog)
								DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_VLC_CONNECTION_DIALOG), hWnd, VLCConnectionDlgProc);
						}
					}
					break;
				case STREAM_TO_ROKU:
					{
						DWORD dwThreadID;
						HANDLE hThread;

						hThread = CreateThread(NULL, 0, RokuTelnetControlThread, (LPVOID)0, 0, &dwThreadID);
						CloseHandle(hThread);
					}
					break;
				}
			}
		}
	}
}

void GetRecordStartTime()
{
	SYSTEMTIME stSystemTime;
	FILETIME ftNow;

	GetSystemTime(&stSystemTime);
	SystemTimeToFileTime(&stSystemTime, &ftNow);
	memcpy(&v->lnRecordTime, &ftNow, sizeof(DWORD64));
}

#ifdef PRO
DWORD WINAPI AdvancedRecordFreeSpaceMonitorThread(LPVOID lpv)
{
	while (v->fRecording)
	{
		int i;
		int nCurrentFreeGB;
		ULARGE_INTEGER FreeBytesAvailable;
		ULARGE_INTEGER TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;
		char szCurrentRecordFile[MAX_PATH];
		char szDrive[4];

		EnterCriticalSection(&v->csActualRecordFilename);
		lstrcpy(szCurrentRecordFile, v->szActualRecordFile);
		LeaveCriticalSection(&v->csActualRecordFilename);

		while(TRUE)
		{
			szDrive[0] = szCurrentRecordFile[0];
			szDrive[1] = szCurrentRecordFile[1];
			szDrive[2] = '\0';
			GetDiskFreeSpaceEx(szDrive, &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);
			nCurrentFreeGB = (int)((double)(__int64)FreeBytesAvailable.QuadPart / 1024.0 / 1024.0 / 1024.0);

			if (nCurrentFreeGB >= v->nAdvancedRecordRemoveOldLimitGB)
				break;
			{
				// Time to remove old files
				ULARGE_INTEGER ulOldestFile;
				HANDLE hFind;
				WIN32_FIND_DATA fd;
				char * szExtensionPointer;
				char szSearchPath[MAX_PATH];
				char szOldestFile[MAX_PATH] = {0};
				char szOutputFolder[MAX_PATH];

				lstrcpy(szOutputFolder, szCurrentRecordFile);

				for (i = lstrlen(szOutputFolder); i > 0; i--)
				{
					if (szOutputFolder[i] == '.')
						szExtensionPointer = &szOutputFolder[i];
					else if (szOutputFolder[i] == '\\')
					{
						szOutputFolder[i] = '\0';
						break;
					}
				}

				ulOldestFile.QuadPart = -1;

				lstrcpy(szSearchPath, szOutputFolder);
				lstrcat(szSearchPath, "\\*");
				lstrcat(szSearchPath, szExtensionPointer);
				hFind = FindFirstFile(szSearchPath, &fd);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					do
					{
						HANDLE hFile;
						ULARGE_INTEGER ulFileDate;
						char szFilename[MAX_PATH];

						if ((fd.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
							continue;				

						lstrcpy(szFilename, szOutputFolder);
						lstrcat(szFilename, "\\");
						lstrcat(szFilename, fd.cFileName);
						if (lstrcmp(szFilename, szCurrentRecordFile) == 0)
							continue;	// don't try to delete the currently recording file!
						hFile = CreateFile(szFilename, GENERIC_READ, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
						if (hFile != INVALID_HANDLE_VALUE)
						{
							GetFileTime(hFile, (LPFILETIME)&ulFileDate, NULL, NULL);
							CloseHandle(hFile);
							if (ulFileDate.QuadPart < ulOldestFile.QuadPart)
							{
								ulOldestFile.QuadPart = ulFileDate.QuadPart;
								lstrcpy(szOldestFile, szFilename);
							}
						}
					} while (FindNextFile(hFind, &fd) == TRUE);
					FindClose(hFind);

					// Now szOldestFile has the oldest record file
					if (lstrlen(szOldestFile))
					{
						char szTemp[128 + MAX_PATH];

						wsprintf(szTemp, "TSReader: Free-up %s\n", szOldestFile);
						OutputDebugString(szTemp);
						DeleteFile(szOldestFile);
					}
					else
						break;	// didn't find an old file
				}
			}
		}

		// zzz for a minute
		for (i = 0; i < 600 && v->fRecording; i++)
			Sleep(100);
	}

	return 0;
}
#endif PRO

void RecordStream(HWND hWnd, BOOL fEntireTS, int nPID)
{
	if (v->fRecording == TRUE)
	{
		if (v->fRecordAllTS == FALSE)
		{
			EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_ALL, MF_ENABLED | MF_BYCOMMAND);			
			CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD, MF_UNCHECKED | MF_BYCOMMAND);
		}
#ifndef LITE
		else
		{
			EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD, MF_ENABLED | MF_BYCOMMAND);
			CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_ALL, MF_UNCHECKED | MF_BYCOMMAND);
		}
#endif LITE
		EnableOrDisableStreamMenuItems(hWnd, TRUE);
		v->fRecording = FALSE;
		v->fRecordAllTS = FALSE;
		memset(&v->lnRecordTime, 0, sizeof(v->lnRecordTime));
		v->nRecordProgram = 0;
		SendMessage(v->hWndST, SB_SETICON, 4, (LPARAM)v->hStatusBarIcons[0]);
		//UpdateMainStatusText("");
		//UpdateSecondaryStatusText("");

#ifndef LITE
		if (v->fRecordAllTS == FALSE && v->fRecordPIDMode == FALSE && v->fRecordProgramStream == TRUE)
			PS__StopWriting();
#endif LITE

		CloseHandle(v->hRecordFile);
		//if (v->nAutoRecord != AUTO_RECORD_NONE)
		//	PostMessage(hWnd, WM_CLOSE, 0, 0);
	}
	else
	{
		if (fEntireTS == FALSE)
		{
			if (VerifySelectedProgram(hWnd) == FALSE)
				return;
		}

		v->fRecordAllTS = fEntireTS;
		v->fRecordDialogStreamOnly = FALSE;
		v->nRecordProgram = v->pat.pmt[v->nSelectedProgram].nProgramNumber;
		if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_RECORD), hWnd, RecordDlgProc) == TRUE)
		{
			v->dTotalRecorded = 0;
			v->dThisFileRecorded = 0;
			if (v->fRecordAllTS == FALSE)
			{
				SetupPriorToProgramRecording();
				if (v->fRecordProgramStream == FALSE)
					WritePATandPMTandSDT();
			}
			if (v->fRecordAllTS == FALSE)
			{
				CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD, MF_CHECKED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_ALL, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);			
			}
#ifndef LITE
			else
			{
				CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_ALL, MF_CHECKED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
			}
#endif LITE
			EnableOrDisableStreamMenuItems(hWnd, FALSE);

			v->nPATRecordCounter = 0;
#ifndef LITE
			if (v->fRecordAllTS == FALSE && v->fRecordPIDMode == FALSE && v->fRecordProgramStream == TRUE)
				PS__StartWriting();
#endif LITE

			SendMessage(v->hWndST, SB_SETICON, 4, (LPARAM)v->hStatusBarIcons[5]);
			v->fLiteMonitorThreadActive = FALSE;
			v->dwRecordTickCounter = GetTickCount();
			v->fRecording = TRUE;
			GetRecordStartTime();
#ifdef PRO
			if (v->fAdvancedRecordRemoveOldEnabled)
			{
				DWORD dwThreadID;
				HANDLE hThread;

				hThread = CreateThread(NULL, 0, AdvancedRecordFreeSpaceMonitorThread, (LPVOID)0, 0, &dwThreadID);
				CloseHandle(hThread);										
			}
#endif PRO
		}
		else
			v->nRecordProgram = 0;
	}
}

BOOL CheckForFileSplit()
{
	if (v->fSplitRecord == TRUE)
	{
		BOOL fSplitRequired = FALSE;

		if (v->fSplitSeconds == FALSE)
		{
			double dSplitSize = (double)v->nSplitFileSize * 1024.0 * 1024.0;
			if (v->dThisFileRecorded >= dSplitSize)
			{
				fSplitRequired = TRUE;
				v->dThisFileRecorded = 0;
			}
		}
		else
		{
			if (v->lnRecordSplitPCR == -1)
				v->lnRecordSplitPCR = v->lnMuxRatePCR;
			if (v->lnMuxRatePCR < v->lnRecordSplitPCR)
			{
				// rollover of the PCR - just reset for now
				// todo - do this right
				v->lnRecordSplitPCR = v->lnMuxRatePCR;
				OutputDebugString("TSReader: PCR rollover\n");
			}
			if (v->lnMuxRatePCR > (v->lnRecordSplitPCR + (__int64)v->nSplitFileSize * (__int64)27000000))
			{
				fSplitRequired = TRUE;
				v->lnRecordSplitPCR = v->lnMuxRatePCR;
			}
		}

		if (fSplitRequired)
		{
#ifndef LITE
			if (v->fRecordProgramStream == TRUE && v->fRecordAllTS == FALSE && v->fRecordPIDMode == FALSE)
			{
				DWORD dwWritten;
				BYTE bEndCode[4] = {0x00, 0x00, 0x01, 0xb9};
				WriteFile(v->hRecordFile, bEndCode, sizeof(bEndCode), &dwWritten, NULL);
			}
#endif LITE
			CloseHandle(v->hRecordFile);
			v->nPATRecordCounter = 0;

			v->nSplitFileNumber++;
			GenerateSplitFilename(v->szRecordFile, v->szActualRecordFile, v->szSplitFormatString, v->nSplitFileNumber);				
			v->hRecordFile = CreateFile(v->szActualRecordFile,
								  GENERIC_WRITE,
								  FILE_SHARE_READ,
								  (LPSECURITY_ATTRIBUTES) NULL,
								  CREATE_ALWAYS,
								  FILE_ATTRIBUTE_NORMAL,
								  (HANDLE) NULL);
			if (v->hRecordFile == INVALID_HANDLE_VALUE)
			{
				char szTemp[128 + MAX_PATH];

				RecordStream(v->hWndMainWindow, v->fRecordAllTS, -1);
				wsprintf(szTemp, "Unable to open file %s", v->szActualRecordFile);
				MessageBox(v->hWndMainWindow, szTemp, gszAppName, MB_OK | MB_ICONSTOP);
			}
			else
			{
				char szTemp[128 + MAX_PATH];
				wsprintf(szTemp, "Recording to %s", v->szActualRecordFile);
				UpdateSecondaryStatusText(szTemp);
				if (v->fRecordProgramStream == FALSE && v->fRecordAllTS == FALSE)
					WritePATandPMTandSDT();
			}
			return TRUE;
		}
	}
	
	return FALSE;
}

#ifndef LITE
void RecordAllTS(BYTE * buffer, int nLength, int nPacketLength)
{
	int n;
	int nPacketCounter = 0;
	DWORD dwWritten;

	for (n = 0; n < nLength; n += nPacketLength)
	{
		int nPID = (buffer[n + 1] << 8 | buffer[n + 2]) & 0x1fff;
#ifdef PRO
		if (v->fAdvancedRecordDropPID)
		{
			if (v->bAdvancedDropPID[nPID])
				continue;
		}
#endif PRO
		if (!(v->fDiscardNULLPIDs == TRUE && nPID == 0x1fff))
		{
			if (v->ss.fTimestampPackets && tv->pIncomingTimestamps != NULL)
			{
				WriteFile(v->hRecordFile, &tv->pIncomingTimestamps[nPacketCounter], 4, &dwWritten, NULL);
				v->dTotalRecorded += (double)dwWritten;
				v->dThisFileRecorded += (double)dwWritten;
			}
			WriteFile(v->hRecordFile, &buffer[n], nPacketLength, &dwWritten, NULL);
			v->dTotalRecorded += (double)dwWritten;
			v->dThisFileRecorded += (double)dwWritten;
			CheckForFileSplit();
			nPacketCounter++;
		}
	}
}
#endif LITE

void UpdateInputActivityPosition(int nPosition)
{
	switch(nPosition)
	{
	case 0:
		SendMessage(v->hWndST, SB_SETICON, 5, (LPARAM)v->hStatusBarIcons[1]);
		break;
	case 1:
		SendMessage(v->hWndST, SB_SETICON, 5, (LPARAM)v->hStatusBarIcons[2]);
		break;
	case 2:
		SendMessage(v->hWndST, SB_SETICON, 5, (LPARAM)v->hStatusBarIcons[3]);
		break;
	case 3:
		SendMessage(v->hWndST, SB_SETICON, 5, (LPARAM)v->hStatusBarIcons[4]);
		break;
	}
}

#ifdef PRO
void UpdateForwarderActivityPosition(int nPosition)
{
	if (!v->fForwarderEnabled && !v->fwd.nForwarderModulesActive)
		SendMessage(v->hWndST, SB_SETICON, 6, (LPARAM)v->hStatusBarIcons[0]);
	else
	{
		switch(nPosition)
		{
		case 0:
			SendMessage(v->hWndST, SB_SETICON, 6, (LPARAM)v->hStatusBarIcons[7]);
			break;
		case 1:
			SendMessage(v->hWndST, SB_SETICON, 6, (LPARAM)v->hStatusBarIcons[8]);
			break;
		case 2:
			SendMessage(v->hWndST, SB_SETICON, 6, (LPARAM)v->hStatusBarIcons[9]);
			break;
		case 3:
			SendMessage(v->hWndST, SB_SETICON, 6, (LPARAM)v->hStatusBarIcons[10]);
			break;
		}
	}
}
#endif PRO

void ForcePID(BYTE * pTSPacket, int nPID)
{
	*(pTSPacket + 1) &= 0xe0;	// clear the PID fields
	*(pTSPacket + 1) |= nPID >> 8;
	*(pTSPacket + 2) = nPID & 0xff;
}

BOOL WaitForNextTSBuffer()
{
	BOOL fOKToContinue = FALSE;
	BOOL fAbort = FALSE;
	int nBuffers;
	do
	{
		EnterCriticalSection(&v->ss.csTSBuffersInUse);
		if (v->ss.nTSBuffersInUse > 0)
		{
			fOKToContinue = TRUE;
			nBuffers = v->ss.nTSBuffersInUse--;
		}
		else if (v->ss.nTSBuffersInUse == -1000)
		{
			fOKToContinue = TRUE;
			fAbort = TRUE;
		}
		LeaveCriticalSection(&v->ss.csTSBuffersInUse);
		if (!fOKToContinue)
		{
			Sleep(5);				
		}
	} while (fOKToContinue == FALSE);
	if (nBuffers != tv->nPreviousBuffers && !fAbort)
	{
		tv->nPreviousBuffers = nBuffers;
		v->nTSBuffers = nBuffers;
	}
	EnterCriticalSection(&v->csAutoRestartOnDataStopCounter);
	v->nAutoRestartOnDataStopCounter = 0;
	LeaveCriticalSection(&v->csAutoRestartOnDataStopCounter);
	return fAbort;
}

void RecordOutstandingTSBuffers()
{
	int i;

	for (i = 0; i < tv->nActualMaxRecordBuffers; i++)
	{
		if (tv->dwRecordSize[i])
		{
			DWORD dwWritten = 0;

			if (v->fRecordPIDMode == FALSE)
			{
				BOOL fRecordCurrentBuffer = TRUE;

				if (v->fStradisActive && v->nStreamTo != STREAM_TO_VLC)
				{
					int nOffset;

					// Make sure these are decrypted packets before sending over
					for (nOffset = 0; nOffset < (int)tv->dwRecordSize[i]; nOffset += tv->nPacketLength)
					{
						if (*(tv->pOutputRecordPackets[i] + nOffset + 3) & 0xc0)
						{
							fRecordCurrentBuffer = FALSE;
							break;
						}
					}
				}
				if (fRecordCurrentBuffer)
				{
#ifndef LITE
					BOOL fWritePS = FALSE;

					if (v->fRecordProgramStream && !v->fStradisActive)
						fWritePS = TRUE;

					if (fWritePS)
						PS__TranslateToProgramStream(tv->pOutputRecordPackets[i], tv->dwRecordSize[i]);
					else
#endif LITE
					{
						WriteFile(v->hRecordFile, tv->pOutputRecordPackets[i], tv->dwRecordSize[i], &dwWritten, NULL);
						if (v->fStradisActive == TRUE)
						{
							int nPipeBytes;
							EnterCriticalSection(&v->csPipeBytes);
							v->nPipeBytes += tv->dwRecordSize[i];
							nPipeBytes = v->nPipeBytes;
							LeaveCriticalSection(&v->csPipeBytes);
							v->nStreamBuffers = nPipeBytes;
						}
#ifdef LITE
						if (v->fLiteMonitorThreadActive == FALSE)
						{
							DWORD dwThreadID;
							HANDLE hThread;
							v->fLiteMonitorThreadActive = TRUE;
	
							hThread = CreateThread(NULL, 0, LiteMonitorThread, (LPVOID)0, 0, &dwThreadID);
							CloseHandle(hThread);
						}
#endif LITE
					}
					
				}
			}
			else
			{
				if (v->fRecordPIDNoTSHeader == FALSE)
					WriteFile(tv->hRecordPIDFile[i], tv->pOutputRecordPackets[i], tv->dwRecordSize[i], &dwWritten, NULL);
				else
				{
					int nPacket;
					BYTE * pWritePtr = tv->pOutputRecordPackets[i];
					DWORD dwThisWrite;
					for (nPacket = 0; nPacket < (int)tv->dwRecordSize[i] / tv->nPacketLength; nPacket++)
					{
						WriteFile(tv->hRecordPIDFile[i], pWritePtr + 4, tv->nPacketLength - 4, &dwThisWrite, NULL);
						dwWritten += dwThisWrite;
						pWritePtr += tv->nPacketLength;
					}
				}
			}
			if (v->fRecordProgramStream == FALSE || v->fRecordPIDMode == TRUE)
			{
				v->dTotalRecorded += (double)dwWritten;
				v->dThisFileRecorded += (double)dwWritten;
				if (v->fRecordPIDMode == FALSE && !v->nStreamTo)
					CheckForFileSplit();
			}
			tv->dwRecordSize[i] = 0;
		}
		tv->fRecordBufferActive[i] = FALSE;
	}
}

void CheckPIDTEI(int nPID)
{
	if (tv->nPacketLength == 188)
	{
		if (v->fIgnoreTEIErrors)
			tv->pIncomingBuffer[tv->nBufferOffset + 1] &= ~0x80;
		if (tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x80)
		{
			v->nPIDTEICount[nPID]++;
			v->nTEIErrors++;
		}
	}
}

void CheckPIDContinuity(int nPID)
{
	int nCurrentContinuity;
	int nAdaptation = 0;

	if (nPID != v->nNullPID && tv->fIgnoreContinuity == FALSE && v->ss.fIgnoreContinuity == FALSE)
	{
		if (tv->nPacketLength == 188)
		{
			nAdaptation = (tv->pIncomingBuffer[tv->nBufferOffset + 3] >> 4) & 0x03;
			nCurrentContinuity = tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x0f;
		}
		else
		{
			if (tv->pIncomingBuffer[tv->nBufferOffset + 3] == 0x00)
				return;
			nCurrentContinuity = (tv->pIncomingBuffer[tv->nBufferOffset + 3] >> 4) & 0x0f;
		}
		if (v->nPIDContinuity[nPID] == -1)
		{
			// First time if it's an adaptation only packet
			// then the next real packet will be 1 above the
			// current adaptation only packet which can be 
			// construed as a duplicate
			if (nAdaptation != 2)
				v->nPIDContinuity[nPID] = nCurrentContinuity;
			else
				v->nPIDContinuity[nPID] = (nCurrentContinuity + 1) & 0x0f;
		}

		if (v->nPIDContinuity[nPID] != nCurrentContinuity)
		{
			if (nAdaptation != 2)	// 2 means adaptation, no payload
			{
				// See if this is a duplicate packet on the PID
				int nPreviousContinuity = v->nPIDContinuity[nPID] - 1;

				if (nPreviousContinuity == -1)
					nPreviousContinuity = 0x0f;
				if (nPreviousContinuity != nCurrentContinuity)
				{
					v->nContinuityErrors++;
					v->nPIDHasContinuityErrors[nPID]++;
				}
				v->nPIDContinuity[nPID] = nCurrentContinuity;
				if (tv->nPacketLength == 188)
				{
					if   ( ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x20) == 0x20)	// Adaptation field?
						&& (tv->pIncomingBuffer[tv->nBufferOffset + 4] > 0)					// Adaptation len
						&& ((tv->pIncomingBuffer[tv->nBufferOffset + 5] & 0x10) == 0x10)	// PCR flag
						&& ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x80) == 0x00) )	// no TEI error
					{
						int a=1;
					}
				}
			}
		}

		if (nAdaptation != 2) // 2 means adaptation, no payload
		{
			v->nPIDContinuity[nPID]++;
			v->nPIDContinuity[nPID] &= 0x0f;
		}
	}
}

void TimestampRecordPID(BYTE * pInputAdaptationField)
{
#ifndef LITE
	static BYTE bTSReaderSig[] = {"PCR packet generated by TSReader"};

	if (v->fRecording == TRUE)
	{
		if (v->fRecordPIDMode == TRUE && v->fRecordPIDsOneFile == TRUE && v->fRecordPIDIncludePCR == TRUE)
		{
			if (tv->fRecordBufferActive[0])
			{
				int i;

				set_buf(BM_PARSER_THREAD, &tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], 188, TRUE);
				set_bits(BM_PARSER_THREAD, 0x47, 8);	// sync
				set_bits(BM_PARSER_THREAD, 0, 1);		// TEI
				set_bits(BM_PARSER_THREAD, 0, 1);		// start
				set_bits(BM_PARSER_THREAD, 0, 1);		// priority
				set_bits(BM_PARSER_THREAD, v->nRecordPIDsPCRPID, 13);
				set_bits(BM_PARSER_THREAD, 0, 2);		// TSF
				set_bits(BM_PARSER_THREAD, 2, 2);		// adaptation control field
				set_bits(BM_PARSER_THREAD, v->nRecordPIDsPCRContinuity, 4);
				v->nRecordPIDsPCRContinuity = (v->nRecordPIDsPCRContinuity + 1) % 0xf;

				set_bits(BM_PARSER_THREAD, 183, 8);		// according to ISO
				set_bits(BM_PARSER_THREAD, 0, 1);		// discontinuity_indicator
				set_bits(BM_PARSER_THREAD, 0, 1);		// random_access_indicator
				set_bits(BM_PARSER_THREAD, 0, 1);		// elementary_stream_priority_indicator
				set_bits(BM_PARSER_THREAD, 1, 1);		// PCR_flag
				set_bits(BM_PARSER_THREAD, 0, 1);		// OPCR_flag
				set_bits(BM_PARSER_THREAD, 0, 1);		// splicing_point_flag
				set_bits(BM_PARSER_THREAD, 1, 1);		// transport_private_data_flag
				set_bits(BM_PARSER_THREAD, 0, 1);		// adaptation_field_extension_flag
				for (i = 0; i < 6; i++)
					set_bits(BM_PARSER_THREAD, pInputAdaptationField[2 + i], 8);
				set_bits(BM_PARSER_THREAD, lstrlen(bTSReaderSig), 8);
				for (i = 0; i < lstrlen(bTSReaderSig); i++)
					set_bits(BM_PARSER_THREAD, bTSReaderSig[i], 8);
				while (get_byte_pos(BM_PARSER_THREAD) <= 188)
					set_bits(BM_PARSER_THREAD, 0xff, 8);

				tv->dwRecordSize[0] += 188;
			}
		}
	}
#endif LITE
}

__int64 DecodeDSSRTC(BYTE * pRTC)
{
	return ((__int64)pRTC[0] << 24 | (__int64)pRTC[1] << 16 | (__int64)pRTC[2] << 8 | (__int64)pRTC[3]);
}

void DSSMuxrateProcessing(int nPID)
{
	// Multiplex bitrate calculation
	if (v->nMuxRatePID == v->nNullPID)
	{
		if (nPID < 0x0a || nPID > 0xa0)
			return;

		if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0xa0) == 0xa0
			&& tv->pIncomingBuffer[tv->nBufferOffset + 4] == 0xc3
			&& tv->pIncomingBuffer[tv->nBufferOffset + 5] == 0x7d)
		{
			int nES;

			v->lnMuxRatePCR = DecodeDSSRTC(&tv->pIncomingBuffer[tv->nBufferOffset + 7]);

			for (nES = 0; nES < MAX_ES_PARSERS; nES++)
			{
				if (v->lnESParseStartTime[nES] != 0)
					v->lnESParseStartTime[nES] = v->lnMuxRatePCR;
			}
			v->nMuxRatePID = nPID;
			v->nMuxRateBytes = 0;
			EnterCriticalSection(&v->ss.csPIDCounter);							
			v->dDisplayMuxRate = 0;
			v->nMuxRateCounter = 0;
			LeaveCriticalSection(&v->ss.csPIDCounter);
			v->nDSSRTCOffset = 0;
			if (nPID != 0)
			{
				char szTemp[128];
				wsprintf(szTemp, "TSReader: Chose SCID %s for bitrate calculation PCR = %d\n", FormatTooltipPID(v->nMuxRatePID), (DWORD)v->lnMuxRatePCR);
				OutputDebugString(szTemp);
			}
		}
	}
	else
	{
		v->nMuxRateBytes += tv->nPacketLength;
		if (v->nMuxRatePID == nPID)
		{
			if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0xa0) == 0xa0
				&& tv->pIncomingBuffer[tv->nBufferOffset + 4] == 0xc3)
			{
				double dRate;
				__int64 nCurrentPCR = DecodeDSSRTC(&tv->pIncomingBuffer[tv->nBufferOffset + 7]) + v->nDSSRTCOffset;
				
				if (nCurrentPCR < v->lnMuxRatePCR)
				{
					v->nDSSRTCOffset += 4294967296;
					nCurrentPCR += 4294967296;
				}
					
				if (nCurrentPCR >= v->lnMuxRatePCR)
				{
					dRate = v->nMuxRateBytes * (1.0 / (1.0/27000000.0 * ((double)nCurrentPCR - (double)v->lnMuxRatePCR)));				
					if (dRate > 1000)
					{
						EnterCriticalSection(&v->ss.csPIDCounter);							
						v->dDisplayMuxRate += dRate;
						v->nMuxRateCounter++;
						LeaveCriticalSection(&v->ss.csPIDCounter);
						v->lnMuxRatePCR = nCurrentPCR;
						v->nMuxRateBytes = 0;
					}
					else
					{
						OutputDebugString("TSReader: Reset mux rate calculation SCID - negative PCR\n");
						v->nMuxRatePID = v->nNullPID;
						return;
					}
				}
			}
		}
	}
}

double dPriorPCR = -1.0;
double dPriorSystem = -1.0;

void MPEG2MuxrateProcessing(int nPID)
{
	// Multiplex bitrate calculation
	if (v->nMuxRatePID == v->nNullPID)
	{
		if   ( ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x20) == 0x20)	// Adaptation field?
			&& (tv->pIncomingBuffer[tv->nBufferOffset + 4] > 0)					// Adaptation len
			&& ((tv->pIncomingBuffer[tv->nBufferOffset + 5] & 0x10) == 0x10)	// PCR flag
			&& ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x80) == 0x00) 	// no TEI error
			&& (v->lnPIDCounter[nPID] > 20) )									// and a real stream (no noise)
		{
			int nES;

			// We can now start monitoring for the bitrate
			v->lnMuxRatePCR = DecodeMPEG2PCR(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
			TimestampRecordPID(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
			
			// If we're already monitoring time and we got a discontiuity
			// then let's reset the ES parsers

			for (nES = 0; nES < MAX_ES_PARSERS; nES++)
			{
				if (v->lnESParseStartTime[nES] != 0)
					v->lnESParseStartTime[nES] = v->lnMuxRatePCR;
			}
			v->nMuxRatePID = nPID;
			v->nMuxRateBytes = 0;
			if (nPID != 0x1fff)
			{
				char szTemp[128];
				wsprintf(szTemp, "TSReader: Chose PID %s for bitrate calculation PCR = %d\n", FormatTooltipPID(v->nMuxRatePID), (DWORD)v->lnMuxRatePCR);
				OutputDebugString(szTemp);
			}
		}
	}
	else
	{
		v->nMuxRateBytes += tv->nPacketLength;
		if (v->nMuxRatePID == nPID)
		{
			if   ( ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x20) == 0x20)	// Adaptation field?
				&& (tv->pIncomingBuffer[tv->nBufferOffset + 4] > 0)					// Adaptation length
				&& ((tv->pIncomingBuffer[tv->nBufferOffset + 5] & 0x10) == 0x10)	// PCR flag
			    && ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x80) == 0x00) )	// no TEI error
			{
				__int64 nCurrentPCR = DecodeMPEG2PCR(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
				double dRate = v->nMuxRateBytes * (1.0 / (1.0/27000000.0 * ((double)nCurrentPCR - (double)v->lnMuxRatePCR)));				
				TimestampRecordPID(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
				
				if (nCurrentPCR != v->lnMuxRatePCR)
				{
					if (dRate > 1000)
					{
						EnterCriticalSection(&v->ss.csPIDCounter);							
						v->dDisplayMuxRate += dRate;
						v->nMuxRateCounter++;
						LeaveCriticalSection(&v->ss.csPIDCounter);
						v->lnMuxRatePCR = nCurrentPCR;
						v->nMuxRateBytes = 0;
					}
					else
					{
						v->dDisplayMuxRate = 0;
						v->nMuxRateCounter = 0;
						v->lnMuxRatePCR = 0;
						v->nMuxRateBytes = 0;
#ifdef PRO
						if (v->nSelectedPCRPID != 0)
						{
							v->nSelectedPCRPID = 0;
							v->lnMuxRatePCR = nCurrentPCR;
						}
						else
#endif PRO
							v->nMuxRatePID = v->nNullPID;
						OutputDebugString("TSReader: Reset mux rate calculation PID - dRate too small\n");
						return;
					}
				}
			}
		}
	}

	// Now calculate the rate for each PID if it has adaptation fields
	if (   ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x20) == 0x20)	// Adaptation field?
		&& (tv->pIncomingBuffer[tv->nBufferOffset + 4] > 0)					// Adaptation len
		&& ((tv->pIncomingBuffer[tv->nBufferOffset + 5] & 0x10) == 0x10)	// PCR flag
		&& ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x80) == 0x00) )	// no TEI error
	{
		if (v->lnPIDRatePCR[nPID] == 0)
		{
			// Haven't gotten a packet with PCR for this PID yet, so let's start monitoring
			v->lnPIDRatePCR[nPID] = DecodeMPEG2PCR(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
			v->lnPIDRateBytes[nPID] = 0;
			v->lnPIDRateSamples[nPID] = 0;
		}
		else
		{
			__int64 nCurrentPCR = DecodeMPEG2PCR(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
			if (nCurrentPCR != v->lnPIDRatePCR[nPID])
			{
				if (v->fRealtimeCharting)
				{
					v->dPIDRate[nPID] = v->lnPIDRateBytes[nPID] * (1.0 / (1.0/27000000.0 * ((double)nCurrentPCR - (double)v->lnPIDRatePCR[nPID])));
					v->lnPIDRateSamples[nPID] = 1;
				}
				else
				{
					v->dPIDRate[nPID] += v->lnPIDRateBytes[nPID] * (1.0 / (1.0/27000000.0 * ((double)nCurrentPCR - (double)v->lnPIDRatePCR[nPID])));
					v->lnPIDRateSamples[nPID]++;
				}
				v->lnPIDRatePCR[nPID] = nCurrentPCR;
				v->lnPIDRateBytes[nPID] = 0;
			}
		}
	}

	// Now bump byte counters if we're on a PID that we're counting PCRs
	if (v->lnPIDRatePCR[nPID])
		v->lnPIDRateBytes[nPID] += tv->nPacketLength;

	/*

  // This is test code for PCR jitter that doesn't work

	if (   ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x20) == 0x20)	// Adaptation field?
		&& (tv->pIncomingBuffer[tv->nBufferOffset + 4] > 0)					// Adaptation len
		&& ((tv->pIncomingBuffer[tv->nBufferOffset + 5] & 0x10) == 0x10)	// PCR flag
		&& ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x80) == 0x00) )	// no TEI error
	{
		LARGE_INTEGER nCurrentSystemCount;
		double dCurrentSystemTime, dCurrentPCRTime;
		double dDeltaSystem, dDeltaPCR;
		LARGE_INTEGER nCurrentPCR;
		char szTemp[128];
		
		dDeltaSystem = dDeltaPCR = 0.0;
		nCurrentPCR.QuadPart = DecodeMPEG2PCR(&tv->pIncomingBuffer[tv->nBufferOffset + 4]);
		QueryPerformanceCounter(&nCurrentSystemCount);
		dCurrentSystemTime = (double)nCurrentSystemCount.QuadPart / (double)v->lnTicksPerSecond;
		dCurrentPCRTime = (1.0/27000000.0) * (double)nCurrentPCR.QuadPart;

		if (dPriorPCR != -1.0)
		{
			dDeltaPCR = dCurrentPCRTime - dPriorPCR;
			dDeltaSystem = dCurrentSystemTime - dPriorSystem;
		}
		dPriorPCR = dCurrentPCRTime;
		dPriorSystem = dCurrentSystemTime;

		sprintf(szTemp, "%.10f %.10f | %.10f %.10f\n", 
			     dCurrentPCRTime, dDeltaPCR,
			     dCurrentSystemTime, dDeltaSystem);
		OutputDebugString(szTemp);	
	}*/
}

void BufferRecordChannelData(int nPID)
{
	if (nPID == v->nNullPID)
		return;

	if (GetTickCount() > v->dwRecordTickCounter + 100)
	{
		// Been at least 100ms - send a PAT/PMT pair
		v->dwRecordTickCounter = GetTickCount();
		if (tv->fRecordBufferActive[0])
		{
			if (   (v->nPATRecordCounter++ >= 10 && ATSCPIDs() == FALSE)
				&& (v->nNetworkPID == 0x0010) 
				&& (v->nRecordVideoPID != 0x0010)
				&& (v->nStreamTo != STREAM_TO_STRADIS) )
			{
				v->nPATRecordCounter = 0;
				memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], v->out_sdt, tv->nPacketLength);
				tv->dwRecordSize[0] += tv->nPacketLength;
				UpdateSDTContinuity();
			}
			else
			{
				memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], v->out_pat, tv->nPacketLength);
				tv->dwRecordSize[0] += tv->nPacketLength;
				UpdatePATContinuity();
			}
		}
		if (tv->fRecordBufferActive[0])
		{
			memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], v->out_pmt, 188 * v->nOutputPMTPackets);
			tv->dwRecordSize[0] += 188 * v->nOutputPMTPackets;
			UpdatePMTContinuity();
		}
	}
	
	if (nPID == v->nRecordVideoPID)
	{
		if (tv->fGotAdaptation == FALSE)
		{
			if (v->nRecordVideoPID == v->nRecordPCRPID)
			{
				if ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0x20) == 0x20)
					tv->fGotAdaptation = TRUE;
			}
		}
		if (tv->fGotAdaptation == TRUE)
		{
			if (tv->fRecordBufferActive[0])
			{
				memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], &tv->pIncomingBuffer[tv->nBufferOffset], tv->nPacketLength);
				if (ATSCPIDs())
					ForcePID(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], 0x0031);
				tv->dwRecordSize[0] += tv->nPacketLength;
			}
		}
	}
	else if (nPID == v->nRecordPCRPID && v->nRecordPCRPID != v->nRecordVideoPID && v->nRecordPCRPID != 0x1fff)
	{
		if (tv->fRecordBufferActive[0])
		{
			memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], &tv->pIncomingBuffer[tv->nBufferOffset], tv->nPacketLength);
			tv->dwRecordSize[0] += tv->nPacketLength;
			tv->fGotAdaptation = TRUE;
		}
	}
	else
	{
		int i;
		for (i = 0; i < MAX_AUDIO_STREAMS; i++)
		{
			if (v->nRecordAudioPID[i] == 0x1fff)
				break;
			if (nPID == v->nRecordAudioPID[i])
			{
				if (tv->fRecordBufferActive[0])
				{
					if (tv->fGotAdaptation == TRUE)
					{
						memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], &tv->pIncomingBuffer[tv->nBufferOffset], tv->nPacketLength);
						if (ATSCPIDs())
							ForcePID(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], 0x0034 + i);
						tv->dwRecordSize[0] += tv->nPacketLength;
					}
				}
			}
		}
	}

	{
		int i;
		for (i = 0; i < MAX_OTHER_STREAMS; i++)
		{
			if (v->nRecordOtherPID[i] == 0x1fff)
				break;
			if (nPID == v->nRecordOtherPID[i])
			{
				if (tv->fRecordBufferActive[0])
				{
					memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], &tv->pIncomingBuffer[tv->nBufferOffset], tv->nPacketLength);
					tv->dwRecordSize[0] += tv->nPacketLength;
				}
			}
		}
	}

#ifndef LITE
	if (v->fIncludeCAData)
	{
		int nCAPIDIndex;

		for (nCAPIDIndex = 0; nCAPIDIndex < MAX_CA_PIDS; nCAPIDIndex++)
		{
			if (v->nCAPIDs[nCAPIDIndex] == 0x1fff)
				break;
			if (v->nCAPIDs[nCAPIDIndex] == nPID)
			{
				memcpy(&tv->pOutputRecordPackets[0][tv->dwRecordSize[0]], &tv->pIncomingBuffer[tv->nBufferOffset], tv->nPacketLength);
				tv->dwRecordSize[0] += tv->nPacketLength;
			}
		}
	}
#endif LITE
}

#ifndef LITE
//void CheckForGeneralESParsing(int nPID, int nComparePID, BYTE * bESBuffer, int * nESLength, int * nESFillPtr, td_InputData CompletionRoutine, int nChartIndex)
void CheckForGeneralESParsing(int nPID, int nComparePID, BYTE * bESBuffer, int * nESLength, int * nESFillPtr, int nCompletionRoutine, int nChartIndex)
{
	if (nComparePID == nPID)
	{
		int nAdaptation = (tv->pIncomingBuffer[tv->nBufferOffset + 3] >> 4) & 0x03;
		BYTE * pWritePtr;
		int nWriteLen = 0;

		switch(nAdaptation)
		{
		case 0:		// not allowed
			break;	
		case 1:		// no adaptation
			pWritePtr = &tv->pIncomingBuffer[tv->nBufferOffset + 4];
			nWriteLen = 184;
			break;
		case 2:		// entirely adaptation
			break;
		case 3:		// adaptation + payload
			{
				int nAdaptationLen = tv->pIncomingBuffer[tv->nBufferOffset + 4];
				pWritePtr = &tv->pIncomingBuffer[tv->nBufferOffset + 5 + nAdaptationLen];
				nWriteLen = tv->nPacketLength - 5 - nAdaptationLen;
			}
			break;
		}

		if (nWriteLen)
		{
			if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x40) == 0x40)		// PES start?
			{
				BOOL fRetVal = FALSE;

				// If we have data from the last packet, send it to the decoder thread
				if (*nESFillPtr)
				{
					int nActualLength;

					if (*nESLength == -1)
						*nESLength = *nESFillPtr;
					nActualLength = *(nESLength);				
					switch(nCompletionRoutine)
					{
#ifdef PRO
					case 0:
						InputCCData(bESBuffer, nActualLength, nChartIndex);
						break;
#endif PRO
					case 1:
						InputMPEG2VideoCompositionESData(bESBuffer, nActualLength, nChartIndex);
						break;
#ifdef PRO
					case 2:
						/*{
							DWORD dwWritten;
							WriteFile(v->hDebugFile, bESBuffer, nActualLength, &dwWritten, NULL);
						}*/
						InputH264VideoCompositionESData(bESBuffer, nActualLength, nChartIndex);
						break;
#endif PRO
					}
				}
				*nESLength = 0;
				*nESFillPtr = 0;				
				*nESLength = pWritePtr[4] << 8 | pWritePtr[5];
				if (nWriteLen - 14 > 0)
				{
					memcpy(bESBuffer, pWritePtr + 14, nWriteLen - 14);
					*nESFillPtr = nWriteLen - 14;
				}

				if (*nESLength != 0)
					*nESLength -= 8; // minus header
				else
					*nESLength = -1;
			}
			else
			{
				if (*nESLength)
				{
					if (nWriteLen > 0)
					{
						if (*nESFillPtr + nWriteLen < ES_BUFFER_SIZE)
						{
							memcpy(bESBuffer + *nESFillPtr, pWritePtr, nWriteLen);
							*nESFillPtr += nWriteLen;
						}
					}
				}
			}
		}
	}
}
#endif LITE

#ifndef LITE
void LogTableMonitorData()
{
	int nPacketLength = v->nTableMonitorFillPtr;
	int nTableID;
	__int64 lnNow;
	BYTE * pSectionPointer = v->bTableMonitorBuffer;

	do
	{
		if (*pSectionPointer != 0xff)
			break;
		pSectionPointer++;
		nPacketLength--;
	} while (nPacketLength > 0);
	if (nPacketLength == 0)
		return;
	nTableID = pSectionPointer[0];
	//if ((pSectionPointer[1] & 0x80) == 0)
	//	return;		// must have section_syntax_indicator

	v->tablemonitor[nTableID].nPacketCount++;
	lnNow = v->lnMuxRatePCR;
	if (v->tablemonitor[nTableID].lnLastTime)
	{
		__int64 lnDelay = lnNow - v->tablemonitor[nTableID].lnLastTime;

		v->tablemonitor[nTableID].lnDelay += lnDelay;
		v->tablemonitor[nTableID].lnDelayItems++;

		if (lnDelay > v->tablemonitor[nTableID].lnDelayMax)
			v->tablemonitor[nTableID].lnDelayMax = lnDelay;
		if (lnDelay < v->tablemonitor[nTableID].lnDelayMin)
			v->tablemonitor[nTableID].lnDelayMin = lnDelay;

	}
	v->tablemonitor[nTableID].lnLastTime = lnNow;
#ifdef PRO
	if (v->fTableMonitorSectionDisplayEnabled && nTableID == v->nTableMonitorSectionTable)
	{
		char szFullOutput[32 * 1024] = {0};

		int nOffset, nOffset2;

		for (nOffset = 0; nOffset < nPacketLength; nOffset += 16)
		{
			char szOutput[128] = {0};
			for (nOffset2 = 0; nOffset2 < 16 && nOffset + nOffset2 < nPacketLength; nOffset2++)
			{
				char szTemp[16];
				wsprintf(szTemp, "%02x ", pSectionPointer[nOffset + nOffset2]);
				lstrcat(szOutput, szTemp);
				if (nOffset2 == 7)
					lstrcat(szOutput, " ");
			}
			while (lstrlen(szOutput) < 52)
				lstrcat(szOutput, " ");
			for (nOffset2 = 0; nOffset2 < 16 && nOffset + nOffset2 < nPacketLength; nOffset2++)
			{
				char szTemp[16];
				szTemp[0] = pSectionPointer[nOffset + nOffset2];
				if (pSectionPointer[nOffset + nOffset2] < 32)
					szTemp[0] = '.';
				szTemp[1] = '\0';
				lstrcat(szOutput, szTemp);
			}
			lstrcat(szOutput, "\r\n");
			lstrcat(szFullOutput, szOutput);
		}
		SendMessage(v->hTableViewerSectionDisplayWindow, WM_SETTEXT, 0, (LPARAM)szFullOutput);
	}
#endif PRO
}
#endif LITE

void CheckForTableMonitor(int nPID)
{
#ifndef LITE
	if (!v->fTableMonitorRunning)
		return;

	if (nPID == v->nTableMonitorPID)
	{
		if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x40) == 0x40)		// PES/PSI start?
		{
			int nPointer = tv->pIncomingBuffer[tv->nBufferOffset + 4] + v->nPATPointerKludge;
			if ( (nPointer > tv->nPacketLength) || (nPointer < 0) )
			{
				v->nTableMonitorFillPtr = 0;
				return;
			}
			if (v->nTableMonitorFillPtr > 0)
			{
				// Complete the rest of this section
				if (nPointer > 0)
				{
					memcpy(&v->bTableMonitorBuffer[v->nTableMonitorFillPtr], &tv->pIncomingBuffer[tv->nBufferOffset + 5], nPointer);
					v->nTableMonitorFillPtr += nPointer;
				}
				
				// Now process the table
				LogTableMonitorData();
				v->nTableMonitorFillPtr = 0;
			}
			if ( (v->nTableMonitorFillPtr + (tv->nPacketLength - 5 - nPointer) > 65536)	// make sure it's going to fit
				|| (v->nTableMonitorFillPtr + (tv->nPacketLength - 5 - nPointer) < 1) )
			{
				v->nTableMonitorFillPtr = 0;
				return;
			}
			memcpy(&v->bTableMonitorBuffer[v->nTableMonitorFillPtr], &tv->pIncomingBuffer[5 + tv->nBufferOffset + nPointer], tv->nPacketLength - 5 - nPointer);
			v->nTableMonitorFillPtr += tv->nPacketLength - 5 - nPointer;
		}
		else
		{
			if (v->nTableMonitorFillPtr != 0)
			{
				memcpy(&v->bTableMonitorBuffer[v->nTableMonitorFillPtr], &tv->pIncomingBuffer[4 + tv->nBufferOffset], tv->nPacketLength - 4);
				v->nTableMonitorFillPtr += tv->nPacketLength - 4;
			}
		}
		if (v->nTableMonitorFillPtr > 65536 - tv->nPacketLength)
		{
			v->nTableMonitorFillPtr = 0;
			return;
		}
	}
#endif LITE
}

void CheckForESParsing(int nPID, int nES)
{
	if (v->nESParsePID[nES] != v->nNullPID && !v->fDisableBlacklisting)
	{
		if (v->lnPIDCounter[nPID] == 0)
			v->nDecodeNoPIDTrafficCounter[nES]++;
		if (v->nDecodeNoPIDTrafficCounter[nES] > MAX_DECODE_NO_PID_COUNTER)
		{
			OutputDebugString("TSReader: ES blacklisted - nDecodeNoPIDTrafficCounter > MAX_DECODE_NO_PID_COUNTER\n");
			if (v->fArchiveRunning == FALSE)
				v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].nBlacklisted = BLACKLIST_NO_TRAFFIC;
			tv->nPESLength[nES] = 0;
			tv->nESFillPtr[nES] = 0;
			tv->fBufferSections[nES] = FALSE;
			EnterCriticalSection(&v->csNextESPID);			
			GetNextESPID(FALSE, nES);
			LeaveCriticalSection(&v->csNextESPID);
			return;
		}
	}

	if (v->lnESParseStartTime[nES] && v->lnMuxRatePCR) //todo
	{
#ifdef _DEBUG
		__int64 lnTimeoutSeconds = 30;
#else _DEBUG
		__int64 lnTimeoutSeconds = 15;
#endif _DEBUG

		if (v->lnMuxRatePCR > v->lnESParseStartTime[nES] + (lnTimeoutSeconds * (__int64)27000000) && !v->fDisableBlacklisting)
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: ES PID 0x%04x blacklisted - no appreciable data for %d seconds\n", v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].nESPID, (int)lnTimeoutSeconds);
			OutputDebugString(szTemp);
			if (v->fArchiveRunning == FALSE)
				v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].nBlacklisted = BLACKLIST_NO_DATA;
			tv->nPESLength[nES] = 0;
			tv->nESFillPtr[nES] = 0;
			tv->fBufferSections[nES] = FALSE;
			EnterCriticalSection(&v->csNextESPID);
			GetNextESPID(FALSE, nES);
			LeaveCriticalSection(&v->csNextESPID);
			return;
		}
	}

	if (v->nESParsePID[nES] == nPID) 
	{
		int nAdaptation = (tv->pIncomingBuffer[tv->nBufferOffset + 3] >> 4) & 0x03;
		BYTE * pWritePtr;
		int nWriteLen = 0;

		switch(nAdaptation)
		{
		case 0:		// not allowed
			break;	
		case 1:		// no adaptation
			pWritePtr = &tv->pIncomingBuffer[tv->nBufferOffset + 4];
			nWriteLen = 184;
			break;
		case 2:		// entirely adaptation
			break;
		case 3:		// adaptation + payload
			{
				int nAdaptationLen = tv->pIncomingBuffer[tv->nBufferOffset + 4];
				pWritePtr = &tv->pIncomingBuffer[tv->nBufferOffset + 5 + nAdaptationLen];
				nWriteLen = tv->nPacketLength - 5 - nAdaptationLen;
			}
			break;
		}

		if (nWriteLen == 0 && !v->fDisableBlacklisting)
		{
			v->nDecodeNoPESLengthCounter[nES]++;
			if (v->nDecodeNoPESLengthCounter[nES] > MAX_DECODE_NO_PES_COUNTER)
			{
				OutputDebugString("ES blacklisted - nDecodeNoPESLengthCounter[nES] > MAX_DECODE_NO_PES_COUNTER\n");
				if (v->fArchiveRunning == FALSE)
					v->pat.pmt[v->nESParsePMTIndex[nES]].es[v->nESParseESIndex[nES]].nBlacklisted = BLACKLISTED_NO_PES_PACKETS;
				tv->nPESLength[nES] = 0;
				tv->nESFillPtr[nES] = 0;
				tv->fBufferSections[nES] = FALSE;
				EnterCriticalSection(&v->csNextESPID);
				GetNextESPID(FALSE, nES);
				LeaveCriticalSection(&v->csNextESPID);
				return;
			}
		}
		else
		{
			v->nDecodeNoPESLengthCounter[nES] = 0;
			if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x40) == 0x40)		// PES start?
			{
				BOOL fRetVal = FALSE;

				// If we have data from the last packet, send it to the decoder thread
				if (tv->nESFillPtr[nES])
				{
					// ES PID processing
					if ( (v->fPIDScrambled[nPID] == FALSE) && (v->fPIDActive[nPID]) ) 
					{								
						if (tv->nPESLength[nES] == -1)
							tv->nPESLength[nES] = tv->nESFillPtr[nES];
						switch(v->nESParseType[nES])
						{
						case PARSE_ES_TYPE_MPEG2_VIDEO:
							fRetVal = DecodeMPEG2Video(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
#ifdef PRO
						case PARSE_ES_TYPE_VC1_VIDEO:
							fRetVal = DecodeVC1Video(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						case PARSE_ES_TYPE_AUDIO_TITLE:
							fRetVal = DecodeAudioTitleData(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
#endif PRO
#ifndef LITE
						case PARSE_ES_TYPE_MPEG2_AAC_AUDIO:
							fRetVal = DecodeMPEGAACAudio(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						case PARSE_ES_TYPE_MPEG4_AAC_AUDIO:
							fRetVal = DecodeMPEGAACAudio(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						case PARSE_ES_TYPE_H264_VIDEO:
							fRetVal = DecodeH264Video(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						case PARSE_ES_TYPE_MPEG4_VIDEO:
							fRetVal = DecodeMPEG4Video(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
#endif LITE
						case PARSE_ES_TYPE_MPEG_AUDIO:
							fRetVal = DecodeMPEGAudioHeader(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						case PARSE_ES_TYPE_AC3_AUDIO:
							fRetVal = DecodeAC3AudioHeader(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						case PARSE_ES_TYPE_TELETEXT:
							fRetVal = DecodeTeletextVBIHeader(tv->bESBuffer[nES], tv->nPESLength[nES], nES);
							break;
						}
					}
					else
					{
						if ( (v->fPIDScrambled[nPID] == TRUE) && (v->fWaitForCAThumbnail == TRUE) )
						{
							if (GetTickCount() >= v->dwWaitForCACounter)
							{
								if (v->fShowScrambledChannels == TRUE)
									SetupScrambledChannelThumbnail(v->nESParsePMTIndex[nES], v->nESParseESIndex[nES]);
								fRetVal = TRUE;		// scrambled and didn't descramble
							}
						}
						else
						{
							if (v->fShowScrambledChannels == TRUE)
								SetupScrambledChannelThumbnail(v->nESParsePMTIndex[nES], v->nESParseESIndex[nES]);
							fRetVal = TRUE;		// scrambled PID - move onto the next one
						}
					}

					if (fRetVal)
					{
						tv->nPESLength[nES] = 0;
						tv->nESFillPtr[nES] = 0;
						tv->fBufferSections[nES] = FALSE;
						EnterCriticalSection(&v->csNextESPID);
						GetNextESPID(FALSE, nES);
						LeaveCriticalSection(&v->csNextESPID);
					}
				}
				
				// Now start a new packet
				if (fRetVal == FALSE)
				{
					int nPTSDTSLength = 9;
					int nPESHeaderLength = 3;

					if (tv->fBufferSections[nES] == FALSE)
					{
						tv->nPESLength[nES] = pWritePtr[4] << 8 | pWritePtr[5];
						switch(pWritePtr[7] & 0xc0)
						{
						case 0x80:
							nPTSDTSLength += 5;
							nPESHeaderLength += 5;
							break;
						case 0xc0:
							nPTSDTSLength += 10;
							nPESHeaderLength += 10;
							break;
						}
					}
					else
						nPTSDTSLength = 0;

					if (nWriteLen - nPTSDTSLength > 0)
					{
						memcpy(tv->bESBuffer[nES], pWritePtr + nPTSDTSLength, nWriteLen - nPTSDTSLength);
						tv->nESFillPtr[nES] = nWriteLen - nPTSDTSLength;
					}

					if (tv->nPESLength[nES] != 0)
						tv->nPESLength[nES] -= nPESHeaderLength; // minus header
					else
						tv->nPESLength[nES] = -1;
				}
			}
			else
			{
				if (tv->nPESLength[nES])
				{
					if (nWriteLen > 0)
					{
						if (tv->nESFillPtr[nES] + nWriteLen < ES_BUFFER_SIZE)
						{
							memcpy(tv->bESBuffer[nES] + tv->nESFillPtr[nES], pWritePtr, nWriteLen);
							tv->nESFillPtr[nES] += nWriteLen;
						}
					}
				}
			}
		}
	}
}

void GetNextECMPID()
{
	do
	{
		int nProgramInfoLength;
		int nOffset;
		BYTE * pDescriptor;

		v->nDCIIECMPMTIndex++;
		if (v->pat.pmt[v->nDCIIECMPMTIndex].nPMTPID == 0)
		{
			v->nDCIIECMDescriptorPID = -1;	// all done
			//UpdateMainStatusText("");
			PostMessage(v->hDlgSIParser, WM_USER + 3, 0, 1);
			return;
		}
		pDescriptor = v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo;
		if (pDescriptor == NULL)
			continue;
		nProgramInfoLength = v->pat.pmt[v->nDCIIECMPMTIndex].nProgramInfoLength;
		if (nProgramInfoLength == 0)
			continue;
		nOffset = 0;
		while (nProgramInfoLength)
		{
			int nDescriptor = v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo[nOffset];
			int nDescriptorLength = v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo[nOffset + 1];

			if (nDescriptor == 9) // CA
			{
				int CA_system_ID = v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo[nOffset + 2] << 8 | v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo[nOffset + 3];
				if (CA_system_ID == 0x4749)	// GI?
				{
					char szTemp[128];

					int CA_PID = (v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo[nOffset + 4] << 8 | v->pat.pmt[v->nDCIIECMPMTIndex].pProgramInfo[nOffset + 5]) & 0x1fff;
					if (v->fPIDActive[CA_PID] != 0)
					{
						char szTemp2[128];

						v->nDCIIECMDescriptorPID = CA_PID;
						v->nDCIIECMDescriptorTimeout = 0;
						wsprintf(szTemp, "Parsing DCII ECM for service %d on PID 0x%04x", v->pat.pmt[v->nDCIIECMPMTIndex].nProgramNumber, v->nDCIIECMDescriptorPID);
						UpdateMainStatusText(szTemp);
						wsprintf(szTemp2, "TSReader: %s\n", szTemp);
						OutputDebugString(szTemp2);
						return;		// ready to process
					}
				}
			}
			nOffset += nDescriptorLength + 2;
			nProgramInfoLength -= nDescriptorLength + 2;			
		}
	} while (TRUE);
}

void PMTParsingComplete()
{
	int nES;
	int nMaxThreads = 1;
	HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);

#ifndef LITE
	int nPMTListenIndex;

	for (nPMTListenIndex = 0; nPMTListenIndex < MAX_PAT_ENTRIES; nPMTListenIndex++)
		v->pmtlisten[nPMTListenIndex].nFillPtr = 0;

#endif LITE

	v->nPMTPID = v->nNullPID; 
	v->ss.nPATCATProcessed |= 3;
	v->nFillPtr[tv->nBufferID] = 0;
	
#ifdef PRO
	if (!v->fWaitForCAThumbnail)
		nMaxThreads = v->nMaximumThumbnailThreads;
#endif PRO
	EnterCriticalSection(&v->csNextESPID);
	for (nES = 0; nES < nMaxThreads; nES++)
	{
		tv->nPESLength[nES] = 0;
		tv->nESFillPtr[nES] = 0;
		tv->fBufferSections[nES] = FALSE;
		if (GetNextESPID(FALSE, nES) == 0x1fff)
			break;
	}
	LeaveCriticalSection(&v->csNextESPID);

	SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, FALSE, 0);
	SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, TRUE, 0);
	if (v->nAutoRecord != AUTO_RECORD_NONE)
	{
		AutoRecord();
	}
	else if (v->nAutoRecordPIDsPID[0] != 0x1fff)
		AutoRecordPIDs();
	else
	{
		if (!v->fTreeViewSelectedAtLeastOnce)
		{
			TreeView_SelectItem(hWndTV, v->pat.hPATTreeItem);
			TreeView_SelectSetFirstVisible(hWndTV, v->pat.hPATTreeItem);
		}
	}

#ifdef PRO
	if (v->fAutomaticForwarding)
		PostMessage(v->hWndMainWindow, WM_COMMAND, ID_FORWARD_FORWARDTOUDP, 0);
	if (v->fAutomaticStreamMonitor)
	{
		v->fAutomaticStreamMonitor = FALSE;
		PostMessage(v->hWndMainWindow, WM_COMMAND, ID_VIEW_STREAMMONITOR, 0);
	}
	if (v->fAutomaticRecordAll)
		PostMessage(v->hWndMainWindow, WM_COMMAND, ID_RECORD_RECORDALLPROGRAMS, 0);
#endif PRO
#ifndef LITE
	if (v->nCommandLineChart)
	{
		if (v->wChartMenuItems[v->nCommandLineChart - 1] != 0)
			PostMessage(v->hWndMainWindow, WM_COMMAND, v->wChartMenuItems[v->nCommandLineChart - 1], 0);
		v->nCommandLineChart = 0;
	}
#endif LITE

	// See about parsing DCII ECM PIDs to get channel names
	if (v->nNetworkPID == 0x0ffe)
		GetNextECMPID();

	// Turn back on EIT on interfaces with a demux
	if (v->nMaxSourcePIDs < 8192)
	{
		PIDManagement(TRUE, v->nEITPID, TRUE);
	}
}

void SetupForNextPMTEntry()
{
	if (v->nPMTPID == 0x0000)
	{
		v->nPMTPID = v->nNullPID;
		v->ss.nPATCATProcessed |= 3;
		v->nFillPtr[tv->nBufferID] = 0;
		EnterCriticalSection(&v->csNextESPID);
		GetNextESPID(FALSE, 0); //todo
		LeaveCriticalSection(&v->csNextESPID);
		UpdateMainStatusText("No PAT - possibly an IP/DVB mux or may require manual channels");
		if (v->nAutoRecord != AUTO_RECORD_NONE)
			AutoRecord();
		else if (v->nAutoRecordPIDsPID[0] != 0x1fff)
			AutoRecordPIDs();
		return;
	}
retry_pmt:
	v->nPMTProgramIndex++;
	if (   (v->pat.pmt[v->nPMTProgramIndex].nPMTPID == 0)
		|| (v->pat.pmt[v->nPMTProgramIndex].nProgramNumber == 0)
		|| (v->pat.pmt[v->nPMTProgramIndex].nProgramNumber >= 65534)
		|| (v->fIgnorePMT65500 && (v->pat.pmt[v->nPMTProgramIndex].nProgramNumber >= 65500))
		|| (v->fIgnorePMT800x0ff6 && (v->pat.pmt[v->nPMTProgramIndex].nProgramNumber == 80) & (v->pat.pmt[v->nPMTProgramIndex].nPMTPID == 0x0ff6))
	   )
	{
		// We're done with the PMTs
		PMTParsingComplete();
	}
	else
	{
		// check for NULL PID for the PMT
		if (v->pat.pmt[v->nPMTProgramIndex].nPMTPID == 0x1fff)
			goto retry_pmt;

		// onto the next one
		v->nPMTPID = v->pat.pmt[v->nPMTProgramIndex].nPMTPID;
		PIDManagement(TRUE, v->nPMTPID, TRUE);
		v->nPMTTimeoutCounter = 0;
		{
			char szTemp[128];
			wsprintf(szTemp, "TSReader: Reading PMT for program %d from PID %s\n", v->pat.pmt[v->nPMTProgramIndex].nProgramNumber, FormatTooltipPID(v->nPMTPID));
			OutputDebugString(szTemp);
			UpdateMainStatusText(szTemp);
		}
	}
}

void PostPATProcessing()
{
	int nCount;
	char szTemp[128];

	v->ss.nPATCATProcessed |= 1;
	//PIDManagement(FALSE, 0x0000, FALSE); - don't turn off - we now parse PAT all the time

	for (nCount = 0; nCount < MAX_PAT_ENTRIES; nCount++)
	{
		if (v->pat.pmt[nCount].nPMTPID == 0)
			break;
	}
	if (!(lstrlen(v->szAutoLoadManualChannelFilename)))
		qsort(v->pat.pmt, nCount, sizeof(PMT), SortPMTCompareFunction);
	PostMessage(v->hDlgSIParser, WM_USER + 2, SI_PARSER_PAT, 0);

#ifndef LITE
	if (v->nMaxSourcePIDs == 8192 && !v->fFastPMTParserDisabled)
	{
		int i;
		int nListenCount = 0;
		for (i = 0; i < MAX_PAT_ENTRIES; i++)
		{
			if (v->pat.pmt[i].nPMTPID == 0)
				break;
			if (!(v->pat.pmt[i].nProgramNumber == 0 ||
				  v->pat.pmt[i].nPMTPID == -2 || 
				  v->pat.pmt[i].nPMTPID == 0x1fff ||
				  v->pat.pmt[i].nProgramNumber >= 65534 ||
				  (v->fIgnorePMT65500 && (v->pat.pmt[i].nProgramNumber >= 65500)) ||
				  (v->fIgnorePMT800x0ff6 && (v->pat.pmt[i].nProgramNumber == 80) & (v->pat.pmt[i].nPMTPID == 0x0ff6))						  
				  ))
			{
				v->pmtlisten[i].fOutstanding = TRUE;
				v->pmtlisten[i].nPID = v->pat.pmt[i].nPMTPID;
				v->pmtlisten[i].nProgramNumber = v->pat.pmt[i].nProgramNumber;
				v->pmtlisten[i].dwStartTickCount = GetTickCount();
				nListenCount++;
			}
		}
		if (!nListenCount)
		{
			UpdateMainStatusText("No channels defined in mux");
			v->nPMTPID = 0x1fff;
			v->ss.nPATCATProcessed |= 3;
			PMTParsingComplete();
		}
		else
		{
			UpdateMainStatusText("Waiting for PMTs to arrive...");
			v->nPMTPID = -2;
		}
	}
	else
#endif LITE
	{
		v->nPMTProgramIndex = 0;	
		while (v->pat.pmt[v->nPMTProgramIndex].nProgramNumber == 0
			   || v->pat.pmt[v->nPMTProgramIndex].nPMTPID == -2
			   || v->pat.pmt[v->nPMTProgramIndex].nPMTPID == 0x1fff)
		{
			v->nPMTProgramIndex++;				// go with first real program
			if (v->nPMTProgramIndex == MAX_PAT_ENTRIES)
				break;
		}
		
		if (v->pat.pmt[v->nPMTProgramIndex].nPMTPID)
		{
			v->nPMTPID = v->pat.pmt[v->nPMTProgramIndex].nPMTPID;
			PIDManagement(TRUE, v->nPMTPID, TRUE);
		}
		else
			v->nPMTPID = 0x1fff;

		v->nPMTTimeoutCounter = 0;
		if (v->nPMTPID != 0x1fff)
		{
			wsprintf(szTemp, "TSReader: Reading PMT for program %d from PID %s\n", v->pat.pmt[v->nPMTProgramIndex].nProgramNumber, FormatTooltipPID(v->nPMTPID));
			OutputDebugString(szTemp);
		}
		else
			lstrcpy(szTemp, "No channels defined in mux");
		UpdateMainStatusText(szTemp);
	}
}

BOOL CheckPATVersionNumber(BYTE * pSectionPointer, int nPacketLength)
{
	int nTableID, nSectionLength;
	int nVersionNumber;

	nTableID = pSectionPointer[0];
	if (nTableID != 0)
		return FALSE;

	nSectionLength = ((pSectionPointer[1] << 8) + pSectionPointer[2]) & 0xfff;
	if ( (nSectionLength <= 0) || (nSectionLength > 1024) )
		return FALSE;

	if (SourceHelper_CRC_Check(pSectionPointer, nSectionLength + 3) != TRUE)
	{
		// Let's see if this is a screwy ONN like feed with an extra
		// byte after the pointer
		if (   (pSectionPointer[0] == 0x00)
			&& (pSectionPointer[1] == 0x00) 
			&& ((pSectionPointer[2] & 0x80) == 0x80) 
			&& (v->nPATPointerKludge == 0) )
		{
			v->nPATPointerKludge = 1;
			return FALSE;
		}
		v->nSIParserCRCs[SI_PARSER_STATS_PAT]++;
		if (!v->fIgnoreTableCRCErrors)
			return FALSE;
	}

	if ((pSectionPointer[5] & 1) == 0)	// not current_next
		return FALSE;

	nVersionNumber = (pSectionPointer[5] >> 1) & 0x1f;

	// If it's a different version, let's re-generate the PMT in memory
	if (nVersionNumber != v->pat.nVersionNumber)
	{
		char szTemp[128];
		wsprintf(szTemp, "TSReader: PAT Version changed from %d to %d\n", nVersionNumber, v->pat.nVersionNumber);
		OutputDebugString(szTemp);
		v->pat.nVersionNumber = nVersionNumber;
		return TRUE;		// need a restart!
	}

	return FALSE;
}

#ifdef PRO
void RecordTableData(int nRecordIndex, BYTE * pPacket, int nPacketLength)
{
	do
	{
		if (*pPacket != 0xff)
			break;
		pPacket++;
		nPacketLength--;
	} while (nPacketLength > 0);
	if (nPacketLength == 0)
		return;

	while (nPacketLength > 0)
	{
		int nTableID = pPacket[0];
		int nSectionLength = (pPacket[1] << 8 | pPacket[2]) & 0x0fff;

		if (nTableID >= v->record_tables[nRecordIndex].nStartTable && nTableID <= v->record_tables[nRecordIndex].nEndTable)
		{
			DWORD dwWritten;
			int nPacketDifference = -1;

			if (v->fRecordTablesHexASCII)
			{
				int nOutputByteCount = nSectionLength + 3;
				int nCurrentOutputIndex = 0;
				char szTemp[256];
				char szASCII[64];

				if (v->record_tables[nRecordIndex].dwPriorTickCount == 0)
					v->record_tables[nRecordIndex].dwPriorTickCount = GetTickCount();
				else
				{
					DWORD dwTickNow = GetTickCount();
					nPacketDifference = (int)(dwTickNow - v->record_tables[nRecordIndex].dwPriorTickCount);
					v->record_tables[nRecordIndex].dwPriorTickCount = dwTickNow;
				}
				wsprintf(szTemp, "Table Count %d PID %s", 
					     v->record_tables[nRecordIndex].nTableCount++, 
						 FormatTooltipPID(v->record_tables[nRecordIndex].nPID));
				if (nPacketDifference != -1)
				{
					char szTemp2[64];
					wsprintf(szTemp2, " Delta %d ms", nPacketDifference);
					lstrcat(szTemp, szTemp2);
				}
				lstrcat(szTemp, "\r\n");
				WriteFile(v->record_tables[nRecordIndex].hFile, szTemp, lstrlen(szTemp), &dwWritten, NULL);

				szTemp[0] = szASCII[0] = '\0';
				while (nOutputByteCount)
				{
					char szTemp2[64];
					wsprintf(szTemp2, "%02x ", pPacket[nCurrentOutputIndex]);
					lstrcat(szTemp, szTemp2);
					if ((pPacket[nCurrentOutputIndex] & 0x7f) < ' ')
						lstrcpy(szTemp2, ".");
					else
						wsprintf(szTemp2, "%c", pPacket[nCurrentOutputIndex]);
					lstrcat(szASCII, szTemp2);
					nCurrentOutputIndex++;
					if (nCurrentOutputIndex % 16 == 0 && nCurrentOutputIndex > 0)
					{
						lstrcat(szTemp, " ");
						lstrcat(szTemp, szASCII);
						lstrcat(szTemp, "\r\n");
						WriteFile(v->record_tables[nRecordIndex].hFile, szTemp, lstrlen(szTemp), &dwWritten, NULL);
						szTemp[0] = szASCII[0] = '\0';
					}
					nOutputByteCount--;
				}
				if (lstrlen(szTemp))
				{
					while (lstrlen(szTemp) < 50)
						lstrcat(szTemp, " ");
					lstrcat(szTemp, szASCII);
					lstrcat(szTemp, "\r\n");
					WriteFile(v->record_tables[nRecordIndex].hFile, szTemp, lstrlen(szTemp), &dwWritten, NULL);
				}
				lstrcpy(szTemp, "\r\n");
				WriteFile(v->record_tables[nRecordIndex].hFile, szTemp, lstrlen(szTemp), &dwWritten, NULL);
			}
			else
			{
				WriteFile(v->record_tables[nRecordIndex].hFile, pPacket, nSectionLength + 3, &dwWritten, NULL);
			}
		}

		pPacket += nSectionLength + 3;
		nPacketLength -= nSectionLength + 3;
	}
}
#endif PRO

void DispatchSIPacket()
{
#ifdef DEBUG_MESSAGES
	{
		char szTemp[128];
		wsprintf(szTemp, "TSReader: DispatchSIPacket+ %d\n", tv->nBufferID);
		OutputDebugString(szTemp);
	}
#endif DEBUG_MESSAGES
	if (tv->nBufferID >= BUFFER_EIT0 && tv->nBufferID <= BUFFER_EIT63)
		ParseATSCEITPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID], tv->nBufferID - BUFFER_EIT0);
	else if (tv->nBufferID >= BUFFER_ETT0 && tv->nBufferID <= BUFFER_ETT63)
		ParseATSCETTPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
	else
	{
		switch(tv->nBufferID)
		{
		case BUFFER_EIT:
			if (v->nNetworkPID == 0x0010)		// must be DVB
			{
				v->nSIParserPackets[SI_PARSER_STATS_EIT]++;
				ParseDVBEITPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
		case BUFFER_SDT:
			if (v->nNetworkPID == 0x0010 || v->nNetworkPID == -1) // must be DVB
			{
				v->nSIParserPackets[SI_PARSER_STATS_SDT]++;
				ParseDVBSDTPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			else if (v->nNetworkPID == 0x0ffe && v->nDCIIECMDescriptorPID != -1)
			{
				ParseDCIIECMPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
		case BUFFER_NETWORK:
			v->nSIParserPackets[SI_PARSER_STATS_NIT]++;
			if (v->nNetworkPID == 0x0010)
				ParseDVBNITPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			else
				ParseDCIINetworkPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			break;
		case BUFFER_CAT:
			v->nSIParserPackets[SI_PARSER_STATS_CAT]++;
			if (ParseCATPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]) == TRUE)
			{
				PostMessage(v->hDlgSIParser, WM_USER + 2, SI_PARSER_CAT, 0);
				v->fDidCAT = TRUE;
				v->ss.nPATCATProcessed |= 3;
				PIDManagement(FALSE, 0x0001, FALSE);
			}
			break;
		case BUFFER_PSIP:
			if (v->nNetworkPID == -1)
			{
				v->nNetworkPID = 0x1ffb;
				v->nSIParserPackets[SI_PARSER_STATS_SDT] = 0;
			}
			ParsePSIPPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			break;
		case BUFFER_RST:
			if (v->nNetworkPID == 0x0010)		// must be DVB
			{
				v->nSIParserPackets[SI_PARSER_STATS_RST]++;
				ParseDVBRSTPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
		case BUFFER_TDT:
			if (v->nNetworkPID == 0x0010)		// must be DVB
			{
				v->nSIParserPackets[SI_PARSER_STATS_TDT]++;
				ParseDVBTDTPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
		case BUFFER_BIT:
			if (v->nNetworkPID == 0x0010)		// must be at least DVB
			{
				ParseISDBBITPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
		case BUFFER_CETT:
			ParseATSCCETT(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			break;
#ifdef PRO
		case BUFFER_INT:
			if (v->nNetworkPID == 0x0010)		// must be DVB
			{
				ParseDVBINTPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
		case BUFFER_RECORD_TABLES1:
		case BUFFER_RECORD_TABLES2:
		case BUFFER_RECORD_TABLES3:
		case BUFFER_RECORD_TABLES4:
		case BUFFER_RECORD_TABLES5:
		case BUFFER_RECORD_TABLES6:
		case BUFFER_RECORD_TABLES7:
		case BUFFER_RECORD_TABLES8:
		case BUFFER_RECORD_TABLES9:
		case BUFFER_RECORD_TABLES10:
		case BUFFER_RECORD_TABLES11:
		case BUFFER_RECORD_TABLES12:
		case BUFFER_RECORD_TABLES13:
		case BUFFER_RECORD_TABLES14:
		case BUFFER_RECORD_TABLES15:
		case BUFFER_RECORD_TABLES16:
			{
				int nRecordIndex = tv->nBufferID - BUFFER_RECORD_TABLES1;
				RecordTableData(nRecordIndex, v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]);
			}
			break;
#endif PRO
#ifndef LITE
		case BUFFER_SKY_EPG_PID1:
		case BUFFER_SKY_EPG_PID2:
		case BUFFER_SKY_EPG_PID3:
		case BUFFER_SKY_PID_80:
		case BUFFER_SKY_PID_81:
		case BUFFER_SKY_PID_85:
			if (v->fSkyEPG)
				ParseSkyEPG(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID], tv->nBufferID == BUFFER_SKY_EPG_PID1);
			break;
#endif LITE
		case BUFFER_PAT_PMT:
			if ((tv->nBufferID == 3) && (v->nPMTPID == v->nNullPID))	// we're done with the PAT/PMT/CAT
			{
				v->nSIParserPackets[SI_PARSER_STATS_PAT]++;
				if (CheckPATVersionNumber(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]) == TRUE)
				{
#ifndef LITE
					if (v->fAutoRestartOnPATVersionChange)
					{
						if (v->fAutoRestartNoTuneDialog == TRUE)
							v->fAutoRestartNoDialogInProgress = TRUE;				
						if (!v->fAutoRestartNoBeep)
							PostMessage(v->hWndMainWindow, WM_USER + 10, 0, 0);
						PostMessage(v->hWndMainWindow, WM_COMMAND, ID_FILE_RESTART_SOURCE, 0);
					}
#endif LITE
				}
				break;
			}
			switch(v->nPMTPID)
			{
			case 0:	// doing the PAT
				v->nSIParserPackets[SI_PARSER_STATS_PAT]++;
				if (ParsePATPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID]) == TRUE)
					PostPATProcessing();
				break;
			default:	// probably doing a PMT
#ifndef LITE
				if (v->nMaxSourcePIDs == 8192 && !v->fFastPMTParserDisabled)
					break;
#endif LITE
				if (v->nPMTPID != v->nNullPID)
				{
					v->nSIParserPackets[SI_PARSER_STATS_PMT]++;
					if (ParsePMTPacket(v->bSIBuffer[tv->nBufferID],
						v->nFillPtr[tv->nBufferID],
						v->pat.pmt[v->nPMTProgramIndex].nProgramNumber, -1) == TRUE)
					{
						PostMessage(v->hDlgSIParser, WM_USER + 2, SI_PARSER_PMT, v->nPMTProgramIndex);
						SetupForNextPMTEntry();
					}
				}
				break;
			}
			break;
		}
	}
	memset(v->bSIBuffer[tv->nBufferID], 0, sizeof(v->bSIBuffer[tv->nBufferID]));
#ifdef DEBUG_MESSAGES
	OutputDebugString("TSReader: DispatchSIPacket-\n");
#endif DEBUG_MESSAGES
}

#ifndef LITE
void PMTListenPMTParser(BYTE * pSectionBuffer, int nSectionLength, int nPMTListenIndex)
{
	char szTemp[128];
			 
	if (ParsePMTPacket(pSectionBuffer, nSectionLength, v->pmtlisten[nPMTListenIndex].nProgramNumber, nPMTListenIndex) == TRUE)
	{
		int i;
		int nOutstanding = 0;
		char szPMTString[8] = {"PMT"};

		v->pmtlisten[nPMTListenIndex].fOutstanding = FALSE;
		v->nSIParserPackets[SI_PARSER_STATS_PMT]++;
		PostMessage(v->hDlgSIParser, WM_USER + 2, SI_PARSER_PMT, nPMTListenIndex);

		for (i = 0; i < MAX_PAT_ENTRIES; i++)
		{
			if (v->pmtlisten[i].fOutstanding)
				nOutstanding++;
		}
		if (nOutstanding > 1)
			lstrcat(szPMTString, "s");
		if (v->nPMTPID == v->nNullPID)
			wsprintf(szTemp, "TSReader: Completed PMT for program %d on PID 0x%04x", v->pmtlisten[nPMTListenIndex].nProgramNumber, v->pmtlisten[nPMTListenIndex].nPID);
		else
			wsprintf(szTemp, "TSReader: Completed PMT for program %d on PID 0x%04x - %d %s left to process", v->pmtlisten[nPMTListenIndex].nProgramNumber, v->pmtlisten[nPMTListenIndex].nPID, nOutstanding, szPMTString);
		UpdateMainStatusText(szTemp);
		lstrcat(szTemp, "\n");
		OutputDebugString(szTemp);
	}
	else
	{
		if (v->nPMTPID == v->nNullPID)
			v->nSIParserPackets[SI_PARSER_STATS_PMT]++;
	}
}

void PMTListenParser(int nPMTListenIndex, int nPID, BYTE * pPacket)
{
	if ((pPacket[1] & 0x40) == 0x40)			// PES/PSI start?
	{
		int nPointer = pPacket[4] + v->pmtlisten[nPMTListenIndex].nPMTPointerKludge;

		if (nPointer > 188 || nPointer < 0)
		{
#ifdef DEBUG_MESSAGES
			char szTemp[128];
			wsprintf(szTemp, "TSReader: nPointer out of range = %d PMT PID = 0x%04x\n", nPointer, nPID);
			OutputDebugString(szTemp);
#endif DEBUG_MESSAGES
		}
		else
		{
			if (v->pmtlisten[nPMTListenIndex].nFillPtr > 0)
			{
				// Complete the rest of this section
				if (nPointer > 0)
				{
					memcpy(&v->pmtlisten[nPMTListenIndex].bSectionBuffer[v->pmtlisten[nPMTListenIndex].nFillPtr], &pPacket[5], nPointer);
					v->pmtlisten[nPMTListenIndex].nFillPtr += nPointer;
				}
						
				// Now process the section
				PMTListenPMTParser(v->pmtlisten[nPMTListenIndex].bSectionBuffer, v->pmtlisten[nPMTListenIndex].nFillPtr, nPMTListenIndex);
				v->pmtlisten[nPMTListenIndex].nFillPtr = 0;
			}
			if ( (v->pmtlisten[nPMTListenIndex].nFillPtr + (188 - 5 - nPointer) > 1024)	// make sure it's going to fit
				|| (v->pmtlisten[nPMTListenIndex].nFillPtr + (188 - 5 - nPointer) < 1) )
			{
#ifdef DEBUG_MESSAGES
				char szTemp[128];
				wsprintf(szTemp, "TSReader: Out of range nPointer = %d nFillPtr = %d PMT PID = 0x%04x\n", nPointer, v->pmtlisten[nPMTListenIndex].nFillPtr, nPID);
				OutputDebugString(szTemp);
#endif DEBUG_MESSAGES
				v->nFillPtr[tv->nBufferID] = 0;
			}
			else
			{
				memcpy(&v->pmtlisten[nPMTListenIndex].bSectionBuffer[v->pmtlisten[nPMTListenIndex].nFillPtr], &pPacket[5 + nPointer], 188 - 5 - nPointer);
				v->pmtlisten[nPMTListenIndex].nFillPtr += 188 - 5 - nPointer;
			}
		}
	}
	else
	{
		if (v->pmtlisten[nPMTListenIndex].nFillPtr > 0)
		{
			memcpy(&v->pmtlisten[nPMTListenIndex].bSectionBuffer[v->pmtlisten[nPMTListenIndex].nFillPtr], &pPacket[4], 188 - 4);
			v->pmtlisten[nPMTListenIndex].nFillPtr += tv->nPacketLength - 4;
		}
		if (v->pmtlisten[nPMTListenIndex].nFillPtr > 1024)
			v->pmtlisten[nPMTListenIndex].nFillPtr = 0;
	}
}
#endif LITE

void BufferSections(int nPID)
{
	if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x40) == 0x40)		// PES/PSI start?
	{
		tv->nPointer = tv->pIncomingBuffer[tv->nBufferOffset + 4] + v->nPATPointerKludge;
		if ( (tv->nPointer > tv->nPacketLength) || (tv->nPointer < 0) )
		{
#ifdef DEBUG_MESSAGES
			char szTemp[128];
			wsprintf(szTemp, "TSReader: tv->nPointer out of range = %d buffer = %d\n", tv->nPointer, tv->nBufferID);
			OutputDebugString(szTemp);
#endif DEBUG_MESSAGES
			return;
		}
		if (v->nFillPtr[tv->nBufferID] > 0)
		{
			// Complete the rest of this section
			if (tv->nPointer > 0)
			{
				memcpy(&v->bSIBuffer[tv->nBufferID][v->nFillPtr[tv->nBufferID]], &tv->pIncomingBuffer[tv->nBufferOffset + 5], tv->nPointer);
				v->nFillPtr[tv->nBufferID] += tv->nPointer;
			}
			
			// Now process the section
			if (v->fIPDVBMode == FALSE)
				DispatchSIPacket();
			else
				ParseIPPacket(v->bSIBuffer[tv->nBufferID], v->nFillPtr[tv->nBufferID], nPID, tv->nBufferID);
			v->nFillPtr[tv->nBufferID] = 0;
		}
		if ( (v->nFillPtr[tv->nBufferID] + (tv->nPacketLength - 5 - tv->nPointer) > 65536)	// make sure it's going to fit
			|| (v->nFillPtr[tv->nBufferID] + (tv->nPacketLength - 5 - tv->nPointer) < 1) )
		{
#ifdef DEBUG_MESSAGES
			char szTemp[128];
			wsprintf(szTemp, "TSReader: Out of range nPointer = %d Buffer = %d\n", v->nFillPtr[tv->nBufferID], tv->nBufferID);
			OutputDebugString(szTemp);
#endif DEBUG_MESSAGES
			v->nFillPtr[tv->nBufferID] = 0;
			return;
		}
		memcpy(&v->bSIBuffer[tv->nBufferID][v->nFillPtr[tv->nBufferID]], &tv->pIncomingBuffer[5 + tv->nBufferOffset + tv->nPointer], tv->nPacketLength - 5 - tv->nPointer);
		v->nFillPtr[tv->nBufferID] += tv->nPacketLength - 5 - tv->nPointer;
	}
	else
	{
		if (v->nFillPtr[tv->nBufferID] != 0)
		{
			memcpy(&v->bSIBuffer[tv->nBufferID][v->nFillPtr[tv->nBufferID]], &tv->pIncomingBuffer[4 + tv->nBufferOffset], tv->nPacketLength - 4);
			v->nFillPtr[tv->nBufferID] += tv->nPacketLength - 4;
		}
	}
	if (v->nFillPtr[tv->nBufferID] > 65536 - tv->nPacketLength)
	{
		v->nFillPtr[tv->nBufferID] = 0;
		return;
	}
}


DWORD WINAPI ParseIncomingTSDataThread(LPVOID lpv)
{
	int i;

	tv = LocalAlloc(LPTR, sizeof(TSPARSERVARIABLES));
	for (i = 0; i < MAX_ES_PARSERS; i++)
	{
		tv->bESBuffer[i] = LocalAlloc(LPTR, ES_BUFFER_SIZE);
		tv->nPESLength[i] = 0;
		tv->nESFillPtr[i] = 0;
		tv->fBufferSections[i] = FALSE;
	}

#ifndef LITE
#ifdef PRO
	tv->bCCESBuffer = LocalAlloc(LPTR, ES_BUFFER_SIZE);
#endif PRO
	for (i = 0; i < MAX_CHARTS; i++)
		tv->bVideoESBuffer[i] = LocalAlloc(LPTR, ES_BUFFER_SIZE);
#endif LITE
	tv->nBufferNumber = -1;
	tv->nPreviousBuffers = -1;
	tv->nPacketLength = 188;
	tv->nActivityByteCounter = 0;
	tv->fGotAdaptation = TRUE;
	tv->nActualMaxRecordBuffers = ACTUAL_MAX_RECORD_BUFFERS;

#ifdef DEBUG_MESSAGES
	OutputDebugString("TSReader: +ParseIncomingTSDataThread()\n");
#endif DEBUG_MESSAGES

	if (v->fParserDisabled)
	{
		EnableWindow(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), FALSE);
		EnableWindow(GetDlgItem(v->hDlgSIParser, IDC_SI_TEXT), FALSE);
	}

	v->nNullPID = 0x1fff;	// assume MPEG-2 null PID
	if (v->nEITPID <= 0 || v->nEITPID >= 8191)
		v->nEITPID = 0x0012;

	v->fParseThreadRunning = TRUE;

	for (i = 0; i < MAX_SECTION_BUFFERS; i++)
		v->nFillPtr[i] = 0;
	v->fDidCAT = FALSE;
	v->nPATPointerKludge = 0;
	v->nPMTPointerKludge = 0;

	for (i  = 0; i < tv->nActualMaxRecordBuffers; i++)
	{
		tv->pOutputRecordPackets[i] = LocalAlloc(LPTR, TS_BUFFER_SIZE);
		tv->dwRecordSize[i] = 0;
		tv->fRecordBufferActive[i] = FALSE;
	}

	do
	{
		RecordOutstandingTSBuffers();

		// Setup for next buffer
		tv->nBufferNumber++;
		if (tv->nBufferNumber == MAX_TS_BUFFERS)
			tv->nBufferNumber = 0;
		
		if (WaitForNextTSBuffer())
		{
			OutputDebugString("TSReader: quitting main processing loop\n");
			break;
		}
		v->fDataReceviedInParseIncomingDataThread = TRUE;

		// Now get the incoming data
		tv->pIncomingBuffer = v->ss.tsb[tv->nBufferNumber].pData;
		tv->pIncomingTimestamps = v->ss.tsb[tv->nBufferNumber].pTimestamps;
		tv->nIncomingBufferLength = v->ss.tsb[tv->nBufferNumber].nSize;
		tv->nBufferOffset = 0;

		// MD Plugins and CSA descrambler if they're there		
		if (v->fMDPluginsLoaded == TRUE)
			MD__DataToFilters(tv->pIncomingBuffer, &tv->nIncomingBufferLength, &tv->nPacketLength);

		// Check to see if we switched to DSS mode
		if (tv->pIncomingBuffer[0] == 0x1d && !v->fNoDSSSupport)
		{
			if (tv->nPacketLength == 188)
			{
				OutputDebugString("TSReader: Switching to DSS packet mode\n");
				tv->nPacketLength = 131;
				v->nNullPID = 0;
				v->nMuxRatePID = 0;
			}
		}

#ifdef PRO
		if (v->fMDIIndexActive)
		{
			MDITIME ts;
			__int64 lnCurrentCounterValue;
			double dnsPerTick;

			dnsPerTick = 1000000000.0 / (double)v->lnTicksPerSecond;
			QueryPerformanceCounter((LARGE_INTEGER *)&lnCurrentCounterValue);
			ts = lnCurrentCounterValue * (__int64)dnsPerTick;
			EnterCriticalSection(&v->csMDI);
			mdiPacket(ts, tv->pIncomingBuffer, tv->nIncomingBufferLength, mdi);
			LeaveCriticalSection(&v->csMDI);
		}
#endif PRO

		// Bump the activity indicator
		tv->nActivityByteCounter += tv->nIncomingBufferLength;
		if (tv->nActivityByteCounter > 1024 * 1024)
		{
			tv->nActivityByteCounter -= 1024 * 1024;
			v->nInputActivityPosition++;
			if (v->nInputActivityPosition > 3)
				v->nInputActivityPosition = 0;
		}

		// Record entire transport stream or setup for individual PIDs
		if (v->fRecording == TRUE)
		{
#ifndef LITE
			if (v->fRecordAllTS == TRUE)
				RecordAllTS(tv->pIncomingBuffer, tv->nIncomingBufferLength, tv->nPacketLength);
			else
#endif LITE
			{
				if (v->fRecordPIDMode == TRUE)
				{
					int nBufferIndex;
					for (nBufferIndex = 0; nBufferIndex < tv->nActualMaxRecordBuffers; nBufferIndex++)
					{
						tv->dwRecordSize[nBufferIndex] = 0;
						tv->fRecordBufferActive[nBufferIndex] = TRUE;
					}
				}
				else
				{
					tv->dwRecordSize[0] = 0;
					tv->fRecordBufferActive[0] = TRUE;
				}
			}
		}

#ifdef PRO
		// Potentially pass to the Pro feature parsers, but not for DSS packets
		if (tv->nPacketLength == 188)
		{
			if (v->fArchiveRunning)
				ArchiveProgramData(tv->pIncomingBuffer, tv->nIncomingBufferLength);
			if (v->fForwarderEnabled || v->fwd.nForwarderModulesActive)
				ForwardProgramData(tv->pIncomingBuffer, tv->nIncomingBufferLength);
			if (v->fMonitorRunning)
				MonitorProgramData(tv->pIncomingBuffer, tv->nIncomingBufferLength);
		}
#endif PRO

		// Loop for each packet
		do
		{
			int nPID;
			
			tv->fIgnoreContinuity = FALSE;

			if (tv->nPacketLength == 188)
				nPID = (tv->pIncomingBuffer[tv->nBufferOffset + 1] << 8 | tv->pIncomingBuffer[tv->nBufferOffset + 2]) & 0x1fff;
			else
				nPID = (tv->pIncomingBuffer[tv->nBufferOffset + 1] << 8 | tv->pIncomingBuffer[tv->nBufferOffset + 2]) & 0xfff;

			EnterCriticalSection(&v->ss.csPIDCounter);
			v->lnPIDCounter[nPID]++;
			v->fPIDActive[nPID] = TRUE;
			v->lnTotalTSPackets++;
			if (tv->nPacketLength == 188)
			{
				if ((tv->pIncomingBuffer[tv->nBufferOffset + 3] & 0xc0) != 0)
				{
					v->fPIDScrambled[nPID] = TRUE;
					if (v->cat.nVersionNumber == -1)
						v->nSIParserTableErrors[SI_PARSER_STATS_CAT]++;
				}
				else
					v->fPIDScrambled[nPID] = FALSE;
			}
			else
			{
				if ((tv->pIncomingBuffer[tv->nBufferOffset + 1] & 0x20) == 0)
				{
					v->fPIDScrambled[nPID] = TRUE;
					tv->fIgnoreContinuity = TRUE;
				}
				else
					v->fPIDScrambled[nPID] = FALSE;
			}
			LeaveCriticalSection(&v->ss.csPIDCounter);

			CheckPIDContinuity(nPID);
			CheckPIDTEI(nPID);
			if (tv->nPacketLength == 188)
				MPEG2MuxrateProcessing(nPID);
			else
				DSSMuxrateProcessing(nPID);

			if (v->fParserDisabled)
				continue;

			// See if we need to flush buffers when we switch to IP mode
			if (v->fIPDVBModeChanged == TRUE)
			{
				v->fIPDVBModeChanged = FALSE;
				for (i = 0; i < MAX_SECTION_BUFFERS; i++)
					v->nFillPtr[i] = 0;
			}

			// Record a channel if selected
			if (v->fRecording == TRUE)
			{
				if ( (v->fRecordAllTS == FALSE) && (v->fRecordPIDMode == FALSE) )
					BufferRecordChannelData(nPID);

				if (v->fRecordPIDMode == TRUE)
				{
					if (v->nIPMonitorPID[nPID] != -1)
					{
						int nActualBuffer = v->nIPMonitorPID[nPID];
						if (v->fRecordPIDsOneFile == TRUE)
							nActualBuffer = 0;
						if (tv->fRecordBufferActive[nActualBuffer])
						{
							memcpy(&tv->pOutputRecordPackets[nActualBuffer][tv->dwRecordSize[nActualBuffer]], &tv->pIncomingBuffer[tv->nBufferOffset], tv->nPacketLength);
							tv->dwRecordSize[nActualBuffer] += tv->nPacketLength;
							if (v->fRecordPIDsOneFile == FALSE)
								tv->hRecordPIDFile[nActualBuffer] = v->hRecordPIDFile[nPID];
							else
								tv->hRecordPIDFile[nActualBuffer] = v->hRecordPIDFile[0];
						}
					}
				}
			}

			tv->nBufferID = -1;
			if (v->fIPDVBMode == FALSE)
			{
				int nES;
				int nMaxThreads = 1;

				if (nPID == v->nNullPID)
					continue;

#ifdef PRO
				if (!v->fWaitForCAThumbnail)
					nMaxThreads = v->nMaximumThumbnailThreads;
#endif PRO
				for (nES = 0; nES < nMaxThreads; nES++)
					CheckForESParsing(nPID, nES);
#ifndef LITE
#ifdef PRO
				if (v->nCaptionPID != -1)
					CheckForGeneralESParsing(nPID, v->nCaptionPID, tv->bCCESBuffer, &tv->nCCPESLength, &tv->nCCESFillPtr, 0, 0);
				else
					tv->nCCESFillPtr = 0;
#endif PRO
				{
					int nChartIndex = 0;
					for (nChartIndex = 0; nChartIndex < MAX_CHARTS; nChartIndex++)
					{
						if (v->nVideoCompositionPID[nChartIndex] != -1)
						{
							int nParseType = 1;

							if ((v->nVideoCompositionPID[nChartIndex] & 0xffff0000) == 0x00010000)
								nParseType = 2;
							CheckForGeneralESParsing(nPID,
													 v->nVideoCompositionPID[nChartIndex] & 0x1fff,
													 tv->bVideoESBuffer[nChartIndex],
													 &tv->nVideoPESLength[nChartIndex],
													 &tv->nVideoESFillPtr[nChartIndex],
													 nParseType,
													 nChartIndex);
						}
					}
				}
#endif LITE
				CheckForTableMonitor(nPID);

#ifndef LITE
				if (v->nPMTPID != v->nNullPID && v->nMaxSourcePIDs != 8192 && !v->fFastPMTParserDisabled)
#else LITE
				if (v->nPMTPID != v->nNullPID)
#endif LITE
				{
					if (v->nPMTTimeoutCounter >= PMT_TIMEOUT)
					{
						char szTemp[128];
						wsprintf(szTemp, "TSReader: Timeout on PMT for program %d\n", v->pat.pmt[v->nPMTProgramIndex].nProgramNumber);
						OutputDebugString(szTemp);
						SetupForNextPMTEntry();
					}
				}

				if (tv->nPacketLength != 188)
				{
					if (nPID == 0x0001)
						tv->nBufferID = BUFFER_PAT_PMT;
				}
				else
				{
					if (nPID == v->nEITPID && !v->fSkyEPG)
					{
						if (!v->fIgnoreEIT)
						{
							if (v->nNetworkPID == 0x0010)		// must be DVB
								tv->nBufferID = BUFFER_EIT;
						}
					}
					else
					{
						if (nPID == v->nPMTPID)
							tv->nBufferID = BUFFER_PAT_PMT;
						else if (nPID == 0x1ffb && !v->fIgnorePSIP)
							tv->nBufferID = BUFFER_PSIP;
						else if (nPID == v->nNetworkPID)
							tv->nBufferID = BUFFER_NETWORK;
						else if (v->nPMTPID == v->nNullPID && nPID == 0x0000)
						{
							// done with PAT/PMT parsing but we keep an eye on the PAT for changes
							tv->nBufferID = BUFFER_PAT_PMT;		
						}
						else if (nPID == 0x0011 && (v->nNetworkPID == 0x0010 || v->nNetworkPID == -1) && !v->fIgnoreEIT)
							tv->nBufferID = BUFFER_SDT;
						else if (v->nNetworkPID == 0x0ffe && v->nDCIIECMDescriptorPID == nPID)
							tv->nBufferID = BUFFER_SDT;
						else if (nPID == 0x0013 && v->nNetworkPID == 0x0010)
							tv->nBufferID = BUFFER_RST;
						else if (nPID == 0x0014 && v->nNetworkPID == 0x0010)
							tv->nBufferID = BUFFER_TDT;
						else if (nPID == 0x0001)
							tv->nBufferID = BUFFER_CAT;
						else if (nPID == 0x0024 && v->fISDB)
							tv->nBufferID = BUFFER_BIT;
#ifdef PRO
						else if (nPID == v->nINTPID && v->nINTService)
							tv->nBufferID = BUFFER_INT;
#endif PRO
						else if (nPID == v->nATSCCETTPID) tv->nBufferID = BUFFER_CETT;
						else if (nPID == v->nATSCEITPID[0] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT0;
						else if (nPID == v->nATSCEITPID[1] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT1;
						else if (nPID == v->nATSCEITPID[2] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT2;
						else if (nPID == v->nATSCEITPID[3] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT3;
						else if (nPID == v->nATSCEITPID[4] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT4;
						else if (nPID == v->nATSCEITPID[5] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT5;
						else if (nPID == v->nATSCEITPID[6] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT6;
						else if (nPID == v->nATSCEITPID[7] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT7;
						else if (nPID == v->nATSCEITPID[8] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT8;
						else if (nPID == v->nATSCEITPID[9] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT9;
						else if (nPID == v->nATSCEITPID[10] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT10;
						else if (nPID == v->nATSCEITPID[11] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT11;						
						else if (nPID == v->nATSCEITPID[12] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT12;						
						else if (nPID == v->nATSCEITPID[13] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT13;						
						else if (nPID == v->nATSCEITPID[14] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT14;						
						else if (nPID == v->nATSCEITPID[15] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT15;						
						else if (nPID == v->nATSCEITPID[16] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT16;						
						else if (nPID == v->nATSCEITPID[17] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT17;						
						else if (nPID == v->nATSCEITPID[18] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT18;						
						else if (nPID == v->nATSCEITPID[19] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT19;						
						else if (nPID == v->nATSCEITPID[20] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT20;						
						else if (nPID == v->nATSCEITPID[21] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT21;						
						else if (nPID == v->nATSCEITPID[22] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT22;						
						else if (nPID == v->nATSCEITPID[23] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT23;												
						else if (nPID == v->nATSCEITPID[24] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT24;
						else if (nPID == v->nATSCEITPID[25] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT25;
						else if (nPID == v->nATSCEITPID[26] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT26;
						else if (nPID == v->nATSCEITPID[27] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT27;
						else if (nPID == v->nATSCEITPID[28] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT28;
						else if (nPID == v->nATSCEITPID[29] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT29;
						else if (nPID == v->nATSCEITPID[30] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT30;
						else if (nPID == v->nATSCEITPID[31] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT31;
						else if (nPID == v->nATSCEITPID[32] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT32;
						else if (nPID == v->nATSCEITPID[33] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT33;
						else if (nPID == v->nATSCEITPID[34] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT34;
						else if (nPID == v->nATSCEITPID[35] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT35;						
						else if (nPID == v->nATSCEITPID[36] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT36;						
						else if (nPID == v->nATSCEITPID[37] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT37;						
						else if (nPID == v->nATSCEITPID[38] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT38;						
						else if (nPID == v->nATSCEITPID[39] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT39;						
						else if (nPID == v->nATSCEITPID[40] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT40;						
						else if (nPID == v->nATSCEITPID[41] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT41;						
						else if (nPID == v->nATSCEITPID[42] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT42;						
						else if (nPID == v->nATSCEITPID[43] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT43;						
						else if (nPID == v->nATSCEITPID[44] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT44;						
						else if (nPID == v->nATSCEITPID[45] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT45;						
						else if (nPID == v->nATSCEITPID[46] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT46;						
						else if (nPID == v->nATSCEITPID[47] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT47;
						else if (nPID == v->nATSCEITPID[48] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT48;
						else if (nPID == v->nATSCEITPID[49] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT49;
						else if (nPID == v->nATSCEITPID[50] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT50;
						else if (nPID == v->nATSCEITPID[51] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT51;
						else if (nPID == v->nATSCEITPID[52] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT52;
						else if (nPID == v->nATSCEITPID[53] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT53;
						else if (nPID == v->nATSCEITPID[54] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT54;
						else if (nPID == v->nATSCEITPID[55] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT55;
						else if (nPID == v->nATSCEITPID[56] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT56;
						else if (nPID == v->nATSCEITPID[57] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT57;
						else if (nPID == v->nATSCEITPID[58] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT58;
						else if (nPID == v->nATSCEITPID[59] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT59;
						else if (nPID == v->nATSCEITPID[60] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT60;
						else if (nPID == v->nATSCEITPID[61] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT61;
						else if (nPID == v->nATSCEITPID[62] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT62;
						else if (nPID == v->nATSCEITPID[63] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_EIT63;
						else if (nPID == v->nATSCETTPID[0] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT0;
						else if (nPID == v->nATSCETTPID[1] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT1;
						else if (nPID == v->nATSCETTPID[2] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT2;
						else if (nPID == v->nATSCETTPID[3] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT3;
						else if (nPID == v->nATSCETTPID[4] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT4;
						else if (nPID == v->nATSCETTPID[5] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT5;
						else if (nPID == v->nATSCETTPID[6] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT6;
						else if (nPID == v->nATSCETTPID[7] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT7;
						else if (nPID == v->nATSCETTPID[8] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT8;
						else if (nPID == v->nATSCETTPID[9] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT9;
						else if (nPID == v->nATSCETTPID[10] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT10;
						else if (nPID == v->nATSCETTPID[11] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT11;
						else if (nPID == v->nATSCETTPID[12] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT12;
						else if (nPID == v->nATSCETTPID[13] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT13;
						else if (nPID == v->nATSCETTPID[14] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT14;
						else if (nPID == v->nATSCETTPID[15] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT15;
						else if (nPID == v->nATSCETTPID[16] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT16;
						else if (nPID == v->nATSCETTPID[17] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT17;
						else if (nPID == v->nATSCETTPID[18] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT18;
						else if (nPID == v->nATSCETTPID[19] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT19;
						else if (nPID == v->nATSCETTPID[20] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT20;
						else if (nPID == v->nATSCETTPID[21] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT21;
						else if (nPID == v->nATSCETTPID[22] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT22;
						else if (nPID == v->nATSCETTPID[23] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT23;
						else if (nPID == v->nATSCETTPID[24] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT24;
						else if (nPID == v->nATSCETTPID[25] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT25;
						else if (nPID == v->nATSCETTPID[26] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT26;
						else if (nPID == v->nATSCETTPID[27] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT27;
						else if (nPID == v->nATSCETTPID[28] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT28;
						else if (nPID == v->nATSCETTPID[29] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT29;
						else if (nPID == v->nATSCETTPID[30] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT30;
						else if (nPID == v->nATSCETTPID[31] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT31;
						else if (nPID == v->nATSCETTPID[32] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT32;
						else if (nPID == v->nATSCETTPID[33] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT33;
						else if (nPID == v->nATSCETTPID[34] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT34;
						else if (nPID == v->nATSCETTPID[35] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT35;						
						else if (nPID == v->nATSCETTPID[36] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT36;						
						else if (nPID == v->nATSCETTPID[37] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT37;						
						else if (nPID == v->nATSCETTPID[38] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT38;						
						else if (nPID == v->nATSCETTPID[39] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT39;						
						else if (nPID == v->nATSCETTPID[40] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT40;						
						else if (nPID == v->nATSCETTPID[41] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT41;						
						else if (nPID == v->nATSCETTPID[42] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT42;						
						else if (nPID == v->nATSCETTPID[43] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT43;						
						else if (nPID == v->nATSCETTPID[44] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT44;						
						else if (nPID == v->nATSCETTPID[45] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT45;						
						else if (nPID == v->nATSCETTPID[46] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT46;						
						else if (nPID == v->nATSCETTPID[47] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT47;												
						else if (nPID == v->nATSCETTPID[48] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT48;
						else if (nPID == v->nATSCETTPID[49] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT49;
						else if (nPID == v->nATSCETTPID[50] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT50;
						else if (nPID == v->nATSCETTPID[51] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT51;
						else if (nPID == v->nATSCETTPID[52] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT52;
						else if (nPID == v->nATSCETTPID[53] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT53;
						else if (nPID == v->nATSCETTPID[54] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT54;
						else if (nPID == v->nATSCETTPID[55] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT55;
						else if (nPID == v->nATSCETTPID[56] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT56;
						else if (nPID == v->nATSCETTPID[57] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT57;
						else if (nPID == v->nATSCETTPID[58] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT58;
						else if (nPID == v->nATSCETTPID[59] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT59;
						else if (nPID == v->nATSCETTPID[60] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT60;
						else if (nPID == v->nATSCETTPID[61] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT61;
						else if (nPID == v->nATSCETTPID[62] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT62;
						else if (nPID == v->nATSCETTPID[63] && !v->fIgnoreEIT)  tv->nBufferID = BUFFER_ETT63;
#ifndef LITE
						else if (v->fSkyEPG && nPID == v->nSkyEPGPIDs[0])	tv->nBufferID =	BUFFER_SKY_EPG_PID1;
						else if (v->fSkyEPG && nPID == v->nSkyEPGPIDs[1])	tv->nBufferID =	BUFFER_SKY_EPG_PID2;
						else if (v->fSkyEPG && nPID == 80)	tv->nBufferID =	BUFFER_SKY_PID_80;
						else if (v->fSkyEPG && nPID == 81)	tv->nBufferID =	BUFFER_SKY_PID_81;
						//else if (v->fSkyEPG && nPID == 85)	tv->nBufferID =	BUFFER_SKY_PID_85;
#endif LITE
#ifndef LITE
						else
						{
							if (v->nMaxSourcePIDs == 8192 && !v->fFastPMTParserDisabled)
							{
								
								int nPMTListenIndex;
								if (v->nPMTPID == -2)
								{
									BOOL fAllComplete = TRUE;

									for (nPMTListenIndex = 0; nPMTListenIndex < MAX_PAT_ENTRIES; nPMTListenIndex++)
									{
										if (v->pmtlisten[nPMTListenIndex].fOutstanding == TRUE)
										{
											if (v->pmtlisten[nPMTListenIndex].nPID == nPID)
												PMTListenParser(nPMTListenIndex, nPID, &tv->pIncomingBuffer[tv->nBufferOffset]);
											if (GetTickCount() > v->pmtlisten[nPMTListenIndex].dwStartTickCount + 10000)
											{
												char szTemp[128];
												wsprintf(szTemp, "PMT timeout on program %d PID 0x%04x", 
														 v->pmtlisten[nPMTListenIndex].nProgramNumber,
														 v->pmtlisten[nPMTListenIndex].nPID);
												UpdateMainStatusText(szTemp);
												lstrcat(szTemp, "\n");
												OutputDebugString(szTemp);
												v->pmtlisten[nPMTListenIndex].fOutstanding = FALSE;
											}
										}
										if (v->pmtlisten[nPMTListenIndex].fOutstanding == TRUE)
											fAllComplete = FALSE;
									}
									if (fAllComplete)
										PMTParsingComplete();								
								}
								else if (v->nPMTPID == v->nNullPID && !v->fContinuousPMTParserDisabled)
								{
									int nPMTListenIndex;

									for (nPMTListenIndex = 0; nPMTListenIndex < MAX_PAT_ENTRIES; nPMTListenIndex++)
									{
										if (v->pmtlisten[nPMTListenIndex].nPID == nPID)
											PMTListenParser(nPMTListenIndex, nPID, &tv->pIncomingBuffer[tv->nBufferOffset]);
									}
								}
							}
						}
#endif LITE
					}
				}
			}
			else
			{
				// The v->nIPMonitorPID array elements are -1 if the PID isn't being watched
				// Otherwise the element contains the section buffer number - eight max currently
				tv->nBufferID = v->nIPMonitorPID[nPID];
			}
			if (tv->nBufferID != -1)
				BufferSections(nPID);
#ifdef PRO
			if (v->fRecordTablesActive)
			{
				int nTableIndex;

				for (nTableIndex = 0; nTableIndex < MAX_RECORD_TABLES; nTableIndex++)
				{
					if (v->record_tables[nTableIndex].nPID == nPID)
					{
						tv->nBufferID = BUFFER_RECORD_TABLES1 + nTableIndex;
						BufferSections(nPID);
					}
				}
			}
#endif PRO
		} while ((tv->nBufferOffset += tv->nPacketLength) < tv->nIncomingBufferLength );

	} while (TRUE);

	for (i  = 0; i < tv->nActualMaxRecordBuffers; i++)
		LocalFree(tv->pOutputRecordPackets[i]);

	for (i = 0; i < MAX_ES_PARSERS; i++)
		LocalFree(tv->bESBuffer[i]);
#ifndef LITE
#ifdef PRO
	LocalFree(tv->bCCESBuffer);
#endif PRO
	for (i = 0; i < MAX_CHARTS; i++)
		LocalFree(tv->bVideoESBuffer[i]);
#endif LITE
	CloseHandle(v->hStreamProcessingThread);
	LocalFree(tv);
	OutputDebugString("TSReader: ParseIncomingTSDataThread-\n");
	v->fParseThreadRunning = FALSE;
	return 0;
}

void CleanupIPParsingThread(HWND hDlg)
{
	int i;

	for (i = 0; i < 16; i++)
	{
		if (v->ippid[i].nPID)
		{
			PIPMACENTRY pMACCurrent = v->ippid[i].pMACEntries;
			while (pMACCurrent != NULL)
			{
				PIPMACENTRY pMACNext = (PIPMACENTRY)pMACCurrent->dwNext;
				PIPENTRY pIPCurrent = pMACCurrent->pIPEntries;
				while (pIPCurrent != NULL)
				{
					PIPENTRY pIPNext = (PIPENTRY)pIPCurrent->dwNext;
					LocalFree(pIPCurrent);
					pIPCurrent = pIPNext;
				}
				LocalFree(pMACCurrent);
				pMACCurrent = pMACNext;
			}
			v->ippid[i].pMACEntries = NULL;
			v->ippid[i].nPID = 0;
			v->ippid[i].hIPPIDRootItem = NULL;
		}
	}
}

#ifndef LITE
void ReadPersistantEPG()
{
	HANDLE hEITFile;
	char szPersistantFile[MAX_PATH];

	return;

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szPersistantFile, sizeof(szPersistantFile));
	lstrcat(szPersistantFile, "\\tsreader.persistant.eit");
	hEITFile = CreateFile(szPersistantFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hEITFile != INVALID_HANDLE_VALUE)
	{
		PEITEVENT pPrior = NULL;
		PEITEVENT pCurrent = NULL;

		do
		{
			int nChannel;
			int nDescriptorOffset = 0;
			int nFileDescriptorLength;
			int nTextLength;
			DWORD dwRead;

			ReadFile(hEITFile, &nChannel, sizeof(nChannel), &dwRead, NULL);
			if (dwRead == 0 || nChannel == 0)
				break;
			if (v->pEvents[nChannel] == NULL)
			{
				v->pEvents[nChannel] = LocalAlloc(LMEM_FIXED, sizeof(EITEVENT));
				pCurrent = v->pEvents[nChannel];
			}
			else
			{
				pCurrent = LocalAlloc(LMEM_FIXED, sizeof(EITEVENT));
				pPrior->dwNextEvent = (DWORD)pCurrent;
			}
			pPrior = pCurrent;
			ReadFile(hEITFile, pCurrent, sizeof(EITEVENT), &dwRead, NULL);
			pCurrent->dwNextEvent = 0;

			// Descriptors now			
			do
			{
				ReadFile(hEITFile, &nFileDescriptorLength, sizeof(nFileDescriptorLength), &dwRead, NULL);
				if (nFileDescriptorLength == 0)
					break;
				pCurrent->pExtraDescriptors[nDescriptorOffset] = LocalAlloc(LMEM_FIXED, nFileDescriptorLength);
				ReadFile(hEITFile, pCurrent->pExtraDescriptors[nDescriptorOffset], nFileDescriptorLength, &dwRead, NULL);
				nDescriptorOffset++;
			} while (nDescriptorOffset < MAX_EIT_EXTRA_DESCRIPTORS);

			// Short & long descriptions
			ReadFile(hEITFile, &nTextLength, sizeof(nTextLength), &dwRead, NULL);
			if (nTextLength)
			{
				pCurrent->szShortEventDescription = LocalAlloc(LMEM_FIXED, nTextLength);
				ReadFile(hEITFile, pCurrent->szShortEventDescription, nTextLength, &dwRead, NULL);
			}
			ReadFile(hEITFile, &nTextLength, sizeof(nTextLength), &dwRead, NULL);
			if (nTextLength)
			{
				pCurrent->szLongEventDescription = LocalAlloc(LMEM_FIXED, nTextLength);
				ReadFile(hEITFile, pCurrent->szLongEventDescription, nTextLength, &dwRead, NULL);
			}
		} while (TRUE);

		OutputDebugString("\n");
		CloseHandle(hEITFile);
		//DeleteFile(szPersistantFile);	
	}
}

void WritePersistantEPG()
{
	HANDLE hEITFile;
	char szPersistantFile[MAX_PATH];

	return;

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szPersistantFile, sizeof(szPersistantFile));
	lstrcat(szPersistantFile, "\\tsreader.persistant.eit");
	hEITFile = CreateFile(szPersistantFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hEITFile != INVALID_HANDLE_VALUE)
	{
		int nChannel;
		DWORD dwWritten;

		for (nChannel = 0; nChannel < MAX_EIT_CHANNEL_DATA; nChannel++)
		{
			PEITEVENT pCurrent;

			pCurrent = v->pEvents[nChannel];
			if (pCurrent != NULL)
			{
				do
				{
					int nDescriptorIndex;
					int nTextLength;

					// Write channel number and main EIT struct
					WriteFile(hEITFile, &nChannel, sizeof(nChannel), &dwWritten, NULL);
					WriteFile(hEITFile, pCurrent, sizeof(EITEVENT), &dwWritten, NULL);

					// Write descriptors
					for (nDescriptorIndex = 0; nDescriptorIndex  < MAX_EIT_EXTRA_DESCRIPTORS; nDescriptorIndex++)
					{
						if (pCurrent->pExtraDescriptors[nDescriptorIndex] != NULL)
						{
							int nTotalDescriptorLength = pCurrent->pExtraDescriptors[nDescriptorIndex][1] + 2;

							WriteFile(hEITFile, &nTotalDescriptorLength, sizeof(nTotalDescriptorLength), &dwWritten, NULL);
							WriteFile(hEITFile, pCurrent->pExtraDescriptors[nDescriptorIndex], nTotalDescriptorLength, &dwWritten, NULL);
						}
						else
							break;
					}
					nDescriptorIndex = 0;	// terminate descriptors
					WriteFile(hEITFile, &nDescriptorIndex, sizeof(nDescriptorIndex), &dwWritten, NULL);

					// Write short/long texts
					nTextLength = 0;
					if (pCurrent->szShortEventDescription != NULL)
						nTextLength = lstrlen(pCurrent->szShortEventDescription) + 1;
					WriteFile(hEITFile, &nTextLength, sizeof(nTextLength), &dwWritten, NULL);
					if (nTextLength)
						WriteFile(hEITFile, pCurrent->szShortEventDescription, nTextLength, &dwWritten, NULL);

					nTextLength = 0;
					if (pCurrent->szLongEventDescription != NULL)
						nTextLength = lstrlen(pCurrent->szLongEventDescription) + 1;
					WriteFile(hEITFile, &nTextLength, sizeof(nTextLength), &dwWritten, NULL);
					if (nTextLength)
						WriteFile(hEITFile, pCurrent->szLongEventDescription, nTextLength, &dwWritten, NULL);

					// Ready for the next one					
					pCurrent = (PEITEVENT)pCurrent->dwNextEvent;
				} while (pCurrent != NULL);
			}
		}
		nChannel = 0; // terminate with channel zero
		WriteFile(hEITFile, &nChannel, sizeof(nChannel), &dwWritten, NULL);

		CloseHandle(hEITFile);
	}
}

#endif LITE
void CleanupMPEGParsingThread(HWND hDlg)
{
	int i, j;
	int nPMTIndex, nESIndex;

	CursorWait(hDlg);

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: EIT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading EIT data...");
#ifndef LITE
	if (1) // fPersistantEPG
	{
		WritePersistantEPG();
	}
#endif LITE
	for (i = 0; i < MAX_EIT_CHANNEL_DATA; i++)
	{
		PEITEVENT pCurrent, pNext;

		pCurrent = v->pEvents[i];
		if (pCurrent != NULL)
		{
#ifdef DEBUG_MESSAGES
			{
				char szTemp[128];
				wsprintf(szTemp, "CleanupMPEGParsingThread: EIT extra descriptors channel %d\n", i);
				OutputDebugString(szTemp);
			}
#endif DEBUG_MESSAGES
			do
			{
				pNext = (PEITEVENT)pCurrent->dwNextEvent;
				for (j = 0; j < MAX_EIT_EXTRA_DESCRIPTORS; j++)
				{
					if (pCurrent->pExtraDescriptors[j] != NULL)
					{
						LocalFree(pCurrent->pExtraDescriptors[j]);
						pCurrent->pExtraDescriptors[j] = NULL;
					}
				}
				if (pCurrent->szShortEventDescription != NULL)
					LocalFree(pCurrent->szShortEventDescription);
				if (pCurrent->szLongEventDescription != NULL)
					LocalFree(pCurrent->szLongEventDescription);
				LocalFree(pCurrent);
				pCurrent = pNext;
			} while (pCurrent != NULL);
			v->pEvents[i] = NULL;
		}
		
		if (v->pChannelData[i] != NULL)
		{
			for (j = 0; j < MAX_SDT_EXTRA_DESCRIPTORS; j++)
			{
				if (v->pChannelData[i]->pExtraDescriptors[j] != NULL)
				{
					/*{
						char szTemp[128];
						wsprintf(szTemp, "v->pChannelData[%d]->pExtraDescriptors[%d] = %08x\n", i, j, v->pChannelData[i]->pExtraDescriptors[j]);
						OutputDebugString(szTemp);
					}*/
					LocalFree(v->pChannelData[i]->pExtraDescriptors[j]);
					v->pChannelData[i]->pExtraDescriptors[j] = NULL;
				}
			}

			LocalFree(v->pChannelData[i]);
			v->pChannelData[i] = NULL;
		}
	}

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: PMT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading PMT data...");
	for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
	{
		if (v->pat.pmt[nPMTIndex].pProgramInfo != NULL)
		{
			LocalFree(v->pat.pmt[nPMTIndex].pProgramInfo);
			v->pat.pmt[nPMTIndex].pProgramInfo = NULL;
			v->pat.pmt[nPMTIndex].nProgramInfoLength = 0;
		}

		for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
		{
			if (v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors != NULL)
			{
				LocalFree(v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors);
				v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors = NULL;
			}
			if (v->pat.pmt[nPMTIndex].es[nESIndex].pParsedData != NULL)
			{
				LocalFree(v->pat.pmt[nPMTIndex].es[nESIndex].pParsedData);
				v->pat.pmt[nPMTIndex].es[nESIndex].pParsedData = NULL;
			}
			if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
			{
				LocalFree(v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame);
				v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame = NULL;
			}
			v->pat.pmt[nPMTIndex].es[nESIndex].nESPID = 0;
			v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType = 0;
			v->pat.pmt[nPMTIndex].es[nESIndex].as.nChannels = 0;
		}
		memset(&v->pat.pmt[nPMTIndex], 0, sizeof(PMT));
	}
#ifndef LITE
	if (v->pat.pRawPAT != NULL)
	{
		LocalFree(v->pat.pRawPAT);
		v->pat.pRawPAT = NULL;
	}
#endif LITE

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: NIT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading NIT data...");
	for (i = 0; i < MAX_TS_ENTRIES; i++)
	{
		if (v->pNITData[i] != NULL)
		{
			for (j = 0; j < MAX_NIT_EXTRA_DESCRIPTORS; j++)
			{
				if (v->pNITData[i]->pExtraDescriptors[j] != NULL)
					LocalFree(v->pNITData[i]->pExtraDescriptors[j]);
				else
					break;
			}
			if (v->pNITData[i]->nNetworkDescriptorsLength)
				LocalFree(v->pNITData[i]->pNetworkDescriptors);
			LocalFree(v->pNITData[i]);
			v->pNITData[i] = NULL;
		}
	}

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: CAT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading CAT data...");
	for (i = 0; i < MAX_CAT_DESCRIPTORS; i++)
	{
		if (v->cat.pDescriptor[i] != NULL)
		{
			LocalFree(v->cat.pDescriptor[i]);
			v->cat.pDescriptor[i] = NULL;
		}
	}

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: MGT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading MGT data...");
	for (i = 0; i < MAX_MGT_ENTRIES; i++)
	{
		if (v->mgt[i].pDescriptors != NULL)
			LocalFree(v->mgt[i].pDescriptors);
		memset(&v->mgt[i], 0, sizeof(MGT));
		v->mgt[i].nTableType = -1;
	}
	if (v->pMGTDescriptors != NULL)
	{
		LocalFree(v->pMGTDescriptors);
		v->pMGTDescriptors = NULL;
	}

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: CVCT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading CVCT data...");
	for (i = 0; i < MAX_CVCT_ENTRIES; i++)
	{
		for (j = 0; j < MAX_CVCT_CHANNEL_ENTRIES; j++)
		{
			if (v->cvct[i].CVCTEntry[j].pDescriptors != NULL)
				LocalFree(v->cvct[i].CVCTEntry[j].pDescriptors);
			v->cvct[i].CVCTEntry[j].major_channel_number = v->cvct[i].CVCTEntry[j].minor_channel_number = -1;
		}
		if (v->cvct[i].pAdditionalDescriptors != NULL)
			LocalFree(v->cvct[i].pAdditionalDescriptors);
		memset(&v->cvct[i], 0, sizeof(CVCT));
		v->cvct[i].transport_stream_id = -1;
		for (j = 0; j < MAX_CVCT_CHANNEL_ENTRIES; j++)
			v->cvct[i].CVCTEntry[j].major_channel_number = v->cvct[i].CVCTEntry[j].minor_channel_number = -1;
	}

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: BAT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading BAT data...");
	for (i = 0; i < MAX_BAT_ENTRIES; i++)
	{
		if (v->bat[i].bouquet_descriptors != NULL)
			LocalFree(v->bat[i].bouquet_descriptors);
		for (j = 0; j < MAX_BAT_TRANSPORT_ITEMS; j++)
		{
			if (v->bat[i].batts[j].transport_descriptors != NULL)
				LocalFree(v->bat[i].batts[j].transport_descriptors);
		}
		memset(&v->bat[i], 0, sizeof(BAT));
	}

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: TOT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading TOT data...");
	if (v->dvbtot.pDescriptors != NULL)
		LocalFree(v->dvbtot.pDescriptors);
	memset(&v->dvbtot, 0, sizeof(v->dvbtot));

#ifdef DEBUG_MESSAGES
	OutputDebugString("CleanupMPEGParsingThread: RRT\n");
#endif DEBUG_MESSAGES
	UpdateMainStatusText("Unloading RRT data...");	
	for (i = 0; i < 256; i++)
	{
		if (v->prrt[i] != NULL)
		{
			LocalFree(v->prrt[i]);
			v->prrt[i] = NULL;
		}
	}

	//UpdateMainStatusText("");
	CursorNormal();
}

int AddSITreeIcon(int nResource)
{
	HICON hiconItem;
	int nRetVal;

	hiconItem = LoadImage(v->hInstance, MAKEINTRESOURCE(nResource), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	nRetVal = ImageList_AddIcon(v->hSIParserImageList, hiconItem); 
	if (nRetVal == -1)
	{
		OutputDebugString("TSReader: Image not loaded\n");
	}
	DestroyIcon(hiconItem);
	return nRetVal;
}

void InitSITreeViewImageLists(HWND hWndTV)
{
	int nIconID[] = {IDI_SI_PAT, // 0
		             IDI_SI_PMT, // 1
					 IDI_SI_EIT, // 2
					 IDI_SI_PCR, // 3
		             IDI_SI_AUDIO_ES, //4
					 IDI_SI_VIDEO_ES, //5
					 IDI_SI_USER_ES, //6
					 IDI_SI_SDT, //7
					 IDI_SI_DESCRIPTOR, //8
					 IDI_SI_NIT, //9
					 IDI_SI_CAT, //10
					 IDI_SI_IP,  //11
					 IDI_SI_VCT, //12
					 IDI_SI_TDT, //13
					 IDI_SI_SIT, //14
					 IDI_SI_CDT, //15
					 IDI_SI_MMT, //16
					 IDI_SI_USER_VBI, //17
					 IDI_SI_DATA, //18
					 IDI_SI_USER_SUBTL, //19
					 IDI_SI_MGT, //20
					 IDI_SI_IP_STREAMING, //21
					 IDI_SI_IP_SAVING, //22
					 IDI_SI_BAT, //23
					 IDI_SI_SDT_OTHER, //24
					 IDI_SI_NIT_OTHER, //25
					 IDI_SI_BAT_SELECTED, //26
					 IDI_SI_NIT_IGNORED, //27
					 0};

	v->hSIParserImageList = ImageList_Create(16, 16, ILC_MASK, 30, 50);
	if (v->hSIParserImageList != NULL)
	{
		int i;

		for (i = 0; nIconID[i] != 0; i++)
			v->nSITreeIcons[i] = AddSITreeIcon(nIconID[i]);

		// Associate the image list with the tree-view control. 
		TreeView_SetImageList(hWndTV, v->hSIParserImageList, TVSIL_NORMAL); 
	}

}

// AddItemToSITree - adds items to a tree-view control. 
// Returns the handle of the newly added item. 
// hwndTV - handle of the tree-view control 
// lpszItem - text of the item to add 
// nLevel - level at which to add the item 
HTREEITEM AddItemToSITree(HWND hwndTV, LPTSTR lpszItem, int nLevel, int lParam, int nIconIndex, HTREEITEM hParent, HTREEITEM hInsertAfter) 
{ 
	TV_INSERTSTRUCT tvins; 
	LPTV_ITEM tvi = &tvins.item; 
	static HTREEITEM hPrev = (HTREEITEM) TVI_FIRST; 
	static HTREEITEM hPrevRootItem = NULL; 
	static HTREEITEM hPrevLev2Item = NULL; 
 
	tvi->mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM ; 
 
    // Set the text of the item. 
    tvi->pszText = lpszItem; 
    tvi->cchTextMax = lstrlen(lpszItem); 
  
    // Save the heading level in the item's application-defined 
    // data area. 
    tvi->lParam = (LPARAM)lParam; 
    tvi->cChildren = 0;
    if (hInsertAfter == NULL)
		tvins.hInsertAfter = hPrev; 
	else
		tvins.hInsertAfter = hInsertAfter;

	tvi->iImage = v->nSITreeIcons[nIconIndex]; 
	tvi->iSelectedImage = v->nSITreeIcons[nIconIndex];

    // Set the parent item based on the specified level. 
	if (hParent != NULL)
		tvins.hParent = hParent;
	else
	{
		switch (nLevel) 
		{
		case 1:
			tvins.hParent = TVI_ROOT; 
			break;
		default:
			tvins.hParent = hPrevRootItem; 
			break;
		}
	}
	
    // Add the item to the tree-view control. 
    hPrev = (HTREEITEM) SendMessage(hwndTV, TVM_INSERTITEM, 0, (LPARAM)(LPTV_INSERTSTRUCT)&tvins); 
 
    // Save the handle of the item. 
    if (nLevel == 1) 
        hPrevRootItem = hPrev; 
    else if (nLevel == 2) 
        hPrevLev2Item = hPrev; 
 
    return hPrev; 
} 

int __cdecl SortPIDsByPackets(const void *elem1, const void *elem2)
{
	PPIDCOUNTER pPID1 = (PPIDCOUNTER)elem1;
	PPIDCOUNTER pPID2 = (PPIDCOUNTER)elem2;

	if (pPID1->lnPackets > pPID2->lnPackets)
		return -1;
	if (pPID1->lnPackets < pPID2->lnPackets)
		return 1;
	return 0;
}

int __cdecl SortPIDsByPID(const void *elem1, const void *elem2)
{
	PPIDCOUNTER pPID1 = (PPIDCOUNTER)elem1;
	PPIDCOUNTER pPID2 = (PPIDCOUNTER)elem2;

	if (pPID1->nPID < pPID2->nPID)
		return -1;
	if (pPID1->nPID > pPID2->nPID)
		return 1;
	return 0;
}

int GetPMTOffsetFromThumbnailScrollOffset()
{
	int nRetVal = 0;
	int nCounter = v->nThumbnailScrollOffset;
	int nPMTIndex, nESIndex;

	for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
	{
		for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
		{
			if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
				break;

			if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
			{
				nCounter--;
				if (nCounter == 0)
					return nRetVal + 1;
				if (nCounter < 0)
					return nRetVal;
			}
		}
		nRetVal++;
	}
	return nRetVal;
}

BOOL LocateCurrentProgram(int nPMTIndex, char * szCurrentProgram, char * szDescription, SYSTEMTIME * stStart, SYSTEMTIME * stDuration, BOOL fForDisplay)
{
	int nServiceID = v->pat.pmt[nPMTIndex].nProgramNumber;
	PEITEVENT pCurrent;
	PEITEVENT pTargetEvent = NULL;
	SYSTEMTIME stSystemTime;

	EnterCriticalSection(&v->csEIT);
	pCurrent = v->pEvents[nServiceID];
	if (pCurrent != NULL)
	{
		do
		{
			FILETIME ftProgramStart, ftNow;
			DWORD64 lnProgramStart, lnNow;
			DWORD64 lnMultiplier = 10000000;
			DWORD64 lnRunTime = ( (pCurrent->stRunTime.wHour * 60 * 60)
									   + (pCurrent->stRunTime.wMinute * 60)
									   + (pCurrent->stRunTime.wSecond) ) * lnMultiplier;

			GetSystemTime(&stSystemTime);
			SystemTimeToFileTime(&stSystemTime, &ftNow);
			memcpy(&lnNow, &ftNow, sizeof(DWORD64));

			SystemTimeToFileTime(&pCurrent->stStartTime, &ftProgramStart);
			memcpy(&lnProgramStart, &ftProgramStart, sizeof(DWORD64));

			if (lnNow >= lnProgramStart && lnNow <= lnProgramStart + lnRunTime)
				pTargetEvent = pCurrent;
			pCurrent = (PEITEVENT)pCurrent->dwNextEvent;
		} while (pCurrent != NULL);
		if (pTargetEvent != NULL)
		{
			int i;
			int nOutputIndex = 0;

			if (szCurrentProgram != NULL)
			{
				for (i = 0; i < lstrlen(pTargetEvent->szEventName); i++)
				{
					if (pTargetEvent->szEventName[i] == '&' && fForDisplay)
					{
						szCurrentProgram[nOutputIndex++] = '&';
						szCurrentProgram[nOutputIndex++] = '&';
					}
					else
						szCurrentProgram[nOutputIndex++] = pTargetEvent->szEventName[i];
				}
				szCurrentProgram[nOutputIndex] = '\0';
			}

			if (v->nNetworkPID == 0x1ffb)
			{
				for (i = 0; i < MAX_EIT_EXTRA_DESCRIPTORS; i++)
				{
					if (pTargetEvent->pExtraDescriptors[i] == NULL)
						break;
					switch(pTargetEvent->pExtraDescriptors[i][0])
					{
					case 0x87:
						{
							char szTemp[128] = {0};
							DecodeATSCContentAdvisoryDescriptor(szTemp, pTargetEvent->pExtraDescriptors[i], TRUE);
							if (lstrlen(szTemp))
							{
								if (szCurrentProgram != NULL)
								{
									lstrcat(szCurrentProgram, " [");
									lstrcat(szCurrentProgram, szTemp);
									lstrcat(szCurrentProgram, " ]");
								}
							}
						}
						break;
					}
				}
			}

			if (szDescription != NULL)
			{
				szDescription[0] = '\0';
				if (pTargetEvent->szShortEventDescription != NULL)
					lstrcpy(szDescription, pTargetEvent->szShortEventDescription);
			}
			if (stStart != NULL)
				memcpy(stStart, &pTargetEvent->stStartTime, sizeof(SYSTEMTIME));
			if (stDuration != NULL)
				memcpy(stDuration , &pTargetEvent->stRunTime, sizeof(SYSTEMTIME));

			LeaveCriticalSection(&v->csEIT);
			return TRUE;
		}
	}

	LeaveCriticalSection(&v->csEIT);
	return FALSE;
}

int GetNextThumbnailHeight(int nPMTIndex, int nESIndex)
{
	int nRetVal = 0;
	
	nESIndex++;
	for ( ; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
	{
		if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
			break;
		for ( ; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
		{
			if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
				break;
			if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame)
				return (v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight);
		}
		nESIndex = 0;
	}

	return nRetVal;
}

void DrawThumbnail(HDC hDC, int nPMTIndex, int nESIndex, int yCurrent, int yEnd, int xStart, int xEnd)
{
	int nLCN = GetLogicalChannelNumber(v->pat.pmt[nPMTIndex].nProgramNumber);
	int nThumbnailWidth, nThumbnailHeight;
	int nDrawHeight, nDrawWidth;
	DWORD dwChannelTextColor = RGB(0x00, 0xff, 0x00);
	char szChannelDescription[128];
	char szChannelNumber[32];
	char szCurrentProgram[128];

	EnterCriticalSection(&v->csThumbnails);
	if (v->pat.pmt[nPMTIndex].es[nESIndex].nVideoWidth * v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight * 3 <= sizeof(v->pNewPicture))
	{
		memcpy(v->pNewPicture,
			   v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame,
			   v->pat.pmt[nPMTIndex].es[nESIndex].nVideoWidth * v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight * 3);
		nThumbnailWidth = v->pat.pmt[nPMTIndex].es[nESIndex].nVideoWidth;
		nThumbnailHeight = v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight;
	}
	else
	{
		nThumbnailWidth = nThumbnailHeight = 0;
		OutputDebugString("TSReader: Image thumbnail too big!\n");
	}
	LeaveCriticalSection(&v->csThumbnails);
	if (nThumbnailWidth == 0 || nThumbnailHeight == 0)
		return;

	if (v->nSelectedProgram == nPMTIndex)
		dwChannelTextColor = RGB(0x00, 0x00, 0xff);

	if (nLCN == 0)
		wsprintf(szChannelNumber, "%d", v->pat.pmt[nPMTIndex].nProgramNumber);
	else
		wsprintf(szChannelNumber, "%d/%d", nLCN, v->pat.pmt[nPMTIndex].nProgramNumber);
	
	if (v->pat.pmt[nPMTIndex].fSetupSDTName && v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber] != NULL)
	{
		int nOutputIndex = 0;
		int nInputIndex;
		char szNewShortName[128];

		for (nInputIndex = 0; nInputIndex < lstrlen(v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->szShortName); nInputIndex++)
		{
			switch(v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->szShortName[nInputIndex])
			{
			case '&':
				szNewShortName[nOutputIndex++] = '&';
				//note - no break here
			default:
				szNewShortName[nOutputIndex++] = v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->szShortName[nInputIndex];
			}
		}
		szNewShortName[nOutputIndex] = '\0';
		wsprintf(szChannelDescription, "%s - %s", szChannelNumber, szNewShortName);
	}
	else
		wsprintf(szChannelDescription, "%s", szChannelNumber);

	_ISDrawTextOnRGB2(v->pNewPicture,
					  nThumbnailWidth,
					  nThumbnailHeight,
					  szChannelDescription,
					  &v->logfontChannelFont,
					  1,
					  1,
					  dwChannelTextColor);				
	if (   v->pat.pmt[nPMTIndex].es[nESIndex].nParseType == PARSE_ES_TYPE_MPEG2_VIDEO
		|| v->pat.pmt[nPMTIndex].es[nESIndex].nParseType == PARSE_ES_TYPE_H264_VIDEO
#ifdef PRO
		|| v->pat.pmt[nPMTIndex].es[nESIndex].nParseType == PARSE_ES_TYPE_VC1_VIDEO
#endif PRO
		|| v->pat.pmt[nPMTIndex].es[nESIndex].nParseType == PARSE_ES_TYPE_MPEG4_VIDEO)
	{
		if (LocateCurrentProgram(nPMTIndex, szCurrentProgram, NULL, NULL, NULL, TRUE) == TRUE)
		{
			_ISDrawTextOnRGB2(v->pNewPicture,
							  nThumbnailWidth,
							  nThumbnailHeight,
							  szCurrentProgram,
							  &v->logfontChannelFont,
							  1,
							  v->textsizeChannelFont.cy,
							  dwChannelTextColor);				
		}
		if (!v->fHideThumbnailIcons)
		{
			int nIconX = 1;
			int nServices = v->pat.pmt[nPMTIndex].es[nESIndex].nTeletextServices;
			if (nServices & VBI_SERVICE_USER)  {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_user, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_422)   {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_422, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_CC)    {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_cc, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_ITXT)  {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_itxt, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_SUB)   {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_sub, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_TXT)   {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_txt, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_VPS)   {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_vps, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_WSS)   {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_wss, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_DTVCC) {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_dtvcc, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_RC)    {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_rc, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};

			if (nServices & VBI_SERVICE_WSS_43)  {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_4x3, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_WSS_149) {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_14x9, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_WSS_169) {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_16x9, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};
			if (nServices & VBI_SERVICE_AFD)     {_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, v->pRGB_AFD, 24, 13, nIconX, nThumbnailHeight - 14, 1.0, 0x0000ff); nIconX += 25;};

			{
				BYTE * pIcon = NULL;

				switch(v->pat.pmt[nPMTIndex].es[nESIndex].nParseType)
				{
				case PARSE_ES_TYPE_MPEG2_VIDEO:
					if (v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType == 0x80)
					{
						if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
							pIcon = v->pRGB_DCIIVideo;
						else
							pIcon = v->pRGB_BL_DCIIVideo;
					}
					else
					{
						if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
							pIcon = v->pRGB_MPG2Video;
						else
							pIcon = v->pRGB_BL_MPG2Video;
					}
					break;
				case PARSE_ES_TYPE_H264_VIDEO:
					if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
						pIcon = v->pRGB_H264Video;
					else
						pIcon = v->pRGB_BL_H264Video;
					break;
				case PARSE_ES_TYPE_MPEG4_VIDEO:
					if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
						pIcon = v->pRGB_MPG4Video;
					else
						pIcon = v->pRGB_BL_MPG4Video;
					break;
#ifdef PRO
				case PARSE_ES_TYPE_VC1_VIDEO:
					if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
						pIcon = v->pRGB_VC1Video;
					else
						pIcon = v->pRGB_BL_VC1Video;
					break;
#endif PRO
				}
				if (pIcon != NULL)
					_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, pIcon, 24, 13, nThumbnailWidth - 25, 1, 1.0, 0xff0000);
			}
		}
	}
	else
	{
		{
			BYTE * pIcon = NULL;
			switch(v->pat.pmt[nPMTIndex].es[nESIndex].nParseType)
			{
			case PARSE_ES_TYPE_MPEG_AUDIO:
				if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
					pIcon = v->pRGB_MPEGAudio;
				else
					pIcon = v->pRGB_BL_MPEGAudio;
				break;
#ifndef LITE
			case PARSE_ES_TYPE_AC3_AUDIO:
				if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
					pIcon = v->pRGB_AC3Audio;
				else
					pIcon = v->pRGB_BL_AC3Audio;
				break;
			case PARSE_ES_TYPE_MPEG2_AAC_AUDIO:
			case PARSE_ES_TYPE_MPEG4_AAC_AUDIO:
				if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
					pIcon = v->pRGB_AACAudio;
				else
					pIcon = v->pRGB_BL_AACAudio;
				break;
#endif LITE
			}					
			if (pIcon != NULL)
				_ISOverlayRGBTrans(v->pNewPicture, nThumbnailWidth, nThumbnailHeight, pIcon, 24, 13, nThumbnailWidth - 25, 1, 1.0, 0xff0000);
		}
	}

	nDrawHeight = nThumbnailHeight;
	if (yCurrent + nThumbnailHeight > (yEnd - 15))
		nDrawHeight = (yEnd - 15) - yCurrent;
	else
		v->nThumbnailDisplayCount++;
	nDrawWidth = nThumbnailWidth;
	if (xStart + nDrawWidth > xEnd - 20)
		nDrawWidth = (xEnd - xStart) - 20;
	_ISDrawRGB(hDC,
			   v->pNewPicture,
			   nThumbnailWidth, nThumbnailHeight,
			   xStart, yCurrent,
			   nDrawWidth, nDrawHeight,
			   NULL);

	// Save the thumbnail position for when we get clicks later
	{
		int xStartOffset, yStartOffset;

		GetVideoArea(&xStartOffset, &yStartOffset, NULL, NULL);
		v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.left = xStart + xStartOffset;
		v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.top = yCurrent + yStartOffset;
		v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.right = xStart + nDrawWidth + xStartOffset;
		v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.bottom = yCurrent + nDrawHeight + yStartOffset;
	}

}

void UpdateVideoPix(HWND hDlg)
{
	HDC hRealDC = GetDC(hDlg);
	int xWidth, xStart, yStart, xEnd, yEnd;
	int yCurrent;
	int nPMTIndex, nESIndex;
	int nStartPMTIndex;
	int yMaximum = 0;
	int nMaxThumbnailsHeight = 0;
	int xCurrent;
	BOOL fOutOfRoom = FALSE;
	RECT rcParent, rcScrollbar;
	
	if (v->nThumbnailImageCount)
	{
		int nMin, nMax;
		GetScrollRange(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), SB_CTL, &nMin, &nMax);
		if (v->nThumbnailImageCount > nMax)
			SetScrollRange(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), SB_CTL, 0, v->nThumbnailImageCount, TRUE);
	}

	GetWindowRect(GetDlgItem(v->hDlgSIParser, IDC_VIDEO_FRAME), &v->rcVideoBorder);
	GetWindowRect(v->hWndMainWindow, &rcParent);
	GetClientRect(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), &rcScrollbar);
	xWidth = v->rcVideoBorder.right - v->rcVideoBorder.left - 12 - rcScrollbar.right;
	
	xStart = v->rcVideoBorder.left - rcParent.left + 0;
	yCurrent = yStart = v->rcVideoBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) + 10;

	xStart = 0;
	yCurrent = yStart = 0;

	yEnd = yStart + (v->rcVideoBorder.bottom - v->rcVideoBorder.top);
	xEnd = xStart + (v->rcVideoBorder.right - v->rcVideoBorder.left);
	xCurrent = xStart;

	if (v->hThumbnailDC != NULL)
	{
		if (v->memThumbnailBM != NULL)
		{
			DeleteObject(v->memThumbnailBM);
			v->memThumbnailBM = NULL;
		}
		DeleteDC(v->hThumbnailDC);
		v->hThumbnailDC = NULL;
	}

	v->hThumbnailDC = CreateCompatibleDC(hRealDC);
	v->memThumbnailBM = CreateCompatibleBitmap (hRealDC, v->rcVideoBorder.right, v->rcVideoBorder.bottom);
	if (v->memThumbnailBM == NULL)
	{
		ReleaseDC(hDlg, hRealDC);
		return;
	}
	SelectObject(v->hThumbnailDC, v->memThumbnailBM);
	SetBkMode(v->hThumbnailDC, TRANSPARENT);

	nStartPMTIndex = GetPMTOffsetFromThumbnailScrollOffset();
	if (nStartPMTIndex == -1)
		nStartPMTIndex = 0;
	v->nThumbnailDisplayCount = 0;

	for (nPMTIndex = nStartPMTIndex; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
	{
		if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
			break;

		if (!v->fThumbnailsRightToLeft)
		{
			// Top down code	
			for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
			{
				if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
					break;

				if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
				{
					DrawThumbnail(v->hThumbnailDC, nPMTIndex, nESIndex, yCurrent, yEnd, xStart, xEnd);
					yCurrent += v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight + 1;
					if (   (!v->fFullThumbnails && (yCurrent > yEnd - 20))
						|| (v->fFullThumbnails  && (yCurrent > yEnd - GetNextThumbnailHeight(nPMTIndex, nESIndex))) )
					{
						if (!v->fAllowResizing)
						{
							fOutOfRoom = TRUE;
							break;
						}
						xStart += v->pat.pmt[nPMTIndex].es[nESIndex].nVideoWidth + 3;				
						if (xStart + 20 >= xEnd)
						{
							fOutOfRoom = TRUE;
							break;
						}
						yCurrent = v->rcVideoBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION);
						yCurrent = yStart;
					}
				}
			}
		}
		else
		{
			// Right to left code
			int nThumbnailsHeightForChannel = 0;
			int yCurrentStart = yCurrent;

			for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
			{
				if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
					break;

				if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
					nThumbnailsHeightForChannel += v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight;
			}
			if (   v->fFullThumbnails && yCurrent + nThumbnailsHeightForChannel > yEnd
				|| !v->fFullThumbnails && yCurrent > yEnd)
				fOutOfRoom = TRUE;
			else
			{
				int nThumbnailWidth = 0;
				BOOL fDrewAtLeastOneThumbnail = FALSE;

				if (nThumbnailsHeightForChannel > nMaxThumbnailsHeight)
					nMaxThumbnailsHeight = nThumbnailsHeightForChannel;

				for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
				{
					if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
						break;

					if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
					{
						DrawThumbnail(v->hThumbnailDC, nPMTIndex, nESIndex, yCurrent, yEnd, xCurrent, xEnd);
						yCurrent += v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight;
						if (nThumbnailWidth == 0)
							nThumbnailWidth = v->pat.pmt[nPMTIndex].es[nESIndex].nVideoWidth;
						fDrewAtLeastOneThumbnail = TRUE;
					}
				}
				if (fDrewAtLeastOneThumbnail)
				{
					yCurrent = yCurrentStart;
					xCurrent += nThumbnailWidth + 3;
					if (xCurrent + nThumbnailWidth > xEnd)
					{
						xCurrent = xStart;			
						yCurrent += nMaxThumbnailsHeight + 1;
						nMaxThumbnailsHeight = 0;
					}
				}
			}
		}
		if (fOutOfRoom)
			break;
	}

	ReleaseDC(hDlg, hRealDC);
	InvalidateThumbnails();
}

void CreateBrushes(HDC hDC)
{
	v->hBr = CreateSolidBrush(GetSysColor(COLOR_APPWORKSPACE));
	v->hBrRed = CreateSolidBrush(RGB(0xff, 0, 0));
	v->hBrDarkRed = CreateSolidBrush(RGB(0x80, 0, 0));
	v->hBrGreen = CreateSolidBrush(RGB(0, 0xff, 0));
	v->hBrDarkGreen = CreateSolidBrush(RGB(0, 0x80, 0));
	v->hSourceInfoBrush1 = CreateSolidBrush(SOURCE_INFO_COLOR_1);
	v->hSourceInfoBrush2 = CreateSolidBrush(SOURCE_INFO_COLOR_2);

	v->hRedPen = CreatePen(PS_SOLID, 3, RGB(0xff, 0x00, 0x00));
	v->hGreenPen = CreatePen(PS_SOLID, 3, RGB(0x00, 0xff, 0x00));
}

void UpdateSourceInfo(HWND hDlg)
{
	int xWidth, xStart, yStart, yEnd;
	int nLine, yCurrent;
	HDC hDC;
	HGDIOBJ hOldFont;
	RECT rcBorder, rcParent;
	RECT rcFill;
	SIZE textsize;

	if (!v->fRunning)
		return;

	hDC = GetDC(hDlg);
	hOldFont = SelectObject(hDC, v->hSourceInfoFont);

	if (v->hBr == NULL)
		CreateBrushes(hDC);
	
	GetWindowRect(GetDlgItem(v->hDlgSIParser, IDC_SOURCE_BOX), &rcBorder);
	GetWindowRect(v->hWndMainWindow, &rcParent);
	xWidth = rcBorder.right - rcBorder.left - 10;
	xStart = rcBorder.left - rcParent.left;
	yStart = rcBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) + 10;
	yCurrent = yStart;
	yEnd = yStart + (rcBorder.bottom - rcBorder.top) - 20;

	GetTextExtentPoint32(hDC, "X", 1, &textsize);
	for (nLine = 0; yCurrent + textsize.cy <= yEnd; nLine++)
	{
		HANDLE hBackgroundBrush;
		DWORD dwForegroundColor, dwBackgroundColor;
		char szOutput[128];
	
		if ((nLine & 1) == 0)
		{
			dwForegroundColor = SOURCE_INFO_COLOR_1;
			dwBackgroundColor = SOURCE_INFO_COLOR_2;
			hBackgroundBrush = v->hSourceInfoBrush2;
dwForegroundColor = RGB(0xff, 0xff, 0xff);
		}
		else
		{
			dwForegroundColor = SOURCE_INFO_COLOR_2;
			dwBackgroundColor = SOURCE_INFO_COLOR_1;
			hBackgroundBrush = v->hSourceInfoBrush1;
dwForegroundColor = RGB(0x00, 0x00, 0x00);
		}
		SetTextColor(hDC, dwForegroundColor);
		SetBkColor(hDC, dwBackgroundColor);
		
		GetSourceInfoLine(nLine, szOutput);
		rcFill.top = yCurrent; rcFill.bottom = yCurrent + textsize.cy;
		rcFill.left = xStart; rcFill.right = rcFill.left + xWidth;
		FillRect(hDC, &rcFill, hBackgroundBrush);
		TextOut(hDC, xStart, yCurrent, szOutput, lstrlen(szOutput));
		yCurrent += textsize.cy;
	}
	
	SelectObject(hDC, hOldFont);
	ReleaseDC(hDlg, hDC);
}

void UpdatePIDChart(HWND hDlg)
{
	int i;
	int xWidth, xStart, yStart, yEnd;
	int nPIDStart = 0;
	HDC hDC;
	HGDIOBJ hOldFont;
	HBRUSH hBrScrambled, hBrScrambledInactive, hBrUnscrambled, hBrUnscrambledInactive;
	double dScale;
	double dTotalPercent = 0.0;
	RECT rcBorder, rcParent;
#ifdef _DEBUG_MESSAGES
	double dTotalMbps = 0.0;
#endif _DEBUG_MESSAGES

	if (!v->fRunning)
		return;

	if (v->fPIDChartDisabled)
		return;

	if (v->fDataReceviedInParseIncomingDataThread == FALSE)
		return;

	hBrScrambled = CreateSolidBrush(v->dwScrambledPIDColor);
	hBrScrambledInactive = CreateSolidBrush(v->dwScrambledInactivePIDColor);
	hBrUnscrambled = CreateSolidBrush(v->dwUnscrambledPIDColor);
	hBrUnscrambledInactive = CreateSolidBrush(v->dwUnscrambledInactivePIDColor);

	hDC = GetDC(hDlg);
	hOldFont = SelectObject(hDC, v->hPIDFont);
	SetBkMode(hDC, TRANSPARENT);
	if (v->hBr == NULL)
		CreateBrushes(hDC);

	GetWindowRect(GetDlgItem(hDlg, IDC_PIDS_BORDER), &rcBorder);
	GetWindowRect(v->hWndMainWindow, &rcParent);
	xWidth = rcBorder.right - rcBorder.left - 22;
	xStart = rcBorder.left - rcParent.left + 2;
	yStart = rcBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) + 12 + 10;
	yEnd = yStart + (rcBorder.bottom - rcBorder.top) - 10;

	dScale = (double)xWidth / (double)v->lnMaxPackets;
	dScale = (double)xWidth / (double)v->lnCopyTotalTSPackets;

	v->nPIDChartItemCount = 0;
	if (v->fSortChartByPID == FALSE)
	{
		nPIDStart = v->nPIDScrollOffset;
	}
	else
	{
		int nPIDActiveCounter = 0;
		for (i = 0; i < 8192; i++)
		{
			if (v->pc[i].lnPackets != 0)
			{
				if (nPIDActiveCounter == v->nPIDScrollOffset)
				{
					nPIDStart = i;
					break;
				}
				nPIDActiveCounter++;
			}
		}
	}
	for (i = nPIDStart; i < 8192; i++)
	{
		RECT rect;
		SIZE pidsize;
		int j;
		int xTextPos;
		char szPID[50], szMask[50];
		BOOL fResetBkColor = FALSE;
		double dPercent;

		if (v->fSortChartByPID == FALSE)
		{
			if (v->pc[i].lnPackets == 0)
				break;
		}
		else
		{
			if (v->pc[i].lnPackets == 0)
				continue;
		}
		rect.top = yStart;
		rect.bottom = yStart + 12;
		rect.left = xStart;
		rect.right = xStart + (int)(v->pc[i].lnPackets * dScale);
		if (rect.left == rect.right)
			rect.right++;
		if (v->pc[i].fScrambled == TRUE)
		{
			if (IsPIDActive(v->pc[i].nPID) == TRUE)
				FillRect(hDC, &rect, hBrScrambled);
			else
				FillRect(hDC, &rect, hBrScrambledInactive);
		}
		else
		{
			if (IsPIDActive(v->pc[i].nPID) == TRUE)
				FillRect(hDC, &rect, hBrUnscrambled);
			else
				FillRect(hDC, &rect, hBrUnscrambledInactive);
		}
		
		rect.left = xStart + (int)(v->pc[i].lnPackets * dScale);
		rect.right = xStart + xWidth + 3;
		InvalidateRect(hDlg, &rect, TRUE);
		UpdateWindow(hDlg);

		if (v->pc[i].fScrambled == TRUE)
		{
			if (IsPIDActive(v->pc[i].nPID) == TRUE)
				SetTextColor(hDC, v->dwScrambledPIDColor);
			else
				SetTextColor(hDC, v->dwScrambledInactivePIDColor);
		}
		else
			SetTextColor(hDC, 0);

		for (j = 0; j < MAX_ESLIST_ENTRIES; j++)
		{
			if (v->nHighlightPIDs[j] == -1)
				break;
			if (v->nHighlightPIDs[j] == v->pc[i].nPID)
			{
				SetTextColor(hDC, v->dwHighlightedPIDColor);
				break;
			}
		}
		dPercent = ((double)v->pc[i].lnPackets / (double)v->lnCopyTotalTSPackets) * 100.0;
		if (v->pc[i].dPIDRate > 0.0)
		{
			double dRate = v->pc[i].dPIDRate * 8.0;
#ifdef _DEBUG_MESSAGES
			dTotalMbps += dRate;
#endif _DEBUG_MESSAGES
			if (dRate > 1000.0 * 1000.0)
			{
				wsprintf(szMask, "%s (%%.2f%%%% - %%.2f Mbps)", v->szOutputPIDFlags);
				sprintf(szPID, szMask, v->pc[i].nPID, dPercent, dRate / 1000.0 / 1000.0);
			}
			else if (dRate > 1000.0)
			{
				wsprintf(szMask, "%s (%%.2f%%%% - %%.2f Kbps)", v->szOutputPIDFlags);
				sprintf(szPID, szMask, v->pc[i].nPID, dPercent, dRate / 1000.0);
			}
			else
			{
				wsprintf(szMask, "%s (%%.2f%%%% - %%.0f bps)", v->szOutputPIDFlags);
				sprintf(szPID, szMask, v->pc[i].nPID, dPercent, dRate);
			}
		}
		else
		{
			if (v->dDisplayMuxRate)
			{
				double dRate = (v->dDisplayMuxRate / (double)v->nMuxRateCounter) * 8.0;
				dRate = ((dRate / 100.0) * dPercent);
#ifdef _DEBUG_MESSAGES
				dTotalMbps += dRate;
#endif _DEBUG_MESSAGES
				if (dRate > (1000.0 * 1000.0))
				{
					wsprintf(szMask, "%s (%%.2f%%%% ~ %%.2f Mbps)", v->szOutputPIDFlags);
					sprintf(szPID, szMask, v->pc[i].nPID, dPercent, dRate / 1000.0 / 1000.0);
				}
				else if (dRate > 1000.0)
				{
					wsprintf(szMask, "%s (%%.2f%%%% ~ %%.2f Kbps)", v->szOutputPIDFlags);
					sprintf(szPID, szMask, v->pc[i].nPID, dPercent, dRate / 1000.0);
				}
				else
				{
					wsprintf(szMask, "%s (%%.2f%%%% ~ %%.0f bps)", v->szOutputPIDFlags);
					sprintf(szPID, szMask, v->pc[i].nPID, dPercent, dRate);
				}
			}
			else
			{
				wsprintf(szMask, "%s (%%.3f%%%%)", v->szOutputPIDFlags);
				sprintf(szPID, szMask, v->pc[i].nPID, dPercent);
			}
		}
		dTotalPercent += dPercent;

#ifndef LITE
		if (v->pc[i].nPIDHasContinuityErrors)
		{
			if (v->fCountContinuityErrors == FALSE)
				lstrcat(szPID, " *");
			else
			{
				char szTemp[32];
				wsprintf(szTemp, " *%d", v->pc[i].nPIDHasContinuityErrors);
				lstrcat(szPID, szTemp);
			}
		}
		if (v->pc[i].nPIDTEICount)
		{
			char szTemp[32];
			wsprintf(szTemp, "/%d", v->pc[i].nPIDTEICount);
			lstrcat(szPID, szTemp);
		}
#endif LITE

		GetTextExtentPoint32(hDC, szPID, lstrlen(szPID), &pidsize);
		xTextPos = xStart + (int)(v->pc[i].lnPackets * dScale) + 5;
		if (xTextPos + pidsize.cx > xStart + xWidth)
		{
			xTextPos = xStart + (int)(v->pc[i].lnPackets * dScale) - pidsize.cx;
			SetTextColor(hDC, RGB(0, 0, 0));
			fResetBkColor = TRUE;
		}
		TextOut(hDC, xTextPos, rect.top, szPID, lstrlen(szPID));
		if (fResetBkColor == TRUE)
		{
			SetBkColor(hDC, GetSysColor(COLOR_APPWORKSPACE));
		}
		else
		{
			rect.left = xStart + (int)(v->pc[i].lnPackets * dScale) + 5 + pidsize.cx;
			rect.right = rect.left + 5;
			//FillRect(hDC, &rect, hBr);
		}

		v->nPIDChartItemCount++;
		yStart += 14;
		if (yStart >= yEnd - 28)
			break;
	}

	if (yStart < yEnd - 18)
	{
		RECT rcFillIn;

		rcFillIn.top = yStart;
		rcFillIn.bottom = yEnd - 18;
		rcFillIn.left = xStart;
		rcFillIn.right = xStart + xWidth;
		FillRect(hDC, &rcFillIn, (HBRUSH) (COLOR_3DFACE + 1));
	}

	SetBkMode(hDC, OPAQUE);
	SelectObject(hDC, hOldFont);
	ReleaseDC(hDlg, hDC);

	DeleteObject(hBrScrambled);
	DeleteObject(hBrScrambledInactive);
	DeleteObject(hBrUnscrambled);
	DeleteObject(hBrUnscrambledInactive);

#ifdef _DEBUG_MESSAGES
	{
		char szTemp[128];
		sprintf(szTemp, "TSReader: PID chart total bps = %f\n", dTotalMbps);
		OutputDebugString(szTemp);
	}
#endif _DEBUG_MESSAGES
}

void ForcePIDChartRepaint(HWND hDlg)
{
	RECT rcBorder, rcParent, rcInvalidate;

	GetWindowRect(GetDlgItem(hDlg, IDC_PIDS_BORDER), &rcBorder);
	GetWindowRect(hDlg, &rcParent);
	
	rcInvalidate.left = rcBorder.left - rcParent.left;
	rcInvalidate.right = rcInvalidate.left + (rcBorder.right - rcBorder.left);
	rcInvalidate.top = rcBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION);
	rcInvalidate.bottom = rcInvalidate.top + (rcBorder.bottom - rcBorder.top);

	InvalidateRect(hDlg, &rcInvalidate, TRUE);
	UpdateWindow(hDlg);
	UpdatePIDChart(hDlg);
}

typedef struct _tagSortMACs
{
	int nPackets;
	__int64 nBytes;
	BYTE bMAC[8];
} SORTMACS, *PSORTMACS;


int __cdecl SortMACsByDataCompareFunction(const void *elem1, const void *elem2)
{
	PSORTMACS pSortMACs1 = (PSORTMACS)elem1;
	PSORTMACS pSortMACs2 = (PSORTMACS)elem2;

	if (pSortMACs1->nBytes > pSortMACs2->nBytes)
		return -1;
	if (pSortMACs1->nBytes < pSortMACs2->nBytes)
		return 1;
	return 0;
}

int __cdecl SortMACsByPacketsCompareFunction(const void *elem1, const void *elem2)
{
	PSORTMACS pSortMACs1 = (PSORTMACS)elem1;
	PSORTMACS pSortMACs2 = (PSORTMACS)elem2;

	if (pSortMACs1->nPackets > pSortMACs2->nPackets)
		return -1;
	if (pSortMACs1->nPackets < pSortMACs2->nPackets)
		return 1;
	return 0;
}

void DecodeIPParserPID(HWND hDlg, PIPCLICKLPARAM ipclicklParam)
{
	int nIndex = ipclicklParam->dwPtr;
	int nMACCount = 0;
	char szOutput[2048];
	char szOutput2[1024];
	PIPMACENTRY pMACEntries = v->ippid[nIndex].pMACEntries;

	// Figure out MAC count
	while (pMACEntries)
	{
		nMACCount++;
		pMACEntries = (PIPMACENTRY)pMACEntries->dwNext;
	}
	sprintf(szOutput, "PID 0x%04x\r\nPackets: %d\r\nMACs carried on PID: %d\r\nMBytes: %.3f\r\n",
			 v->ippid[nIndex].nPID,
			 v->ippid[nIndex].nPacketCount,
			 nMACCount,
			 (double)v->ippid[nIndex].nByteCount / 1024.0 / 1024.0);

	// Figure top packet/byte users
	if (nMACCount)
	{
		PSORTMACS pSort = LocalAlloc(LPTR, nMACCount * sizeof(SORTMACS));
		int nSortIndex = 0;
		int i;

		pMACEntries = v->ippid[nIndex].pMACEntries;
		while (pMACEntries)
		{
			memcpy(pSort[nSortIndex].bMAC, pMACEntries->bMAC, sizeof(BYTE) * 6);
			pSort[nSortIndex].nPackets = pMACEntries->nPacketCount;
			pSort[nSortIndex++].nBytes = pMACEntries->nByteCount;			
			pMACEntries = (PIPMACENTRY)pMACEntries->dwNext;
		}
		
		qsort(pSort, nSortIndex, sizeof(SORTMACS), SortMACsByDataCompareFunction);
		lstrcpy(szOutput2, "\r\nTop ten data MACs:\r\n");
		for (i = 0; i < 10; i++)
		{
			char szOutput3[128];

			if (i == nSortIndex)
				break;
			sprintf(szOutput3, " %02x:%02x:%02x:%02x:%02x:%02x %.3f MB\r\n",
				    pSort[i].bMAC[0], pSort[i].bMAC[1], pSort[i].bMAC[2], 
				    pSort[i].bMAC[3], pSort[i].bMAC[4], pSort[i].bMAC[5],
					(double)pSort[i].nBytes / 1024.0 / 1024.0);
			lstrcat(szOutput2, szOutput3);
		}
		lstrcat(szOutput, szOutput2);

		qsort(pSort, nSortIndex, sizeof(SORTMACS), SortMACsByPacketsCompareFunction);
		lstrcpy(szOutput2, "\r\nTop ten packet MACs:\r\n");
		for (i = 0; i < 10; i++)
		{
			char szOutput3[128];

			if (i == nSortIndex)
				break;
			sprintf(szOutput3, " %02x:%02x:%02x:%02x:%02x:%02x %d packets\r\n",
				    pSort[i].bMAC[0], pSort[i].bMAC[1], pSort[i].bMAC[2], 
				    pSort[i].bMAC[3], pSort[i].bMAC[4], pSort[i].bMAC[5],
					pSort[i].nPackets);
			lstrcat(szOutput2, szOutput3);
		}
		lstrcat(szOutput, szOutput2);

		LocalFree(pSort);		
	}

	SetDlgItemText(hDlg, IDC_SI_TEXT, szOutput);
	memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
	v->nHighlightPIDs[0] = v->ippid[nIndex].nPID;
	ForcePIDChartRepaint(hDlg);
}

void HandleTreeClickIPDVB(HWND hDlg, LPNMHDR pnmh)
{
	PIPCLICKLPARAM ipclicklParam;
	LPNM_TREEVIEW pnmtv = (LPNM_TREEVIEW)pnmh;
	HTREEITEM hti = pnmtv->itemNew.hItem;
	TVITEM tvi;

	memset(&tvi, 0, sizeof(tvi));
	tvi.mask = TVIF_PARAM;
	tvi.hItem = hti;
	TreeView_GetItem(GetDlgItem(hDlg, IDC_SI_TREE), &tvi);
	v->nSelectedProgram = -1;
	v->hLastSelectedTreeItem = hti;

	ipclicklParam = (PIPCLICKLPARAM)tvi.lParam;	
	switch(ipclicklParam->nType)
	{
	case SI_PARSER_IP_PID:
		DecodeIPParserPID(hDlg, ipclicklParam);
		break;
	case SI_PARSER_IP_MAC:
		{
			PIPMACENTRY pCurrentMAC = (PIPMACENTRY)ipclicklParam->dwPtr;
			PIPENTRY pIPEntries = pCurrentMAC->pIPEntries;
			int nAddressComboCount = 0;
			char szOutput[200];

			while (pIPEntries)
			{
				nAddressComboCount++;
				pIPEntries = (PIPENTRY)pIPEntries->dwNext;
			}
			sprintf(szOutput, "MAC %02x:%02x:%02x:%02x:%02x:%02x\r\nPackets: %d\r\nIP destinations carried on MAC: %d\r\nMBytes: %.3f\r\n",
				     pCurrentMAC->bMAC[0], pCurrentMAC->bMAC[1], pCurrentMAC->bMAC[2],
				     pCurrentMAC->bMAC[3], pCurrentMAC->bMAC[4], pCurrentMAC->bMAC[5],
					 pCurrentMAC->nPacketCount,
					 nAddressComboCount,
					 (double)pCurrentMAC->nByteCount / 1024.0 / 1024.0);
			SetDlgItemText(hDlg, IDC_SI_TEXT, szOutput);
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			ForcePIDChartRepaint(hDlg);
		}
		break;
	case SI_PARSER_IP_IP:
		{
			PIPENTRY pCurrentIP = (PIPENTRY)ipclicklParam->dwPtr;
			char szProtocol[10];
			char szOutput[200];

			switch(pCurrentIP->dwProtocol)
			{
			case IP_UDP_ID:
				lstrcpy(szProtocol, "UDP");
				break;
			case 6:
				lstrcpy(szProtocol, "TCP");
				break;
			default:
				wsprintf(szProtocol, "%d", pCurrentIP->dwProtocol);
				break;
			}
			sprintf(szOutput, "Destination Address: %d.%d.%d.%d\r\nProtocol: %s\r\nPackets: %d\r\nMBytes: %.3f\r\n",
					 (pCurrentIP->dwDestinationAddress >> 24) & 0xff,
					 (pCurrentIP->dwDestinationAddress >> 16) & 0xff,
					 (pCurrentIP->dwDestinationAddress >> 8) & 0xff,
					 (pCurrentIP->dwDestinationAddress & 0xff),
					 szProtocol,
					 pCurrentIP->nPacketCount,
					 (double)pCurrentIP->nByteCount / 1024.0 / 1024.0);
			SetDlgItemText(hDlg, IDC_SI_TEXT, szOutput);
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			ForcePIDChartRepaint(hDlg);
		}
		break;
	}

}

void HandleTreeClickMPEG2(HWND hDlg, LPNMHDR pnmh)
{
	static int nLastNITClick = -1;
	LPNM_TREEVIEW pnmtv = (LPNM_TREEVIEW)pnmh;
	HTREEITEM hti = pnmtv->itemNew.hItem;
	TVITEM tvi;
	int nItemIndex;
	
	v->fTreeViewSelectedAtLeastOnce = TRUE;
	memset(&tvi, 0, sizeof(tvi));
	tvi.mask = TVIF_PARAM;
	tvi.hItem = hti;
	TreeView_GetItem(GetDlgItem(hDlg, IDC_SI_TREE), &tvi);
	v->nSelectedProgram = -1;
	v->hLastSelectedTreeItem = hti;

	switch(tvi.lParam & 0xf0000000)
	{
	case SI_PARSER_CVCT:
		nItemIndex = tvi.lParam - SI_PARSER_CVCT;
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatCVCT(nItemIndex));
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = 0x1ffb;
		PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_BAT:
		nItemIndex = tvi.lParam - SI_PARSER_BAT;
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatBAT(nItemIndex));
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = 0x0011;
		PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_MGT:
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatMGT());
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = 0x1ffb;
		PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_CDT:	// Also the RRT
		switch(v->nNetworkPID)
		{
		default:
			nItemIndex = tvi.lParam - SI_PARSER_CDT;
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatCDTEntry(nItemIndex));
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[0] = v->nNetworkPID;
			PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
			ForcePIDChartRepaint(hDlg);
			break;
		case 0x1ffb:
			nItemIndex = tvi.lParam - SI_PARSER_RRT;
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatRRTEntry(nItemIndex));
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[0] = 0x1ffb;
			PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
			ForcePIDChartRepaint(hDlg);
			break;
		}
		break;
	case SI_PARSER_TDT:
		switch(v->nNetworkPID)
		{
		default:
			nItemIndex = tvi.lParam - SI_PARSER_TDT;
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatTDTEntry(nItemIndex));
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[0] = v->nNetworkPID;
			PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
			ForcePIDChartRepaint(hDlg);
			break;
		case 0x0010:
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[0] = 0x0014;
			PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
			ForcePIDChartRepaint(hDlg);
			if ((tvi.lParam & 0x0fffffff) == 0)
			{
				SetDlgItemText(hDlg, IDC_SI_TEXT, "");
			}
			else
			{
				SetDlgItemText(hDlg, IDC_SI_TEXT, FormatDVBTOT());
			}
			break;
		case 0x1ffb:
			{
				int DS_status = (v->dvbtdt.nDaylightSavings >> 15) & 1;
				int DS_day_of_month = (v->dvbtdt.nDaylightSavings >> 8) & 0x1f;
				int DS_hour = v->dvbtdt.nDaylightSavings & 0xff;
				char szTemp[1024];

				wsprintf(szTemp, "STT: GPS/UTC offset: %d\r\nSTT: DS Status: %d DS Day of Month: %d DS Hour: %d\r\n",
					     v->dvbtdt.nGPSOffset,
						 DS_status,
						 DS_day_of_month,
						 DS_hour);
#ifdef PRO
				{
					__int64 nSystemTime, nStream;
					__int64 nDifference;
					SYSTEMTIME stSystemTime, stStreamTime;
					char szTemp2[128];

					GetSystemTime(&stSystemTime);
					SystemTimeToFileTime(&stSystemTime, (FILETIME *)&nSystemTime);
					
					memset(&stStreamTime, 0, sizeof(stStreamTime));
					stStreamTime.wYear = v->dvbtdt.nYear;
					stStreamTime.wMonth = v->dvbtdt.nMonth;
					stStreamTime.wDay = v->dvbtdt.nDay;
					stStreamTime.wHour = v->dvbtdt.nHour;
					stStreamTime.wMinute = v->dvbtdt.nMinute;
					stStreamTime.wSecond = v->dvbtdt.nSecond;
					SystemTimeToFileTime(&stStreamTime, (FILETIME *)&nStream);

					nDifference = nStream - nSystemTime;
					nDifference /= 10000000;
					wsprintf(szTemp2, "Difference between stream and PC time: %d seconds\r\n", (int)nDifference);
					lstrcat(szTemp, szTemp2);
				}
#endif PRO
				SetDlgItemText(hDlg, IDC_SI_TEXT, szTemp);
			}
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[0] = 0x1ffb;
			PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
			ForcePIDChartRepaint(hDlg);
			break;
		}
		break;
	case SI_PARSER_SIT:
		nItemIndex = tvi.lParam - SI_PARSER_SIT;
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatSITEntry(nItemIndex));
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = v->nNetworkPID;
		PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_MMT:
		nItemIndex = tvi.lParam - SI_PARSER_MMT;
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatMMTEntry(nItemIndex));
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = v->nNetworkPID;
		PIDManagement(TRUE, v->nHighlightPIDs[0], TRUE);
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_NIT:
		nItemIndex = tvi.lParam - SI_PARSER_NIT;
		if (nItemIndex != 0x0fffffff)
		{
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatNITEntry(nItemIndex, FALSE));
			nLastNITClick = nItemIndex;
		}
		else
		{
			int nNITIndex;
			int nNITItemCount = 0;
			char szTemp[128];

			for (nNITIndex = 0; nNITIndex < MAX_TS_ENTRIES; nNITIndex++)
			{
				if (v->pNITData[nNITIndex] != NULL)
					nNITItemCount++;
			}
			if (nNITItemCount == 1)
				wsprintf(szTemp, "NIT contains %d entry", nNITItemCount);
			else
				wsprintf(szTemp, "NIT contains %d entries", nNITItemCount);
			SetDlgItemText(hDlg, IDC_SI_TEXT, szTemp);
		}
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = 0x0010;
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_PAT:
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatPAT(FALSE, v->nExportSITables));
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = 0x0000;
		ForcePIDChartRepaint(hDlg);
		v->nSelectedVideoDisplayProgram = 0;			
		UpdateVideoPix(hDlg);
		break;
	case SI_PARSER_PMT:
		{
			int nHighlightIndex = 0;
			int nCAIndex = 0;
			BOOL fRestartStradis = FALSE;

			if (v->nProgramPIDCount)
			{
				int i;

				for (i = 0; i < v->nProgramPIDCount; i++)
					PIDManagement(FALSE, v->nProgramPIDs[i], TRUE);
			}

			nItemIndex = tvi.lParam - SI_PARSER_PMT;
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatPMTEntry(nItemIndex, FALSE));
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[nHighlightIndex++] = v->pat.pmt[nItemIndex].nPMTPID;
			if (v->pat.pmt[nItemIndex].nProgramNumber != 0)
			{
				int i;
				int nVideoPID = 0x1fff;
				int nAudioPID = 0x1fff;
				int nTeletextPID = 0x1fff;

				if (v->fStradisActive == TRUE)
				{
					StreamDecoder(v->hWndMainWindow);
					fRestartStradis = TRUE;
				}

				memset(&v->nDecryptPIDs, 0, sizeof(v->nDecryptPIDs));
				v->nDecryptPIDCounter = 0;
				memset(&v->key.odd_ck[0], 0, 8);
				memset(&v->key.even_ck[0], 0, 8);
				for (i = 0; i < MAX_ESLIST_ENTRIES; i++)
				{
					if (v->pat.pmt[nItemIndex].es[i].nESPID == 0)
						break;
					v->nHighlightPIDs[nHighlightIndex++] = v->pat.pmt[nItemIndex].es[i].nESPID;
					v->nDecryptPIDs[v->nDecryptPIDCounter++] = v->pat.pmt[nItemIndex].es[i].nESPID;
					switch(v->pat.pmt[nItemIndex].es[i].nStreamType)
					{
					case 0x01:	// MPEG-1
					case 0x02:	// MPEG-2
					case 0x10:	// MPEG-4
					case 0x1b:	// H264
					case 0x80:	// DCII
					case 0xea:	// VC1
						nVideoPID = v->pat.pmt[nItemIndex].es[i].nESPID;
						break;
					case 0x03:	// MPEG-1
					case 0x04:	// MPEG-2
					case 0x0f:	// MPEG-2 AAC
					case 0x11:	// MPEG-4 AAC
					case 0x81:	// AC3
					case 0x83:	// LPCM
					case 0x85:	// DTS
					case 0xe6:	// WM9
						if (nAudioPID == 0x1fff)
							nAudioPID = v->pat.pmt[nItemIndex].es[i].nESPID;
						break;
					case 0x06:
						if (IsAC3AudioStream(nItemIndex, i) == TRUE)
						{
							if (nAudioPID == 0x1fff)
								nAudioPID = v->pat.pmt[nItemIndex].es[i].nESPID;
						}
						else if (IsPCMAudioStream(nItemIndex, i) == TRUE)
						{
							if (nAudioPID == 0x1fff)
								nAudioPID = v->pat.pmt[nItemIndex].es[i].nESPID;
						}
						else if (IsDTSAudioStream(nItemIndex, i) == TRUE)
						{
							if (nAudioPID == 0x1fff)
								nAudioPID = v->pat.pmt[nItemIndex].es[i].nESPID;
						}
						else if (IsTeleTextOrVBIStream(nItemIndex, i) == TRUE)
						{
							if (nTeletextPID == 0x1fff)
								nTeletextPID = v->pat.pmt[nItemIndex].es[i].nESPID;
						}
						break;
					}
					if (v->pat.pmt[nItemIndex].es[i].pDescriptors != NULL)
					{
						BYTE * pDescriptorData = v->pat.pmt[nItemIndex].es[i].pDescriptors;
						int nDescriptorsLength = v->pat.pmt[nItemIndex].es[i].nDescriptorsLength;
						int nCurrentIndex = 0;

						do
						{
							if (pDescriptorData[nCurrentIndex + 0] == 9)
							{
								int nECMPID = ((pDescriptorData[nCurrentIndex + 4] << 8) + pDescriptorData[nCurrentIndex + 5]) & 0x1fff;
								v->nHighlightPIDs[nHighlightIndex++] = nECMPID;
							}
							nCurrentIndex += (BYTE)pDescriptorData[nCurrentIndex + 1]; // descriptor length
							nCurrentIndex += 2;	// descriptor tag and length
						} while (nCurrentIndex < nDescriptorsLength);
					}
				}

				for (i = 0; i < nHighlightIndex; i++)
				{
					v->nProgramPIDs[i] = v->nHighlightPIDs[i];
					PIDManagement(TRUE, v->nHighlightPIDs[i], TRUE);
				}
				v->nProgramPIDCount = nHighlightIndex;

				ForcePIDChartRepaint(hDlg);
				v->nSelectedVideoDisplayProgram = v->nSelectedProgram = nItemIndex;			
				UpdateVideoPix(hDlg);				
				if (v->fMDPluginsLoaded == TRUE)
				{
					struct TCA_System CA[MAX_CA_SYSTEMS];
					char szChannelName[256];

					// Setup CA structure
					memset(CA, 0, sizeof(CA));
					if (v->fDidCAT)
					{
						for (i = 0; i < MAX_CAT_DESCRIPTORS; i++)
						{
							BYTE * pDescriptorData = v->cat.pDescriptor[i];
							if (pDescriptorData == NULL)
								break;
							if (pDescriptorData[0] == 0x09)	// CA descriptor
							{
								BOOL fAddThisOne = TRUE;

								int nCASystemID = (pDescriptorData[2] << 8) + pDescriptorData[3];
								int nCAPID = ((pDescriptorData[4] << 8) + pDescriptorData[5]) & 0x1fff;
								
								if (v->fUsePreferedCAID && nCASystemID != v->nPrefereredCAID)
									fAddThisOne = FALSE;

								if (fAddThisOne)
								{
									CA[nCAIndex].CA_Typ = nCASystemID;
									CA[nCAIndex].EMM = nCAPID;
									CA[nCAIndex].ECM = GetECMPID(nItemIndex, TRUE, nCASystemID);
									CA[nCAIndex].Provider_Id = 1;
									nCAIndex++;
								}
							}
						}
					}

					// Setup channel name
					wsprintf(szChannelName, "Ch.%d", v->pat.pmt[nItemIndex].nProgramNumber);
					if (v->pChannelData[v->pat.pmt[nItemIndex].nProgramNumber] != NULL)
						lstrcpy(szChannelName, v->pChannelData[v->pat.pmt[nItemIndex].nProgramNumber]->szShortName);

					// Tell the plugins we've changed channel
					MD__ChannelChange(v->pat.pmt[nItemIndex].nProgramNumber,
									  nVideoPID, nAudioPID, nTeletextPID, v->pat.pmt[nItemIndex].nPCRPID,
									  v->pat.pmt[nItemIndex].nPMTPID, GetECMPID(nItemIndex, v->fUsePreferedCAID, v->nPrefereredCAID),
									  CA,
									  szChannelName);

				}

				// Tell the CI CAM we've changed
				if ( (v->dwSourceCapabilities & CAPABILITIES_CI_CAM) && v->fDidCAT)
				{
					BYTE bCAMMessage[1024];

					SendCAPMT = (td_SendCAPMT)GetProcAddress(v->hSource, "TSReader_SendCAPMT");
					if (SendCAPMT != NULL)
					{
						int nCAPMTLength;

						nCAPMTLength = GenerateCAPMT(bCAMMessage, sizeof(bCAMMessage), nItemIndex);
						SendCAPMT(bCAMMessage, nCAPMTLength);
					}
				}
#ifndef LITE
				if (   (v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) 
					&& (v->ss.fSerialReceiverControlEnabled == TRUE)
					&& (v->fSerialReceiverControlThreadRunning == TRUE) )
				{
					int nNITIndex;
					int nNetworkID = 0;

					if (v->nNetworkPID == 0x0010)
					{
						for (nNITIndex = 0; nNITIndex < MAX_TS_ENTRIES; nNITIndex++)
						{
							if (v->pNITData[nNITIndex] != NULL)
							{
								if (v->pNITData[nNITIndex]->nTransportStreamID == v->pat.nTransportStreamID)
								{
									nNetworkID = v->pNITData[nNITIndex]->nNetworkID;
									break;
								}
							}
						}
					}
					SourceHelper_SetChannelSerialControl(v->pat.pmt[nItemIndex].nProgramNumber,
														 v->pat.nTransportStreamID,
 												 		 nNetworkID);
				}
#endif LITE

				v->fStradisAutostart = FALSE;
				if (fRestartStradis == TRUE)
				{
					SetTimer(v->hDlgSIParser, 7, 500, NULL);
					//v->fStradisAutostart = TRUE;
					//StreamDecoder(v->hWndMainWindow);
				}
			}
			break;
		}
	case SI_PARSER_NOP:
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		SetDlgItemText(hDlg, IDC_SI_TEXT, "");
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_ES:
		{
			nItemIndex = tvi.lParam - SI_PARSER_ES;	// this isn't the index, it's the ES PID
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatESEntry(nItemIndex));
			memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
			v->nHighlightPIDs[0] = nItemIndex;
			ForcePIDChartRepaint(hDlg);
		}
		break;
	case SI_PARSER_SDT:
	case SI_PARSER_VCT:		
		nItemIndex = tvi.lParam - SI_PARSER_SDT;
		if (nItemIndex != 0x0fffffff)
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatSDTEntry(nItemIndex, FALSE));
		else
		{
			char szTemp[128] = {""};

			int nChannel;
			int nSDTChannelCount = 0;

			for (nChannel = 0; nChannel < MAX_EIT_CHANNEL_DATA; nChannel++)
			{
				if (v->pChannelData[nChannel] != NULL)
					nSDTChannelCount++;
			}
			wsprintf(szTemp, "SDT defines %d channel(s)", nSDTChannelCount);
			SetDlgItemText(hDlg, IDC_SI_TEXT, szTemp);
		}
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		if (v->nNetworkPID == 0x1ffb)
			v->nHighlightPIDs[0] = 0x0011;
		else
			v->nHighlightPIDs[0] = 0x1ffb;
		ForcePIDChartRepaint(hDlg);
		break;
	case SI_PARSER_EIT:
		CursorWait(hDlg);
		nItemIndex = tvi.lParam - SI_PARSER_EIT;
		if (nItemIndex != 0x0fffffff)
			SetDlgItemText(hDlg, IDC_SI_TEXT, FormatEITEntry(nItemIndex, EIT_FORMAT_PLAIN, FALSE));
		else
		{
			char szTemp[128] = {""};

			int nChannel;
			int nEITChannelCount = 0;
			int nEITEventCount = 0;

			for (nChannel = 0; nChannel < MAX_EIT_CHANNEL_DATA; nChannel++)
			{
				if (v->pEvents[nChannel] != NULL)
				{
					PEITEVENT pCurrent;

					nEITChannelCount++;
					pCurrent = v->pEvents[nChannel];
					while (pCurrent != NULL)
					{
						nEITEventCount++;
						pCurrent = (PEITEVENT)pCurrent->dwNextEvent;
					}
				}
			}
			wsprintf(szTemp, "EIT contains %d channel(s) and %d event(s)", nEITChannelCount, nEITEventCount);
			SetDlgItemText(hDlg, IDC_SI_TEXT, szTemp);
		}
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = v->nEITPID;
		ForcePIDChartRepaint(hDlg);
		CursorNormal();
		break;
	case SI_PARSER_CAT:
		SetDlgItemText(hDlg, IDC_SI_TEXT, FormatCAT(FALSE));
		memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
		v->nHighlightPIDs[0] = 0x0001;
		ForcePIDChartRepaint(hDlg);
		break;
	default:
		break;
	}
}

void SIParserMsgNotify(HWND hDlg, UINT uMessage, WPARAM wparam, LPARAM lParam)
{
	LPNMHDR pnmh = (LPNMHDR)lParam;

    switch (pnmh->code)
    {
	case TVN_KEYDOWN:
		{
			LPNMTVKEYDOWN ptvkd = (LPNMTVKEYDOWN) lParam;
			if (ptvkd->wVKey == VK_ESCAPE)
				PostMessage(v->hWndMainWindow, WM_CLOSE, 0, 0);
		}
		break;
	case TVN_SELCHANGED:
		{
			if (v->fDeletingAllTVItems == TRUE)
				break;
			if (v->fIPDVBMode == FALSE)
				HandleTreeClickMPEG2(hDlg, pnmh);
			else
				HandleTreeClickIPDVB(hDlg, pnmh);
		}
		break;
	case TVN_DELETEITEM:
		if (v->fIPDVBMode == TRUE)
		{
			LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;
			LocalFree((HLOCAL)pnmtv->itemOld.lParam);
		}
		break;
	case NM_RCLICK:
		{
			HTREEITEM hSelected = (HTREEITEM)SendDlgItemMessage(hDlg, IDC_SI_TREE, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
			if (hSelected != NULL)
				SendDlgItemMessage(hDlg, IDC_SI_TREE, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hSelected);
			else
				hSelected = v->hLastSelectedTreeItem;
			if (hSelected == NULL)
				break;
			if (v->fIPDVBMode == FALSE)
			{
				TVITEM tvi;

				memset(&tvi, 0, sizeof(tvi));
				tvi.mask = TVIF_PARAM;
				tvi.hItem = hSelected;
				TreeView_GetItem(GetDlgItem(hDlg, IDC_SI_TREE), &tvi);

				if ((tvi.lParam & 0xf0000000) == SI_PARSER_NIT)
				{
					RECT rc;
					POINT pt;
					HMENU hMenu;            // menu template         
					HMENU hMenuTrackPopup;  // floating pop-up menu
					int nNITRightClickIndex;

					nNITRightClickIndex = tvi.lParam & 0x0fffffff;
					if (nNITRightClickIndex == 0x0fffffff)
					{
#ifndef LITE
						// root of the NIT
						// Get the bounding rectangle of the client area
						GetClientRect(hDlg, (LPRECT) &rc); 

						// Get the client coordinates for the mouse click
						TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
						pt.x = rc.right; 
						pt.y = rc.top; 

						hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_NIT_EXPORT)); 
						if (hMenu == NULL) 
							return; 

						// Get the first pop-up menu in the menu template. This is the
						// menu that TrackPopupMenu displays. 
						hMenuTrackPopup = GetSubMenu(hMenu, 0);
						ClientToScreen(hDlg, (LPPOINT) &pt); 
						TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
						DestroyMenu(hMenu); 
#endif LITE
						return;

					}
					if (v->pNITData[nNITRightClickIndex] == NULL)
						return;

					if (   v->pNITData[nNITRightClickIndex]->nType == NIT_DVBT
						|| v->pNITData[nNITRightClickIndex]->nType == NIT_DVBS
						|| v->pNITData[nNITRightClickIndex]->nType == NIT_DVBC)
					{
						BOOL fNoRetune = FALSE;

						v->nNITRightClickIndex = nNITRightClickIndex;
						// Get the bounding rectangle of the client area
						GetClientRect(hDlg, (LPRECT) &rc); 

						// Get the client coordinates for the mouse click
						TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
						pt.x = rc.right; 
						pt.y = rc.top; 

						hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_NIT_POPUP)); 
						if (hMenu == NULL) 
							return; 

						// Get the first pop-up menu in the menu template. This is the
						// menu that TrackPopupMenu displays. 
						hMenuTrackPopup = GetSubMenu(hMenu, 0);
						if (strstr(v->szSourceName, "TSReader_File") != NULL)
							fNoRetune = TRUE;
						else if (v->dwSourceCapabilities & CAPABILITIES_TUNE_BY_CHANNEL)
							fNoRetune = TRUE;
						if (fNoRetune)
							DeleteMenu(hMenuTrackPopup, ID_IDRNITPOPUP_RETUNETOTHISMUX, MF_BYCOMMAND);
						ClientToScreen(hDlg, (LPPOINT) &pt); 
						TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
						DestroyMenu(hMenu); 
					}
				}
				else if ((tvi.lParam & 0xf0000000) == SI_PARSER_SDT)
				{
					RECT rc;
					POINT pt;
					HMENU hMenu;            // menu template         
					HMENU hMenuTrackPopup;  // floating pop-up menu
					int nSDTRightClickIndex;

					nSDTRightClickIndex = tvi.lParam & 0x0fffffff;
					if (strstr(v->szSourceName, "TSReader_File") != NULL)
						return;	// n/a for a file!
					if (v->nNetworkPID != 0x0010)
						return;
					{
						v->nSDTRightClickIndex = nSDTRightClickIndex;
						// Get the bounding rectangle of the client area
						GetClientRect(hDlg, (LPRECT) &rc); 

						// Get the client coordinates for the mouse click
						TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
						pt.x = rc.right; 
						pt.y = rc.top; 

						if (!(v->dwSourceCapabilities & CAPABILITIES_TUNE_BY_CHANNEL))
							hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_SDT_POPUP)); 
						else
							hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_SDT_CH_POPUP)); 
						if (hMenu == NULL) 
							return; 

						// Get the first pop-up menu in the menu template. This is the
						// menu that TrackPopupMenu displays. 
						hMenuTrackPopup = GetSubMenu(hMenu, 0);
						ClientToScreen(hDlg, (LPPOINT) &pt); 
						TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
						DestroyMenu(hMenu); 
					}
				}
#ifndef LITE
				else if ((tvi.lParam & 0xf0000000) == SI_PARSER_BAT)
				{
					RECT rc;
					POINT pt;
					HMENU hMenu;            // menu template         
					HMENU hMenuTrackPopup;  // floating pop-up menu

					if (tvi.lParam == SI_PARSER_BAT + MAX_BAT_ENTRIES)
						return;

					v->nBATRightClickIndex = tvi.lParam & 0x0fffffff;
					// Get the bounding rectangle of the client area
					GetClientRect(hDlg, (LPRECT) &rc); 

					// Get the client coordinates for the mouse click
					TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
					pt.x = rc.right; 
					pt.y = rc.top; 

					hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_BAT_POPUP)); 
					if (hMenu == NULL) 
						return; 

					// Get the first pop-up menu in the menu template. This is the
					// menu that TrackPopupMenu displays. 
					hMenuTrackPopup = GetSubMenu(hMenu, 0);
					ClientToScreen(hDlg, (LPPOINT) &pt); 
					TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
					DestroyMenu(hMenu); 
				}
				else if ((tvi.lParam & 0xf0000000) == SI_PARSER_TDT)
				{
					RECT rc;
					POINT pt;
					HMENU hMenu;            // menu template         
					HMENU hMenuTrackPopup;  // floating pop-up menu

					v->nTDTRightClickIndex = tvi.lParam & 0x0fffffff;
					GetClientRect(hDlg, (LPRECT) &rc); 
					TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
					pt.x = rc.right; 
					pt.y = rc.top; 
					hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_TDT_POPUP)); 
					if (hMenu == NULL) 
						return; 

					// Get the first pop-up menu in the menu template. This is the
					// menu that TrackPopupMenu displays. 
					hMenuTrackPopup = GetSubMenu(hMenu, 0);
					ClientToScreen(hDlg, (LPPOINT) &pt); 
					TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
					DestroyMenu(hMenu); 
				}
				else if ((tvi.lParam & 0xf0000000) == SI_PARSER_ES)
				{
					int nPMTIndex, nESIndex;
					BOOL fFound = FALSE;

					// It's an ES we just clicked on
					for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
					{
						if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
							break;
						for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
						{
							if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
								break;

							if (v->pat.pmt[nPMTIndex].es[nESIndex].hESTreeItem == hSelected)
							{
								fFound = TRUE;
								break;
							}
						}
						if (fFound == TRUE)
							break;
					}
					if (fFound)
					{
						RECT rc;
						POINT pt;
						HMENU hMenu;            // menu template         
						HMENU hMenuTrackPopup;  // floating pop-up menu

						v->nPopupSelectedPMTIndex = nPMTIndex;
						v->nPopupSelectedESIndex = nESIndex;
						
						GetClientRect(hDlg, (LPRECT) &rc); 
						TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
						pt.x = rc.right; 
						pt.y = rc.top; 
						hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_ES_POPUP)); 
						if (hMenu == NULL) 
							return; 

						// Get the first pop-up menu in the menu template. This is the
						// menu that TrackPopupMenu displays. 
						hMenuTrackPopup = GetSubMenu(hMenu, 0);
						if (!v->pat.pmt[nPMTIndex].es[nESIndex].nBlacklisted)
							EnableMenuItem(hMenuTrackPopup, ID_IDRESPOPUP_REMOVEESBLACKLIST, MF_GRAYED);
						ClientToScreen(hDlg, (LPPOINT) &pt); 
						TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
						DestroyMenu(hMenu); 
						break;
					}
				}
#ifdef PRO
				else
				{
					// See if it's a PCR tree-item
					int nPMTIndex;

					for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
					{
						if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
							break;
						if (v->pat.pmt[nPMTIndex].hPCRTreeItem == hSelected)
						{
							RECT rc;
							POINT pt;
							HMENU hMenu;            // menu template         
							HMENU hMenuTrackPopup;  // floating pop-up menu

							v->nSelectedPCRPID = v->pat.pmt[nPMTIndex].nPCRPID;
							GetClientRect(hDlg, (LPRECT) &rc); 
							TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
							pt.x = rc.right; 
							pt.y = rc.top; 
							hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_PCR_POPUP)); 
							if (hMenu == NULL) 
								return; 

							// Get the first pop-up menu in the menu template. This is the
							// menu that TrackPopupMenu displays. 
							hMenuTrackPopup = GetSubMenu(hMenu, 0);
							ClientToScreen(hDlg, (LPPOINT) &pt); 
							TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
							DestroyMenu(hMenu); 
							break;
						}
					}
				}
#endif PRO
#endif LITE
			}
			else
			{
				TVITEM tvi;
				PIPCLICKLPARAM ipclicklParam;

				memset(&tvi, 0, sizeof(tvi));
				tvi.mask = TVIF_PARAM;
				tvi.hItem = hSelected;
				TreeView_GetItem(GetDlgItem(hDlg, IDC_SI_TREE), &tvi);

				ipclicklParam = (PIPCLICKLPARAM)tvi.lParam;
				if (ipclicklParam == NULL)
					break;
				switch(ipclicklParam->nType)
				{
				case SI_PARSER_IP_PID:
				case SI_PARSER_IP_MAC:
					{
						RECT rc;
						POINT pt;
						HMENU hMenu;            // menu template         
						HMENU hMenuTrackPopup;  // floating pop-up menu											
						PIPENTRY pipentry = (PIPENTRY)ipclicklParam->dwPtr;

						// Get the bounding rectangle of the client area
						GetClientRect(hDlg, (LPRECT) &rc); 

						// Get the client coordinates for the mouse click
						TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
						pt.x = rc.right; 
						pt.y = rc.top; 

						if (ipclicklParam->nType == SI_PARSER_IP_PID)
						{
							hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_IP_POPUP_PID));
							v->pLastClickedIPPIDEntry = (PIPMACENTRY)v->ippid[ipclicklParam->dwPtr].pMACEntries;
						}
						else
						{
							hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_IP_POPUP_MAC)); 
							v->pLastClickedIPMACEntry = (PIPMACENTRY)ipclicklParam->dwPtr;
						}
						if (hMenu == NULL) 
							return; 

						// Get the first pop-up menu in the menu template. This is the
						// menu that TrackPopupMenu displays. 
						hMenuTrackPopup = GetSubMenu(hMenu, 0);						
						ClientToScreen(hDlg, (LPPOINT) &pt); 
						TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
						DestroyMenu(hMenu); 
					}
					break;
				case SI_PARSER_IP_IP:
					{
						RECT rc;
						POINT pt;
						HMENU hMenu;            // menu template         
						HMENU hMenuTrackPopup;  // floating pop-up menu
						PIPENTRY pipentry = (PIPENTRY)ipclicklParam->dwPtr;

						v->pLastClickedIPEntry = pipentry;

						// Get the bounding rectangle of the client area
						GetClientRect(hDlg, (LPRECT) &rc); 

						// Get the client coordinates for the mouse click
						TreeView_GetItemRect(GetDlgItem(hDlg, IDC_SI_TREE), hSelected, &rc, TRUE);
						pt.x = rc.right; 
						pt.y = rc.top; 

						hMenu = LoadMenu(v->hInstance, MAKEINTRESOURCE(IDR_IP_POPUP)); 
						if (hMenu == NULL) 
							return; 

						// Get the first pop-up menu in the menu template. This is the
						// menu that TrackPopupMenu displays. 
						hMenuTrackPopup = GetSubMenu(hMenu, 0);
						if (pipentry->hSaveFile == NULL)
							EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_STOPSAVING, MF_GRAYED);
						else
						{
							EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_SAVEPAYLOAD, MF_GRAYED);
							EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_SAVEPAYLOADANDXXXIPHEADER, MF_GRAYED);
							EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_SAVEPAYLOADXXXIPHEADERANDMPEHEADER, MF_GRAYED);
						}
						if (pipentry->dwProtocol != IP_UDP_ID) // UDP
						{
							EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_RETRANSMITPAYLOAD, MF_GRAYED);
							EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_STOPTRANSMITTING, MF_GRAYED);
						}
						else
						{
							if (pipentry->fTransmitting == FALSE)
								EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_STOPTRANSMITTING, MF_GRAYED);
							else
								EnableMenuItem(hMenuTrackPopup, ID_IDRIPPOPUP_RETRANSMITPAYLOAD, MF_GRAYED);
						}
						
						ClientToScreen(hDlg, (LPPOINT) &pt); 
						TrackPopupMenu(hMenuTrackPopup, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hDlg, NULL); 
						DestroyMenu(hMenu); 
					}
					break;
				}						
			}
		}
		break;
	}
}

void UpdatePIDChartingData(HWND hDlg)
{
	int i;
	int nOutputPIDCounter = 0;
	int nActivePIDCount = 0;

	if (v->fSortChartByPID == FALSE)
		memset(v->pc, 0, sizeof(PIDCOUNTER) * 8192);

	EnterCriticalSection(&v->ss.csPIDCounter);//zz
	for (i = 0; i < 8192; i++)
	{
		v->pc[nOutputPIDCounter].nPID = i;
		v->pc[nOutputPIDCounter].lnPackets = v->lnPIDCounter[i];
		if (v->fRealtimeCharting)
			v->lnPIDCounter[i] = 0;
		v->pc[nOutputPIDCounter].fScrambled = v->fPIDScrambled[i];
		if (v->lnPIDRateSamples[i])
			v->pc[nOutputPIDCounter].dPIDRate = v->dPIDRate[i] / (double)v->lnPIDRateSamples[i];
		else
			v->pc[nOutputPIDCounter].dPIDRate = 0.0;
		v->pc[nOutputPIDCounter].nPIDHasContinuityErrors = v->nPIDHasContinuityErrors[i];
		v->pc[nOutputPIDCounter].nPIDTEICount = v->nPIDTEICount[i];
		if (v->fSortChartByPID == FALSE)
		{
			if (v->pc[nOutputPIDCounter].lnPackets)
				nOutputPIDCounter++;
		}
		else
			nOutputPIDCounter++;
	}
	v->lnCopyTotalTSPackets = v->lnTotalTSPackets;
	if (v->fRealtimeCharting)
		v->lnTotalTSPackets = 0;
	LeaveCriticalSection(&v->ss.csPIDCounter);//zz

#ifdef PRO
	if (v->nPIDUsageStackedAreaChartIndex != -1)
		AddDataToPIDUsageStackedChart(v->nPIDUsageStackedAreaChartIndex);
#endif PRO

	if (v->fSortChartByPID == FALSE)
	{
		qsort(v->pc, nOutputPIDCounter, sizeof(PIDCOUNTER), SortPIDsByPackets);
		if (v->fSortChartDecending == TRUE)
		{
			int nStartPIDOffset;
			int nOutIndex = 0;

			memset(v->new_pc, 0, sizeof(PIDCOUNTER) * 8192);
			for (nStartPIDOffset = 0; nStartPIDOffset < 8192; nStartPIDOffset++)
			{
				if (v->pc[nStartPIDOffset].lnPackets == 0)
				{
					nStartPIDOffset--;
					break;
				}
			}
			for (i = nStartPIDOffset; i >= 0; i--)
				memcpy(&v->new_pc[nOutIndex++], &v->pc[i], sizeof(PIDCOUNTER));
			memcpy(v->pc, v->new_pc, sizeof(PIDCOUNTER) * 8192);
		}
	}
	else
	{
		if (v->fSortChartDecending == TRUE)
		{
			int nOutIndex = 0;

			memset(v->new_pc, 0, sizeof(PIDCOUNTER) * 8192);		
			for (i = 8191; i >= 0; i--)
				memcpy(&v->new_pc[nOutIndex++], &v->pc[i], sizeof(PIDCOUNTER));
			memcpy(v->pc, v->new_pc, sizeof(PIDCOUNTER) * 8192);
		}
	}
	for (i = 0; i < 8192; i++)
	{
		if (v->pc[i].lnPackets > v->lnMaxPackets)
			v->lnMaxPackets = v->pc[i].lnPackets;
		if (v->pc[i].lnPackets)
			nActivePIDCount++;
	}
	if (v->nActivePIDCount != nActivePIDCount)
	{
		SetScrollRange(GetDlgItem(hDlg, IDC_PIDS_SCROLL), SB_CTL, 0, nActivePIDCount - v->nPIDChartItemCount, TRUE);
		v->nActivePIDCount = nActivePIDCount;
	}
}

void ShowStatistics(HDC hDC, int xStart, int yStart, int nTable, char * szDescription)
{
	int xOffset = 80 + (nTable * 47);
	SIZE textsize;
	char szTemp[64];

	GetTextExtentPoint(hDC, "X", 1, &textsize);

	TextOut(hDC, xStart + xOffset, yStart, szDescription, lstrlen(szDescription));
	
	if (v->nSIParserPackets[nTable] > 1000 * 1000)
		sprintf(szTemp, "%.1fm", (double)v->nSIParserPackets[nTable] / 1000.0 / 1000.0);
	else if (v->nSIParserPackets[nTable] > 1000)
		sprintf(szTemp, "%.1fk", (double)v->nSIParserPackets[nTable] / 1000.0);
	else
		wsprintf(szTemp, "%d", v->nSIParserPackets[nTable]);
	TextOut(hDC, xStart + xOffset, yStart + textsize.cy, szTemp, lstrlen(szTemp));
	
	if (v->nSIParserCRCs[nTable] > 1000 * 1000)
		sprintf(szTemp, "%.1fm", (double)v->nSIParserCRCs[nTable] / 1000.0 / 1000.0);
	else if (v->nSIParserCRCs[nTable] > 1000)
		sprintf(szTemp, "%.1fk", (double)v->nSIParserCRCs[nTable] / 1000.0);
	else
		wsprintf(szTemp, "%d", v->nSIParserCRCs[nTable]);
	TextOut(hDC, xStart + xOffset, yStart + textsize.cy + textsize.cy, szTemp, lstrlen(szTemp));
}

void UpdateStatistics(HWND hDlg, BOOL fFromTimer)
{
//		EnableDisableSDTEITTotals(hWnd, !v->fIgnoreEIT);

	__int64 nTotalBytes = 0;
	int nActiveByteSamples = 0;
	int xWidth, xStart, yStart, yEnd;
	int yCurrent;
	HDC hDC;
	HGDIOBJ hOldFont;
	RECT rcBorder, rcParent;
	RECT rcFill;
	SIZE textsize;

	if (!v->fRunning)
		return;

	hDC = GetDC(hDlg);
	if (v->hBr == NULL)
		CreateBrushes(hDC);
	hOldFont = SelectObject(hDC, v->hSourceInfoFont);
	GetTextExtentPoint(hDC, "X", 1, &textsize);
	
	GetWindowRect(GetDlgItem(v->hDlgSIParser, IDC_STATISTICS_FRAME), &rcBorder);
	GetWindowRect(v->hWndMainWindow, &rcParent);
	xWidth = rcBorder.right - rcBorder.left - 10;
	xStart = rcBorder.left - rcParent.left;
	yStart = rcBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) + 10;
	yCurrent = yStart;
	yEnd = yStart + (rcBorder.bottom - rcBorder.top) - 20;

	rcFill.top = yStart;
	rcFill.bottom = yEnd;
	rcFill.left = xStart;
	rcFill.right = xStart + xWidth;
	FillRect(hDC, &rcFill, GetStockObject(BLACK_BRUSH));

	yStart += 2;
	xStart += 5;
	
	SetBkColor(hDC, RGB(0x00, 0x00, 0x00));
	{
		int i;
		double dRate;

		static char szSections[] = {"Sections"};
		static char szCRCErrors[] = {"CRC Errors"};
		static char szContinuityErrors[] = {"Continuity Errors:"};
		static char szTEIErrors[] = {"TEI Errors:"};
		static char szMuxBitrate[] = {"Mux. bitrate:"};
		static char szRecorded[] = {"Recorded:"};
		static char szLastSecond[] = {"Last sec.:"};
		static char szContinuityIndicator[] = {"* after the bitrate indicates the PID has continuity errors"};
		static char szNotAvailable[] = {"n/a"};
		static char szSyncLosses[] = {"Sync losses:"};
		static char szRetunes[] = {"Retunes:"};
		static char szInputBuffer[] = {"In buffer:"};
		static char szOutputBuffer[] = {"Out buffer:"};
		static char szDataLastSecond[64] = {0};
		char szNITLabel[8] = {"NIT"};
		char szSDTLabel[8] = {"SDT"};
		char szTemp[256];

		if (v->fParserDisabled)
			SetTextColor(hDC, RGB(0x00, 0x7f, 0x00));
		else
			SetTextColor(hDC, RGB(0x00, 0xff, 0x00));

		TextOut(hDC, xStart, yStart + textsize.cy, szSections, lstrlen(szSections));
		TextOut(hDC, xStart, yStart + textsize.cy + textsize.cy, szCRCErrors, lstrlen(szCRCErrors));

		if (v->nNetworkPID == 0x1ffb)
		{
			lstrcpy(szNITLabel, "ETT");
			lstrcpy(szSDTLabel, "PSIP");
		}
				
		ShowStatistics(hDC, xStart, yStart, SI_PARSER_STATS_PAT, "PAT");
		ShowStatistics(hDC, xStart, yStart, SI_PARSER_STATS_CAT, "CAT");
		ShowStatistics(hDC, xStart, yStart, SI_PARSER_STATS_PMT, "PMT");
		ShowStatistics(hDC, xStart, yStart, SI_PARSER_STATS_NIT, szNITLabel);
		if (v->nNetworkPID != 0x0ffe)
		{
			ShowStatistics(hDC, xStart, yStart, SI_PARSER_STATS_SDT, szSDTLabel);
			ShowStatistics(hDC, xStart, yStart, SI_PARSER_STATS_EIT, "EIT");
		}

		SetTextColor(hDC, RGB(0x00, 0xff, 0x00));
		yStart += (textsize.cy * 3) + 2;
		wsprintf(szTemp, "%d", v->nContinuityErrors);
		TextOut(hDC, xStart, yStart, szContinuityErrors, lstrlen(szContinuityErrors));
		TextOut(hDC, xStart + 125, yStart, szTemp, lstrlen(szTemp));
		wsprintf(szTemp, "%d", v->nTEIErrors);
		TextOut(hDC, xStart, yStart + textsize.cy, szTEIErrors, lstrlen(szTEIErrors));
		TextOut(hDC, xStart + 125, yStart + textsize.cy, szTemp, lstrlen(szTemp));

		if (GetSyncLossCount != NULL)
		{
			v->nSyncLossCount = GetSyncLossCount(FALSE);
			wsprintf(szTemp, "%d", v->nSyncLossCount);
			TextOut(hDC, xStart, yStart + textsize.cy + textsize.cy, szSyncLosses, lstrlen(szSyncLosses));
			TextOut(hDC, xStart + 125, yStart + textsize.cy + textsize.cy, szTemp, lstrlen(szTemp));
		}
		if (GetRetuneCount != NULL)
		{
			v->nRetuneCount = GetRetuneCount(FALSE);
			wsprintf(szTemp, "%d", v->nRetuneCount);
			TextOut(hDC, xStart, yStart + textsize.cy + textsize.cy + textsize.cy, szRetunes, lstrlen(szRetunes));
			TextOut(hDC, xStart + 125, yStart + textsize.cy + textsize.cy + textsize.cy, szTemp, lstrlen(szTemp));
		}
#ifndef LITE
		if (v->nContinuityErrors)
			TextOut(hDC, xStart, yEnd - textsize.cy, szContinuityIndicator, lstrlen(szContinuityIndicator));
#endif LITE
		
		TextOut(hDC, xStart + 180, yStart, szMuxBitrate, lstrlen(szMuxBitrate));
		if (v->dDisplayMuxRate > 0)
		{
			EnterCriticalSection(&v->ss.csPIDCounter);
			sprintf(szTemp, "%.0f bps", (v->dDisplayMuxRate / (double)v->nMuxRateCounter) * 8.0);
			LeaveCriticalSection(&v->ss.csPIDCounter);
			TextOut(hDC, xStart + 270, yStart, szTemp, lstrlen(szTemp));
		}
		else
			TextOut(hDC, xStart + 270, yStart, szNotAvailable, lstrlen(szNotAvailable));
			
		if (v->fRecording == TRUE)
		{
			if ((!v->fRecordDialogStreamOnly) || (v->nStreamTo == STREAM_TO_DVHS))
			{
				if (v->dTotalRecorded / 1024.0 / 1024.0 > 1000)
					sprintf(szTemp, "%.3f GB", v->dTotalRecorded / 1024.0 / 1024.0 / 1024.0);
				else			
					sprintf(szTemp, "%.3f MB", v->dTotalRecorded / 1024.0 / 1024.0);
				TextOut(hDC, xStart + 180, yStart + (textsize.cy * 4), szRecorded, lstrlen(szRecorded));
				TextOut(hDC, xStart + 270, yStart + (textsize.cy * 4), szTemp, lstrlen(szTemp));
			}
		}

		if (fFromTimer)
		{
			EnterCriticalSection(&v->ss.csPIDCounter);
			for (i = 0; i < 59; i++)
				v->lnPIDSecondCounter[i] = v->lnPIDSecondCounter[i + 1];
			v->lnPIDSecondCounter[59] = v->ss.nLastSecondByteCounter;
			v->ss.nLastSecondByteCounter = 0;
			LeaveCriticalSection(&v->ss.csPIDCounter);
			for (i = 0; i < 60; i++)
			{
				nTotalBytes += v->lnPIDSecondCounter[i];
				if (v->lnPIDSecondCounter[i] != 0)
					nActiveByteSamples++;
			}	
			//dRate = ((double)nTotalBytes / (double)nActiveByteSamples) * 8.0;
			dRate = (double)v->lnPIDSecondCounter[59] * 8.0;
			sprintf(szDataLastSecond, "%.3f Mbit", dRate / 1000.0 / 1000.0);
		}
		TextOut(hDC, xStart + 180, yStart + textsize.cy, szLastSecond, lstrlen(szLastSecond));
		TextOut(hDC, xStart + 270, yStart + textsize.cy, szDataLastSecond, lstrlen(szDataLastSecond));		

		// Input & output buffer fullness
		{
			double dPercent = ((double)v->nTSBuffers / (double)MAX_TS_BUFFERS) * 100.0;
			int nPercent = (int)dPercent;
			POINT pt[2];

			TextOut(hDC, xStart + 180, yStart + (textsize.cy) * 2, szInputBuffer, lstrlen(szInputBuffer));
			if (nPercent > 100)
				nPercent = 100;
			if (nPercent)
			{
				if (nPercent > 75)
					SelectObject(hDC, v->hRedPen);
				else
					SelectObject(hDC, v->hGreenPen);
				pt[0].x = xStart + 265;
				pt[1].x = pt[0].x + nPercent;
				pt[0].y = pt[1].y = yStart + ((textsize.cy) * 2) + textsize.cy / 2;
				Polyline(hDC, pt, 2);
			}
			
			TextOut(hDC, xStart + 180, yStart + (textsize.cy) * 3, szOutputBuffer, lstrlen(szOutputBuffer));		
			dPercent = ((double)v->nStreamBuffers / ((double)v->nStreamingPipeSize * 1024.0 * 1024.0)) * 100.0;
			nPercent = (int)dPercent;
			if (nPercent > 100)
				nPercent = 100;
			if (nPercent)
			{
				if (nPercent > 75)
					SelectObject(hDC, v->hRedPen);
				else
					SelectObject(hDC, v->hGreenPen);
				pt[0].x = xStart + 265;
				pt[1].x = pt[0].x + nPercent;
				pt[0].y = pt[1].y = yStart + ((textsize.cy) * 3) + textsize.cy / 2;
				Polyline(hDC, pt, 2);
			}
		}			
	}
	
	SelectObject(hDC, hOldFont);
	ReleaseDC(hDlg, hDC);
}

void ResetTreeViewHandles()
{
	int i;

	v->hNITRootTreeItem = NULL;
	v->hSDTRootTreeItem = NULL;
	v->hEITRootTreeItem = NULL;
	v->hMMTRootTreeItem = NULL;
	v->hCDTRootTreeItem = NULL;
	v->hSITRootTreeItem = NULL;
	v->hTDTRootTreeItem = NULL;
	v->hMGTRootTreeItem = NULL;
	v->hBATRootTreeItem = NULL;
	v->pat.hPATTreeItem = NULL;
	v->dvbtdt.hRootTreeItem = NULL;
	v->dvbtdt.hTreeItem = NULL;
	v->dvbtot.hRootTreeItem = NULL;
	v->dvbtot.hTreeItem = NULL;
	v->hRRTRootTreeItem = NULL;
	v->hCVCTRootTreeItem = NULL;
	for (i = 0; i < MAX_PAT_ENTRIES; i++)
		v->pat.pmt[i].hPMTTreeItem = NULL;
	for (i = 0; i < MAX_EIT_CHANNEL_DATA; i++)
		v->hEITTreeItem[i] = NULL;
}

void ResetParserPackets()
{
	v->nSIParserPackets[SI_PARSER_STATS_PAT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_PMT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_NIT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_SDT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_EIT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_CAT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_BAT] = 0;
	v->nSIParserPackets[SI_PARSER_STATS_TDT] = 0;
}

void ResetParserCRCs()
{
	v->nSIParserCRCs[SI_PARSER_STATS_PAT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_PMT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_NIT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_SDT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_EIT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_CAT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_BAT] = 0;
	v->nSIParserCRCs[SI_PARSER_STATS_TDT] = 0;
}

#ifdef PRO
void ResetParserTableErrors()
{
	v->nSIParserTableErrors[SI_PARSER_STATS_PAT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_PMT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_NIT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_SDT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_EIT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_CAT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_BAT] = 0;
	v->nSIParserTableErrors[SI_PARSER_STATS_TDT] = 0;
}

void ResetTableTimes()
{
	v->dSIParserTableTime[SI_PARSER_STATS_PAT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_PMT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_NIT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_SDT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_EIT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_CAT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_BAT] = 0.0;
	v->dSIParserTableTime[SI_PARSER_STATS_TDT] = 0.0;
}

void ResetTableTimingErrors()
{
	v->nSIParserTimingErrors[SI_PARSER_STATS_PAT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_PMT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_NIT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_SDT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_EIT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_CAT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_BAT] = 0;
	v->nSIParserTimingErrors[SI_PARSER_STATS_TDT] = 0;
}
#endif PRO

void ResetPIDChart(HWND hWnd)
{
	int nPID;

	EnterCriticalSection(&v->ss.csPIDCounter);
	for (nPID = 0; nPID < 8192; nPID++)
	{
		v->lnPIDCounter[nPID] = 0;
		v->fPIDActive[nPID] = 0;
		v->dPIDRate[nPID] = 0;
		v->lnPIDRateSamples[nPID] = 0;
		v->lnPIDRateBytes[nPID] = 0;
		v->lnPIDRatePCR[nPID] = 0;
	}
	v->lnTotalTSPackets = 0;
	v->nMuxRatePID = 0x1fff;
	v->dDisplayMuxRate = 0;
	v->nMuxRateCounter = 0;
	v->lnMuxRatePCR = 0;
	v->nMuxRateBytes = 0;
	LeaveCriticalSection(&v->ss.csPIDCounter);
	UpdatePIDChart(v->hDlgSIParser);
	UpdateStatistics(v->hDlgSIParser, FALSE);
}

int ThumbnailThreadsRunning()
{
	int nRetVal = 0;
	int nES;

	for (nES = 0; nES < MAX_ES_PARSERS; nES++)
	{
		EnterCriticalSection(&v->esparserinfo[nES].csThreadSignal);
		if (v->fMPEG2DecoderThreadRunning[nES])
			nRetVal++;
		LeaveCriticalSection(&v->esparserinfo[nES].csThreadSignal);
	}
	return nRetVal;
}

void WaitForThumbnailThread(HWND hWnd)
{
	int nMPEGWaitCount = 0;

	while (ThumbnailThreadsRunning() && nMPEGWaitCount++ < 100)
	{
		MSG msg;

		while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(10);
	}
	if (nMPEGWaitCount < 100)
		OutputDebugString("TSReader: Thumbnail decoder thread completed\n");
	else
		OutputDebugString("TSReader: Thumbnail decoder thread timeout\n");
}

#ifndef LITE
void SetupForNewStream(HWND hWnd)
{
	int i;
	HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);

	SetDlgItemText(v->hDlgSIParser, IDC_SI_TEXT, "");

	SendMessage(hWndTV, WM_SETREDRAW, FALSE, 0);
	v->fDeletingAllTVItems = TRUE;
	TreeView_DeleteAllItems(hWndTV);	
	v->fDeletingAllTVItems = FALSE;
	v->fTreeViewSelectedAtLeastOnce = FALSE;
	SendMessage(hWndTV, WM_SETREDRAW, TRUE, 0);
	
	WaitForThumbnailThread(hWnd);
	CleanupMPEGParsingThread(hWnd);
	memset(&v->pat, 0, sizeof(v->pat));
	v->pat.nVersionNumber = -1;
	v->cat.nVersionNumber = -1;
	v->nCaptionPID = -1;
	for (i = 0; i < MAX_CHARTS; i++)
		v->nVideoCompositionPID[i] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_BAT] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_SDT] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_NIT] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_EIT] = -1;
	v->ss.nPATCATProcessed = 0;
	v->fSkyEPGMapComplete = FALSE;
	
	v->nEITTreeItemCount = 0;
	v->nSDTTreeItemCount = 0;
	v->nNITTreeItemCount = 0;
	v->nBATTreeItemCount = 0;

#ifndef LITE
	memset(v->bDescriptorTagArray, 0, sizeof(bDescriptorTagArray));
	for (i = 0; i < MAX_PAT_ENTRIES; i++)
		memset(&v->pmtlisten[i], 0, sizeof(PMTLISTEN));
#endif LITE

	v->fIPDVBMode = FALSE;
	CleanupIPParsingThread(hWnd);
	CheckMenuItem(GetMenu(v->hWndMainWindow), IDC_SI_PARSER_IP_DVB_MODE, MF_UNCHECKED | MF_BYCOMMAND);		
		
	for (i = 0; i < 60; i++)
		v->lnPIDSecondCounter[i] = 0;
	v->ss.nLastSecondByteCounter = 0;
	v->nAutoRestartOnDataStopCounter = 0;
	for (i = 0; i < MAX_ES_PARSERS; i++)
		v->lnESParseStartTime[i] = 0;

	//memset(&v->lnPIDSecondCounter, 0, sizeof(v->lnPIDSecondCounter));
	for (i = 0; i < 8192; i++)
	{
		v->lnPIDCounter[i] = 0;
		v->fPIDActive[i] = FALSE;
		v->fPIDScrambled[i] = FALSE;
		v->nPIDContinuity[i] = -1;
		v->dPIDRate[i] = 0;
		v->lnPIDRateSamples[i] = 0;
		v->nPIDHasContinuityErrors[i] = 0;
		v->nPIDTEICount[i] = 0;
		v->lnPIDRateBytes[i] = 0;
		v->nProgramPIDs[i] = 0;
		v->lnPIDRatePCR[i] = 0;
		memset(&v->pc[i], 0, sizeof(v->pc[i]));
	}

	v->lnTotalTSPackets = v->lnCopyTotalTSPackets = 0;
	v->dDisplayMuxRate = 0;
	v->nMuxRateCounter = 0;
	v->nPMTPID = 0;
	v->nCheckPATCounter = 0;
	v->nLastPATHighestVersionNumber = -1;
	v->ss.nTSBuffersInUse = 0;
	v->nSelectedProgram	= -1;
	if (!v->fForcedNetworkType)
	{
		v->nNetworkPID = -1;
		v->fISDB = FALSE;
	}

	for (i = 0; i < MAX_ES_PARSERS; i++)
		v->nESParsePID[i] = 0x1fff;
	v->nMuxRatePID = 0x1fff;
	v->nPMTTimeoutCounter = 0;
	v->nThumbnailImageCount = 0;
	v->nMinimumSDTChannel = 65536;
	v->nMinimumEITChannel = 65536;
	v->nTransmittingCount = 0;
	v->fDirtyManualChannels = FALSE;
	v->nTreeUpdateCounter2 = 0;
	v->nSkyEPGPIDs[0] = 0x36;
	v->nSkyEPGPIDs[1] = 0x46;
	v->dvbtdt.nGPSOffset = 0;
	v->dvbtdt.fSTTSeen = FALSE;

	for (i = 0; i < MAX_SIT_ENTRIES; i++)
		memset(&v->sit[i], 0, sizeof(v->sit[i]));
	for (i = 0; i < MAX_TDT_ENTRIES; i++)
		memset(&v->tdt[i], 0, sizeof(v->tdt[i]));
	for (i = 0; i < 64; i++)
		v->nATSCEITPID[i] = v->nATSCETTPID[i] = 0x1fff;
	v->nATSCCETTPID = -1;

	ResetParserPackets();
	ResetParserCRCs();
#ifdef PRO
	ResetParserTableErrors();
	ResetTableTimes();
	ResetTableTimingErrors();
#endif PRO
	v->nDCIIECMPMTIndex = 0;
	v->nDCIIECMDescriptorPID = -1;

	v->nContinuityErrors = 0;
	v->nTEIErrors = 0;
	memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));

	ResetTreeViewHandles();

	for (i = 0; i < MAX_MGT_ENTRIES; i++)
		v->mgt[i].nTableType = -1;
	for (i = 0; i < MAX_PAT_ENTRIES; i++)
		v->pat.pmt[i].fCompleted = FALSE;

	v->fPostInitialParse = FALSE;
	GetNextESPID(TRUE, 0);

	v->nThumbnailScrollOffset = 0;
	SetScrollPos(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), SB_CTL, v->nThumbnailScrollOffset, TRUE);
	SetScrollRange(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), SB_CTL, 0, 0, TRUE);
	v->nPIDScrollOffset = 0;
	SetScrollPos(GetDlgItem(v->hDlgSIParser, IDC_PIDS_SCROLL), SB_CTL, v->nPIDScrollOffset, TRUE);

	if (v->hThumbnailDC != NULL)
	{
		DeleteObject(v->memThumbnailBM);
		DeleteDC(v->hThumbnailDC);
		v->hThumbnailDC = NULL;
	}
	InvalidateRect(v->hDlgSIParser, NULL, TRUE);

#ifdef PRO
	{
		int nTableIndex;
		for (nTableIndex = 0; nTableIndex < MAX_RECORD_TABLES; nTableIndex++)
			v->record_tables[nTableIndex].nPID = -1;
	}
	if (v->fStreamingXMLMode)
	{
		v->nXMLLogCount = v->nXMLLogMax = 0;
		if (v->XMLLog != NULL)
		{
			LocalFree(v->XMLLog);
			v->XMLLog = NULL;
		}
	}
#endif PRO
}
#endif LITE

#ifndef LITE
BOOL RetuneForSIParser(HWND hDlg, int nFrequency, int nSymbolRate, int nPolarization)
{
	BOOL fRetVal = FALSE;
	DWORD dwThreadID = 0;
	char szTemp[128];

	wsprintf(szTemp, "Retuning to %d MHz", nFrequency);
	UpdateMainStatusText(szTemp);

	v->ss.fTerminateReadThread = TRUE;
	while (v->ss.fReadThreadTerminated == FALSE)
		Sleep(50);
	
	SetupForNewStream(hDlg);

	return fRetVal;
}
#endif LITE


double GetPercentageOfTransportStream(int nPID)
{
	int i;

	for (i = 0; i < 8191; i++)
	{
		if (nPID == v->pc[i].nPID)
		{
			double dPercent = ((double)v->pc[i].lnPackets / (double)v->lnCopyTotalTSPackets) * 100.0;			
			return dPercent;
		}
	}
	return 0.0;
}

void SIParserExportNIT(HWND hWnd)
{
#ifdef LITE
	MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
	int nNITIndex;
	int nRetVal;
	BOOL fFirstTime = TRUE;
	OPENFILENAME ofn;
	HANDLE hHTMFile;
	char szFile[MAX_PATH] = TEXT(".htm\0");
	char szTemp[1024];

	if (v->nNetworkPID != 0x0010)
	{
		MessageBox(hWnd, "This function is only supported on DVB networks", gszAppName, MB_ICONSTOP);
		return;
	}

	MessageBox(hWnd, "This function is currently broken", gszAppName, MB_ICONSTOP);
	return;

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("HTML Files (*.htm)\0*.htm\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Export SI via NIT");
	ofn.lpstrDefExt = TEXT("htm");
	ofn.lpstrInitialDir = v->szHTMInitialDir;
	ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	ofn.hInstance = v->hInstance;
	if (myGetSaveFileName(&ofn) == FALSE)
		return;
	hHTMFile = CreateFile(ofn.lpstrFile,
				          GENERIC_WRITE,
				          0,
						  (LPSECURITY_ATTRIBUTES) NULL,
						  CREATE_ALWAYS,
						  FILE_ATTRIBUTE_NORMAL,
						  (HANDLE) NULL);
	if (hHTMFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hWnd, TEXT("Unable to open file for output"), gszAppName, MB_OK | MB_ICONINFORMATION);
		return;
	}
	else
	{
#ifdef DEBUG_MESSAGES
		wsprintf(szTemp, "Handle %x\n", hHTMFile);
		OutputDebugString(szTemp);
#endif DEBUG_MESSAGES
	}
	CursorWait(hWnd);

	// Write the header
	WriteHTMLLine(hHTMFile, "<HTML>");
	WriteHTMLLine(hHTMFile, "<TITLE>DVB-SI Parsing by COOLSTF.com TSReader</TITLE>");
	WriteHTMLLine(hHTMFile, "<BODY>");

	for (nNITIndex = 0; nNITIndex < MAX_TS_ENTRIES; nNITIndex++)
	{		
		if (v->pNITData[nNITIndex] != NULL)
		{
			if (v->pNITData[nNITIndex]->fThisTS == TRUE)
			{
				nRetVal = RetuneForSIParser(
								 hWnd,
								 v->pNITData[nNITIndex]->nFrequency / 100,
								 v->pNITData[nNITIndex]->dvbs.nSymbolRate / 10,
								 !(v->pNITData[nNITIndex]->dvbs.nPolarization & 1));
				if (!nRetVal)
				{
					char szPolarity[20];

					FormatPolarity(szPolarity, v->pNITData[nNITIndex]->dvbs.nPolarization, TRUE);
					sprintf(szTemp, "Failed to lock %.3f GHz %d %s NID = %d TID = %d<P>",
							 (double)v->pNITData[nNITIndex]->nFrequency / 100000.0,
					         v->pNITData[nNITIndex]->dvbs.nSymbolRate / 10,
							 szPolarity,
							v->pNITData[nNITIndex]->nNetworkID,
							v->pNITData[nNITIndex]->nTransportStreamID);
					WriteHTMLLine(hHTMFile, szTemp);
				}
				else
				{
					int i;
					char szFEC[10];
					char szPolarity[20];

					if (fFirstTime == TRUE)
					{
						fFirstTime = FALSE;
						wsprintf(szTemp, "<B>DVB Network Scan for %s</B><P>", v->pNITData[nNITIndex]->szNetworkName);
						WriteHTMLLine(hHTMFile, szTemp);
					}
					FormatPolarity(szPolarity, v->pNITData[nNITIndex]->dvbs.nPolarization, TRUE);
					DecodeFEC(v->pNITData[nNITIndex]->dvbs.nFEC, szFEC, FALSE);
					sprintf(szTemp, "%.3f GHz %s %5d %s NID = %d TID = %d<BR>",
							(double)v->pNITData[nNITIndex]->nFrequency / 100000.0,
							szPolarity,
							v->pNITData[nNITIndex]->dvbs.nSymbolRate / 10,
							szFEC,
							v->pNITData[nNITIndex]->nNetworkID,
							v->pNITData[nNITIndex]->nTransportStreamID);
					WriteHTMLLine(hHTMFile, szTemp);

					// Wait for PMT procesing to complete
					while (v->nPMTPID != 0x1fff)
					{
						MSG msg;

						while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}

						Sleep(50);				
					}
					Sleep(250);
					UpdateMainStatusText("Waiting for PID percentage...");
					for (i = 0; i < 50; i++)
					{
						MSG msg;

						while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}

						Sleep(100);				
					}

					for (i = 0; i < MAX_PAT_ENTRIES; i++)
					{
						int j;

						if (v->pat.pmt[i].nPMTPID == 0)
							break;
						if (!v->pat.pmt[i].nProgramNumber)
							continue;

						wsprintf(szTemp, "%5d ", v->pat.pmt[i].nProgramNumber);
						if (v->pChannelData[v->pat.pmt[i].nProgramNumber] != NULL)
						{
							char szTemp2[128];

							wsprintf(szTemp2, "%s (%s)", v->pChannelData[v->pat.pmt[i].nProgramNumber]->szShortName, v->pChannelData[v->pat.pmt[i].nProgramNumber]->szLongName);
							lstrcat(szTemp, szTemp2);
						}
						WriteHTMLLine(hHTMFile, szTemp);

						szTemp[0] = '\0';
						for (j = 0; j < MAX_ESLIST_ENTRIES; j++)
						{
							char szTemp2[128];

							if (v->pat.pmt[i].es[j].nESPID == 0)
								break;

							switch(v->pat.pmt[i].es[j].nStreamType)
							{
							default:
								{
									BOOL fEncrypted;
SIParserExportNIT_default:
									fEncrypted = FALSE;

									if (v->pat.pmt[i].es[j].pDescriptors != NULL)
									{
										int nDescriptorsLength = v->pat.pmt[i].es[j].nDescriptorsLength;
										int nCurrentIndex = 0;

										do
										{
											if (v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex] == 9)
												fEncrypted = TRUE;
											nCurrentIndex += (BYTE)v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex + 1]; // descriptor length
											nCurrentIndex += 2;	// descriptor tag and length
										} while (nCurrentIndex < nDescriptorsLength);
									}
									if (fEncrypted == TRUE)
										lstrcat(szTemp, "<FONT COLOR=RED>");
									else
										lstrcat(szTemp, "<FONT COLOR=GREEN>");
									sprintf(szTemp2, "ES%02x %d (%.3f%%) ", v->pat.pmt[i].es[j].nStreamType, v->pat.pmt[i].es[j].nESPID, GetPercentageOfTransportStream(v->pat.pmt[i].es[j].nESPID));
									lstrcat(szTemp, szTemp2);
									lstrcat(szTemp, "</FONT>");
								}
								break;
							case 1:		// mpeg-1 video
							case 2:		// mpeg-2 video
								{
									BOOL fEncrypted = FALSE;

									if (v->pat.pmt[i].es[j].pDescriptors != NULL)
									{
										int nDescriptorsLength = v->pat.pmt[i].es[j].nDescriptorsLength;
										int nCurrentIndex = 0;

										do
										{
											if (v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex] == 9)
												fEncrypted = TRUE;
											nCurrentIndex += (BYTE)v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex + 1]; // descriptor length
											nCurrentIndex += 2;	// descriptor tag and length
										} while (nCurrentIndex < nDescriptorsLength);
									}
									if (fEncrypted == TRUE)
										lstrcat(szTemp, "<FONT COLOR=RED>");
									else
										lstrcat(szTemp, "<FONT COLOR=GREEN>");
									sprintf(szTemp2, "V %d (%.3f%%) ", v->pat.pmt[i].es[j].nESPID, GetPercentageOfTransportStream(v->pat.pmt[i].es[j].nESPID));
									lstrcat(szTemp, szTemp2);
									lstrcat(szTemp, "</FONT>");
								}
								break;
							case 0x03:	// mpeg-1 audio
							case 0x04:	// mpeg-2 audio
							case 0x06:	// perhaps AC-3
							case 0x81:	// ac-3 audio
							case 0x83:	// LPCM audio
							case 0x85:	// DTS audio
								{
									BOOL fEncrypted = FALSE;
									char szLanguage[4] = {0};

									if (v->pat.pmt[i].es[j].nStreamType == 0x06)
									{
										if (   IsAC3AudioStream(i, j) == FALSE
											&& IsPCMAudioStream(i, j) == FALSE
											&& IsDTSAudioStream(i, j) == FALSE)
											goto SIParserExportNIT_default;
									}

									if (v->pat.pmt[i].es[j].pDescriptors != NULL)
									{
										int nDescriptorsLength = v->pat.pmt[i].es[j].nDescriptorsLength;
										int nCurrentIndex = 0;

										do
										{
											if (v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex] == 9)
												fEncrypted = TRUE;
											if (v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex] == 10)
											{
												szLanguage[0] = v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex + 2];
												szLanguage[1] = v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex + 3];
												szLanguage[2] = v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex + 4];
												szLanguage[3] = 0;
											}
											nCurrentIndex += (BYTE)v->pat.pmt[i].es[j].pDescriptors[nCurrentIndex + 1]; // descriptor length
											nCurrentIndex += 2;	// descriptor tag and length
										} while (nCurrentIndex < nDescriptorsLength);
									}
									if (fEncrypted == TRUE)
										lstrcat(szTemp, "<FONT COLOR=RED>");
									else
										lstrcat(szTemp, "<FONT COLOR=GREEN>");
									sprintf(szTemp2, "A %d (%.3f%%) ", v->pat.pmt[i].es[j].nESPID, GetPercentageOfTransportStream(v->pat.pmt[i].es[j].nESPID));
									lstrcat(szTemp, szTemp2);
									if (v->pat.pmt[i].es[j].nStreamType == 0x81)
										lstrcat(szTemp, "(AC3) ");
									if (v->pat.pmt[i].es[j].nStreamType == 0x83)
										lstrcat(szTemp, "(LPCM) ");
									if (v->pat.pmt[i].es[j].nStreamType == 0x85)
										lstrcat(szTemp, "(DTS) ");
									if (lstrlen(szLanguage))
									{
										wsprintf(szTemp2, "(%s) ", szLanguage);
										lstrcat(szTemp, szTemp2);
									}
									lstrcat(szTemp, "</FONT>");
									break;
								}
							}
						}
						if (lstrlen(szTemp))
							WriteHTMLLine(hHTMFile, szTemp);

						WriteHTMLLine(hHTMFile, "<BR>");
					}

					WriteHTMLLine(hHTMFile, "<P>");
				}
			}
		}
	}

	CloseHandle(hHTMFile);

	nRetVal = RetuneForSIParser(
					 hWnd,
					 v->ss.nFrequency,
					 v->ss.nSymbolRate,
					 v->ss.nPolarity);	
	
	CursorNormal();
#endif LITE
}

void HandleThumbnailScroll(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	BOOL fRedraw = FALSE;

	switch(LOWORD(wParam))
	{
	case SB_LINEUP:
		if (v->nThumbnailScrollOffset > 0)
		{
			v->nThumbnailScrollOffset--;
			SetScrollPos(GetDlgItem(hDlg, IDC_SCROLL_THUMBNAILS), SB_CTL, v->nThumbnailScrollOffset, TRUE);
			fRedraw = TRUE;
		}
		break;
	case SB_LINEDOWN:
		if (v->nThumbnailScrollOffset < v->nThumbnailImageCount - v->nThumbnailDisplayCount)
		{
			v->nThumbnailScrollOffset++;
			SetScrollPos(GetDlgItem(hDlg, IDC_SCROLL_THUMBNAILS), SB_CTL, v->nThumbnailScrollOffset, TRUE);
			fRedraw = TRUE;
		}
		break;
	case SB_THUMBTRACK:
		v->nThumbnailScrollOffset = HIWORD(wParam);
		SetScrollPos(GetDlgItem(hDlg, IDC_SCROLL_THUMBNAILS), SB_CTL, v->nThumbnailScrollOffset, TRUE);
		fRedraw = TRUE;
		break;
	case SB_PAGEUP:
		{
			v->nThumbnailScrollOffset -= v->nThumbnailDisplayCount;
			if (v->nThumbnailScrollOffset < 0)
				v->nThumbnailScrollOffset = 0;
			SetScrollPos(GetDlgItem(hDlg, IDC_SCROLL_THUMBNAILS), SB_CTL, v->nThumbnailScrollOffset, TRUE);
			fRedraw = TRUE;
			break;
		}
	case SB_PAGEDOWN:
		{
			v->nThumbnailScrollOffset += v->nThumbnailDisplayCount;
			if (v->nThumbnailScrollOffset > v->nThumbnailImageCount - v->nThumbnailDisplayCount)
				v->nThumbnailScrollOffset = v->nThumbnailImageCount - v->nThumbnailDisplayCount;
			SetScrollPos(GetDlgItem(hDlg, IDC_SCROLL_THUMBNAILS), SB_CTL, v->nThumbnailScrollOffset, TRUE);
			fRedraw = TRUE;
			break;
		}
	}
	if (fRedraw == TRUE)
		UpdateVideoPix(hDlg);
}

void HandlePIDScroll(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	BOOL fRedraw = FALSE;

	switch(LOWORD(wParam))
	{
	case SB_LINEUP:
		if (v->nPIDScrollOffset > 0)
		{
			v->nPIDScrollOffset--;
			SetScrollPos(GetDlgItem(hDlg, IDC_PIDS_SCROLL), SB_CTL, v->nPIDScrollOffset, TRUE);
			fRedraw = TRUE;
		}
		break;
	case SB_LINEDOWN:
		if (v->nPIDScrollOffset < v->nActivePIDCount - v->nPIDChartItemCount)
		{
			v->nPIDScrollOffset++;
			SetScrollPos(GetDlgItem(hDlg, IDC_PIDS_SCROLL), SB_CTL, v->nPIDScrollOffset, TRUE);
			fRedraw = TRUE;
		}
		break;
	case SB_THUMBTRACK:
		v->nPIDScrollOffset = HIWORD(wParam);
		SetScrollPos(GetDlgItem(hDlg, IDC_PIDS_SCROLL), SB_CTL, v->nPIDScrollOffset, TRUE);
		fRedraw = TRUE;
		break;
	case SB_PAGEUP:
		{
			v->nPIDScrollOffset -= v->nPIDChartItemCount;
			if (v->nPIDScrollOffset < 0)
				v->nPIDScrollOffset = 0;
			SetScrollPos(GetDlgItem(hDlg, IDC_PIDS_SCROLL), SB_CTL, v->nPIDScrollOffset, TRUE);
			fRedraw = TRUE;
			break;
		}
	case SB_PAGEDOWN:
		{
			v->nPIDScrollOffset += v->nPIDChartItemCount;
			if (v->nPIDScrollOffset > v->nActivePIDCount - v->nPIDChartItemCount)
				v->nPIDScrollOffset = v->nActivePIDCount - v->nPIDChartItemCount;
			SetScrollPos(GetDlgItem(hDlg, IDC_PIDS_SCROLL), SB_CTL, v->nPIDScrollOffset, TRUE);
			fRedraw = TRUE;
			break;
		}
	}

	if (fRedraw == TRUE)
		ForcePIDChartRepaint(hDlg);
}

void FormatIPv6Address(char * szOutput, BYTE * bIPV6Address)
{
	int i;

	szOutput[0] = '\0';
	for (i = 0; i < 16; i += 2)
	{
		char szTemp[32];

		WORD wValue = bIPV6Address[i + 0] << 8 | bIPV6Address[i + 1];
		wsprintf(szTemp, "%X:", wValue);
		lstrcat(szOutput, szTemp);
	}
	szOutput[lstrlen(szOutput) - 1] = '\0';
}

void HandleWMUSER2IPMode(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndTV = GetDlgItem(hDlg, IDC_SI_TREE);
	SendMessage(hWndTV, WM_SETREDRAW, FALSE, 0);
	switch(wParam)
	{
	case SI_PARSER_IP_PID:
		{
			int nIndex = (int)lParam;
			PIPCLICKLPARAM ipclicklParam;
			char szTemp[128];
			char szMask[128];

			wsprintf(szMask, "PID %s", v->szOutputPIDFlags);
			wsprintf(szTemp, szMask, v->ippid[nIndex].nPID);
			ipclicklParam = LocalAlloc(LPTR, sizeof(IPCLICKLPARAM));
			ipclicklParam->dwPtr = nIndex;
			ipclicklParam->nType = SI_PARSER_IP_PID;
			v->ippid[nIndex].hIPPIDRootItem = AddItemToSITree(hWndTV, szTemp, 1, (int)ipclicklParam, 11, NULL, TVI_FIRST);
		}
		break;
	case SI_PARSER_IP_MAC:
		{
			PIPMACENTRY pCurrentMac = (PIPMACENTRY)lParam;
			PIPCLICKLPARAM ipclicklParam;
			char szTemp[128];

			wsprintf(szTemp, "MAC %02x:%02x:%02x:%02x:%02x:%02x",
				     pCurrentMac->bMAC[0], pCurrentMac->bMAC[1], pCurrentMac->bMAC[2],
					 pCurrentMac->bMAC[3], pCurrentMac->bMAC[4], pCurrentMac->bMAC[5]);
			ipclicklParam = LocalAlloc(LPTR, sizeof(IPCLICKLPARAM));
			ipclicklParam->dwPtr = (DWORD)pCurrentMac;
			ipclicklParam->nType = SI_PARSER_IP_MAC;
			pCurrentMac->hIPMacItem = AddItemToSITree(hWndTV, szTemp, 2, (int)ipclicklParam, 11, pCurrentMac->hIPPIDRootItem, NULL);
			TreeView_Expand(hWndTV, pCurrentMac->hIPPIDRootItem, TVE_EXPAND);
		}
		break;
	case SI_PARSER_IP_IP:
		{
			PIPENTRY pCurrentIP = (PIPENTRY)lParam;
			PIPCLICKLPARAM ipclicklParam;
			char szTemp[128];
			char szProtocol[10];

			switch(pCurrentIP->dwProtocol)
			{
			case IP_UDP_ID:
				lstrcpy(szProtocol, "U");
				break;
			case 6:
				lstrcpy(szProtocol, "T");
				break;
			default:
				wsprintf(szProtocol, "%d", pCurrentIP->dwProtocol);
				break;
			}
			if (pCurrentIP->nIPVersion == 4)
			{
				wsprintf(szTemp, "%s:%d.%d.%d.%d",
						 szProtocol,
						 (pCurrentIP->dwDestinationAddress >> 24) & 0xff,
						 (pCurrentIP->dwDestinationAddress >> 16) & 0xff,
						 (pCurrentIP->dwDestinationAddress >> 8) & 0xff,
						 (pCurrentIP->dwDestinationAddress & 0xff));
			}
			else
			{
				FormatIPv6Address(szTemp, pCurrentIP->bDestinationAddressIPv6);
			}
			ipclicklParam = LocalAlloc(LPTR, sizeof(IPCLICKLPARAM));
			ipclicklParam->dwPtr = (DWORD)pCurrentIP;
			ipclicklParam->nType = SI_PARSER_IP_IP;
			pCurrentIP->hIPItem = AddItemToSITree(hWndTV, szTemp, 3, (int)ipclicklParam, 11, pCurrentIP->hIPMacItem, NULL);
			if (v->fAutoExpandIPs)
				TreeView_Expand(hWndTV, pCurrentIP->hIPMacItem, TVE_EXPAND);
		}
		break;
	}
	SendMessage(hWndTV, WM_SETREDRAW, TRUE, 0);
}

#ifdef PRO
void XMLLogCheckItemCount()
{
	if (v->nXMLLogCount == v->nXMLLogMax)
	{
		PXMLLOG pNewLog = LocalAlloc(LPTR, sizeof(XMLLOG) * (v->nXMLLogMax + 1000));
		if (v->XMLLog != NULL)
		{
			memcpy(pNewLog, v->XMLLog, v->nXMLLogMax);
			LocalFree(v->XMLLog);
		}
		v->XMLLog = pNewLog;
		v->nXMLLogMax += 1000;
#ifdef _DEBUG
		{
			char szTemp[128];

			wsprintf(szTemp, "TSReader: XML Log has space for %d items\n", v->nXMLLogMax);
			OutputDebugString(szTemp);
		}
#endif _DEBUG
	}
}

void LogXMLItem(WPARAM wParam, LPARAM lParam)
{
	if (v->fStreamingXMLMode)
	EnterCriticalSection(&v->csXMLLog);
	XMLLogCheckItemCount();
	v->XMLLog[v->nXMLLogCount].nXMLLogType = XML_LOG_TYPE_WMUSER2;
	v->XMLLog[v->nXMLLogCount].wParam = wParam;
	v->XMLLog[v->nXMLLogCount].lParam = lParam;
	v->XMLLog[v->nXMLLogCount].fSent = FALSE;
	v->nXMLLogCount++;
	LeaveCriticalSection(&v->csXMLLog);
}
#endif PRO

void HandleWMUSER2MPEG2Mode(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	HWND hWndTV = GetDlgItem(hDlg, IDC_SI_TREE);
	
	if (wParam != SI_PARSER_TDT)
	{
		v->nTreeUpdateCounter = 0;
		v->nTreeUpdateCounter2 = 0;
	}

	//if (v->fAutoXMLExport == FALSE)
	//	SendMessage(hWndTV, WM_SETREDRAW, FALSE, 0);

	switch(wParam)
	{
	case SI_PARSER_PAT:
		{
			int nPMTIndex;
			char szTemp[128];
			char szMask[128];

			if (v->pat.hPATTreeItem == NULL)
			{
				wsprintf(szMask, "PAT PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0);
				v->pat.hPATTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_PAT, 0, NULL, TVI_FIRST);
			}

			for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
			{
				if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
					break;
				if (v->pat.pmt[nPMTIndex].nPMTPID == -2)
					continue;
				if (v->pat.pmt[nPMTIndex].nProgramNumber)
				{
					wsprintf(szMask, "PMT PID %s - Program %%d", v->szOutputPIDFlags);
					wsprintf(szTemp, szMask, v->pat.pmt[nPMTIndex].nPMTPID, v->pat.pmt[nPMTIndex].nProgramNumber);
				}
				else
				{
					wsprintf(szMask, "PMT PID %s - Network", v->szOutputPIDFlags);
					wsprintf(szTemp, szMask, v->pat.pmt[nPMTIndex].nPMTPID);
				}
				v->pat.pmt[nPMTIndex].hPMTTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_PMT + nPMTIndex, 1, v->pat.hPATTreeItem, NULL);
#ifdef DEBUG_MESSAGES				
				{
					char szTemp2[128];
					wsprintf(szTemp2, "TSReader: PMT for %s item #%d\n", szTemp, nPMTIndex);
					OutputDebugString(szTemp2);
				}
#endif DEBUG_MESSAGES
				if (v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber] != NULL)
				{
					if (v->pat.pmt[nPMTIndex].fSetupSDTName == FALSE)
					{
						int nIcon;

						v->pat.pmt[nPMTIndex].fSetupSDTName = TRUE;
						if (v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->fATSC == FALSE)
						{
							wsprintf(szTemp, "SDT: %s", v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->szShortName);
							if (v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->nFromTable == 0x42)
								nIcon = 7;
							else
								nIcon = 24;
						}
						else
						{
							wsprintf(szTemp, "VCT: %s", v->pChannelData[v->pat.pmt[nPMTIndex].nProgramNumber]->szShortName);
							nIcon = 12;
						}
						AddItemToSITree(hWndTV, szTemp, 3, SI_PARSER_SDT + v->pat.pmt[nPMTIndex].nProgramNumber, nIcon, v->pat.pmt[nPMTIndex].hPMTTreeItem, TVI_FIRST);
						if (v->fAutoExpandPMTs == TRUE)
							TreeView_Expand(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem, TVE_EXPAND);
					}
				}
			}
			TreeView_Expand(hWndTV, v->pat.hPATTreeItem, TVE_EXPAND);
		}
		break;
	case SI_PARSER_PMT:
		{
			int nPMTIndex = (int)lParam;
			int nESIndex;
			int nIcon;
			BOOL fVideoService = FALSE;
			char szTemp[128];
			char szMask[128];

			for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
			{
				if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
					break;

				wsprintf(szMask, "ES PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, v->pat.pmt[nPMTIndex].es[nESIndex].nESPID);
				switch(v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType)
				{
				case 0x01:	// MPEG-1
				case 0x02:	// MPEG-2
				case 0x10:	// MPEG-4
				case 0x1b:	// H264
				case 0xea:	// VC1
					fVideoService = TRUE;
					nIcon = 5;
					break;
				case 0x03:	// MPEG-1
				case 0x04:	// MPEG-2
				case 0x0f:	// MPEG-2 AAC
				case 0x11:	// MPEG-4 AAC
				case 0x81:	// AC3
				case 0x83:	// LPCM
				case 0x85:	// DTS
				case 0xe6:	// WM9
					nIcon = 4;
					break;
				case 0x06:			// perhaps AC3/PCM. Let's see if there's an AC3 descriptor
					if (   IsAC3AudioStream(nPMTIndex, nESIndex) == TRUE
						|| IsPCMAudioStream(nPMTIndex, nESIndex) == TRUE
						|| IsDTSAudioStream(nPMTIndex, nESIndex) == TRUE)
						nIcon = 4;
					else if (IsTeleTextOrVBIStream(nPMTIndex, nESIndex) == TRUE)
						nIcon = 17;
					else if (IsSubtitleStream(nPMTIndex, nESIndex) == TRUE)
						nIcon = 19;
					else
						nIcon = 6;
					break;
				default:
					if (v->nNetworkPID != 0x0010)
					{
						if (v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType == 0x80) // DCII video
						{
							fVideoService = TRUE;
							nIcon = 5;
							break;
						}
					}
					if (IsDataBroadcastStream(nPMTIndex, nESIndex) == TRUE)
					{
						nIcon = 18;
						break;
					}
					nIcon = 6;
					break;
				}
				v->pat.pmt[nPMTIndex].es[nESIndex].hESTreeItem = AddItemToSITree(hWndTV, szTemp, 3, SI_PARSER_ES + v->pat.pmt[nPMTIndex].es[nESIndex].nESPID, nIcon, v->pat.pmt[nPMTIndex].hPMTTreeItem, NULL);
				if (v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors != NULL)
				{
					int nDescriptorsLength = v->pat.pmt[nPMTIndex].es[nESIndex].nDescriptorsLength;
					int nCurrentIndex = 0;

					do
					{
						char szDescriptor[128];

						DecodeDescriptorNames(szDescriptor, v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors[nCurrentIndex]);
						AddItemToSITree(hWndTV, szDescriptor, 4, SI_PARSER_NOP, 8, v->pat.pmt[nPMTIndex].es[nESIndex].hESTreeItem, NULL);
						nCurrentIndex += (BYTE)v->pat.pmt[nPMTIndex].es[nESIndex].pDescriptors[nCurrentIndex + 1]; // descriptor length
						nCurrentIndex += 2;	// descriptor tag and length
					} while (nCurrentIndex < nDescriptorsLength);
				}
			}
			if ( (v->pat.pmt[nPMTIndex].nPCRPID > 0) && (v->pat.pmt[nPMTIndex].nPCRPID < 0x1fff) )
			{
				if (fVideoService == TRUE || v->fShowNonVideoPCR)
				{							
					wsprintf(szMask, "PCR PID %s", v->szOutputPIDFlags);
					wsprintf(szTemp, szMask, v->pat.pmt[nPMTIndex].nPCRPID);
					v->pat.pmt[nPMTIndex].hPCRTreeItem = AddItemToSITree(hWndTV, szTemp, 3, SI_PARSER_NOP, 3, v->pat.pmt[nPMTIndex].hPMTTreeItem, NULL);
				}
			}
			if (v->fAutoExpandPMTs == TRUE)
				TreeView_Expand(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem, TVE_EXPAND);
			if (v->pat.pmt[nPMTIndex].fPostTreeAddSelect)
			{
				v->pat.pmt[nPMTIndex].fPostTreeAddSelect = FALSE;
				TreeView_SelectItem(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
			}
		}
		break;
	case SI_PARSER_NIT:
		{
			int nTransportStreamID = (int)lParam;
			int nIcon = 9;
			int i;
			char szTemp[128], szMask[128], szPolarity[16], szEW[2];
			char szOrbitalPosition[6];

			if (v->hNITRootTreeItem == NULL)
			{
				wsprintf(szMask, "NIT PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0x0010);
				v->hNITRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_NIT + 0x0fffffff, 9, NULL, NULL);
			}
			szTemp[0] = '\0';
			switch(v->pNITData[nTransportStreamID]->nType)
			{
			case NIT_DVBS:
				FormatPolarity(szPolarity, v->pNITData[nTransportStreamID]->dvbs.nPolarization, FALSE);

				if (v->pNITData[nTransportStreamID]->dvbs.fEastern == TRUE)
					lstrcpy(szEW, "E");
				else
					lstrcpy(szEW, "W");

				sprintf(szOrbitalPosition, "%3.1f", (double)v->pNITData[nTransportStreamID]->dvbs.nOrbitalPosition / 10.0);
				while (lstrlen(szOrbitalPosition) < 5)
				{
					lstrcpy(szTemp, "0");
					lstrcat(szTemp, szOrbitalPosition);
					lstrcpy(szOrbitalPosition, szTemp);
				};
				sprintf(szTemp, "%s%s %.3f %s",
						szOrbitalPosition,
						szEW,
						(double)v->pNITData[nTransportStreamID]->nFrequency / 100000.0,
						szPolarity);
				break;
			case NIT_DVBT:
				sprintf(szTemp, "%.1f MHz", (double)v->pNITData[nTransportStreamID]->nFrequency / 100000.0);
				break;
			case NIT_DVBC:
				sprintf(szTemp, "%.1f MHz", (double)v->pNITData[nTransportStreamID]->nFrequency / 10000.0);
				break;
			}
			if (v->pNITData[nTransportStreamID]->fThisTS != TRUE)
				nIcon = 25;
			for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
			{
				if (v->nIgnoredNetworks[i] == 0)
					break;
				if (v->nIgnoredNetworks[i] == v->pNITData[nTransportStreamID]->nNetworkID)
				{
					nIcon = 27;
					break;
				}
			}
			v->pNITData[nTransportStreamID]->hNITTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_NIT + nTransportStreamID, nIcon, v->hNITRootTreeItem, TVI_SORT);
			v->nNITTreeItemCount++;
			{
				TVITEM tvi;

				wsprintf(szMask, "NIT PID %s <%%d>", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0x0010, v->nNITTreeItemCount);
				memset(&tvi, 0, sizeof(tvi));
				tvi.hItem = v->hNITRootTreeItem;
				tvi.mask = TVIF_TEXT;
				tvi.pszText = szTemp;
				TreeView_SetItem(hWndTV, &tvi);
			}
		}
		break;
	case SI_PARSER_SDT:
	case SI_PARSER_VCT:
		{
			int nChannelNumber = (int)lParam;
			int i;
			int nIcon;
			char szTemp[256], szMask[256];
			
			if (v->nNetworkPID == 0x0ffe)
				break;

			if (v->hWndEPGGrid != NULL && v->fEPGDisplayActive && v->fEPGUpdateRealtime)
				PostMessage(v->hWndEPGGrid, WM_USER + 2, wParam, lParam);

			if (wParam == SI_PARSER_SDT)
				nIcon = 7;
			else
				nIcon = 12;

			if (v->hSDTRootTreeItem == NULL)
			{
				if (wParam == SI_PARSER_SDT)
				{
					wsprintf(szMask, "SDT PID %s", v->szOutputPIDFlags);
					wsprintf(szTemp, szMask, 0x0011);
				}
				else
				{
					wsprintf(szMask, "TVCT PID %s", v->szOutputPIDFlags);
					wsprintf(szTemp, szMask, 0x1ffb);
				}
				v->hSDTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_SDT | 0x0fffffff, nIcon, NULL, NULL);
			}
			wsprintf(szTemp, "%d %s", v->pChannelData[nChannelNumber]->nChannelNumber, v->pChannelData[nChannelNumber]->szShortName);

			if (wParam == SI_PARSER_SDT)
			{
				if (v->pChannelData[nChannelNumber]->nFromTable == 0x42)
					nIcon = 7;
				else
					nIcon = 24;
			}
			if (nChannelNumber < v->nMinimumSDTChannel)
			{
				v->nMinimumSDTChannel = nChannelNumber;
				v->pChannelData[nChannelNumber]->hSDTTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_SDT + nChannelNumber, nIcon, v->hSDTRootTreeItem, TVI_FIRST);
				v->nSDTTreeItemCount++;
			}
			else
			{
				HTREEITEM hInsertAfter;

				for (i = 0; i < MAX_EIT_CHANNEL_DATA; i++)
				{
					if (v->pChannelData[i] != NULL)
					{
						if (v->pChannelData[i]->hSDTTreeItem != NULL)
						{
							if (v->pChannelData[i]->nChannelNumber < nChannelNumber)
							{
								hInsertAfter = v->pChannelData[i]->hSDTTreeItem;
							}
						}
					}
				}
				v->pChannelData[nChannelNumber]->hSDTTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_SDT + nChannelNumber, nIcon, v->hSDTRootTreeItem, hInsertAfter);
				v->nSDTTreeItemCount++;
			}
			if (wParam == SI_PARSER_SDT)
			{
				TVITEM tvi;

				wsprintf(szMask, "SDT PID %s <%%d>", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0x0011, v->nSDTTreeItemCount);
				memset(&tvi, 0, sizeof(tvi));
				tvi.hItem = v->hSDTRootTreeItem;
				tvi.mask = TVIF_TEXT;
				tvi.pszText = szTemp;
				TreeView_SetItem(hWndTV, &tvi);
			}

			// See if we have any names for the PAT entries
			for (i = 0; i < MAX_PAT_ENTRIES; i++)
			{
				if (v->pat.pmt[i].nPMTPID == 0)
					break;
				if (v->pat.pmt[i].fSetupSDTName == FALSE)
				{
					int nProgramNumber;

					nProgramNumber = v->pat.pmt[i].nProgramNumber;
					if (v->pChannelData[nProgramNumber] != NULL && v->pat.pmt[i].hPMTTreeItem != NULL)
					{
						int nIcon;

						v->pat.pmt[i].fSetupSDTName = TRUE;
						if (wParam == SI_PARSER_SDT)
						{
							wsprintf(szTemp, "SDT: %s", v->pChannelData[nProgramNumber]->szShortName);
							if (v->pChannelData[nProgramNumber]->nFromTable == 0x42)
								nIcon = 7;
							else
								nIcon = 24;
						}
						else
						{
							wsprintf(szTemp, "VCT: %s", v->pChannelData[nProgramNumber]->szShortName);
							nIcon = 12;
						}
						AddItemToSITree(hWndTV, szTemp, 3, SI_PARSER_SDT + nProgramNumber, nIcon, v->pat.pmt[i].hPMTTreeItem, TVI_FIRST);
						v->nSDTTreeItemCount++;
						break;
					}
				}
			}

			// See if we have any names for the EIT entries
			for (i = 0; i < MAX_EIT_CHANNEL_DATA; i++)
			{
				if (v->pChannelData[i] != NULL)
				{
					if (v->pChannelData[i]->fSetupEITName == FALSE)
					{
						if (v->hEITTreeItem[i] != NULL)
						{
							if (lstrlen(v->pChannelData[i]->szShortName) > 0)
							{
								TVITEM tvi;

								wsprintf(szTemp, "%d %s", v->pChannelData[i]->nChannelNumber, v->pChannelData[i]->szShortName);
								memset(&tvi, 0, sizeof(tvi));
								tvi.hItem = v->hEITTreeItem[i];
								tvi.mask = TVIF_TEXT;
								tvi.pszText = szTemp;
								TreeView_SetItem(hWndTV, &tvi);
								v->pChannelData[i]->fSetupEITName = TRUE;
							}
						}
					}
				}
			}
		}
		break;
	case SI_PARSER_EIT:
		{
			int nChannelNumber = (int)lParam;
			int i;
			char szTemp[128], szMask[128];

			if (v->hEITRootTreeItem == NULL)
			{
				if (v->nNetworkPID == 0x0010)
				{
					wsprintf(szMask, "EIT PID %s", v->szOutputPIDFlags);
					wsprintf(szTemp, szMask, v->nEITPID);
				}
				else
					lstrcpy(szTemp, "EIT/ETT");
				v->hEITRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_EIT | 0x0fffffff, 2, NULL, NULL);
			}
			if (v->hEITTreeItem[nChannelNumber] != NULL)
				break;

			wsprintf(szTemp, "%d", nChannelNumber);
			if (v->pChannelData[nChannelNumber] != NULL)
			{
				if (v->pChannelData[nChannelNumber]->fSetupEITName == FALSE)
				{
					if ((v->pChannelData[nChannelNumber]->szShortName) > 0)
					{
						char szTemp2[50];

						v->pChannelData[nChannelNumber]->fSetupEITName = TRUE;
						wsprintf(szTemp2, " %s", v->pChannelData[nChannelNumber]->szShortName);
						lstrcat(szTemp, szTemp2);
					}
				}
			}

			for (i = nChannelNumber - 1; i > 0; i--)
			{
				if (v->hEITTreeItem[i] != NULL)
				{
					v->hEITTreeItem[nChannelNumber] = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_EIT + nChannelNumber, 2, v->hEITRootTreeItem, v->hEITTreeItem[i]);
					break;
				}
			}
			if (i == 0)
				v->hEITTreeItem[nChannelNumber] = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_EIT + nChannelNumber, 2, v->hEITRootTreeItem, TVI_FIRST);
			v->nEITTreeItemCount++;
			if (v->nNetworkPID == 0x0010)
			{
				TVITEM tvi;

				wsprintf(szMask, "EIT PID %s <%%d>", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, v->nEITPID, v->nEITTreeItemCount);
				memset(&tvi, 0, sizeof(tvi));
				tvi.hItem = v->hEITRootTreeItem;
				tvi.mask = TVIF_TEXT;
				tvi.pszText = szTemp;
				TreeView_SetItem(hWndTV, &tvi);
			}
		}
		break;
	case SI_PARSER_CAT:
		{
			int i;
			char szTemp[128], szMask[128];

			wsprintf(szMask, "CAT PID %s", v->szOutputPIDFlags);
			wsprintf(szTemp, szMask, 0x0001);
			v->cat.hCATTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_CAT, 10, NULL, v->pat.hPATTreeItem);

			for (i = 0; i < MAX_CAT_DESCRIPTORS; i++)
			{
				char szDescriptor[128];
				char szCAName[128] = {0};

				if (v->cat.pDescriptor[i] == NULL)
					break;

				DecodeDescriptorNames(szDescriptor, v->cat.pDescriptor[i][0]);
				set_buf(BM_USER_THREAD, v->cat.pDescriptor[i], 0, FALSE);
				{
					int descriptor_tag = get_bits(BM_USER_THREAD, 8);
					int descriptor_length = get_bits(BM_USER_THREAD, 8);
					int CA_system_ID = get_bits(BM_USER_THREAD, 16);
					FormatCASystemName(CA_system_ID, szCAName);
				}
				if (lstrlen(szCAName))
				{
					lstrcat(szDescriptor, " - ");
					lstrcat(szDescriptor, szCAName);
				}
				AddItemToSITree(hWndTV, szDescriptor, 2, SI_PARSER_NOP, 8, v->cat.hCATTreeItem, NULL);
			}
			TreeView_Expand(hWndTV, v->cat.hCATTreeItem, TVE_EXPAND);
		}
		break;
	case SI_PARSER_MMT:
		{
			int i;
			char szTemp[128], szMask[128];

			wsprintf(szMask, "MMT PID %s", v->szOutputPIDFlags);
			wsprintf(szTemp, szMask, v->nNetworkPID);
			v->hMMTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_NOP, 16, NULL, v->pat.hPATTreeItem);

			for (i = 0; i < v->nMaxMMT; i++)
			{
				char szFEC[12];
				int nDisplaySR = v->mmt[i].symbol_rate / 1000;

				DecodeFEC(v->mmt[i].inner_coding_mode, szFEC, TRUE);
				wsprintf(szTemp, "%d %s", nDisplaySR, szFEC);
				AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_MMT + i, 16, v->hMMTRootTreeItem, NULL);
			}
		}
		break;
	case SI_PARSER_CDT:		// also the RRT
		if (v->nNetworkPID == 0x1ffb)
		{
			int nRRTRegion = (int)lParam;
			char szTemp[128], szMask[128];

			if (v->hRRTRootTreeItem == NULL)
			{
				wsprintf(szMask, "RRT PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, v->nNetworkPID);
				v->hRRTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_NOP, 15, NULL, v->pat.hPATTreeItem);
			}
			wsprintf(szTemp, "Region %d", nRRTRegion);
			AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_RRT + nRRTRegion, 15, v->hRRTRootTreeItem, NULL);
		}
		else
		{
			int i;
			char szTemp[128], szMask[128];

			wsprintf(szMask, "CDT PID %s", v->szOutputPIDFlags);
			wsprintf(szTemp, szMask, v->nNetworkPID);
			v->hCDTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_NOP, 15, NULL, v->pat.hPATTreeItem);

			for (i = 0; i < v->nMaxCDT; i++)
			{
				double dFirstCarrier = (double)v->cdt[i].first_carrier_frequency;
				double dMultiplier;

				if (v->cdt[i].frequency_unit)
					dMultiplier = 125.0;
				else
					dMultiplier = 10.0;

				sprintf(szTemp, "%.3f MHz", (dFirstCarrier * dMultiplier) / 1000.0);
				AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_CDT + i, 15, v->hCDTRootTreeItem, NULL);
			}
		}
		break;
	case SI_PARSER_SIT:
		{
			int nSITIndex = (int)lParam;
			char szTemp[128], szMask[128];

			if (v->hSITRootTreeItem == NULL)
			{
				wsprintf(szMask, "SIT PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, v->nNetworkPID);
				v->hSITRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_NOP, 14, NULL, v->pat.hPATTreeItem);
			}

			sprintf(szTemp, "%d @ %.3f deg ", v->sit[nSITIndex].satellite_ID, (double)v->sit[nSITIndex].orbital_position / 10.0);
			if (!v->sit[nSITIndex].hemisphere)
				lstrcat(szTemp, "W");
			else
				lstrcat(szTemp, "E");
			AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_SIT + nSITIndex, 14, v->hSITRootTreeItem, NULL);
		}
		break;
	case SI_PARSER_TDT:
		{
			int nTDTIndex = (int)lParam;
			char szTemp[128], szMask[128];

			switch(v->nNetworkPID)
			{
			case 0x0010:
				if (lParam == 0)
				{
					// DVB Time and Date table
					wsprintf(szTemp, "%04d/%02d/%02d %02d:%02d:%02d",
							 v->dvbtdt.nYear, v->dvbtdt.nMonth, v->dvbtdt.nDay,
							 v->dvbtdt.nHour, v->dvbtdt.nMinute, v->dvbtdt.nSecond);
					if (v->dvbtdt.hRootTreeItem == NULL)
					{
						char szTemp2[128];
						wsprintf(szMask, "TDT PID %s", v->szOutputPIDFlags);
						wsprintf(szTemp2, szMask, 0x0014);
						v->dvbtdt.hRootTreeItem = AddItemToSITree(hWndTV, szTemp2, 1, SI_PARSER_NOP, 13, NULL, v->pat.hPATTreeItem);
						v->dvbtdt.hTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_TDT, 13, v->dvbtdt.hRootTreeItem, NULL);
						TreeView_Expand(hWndTV, v->dvbtdt.hRootTreeItem, TVE_EXPAND);
					}
					else
					{
						TVITEM tvi;

						memset(&tvi, 0, sizeof(tvi));
						tvi.hItem = v->dvbtdt.hTreeItem;
						tvi.mask = TVIF_TEXT;
						tvi.pszText = szTemp;
						TreeView_SetItem(hWndTV, &tvi);
					}
				}
				else
				{
					wsprintf(szTemp, "%04d/%02d/%02d %02d:%02d:%02d",
							 v->dvbtot.nYear, v->dvbtot.nMonth, v->dvbtot.nDay,
							 v->dvbtot.nHour, v->dvbtot.nMinute, v->dvbtot.nSecond);
					if (v->dvbtot.hRootTreeItem == NULL)
					{
						char szTemp2[128];
						wsprintf(szMask, "TOT PID %s", v->szOutputPIDFlags);
						wsprintf(szTemp2, szMask, 0x0014);
						v->dvbtot.hRootTreeItem = AddItemToSITree(hWndTV, szTemp2, 1, SI_PARSER_NOP, 13, NULL, v->pat.hPATTreeItem);
						v->dvbtot.hTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_TDT + 1, 13, v->dvbtot.hRootTreeItem, NULL);
						TreeView_Expand(hWndTV, v->dvbtot.hRootTreeItem, TVE_EXPAND);
					}
					else
					{
						TVITEM tvi;

						memset(&tvi, 0, sizeof(tvi));
						tvi.hItem = v->dvbtot.hTreeItem;
						tvi.mask = TVIF_TEXT;
						tvi.pszText = szTemp;
						TreeView_SetItem(hWndTV, &tvi);
					}
				}
				break;
			case 0x1ffb:
				{
					// ATSC STT
					wsprintf(szTemp, "%04d/%02d/%02d %02d:%02d:%02d",
							 v->dvbtdt.nYear, v->dvbtdt.nMonth, v->dvbtdt.nDay,
							 v->dvbtdt.nHour, v->dvbtdt.nMinute, v->dvbtdt.nSecond);
					if (v->dvbtdt.hRootTreeItem == NULL)
					{
						char szTemp2[128];
						wsprintf(szMask, "STT PID %s", v->szOutputPIDFlags);
						wsprintf(szTemp2, szMask, 0x1ffb);
						v->dvbtdt.hRootTreeItem = AddItemToSITree(hWndTV, szTemp2, 1, SI_PARSER_NOP, 13, NULL, v->pat.hPATTreeItem);
						v->dvbtdt.hTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_TDT, 13, v->dvbtdt.hRootTreeItem, NULL);
						TreeView_Expand(hWndTV, v->dvbtdt.hRootTreeItem, TVE_EXPAND);
					}
					else
					{
						TVITEM tvi;

						memset(&tvi, 0, sizeof(tvi));
						tvi.hItem = v->dvbtdt.hTreeItem;
						tvi.mask = TVIF_TEXT;
						tvi.pszText = szTemp;
						TreeView_SetItem(hWndTV, &tvi);
					}
				}
				break;
			default:
				{
					// DCII Transponder Definition Table
					if (v->hTDTRootTreeItem == NULL)
					{
						wsprintf(szMask, "TDT PID %s", v->szOutputPIDFlags);
						wsprintf(szTemp, szMask, v->nNetworkPID);
						v->hTDTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_NOP, 13, NULL, v->pat.hPATTreeItem);
					}
					wsprintf(szTemp, "Sat. %d txp. %d", v->tdt[nTDTIndex].satellite_ID, v->tdt[nTDTIndex].transponder_number);
					AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_TDT + nTDTIndex, 13, v->hTDTRootTreeItem, NULL);
				}
				break;
			}
		}
		break;
	case SI_PARSER_MGT:
		{
			char szTemp[128], szMask[128];

			wsprintf(szMask, "MGT PID %s", v->szOutputPIDFlags);
			wsprintf(szTemp, szMask, 0x1ffb);
			v->hMGTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_MGT, 20, NULL, v->pat.hPATTreeItem);
		}
		break;
	case SI_PARSER_CVCT:
		{
			int nCVCTIndex = (int)lParam;
			char szTemp[128], szMask[128];

			if (v->hCVCTRootTreeItem == NULL)
			{
				wsprintf(szMask, "CVCT PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0x1ffb);
				v->hCVCTRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_CVCT + MAX_CVCT_ENTRIES + 1, 12, NULL, v->pat.hPATTreeItem);
			}
			wsprintf(szMask, "CVCT TSID %s", v->szOutputPIDFlags);
			wsprintf(szTemp, szMask, v->cvct[nCVCTIndex].transport_stream_id);
			AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_CVCT + nCVCTIndex, 12, v->hCVCTRootTreeItem, NULL);
		}
		break;
	case SI_PARSER_BAT:
		{
			int nIcon = 23;
			int nBATIndex = (int)lParam;
			char szTemp[128], szMask[128];

			if (nBATIndex == -1)
			{
				SendMessage(hWndTV, TVM_DELETEITEM, 0, (LPARAM)v->hBATRootTreeItem);
				v->hBATRootTreeItem = NULL;
				break;
			}

			if (v->hBATRootTreeItem == NULL)
			{
				wsprintf(szMask, "BAT PID %s", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0x0011);
				v->hBATRootTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_BAT + MAX_BAT_ENTRIES, 23, NULL, v->pat.hPATTreeItem);
			}
			wsprintf(szMask, "BAT %s (v%d)", v->szOutputPIDFlags, v->bat[nBATIndex].version_number);
			wsprintf(szTemp, szMask, v->bat[nBATIndex].bouquet_id);
#ifndef LITE
			if (v->bat[nBATIndex].bouquet_id == v->nCurrentBATID)
			{
				nIcon = 26;
			}
#endif LITE
			v->bat[nBATIndex].hTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_BAT + nBATIndex, nIcon, v->hBATRootTreeItem, NULL);
			v->nBATTreeItemCount++;
			{
				TVITEM tvi;

				wsprintf(szMask, "BAT PID %s <%%d>", v->szOutputPIDFlags);
				wsprintf(szTemp, szMask, 0x0011, v->nBATTreeItemCount);
				memset(&tvi, 0, sizeof(tvi));
				tvi.hItem = v->hBATRootTreeItem;
				tvi.mask = TVIF_TEXT;
				tvi.pszText = szTemp;
				TreeView_SetItem(hWndTV, &tvi);
			}
		}
	}
	
#ifdef PRO
	if (v->fStreamingXMLMode)
		LogXMLItem(wParam, lParam);
#endif PRO

}

void UpdateRecordPIDsOneFileOptions(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, IDC_RECORD_PID_NO_HEADER), !v->fRecordPIDsOneFile);
	EnableWindow(GetDlgItem(hDlg, IDC_RECORD_PID_INCLUDE_PCR), v->fRecordPIDsOneFile);
	if (v->fRecordPIDsOneFile)
		SetDlgItemText(hDlg, IDC_OUTPUT_FOLDER_CAPTION, "Output Filename");
	else
		SetDlgItemText(hDlg, IDC_OUTPUT_FOLDER_CAPTION, "Output Folder");
}

BOOL CALLBACK IPDVBPIDSelectDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int i;
			BOOL fAutoRecordPIDMode = FALSE;
			char szPID[32];

			for (i = 0; i < 8192; i++)
			{
				if (v->pc[i].lnPackets)
				{
					if (v->pc[i].nPID != 0x1fff)
					{
						int nIndex;
						int j = 0;

						wsprintf(szPID, "%s", FormatTooltipPID(v->pc[i].nPID));			
						nIndex = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_ADDSTRING, 0, (LPARAM)szPID);
#ifndef LITE
						for (j = 0; j < MAX_RECORD_BUFFERS; j++)
						{
							if (v->nAutoRecordPIDsPID[j] == v->pc[i].nPID)
							{
								fAutoRecordPIDMode = TRUE;
								SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_SETSEL, TRUE, nIndex);
							}
						}
#endif LITE
					}
				}
			}

#ifndef LITE
			if (fAutoRecordPIDMode == TRUE)
			{
				lstrcpy(v->szRecordPIDFolder, v->szAutoRecordPIDsFile);
				v->fRecordPIDNoTSHeader = v->fRecordPIDsOneFile = v->fRecordPIDsAppend = v->fRecordPIDIncludePCR = FALSE;
				if (v->nAutoRecordPIDsOptions & 1)
					v->fRecordPIDNoTSHeader = TRUE;
				if (v->nAutoRecordPIDsOptions & 2)
					v->fRecordPIDsOneFile = TRUE;
				if (v->nAutoRecordPIDsOptions & 4)
					v->fRecordPIDsAppend = TRUE;
				if (v->nAutoRecordPIDsOptions & 8)
					v->fRecordPIDIncludePCR = TRUE;
			}
#endif LITE

			if (v->fRecordPIDMode == TRUE)
			{
				SetDlgItemText(hDlg, IDC_RECORD_PID_FOLDER, v->szRecordPIDFolder);
				CheckDlgButton(hDlg, IDC_RECORD_PID_NO_HEADER, v->fRecordPIDNoTSHeader);
				CheckDlgButton(hDlg, IDC_RECORD_PID_ONE_FILE, v->fRecordPIDsOneFile);
				CheckDlgButton(hDlg, IDC_RECORD_PID_APPEND, v->fRecordPIDsAppend);
#ifndef LITE
				CheckDlgButton(hDlg, IDC_RECORD_PID_INCLUDE_PCR, v->fRecordPIDIncludePCR);
#endif LITE
				UpdateRecordPIDsOneFileOptions(hDlg);
			}
			SetFocus(GetDlgItem(hDlg, IDOK));			
#ifndef LITE
			if (fAutoRecordPIDMode)
			{				
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
				SetTimer(v->hDlgSIParser, 5, v->nAutoRecordPIDsDuration * 1000, NULL);
			}
				
#endif LITE
		}
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case LBN_DBLCLK:
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_RECORD_AUDIO_ES))
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
			break;
		case BN_CLICKED:			
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					int nItemList[8192];	// in theory, this is possible!
					int nRetVal;
					BOOL fPIDClash = FALSE;

					v->nRecordPIDsPCRPID = 0x1ffe;
					nRetVal = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETSELITEMS, (WPARAM)sizeof(nItemList) / sizeof(int), (LPARAM)&nItemList[0]);
					if (nRetVal)
					{
						int i;

#ifdef LITE
						if (nRetVal > 1)
						{
							nRetVal = 1;
							if (v->fRecordPIDMode == FALSE)
								MessageBox(hDlg, "This version of TSReader is limited to processing IP/DVB traffic from only one PID at a time.\nThe first PID selected has been used.", gszAppName, MB_OK | MB_ICONINFORMATION);
							else
								MessageBox(hDlg, "This version of TSReader is limited to recording from one PID at a time.\nThe first PID selected has been used.", gszAppName, MB_OK | MB_ICONINFORMATION);
						}
#else LITE
						if (nRetVal > tv->nActualMaxRecordBuffers)
						{
							char szTemp[256];

							nRetVal = tv->nActualMaxRecordBuffers;
							if (v->fRecordPIDMode == FALSE)
								wsprintf(szTemp, "This application is currently limited to processing IP traffic from\n%d PIDs simultaneously. Please contact us if you feel this\nrestriction should be lifted.\n\nOnly the first %d selected PIDs will be processed", tv->nActualMaxRecordBuffers, tv->nActualMaxRecordBuffers);
							else
								wsprintf(szTemp, "This application is currently limited to recording traffic from\n%d PIDs simultaneously. Please contact us if you feel this\nrestriction should be lifted.\n\nOnly the first %d selected PIDs will be processed", tv->nActualMaxRecordBuffers, tv->nActualMaxRecordBuffers);
							MessageBox(hDlg, szTemp, gszAppName, MB_OK | MB_ICONINFORMATION);
						}
#endif LITE

						if (v->fRecordPIDMode == TRUE)
						{
							GetDlgItemText(hDlg, IDC_RECORD_PID_FOLDER, v->szRecordPIDFolder, sizeof(v->szRecordPIDFolder));
							v->fRecordPIDNoTSHeader = IsDlgButtonChecked(hDlg, IDC_RECORD_PID_NO_HEADER);
							//v->fRecordPIDsOneFile is already setup
						}

						memset(&v->nIPMonitorPID[0], -1, sizeof(v->nIPMonitorPID[0]) * 8192);
#ifdef LITE
						nRetVal = 1;
#endif LITE
						for (i = 0; i < nRetVal; i++)
						{
							int nPID;
							char szPID[32];
							
							SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETTEXT, (WPARAM)nItemList[i], (LPARAM)szPID);
							if (v->fDecimalPIDs == FALSE)
								sscanf(&szPID[2], "%x", &nPID);
							else
								sscanf(szPID, "%d", &nPID);
							v->nIPMonitorPID[nPID] = i;
							if (nPID == v->nRecordPIDsPCRPID)
								fPIDClash = TRUE;
#ifdef LITE
							break;
#endif LITE
						}
						if (fPIDClash == TRUE && v->fRecordPIDIncludePCR && v->fRecordPIDsOneFile)
							MessageBox(hDlg, "Warning - PID 0x1ffe is already used in this mux and this PID\nis used for the PCR generation option which you've enabled.\n\nThe PCR generation option should be turned off for this mux.", gszAppName, MB_ICONWARNING);
						EndDialog(hDlg, TRUE);
					}
				}
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			case IDC_RECORD_PID_ONE_FILE:
				v->fRecordPIDsOneFile = IsDlgButtonChecked(hDlg, IDC_RECORD_PID_ONE_FILE);
				UpdateRecordPIDsOneFileOptions(hDlg);
				break;
			case IDC_RECORD_PID_APPEND:
				v->fRecordPIDsAppend = IsDlgButtonChecked(hDlg, IDC_RECORD_PID_APPEND);
				break;
#ifndef LITE
			case IDC_RECORD_PID_INCLUDE_PCR:
				v->fRecordPIDIncludePCR = IsDlgButtonChecked(hDlg, IDC_RECORD_PID_INCLUDE_PCR);
				break;
			case IDC_RECORD_PID_LOAD:
				LoadPIDListDialog(hDlg);
				break;
			case IDC_RECORD_PID_SAVE:
				SavePIDListDialog(hDlg);
				break;
#else LITE
			case IDC_RECORD_PID_INCLUDE_PCR:
			case IDC_RECORD_PID_LOAD:
			case IDC_RECORD_PID_SAVE:
				MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
				break;
#endif LITE
			case IDC_RECORD_PID_BROWSE:
				if (v->fRecordPIDsOneFile == TRUE)
					return SendMessage(hDlg, WM_COMMAND, IDC_RECORD_BROWSE, 0);
				{
					LPITEMIDLIST ItemID;
					BROWSEINFO BrowsingInfo;
					char DirPath[MAX_PATH];
					char FolderName[MAX_PATH];

					lstrcpy(FolderName, v->szRecordPIDFolder);
					memset(&BrowsingInfo, 0, sizeof(BROWSEINFO));
					memset(DirPath, 0, MAX_PATH);

					BrowsingInfo.hwndOwner      = hDlg;
					BrowsingInfo.pszDisplayName = FolderName;
					BrowsingInfo.lpszTitle      = "Select output folder for recorded PID file(s)";
					BrowsingInfo.ulFlags = BIF_NEWDIALOGSTYLE;					
					ItemID = SHBrowseForFolder(&BrowsingInfo);
					if (ItemID)
					{
						SHGetPathFromIDList(ItemID, DirPath);
						SetDlgItemText(hDlg, IDC_RECORD_PID_FOLDER, DirPath);
					} 
				}
				break;
			case IDC_RECORD_BROWSE:
				{
					OPENFILENAME ofn;

					memset( &(ofn), 0, sizeof(ofn));
					ofn.lStructSize	= sizeof(ofn);
					ofn.hwndOwner = hDlg;
					ofn.lpstrFile = v->szRecordPIDFolder;
					ofn.nMaxFile = MAX_PATH;
					ofn.lpstrFilter = TEXT("MPEG Files (*.mpg)\0*.mpg\0All Files (*.*)\0*.*\0\0");	
					ofn.lpstrTitle = TEXT("Record PID(s)");
					ofn.lpstrDefExt = TEXT("mpg");
					ofn.lpstrInitialDir = v->ss.szTransportStreamInitialDir;
					ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
					ofn.hInstance = v->hInstance;						
					if (myGetSaveFileName(&ofn) == TRUE)
						SetDlgItemText(hDlg, IDC_RECORD_PID_FOLDER, v->szRecordPIDFolder);
				}
				break;
			}
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int nMaxPIDs = 0;
			DWORD dwCapabilities = 0;
			BOOL fCanBeStopped = FALSE;
			char szTemp[128];
			char szSourceDescription[MAX_PATH] = {0};
			char szCommandLineParameters[128] = {0};
			char szCapabilities[1024] = {0};

			wsprintf(szTemp, "%s version %s", gszAppName, GetTSRVersion(NULL));
			SetDlgItemText(hDlg, IDC_VERSION, szTemp);

			wsprintf(szTemp, "Built %s %s", __DATE__, __TIME__);
#ifdef BETA
			{
				char szTemp2[64];
				wsprintf(szTemp2, " Expires %02d/%04d", BETA_EXPIRE_MONTH, BETA_EXPIRE_YEAR);
				lstrcat(szTemp, szTemp2);
			}
#endif BETA
			SetDlgItemText(hDlg, IDC_BUILT, szTemp);

			if (GetDescription != NULL)
			{
				GetDescription(szSourceDescription,
							   szCommandLineParameters,
							   &fCanBeStopped,
							   &nMaxPIDs,
							   &dwCapabilities);
				SetDlgItemText(hDlg, IDC_CURRENT_SOURCE, szSourceDescription);
				SetDlgItemText(hDlg, IDC_SOURCE_PARAMETERS, szCommandLineParameters);
				SetDlgItemInt(hDlg, IDC_SOURCE_MAX_PIDS, nMaxPIDs, FALSE);
				if (fCanBeStopped)
					SetDlgItemText(hDlg, IDC_SOURCE_STOPABLE, "True");
				else
					SetDlgItemText(hDlg, IDC_SOURCE_STOPABLE, "False");

				if (dwCapabilities & CAPABILITIES_DISEQC)
					lstrcat(szCapabilities, "DiSEqC switch, ");
				if (dwCapabilities & CAPABILITIES_TONEBURST)
					lstrcat(szCapabilities, "ToneBurst switch, ");
				if (dwCapabilities & CAPABILITIES_POWER)
					lstrcat(szCapabilities, "Supplies power, ");
				if (dwCapabilities & CAPABILITIES_SERIAL_CONTROL)
					lstrcat(szCapabilities, "Serial receiver control, ");
				if (dwCapabilities & CAPABILITIES_UNIPROCESSOR)
					lstrcat(szCapabilities, "Uniprocessor, ");
				if (dwCapabilities & CAPABILITIES_DISEQC_POSITIONER)
					lstrcat(szCapabilities, "DiSEqC 1.2 positioner, ");
				if (dwCapabilities & CAPABILITIES_DISH_SWITCH)
					lstrcat(szCapabilities, "Dish Network switch, ");					
				if (dwCapabilities & CAPABILITIES_TIMESTAMP)
					lstrcat(szCapabilities, "Can timestamp packets, ");			
				if (dwCapabilities & CAPABILITIES_MULTICARD)
					lstrcat(szCapabilities, "Multi-card support, ");
				if (dwCapabilities & CAPABILITIES_CI_CAM)
					lstrcat(szCapabilities, "CI-CAM support, ");
				if (dwCapabilities & CAPABILITIES_TUNE_BY_CHANNEL)
					lstrcat(szCapabilities, "Tunes by channel, ");
				if (dwCapabilities & CAPABILITIES_ACTIVE_ANTENNA)
					lstrcat(szCapabilities, "Active antenna power, ");

				if (lstrlen(szCapabilities))
				{
					if (szCapabilities[lstrlen(szCapabilities) - 1] == ' ')
						szCapabilities[lstrlen(szCapabilities) - 2] = '\0';
				}
				else
					lstrcpy(szCapabilities, "n/a");
				SetDlgItemText(hDlg, IDC_SOURCE_CAPABILITIES, szCapabilities);
			}
			else
			{
				SetDlgItemText(hDlg, IDC_CURRENT_SOURCE, "");
				SetDlgItemText(hDlg, IDC_SOURCE_PARAMETERS, "");
				SetDlgItemText(hDlg, IDC_SOURCE_MAX_PIDS, "");
				SetDlgItemText(hDlg, IDC_SOURCE_STOPABLE, "");
				SetDlgItemText(hDlg, IDC_SOURCE_CAPABILITIES, "");

				SetDlgItemText(hDlg, IDC_CURRENT_SOURCE_CAPTION, "");
				SetDlgItemText(hDlg, IDC_SOURCE_PARAMETERS_CAPTION, "");
				SetDlgItemText(hDlg, IDC_SOURCE_MAX_PIDS_CAPTION, "");
				SetDlgItemText(hDlg, IDC_SOURCE_STOPABLE_CAPTION, "");
				SetDlgItemText(hDlg, IDC_SOURCE_CAPABILITIES_CAPTION, "");
			}

			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}
	return FALSE;
}

unsigned short crc16tab[256] = 
{
0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0
};
#define CRC16(cp,crc) (crc16tab[(((unsigned short)crc>>8)&255)]^((unsigned short)crc<<8)^(unsigned char)cp)

unsigned short crc16(int count, char *buf)
{
	int	i;
	unsigned short crc = 0;

	for (i=0; i < count; i++)
		crc = CRC16(*buf++,crc);
	return (crc);
}

BOOL FAR PASCAL LicenseDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (!v->fAgreedToLicense)
		{
			TCHAR szLicense[] =
			{
				TEXT("IMPORTANT. This license agreement supersedes any other\r\n")
				TEXT("license agreement provided with the software.\r\n")
				TEXT("\r\n")
				TEXT("Read this License Agreement before installing this software.\r\n")
				TEXT("By installing this software, you indicate your acceptance\r\n")
				TEXT("of this License Agreement.\r\n")
				TEXT("\r\n")
				TEXT("COOL.STF LICENSE AGREEMENT\r\n")
#ifndef LITE
 #ifdef PRO
				TEXT("COOL.STF TSReader Professional Release 2\r\n")
 #else PRO
				TEXT("COOL.STF TSReader Standard Release 2\r\n")
 #endif PRO
#else LITE
				TEXT("COOL.STF TSReader Lite Release 2\r\n")
#endif LITE
				TEXT("\r\n")
				TEXT("1. LICENSE RIGHTS AND RESTRICTIONS.\r\n")
				TEXT("\r\n")
				TEXT("COOL.STF grants to you the non-exclusive, non-assignable\r\n")
				TEXT("right to use the enclosed product in object code form (the\r\n")
				TEXT("\"TSReader Code\") by a single user on a single computer\r\n")
				TEXT("system. You may not reverse engineer, decompile, or\r\n")
				TEXT("disassemble the TSReader Code, except to the extent\r\n")
				TEXT("that the foregoing restriction is expressly prohibited by\r\n")
				TEXT("applicable law. You may not rent or lease the TSReader\r\n")
				TEXT("Code, or otherwise transfer the TSReader Code and any\r\n")
				TEXT("accompanying written materials. All rights not expressly\r\n")
				TEXT("granted are reserved by COOL.STF.\r\n")
				TEXT("\r\n")
				TEXT("2. NO WARRANTIES.\r\n")
				TEXT("\r\n")
				TEXT("The TSReader Code and accompanying written\r\n")
				TEXT("materials are provided \"as is\", without warranty of any kind.\r\n")
				TEXT("To the maximum extent permitted by law, COOL.STF\r\n")
				TEXT("disclaims all warranties, either express or implied, including\r\n")
				TEXT("but not limited to implied warranties of merchantability, fitness\r\n")
				TEXT("for a particular purpose and noninfringment. The entire risk\r\n")
				TEXT("arising out of the use or performance of the TSReader\r\n")
				TEXT("Code and any accompanying written materials remains with\r\n")
				TEXT("you.\r\n")
				TEXT("\r\n")
				TEXT("3. NO LIABILITY FOR CONSEQUENTIAL DAMAGES.\r\n")
				TEXT("\r\n")
				TEXT("To the maximum extent permitted by applicable law: in no\r\n")
				TEXT("event shall COOL.STF or its suppliers be liable for any\r\n")
				TEXT("damages whatsoever (including, without limitation, damages\r\n")
				TEXT("for loss of business profits, business interruption, loss of\r\n")
				TEXT("business information, or other pecuniary loss) arising out of\r\n")
				TEXT("the use of or inability to use the TSReader Code, even\r\n")
				TEXT("if COOL.STF has been advised of the possibility of such\r\n")
				TEXT("damages.  Because some states/jurisdictions do not allow\r\n")
				TEXT("the exclusion or limitation of liability for consequential or\r\n")
				TEXT("incidental damages, the above limitation may not apply\r\n")
				TEXT("to you.\r\n")
				TEXT("\r\n")
				TEXT("4. MISCELLANEOUS.\r\n")
				TEXT("\r\n")
				TEXT("If you acquired this product in the United States, this\r\n")
				TEXT("Agreement is governed by the laws of the State of Maryland.\r\n")
				TEXT("If you acquired this product outside the United States, local\r\n")
				TEXT("law may apply. If either party employs attorneys to enforce\r\n")
				TEXT("any rights arising out of or relating to this Agreement, the\r\n")
				TEXT("prevailing party shall be entitled to recover reasonable\r\n")
				TEXT("attorneys fees, costs and expenses.\r\n")
#ifdef LITE
				TEXT("5. COMMERCIAL-USE PROHIBITED.\r\n")
				TEXT("\r\n")
				TEXT("You may not use the TSReader Code in commercial applications\r\n")
				TEXT("other than to evaluate the TSReader Code for use in your\r\n")
				TEXT("application with the intent to purchase the full version of\r\n")
				TEXT("the TSReader Code should it meet your needs.\r\n")
#endif LITE
			};
			char szStamp[] = {"[End of license agreement]"};

			SetDlgItemText(hDlg, IDC_LICENSE_TEXT, szStamp);
			SetDlgItemText(hDlg, IDC_LICENSE_TEXT, szLicense);
			SetForegroundWindow(hDlg);
			SetFocus(GetDlgItem(hDlg, IDC_BOGUS));
			ShowWindow(GetDlgItem(hDlg, IDC_BOGUS), SW_HIDE);
			return FALSE;
		}
		else
		{
			EndDialog(hDlg, TRUE);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
#ifdef NONOEM
 #ifndef LITE
			MessageBox(hDlg, "Please contact Digital River to obtain a refund of your license fees.\n\nYou are also required to remove this software from your computer since\nyou don't accept our license.\n\nTSReader will now close.", gszAppName, MB_ICONSTOP);
 #else LITE
			MessageBox(hDlg, "Please remove this software from your computer since you don't accept our license.\n\nTSReader will now close.", gszAppName, MB_ICONSTOP);
 #endif LITE
#else NONOEM
 #ifdef INEOQUEST
			MessageBox(hDlg, "Please contact IneoQuest for further information. TSReader will now close.", gszAppName, MB_ICONSTOP);
 #endif INEOQUEST
#endif NONOEM
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			v->fAgreedToLicense = TRUE;
			EndDialog(hDlg, TRUE);
			break;
		}
		break;
	}
	return FALSE;
}

#ifndef LITE
void RestartTSReader_Stop(HWND hWnd)
{
	int i;
	MSG msg;

	v->fStopping = TRUE;

#ifdef PRO
	if (v->fArchiveRunning)
		SendMessage(hWnd, WM_COMMAND, ID_RECORD_RECORDALLPROGRAMS, 0);
	if (v->fForwarderEnabled)
		SendMessage(hWnd, WM_COMMAND, ID_FORWARD_FORWARDTOUDP, 0);
	if (v->fMonitorRunning)
		SendMessage(v->hDlgStreamMonitor, WM_CLOSE, 0, 0);

	for (i = 0; i < MAX_FWD_DLLS; i++)
	{
		if (v->fwd.fActive[i])
			ForwarderModuleStartStop(hWnd, i);
	}
#endif PRO

	KillTimer(v->hDlgSIParser, 1);
	KillTimer(v->hDlgSIParser, 2);
	KillTimer(v->hDlgSIParser, 6);

	if (v->fRecording)
	{
		if (v->fStradisActive)
		{
			OutputDebugString("Shutdown stream decoder\n");
			StreamDecoder(hWnd);
		}
	}
	v->nGotKeys = GOT_DISABLE;
	
	for (i = 0; i < MAX_CHARTS; i++)
	{
		if (v->hWndChart[i] != NULL)
		{
			DestroyWindow(v->hWndChart[i]);
			v->hWndChart[i] = NULL;
		}
	}
	if (v->hWndEPGGrid != NULL)
	{
		DestroyWindow(v->hWndEPGGrid);
		v->hWndEPGGrid = NULL;
	}
#ifdef PRO
	if (v->hWndVideoMosaic != NULL)
	{
		DestroyWindow(v->hWndVideoMosaic);
		v->hWndVideoMosaic = NULL;
	}
	if (v->hWndCCDisplay != NULL)
	{
		DestroyWindow(v->hWndCCDisplay);
		v->hWndCCDisplay = NULL;
	}
#endif PRO

	Stop();
	DeInit();

	if ( (v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) && (v->ss.fSerialReceiverControlEnabled == TRUE) )
		SourceHelper_DeInitSerialControl();

	EnterCriticalSection(&v->ss.csTSBuffersInUse);
	v->ss.nTSBuffersInUse = -1000;
	LeaveCriticalSection(&v->ss.csTSBuffersInUse);

	do
	{
		while (PeekMessage(&msg, hWnd, WM_USER + 5, WM_USER + 5, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(50);
	} while ( (v->fParseThreadRunning == TRUE) || (v->ss.fReadThreadTerminated == FALSE) );

	// Let any outstanding messages from the SI parser thread
	// through before we delete stuff
	while (PeekMessage(&msg, hWnd, WM_USER + 2, WM_USER + 2, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	InvalidateRect(hWnd, NULL, TRUE);
	SetupForNewStream(hWnd);

	for ( i = 0 ; i < 5 ; i++ )
	{
		if (Ext_Dll[i].Extern_Src_Restart_Function != NULL )
			(Ext_Dll[i].Extern_Src_Restart_Function)();
	}


	SendMessage(v->hWndST, SB_SETTEXT, (WPARAM)0, (LPARAM)"");		
	SendMessage(v->hWndST, SB_SETTEXT, (WPARAM)1, (LPARAM)"");		
	SendMessage(v->hWndST, SB_SETTEXT, (WPARAM)2, (LPARAM)"");		
	v->fStopping = FALSE;
}
#endif LITE

void SaveManualChannels(HWND hDlg)
{
#ifdef LITE
	MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
	OPENFILENAME ofn;
	char szOutputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szOutputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Manual Channel Files(*.tmc)\0*.tmc\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Save Manual Channels");
	ofn.lpstrDefExt = TEXT("tmc");
	ofn.lpstrInitialDir = v->szManualChannelsInitialDir;
	ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	ofn.hInstance = v->hInstance;						
	if (myGetSaveFileName(&ofn) == TRUE)
	{
		HANDLE hOutputFile = CreateFile(szOutputFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
		if (hOutputFile != INVALID_HANDLE_VALUE)					
		{
			int nPMTIndex;

			for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
			{
				if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
					break;
				if (v->pat.pmt[nPMTIndex].nPMTPID == -2)
				{
					DWORD dwWritten;
					int nESIndex;
					char szOutputLine[128];

					wsprintf(szOutputLine, "%d,%d\r\n", v->pat.pmt[nPMTIndex].nProgramNumber, v->pat.pmt[nPMTIndex].nPCRPID);
					WriteFile(hOutputFile, szOutputLine, lstrlen(szOutputLine), &dwWritten, NULL);
					for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
					{
						if (v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType == 0)
							break;
						wsprintf(szOutputLine, "%d,%d\r\n", v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType, v->pat.pmt[nPMTIndex].es[nESIndex].nESPID);
						WriteFile(hOutputFile, szOutputLine, lstrlen(szOutputLine), &dwWritten, NULL);
					}
					lstrcpy(szOutputLine, "----\r\n");
					WriteFile(hOutputFile, szOutputLine, lstrlen(szOutputLine), &dwWritten, NULL);
				}
			}
			CloseHandle(hOutputFile);
			v->fDirtyManualChannels = FALSE;
		}
		else
			MessageBox(hDlg, "Unable to open output file", gszAppName, MB_ICONSTOP);
	}
#endif LITE
}

#ifndef LITE
void RestartTSReader_Start(HWND hWnd)
{
	Init(&v->ss);
	if ( (v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) && (v->ss.fSerialReceiverControlEnabled == TRUE) )
	{
		if (SourceHelper_InitSerialControl() == FALSE)
			v->ss.fSerialReceiverControlEnabled = FALSE;
	}

	PostMessage(hWnd, WM_USER + 5, 0, 0);
}
#endif LITE

void RestartTSReader(HWND hDlg)
{
#ifdef LITE
	MessageBox(hDlg, "The Restart Source function is not supported in this version of TSReader.\nYou must exit and restart TSReader to restart the source.", gszAppName, MB_ICONSTOP);
#else LITE
	if (v->fDirtyManualChannels == TRUE)
	{
		if (MessageBox(hDlg, "Manual channels have not been saved. Would you like to save them now?", gszAppName, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1) == IDYES)
			SaveManualChannels(hDlg);
	}

	RestartTSReader_Stop(hDlg);
	RestartTSReader_Start(hDlg);
#endif LITE
}

void UpdateThreadDialogPriorities(HWND hDlg)
{
	HMENU hMenu = GetMenu(hDlg);

	CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_CRITICAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_NORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_HIGH, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_LOW, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_NORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_HIGH, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_LOW, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_NORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_HIGH, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_LOW, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_DISABLED, MF_BYCOMMAND | MF_UNCHECKED);
	
	CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_REALTIME, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_HIGH, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_ABOVENORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_NORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_BELOWNORMAL, MF_BYCOMMAND | MF_UNCHECKED);
	CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_LOW, MF_BYCOMMAND | MF_UNCHECKED);

	switch(v->ss.nInputThreadPriority)
	{
	case 0:
		CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_NORMAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 1:
		CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_HIGH, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 2:
		CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_LOW, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 3:
		CheckMenuItem(hMenu, ID_SETTINGS_DATAINPUTTHEADPRIORITY_CRITICAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	}
	switch(v->nStreamProcessingThreadPriority)
	{
	case 0:
		CheckMenuItem(hMenu, ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_NORMAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 1:
		CheckMenuItem(hMenu, ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_HIGH, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 2:
		CheckMenuItem(hMenu, ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_LOW, MF_BYCOMMAND | MF_CHECKED);
		break;
	}
	switch(v->nThumbnailProcessingThreadPriority)
	{
	case 0:
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_NORMAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 1:
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_HIGH, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 2:
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_LOW, MF_BYCOMMAND | MF_CHECKED);
		break;
	case 3:
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_DISABLED, MF_BYCOMMAND | MF_CHECKED);
		break;		
	}
	EnableWindow(GetDlgItem(v->hDlgSIParser, IDC_VIDEO_FRAME), v->nThumbnailProcessingThreadPriority != 3);

	switch(v->nProcessPriority)
	{
	case REALTIME_PRIORITY_CLASS:
		CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_REALTIME, MF_BYCOMMAND | MF_CHECKED);
		break;
	case HIGH_PRIORITY_CLASS:
		CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_HIGH, MF_BYCOMMAND | MF_CHECKED);
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_ABOVENORMAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case NORMAL_PRIORITY_CLASS:
		CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_NORMAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_BELOWNORMAL, MF_BYCOMMAND | MF_CHECKED);
		break;
	case IDLE_PRIORITY_CLASS:
		CheckMenuItem(hMenu, ID_SETTINGS_MAINPROCESSPRIORITY_LOW, MF_BYCOMMAND | MF_CHECKED);
		break;
	}
}

void SetupMenu(HWND hWnd)
{
	HMENU hTopMenu = GetMenu(hWnd);

	if (v->fStradisInterface == FALSE)
		EnableMenuItem(hTopMenu, IDC_SI_PARSER_TO_STRADIS, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	if (v->fDSInterface == FALSE)
		EnableMenuItem(hTopMenu, ID_PLAYBACK_DIRECTSHOW, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);			
	if (v->fXNSInterface == FALSE)
		EnableMenuItem(hTopMenu, ID_PLAYBACK_XNSSERVER, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	if (v->fDVHSInterface == FALSE)
		EnableMenuItem(hTopMenu, ID_RECORD_RECORDPROGRAMTODVHS, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);

	if (v->fVLCInterface == FALSE)
	{
		int i;

		for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
			EnableMenuItem(hTopMenu, ID_PLAYBACK_VLC_1 + i, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);	
	}

#ifndef INEOQUEST
#ifndef LITE
	{
		HMENU hSubMenu = GetSubMenu(hTopMenu, 8);	// Help Menu
		DeleteMenu(hSubMenu, ID_HELP_PURCHASETSREADER, MF_BYCOMMAND);	// remove Purchase TSReader
	}
#endif LITE
#ifndef PRO
	{
		HMENU hSubMenu;

		DeleteMenu(hTopMenu, 5, MF_BYPOSITION);							// remove Forward menu

		hSubMenu = GetSubMenu(hTopMenu, 2);	// View Menu
		DeleteMenu(hSubMenu, ID_VIEW_ARCHIVEDFILES, MF_BYCOMMAND);		// remove the Archived Files menu
		DeleteMenu(hSubMenu, ID_VIEW_CLOSEDCAPTIONS, MF_BYCOMMAND);		// remove the Closed Captions menu
		DeleteMenu(hSubMenu, ID_VIEW_VIDEOMOSAIC, MF_BYCOMMAND);		// remove the Video Mosaic menu
		DeleteMenu(hSubMenu, ID_VIEW_MDIINDEX, MF_BYCOMMAND);			// remove the MDI Index menu
		DeleteMenu(hSubMenu, ID_VIEW_DESCRIPTORUSAGE, MF_BYCOMMAND);	// remove the Descriptors menu
		DeleteMenu(hSubMenu, ID_VIEW_STREAMMONITOR, MF_BYCOMMAND);		// remove the Stream Monitor menu
		hSubMenu = GetSubMenu(hSubMenu, 1);	// View/Chart Menu
		DeleteMenu(hSubMenu, ID_VIEW_CHART_SETTINGS_SAVECHARTDATA, MF_BYCOMMAND);
		DeleteMenu(hSubMenu, ID_VIEW_CHART_PIDUSAGEVBRSTACKEDAREA, MF_BYCOMMAND);

		hSubMenu = GetSubMenu(hTopMenu, 3);			// Record Menu
		DeleteMenu(hSubMenu, ID_RECORD_RECORDALLPROGRAMS, MF_BYCOMMAND);// remove the Record All Programs menu
		DeleteMenu(hSubMenu, ID_RECORD_RECORDTABLES, MF_BYCOMMAND);		// remove the Record Tables menu
			
		hSubMenu = GetSubMenu(hTopMenu, 6);			// Settings Menu
		DeleteMenu(hSubMenu, ID_SETTINGS_EITSERVER, MF_BYCOMMAND);		// remove EIT server menu
		DeleteMenu(hSubMenu, ID_SETTINGS_EMAILSETTINGS, MF_BYCOMMAND);	// remove email setup menu
		
		hSubMenu = GetSubMenu(hTopMenu, 1);			// Export Menu
		DeleteMenu(hSubMenu, ID_EXPORT_SAVEEPGDATA, MF_BYCOMMAND);		// remove Save EPG data
		DeleteMenu(hSubMenu, ID_EXPORT_GPSSIGNALEXPORT, MF_BYCOMMAND);	// remove Export GPS/Signal
	}
#else PRO
	{
		HMENU hSubMenu;

		hSubMenu = GetSubMenu(hTopMenu, 7);			// Settings Menu
		ModifyMenu(hSubMenu, ID_SETTINGS_THUMBNAILTHREAD_ENABLEAUDIOTHUMBNAILS, MF_STRING | MF_BYCOMMAND, ID_SETTINGS_THUMBNAILTHREAD_ENABLEAUDIOTHUMBNAILS, "Audio thumbnail settings...");
	}
#endif PRO
#endif INEOQUEST
#ifdef ZANALYZER
	{
		HMENU hSubMenu = GetSubMenu(hTopMenu, 4);	// Playback Menu
		DeleteMenu(hSubMenu, 0, MF_BYPOSITION);		// remove Roku Playback
		DeleteMenu(hSubMenu, 1, MF_BYPOSITION);		// remove XNS Playback
	}
#endif ZANALYZER
}

void SetupVLCMenuNames(HWND hDlg)
{
	int i;
	HMENU hMenu = GetMenu(hDlg);

	for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
	{
		char szNewString[MAX_PATH] = {" - "};

		if (lstrlen(v->szVLCConfigDescription[i]))
			lstrcpy(szNewString, v->szVLCConfigDescription[i]);
		if (i < 10)
		{
			char szTemp[16];
			wsprintf(szTemp, "\tCtrl+%d", i);
			lstrcat(szNewString, szTemp);
		}

		ModifyMenu(hMenu, ID_PLAYBACK_VLC_1 + i, MF_STRING | MF_BYCOMMAND, ID_PLAYBACK_VLC_1 + i, szNewString);
	}

	if (v->fRecording == TRUE && v->nStreamTo == STREAM_TO_VLC)
	{
		CheckMenuItem(GetMenu(hDlg), ID_PLAYBACK_VLC_1 + v->nVLCPlaybackConfig, MF_CHECKED | MF_BYCOMMAND);
		for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
		{
			if (i != v->nVLCPlaybackConfig)
				EnableMenuItem(GetMenu(hDlg), ID_PLAYBACK_VLC_1 + i, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
		}
	}
}

void IPDVBModeOn(HWND hWnd)
{
	SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, FALSE, 0);
	v->fDeletingAllTVItems = TRUE;
	TreeView_DeleteAllItems(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE));	
	v->fDeletingAllTVItems = FALSE;
	v->fIPDVBModeChanged = TRUE;
	v->fIPDVBMode = TRUE;
	WaitForThumbnailThread(hWnd);
	CleanupMPEGParsingThread(hWnd);
	memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
	ForcePIDChartRepaint(v->hDlgSIParser);
	CheckMenuItem(GetMenu(v->hWndMainWindow), IDC_SI_PARSER_IP_DVB_MODE, MF_CHECKED | MF_BYCOMMAND);					
	//UpdateMainStatusText("");
	SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, TRUE, 0);
	SetDlgItemText(v->hDlgSIParser, IDC_SI_TEXT, "");
	ResetParserPackets();
	ResetParserCRCs();
#ifdef PRO
	ResetParserTableErrors();
	ResetTableTimes();
	ResetTableTimingErrors();
#endif PRO
}

void IPDVBModeOff(HWND hWnd)
{
	SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, FALSE, 0);
	v->fDeletingAllTVItems = TRUE;
	TreeView_DeleteAllItems(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE));	
	v->fDeletingAllTVItems = FALSE;
	v->fIPDVBModeChanged = TRUE;
	v->fIPDVBMode = FALSE;
	CleanupIPParsingThread(hWnd);
	memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
	ForcePIDChartRepaint(v->hDlgSIParser);
	CheckMenuItem(GetMenu(v->hWndMainWindow), IDC_SI_PARSER_IP_DVB_MODE, MF_UNCHECKED | MF_BYCOMMAND);		
	v->fDidCAT = FALSE;
	if (!v->fForcedNetworkType)
	{
		v->nNetworkPID = -1;
		v->fISDB = FALSE;
	}
	v->nPMTTimeoutCounter = v->nPMTPID = 0;
	v->nDCIIECMPMTIndex = 0;
	v->nDCIIECMDescriptorPID = -1;
	ResetTreeViewHandles();
	v->nMinimumSDTChannel = 65536;
	v->nMinimumEITChannel = 65536;
	SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, TRUE, 0);
	SetDlgItemText(v->hDlgSIParser, IDC_SI_TEXT, "");
	UpdateMainStatusText("Reading PAT");
	ResetParserPackets();
	ResetParserCRCs();
#ifdef PRO
	ResetParserTableErrors();
	ResetTableTimes();
	ResetTableTimingErrors();
#endif PRO
	GetNextESPID(TRUE, 0);
}

void ToggleIPDVBMode(HWND hWnd)
{
	v->fRecordPIDMode = FALSE;
	if (v->fIPDVBMode == FALSE)
	{
		if (DialogBoxParam(v->hInstance, MAKEINTRESOURCE(IDD_IPDVB_PID), hWnd, IPDVBPIDSelectDlgProc, FALSE) == TRUE)
			IPDVBModeOn(hWnd);
	}
	else
		IPDVBModeOff(hWnd);
}

void SaveThumbnails(HWND hDlg, BOOL fContinuous)
{
#ifdef LITE
	MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
	int nExportCount = 0;
	OPENFILENAME ofn;

	if (fContinuous == TRUE && v->fSaveThumbnails == TRUE)
	{
		HMENU hMenu = GetMenu(hDlg);

		v->fSaveThumbnails = FALSE;
		CheckMenuItem(hMenu, ID_EXPORT_SAVEALLTHUMBNAILS, MF_UNCHECKED | MF_BYCOMMAND);
		EnableMenuItem(hMenu, ID_EXPORT_SAVETHUMBNAILS, MF_ENABLED | MF_BYCOMMAND);
		return;
	}

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = v->szThumbnailBaseFilename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("JPG Files (*.jpg)\0*.jpg\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Export Thumbnails");
	ofn.lpstrDefExt = TEXT("jpg");
	ofn.lpstrInitialDir = v->szThumbnailInitialDir;
	ofn.Flags =  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER;
	ofn.hInstance = v->hInstance;						
	if (myGetSaveFileName(&ofn) == TRUE)
	{
		if (fContinuous)
		{
			HMENU hMenu = GetMenu(hDlg);
			
			v->fSaveThumbnails = TRUE;
			CheckMenuItem(hMenu, ID_EXPORT_SAVEALLTHUMBNAILS, MF_CHECKED | MF_BYCOMMAND);
			EnableMenuItem(hMenu, ID_EXPORT_SAVETHUMBNAILS, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
		}
		else
		{
			int nPMTIndex, nESIndex;

			{
				char szTemp[MAX_PATH];
				wsprintf(szTemp, "TSReader: Thumbnail base file %s\n", v->szThumbnailBaseFilename);
				OutputDebugString(szTemp);
			}
			for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
			{
				for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
				{
					if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
						break;
					EnterCriticalSection(&v->csThumbnails);
					if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
					{
						char szRealFilename[MAX_PATH];
						char * szExtension;

						lstrcpy(szRealFilename, v->szThumbnailBaseFilename);
						szExtension = GetExtensionPtr(szRealFilename);
						if (szExtension != NULL)
						{
							HISDEST	hDestinationObject;
							char szSavedExtension[128];
							char szChannelTag[128];
							
							*szExtension = '\0';
							lstrcpy(szSavedExtension, szExtension + 1);
							wsprintf(szChannelTag, "_%d.%s", v->pat.pmt[nPMTIndex].nProgramNumber, szSavedExtension);
							lstrcat(szRealFilename, szChannelTag);
							hDestinationObject = _ISOpenFileDest(szRealFilename);
							{
								char szTemp[MAX_PATH];
								wsprintf(szTemp, "TSReader: Thumbnail current file %s hDestinationObject = %08x\n", szRealFilename, hDestinationObject);
								OutputDebugString(szTemp);
							}
							if (hDestinationObject != NULL)
							{
								_ISWriteRGBToJPG(hDestinationObject,
												 v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame,
												 v->pat.pmt[nPMTIndex].es[nESIndex].nVideoWidth,
												 v->pat.pmt[nPMTIndex].es[nESIndex].nVideoHeight,
												 100,
												 0);
								_ISCloseDest(hDestinationObject);
								nExportCount++;
							}
						}
					}
					else
					{
						char szTemp[256];
						wsprintf(szTemp, "TSReader: Thumbnail v->pat.pmt[nPMTIndex=%d].es[nESIndex=%d].pRGBVideoFrame == NULL\n", nPMTIndex, nESIndex);
						OutputDebugString(szTemp);
					}
					LeaveCriticalSection(&v->csThumbnails);
				}
			}
			if (nExportCount)
			{
				char szTemp[256];
				wsprintf(szTemp, "%d Thumbnail(s) exported", nExportCount);
				MessageBox(hDlg, szTemp, gszAppName, MB_OK | MB_ICONINFORMATION);
			}
			else
				MessageBox(hDlg, "No thumbnails exported", gszAppName, MB_OK | MB_ICONINFORMATION);
		}
	}
#endif LITE
}
/*
void AddMemoryItem(char * szTags, char * szUsage, char * szTitle, int nDataItem, BOOL fDontTranslate)
{
	char szTemp[128];

	lstrcat(szTags, szTitle);
	lstrcat(szTags, ":\n");

	if ( (nDataItem < 1000) || (fDontTranslate == TRUE) )
		wsprintf(szTemp, "%d\n", nDataItem);
	else if (nDataItem < 1000 * 1000)
		sprintf(szTemp, "%.3f KB\n", (float)nDataItem / 1024.0f);
	else 
		sprintf(szTemp, "%.3f MB\n", (float)nDataItem / 1024.0f / 1024.0f);
	lstrcat(szUsage, szTemp);
}
*/
BOOL CALLBACK ThumbnailRefreshRateDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_REFRESH_RATE, v->nESParsingCounterReload, FALSE);
		SetDlgItemInt(hDlg, IDC_MAXIMUM_MPEG_PICTURES, v->nMaximumMPEGPictures, FALSE);
		SetDlgItemInt(hDlg, IDC_MAXIMUM_DCII_PICTURES, v->nMaximumDCIIPictures, FALSE);
#ifndef LITE
		SetDlgItemInt(hDlg, IDC_MAXIMUM_H264_PICTURES, v->nMaximumH264Pictures, FALSE);
#endif LITE
#ifdef PRO
		SetDlgItemInt(hDlg, IDC_MAXIMUM_THUMBNAIL_THREADS, v->nMaximumThumbnailThreads, FALSE);
#endif PRO

#ifdef LITE
		ShowWindow(GetDlgItem(hDlg, IDC_MAXIMUM_H264_PICTURES), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_MAXIMUM_H264_PICTURES_CAPTION), SW_HIDE);
#endif LITE

#ifdef LITE
		EnableWindow(GetDlgItem(hDlg, IDC_MAXIMUM_MPEG_PICTURES), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_MAXIMUM_DCII_PICTURES), FALSE);
#endif LITE
		SendDlgItemMessage(hDlg, IDC_REFRESH_RATE, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_REFRESH_RATE));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->nESParsingCounterReload = GetDlgItemInt(hDlg, IDC_REFRESH_RATE, NULL, FALSE);
			v->nMaximumMPEGPictures = GetDlgItemInt(hDlg, IDC_MAXIMUM_MPEG_PICTURES, NULL, FALSE);
			v->nMaximumDCIIPictures = GetDlgItemInt(hDlg, IDC_MAXIMUM_DCII_PICTURES, NULL, FALSE);
#ifndef LITE
			v->nMaximumH264Pictures = GetDlgItemInt(hDlg, IDC_MAXIMUM_H264_PICTURES, NULL, FALSE);
#endif LITE
#ifdef PRO
			v->nMaximumThumbnailThreads = GetDlgItemInt(hDlg, IDC_MAXIMUM_THUMBNAIL_THREADS, NULL, FALSE);
			if (v->nMaximumThumbnailThreads > MAX_ES_PARSERS || v->nMaximumThumbnailThreads <= 0)
				v->nMaximumThumbnailThreads = MAX_ES_PARSERS;
#endif PRO
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK AutoRestartDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_AUTORESTART_PAT_CHANGE, v->fAutoRestartOnPATVersionChange);
		CheckDlgButton(hDlg, IDC_AUTORESTART_STOP, v->fAutoRestartOnDataStop);
		SetDlgItemInt(hDlg, IDC_AUTORESTART_STOP_DELAY, v->nAutoRestartOnDataStopDelay, FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_AUTORESTART_STOP_DELAY), v->fAutoRestartOnDataStop);
		CheckDlgButton(hDlg, IDC_AUTORESTART_NO_TUNE_DIALOG, v->fAutoRestartNoTuneDialog);
		CheckDlgButton(hDlg, IDC_RESTART_NOBEEP, v->fAutoRestartNoBeep);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fAutoRestartOnPATVersionChange = IsDlgButtonChecked(hDlg, IDC_AUTORESTART_PAT_CHANGE);
			v->fAutoRestartOnDataStop = IsDlgButtonChecked(hDlg, IDC_AUTORESTART_STOP);
			v->nAutoRestartOnDataStopDelay = GetDlgItemInt(hDlg, IDC_AUTORESTART_STOP_DELAY, NULL, FALSE);
			v->fAutoRestartNoTuneDialog = IsDlgButtonChecked(hDlg, IDC_AUTORESTART_NO_TUNE_DIALOG);
			v->fAutoRestartNoBeep = IsDlgButtonChecked(hDlg, IDC_RESTART_NOBEEP);

			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_AUTORESTART_STOP:
			EnableWindow(GetDlgItem(hDlg, IDC_AUTORESTART_STOP_DELAY), IsDlgButtonChecked(hDlg, IDC_AUTORESTART_STOP));
			break;
		}
		break;			
	}

	return FALSE;
}

BOOL CALLBACK ConfirmATSCRCDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_ATSC_RC_CONFIRM), FALSE);
		SetFocus(GetDlgItem(hDlg, IDCANCEL));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case EN_CHANGE:
			{
				char szString[128];
				
				GetDlgItemText(hDlg, IDC_ATSC_RC_CONFIRM, szString, sizeof(szString));
				strupr(szString);
				if (lstrcmp(szString, "I CONFIRM") == 0)
					EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
				else
					EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
			}
			break;
		case BN_CLICKED:
			switch(LOWORD(wParam))
			{
			case IDOK:
				EndDialog(hDlg, TRUE);
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			case IDC_ATSC_RC_FCC:
			case IDC_ATSC_RC_OUTSIDE_FCC:
				EnableWindow(GetDlgItem(hDlg, IDC_ATSC_RC_CONFIRM), TRUE);
				SetFocus(GetDlgItem(hDlg, IDC_ATSC_RC_CONFIRM));
				break;
			}
			break;
		}
		break;
	}

	return FALSE;
}

void GetSourceDispInfo(LV_DISPINFO *pnmv) 
{ 
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nModuleIndex = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			lstrcpy(pnmv->item.pszText, v->sourcemodules[nModuleIndex].szDisplayName);
			break;
		case 1:
			lstrcpy(pnmv->item.pszText, v->sourcemodules[nModuleIndex].szDescription);
			break;
		}
	}
}

#ifndef LITE
void BrowseIPSaveFolder(HWND hDlg)
{
	BROWSEINFO BrowsingInfo;
	char DirPath[MAX_PATH];
	char FolderName[MAX_PATH];
	LPITEMIDLIST ItemID;

	lstrcpy(FolderName, v->szIPSaveFolder);

	memset(&BrowsingInfo, 0, sizeof(BROWSEINFO));
	memset(DirPath, 0, MAX_PATH);

	BrowsingInfo.hwndOwner      = hDlg;
	BrowsingInfo.pszDisplayName = FolderName;
	BrowsingInfo.lpszTitle      = "Select output folder for recorded IP data";
	BrowsingInfo.ulFlags = BIF_NEWDIALOGSTYLE;

	ItemID = SHBrowseForFolder(&BrowsingInfo);
	if (ItemID)
	{
		SHGetPathFromIDList(ItemID, DirPath);
		SetDlgItemText(hDlg, IDC_IP_SAVE_FOLDER, DirPath);
	}
}

BOOL CALLBACK IPSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_IP_SAVE_FOLDER, v->szIPSaveFolder);
		SendDlgItemMessage(hDlg, IDC_IP_SAVE_FOLDER, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_IP_SAVE_FOLDER));
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_IP_SAVE_FOLDER, v->szIPSaveFolder, sizeof(v->szIPSaveFolder));
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_IP_SAVE_FOLDER_ELIPSES:
			BrowseIPSaveFolder(hDlg);
			break;
		}
	}

	return FALSE;
}
#endif LITE

BOOL CALLBACK PIDListDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int i;
			int nActivePIDs = 0;
			char szTemp[64];

			for (i = 0; i < 8192; i++)
			{
				char szTemp[128];

				if (v->fSortChartByPID == FALSE)
				{
					if (v->pc[i].lnPackets == 0)
						break;
				}
				else
				{
					if (v->pc[i].lnPackets == 0)
						continue;
				}

				GetPIDTooltipInfo(v->pc[i].nPID, szTemp);
				SendDlgItemMessage(hDlg, IDC_PID_LISTBOX, LB_ADDSTRING, 0, (LPARAM)szTemp);
				nActivePIDs++;
			}
			wsprintf(szTemp, "PID List - %d PIDs active", nActivePIDs);
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)szTemp);
			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, TRUE);
			break;
		case IDC_PID_LIST_COPY:
			CopyListControlToClipboard(GetDlgItem(hDlg, IDC_PID_LISTBOX), TRUE);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

#ifndef LITE
BOOL CALLBACK ControlServerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			BOOL fControlServerPriorEnabled = v->fControlServerEnabled;
			CheckDlgButton(hDlg, IDC_CONTROL_SERVER_ENABLED, v->fControlServerEnabled);
			SetDlgItemInt(hDlg, IDC_CONTROL_SERVER_PORT, v->nControlServerPort, FALSE);
			if (fControlServerPriorEnabled == FALSE && v->fControlServerEnabled == TRUE)
			{
				// Previously wasn't on - now is, so start it
				StartControlServer();
			} else if (fControlServerPriorEnabled == TRUE && v->fControlServerEnabled == FALSE)
			{
				// Previously was on - now it isn't, so stop it
				TerminateControlServer();
			}
			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			if (IsDlgButtonChecked(hDlg, IDC_CONTROL_SERVER_ENABLED) && v->fControlServerEnabled == FALSE)
				MessageBox(hDlg, "You need to quit and launch TSReader again for this change to take effect", gszAppName, MB_ICONINFORMATION);
			v->fControlServerEnabled = IsDlgButtonChecked(hDlg, IDC_CONTROL_SERVER_ENABLED);
			v->nControlServerPort = GetDlgItemInt(hDlg, IDC_CONTROL_SERVER_PORT, NULL, FALSE);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}
#endif LITE

BOOL CALLBACK BufferSizesDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_STREAMING_PIPE_SIZE, v->nStreamingPipeSize, FALSE);
		SetDlgItemInt(hDlg, IDC_THUMBNAIL_PIPE_SIZE, v->nThumbnailPipeSize, FALSE);
		SendDlgItemMessage(hDlg, IDC_STREAMING_PIPE_SIZE, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_STREAMING_PIPE_SIZE));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->nStreamingPipeSize = GetDlgItemInt(hDlg, IDC_STREAMING_PIPE_SIZE, NULL, FALSE);
			v->nThumbnailPipeSize = GetDlgItemInt(hDlg, IDC_THUMBNAIL_PIPE_SIZE, NULL, FALSE);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

void DeleteObsoleteSourceModules(char * szSourceModule)
{
	if (strstr(szSourceModule, "TSReader_TTBudget.dll") != NULL)
		DeleteFile(szSourceModule);
	if (strstr(szSourceModule, "TSReader_Twinhan2.dll") != NULL)
		DeleteFile(szSourceModule);
	if (strstr(szSourceModule, "TSReader_Twinhan.dll") != NULL)
		DeleteFile(szSourceModule);
	if (strstr(szSourceModule, "TSReader_SkySeeker") != NULL)
		DeleteFile(szSourceModule);
}

int PopulateSourceList(HWND hDlg, HWND hWndLV)
{
	int nModuleIndex = 0;
	HANDLE hFind;
	LV_ITEM lvi; 
	char szCurrentDir[MAX_PATH];
	char szCurrentFile[MAX_PATH * 2];
	WIN32_FIND_DATA fd;

	// Find sources
	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	sprintf(szCurrentFile, "%s\\Sources\\*.dll", szCurrentDir);
	hFind = FindFirstFile(szCurrentFile, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			HMODULE hSource;
			char * szStartOfModuleName;

			wsprintf(szCurrentFile, "%s\\Sources\\%s", szCurrentDir, fd.cFileName);
			DeleteObsoleteSourceModules(szCurrentFile);		
			szStartOfModuleName = strstr(fd.cFileName, "_");
			if (szStartOfModuleName != NULL)
			{
				hSource = LoadLibrary(szCurrentFile);
				if (hSource != NULL)
				{
					BOOL (* myGetDescription) (char * szDescription, char * szCommandLineParameters, BOOL * fCanBeStopped, int * nMaxPIDs, DWORD * dwCapabilities);
					myGetDescription = (td_GetDescription)GetProcAddress(hSource, "TSReader_GetDescription");
					if (myGetDescription != NULL)
					{
						lstrcpy(v->sourcemodules[nModuleIndex].szDisplayName, szStartOfModuleName + 1);
						lstrcpy(v->sourcemodules[nModuleIndex].szFilename, szCurrentFile);
						v->sourcemodules[nModuleIndex].szDescription[0] = '\0';
						v->sourcemodules[nModuleIndex].szCommandLineParameters[0] = '\0';
						myGetDescription(v->sourcemodules[nModuleIndex].szDescription,
									   v->sourcemodules[nModuleIndex].szCommandLineParameters,
									   NULL,
									   NULL,
									   &v->sourcemodules[nModuleIndex].dwCapabilities);

						memset(&lvi, 0, sizeof(lvi));
						lvi.state = 0; 
						lvi.stateMask = 0; 
						lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
						lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
						lvi.iItem = nModuleIndex++; 
						lvi.iSubItem = 0; 
						lvi.lParam = (LPARAM) 0;    // item data 
						ListView_InsertItem(hWndLV, &lvi);
						if (nModuleIndex == MAX_SOURCE_MODULES)
						{
							MessageBox(hDlg, "Too many source modules. Please email support@tsreader.co.uk and tell me this happened!", gszAppName, MB_ICONSTOP);
							break;
						}
					}
					FreeLibrary(hSource);
				}
			}
		} while (FindNextFile(hFind, &fd) == TRUE);
		FindClose(hFind);
	}

	return nModuleIndex;
}

BOOL CALLBACK SourceDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int nColumnPosition = 0;
			HWND hWndSourceList = GetDlgItem(hDlg, IDC_SOURCE_LIST);
			LV_COLUMN lvc; 
			char szTemp[128];

			SetDlgItemText(hDlg, IDC_COMMAND_LINE_PARAMETERS, "");

			// Setup list view columns. 
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			lvc.pszText = szTemp; 

			lvc.fmt = LVCFMT_LEFT; 
			lvc.cx = 150; 
			lstrcpy(szTemp, TEXT("Name"));
			ListView_InsertColumn(hWndSourceList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_LEFT; 
			lvc.cx = 365; 
			lstrcpy(szTemp, TEXT("Description"));
			ListView_InsertColumn(hWndSourceList, nColumnPosition++, &lvc); 

			ListView_SetExtendedListViewStyle(hWndSourceList, LVS_EX_FULLROWSELECT);

			PopulateSourceList(hDlg, hWndSourceList);
			v->nSelectedSource = -1;

			SendDlgItemMessage(hDlg, IDC_COMMAND_LINE_PARAMETERS, WM_SETFONT, (WPARAM)v->hCourierNew, MAKELONG(TRUE, 0));
			SetForegroundWindow(hDlg);
			SetFocus(GetDlgItem(hDlg, IDCANCEL));
			EnableWindow(GetDlgItem(hDlg, IDC_SOURCE_DEVICE_NUMBER), FALSE);
			SetDlgItemInt(hDlg, IDC_SOURCE_DEVICE_NUMBER, v->ss.nSourceIndex, FALSE);
		}
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			lstrcpy(v->szSourceName, v->sourcemodules[v->nSelectedSource].szFilename);
			if (v->sourcemodules[v->nSelectedSource].dwCapabilities & CAPABILITIES_MULTICARD)
				v->ss.nSourceIndex = GetDlgItemInt(hDlg, IDC_SOURCE_DEVICE_NUMBER, NULL, FALSE);
			else
				v->ss.nSourceIndex = 0;
			EndDialog(hDlg, TRUE);
			break;
		}
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetSourceDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
				{
					v->nSelectedSource = pnmv->iItem;
					EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
					SetDlgItemText(hDlg, IDC_COMMAND_LINE_PARAMETERS, v->sourcemodules[v->nSelectedSource].szCommandLineParameters);
					EnableWindow(GetDlgItem(hDlg, IDC_SOURCE_DEVICE_NUMBER), v->sourcemodules[v->nSelectedSource].dwCapabilities & CAPABILITIES_MULTICARD);					
				}
			}
			break;
		case NM_DBLCLK:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
			}
			break;
		}
		break;
	}
	return FALSE;
}

#ifndef LITE
void PopulateManualChannelList(HWND hWndList)
{
	int nPMTIndex;

	ListView_DeleteAllItems(hWndList);
	for (nPMTIndex  = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
	{
		if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
			break;
		if (v->pat.pmt[nPMTIndex].nPMTPID == -2)
		{
			LV_ITEM lvi; 
			memset(&lvi, 0, sizeof(lvi));
			lvi.state = 0; 
			lvi.stateMask = 0; 
			lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
			lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
			lvi.iItem = 0; 
			lvi.iSubItem = 0; 
			lvi.lParam = (LPARAM)nPMTIndex;    // item data 
			ListView_InsertItem(hWndList, &lvi);
		}
	}
}

void GetManualChannelDisplayInfo(LV_DISPINFO *pnmv) 
{ 
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nPMTIndex = (int)(pnmv->item.lParam);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			wsprintf(pnmv->item.pszText, "%d", v->pat.pmt[nPMTIndex].nProgramNumber);
			break;
		case 1:
			{
				int nESIndex;
				char szDetails[128] = {0};
				char szTemp[128];

				for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
				{
					if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
						break;

					switch(v->pat.pmt[nPMTIndex].es[nESIndex].nStreamType)
					{
						case 0x01:	// MPEG-1 or MPEG-2 video
						case 0x02:
						case 0x10:	// MPEG-4 video
						case 0x1b:	// H264 video
						case 0x80:	// DCII video
						case 0xea:	// VC1 video
							wsprintf(szTemp, "VPID = %04x ", v->pat.pmt[nPMTIndex].es[nESIndex].nESPID);
							lstrcat(szDetails, szTemp);
							break;
						case 0x03:	// MPEG-1 or MPEG-2 audio
						case 0x04:
						case 0x0f:	// MPEG-2 AAC
						case 0x11:	// MPEG-4 AAC
						case 0x81:	// Dolby AC3 audio
						case 0x83:	// LPCM audio
						case 0x85:	// DTS audio
						case 0xe6:	// WM9 audio
							wsprintf(szTemp, "APID = %04x ", v->pat.pmt[nPMTIndex].es[nESIndex].nESPID);
							lstrcat(szDetails, szTemp);
							break;
					}
				}

				wsprintf(szTemp, "PCR = %04x", v->pat.pmt[nPMTIndex].nPCRPID);
				lstrcat(szDetails, szTemp);
				lstrcpy(pnmv->item.pszText, szDetails);
			}
			break;
		}
	}
}

BOOL CALLBACK ManualChannelEditDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			if (v->editpmt.nProgramNumber)
			{
				int nESIndex;
				char szTemp[32];

				SetDlgItemInt(hDlg, IDC_MANUAL_PROGRAM_NUMBER, v->editpmt.nProgramNumber, FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_MANUAL_PROGRAM_NUMBER), FALSE);

				wsprintf(szTemp, v->szOutputPIDFlags, v->editpmt.nPCRPID);
				SetDlgItemText(hDlg, IDC_MANUAL_PCR_PID, szTemp);
				for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
				{
					char szMask[32];

					if (v->editpmt.es[nESIndex].nStreamType == 0)
						break;

					if (v->fDecimalPIDs)
						lstrcpy(szMask, "%02x: %d");
					else
						lstrcpy(szMask, "%02x: %04x");
					wsprintf(szTemp, szMask, v->editpmt.es[nESIndex].nStreamType, v->editpmt.es[nESIndex].nESPID);
					SendDlgItemMessage(hDlg, IDC_MANUAL_ES_LIST, LB_ADDSTRING, 0, (LPARAM)szTemp);
				}
			}
			if (v->fDecimalPIDs)
			{
				SetDlgItemText(hDlg, IDC_MANUAL_PCR_PID_CAPTION, "PCR PID (dec)");
				SetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_PID_CAPTION, "Stream PID (dec)");

			}
			SetFocus(GetDlgItem(hDlg, IDC_MANUAL_PROGRAM_NUMBER));
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_MANUAL_ES_MPEG_VIDEO:
			SetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_TYPE, "02"); // MPEG-2 Video
			SetFocus(GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_ES_PID));
			break;
		case IDC_MANUAL_ES_MPEG_AUDIO:
			SetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_TYPE, "03"); // MPEG-1 Audio
			SetFocus(GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_ES_PID));
			break;
		case IDC_MANUAL_ES_AC3_AUDIO:
			SetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_TYPE, "81"); // AC3 Audio
			SetFocus(GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_ES_PID));
			break;
		case IDOK:
			{
				int nProgramNumber = GetDlgItemInt(hDlg, IDC_MANUAL_PROGRAM_NUMBER, NULL, FALSE);
				int nPCRPID = 0;
				int nPMTIndex;
				char szTemp[64];

				GetDlgItemText(hDlg, IDC_MANUAL_PCR_PID, szTemp, sizeof(szTemp));
				if (v->fDecimalPIDs)
					sscanf(szTemp, "%d", &nPCRPID);
				else
					sscanf(szTemp, "%x", &nPCRPID);
				if (nPCRPID <= 0 || nPCRPID > 8191)
				{
					MessageBox(hDlg, "The PCR PID is not valid", gszAppName, MB_ICONSTOP);
					SetFocus(GetDlgItem(hDlg, IDC_MANUAL_PCR_PID));
					return FALSE;
				}
				v->editpmt.nPCRPID = nPCRPID;

				if (v->editpmt.nProgramNumber == 0)
				{
					if (nProgramNumber <= 0 || nProgramNumber > 65534)
					{
						MessageBox(hDlg, "The program number is not valid", gszAppName, MB_ICONSTOP);
						SetFocus(GetDlgItem(hDlg, IDC_MANUAL_PROGRAM_NUMBER));
						return FALSE;
					}
					for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
					{
						if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
							break;
						if (v->pat.pmt[nPMTIndex].nProgramNumber == nProgramNumber)
						{
							MessageBox(hDlg, "This program number already exists - choose another", gszAppName, MB_ICONSTOP);
							SetFocus(GetDlgItem(hDlg, IDC_MANUAL_PROGRAM_NUMBER));
							return FALSE;
						}
					}
				}

				for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
				{
					if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
					{
						if (v->editpmt.nProgramNumber == 0)
						{
							HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);
							LV_ITEM lvi; 
							char szTemp[128];

							// add this channel to the end
							v->editpmt.nProgramNumber = nProgramNumber;
							v->editpmt.nPMTPID = -2;
							memcpy(&v->pat.pmt[nPMTIndex], &v->editpmt, sizeof(PMT));

							if (v->pat.hPATTreeItem == NULL)
							{
								char szMask[64];

								wsprintf(szMask, "PAT PID %s", v->szOutputPIDFlags);
								wsprintf(szTemp, szMask, 0);
								v->pat.hPATTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_PAT, 0, NULL, TVI_FIRST);
							}

							wsprintf(szTemp, "Manual - Program %d", v->pat.pmt[nPMTIndex].nProgramNumber);
							v->pat.pmt[nPMTIndex].hPMTTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_PMT + nPMTIndex, 1, v->pat.hPATTreeItem, NULL);
							PostMessage(v->hDlgSIParser, WM_USER + 2, SI_PARSER_PMT, nPMTIndex);

							memset(&lvi, 0, sizeof(lvi));
							lvi.state = 0; 
							lvi.stateMask = 0; 
							lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
							lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
							lvi.iItem = 0; 
							lvi.iSubItem = 0; 
							lvi.lParam = (LPARAM)nPMTIndex;    // item data 
							ListView_InsertItem(GetDlgItem(GetParent(hDlg), IDC_MANUAL_CHANNEL_LIST), &lvi);
						}
						break;
					}
					if (v->pat.pmt[nPMTIndex].nProgramNumber == nProgramNumber)
					{
						memcpy(&v->pat.pmt[nPMTIndex], &v->editpmt, sizeof(PMT));
						break;
					}
				}
			}
			v->fDirtyManualChannels = TRUE;
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_MANUAL_ES_ADD:
			{
				int nESType = 0;
				int nESPID = 0;
				int nESIndex;
				char szTemp[16];

				GetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_TYPE, szTemp, sizeof(szTemp));
				sscanf(szTemp, "%x", &nESType);
				GetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_PID, szTemp, sizeof(szTemp));
				if (v->fDecimalPIDs)
					sscanf(szTemp, "%d", &nESPID);
				else
					sscanf(szTemp, "%x", &nESPID);
				if (nESPID == 0 || nESType == 0)
				{
					MessageBox(hDlg, "Set the ES type and ES PID before using this function", gszAppName, MB_ICONSTOP);
					return FALSE;
				}
				if (nESPID <= 0 || nESPID > 8190)
				{
					MessageBox(hDlg, "The ES PID is not valid", gszAppName, MB_ICONSTOP);
					SetFocus(GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_ES_PID));
					return FALSE;
				}
				if (nESType < 0 || nESType > 0xff)
				{
					MessageBox(hDlg, "The ES Type is not valid", gszAppName, MB_ICONSTOP);
					SetFocus(GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_ES_PID));
					return FALSE;
				}
				
				for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
				{
					if (v->editpmt.es[nESIndex].nStreamType == 0)
					{
						char szTemp[32];
						char szMask[32];

						v->editpmt.es[nESIndex].nStreamType = nESType;
						v->editpmt.es[nESIndex].nESPID = nESPID;
						if (v->fDecimalPIDs)
							lstrcpy(szMask, "%02x: %d");
						else
							lstrcpy(szMask, "%02x: %04x");						
						wsprintf(szTemp, szMask, v->editpmt.es[nESIndex].nStreamType, v->editpmt.es[nESIndex].nESPID);							
						SendDlgItemMessage(hDlg, IDC_MANUAL_ES_LIST, LB_ADDSTRING, 0, (LPARAM)szTemp);
						SetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_TYPE, "");
						SetDlgItemText(hDlg, IDC_MANUAL_CHANNEL_ES_PID, "");
						SetFocus(GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_ES_TYPE));
						break;
					}
				}
				if (nESIndex == MAX_ESLIST_ENTRIES)
					MessageBox(hDlg, "Not enough room for this many elementary streams!", gszAppName, MB_ICONSTOP);
			}
			break;
		case IDC_MANUAL_ES_DELETE:
			{
				int nSelected;

				nSelected = SendDlgItemMessage(hDlg, IDC_MANUAL_ES_LIST, LB_GETCURSEL, 0, 0);
				if (nSelected == LB_ERR)
					MessageBox(hDlg, "Select an ES entry to delete first", gszAppName, MB_ICONSTOP);
				else
				{
					PMT newpmt;
					int nInputESIndex, nOutputESIndex = 0;

					memset(&newpmt, 0, sizeof(newpmt));
					newpmt.nPCRPID = v->editpmt.nPCRPID;
					newpmt.nProgramNumber = v->editpmt.nProgramNumber;

					for (nInputESIndex = 0; nInputESIndex < MAX_ESLIST_ENTRIES; nInputESIndex++)
					{
						if (nSelected != nInputESIndex)
							memcpy(&newpmt.es[nOutputESIndex++], &v->editpmt.es[nInputESIndex], sizeof(ESLIST));
					}
					memcpy(&v->editpmt, &newpmt, sizeof(PMT));
					SendDlgItemMessage(hDlg, IDC_MANUAL_ES_LIST, LB_DELETESTRING, nSelected, 0);
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

int GetManualChannelListSelected(HWND hWndManualChannelList, int * nPMTIndex)
{
	int nCount = ListView_GetItemCount(hWndManualChannelList);
	int i;

	for (i = 0; i < nCount; i++)
	{
		LV_ITEM lvItem;

		lvItem.mask = LVIF_STATE | LVIF_PARAM;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.stateMask = LVIS_SELECTED;
		ListView_GetItem(hWndManualChannelList, &lvItem);
		if ((lvItem.state & LVIS_SELECTED) > 0)
		{
			*nPMTIndex = lvItem.lParam;
			return i;
		}
	}
	return LB_ERR;
}

BOOL CALLBACK ManualChannelsDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hWndManualChannelList = GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_LIST);
			int nColumnPosition = 0;
			LV_COLUMN lvc; 
			char szTemp[128];

			// Setup list view columns. 
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			lvc.pszText = szTemp; 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 75; 
			lstrcpy(szTemp, TEXT("Program #"));
			ListView_InsertColumn(hWndManualChannelList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_LEFT; 
			lvc.cx = 220; 
			lstrcpy(szTemp, TEXT("Details"));
			ListView_InsertColumn(hWndManualChannelList, nColumnPosition++, &lvc); 

			ListView_SetExtendedListViewStyle(hWndManualChannelList, LVS_EX_FULLROWSELECT);
			PopulateManualChannelList(hWndManualChannelList);
			SetFocus(GetDlgItem(hDlg, IDCANCEL));
		}
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetManualChannelDisplayInfo((LV_DISPINFO *) lParam);
			}
			break;
		case NM_DBLCLK:
			PostMessage(hDlg, WM_COMMAND, IDC_EDIT_MANUAL_CHANNEL, 0);
			break;
		}
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_ADD_MANUAL_CHANNEL:
			memset(&v->editpmt, 0, sizeof(v->editpmt));
			DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_MANUAL_CHANNEL_EDIT), hDlg, ManualChannelEditDialogProc);
			break;
		case IDC_EDIT_MANUAL_CHANNEL:
			{
				int nPMTIndex;
				HWND hWndManualChannelList = GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_LIST);
				int nSelected = GetManualChannelListSelected(hWndManualChannelList, &nPMTIndex);
				
				if (nSelected == LB_ERR)
					MessageBox(hDlg, "Please select a channel to edit first", gszAppName, MB_ICONSTOP);
				else
				{
					memcpy(&v->editpmt, &v->pat.pmt[nPMTIndex], sizeof(PMT));
					DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_MANUAL_CHANNEL_EDIT), hDlg, ManualChannelEditDialogProc);
					ListView_RedrawItems(hWndManualChannelList, 0, ListView_GetItemCount(hWndManualChannelList));
				}
			}
			break;
		case IDC_DELETE_MANUAL_CHANNEL:
			{
				int nInPMTIndex, nOutPMTIndex = 0;
				int nDeletePMTIndex;
				HWND hWndManualChannelList = GetDlgItem(hDlg, IDC_MANUAL_CHANNEL_LIST);
				int nSelected = GetManualChannelListSelected(hWndManualChannelList, &nDeletePMTIndex);
				PPMT newpmt = LocalAlloc(LPTR, sizeof(PMT) * MAX_PAT_ENTRIES);

				if (nSelected == LB_ERR)
					MessageBox(hDlg, "Please select the channel to delete first", gszAppName, MB_ICONSTOP);

				for (nInPMTIndex = 0; nInPMTIndex < MAX_PAT_ENTRIES; nInPMTIndex++)
				{
					if (v->pat.pmt[nInPMTIndex].nPMTPID == 0)
						break;
					if (nInPMTIndex != nDeletePMTIndex)
						memcpy(&newpmt[nOutPMTIndex++], &v->pat.pmt[nInPMTIndex], sizeof(PMT));
					else
					{
						ListView_DeleteItem(hWndManualChannelList, nSelected);
						TreeView_DeleteItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), v->pat.pmt[nInPMTIndex].hPMTTreeItem);
					}
				}
				memcpy(&v->pat.pmt[0], newpmt, sizeof(PMT) * MAX_PAT_ENTRIES);
				LocalFree(newpmt);
				v->fDirtyManualChannels = TRUE;
			}
			break;
		}
		break;
	}

	return FALSE;
}
#endif LITE

void DefineManualChannels(HWND hDlg)
{
#ifdef LITE
	MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
	DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_MANUAL_CHANNELS), hDlg, ManualChannelsDialogProc);
#endif LITE
}

#ifndef LITE
BOOL LoadManualChannels(HWND hWnd, char * szInputFile)
{
	HANDLE hInputFile = CreateFile(szInputFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hInputFile != INVALID_HANDLE_VALUE)
	{
		HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);

		do
		{
			int nESIndex, nPMTIndex;
			char szInputLine[MAX_PATH];
			PMT newpmt;

			memset(&newpmt, 0, sizeof(PMT));

			if (SourceHelper_ReadLine(hInputFile, szInputLine, sizeof(szInputLine)) == 0)
				break;
			sscanf(szInputLine, "%d,%d", &newpmt.nProgramNumber, &newpmt.nPCRPID);
			nESIndex = 0;
			do
			{
				if (SourceHelper_ReadLine(hInputFile, szInputLine, sizeof(szInputLine)) == 0)
					break;
				if (szInputLine[0] == '-')
					break;
				sscanf(szInputLine, "%d,%d", &newpmt.es[nESIndex].nStreamType, &newpmt.es[nESIndex].nESPID);
				nESIndex++;
			} while (TRUE);

			// Make sure this program doesn't already exist
			for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
			{
				if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
				{
					char szTemp[128];
					char szMask[128];

					if (v->pat.hPATTreeItem == NULL)
					{
						wsprintf(szMask, "PAT PID %s", v->szOutputPIDFlags);
						wsprintf(szTemp, szMask, 0);
						v->pat.hPATTreeItem = AddItemToSITree(hWndTV, szTemp, 1, SI_PARSER_PAT, 0, NULL, TVI_FIRST);
					}

					newpmt.nPMTPID = -2;
					memcpy(&v->pat.pmt[nPMTIndex], &newpmt, sizeof(PMT));
					wsprintf(szTemp, "Manual - Program %d", v->pat.pmt[nPMTIndex].nProgramNumber);
					v->pat.pmt[nPMTIndex].hPMTTreeItem = AddItemToSITree(hWndTV, szTemp, 2, SI_PARSER_PMT + nPMTIndex, 1, v->pat.hPATTreeItem, NULL);
#ifdef DEBUG_MESSAGES
					{
						char szTemp2[128];
						wsprintf(szTemp2, "TSReader: PMT for %s item #%d\n", szTemp, nPMTIndex);
						OutputDebugString(szTemp2);
					}
#endif DEBUG_MESSAGES
					PostMessage(v->hDlgSIParser, WM_USER + 2, SI_PARSER_PMT, nPMTIndex);
					break;
				}
				if (v->pat.pmt[nPMTIndex].nProgramNumber == newpmt.nProgramNumber)
				{
					char szTemp[128];
					wsprintf(szTemp, "Program %d already exists - manual channel skipped", newpmt.nProgramNumber);
					MessageBox(hWnd, szTemp, gszAppName, MB_ICONWARNING);
					break;
				}
			}

		} while (TRUE);
		CloseHandle(hInputFile);

		if (v->pat.hPATTreeItem != NULL)
			TreeView_Expand(hWndTV, v->pat.hPATTreeItem, TVE_EXPAND);
	}
	else
		return FALSE;

	return TRUE;
}

#endif LITE

void LoadManualChannelsMenu(HWND hDlg)
{
#ifdef LITE
	MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
	OPENFILENAME ofn;
	char szInputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szInputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Manual Channel Files(*.tmc)\0*.tmc\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Load Manual Channels");
	ofn.lpstrDefExt = TEXT("tmc");
	ofn.Flags =  OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = v->szManualChannelsInitialDir;
	
	if (SourceHelper_myGetOpenFileName(&ofn) == TRUE)
	{
		if (LoadManualChannels(hDlg, szInputFile) == FALSE)
			MessageBox(hDlg, "Unable to open input file", gszAppName, MB_ICONSTOP);		
		v->fDirtyManualChannels = FALSE;
	}
#endif LITE
}

void ToggleMenuOption(HWND hDlg, int nMenuID, BOOL * fOption)
{
	*fOption = ~(*fOption) & 1;
	if (*fOption)
		CheckMenuItem(GetMenu(hDlg), nMenuID, MF_CHECKED | MF_BYCOMMAND);
	else
		CheckMenuItem(GetMenu(hDlg), nMenuID, MF_UNCHECKED | MF_BYCOMMAND);
}

void EnableDisablePIDChartButtons(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, IDC_SORT_DECENDING), !v->fPIDChartDisabled);
	EnableWindow(GetDlgItem(hDlg, IDC_SORT_RATE), !v->fPIDChartDisabled);
	EnableWindow(GetDlgItem(hDlg, IDC_SORT_PID), !v->fPIDChartDisabled);
}

void UpdateVLCConfigDisplay(HWND hDlg, int nOffset)
{
	SetDlgItemText(hDlg, IDC_VLC_CONFIG_DESCRIPTION, v->szVLCConfigDescription[nOffset]);
	SetDlgItemText(hDlg, IDC_VLC_CONFIG_COMMAND, v->szVLCConfigCommand[nOffset]);
	SetDlgItemInt(hDlg, IDC_VLC_CONFIG_NUMBER, nOffset + 1, FALSE);
}

void GetVLCConfig(HWND hDlg, int nOffset)
{
	GetDlgItemText(hDlg, IDC_VLC_CONFIG_DESCRIPTION, v->szVLCConfigDescription[nOffset], sizeof(v->szVLCConfigDescription[nOffset]));
	GetDlgItemText(hDlg, IDC_VLC_CONFIG_COMMAND, v->szVLCConfigCommand[nOffset], sizeof(v->szVLCConfigCommand[nOffset]));
}

void LoadSerialPortCombo(HWND hDlg)
{
	HWND hCombo = GetDlgItem(hDlg, IDC_SERIAL_CONTROL_PORT);
	HKEY hCU;
	int i;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
					 TEXT("Hardware\\Devicemap\\Serialcomm"),
					 0,
					 KEY_QUERY_VALUE,
					 &hCU) == ERROR_SUCCESS)
	{
		for (i = 0; ; i++)
		{
			LONG lResult;
			TCHAR szValueName[MAX_PATH];
			TCHAR szValue[MAX_PATH];
			DWORD dwValueName = sizeof(szValueName);
			DWORD dwValue = sizeof(szValue);
			DWORD dwType;

			// look at the values under HKEY_LOCAL_MACHINE\Hardware\Devicemap\Serialcomm
			lResult = RegEnumValue(hCU,
				                   i,
								   szValueName,
								   &dwValueName,
								   NULL,
								   &dwType,
								   szValue,
								   &dwValue);
			if (lResult != ERROR_SUCCESS)
				break;
			else
			{
				int nIndex = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)szValue);
				if (lstrcmp(szValue, v->szSerialReceiverPort) == 0)
					SendMessage(hCombo, CB_SETCURSEL, nIndex, 0);
			}
		}
		RegCloseKey(hCU);
	}
}

#ifndef LITE
void InitSerialControlDialog(HWND hDlg)
{
	int nIndex;

	CheckDlgButton(hDlg, IDC_SERIAL_CONTROL_ENABLED, v->ss.fSerialReceiverControlEnabled);
	LoadSerialPortCombo(hDlg);

	for (nIndex = 0; nIndex < v->nSerialReceiverControlIndex; nIndex++)
	{
		int nItem;
		char * szReceiverName;
		typedef char * (* td_GetReceiverName) ();
		char * (* GetReceiverName) ();
	
		GetReceiverName = (td_GetReceiverName)GetProcAddress(v->hSerialReceiverControl[nIndex], "GetReceiverName");
		if (GetReceiverName != NULL)
		{
			szReceiverName = GetReceiverName();
			nItem = SendDlgItemMessage(hDlg, IDD_SERIAL_CONTROL_RX_TYPES, LB_ADDSTRING, 0, (LPARAM)szReceiverName);
			if (lstrcmp(v->szSerialReceiverType, szReceiverName) == 0)
				SendDlgItemMessage(hDlg, IDD_SERIAL_CONTROL_RX_TYPES, LB_SETCURSEL, nItem, 0);
		}
	}
}

BOOL CALLBACK SerialControlDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		InitSerialControlDialog(hDlg);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_DESTROY:
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nIndex = SendDlgItemMessage(hDlg, IDC_SERIAL_CONTROL_PORT, CB_GETCURSEL, 0, 0);
				if (nIndex == CB_ERR)
				{
					MessageBox(hDlg, "Please select a serial port to use", gszAppName, MB_ICONSTOP);
					break;
				}
				SendDlgItemMessage(hDlg, IDC_SERIAL_CONTROL_PORT, CB_GETLBTEXT, nIndex, (LPARAM)v->szSerialReceiverPort);
				if ((v->ss.fSerialReceiverControlEnabled == FALSE) && (IsDlgButtonChecked(hDlg, IDC_SERIAL_CONTROL_ENABLED)))
					MessageBox(hDlg, "You need to close and restart TSReader for this change to take effect.", gszAppName, MB_ICONINFORMATION);
				v->ss.fSerialReceiverControlEnabled = IsDlgButtonChecked(hDlg, IDC_SERIAL_CONTROL_ENABLED);
				nIndex = SendDlgItemMessage(hDlg, IDD_SERIAL_CONTROL_RX_TYPES, LB_GETCURSEL, 0, 0);
				if (nIndex == LB_ERR)
				{
					MessageBox(hDlg, "Please select a receiver to use", gszAppName, MB_ICONSTOP);
					break;
				}
				SendDlgItemMessage(hDlg, IDD_SERIAL_CONTROL_RX_TYPES, LB_GETTEXT, nIndex, (LPARAM)v->szSerialReceiverType);
				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}
#endif LITE

void VLCSettings_WM_COMMAND(HWND hDlg, WPARAM wParam, LPARAM lParam, BOOL fFromProfileEditor)
{
	switch(LOWORD(wParam))
	{
	case IDC_VLC_SETTINGS_DEFAULT:
		{
			char szVLCProgram[] = {"C:\\Program Files\\VideoLAN\\VLC\\vlc.exe"};
			SetDlgItemText(hDlg, IDC_VLC_EXE_LOCATION, szVLCProgram);
			CheckDlgButton(hDlg, IDC_VLC_CONTROL_ENABLE, BST_CHECKED);
			SetDlgItemInt(hDlg, IDC_VLC_PORT, 1234, FALSE);
			CheckDlgButton(hDlg, IDC_VLC_CONTROL_SEND_LENGTH, BST_UNCHECKED);
		}
		break;
	case IDC_VLC_CONFIG_UP:
		GetVLCConfig(hDlg, v->nCurrentVLCEditConfig);
		v->nCurrentVLCEditConfig++;
		if (v->nCurrentVLCEditConfig > MAX_VLC_CONFIGURATIONS - 1)
			v->nCurrentVLCEditConfig = 0;
		UpdateVLCConfigDisplay(hDlg, v->nCurrentVLCEditConfig);
		break;
	case IDC_VLC_CONFIG_DOWN:
		GetVLCConfig(hDlg, v->nCurrentVLCEditConfig);
		v->nCurrentVLCEditConfig--;
		if (v->nCurrentVLCEditConfig < 0)
			v->nCurrentVLCEditConfig = MAX_VLC_CONFIGURATIONS - 1;
		UpdateVLCConfigDisplay(hDlg, v->nCurrentVLCEditConfig);
		break;
	case IDCANCEL:
		EndDialog(hDlg, FALSE);
		break;
	case IDOK:
		{
			int i;

			GetVLCConfig(hDlg, v->nCurrentVLCEditConfig);
			GetDlgItemText(hDlg, IDC_VLC_EXE_LOCATION, v->szVLCExeLocation, sizeof(v->szVLCExeLocation));
			v->fVLCControl = IsDlgButtonChecked(hDlg, IDC_VLC_CONTROL_ENABLE);
			v->fSendBogusHTTPSize = IsDlgButtonChecked(hDlg, IDC_VLC_CONTROL_SEND_LENGTH);
			v->nVLCPort = GetDlgItemInt(hDlg, IDC_VLC_PORT, NULL, FALSE);

			if (!fFromProfileEditor)
			{
				for (i = 0; i < MAX_VLC_CONFIGURATIONS; i++)
				{
					if (lstrlen(v->szVLCConfigCommand[i]))
					{
						if (strstr(v->szVLCConfigCommand[i], "<IP>") == NULL)
							break;
					}
				}
				if (i < MAX_VLC_CONFIGURATIONS)
					MessageBox(hDlg, "Warning: One or more of the VLC commands don't have the required <IP> tag. When these configurations\nare played, the resulting VLC session will playback only.", gszAppName, MB_ICONINFORMATION);

				EndDialog(hDlg, TRUE);
			}
		}
		break;
	case IDC_VLC_BROWSE:
		{
			OPENFILENAME ofn;
			char szInitialDir[MAX_PATH] = {0};

			memset( &(ofn), 0, sizeof(ofn));
			ofn.lStructSize	= sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = v->szVLCExeLocation;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = TEXT("Executables (*.exe)\0*.exe\0\0");	
			ofn.lpstrTitle = TEXT("Locate VLC.EXE");
			ofn.lpstrDefExt = TEXT("exe");
			ofn.lpstrInitialDir = szInitialDir;
			ofn.Flags =  OFN_HIDEREADONLY | OFN_EXPLORER;
			ofn.hInstance = v->hInstance;						
			if (SourceHelper_myGetOpenFileName(&ofn) == TRUE)
				SetDlgItemText(hDlg, IDC_VLC_EXE_LOCATION, v->szVLCExeLocation);
			break;
		}
		break;
	}
}

BOOL CALLBACK VLCSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_VLC_EXE_LOCATION, v->szVLCExeLocation);
		CheckDlgButton(hDlg, IDC_VLC_CONTROL_ENABLE, v->fVLCControl);
		CheckDlgButton(hDlg, IDC_VLC_CONTROL_SEND_LENGTH, v->fSendBogusHTTPSize);
		SetDlgItemInt(hDlg, IDC_VLC_PORT, v->nVLCPort, FALSE);
		v->nCurrentVLCEditConfig = 0;
		UpdateVLCConfigDisplay(hDlg, v->nCurrentVLCEditConfig);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_DESTROY:
		SetupVLCMenuNames(GetParent(hDlg));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		VLCSettings_WM_COMMAND(hDlg, wParam, lParam, FALSE);
		break;
	}
	
	return FALSE;
}


BOOL CALLBACK XNSServerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_XNS_SERVER_PORT, v->nXNSServerPort, FALSE);
		SendDlgItemMessage(hDlg, IDC_XNS_SERVER_PORT, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_XNS_SERVER_PORT));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			v->nXNSServerPort = GetDlgItemInt(hDlg, IDC_XNS_SERVER_PORT, NULL, FALSE);
			EndDialog(hDlg, TRUE);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

BOOL CALLBACK VLCWarningDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			v->fVLCControl = TRUE;
			EndDialog(hDlg, TRUE);
			break;
		case IDC_VLC_NO_WARNING:
			v->fVLCNoWarn = IsDlgButtonChecked(hDlg, IDC_VLC_NO_WARNING);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

BOOL CALLBACK PIDWarningDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			EndDialog(hDlg, TRUE);
			break;
		case IDC_NO_MORE_PID_WARNING:
			v->fIgnoreRecordAllPIDLimitationWarning = IsDlgButtonChecked(hDlg, IDC_NO_MORE_PID_WARNING);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

void LoadPlugins(HWND hWnd)
{
	static BOOL fCompleted = FALSE;

	if (fCompleted)
		return;
	fCompleted = TRUE;
	
	if (MD__Load_External_Dll(v->hInstance) > 0)
	{
		MD__StartPluginsRunning(v->hInstance, hWnd);
		v->fMDPluginsLoaded = TRUE;
	}
	else
	{
		HMENU hPlugInsMenu;
		HMENU hTopMenu = GetMenu(hWnd);
		int nMenuIndex = 5;		// for TSReader Lite and TSReader

#ifdef PRO
		nMenuIndex = 6;		// for TSReader Pro
#endif PRO
		hPlugInsMenu = GetSubMenu(hTopMenu, nMenuIndex);
		DeleteMenu(hPlugInsMenu, ID_PLUGINS_SOMETHING, MF_BYCOMMAND);	// remove the "something" placeholder
		v->fMDPluginsLoaded = FALSE;
	}
}

void CreateTooltip(HWND hWnd)
{
	DWORD dwStyle;

	dwStyle = WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP;

	v->hWndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, dwStyle,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, v->hInstance, NULL);
    SetWindowPos(v->hWndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void ResizeStatusbar(HWND hWndST)
{
	RECT rect;
	int nParts[7];
	int nIconExtra = 0;

	GetClientRect(hWndST, &rect);
	if (v->fAllowResizing)
		rect.right -= 28;
	nParts[0] = ((rect.right / 20) * 9);					// main part
	nParts[1] = ((rect.right / 20) * 5)  + nParts[0];		// record status
	nIconExtra = 24;
#ifndef PRO
	nParts[2] = rect.right - nIconExtra - 24 - 24;			// profile
#else PRO
	nParts[2] = rect.right - nIconExtra - 24 - 24 - 24;			// profile
#endif PRO
	nParts[3] = 24 + nParts[2];								// ?? icon
	nParts[4] = 24 + nParts[3];								// record icon
#ifndef PRO
	nParts[5] = -1;											// activity icon
	SendMessage(hWndST, SB_SETPARTS, (WPARAM)6, (LPARAM)(LPINT)&nParts);
#else PRO
	nParts[5] = 24 + nParts[4];								// record icon
	nParts[6] = -1;											// activity icon
	SendMessage(hWndST, SB_SETPARTS, (WPARAM)7, (LPARAM)(LPINT)&nParts);
#endif PRO
}

void CreateStatusBar(HWND hWnd)
{
	DWORD dwStyle;
	int nIndex = 0;

	// Load status bar icons
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_BLANK), IMAGE_ICON, 16, 16, 0);				//0
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_RUN_1), IMAGE_ICON, 16, 16, 0);				//1
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_RUN_2), IMAGE_ICON, 16, 16, 0);				//2
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_RUN_3), IMAGE_ICON, 16, 16, 0);				//3
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_RUN_4), IMAGE_ICON, 16, 16, 0);				//4
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_RECORD), IMAGE_ICON, 16, 16, 0);				//5
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_STREAM), IMAGE_ICON, 16, 16, 0);				//6
#ifdef PRO
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_FWD_1), IMAGE_ICON, 16, 16, 0);				//7
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_FWD_2), IMAGE_ICON, 16, 16, 0);				//8
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_FWD_3), IMAGE_ICON, 16, 16, 0);				//9
	v->hStatusBarIcons[nIndex++] = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_FWD_4), IMAGE_ICON, 16, 16, 0);				//10
#endif PRO

	//Create the status bar
	dwStyle = WS_CHILD | WS_VISIBLE | SBT_TOOLTIPS;
	v->hWndST = CreateStatusWindow(dwStyle, TEXT(""), hWnd, 2);
	if (v->hWndST != NULL)
	{
		SendMessage(v->hWndST, SB_SETICON, 4, (LPARAM)v->hStatusBarIcons[0]);
		SendMessage(v->hWndST, SB_SETICON, 5, (LPARAM)v->hStatusBarIcons[0]);
		
		SendMessage(v->hWndST, SB_SETTIPTEXT, 0, (LPARAM)"Status");
		SendMessage(v->hWndST, SB_SETTIPTEXT, 1, (LPARAM)"Record Output");
		SendMessage(v->hWndST, SB_SETTIPTEXT, 2, (LPARAM)"Decoder State");
		SendMessage(v->hWndST, SB_SETTIPTEXT, 4, (LPARAM)"Record/Stream Indicator");
		SendMessage(v->hWndST, SB_SETTIPTEXT, 5, (LPARAM)"Input Data Indicator");
#ifdef PRO
		SendMessage(v->hWndST, SB_SETTIPTEXT, 6, (LPARAM)"Forwarder Data Indicator");
#endif PRO

		SendMessage(v->hWndST, SB_SIMPLE, (WPARAM)FALSE, 0);
		ResizeStatusbar(v->hWndST);
	}
}

void DeleteStatusBar(HWND hWnd)
{
	int i;

	for (i = 0; v->hStatusBarIcons[i] != NULL; i++)
		DestroyIcon(v->hStatusBarIcons[i]);

	if (v->hWndST != NULL)
		DestroyWindow(v->hWndST);

}

#ifdef PRO
BOOL CALLBACK FwdSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_FWD_188, (v->nPacketSizeFlag == FORWARDER_PACKET_188));
		CheckDlgButton(hDlg, IDC_FWD_188_16, (v->nPacketSizeFlag == FORWARDER_PACKET_204));
		CheckDlgButton(hDlg, IDC_FWD_204, (v->nPacketSizeFlag == FORWARDER_PACKET_204_RS));	
		CheckDlgButton(hDlg, IDC_FWD_SYNC_INVERT, (v->nPacketOptions & FORWARDER_PACKET_SYNCINVERT));
		CheckDlgButton(hDlg, IDC_FWD_SYNC_RANDOMIZER, (v->nPacketOptions & FORWARDER_PACKET_RANDOMIZER));
		CheckDlgButton(hDlg, IDC_FWD_SYNC_INTERLEAVER, (v->nPacketOptions & FORWARDER_PACKET_INTERLEAVE));		
		EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_INVERT), (v->nPacketSizeFlag == FORWARDER_PACKET_204_RS));
		EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_RANDOMIZER), (v->nPacketSizeFlag == FORWARDER_PACKET_204_RS));
		EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_INTERLEAVER), (v->nPacketSizeFlag == FORWARDER_PACKET_204_RS));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			if (IsDlgButtonChecked(hDlg, IDC_FWD_188))
				v->nPacketSizeFlag = FORWARDER_PACKET_188;
			else if (IsDlgButtonChecked(hDlg, IDC_FWD_188_16))
				v->nPacketSizeFlag = FORWARDER_PACKET_204;
			else if (IsDlgButtonChecked(hDlg, IDC_FWD_204))
				v->nPacketSizeFlag = FORWARDER_PACKET_204_RS;
			v->nPacketOptions = 0;
			if (IsDlgButtonChecked(hDlg, IDC_FWD_SYNC_INVERT))
				v->nPacketOptions |= FORWARDER_PACKET_SYNCINVERT;
			if (IsDlgButtonChecked(hDlg, IDC_FWD_SYNC_RANDOMIZER))
				v->nPacketOptions |= FORWARDER_PACKET_RANDOMIZER;
			if (IsDlgButtonChecked(hDlg, IDC_FWD_SYNC_INTERLEAVER))
				v->nPacketOptions |= FORWARDER_PACKET_INTERLEAVE;
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_FWD_188:
		case IDC_FWD_188_16:
			EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_INVERT), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_RANDOMIZER), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_INTERLEAVER), FALSE);
			break;
		case IDC_FWD_204:
			EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_INVERT), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_RANDOMIZER), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_FWD_SYNC_INTERLEAVER), TRUE);
			break;
		}
		break;
	}
	return FALSE;
}
#endif PRO

BOOL CALLBACK RokuHD1000Settings(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_ROKU_IP, v->szRokuIP);
		SetDlgItemText(hDlg, IDC_ROKU_USERNAME, v->szRokuUsername);
		SetDlgItemText(hDlg, IDC_ROKU_PASSWORD, v->szRokuPassword);
		SetDlgItemText(hDlg, IDC_ROKU_MPEGPSPLAY_LOCATION, v->szRokuMpegPSPlayLocation);
		CheckDlgButton(hDlg, IDC_ROKU_TRACE_DISABLED, v->fRokuTraceDisabled);
		SendDlgItemMessage(hDlg, IDC_ROKU_IP, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_ROKU_IP));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_ROKU_IP, v->szRokuIP, sizeof(v->szRokuIP));
			GetDlgItemText(hDlg, IDC_ROKU_USERNAME, v->szRokuUsername, sizeof(v->szRokuUsername));
			GetDlgItemText(hDlg, IDC_ROKU_PASSWORD, v->szRokuPassword, sizeof(v->szRokuPassword));
			GetDlgItemText(hDlg, IDC_ROKU_MPEGPSPLAY_LOCATION, v->szRokuMpegPSPlayLocation, sizeof(v->szRokuMpegPSPlayLocation));
			v->fRokuTraceDisabled = IsDlgButtonChecked(hDlg, IDC_ROKU_TRACE_DISABLED);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}
	return FALSE;
}

#ifndef LITE
void GetDescriptorListDispInfo(LV_DISPINFO *pnmv) 
{ 
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nIndex = (int)(pnmv->item.iItem);
		
		switch(pnmv->item.iSubItem)
		{
		case 0:
			switch(nIndex)
			{
			case DESCRIPTOR_CAT:
				lstrcpy(pnmv->item.pszText, "MPEG-CAT");
				break;
			case DESCRIPTOR_PMT:
				lstrcpy(pnmv->item.pszText, "MPEG-PMT");
				break;
			case DESCRIPTOR_BAT:
				lstrcpy(pnmv->item.pszText, "DVB-BAT");
				break;
			case DESCRIPTOR_SDT:
				lstrcpy(pnmv->item.pszText, "DVB-SDT");
				break;
			case DESCRIPTOR_EIT:
				if (v->nNetworkPID == 0x0010)
					lstrcpy(pnmv->item.pszText, "DVB-EIT");
				else
					lstrcpy(pnmv->item.pszText, "ATSC-EIT");
				break;
			case DESCRIPTOR_NIT:
				if (v->nNetworkPID != 0x0ffe)
					lstrcpy(pnmv->item.pszText, "DVB-NIT");
				else
					lstrcpy(pnmv->item.pszText, "DCII-NIT");
				break;
			case DESCRIPTOR_TOT:
				lstrcpy(pnmv->item.pszText, "DVB-TOT");
				break;
			case DESCRIPTOR_VCT:
				lstrcpy(pnmv->item.pszText, "ATSC-VCT");
				break;
			case DESCRIPTOR_MGT:
				lstrcpy(pnmv->item.pszText, "ATSC-MGT");
				break;
			}
			break;
		case 1:
			{
				int nDescriptorTag;
				char szTemp[8];
				char szOutput[1024] = {0};

				for (nDescriptorTag = 0; nDescriptorTag < 256; nDescriptorTag++)
				{
					if (v->bDescriptorTagArray[nIndex][nDescriptorTag])
					{
						wsprintf(szTemp, "%02x ", nDescriptorTag);
						lstrcat(szOutput, szTemp);
					}
				}
				lstrcpy(pnmv->item.pszText, szOutput);
			}
			break;
		}
	}
}

BOOL CALLBACK DescriptorUsageDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hWndTableList = GetDlgItem(hDlg, IDC_DESCRIPTOR_LIST);
			int nColumnPosition = 0;
			int nItem;
			LV_COLUMN lvc; 
			char szTemp[128];

			// Setup list view columns. 
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			lvc.pszText = szTemp; 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 80; 
			lstrcpy(szTemp, TEXT("Table"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_LEFT; 
			lvc.cx = 400; 
			lstrcpy(szTemp, TEXT("Descriptors"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			ListView_SetExtendedListViewStyle(hWndTableList, LVS_EX_FULLROWSELECT);

			for (nItem = 0; nItem < MAX_DESCRIPTOR_TAG_ARRAY; nItem++)
			{
				LV_ITEM lvi; 

				memset(&lvi, 0, sizeof(lvi));
				lvi.state = 0; 
				lvi.stateMask = 0; 
				lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
				lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
				lvi.iItem = nItem; 
				lvi.iSubItem = 0; 
				lvi.lParam = (LPARAM)0;    // item data 
				ListView_InsertItem(hWndTableList, &lvi);
			}
			
			SetFocus(GetDlgItem(hDlg, IDCANCEL));
			SetTimer(hDlg, 1, 1000, NULL);
		}
		break;
	case WM_DESTROY:
		KillTimer(hDlg, 1);
		break;
	case WM_TIMER:
		ListView_RedrawItems(GetDlgItem(hDlg, IDC_DESCRIPTOR_LIST), 0, ListView_GetItemCount(GetDlgItem(hDlg, IDC_DESCRIPTOR_LIST)));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetDescriptorListDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
				{
					v->nSelectedDescriptorItem = pnmv->iItem;
				}
			}
			break;
		case NM_DBLCLK:
			{
				int nDescriptorTag;
				char szDescriptor[256];
				char szTemp[1024];
				char szOutput[10 * 1024] = {0};

				for (nDescriptorTag = 0; nDescriptorTag < 256; nDescriptorTag++)
				{
					if (v->bDescriptorTagArray[v->nSelectedDescriptorItem][nDescriptorTag])
					{
						DecodeDescriptorNames(szDescriptor, nDescriptorTag);
						wsprintf(szTemp, "%02x - %s\n", nDescriptorTag, szDescriptor);
						lstrcat(szOutput, szTemp);
					}
				}
				if (lstrlen(szOutput))
					MessageBox(hDlg, szOutput, gszAppName, MB_ICONINFORMATION);
			}
			break;
		}
		break;
	}

	return FALSE;
}
#endif LITE

BYTE * GetBitmapPtr(char * szResource, HDC hDC)
{
	int nWidth, nHeight;
	HBITMAP hBitmap;
	BYTE * pRetVal;

	hBitmap = _ISLoadResourceBitmap(v->hInstance, szResource, NULL);
	pRetVal = _ISHBITMAPToRGB(hBitmap, &nWidth, &nHeight, hDC, NULL);	
	DeleteObject(hBitmap);
	return pRetVal;
}

void SetupServiceBitmaps(HWND hDlg, BOOL fLoad)
{
	if (fLoad)
	{
		HDC hDC = GetDC(hDlg);

		v->pRGB_422 =  GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_422), hDC);
		v->pRGB_cc =   GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_CC), hDC);
		v->pRGB_itxt = GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_ITXT), hDC);
		v->pRGB_sub =  GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_SUB), hDC);
		v->pRGB_txt =  GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_TXT), hDC);
		v->pRGB_user = GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_USER), hDC);
		v->pRGB_vps =  GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_VPS), hDC);
		v->pRGB_wss =  GetBitmapPtr(MAKEINTRESOURCE(IDB_TXT_WSS), hDC);
		v->pRGB_dtvcc =  GetBitmapPtr(MAKEINTRESOURCE(IDB_ATSC_CC), hDC);
		v->pRGB_rc  =  GetBitmapPtr(MAKEINTRESOURCE(IDB_ATSC_RC), hDC);
		v->pRGB_4x3 = GetBitmapPtr(MAKEINTRESOURCE(IDB_ASPECT_43), hDC);
		v->pRGB_14x9 = GetBitmapPtr(MAKEINTRESOURCE(IDB_ASPECT_149), hDC);
		v->pRGB_16x9 = GetBitmapPtr(MAKEINTRESOURCE(IDB_ASPECT_169), hDC);
		v->pRGB_AFD =  GetBitmapPtr(MAKEINTRESOURCE(IDB_ASPECT_AFD), hDC);

		v->pRGB_MPEGAudio = GetBitmapPtr(MAKEINTRESOURCE(IDB_AUD_MPEG), hDC);
		v->pRGB_AC3Audio = GetBitmapPtr(MAKEINTRESOURCE(IDB_AUD_AC3), hDC);
		v->pRGB_AACAudio = GetBitmapPtr(MAKEINTRESOURCE(IDB_AUD_AAC), hDC);
		v->pRGB_MPG2Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_VID_MPEG2), hDC);
		v->pRGB_DCIIVideo = GetBitmapPtr(MAKEINTRESOURCE(IDB_VID_DCII), hDC);
		v->pRGB_MPG4Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_VID_MPEG4), hDC);
		v->pRGB_H264Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_VID_H264), hDC);
#ifdef PRO
		v->pRGB_VC1Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_VID_VC1), hDC);
#endif PRO
		v->pRGB_BL_MPEGAudio = GetBitmapPtr(MAKEINTRESOURCE(IDB_BAUD_MPEG), hDC);
		v->pRGB_BL_AC3Audio = GetBitmapPtr(MAKEINTRESOURCE(IDB_BAUD_AC3), hDC);
		v->pRGB_BL_AACAudio = GetBitmapPtr(MAKEINTRESOURCE(IDB_BAUD_AAC), hDC);
		v->pRGB_BL_MPG2Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_BVID_MPEG2), hDC);
		v->pRGB_BL_DCIIVideo = GetBitmapPtr(MAKEINTRESOURCE(IDB_BVID_DCII), hDC);
		v->pRGB_BL_MPG4Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_BVID_MPEG4), hDC);
		v->pRGB_BL_H264Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_BVID_H264), hDC);
#ifdef PRO
		v->pRGB_BL_VC1Video = GetBitmapPtr(MAKEINTRESOURCE(IDB_BVID_VC1), hDC);
#endif PRO

		ReleaseDC(hDlg, hDC);
	}
	else
	{
		GlobalFree(v->pRGB_txt);
		GlobalFree(v->pRGB_422);
		GlobalFree(v->pRGB_cc);
		GlobalFree(v->pRGB_itxt);
		GlobalFree(v->pRGB_sub);
		GlobalFree(v->pRGB_user);
		GlobalFree(v->pRGB_vps);
		GlobalFree(v->pRGB_wss);
		GlobalFree(v->pRGB_dtvcc);
		GlobalFree(v->pRGB_rc);
		GlobalFree(v->pRGB_4x3);
		GlobalFree(v->pRGB_14x9);
		GlobalFree(v->pRGB_16x9);
		GlobalFree(v->pRGB_AFD);
		GlobalFree(v->pRGB_MPEGAudio);
		GlobalFree(v->pRGB_AC3Audio);
		GlobalFree(v->pRGB_AACAudio);
		GlobalFree(v->pRGB_MPG2Video);
		GlobalFree(v->pRGB_DCIIVideo);
		GlobalFree(v->pRGB_MPG4Video);
		GlobalFree(v->pRGB_H264Video);
#ifdef PRO
		GlobalFree(v->pRGB_VC1Video);
#endif PRO
		GlobalFree(v->pRGB_BL_MPEGAudio);
		GlobalFree(v->pRGB_BL_AC3Audio);
		GlobalFree(v->pRGB_BL_AACAudio);
		GlobalFree(v->pRGB_BL_MPG2Video);
		GlobalFree(v->pRGB_BL_DCIIVideo);
		GlobalFree(v->pRGB_BL_MPG4Video);
		GlobalFree(v->pRGB_BL_H264Video);
#ifdef PRO
		GlobalFree(v->pRGB_BL_VC1Video);
#endif PRO
	}
}

void SetupChannelFont(HDC hDC)
{
	int nPointSize = 90;
	HFONT hOldObject, hTempTahoma;

	if (v->nThumbnailSize == 1)
		nPointSize = 70;

	memset(&v->logfontChannelFont, 0, sizeof(v->logfontChannelFont));
	v->logfontChannelFont.lfHeight =         -MulDiv(nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 720);
	v->logfontChannelFont.lfWidth =          0;
	v->logfontChannelFont.lfEscapement =     0;
	v->logfontChannelFont.lfOrientation =    0;
	if (v->nThumbnailSize == 1)
		v->logfontChannelFont.lfWeight =         500;
	else
		v->logfontChannelFont.lfWeight =         2000;
	v->logfontChannelFont.lfItalic =         FALSE;
	v->logfontChannelFont.lfUnderline =      FALSE;
	v->logfontChannelFont.lfStrikeOut =      0;
	v->logfontChannelFont.lfCharSet =        DEFAULT_CHARSET;
	v->logfontChannelFont.lfOutPrecision =   OUT_TT_ONLY_PRECIS;
	v->logfontChannelFont.lfClipPrecision =  CLIP_DEFAULT_PRECIS;
	v->logfontChannelFont.lfQuality =        ANTIALIASED_QUALITY;
	v->logfontChannelFont.lfPitchAndFamily = VARIABLE_PITCH ;
	lstrcpy(v->logfontChannelFont.lfFaceName, TEXT("Tahoma"));
	hTempTahoma = CreateFontIndirect(&v->logfontChannelFont);
	hOldObject = SelectObject(hDC, hTempTahoma);
	GetTextExtentPoint32(hDC, "X", 1, &v->textsizeChannelFont);	
	SelectObject(hDC, hOldObject);
	DeleteObject(hTempTahoma);
}

void SetupFonts(HWND hDlg)
{
	HDC hDC;

	hDC = GetDC(hDlg);
	v->hPIDFont = CreateFont(-MulDiv(7, GetDeviceCaps(hDC, LOGPIXELSY), 72),
							   0,
							   0,
							   0,
							   400,
							   FALSE,
							   FALSE,
							   FALSE,
							   ANSI_CHARSET,
							   OUT_DEFAULT_PRECIS,
							   CLIP_DEFAULT_PRECIS,
							   ANTIALIASED_QUALITY,
							   FF_DONTCARE | VARIABLE_PITCH,
							   "Arial");	
	v->hSourceInfoFont = CreateFont(-MulDiv(9, GetDeviceCaps(hDC, LOGPIXELSY), 72),
							   0,
							   0,
							   0,
							   400,
							   FALSE,
							   FALSE,
							   FALSE,
							   ANSI_CHARSET,
							   OUT_DEFAULT_PRECIS,
							   CLIP_DEFAULT_PRECIS,
							   ANTIALIASED_QUALITY,
							   FF_DONTCARE | VARIABLE_PITCH,
							   "Arial");	
	v->hCourierNew = CreateFont(-MulDiv(9, GetDeviceCaps(hDC, LOGPIXELSY), 72),
							   0,
							   0,
							   0,
							   500,
							   FALSE,
							   FALSE,
							   FALSE,
							   ANSI_CHARSET,
							   OUT_DEFAULT_PRECIS,
							   CLIP_DEFAULT_PRECIS,
							   DEFAULT_QUALITY,
							   FIXED_PITCH,
							   "Courier New");	
	SetupChannelFont(hDC);
	ReleaseDC(hDlg, hDC);
}

void MouseMoveTooltips(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int x = LOWORD(lParam);
	int y = HIWORD(lParam);	
	int xWidth, xStart, yStart, yEnd;
	TOOLINFO ti;
	RECT rcBorder, rcParent;
	char szTemp[128];

	if (v->fPIDChartDisabled == TRUE)
		return;

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hDlg;
	ti.hinst = v->hInstance;
	ti.uId = 0;

	// See if the mouse is inside the PID chart
	GetWindowRect(GetDlgItem(hDlg, IDC_PIDS_BORDER), &rcBorder);
	GetWindowRect(v->hWndMainWindow, &rcParent);
	xWidth = rcBorder.right - rcBorder.left - 22;
	xStart = rcBorder.left - rcParent.left + 2;
	yStart = rcBorder.top - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) + 12 + 10;
	yEnd = yStart + (rcBorder.bottom - rcBorder.top) - 10;

	if (x >= xStart && x <= xStart + xWidth && y >= yStart && y <= yEnd)
	{
		int i;
		int nPIDStart = 0;

		if (v->fSortChartByPID == FALSE)
		{
			nPIDStart = v->nPIDScrollOffset;
		}
		else
		{
			int nPIDActiveCounter = 0;
			for (i = 0; i < 8192; i++)
			{
				if (v->pc[i].lnPackets != 0)
				{
					if (nPIDActiveCounter == v->nPIDScrollOffset)
					{
						nPIDStart = i;
						break;
					}
					nPIDActiveCounter++;
				}
			}
		}
		for (i = nPIDStart; i < 8192; i++)
		{
			if (v->fSortChartByPID == FALSE)
			{
				if (v->pc[i].lnPackets == 0)
					break;
			}
			else
			{
				if (v->pc[i].lnPackets == 0)
					continue;
			}
			if (y >= yStart && y < yStart + 14)
			{
				ti.lpszText = szTemp;
				
				// Tooltip control will cover the whole window
				ti.rect.left = x;    
				ti.rect.top = y;
				ti.rect.right = x + 100;
				ti.rect.bottom = y + 100;

				GetPIDTooltipInfo(v->pc[i].nPID, szTemp);
				SendMessage(v->hWndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
				break;
			}
			yStart += 14;
		}
	}
	else
		SendMessage(v->hWndTT, TTM_DELTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
}

void AddTaskbarIcon(HWND hWnd)
{
	NOTIFYICONDATA tnid; 
	HICON hTrayIcon;

	hTrayIcon = LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVB_LOGO_SMALL_BLACK));
	tnid.cbSize = sizeof(NOTIFYICONDATA); 
	tnid.hWnd = hWnd; 
	tnid.uID = 1; 
	tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP; 
	tnid.uCallbackMessage = WM_USER + 8; 
	tnid.hIcon = hTrayIcon; 
	strcpy(tnid.szTip, gszAppName);
	Shell_NotifyIcon(NIM_ADD, &tnid); 
	v->fBalloonQueued = FALSE;
	DestroyIcon(hTrayIcon); 
}

void ResizeDialogWindow(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	int nVerticalSize = HIWORD(lParam);
	int hHorizontalSize = LOWORD(lParam);

	if (v->fBlockResizeMessage)
		return;

#ifndef LITE
	if (v->fAllowResizing)
	{
		{
			RECT rc;
			GetClientRect(v->hWndMainWindow, &rc);
			nVerticalSize = rc.bottom;
			hHorizontalSize = rc.right;
		}
		SetWindowPos(v->hDlgSIParser, NULL, 0, 0, hHorizontalSize, nVerticalSize, SWP_NOMOVE);

		if (wParam != SIZE_MINIMIZED)
		{
			int nVideoFrameWidth, nVideoFrameHeight;
			int nScrollbarTop, nScrollbarHeight, nScrollbarLeft, nScrollbarWidth;
			HWND hWndVideoFrame = GetDlgItem(v->hDlgSIParser, IDC_VIDEO_FRAME);
			HWND hWndVideoScroll = GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS);
			RECT rcVideoFrame, rcParent, rcVideoScrollClient;

			KillTimer(hDlg, 4);

			GetWindowRect(hWndVideoFrame, &rcVideoFrame);
			GetWindowRect(v->hWndMainWindow, &rcParent);
			GetClientRect(hWndVideoScroll, &rcVideoScrollClient);

			nVideoFrameWidth = rcParent.right - rcVideoFrame.left - 10;
			nVideoFrameHeight = rcParent.bottom - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) - 30;		
			SetWindowPos(hWndVideoFrame, NULL, 0, 0, nVideoFrameWidth, nVideoFrameHeight, SWP_NOMOVE);

			nScrollbarTop = 7;
			nScrollbarHeight = nVideoFrameHeight - 8;//rcParent.bottom - rcParent.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYCAPTION) - 30 - 7;		
			nScrollbarLeft = rcVideoFrame.right - rcVideoScrollClient.right - rcParent.left - 6;
			nScrollbarWidth = rcVideoScrollClient.right;
			SetWindowPos(hWndVideoScroll, NULL, nScrollbarLeft, nScrollbarTop, nScrollbarWidth, nScrollbarHeight, 0);

			{
				int nParentHeight;
				int nSITreeTop;
				int nPIDsBoxHeight;
				RECT rcSourceBox;
				RECT rcStatisticsBox;
				RECT rcParent;
				RECT rcSITree;
				RECT rcPIDsBox;
				RECT rcPIDsScroll;
				RECT rcStatisticsWindow;

				GetClientRect(hDlg, &rcParent);
				GetClientRect(GetDlgItem(hDlg, IDC_SOURCE_BOX), &rcSourceBox);
				GetClientRect(GetDlgItem(hDlg, IDC_STATISTICS_FRAME), &rcStatisticsBox);
				GetWindowRect(GetDlgItem(hDlg, IDC_STATISTICS_FRAME), &rcStatisticsWindow);
				GetClientRect(GetDlgItem(hDlg, IDC_PIDS_SCROLL), &rcPIDsScroll);
				GetWindowRect(GetDlgItem(hDlg, IDC_PIDS_BORDER), &rcPIDsBox);
				GetWindowRect(GetDlgItem(hDlg, IDC_SI_TREE), &rcSITree);
				
				nParentHeight = nVideoFrameHeight;
				SetWindowPos(GetDlgItem(hDlg, IDC_SOURCE_BOX), NULL, 0, nParentHeight - rcSourceBox.bottom, 0, 0, SWP_NOSIZE);
				SetWindowPos(GetDlgItem(hDlg, IDC_STATISTICS_FRAME), NULL,
					         rcSourceBox.right + 5, nParentHeight - rcStatisticsBox.bottom,
							 0, 0,
							 SWP_NOSIZE);
				
				nSITreeTop = nParentHeight - rcSourceBox.bottom - 2;
				SetWindowPos(GetDlgItem(hDlg, IDC_SI_TREE), NULL, 0, 0, rcSITree.right - rcSITree.left, nSITreeTop, SWP_NOMOVE);

				nPIDsBoxHeight = rcStatisticsWindow.top - (rcPIDsBox.top - GetSystemMetrics(SM_CYBORDER) + 3);
				SetWindowPos(GetDlgItem(hDlg, IDC_PIDS_BORDER), NULL,
					         0, 0, 
							 rcPIDsBox.right - rcPIDsBox.left, nPIDsBoxHeight,
							 SWP_NOMOVE);
				SetWindowPos(GetDlgItem(hDlg, IDC_PIDS_SCROLL), NULL,
					         0, 0,
							 rcPIDsScroll.right, nPIDsBoxHeight - 22,
							 SWP_NOMOVE);

			}
			SetTimer(hDlg, 4, 50, NULL);
		}

		if (wParam == SIZE_MAXIMIZED)
		{
			v->fCurrentMaximized = TRUE;
			PostMessage(hDlg, WM_SIZE, -1, MAKELONG(nVerticalSize, hHorizontalSize));
		}
		else if (wParam == SIZE_RESTORED && v->fCurrentMaximized == TRUE)
		{
			v->fCurrentMaximized = FALSE;
			PostMessage(hDlg, WM_SIZE, 0, MAKELONG(nVerticalSize, hHorizontalSize));
		}
		if (wParam == -1)
			PostMessage(hDlg, WM_USER + 3, 0, 0);
	}
#endif LITE
}

void SetupProcessorAffinity()
{
	// If source has CAPABILITIES_UNIPROCESSOR then we need to set our affinity
	// to one processor - otherwise all CPUs are OK
	DWORD dwProcessAffinityMask, dwSystemAffinityMask;
	BOOL fRetVal;
	HANDLE hMyProcess;

	hMyProcess = GetCurrentProcess();
	fRetVal = GetProcessAffinityMask(hMyProcess, &dwProcessAffinityMask, &dwSystemAffinityMask);
	if (fRetVal)
	{
		if (   (v->dwSourceCapabilities & CAPABILITIES_UNIPROCESSOR)
			|| (v->fUniprocessorMode) )
			dwProcessAffinityMask = 1;
		else
			dwProcessAffinityMask = dwSystemAffinityMask;
		fRetVal = SetProcessAffinityMask(hMyProcess, dwProcessAffinityMask);
	}
}

BOOL LoadSource(HWND hWnd)
{
	int i;
	char szSourceNameCopy[MAX_PATH];
	char szCurrentDir[MAX_PATH];

	// If Ctrl is down or we're using an obsolete source
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		v->szSourceName[0] = 0;
		v->ss.fSerialReceiverControlEnabled = FALSE;
	}

	if (strstr(v->szSourceName, "TSReader_TTBudget.dll") != NULL)
		v->szSourceName[0] = 0;
	if (strstr(v->szSourceName, "TSReader_Twinhan2.dll") != NULL)
		v->szSourceName[0] = 0;
	if (strstr(v->szSourceName, "TSReader_Twinhan.dll") != NULL)
		v->szSourceName[0] = 0;
	if (strstr(v->szSourceName, "TSReader_SkySeeker") != NULL)
		v->szSourceName[0] = 0;

	// Check to see if we're running TSReader from a different location
	if (lstrlen(v->szSourceName))
	{
		lstrcpy(szSourceNameCopy, v->szSourceName);
		for (i = lstrlen(szSourceNameCopy); i > 0; i--)
		{
			if (szSourceNameCopy[i] == '\\')
			{
				szSourceNameCopy[i] = 0;
				break;
			}
		}
		SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
		lstrcat(szCurrentDir, "\\Sources");
		if (lstrcmp(szCurrentDir, szSourceNameCopy) != 0)
		{
			// Switched to another location so reset the source
			char * szSourceFilenamePtr = v->szSourceName + lstrlen(v->szSourceName);
			do
			{
				if (*szSourceFilenamePtr == '\\')
					break;
			} while (szSourceFilenamePtr-- > v->szSourceName);
			lstrcat(szCurrentDir, szSourceFilenamePtr);
			lstrcpy(v->szSourceName, szCurrentDir);
		}
	}

	do
	{
		BOOL fSourceDialogOK;
		if (lstrlen(v->szSourceName))
		{
			v->hSource = LoadLibrary(v->szSourceName);
			if (v->hSource == NULL)
			{
				if (MessageBox(hWnd, "The source module can't be loaded. Please re-select your source on the following dialog.", gszAppName, MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
					return FALSE;
				v->szSourceName[0] = 0;
				continue;
			}
			Init = (td_Init)GetProcAddress(v->hSource, "TSReader_Init");
			DeInit = (td_DeInit)GetProcAddress(v->hSource, "TSReader_DeInit");
			Start = (td_Start)GetProcAddress(v->hSource, "TSReader_Start");
			Stop = (td_Stop)GetProcAddress(v->hSource, "TSReader_Stop");
			Tune = (td_Tune)GetProcAddress(v->hSource, "TSReader_Tune");
			TuneDialog = (td_TuneDialog)GetProcAddress(v->hSource, "TSReader_TuneDialog");
			GetDescription = (td_GetDescription)GetProcAddress(v->hSource, "TSReader_GetDescription");
			ParseSourceModuleCommandLine = (td_ParseCommandLine)GetProcAddress(v->hSource, "TSReader_ParseCommandLine");
			PIDManagement = (td_PIDManagement)GetProcAddress(v->hSource, "TSReader_PIDManagement");
			IsPIDActive = (td_IsPIDActive)GetProcAddress(v->hSource, "TSReader_IsPIDActive");
			GetTunerString = (td_GetTunerString)GetProcAddress(v->hSource, "TSReader_GetTunerString");
			GetSignalString = (td_GetSignalString)GetProcAddress(v->hSource, "TSReader_GetSignalString");
			GetSyncLossCount = (td_GetSyncLossCount)GetProcAddress(v->hSource, "TSReader_GetSyncLossCount");
			GetRetuneCount = (td_GetRetuneCount)GetProcAddress(v->hSource, "TSReader_GetRetuneCount");
			
			GetDescription(v->szSourceModuleDescription, NULL, &v->fSourceCanBeStopped, &v->nMaxSourcePIDs, &v->dwSourceCapabilities);
			SetupProcessorAffinity();
			{
				char szTemp[128 + MAX_PATH];
				wsprintf(szTemp, "TSReader: Source \"%s\"\n", v->szSourceName);
				OutputDebugString(szTemp);
			}

			return TRUE;
		}
		fSourceDialogOK = DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_SOURCE), hWnd, SourceDlgProc);
		if (fSourceDialogOK == FALSE)
			return FALSE;
	} while (TRUE);

	return FALSE;			// should never get here
}

void EnableDisableSourceMenuItems(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);

	if (v->fSourceCanBeStopped == FALSE)
		EnableMenuItem(hMenu, ID_FILE_STOPSOURCE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	else
		EnableMenuItem(hMenu, ID_FILE_STOPSOURCE, MF_ENABLED | MF_BYCOMMAND);

	if ((v->dwSourceCapabilities & CAPABILITIES_DISEQC_POSITIONER) == 0)
		EnableMenuItem(hMenu, ID_FILE_DISEQCPOSITIONER, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	else
		EnableMenuItem(hMenu, ID_FILE_DISEQCPOSITIONER, MF_ENABLED | MF_BYCOMMAND);

#ifndef LITE
	if ((v->dwSourceCapabilities & CAPABILITIES_TIMESTAMP) == 0)
	{
		EnableMenuItem(hMenu, ID_SETTINGS_TIMESTAMPPACKETS, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
		v->ss.fTimestampPackets = FALSE;
	}
	else
	{
		if (v->ss.fTimestampPackets)
			CheckMenuItem(hMenu, ID_SETTINGS_TIMESTAMPPACKETS, MF_CHECKED | MF_BYCOMMAND);		
	}
#else LITE
	EnableMenuItem(hMenu, ID_SETTINGS_TIMESTAMPPACKETS, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	v->ss.fTimestampPackets = FALSE;
#endif LITE

	if ((v->dwSourceCapabilities & CAPABILITIES_CI_CAM) == 0)
		EnableMenuItem(hMenu, ID_FILE_CICAMMENU, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
	else
		EnableMenuItem(hMenu, ID_FILE_CICAMMENU, MF_ENABLED | MF_BYCOMMAND);
}

#ifndef LITE

BOOL CALLBACK IPDeviceDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int i;
			int nCount = 0;

			for (i = 0; ; i++)
			{
				int nIndex;
				char * szFullNamePtr;
				char * szAbreviated;
				char szDeviceName[MAX_PATH];
				char szDeviceDescription[MAX_PATH];

				if (UDPSender_GetDevices(i, szDeviceName, szDeviceDescription) != TRUE)
					break;

				szAbreviated = strstr(szDeviceDescription, "'");
				if (szAbreviated == NULL)
					szAbreviated = szDeviceDescription;
				nIndex = SendDlgItemMessage(hDlg, IDC_IP_DEVICE_LIST, LB_ADDSTRING, 0, (LPARAM)szAbreviated);
				szFullNamePtr = LocalAlloc(LPTR, lstrlen(szDeviceName) + 1);
				lstrcpy(szFullNamePtr, szDeviceName);
				SendDlgItemMessage(hDlg, IDC_IP_DEVICE_LIST, LB_SETITEMDATA, nIndex, (LPARAM)szFullNamePtr);
				nCount++;
				{
					char szTemp[512];
					wsprintf(szTemp, "TSReader: %s\nTSReader: %s\n", szDeviceDescription, szDeviceName);
					OutputDebugString(szTemp);
				}
			}
			if (nCount == 0)
			{
				MessageBox(hDlg, "No compatible network adapters found - unable to continue", gszAppName, MB_ICONSTOP);
				EndDialog(hDlg, FALSE);
			}
		}
		break;
	case WM_DESTROY:
		{
			int i;
			int nCount = SendDlgItemMessage(hDlg, IDC_IP_DEVICE_LIST, LB_GETCOUNT, 0, 0);
			for (i = 0; i < nCount; i++)
			{
				char * szFullNamePtr;
				szFullNamePtr = (char *)SendDlgItemMessage(hDlg, IDC_IP_DEVICE_LIST, LB_GETITEMDATA, i, 0);
				LocalFree(szFullNamePtr);
			}
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case BN_CLICKED:
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					int nSelected = SendDlgItemMessage(hDlg, IDC_IP_DEVICE_LIST, LB_GETCURSEL, 0, 0);
					if (nSelected != LB_ERR)
					{
						char * szFullNamePtr = (char *)SendDlgItemMessage(hDlg, IDC_IP_DEVICE_LIST, LB_GETITEMDATA, nSelected, 0);
						if (UDPSender_OpenDevice(szFullNamePtr) == TRUE)
							EndDialog(hDlg, TRUE);
						else
						{
							MessageBox(hDlg, "Unable to open output device", gszAppName, MB_ICONSTOP);
							EndDialog(hDlg, FALSE);
						}
					}
					else
						MessageBeep(0);
				}
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			}
			break;
		case LBN_DBLCLK:
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_IP_DEVICE_LIST))
				PostMessage(hDlg, WM_COMMAND, IDOK, 0);
			break;
		}
		break;
	}

	return FALSE;
}

void SetTreeViewIcon(HTREEITEM hTreeItem, int nIcon)
{
	TVITEM tvitem;

	memset(&tvitem, 0, sizeof(tvitem));
	tvitem.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	tvitem.hItem = hTreeItem;
	tvitem.iImage = tvitem.iSelectedImage = nIcon;
	TreeView_SetItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), &tvitem);
}

BOOL CheckWinPcap(HWND hDlg)
{
	if (v->hUDPSender == NULL)
	{
		if (MessageBox(hDlg, "WinPcap is required for this function and has not been installed.\n\nWould you like to go to the WinPcap homepage to download it?", gszAppName, MB_ICONSTOP | MB_YESNO) == IDYES)
			ShellExecute(NULL, "open", "http://winpcap.polito.it/", NULL, NULL, SW_SHOW);
		return FALSE;
	}
	return TRUE;
}

void ReTransmitSelectedIPStream(HWND hDlg, BOOL fStartTransmitting)
{
	if (CheckWinPcap(hDlg) == FALSE)
		return;

	if (fStartTransmitting == FALSE)
	{
		v->pLastClickedIPEntry->fTransmitting = FALSE;
		v->nTransmittingCount--;
		SetTreeViewIcon(v->pLastClickedIPEntry->hIPItem, 11);
		if (v->nTransmittingCount == 0)
			UDPSender_CloseDevice();
	}
	else
	{
		if (v->pLastClickedIPEntry->dwProtocol != IP_UDP_ID)
			return;		// needs to be UDP/IP

		if (v->nTransmittingCount == 0)
		{
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_IP_DEVICE), hDlg, IPDeviceDlgProc) == FALSE)
				return;
		}
		v->pLastClickedIPEntry->fTransmitting = TRUE;
		v->nTransmittingCount++;						
		SetTreeViewIcon(v->pLastClickedIPEntry->hIPItem, 21);
	}
}

void ReTransmitSelectedIPMACStream(HWND hDlg, BOOL fStartTransmitting)
{
	if (CheckWinPcap(hDlg) == FALSE)
		return;

	if (v->pLastClickedIPMACEntry != NULL)
	{
		PIPENTRY pCurrent = v->pLastClickedIPMACEntry->pIPEntries;
		while (pCurrent != NULL)
		{
			v->pLastClickedIPEntry = pCurrent;
			ReTransmitSelectedIPStream(hDlg, fStartTransmitting);
			pCurrent = (PIPENTRY)pCurrent->dwNext;
		}
		v->pLastClickedIPEntry = NULL;
	}

}

void ReTransmitSelectedIPPIDStream(HWND hDlg, BOOL fStartTransmitting)
{
	if (CheckWinPcap(hDlg) == FALSE)
		return;

	if (v->pLastClickedIPPIDEntry != NULL)
	{
		PIPMACENTRY pCurrentMAC = v->pLastClickedIPPIDEntry;
		while (pCurrentMAC != NULL)
		{
			PIPENTRY pCurrent = pCurrentMAC->pIPEntries;
			while (pCurrent != NULL)
			{
				v->pLastClickedIPEntry = pCurrent;
				ReTransmitSelectedIPStream(hDlg, fStartTransmitting);
				pCurrent = (PIPENTRY)pCurrent->dwNext;
			}
			pCurrentMAC = (PIPMACENTRY)pCurrentMAC->dwNext;
		}
		v->pLastClickedIPEntry = NULL;
	}
}

void RecordSelectedIPStream(HWND hDlg, BOOL fStartRecording, int nSaveMode)
{
	if (fStartRecording == FALSE)
	{
		if (v->pLastClickedIPEntry->hSaveFile != NULL)
		{
			CloseHandle(v->pLastClickedIPEntry->hSaveFile);
			v->pLastClickedIPEntry->hSaveFile = NULL;
			v->pLastClickedIPEntry->fGotFirstFragment = FALSE;
			SetTreeViewIcon(v->pLastClickedIPEntry->hIPItem, 11);
		}
	}
	else
	{
		char szOutputName[MAX_PATH];
		char szTemp[MAX_PATH];

		if (v->pLastClickedIPEntry->hSaveFile != NULL)
			return;		// ignore since we're already recording this PID

		if (v->pLastClickedIPEntry->dwProtocol != IP_UDP_ID && v->pLastClickedIPEntry->dwProtocol != 6)
			return;		// needs to be TCP/IP or UDP/IP

		lstrcpy(szOutputName, v->szIPSaveFolder);
		if (lstrlen(szOutputName))
		{
			if (szOutputName[lstrlen(szOutputName)] != '\\')
				lstrcat(szOutputName, "\\");
		}

		wsprintf(szTemp, "%d.%d.%d.%d.ip", 
		 (v->pLastClickedIPEntry->dwDestinationAddress >> 24) & 0xff,
		 (v->pLastClickedIPEntry->dwDestinationAddress >> 16) & 0xff,
		 (v->pLastClickedIPEntry->dwDestinationAddress >> 8) & 0xff,
		 (v->pLastClickedIPEntry->dwDestinationAddress & 0xff));
		lstrcat(szOutputName, szTemp);

		v->pLastClickedIPEntry->nSaveMode = nSaveMode;
		v->pLastClickedIPEntry->hSaveFile = CreateFile(szOutputName,
										  GENERIC_WRITE,
										  FILE_SHARE_READ,
										  (LPSECURITY_ATTRIBUTES) NULL,
										  CREATE_ALWAYS,
										  FILE_ATTRIBUTE_NORMAL,
										  (HANDLE) NULL);
		if (v->pLastClickedIPEntry->hSaveFile == INVALID_HANDLE_VALUE)
		{
			v->pLastClickedIPEntry->hSaveFile = NULL;
			wsprintf(szTemp, "Unable to create output file %s", szOutputName);
			MessageBox(hDlg, szTemp, gszAppName, MB_ICONSTOP);
		}
		else
			SetTreeViewIcon(v->pLastClickedIPEntry->hIPItem, 22);
	}		
}

void RecordSelectedIPMACStream(HWND hDlg, BOOL fStartRecording, int nIPSaveMode)
{
	if (v->pLastClickedIPMACEntry != NULL)
	{
		PIPENTRY pCurrent = v->pLastClickedIPMACEntry->pIPEntries;
		while (pCurrent != NULL)
		{
			v->pLastClickedIPEntry = pCurrent;
			RecordSelectedIPStream(hDlg, fStartRecording, nIPSaveMode);
			pCurrent = (PIPENTRY)pCurrent->dwNext;
		}
		v->pLastClickedIPEntry = NULL;
	}
}

void RecordSelectedIPPIDStream(HWND hDlg, BOOL fStartRecording, int nIPSaveMode)
{
	if (v->pLastClickedIPPIDEntry != NULL)
	{
		PIPMACENTRY pCurrentMAC = v->pLastClickedIPPIDEntry;
		while (pCurrentMAC != NULL)
		{
			PIPENTRY pCurrent = pCurrentMAC->pIPEntries;
			while (pCurrent != NULL)
			{
				v->pLastClickedIPEntry = pCurrent;
				RecordSelectedIPStream(hDlg, fStartRecording, nIPSaveMode);
				pCurrent = (PIPENTRY)pCurrent->dwNext;
			}
			pCurrentMAC = (PIPMACENTRY)pCurrentMAC->dwNext;
		}
		v->pLastClickedIPEntry = NULL;
	}
}

#endif LITE

void ToggleAlwaysOnTop()
{
	if (v->fAlwaysOnTop)
		 SetWindowPos(v->hWndMainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	else
		 SetWindowPos(v->hWndMainWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

#ifndef LITE
void CheckThumbnailDisplayOrderMenu(HMENU hMenu)
{
	if (v->fThumbnailsRightToLeft)
		CheckMenuRadioItem(hMenu,
						   ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_TOPDOWN,
						   ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_RIGHTTOLEFT,
						   ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_RIGHTTOLEFT,
						   MF_BYCOMMAND);
	else
		CheckMenuRadioItem(hMenu,
						   ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_TOPDOWN,
						   ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_RIGHTTOLEFT,
						   ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_TOPDOWN,
						   MF_BYCOMMAND);
}

void CheckThumbnailSizeMenu(HMENU hMenu)
{
	switch(v->nThumbnailSize)
	{
	case 0:
		CheckMenuRadioItem(hMenu,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_NORMAL,
						   MF_BYCOMMAND);
		break;
	case 1:
		CheckMenuRadioItem(hMenu,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL,
						   MF_BYCOMMAND);
		break;
	case 2:
		CheckMenuRadioItem(hMenu,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE,
						   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE,
						   MF_BYCOMMAND);
	}
}
#endif LITE

void SetInitialMenuStates(HWND hWnd)
{
	HMENU hMenu = GetMenu(hWnd);

	if (v->fKeepPastEITData)
		CheckMenuItem(hMenu, ID_SETTINGS_KEEPPASTEITDATA, MF_CHECKED | MF_BYCOMMAND);			
	if (v->fHideWhenMinimized)
		CheckMenuItem(hMenu, ID_SETTINGS_HIDEWHENMINIMIZED, MF_CHECKED | MF_BYCOMMAND);
#ifndef LITE
	if (v->fCountContinuityErrors)
		CheckMenuItem(hMenu, ID_SETTINGS_COUNTCONTINUITYERRORS, MF_CHECKED | MF_BYCOMMAND);
#endif LITE
	if (v->fControlDVHSDeck)
		CheckMenuItem(hMenu, ID_SETTINGS_CONTROLDVHSDECK, MF_CHECKED | MF_BYCOMMAND);			
	if (v->fPowerCycleDVHSDeck)
		CheckMenuItem(hMenu, ID_SETTINGS_DVHSCONTROL_AUTOPOWERONOFF, MF_CHECKED | MF_BYCOMMAND);
	if (v->fIgnoreEIT)
		CheckMenuItem(hMenu, ID_SETTINGS_IGNOREEIT, MF_CHECKED | MF_BYCOMMAND);
	if (v->fIgnorePMT65500)
		CheckMenuItem(hMenu, ID_SETTINGS_IGNOREPMTSABOVECH65500, MF_CHECKED | MF_BYCOMMAND);
	if (v->fIgnorePMT800x0ff6)
		CheckMenuItem(hMenu, ID_SETTINGS_IGNOREDCIIPMTCH80, MF_CHECKED | MF_BYCOMMAND);
	if (v->fThumbnailThreadAnimated)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREADPRIORITY_ANIMATED, MF_CHECKED | MF_BYCOMMAND);
	if (v->fSaveAllThumbnailsSameName)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_SAVEALLUSESSAMENAME, MF_CHECKED | MF_BYCOMMAND);
	if (v->fSavedThumbnailsFullSize)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_FULLSIZESAVEDTHUMBNAILS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fShowScrambledChannels)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_SHOWSCRAMBLEDCHANNELS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fAutoExpandPMTs)
		CheckMenuItem(hMenu, ID_SETTINGS_AUTOEXPANDPMTS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fAutoExpandIPs)
		CheckMenuItem(hMenu, ID_SETTINGS_AUTOEXPANDIPS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fIgnoreTableCRCErrors)
		CheckMenuItem(hMenu, ID_SETTINGS_IGNORETABLECRCERRORS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fDVHSForceATSC)
		CheckMenuItem(hMenu, ID_SETTINGS_DVHS_FORCEPIDSTOBEATSCCOMPATIBLE, MF_CHECKED | MF_BYCOMMAND);
	if (v->fKeepSpecialXMLCharacters)
		CheckMenuItem(hMenu, ID_SETTINGS_KEEPSPECIALXMLCHARACTERS, MF_CHECKED | MF_BYCOMMAND);
#ifndef LITE
	if (v->fReloadManualChannels)
		CheckMenuItem(hMenu, ID_SETTINGS_RELOADMANUALCHANNELSAFTERRESTART, MF_CHECKED | MF_BYCOMMAND);
#endif LITE
	if (v->fWaitForCAThumbnail)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_WAITFORCABEFOREPICTURE, MF_CHECKED | MF_BYCOMMAND);
	if (v->fSDTOnlyForCurrentMux)
		CheckMenuItem(hMenu, ID_SETTINGS_SDTONLYFORCURRENTMUX, MF_CHECKED | MF_BYCOMMAND);
	if (v->fFullThumbnails)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_DISPLAYFULLTHUMBNAILSONLY, MF_CHECKED | MF_BYCOMMAND);
	if (v->fDecimalPIDs)
		CheckMenuItem(hMenu, ID_SETTINGS_DECIMALPIDS, MF_CHECKED | MF_BYCOMMAND);
#ifndef LITE
#ifndef PRO
	if (v->fAudioThumbnails)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_ENABLEAUDIOTHUMBNAILS, MF_CHECKED | MF_BYCOMMAND);		
#endif PRO
	CheckThumbnailSizeMenu(hMenu);
	CheckThumbnailDisplayOrderMenu(hMenu);
	if (v->fRealtimeCharting)
		CheckMenuItem(hMenu, ID_SETTINGS_REALTIMECHARTING, MF_CHECKED | MF_BYCOMMAND);
#else LITE
	CheckMenuRadioItem(hMenu,
					   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL,
					   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE,
					   ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_NORMAL,
					   MF_BYCOMMAND);
#endif LITE
	if (v->fHideThumbnailIcons)
		CheckMenuItem(hMenu, ID_SETTINGS_THUMBNAILTHREAD_HIDETHUMBNAILICONS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fPlainCADescriptors)
		CheckMenuItem(hMenu, ID_SETTINGS_PLAINCADESCRIPTORDECODING, MF_CHECKED | MF_BYCOMMAND);
	if (v->fAlwaysOnTop)
		CheckMenuItem(hMenu, ID_VIEW_ALWAYSONTOP, MF_CHECKED | MF_BYCOMMAND);
	if (v->fShowNonVideoPCR)
		CheckMenuItem(hMenu, ID_SETTINGS_SHOWNONVIDEOPCRICONS, MF_CHECKED | MF_BYCOMMAND);
	if (v->fChartGradientBitmap)
		CheckMenuItem(hMenu, ID_VIEW_CHART_SETTINGS_STYLE_GRADIENTBITMAPBACKGROUND, MF_CHECKED | MF_BYCOMMAND);
	if (v->fWarnBeforeOverwritingRecordings)
		CheckMenuItem(hMenu, ID_SETTINGS_WARNINGBEFOREOVERWRITNGRECORDINGS, MF_CHECKED | MF_BYCOMMAND);

	CheckMenuRadioItem(hMenu,
					   ID_VIEW_CHARTSETTINGS_STYLE_NOSTYLE,
					   ID_VIEW_CHARTSETTINGS_STYLE_DARKCOLORSWITHNOBORDER,
		               v->nChartStyle + ID_VIEW_CHARTSETTINGS_STYLE_NOSTYLE,
					   MF_BYCOMMAND);
}

#ifdef LITE
BOOL CALLBACK LiteChartWarningDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		MessageBeep(0);
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fLiteChartWarning = IsDlgButtonChecked(hDlg, IDC_LITE_CHART_NO_WARN);
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}
#endif LITE

void SetupChartClassName(char * szClassName, int nNewChartIndex)
{
	wsprintf(szClassName, "%s%d", gszChartClass, nNewChartIndex);
}

int CloseExistingChart(HWND hWnd, WORD wMenuID)
{
	int i;

#ifdef LITE
	int nPos = 0;

	while (v->wChartMenuItems[nPos] != 0)
	{
		if (wMenuID != v->wChartMenuItems[nPos])
			EnableMenuItem(GetMenu(v->hWndMainWindow), v->wChartMenuItems[nPos], MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
		nPos++;
	}
	if (v->fLiteChartWarning == FALSE)
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_LITE_CHART_WARNING), hWnd, LiteChartWarningDlgProc);
#endif LITE
	for (i = 0; i < MAX_CHARTS; i++)
	{
		if (v->hWndChart[i] == NULL)
			break;
	}
	if (i == MAX_CHARTS)
		i = 0;	// too many - start at zero again

	if (v->hWndChart[i] != NULL)
	{
		char szClassName[128];

		DestroyWindow(v->hWndChart[i]);
		v->hWndChart[i] = NULL;
		SetupChartClassName(szClassName, i);
		UnregisterClass(szClassName, v->hInstance);
	}

	return i;
}

void GetRetuneCommandLineParameters(char * szCommandLine, int nNITIndex)
{
	switch(v->pNITData[nNITIndex]->nType)
	{
	case NIT_DVBC:
		{
			// freq sr QAM inversion bandwidth
			int nQAMMode = 0;

			switch(v->pNITData[nNITIndex]->dvbc.nModulation)
			{
			case 0x01:	// 16 QAM
				nQAMMode = 0;
				break;
			case 0x02:	// 32 QAM
				nQAMMode = 1;
				break;
			case 0x03:	// 64 QAM
				nQAMMode = 2;
				break;
			case 0x04:	// 128 QAM
				nQAMMode = 3;
				break;
			case 0x05:	// 256 QAM
				nQAMMode = 4;
				break;
			}
			wsprintf(szCommandLine, "%d %d %d %d %d", 
				v->pNITData[nNITIndex]->nFrequency / 10, 
				v->pNITData[nNITIndex]->dvbc.nSymbolRate / 10,
				nQAMMode,
				v->ss.fSpectrumInversion,
				v->ss.nBandwidth);
		}
		break;
	case NIT_DVBT:
		{
			int nBandwidth = 0;
			int nTargetFrequency = v->pNITData[nNITIndex]->nFrequency / 100;
			char szINIFilename[MAX_PATH];
			char szTranslatedFrequency[64];
			char szKeyName[64];

			SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szINIFilename, sizeof(szINIFilename));
			lstrcat(szINIFilename, "\\dvbt.ini");
	
			// freq inversion bandwidth
			switch(v->pNITData[nNITIndex]->dvbt.nBandwidth)
			{
			case 0:
				nBandwidth = 2;
				break;
			case 1:
				nBandwidth = 1;
				break;
			case 2:
				nBandwidth = 0;
				break;
			}

			// Translate DVB-T frequencies for repeaters via a kludgey INI file
			sprintf(szKeyName, "%.3f", (double)v->pNITData[nNITIndex]->nFrequency / 100000.0);
			GetPrivateProfileString("DVBT", szKeyName, "", szTranslatedFrequency, sizeof(szTranslatedFrequency), szINIFilename);
			if (lstrlen(szTranslatedFrequency))
			{
				int nHigh, nLow;
				sscanf(szTranslatedFrequency, "%d.%d", &nHigh, &nLow);
				nTargetFrequency = (nHigh * 1000) + nLow;
			}
			wsprintf(szCommandLine, "%d %d %d", 
				nTargetFrequency, 
				v->ss.fSpectrumInversion,
				nBandwidth);
		}					
		break;
	case NIT_DVBS:
		{
			int nFrequency = v->pNITData[nNITIndex]->nFrequency / 100;
			int nInputPolarity = (~v->pNITData[nNITIndex]->dvbs.nPolarization) & 1;
			int nOutputPolarity = nInputPolarity;
			int nLOF = v->ss.nLNBFrequency;
			int nNetwork = v->pNITData[nNITIndex]->nNetworkID;
			int nOrbital = v->pNITData[nNITIndex]->dvbs.nOrbitalPosition;
			BOOL f22KHz = v->ss.n22KHz;
			int i;
			int nNewDiSEqCInput = 0;

			if (!v->pNITData[nNITIndex]->dvbs.fEastern)
				nOrbital = 3600 - nOrbital;

			// See if auto switch configuration is available
			for (i = 0; i < MAX_SWITCH_PARAMETERS; i++)
			{
				switch(v->sp[i].nAutoSelect)					
				{
				case AUTO_SELECT_LNB_FREQ:
					if (nFrequency >= v->sp[i].nAutoSelectFreqStart && nFrequency <= v->sp[i].nAutoSelectFreqEnd)
						nNewDiSEqCInput = i + 1;
					break;
				case AUTO_SELECT_LNB_ORBITAL:
					if (nOrbital == v->sp[i].nAutoSelectOrbital)
					{
						if (v->sp[i].nAutoSelectPolarity == 0)
							nNewDiSEqCInput = i + 1;							
						else
						{
							if (v->sp[i].nAutoSelectPolarity - 1 == nInputPolarity)
								nNewDiSEqCInput = i + 1;							
						}
					}
					break;
				case AUTO_SELECT_LNB_NETWORK:
					if (nNetwork == v->sp[i].nAutoSelectNetwork)
						nNewDiSEqCInput = i + 1;							
					break;
				}
				if (nNewDiSEqCInput)
				{
					v->ss.nDiSEqCInput = nNewDiSEqCInput;
					break;
				}
			}

			if (v->ss.nDiSEqCInput >= 1 && v->ss.nDiSEqCInput <= 21)
				SourceHelper_CalculateSwitchParameters(nFrequency, nInputPolarity, v->ss.nDiSEqCInput - 1, &nLOF, &f22KHz, &nOutputPolarity);

			if (v->dwSourceCapabilities & CAPABILITIES_ADV_SATELLITE)
			{
				//freq pol sr lnbf 22khz mode fec {input}
				int nMode = 0; // assume DVBS
				int nCodeRate = 5; // assume auto FEC for DVB-S

				switch(v->pNITData[nNITIndex]->dvbs.nModulation)
				{
				case 2:
					switch(v->pNITData[nNITIndex]->dvbs.nFEC)
					{
					case 2:
						nCodeRate = 0; // 2/3
						break;
					case 3:
						nCodeRate = 1; // 3/4 I
						break;
					case 4:
						nCodeRate = 3; // 5/6
						break;
					default:
						nCodeRate = 4; // 8/9
						break;
					}
					nMode = 2; // turbo 8PSK
					break;
				case 3:
					nCodeRate = 2;  // 3/4
					nMode = 1; // turbo QPSK
					break;
				}
				wsprintf(szCommandLine, "%d %d %d %d %d %d %d %d", 
					nFrequency, 
					nOutputPolarity,
					v->pNITData[nNITIndex]->dvbs.nSymbolRate / 10,
					nLOF,
					f22KHz,
					nMode,
					nCodeRate, 
					v->ss.nDiSEqCInput);
			}
			else
			{
				// freq pol sr lnbf 22khz {input}
				wsprintf(szCommandLine, "%d %d %d %d %d %d",
					nFrequency, 
					nOutputPolarity,
					v->pNITData[nNITIndex]->dvbs.nSymbolRate / 10,
					nLOF,
					f22KHz,
					v->ss.nDiSEqCInput);
			}
		}
		break;
	}
}

void RetuneToThisChannel(HWND hWnd, int nChannel)
{
	wsprintf(v->szNewCommandLine, "%d", nChannel);
	if (ParseSourceModuleCommandLine(&v->ss, v->szNewCommandLine, FALSE) == TRUE)
		RestartTSReader(hWnd);
}

void RetuneToThisMux(HWND hDlg)
{
	if (v->nNITRightClickIndex != -1)
	{
		GetRetuneCommandLineParameters(v->szNewCommandLine, v->nNITRightClickIndex);
		{
			char szTemp[1024];
			wsprintf(szTemp, "TSReader: New command line: '%s'\n", v->szNewCommandLine);
			OutputDebugString(szTemp);
		}
		if (ParseSourceModuleCommandLine(&v->ss, v->szNewCommandLine, FALSE) == TRUE)
			RestartTSReader(hDlg);
		v->szNewCommandLine[0] = '\0';
		v->nNITRightClickIndex = -1;
		v->nSDTRightClickIndex = -1;
	}
}

BOOL myChooseColor(HWND hDlg, DWORD * dwColor)
{
	CHOOSECOLOR cc;
	COLORREF acrCustClr[16];
	int i;

	for (i = 0; i < 16; i++)
		acrCustClr[i] = 0;

	memset(&cc, 0, sizeof(cc));
	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hDlg;
	cc.rgbResult = *dwColor;
	cc.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT;
	cc.lpCustColors = (LPDWORD) acrCustClr;

	if (ChooseColor(&cc) != FALSE)
	{
		*dwColor = cc.rgbResult;
		return TRUE;
	}
	return FALSE;
}

void ProcessPIDColorButtonClicks(HWND hDlg, WPARAM wParam)
{
	switch(LOWORD(wParam))
	{
	case IDC_UNSCRAMBLED_PID_COLOR_BROWSE:
		if (myChooseColor(hDlg, &v->dwUnscrambledPIDColor) != FALSE)
		{
			UpdatePIDChart(v->hDlgSIParser);
			InvalidateRect(hDlg, NULL, FALSE);
		}
		break;
	case IDC_UNSCRAMBLED_INACTIVE_PID_COLOR_BROWSE:
		if (myChooseColor(hDlg, &v->dwUnscrambledInactivePIDColor) != FALSE)
		{
			UpdatePIDChart(v->hDlgSIParser);
			InvalidateRect(hDlg, NULL, FALSE);
		}
		break;
	case IDC_SCRAMBLED_PID_COLOR_BROWSE:
		if (myChooseColor(hDlg, &v->dwScrambledPIDColor) != FALSE)
		{
			UpdatePIDChart(v->hDlgSIParser);
			InvalidateRect(hDlg, NULL, FALSE);
		}
		break;
	case IDC_SCRAMBLED_INACTIVE_PID_COLOR_BROWSE:
		if (myChooseColor(hDlg, &v->dwScrambledInactivePIDColor) != FALSE)
		{
			UpdatePIDChart(v->hDlgSIParser);
			InvalidateRect(hDlg, NULL, FALSE);
		}
		break;
	case IDC_HIGHLIGHT_PID_COLOR_BROWSE:
		if (myChooseColor(hDlg, &v->dwHighlightedPIDColor) != FALSE)
		{
			UpdatePIDChart(v->hDlgSIParser);
			InvalidateRect(hDlg, NULL, FALSE);
		}
		break;
	}
}

void PaintPIDChartColorBoxes(HWND hDlg, int nTopOffset, int nLeftOffset)
{
	PAINTSTRUCT ps;
	HDC hDC = BeginPaint(hDlg, &ps);
	DWORD dwColorList[5];
	int i;

	dwColorList[0] = v->dwUnscrambledPIDColor;
	dwColorList[1] = v->dwUnscrambledInactivePIDColor;
	dwColorList[2] = v->dwScrambledPIDColor;
	dwColorList[3] = v->dwScrambledInactivePIDColor;
	dwColorList[4] = v->dwHighlightedPIDColor;

	for (i = 0; i < 5; i++)
	{
		RECT rc;
		HBRUSH hBr = CreateSolidBrush(dwColorList[i]);
		
		rc.top = nTopOffset + (23 * i);
		rc.bottom = rc.top + 12;
		rc.left = nLeftOffset;
		rc.right = rc.left + 50;

		SelectObject(hDC, hBr);
		FillRect(hDC, &rc, hBr);
		DeleteObject(hBr);
	}

	EndPaint(hDlg, &ps);
}

BOOL CALLBACK PIDChartColorsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static DWORD dwScrambledPIDColor, dwUnscrambledPIDColor;
	static DWORD dwScrambledInactivePIDColor, dwUnscrambledInactivePIDColor;
	static DWORD dwHighlightedPIDColor;
	
	switch(uMsg)
	{
	case WM_INITDIALOG:
		dwScrambledPIDColor = v->dwScrambledPIDColor;
		dwUnscrambledPIDColor = v->dwUnscrambledPIDColor;
		dwScrambledInactivePIDColor = v->dwScrambledInactivePIDColor;
		dwUnscrambledInactivePIDColor = v->dwUnscrambledInactivePIDColor;
		dwHighlightedPIDColor = v->dwHighlightedPIDColor;
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			v->dwScrambledPIDColor = dwScrambledPIDColor;
			v->dwUnscrambledPIDColor = dwUnscrambledPIDColor;
			v->dwScrambledInactivePIDColor = dwScrambledInactivePIDColor;
			v->dwUnscrambledInactivePIDColor = dwUnscrambledInactivePIDColor;
			v->dwHighlightedPIDColor = dwHighlightedPIDColor;
			EndDialog(hDlg, FALSE);
			break;
		default:
			ProcessPIDColorButtonClicks(hDlg, wParam);
			break;
		}
		break;
	case WM_PAINT:
		PaintPIDChartColorBoxes(hDlg, 10, 200);
		break;
	}

	return FALSE;
}

BOOL CALLBACK GraphRefreshRateDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int SavednEPGHalfHourWidth;
	static int SavednEPGChannelHeight;
	static int SavedfShowEPGChannelsOnly;
	static int SavedfShowEPGThisMuxOnly;
	
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, IDC_GRAPH_REFRESH_RATE, v->nGraphRefreshRate, FALSE);
#ifdef PRO
		SetDlgItemInt(hDlg, IDC_GRAPH_HISTORICAL_POINTS, v->nGraphHistoricalPoints, FALSE);
#else PRO
		ShowWindow(GetDlgItem(hDlg, IDC_GRAPH_HISTORICAL_POINTS), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_GRAPH_HISTORICAL_POINTS_CAPTION), SW_HIDE);
#endif PRO
		SendDlgItemMessage(hDlg, IDC_GRAPH_REFRESH_RATE, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_GRAPH_REFRESH_RATE));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nTemp = GetDlgItemInt(hDlg, IDC_GRAPH_REFRESH_RATE, NULL, FALSE);
				if (nTemp > 0)
					v->nGraphRefreshRate = nTemp;
#ifdef PRO
				v->nGraphHistoricalPoints = GetDlgItemInt(hDlg, IDC_GRAPH_HISTORICAL_POINTS, NULL, FALSE);
#endif PRO
				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

void SetupEPGGridScrollbars(HWND hDlg)
{
	SetScrollRange(GetDlgItem(hDlg, IDC_EPG_HEIGHT), SB_CTL, 2, 24, TRUE);
	SetScrollRange(GetDlgItem(hDlg, IDC_EPG_WIDTH), SB_CTL, 1, 10, TRUE);
	SetScrollPos(GetDlgItem(hDlg, IDC_EPG_WIDTH), SB_CTL, v->nEPGHalfHourWidth, TRUE);
	SetScrollPos(GetDlgItem(hDlg, IDC_EPG_HEIGHT), SB_CTL, v->nEPGChannelHeight, TRUE);
}

void ProcessEPGScrollbarMessages(HWND hDlg, WPARAM wParam, LPARAM lParam)
{
	if ((HWND)lParam == GetDlgItem(hDlg, IDC_EPG_WIDTH))
	{
		switch(LOWORD(wParam))
		{
		case SB_LINELEFT:
			if (v->nEPGHalfHourWidth > 1)
				v->nEPGHalfHourWidth--;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_LINERIGHT:
			if (v->nEPGHalfHourWidth < 10)
				v->nEPGHalfHourWidth++;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_PAGELEFT:
			v->nEPGHalfHourWidth = 1;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_PAGERIGHT:
			v->nEPGHalfHourWidth = 10;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_THUMBTRACK:
			v->nEPGHalfHourWidth = HIWORD(wParam);
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		}
		SetScrollPos(GetDlgItem(hDlg, IDC_EPG_WIDTH), SB_CTL, v->nEPGHalfHourWidth, TRUE);
	}
	else
	{
		switch(LOWORD(wParam))
		{
		case SB_LINELEFT:
			if (v->nEPGChannelHeight > 2)
				v->nEPGChannelHeight--;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_LINERIGHT:
			if (v->nEPGChannelHeight < 24)
				v->nEPGChannelHeight++;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_PAGELEFT:
			v->nEPGChannelHeight = 2;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_PAGERIGHT:
			v->nEPGChannelHeight = 24;
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case SB_THUMBTRACK:
			v->nEPGChannelHeight = HIWORD(wParam);
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		}
		SetScrollPos(GetDlgItem(hDlg, IDC_EPG_HEIGHT), SB_CTL, v->nEPGChannelHeight, TRUE);
	}
}

BOOL CALLBACK EPGGridSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{	
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			v->SavednEPGHalfHourWidth = v->nEPGHalfHourWidth;
			v->SavednEPGChannelHeight = v->nEPGChannelHeight;
			v->SavedfShowEPGChannelsOnly = v->fShowEPGChannelsOnly;
			v->SavedfShowEPGThisMuxOnly = v->fShowEPGThisMuxOnly;
			v->SavedfEPGTimeGrid = v->fEPGTimeGrid;
			v->SavedfEPGTimeGridOnTop = v->fEPGTimeGridOnTop;
			
			v->dwSavedEPGEventColor = v->dwEPGEventColor;
			v->dwSavedEPGChannelColor = v->dwEPGChannelColor;
			v->dwSavedEPGSelectedColor = v->dwEPGSelectedColor;
			v->dwSavedEPGMainTextColor = v->dwEPGMainTextColor;
			v->dwSavedEPGSubTextColor = v->dwEPGSubTextColor;
			v->dwSavedEPGTimeGridColor = v->dwEPGTimeGridColor;

			SetupEPGGridScrollbars(hDlg);

			CheckDlgButton(hDlg, IDC_EPG_ONLY_EPG_CHANNELS, v->fShowEPGChannelsOnly);
			CheckDlgButton(hDlg, IDC_EPG_THIS_MUX_ONLY, v->fShowEPGThisMuxOnly);
			CheckDlgButton(hDlg, IDC_EPG_SHOW_TIME_GRID, v->fEPGTimeGrid);
			CheckDlgButton(hDlg, IDC_EPG_GRID_ON_TOP, v->fEPGTimeGridOnTop);
			CheckDlgButton(hDlg, IDC_EPG_RECORD_PS, v->fEPGRecordPS);
			CheckDlgButton(hDlg, IDC_EPG_UPDATE_REALTIME, v->fEPGUpdateRealtime);

			CheckDlgButton(hDlg, IDC_SCHEDULER_NO_DATE_TIME, v->fSchedulerNoDateTime);
			CheckDlgButton(hDlg, IDC_SCHEDULER_REQUIRES_LOGIN, v->fSchedulerRequiresLogin);
			CheckDlgButton(hDlg, IDC_EPG_SCHEDULE_WAKE, v->fSchedulerWake);

			SetDlgItemText(hDlg, IDC_SCHEDULER_USERNAME, v->szSchedulerUsername);
			SetDlgItemText(hDlg, IDC_SCHEDULER_PASSWORD, v->szSchedulerPassword);
			SetDlgItemText(hDlg, IDC_SCHEDULER_RECORD_DIRECTORY, v->szSchedulerDirectory);
			SetDlgItemInt(hDlg, IDC_EPG_DEFAULT_PREROLL, v->nSchedulerDefaultPreRoll, FALSE);
			SetDlgItemInt(hDlg, IDC_EPG_DEFAULT_POSTROLL, v->nSchedulerDefaultPostRoll, FALSE);

			if (lParam == TRUE)
				SetFocus(GetDlgItem(hDlg, IDC_SCHEDULER_USERNAME));
			else
				SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fEPGRecordPS = IsDlgButtonChecked(hDlg, IDC_EPG_RECORD_PS);
			GetDlgItemText(hDlg, IDC_SCHEDULER_USERNAME, v->szSchedulerUsername, sizeof(v->szSchedulerUsername));
			GetDlgItemText(hDlg, IDC_SCHEDULER_PASSWORD, v->szSchedulerPassword, sizeof(v->szSchedulerPassword));
			GetDlgItemText(hDlg, IDC_SCHEDULER_RECORD_DIRECTORY, v->szSchedulerDirectory, sizeof(v->szSchedulerDirectory));
			if (v->szSchedulerDirectory[lstrlen(v->szSchedulerDirectory) - 1] != '\\')
				lstrcat(v->szSchedulerDirectory, "\\");
			v->fSchedulerNoDateTime = IsDlgButtonChecked(hDlg, IDC_SCHEDULER_NO_DATE_TIME);
			v->fSchedulerRequiresLogin = IsDlgButtonChecked(hDlg, IDC_SCHEDULER_REQUIRES_LOGIN);
			v->fEPGUpdateRealtime = IsDlgButtonChecked(hDlg, IDC_EPG_UPDATE_REALTIME);
			v->fSchedulerWake = IsDlgButtonChecked(hDlg, IDC_EPG_SCHEDULE_WAKE);
			v->nSchedulerDefaultPreRoll = GetDlgItemInt(hDlg, IDC_EPG_DEFAULT_PREROLL, NULL, FALSE);
			v->nSchedulerDefaultPostRoll = GetDlgItemInt(hDlg, IDC_EPG_DEFAULT_POSTROLL, NULL, FALSE);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			v->nEPGHalfHourWidth = v->SavednEPGHalfHourWidth ;
			v->nEPGChannelHeight = v->SavednEPGChannelHeight;
			v->fShowEPGChannelsOnly = v->SavedfShowEPGChannelsOnly;
			v->fShowEPGThisMuxOnly = v->SavedfShowEPGThisMuxOnly;
			v->fEPGTimeGrid = v->SavedfEPGTimeGrid;
			v->fEPGTimeGridOnTop = v->SavedfEPGTimeGridOnTop;

			v->dwEPGEventColor = v->dwSavedEPGEventColor;
			v->dwEPGChannelColor = v->dwSavedEPGChannelColor;
			v->dwEPGSelectedColor = v->dwSavedEPGSelectedColor;
			v->dwEPGMainTextColor = v->dwSavedEPGMainTextColor;
			v->dwEPGSubTextColor = v->dwSavedEPGSubTextColor;
			v->dwEPGTimeGridColor = v->dwSavedEPGTimeGridColor;

			if (v->hWndEPGGrid != NULL)
			{
				DeleteObject(v->epg.hCellEntryBackground);
				v->epg.hCellEntryBackground = CreateSolidBrush(v->dwEPGEventColor);
				DeleteObject(v->epg.hChannelEntryBackground);
				v->epg.hChannelEntryBackground = CreateSolidBrush(v->dwEPGChannelColor);
				DeleteObject(v->epg.hCellEntrySelectedBackground);
				v->epg.hCellEntrySelectedBackground = CreateSolidBrush(v->dwEPGSelectedColor);

				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			}
			EndDialog(hDlg, FALSE);
			break;
		case IDC_EPG_SHOW_TIME_GRID:
			v->fEPGTimeGrid = IsDlgButtonChecked(hDlg, IDC_EPG_SHOW_TIME_GRID);
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case IDC_EPG_GRID_ON_TOP:
			v->fEPGTimeGridOnTop = IsDlgButtonChecked(hDlg, IDC_EPG_GRID_ON_TOP);
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case IDC_EPG_ONLY_EPG_CHANNELS:
			v->fShowEPGChannelsOnly = IsDlgButtonChecked(hDlg, IDC_EPG_ONLY_EPG_CHANNELS);
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case IDC_EPG_THIS_MUX_ONLY:
			v->fShowEPGThisMuxOnly = IsDlgButtonChecked(hDlg, IDC_EPG_THIS_MUX_ONLY);
			if (v->hWndEPGGrid != NULL)
				InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			break;
		case IDC_EPG_EVENT_BACK_COLOR:
			if (myChooseColor(hDlg, &v->dwEPGEventColor) != FALSE)
			{
				if (v->hWndEPGGrid != NULL)
				{
					DeleteObject(v->epg.hCellEntryBackground);
					v->epg.hCellEntryBackground = CreateSolidBrush(v->dwEPGEventColor);
					InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
				}
			}
			break;
		case IDC_EPG_CHANNEL_BACK_COLOR:
			if (myChooseColor(hDlg, &v->dwEPGChannelColor) != FALSE)
			{
				if (v->hWndEPGGrid != NULL)
				{
					DeleteObject(v->epg.hChannelEntryBackground);
					v->epg.hChannelEntryBackground = CreateSolidBrush(v->dwEPGChannelColor);
					InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
				}
			}
			break;
		case IDC_EPG_SELECTED_BACK_COLOR:
			if (myChooseColor(hDlg, &v->dwEPGSelectedColor) != FALSE)
			{
				if (v->hWndEPGGrid != NULL)
				{
					DeleteObject(v->epg.hCellEntrySelectedBackground);
					v->epg.hCellEntrySelectedBackground = CreateSolidBrush(v->dwEPGSelectedColor);
					InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
				}
			}
			break;
		case IDC_EPG_PRIMARY_TEXT_COLOR:
			if (myChooseColor(hDlg, &v->dwEPGMainTextColor) != FALSE)
			{
				if (v->hWndEPGGrid != NULL)
					InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			}
			break;
		case IDC_EPG_SECONDARY_TEXT_COLOR:
			if (myChooseColor(hDlg, &v->dwEPGSubTextColor) != FALSE)
			{
				if (v->hWndEPGGrid != NULL)
					InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			}
			break;
		case IDC_EPG_TIME_GRID_COLOR:
			if (myChooseColor(hDlg, &v->dwEPGTimeGridColor) != FALSE)
			{
				if (v->hWndEPGGrid != NULL)
					InvalidateRect(v->hWndEPGGrid, NULL, FALSE);
			}
			break;
		}
		break;
	case WM_HSCROLL:
		ProcessEPGScrollbarMessages(hDlg, wParam, lParam);
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

void ValidateWindowXY(int * nX, int * nY)
{
	if (*nX > GetSystemMetrics(SM_CXVIRTUALSCREEN))
		*nX = 0;
	if (*nY > GetSystemMetrics(SM_CYVIRTUALSCREEN))
		*nY = 0;
}

void ShowEPGGrid(HWND hWnd)
{
	DWORD dwStyle;
	int nWidth, nHeight, nX, nY;
	ATOM rcreturn;
	WNDCLASS  wc;

	// Setup the window classes
	memset(&wc, 0, sizeof(wc));
	wc.style =			CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc =	EPGridWndProc;
	wc.cbClsExtra =		0;
	wc.cbWndExtra =		0;
	wc.hInstance =		v->hInstance;
	wc.hIcon =			LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVBSMALL_LOGO));
	wc.hCursor =		LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground =	GetStockObject(BLACK_BRUSH); 
	wc.lpszMenuName =	NULL;//MAKEINTRESOURCE(IDR_TEST);
	wc.lpszClassName =	gszEPGGridClass;
	rcreturn = RegisterClass(&wc);

	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;
	nWidth = nHeight = nX = nY = CW_USEDEFAULT;	
	if (v->nEPGWindowW && v->nEPGWindowH)
	{
		nWidth = v->nEPGWindowW;
		nHeight = v->nEPGWindowH;
		nX = v->nEPGWindowX;
		nY = v->nEPGWindowY;
	}
	ValidateWindowXY(&nX, &nY);
	v->hWndEPGGrid = CreateWindow(gszEPGGridClass,
						   "EPG Grid",
						   dwStyle,
						   nX, nY,
						   nWidth, nHeight,
						   hWnd,
						   NULL,
						   v->hInstance,
						   0);
	if (v->hWndEPGGrid == NULL)
	{
		char szTemp[256];

		wsprintf(szTemp, "CreateWindow for the EPG Grid failed with GetLastError() = %d", GetLastError());
		MessageBox(hWnd, szTemp, gszAppName, MB_OK);
	}
	else
	{
		if (v->fEPGMaximizedFlag)
			ShowWindow(v->hWndEPGGrid, SW_MAXIMIZE);
		else
			ShowWindow(v->hWndEPGGrid, SW_SHOW);
	}
	return;
}

int CreateChartWindow(HWND hWnd, WNDPROC wndproc, int nParameters, WORD wMenuID)
{
	DWORD dwStyle;
	int nWidth, nHeight, nX, nY;
	int nNewChartIndex;
	char szClassName[128];
	ATOM rcreturn;
	WNDCLASS  wc;

	nNewChartIndex = CloseExistingChart(hWnd, wMenuID);

	// Setup the window classes
	memset(&wc, 0, sizeof(wc));
	wc.style =			CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc =	wndproc;
	wc.cbClsExtra =		0;
	wc.cbWndExtra =		0;
	wc.hInstance =		v->hInstance;
	wc.hIcon =			LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVBSMALL_LOGO));
	wc.hCursor =		LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground =	GetStockObject(BLACK_BRUSH); 
	wc.lpszMenuName =	NULL;//MAKEINTRESOURCE(IDR_TEST);
	SetupChartClassName(szClassName, nNewChartIndex);
	wc.lpszClassName =	szClassName;
	rcreturn = RegisterClass(&wc);

	v->nChartParameters = nParameters;

	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;
	nWidth = nHeight = nX = nY = CW_USEDEFAULT;	
	if (v->nChartWindowW && v->nChartWindowH)
	{
		nWidth = v->nChartWindowW;
		nHeight = v->nChartWindowH;
		nX = v->nChartWindowX;
		nY = v->nChartWindowY;
	}
	ValidateWindowXY(&nX, &nY);
	v->hWndChart[nNewChartIndex] = CreateWindow(szClassName,
						   "CHART",
						   dwStyle,
						   nX, nY,
						   nWidth, nHeight,
						   hWnd,
						   NULL,
						   v->hInstance,
						   &nNewChartIndex);
	if (v->hWndChart[nNewChartIndex] == NULL)
	{
		int nGLE = GetLastError();
		char szTemp[256];

		if (nGLE)
		{
			wsprintf(szTemp, "CreateWindow for the Chart failed with GetLastError() = %d", nGLE);
			MessageBox(hWnd, szTemp, gszAppName, MB_OK);
		}
	}
	else
	{
		if (v->fChartMaximizedFlag)
			ShowWindow(v->hWndChart[nNewChartIndex], SW_MAXIMIZE);
		else
			ShowWindow(v->hWndChart[nNewChartIndex], SW_SHOW);
	}

	return nNewChartIndex;
}

void UpdateTableMonitorStats(HWND hDlg)
{
	int nTableID;
	HWND hWndTableMonitorList = GetDlgItem(hDlg, IDC_TABLE_VIEWER_LIST);

	for (nTableID = 0; nTableID < 256; nTableID++)
	{
		if (v->tablemonitor[nTableID].nPacketCount)
		{
			if (v->tablemonitor[nTableID].fInList == FALSE)
			{
				LV_ITEM lvi; 

				memset(&lvi, 0, sizeof(lvi));
				lvi.state = 0; 
				lvi.stateMask = 0; 
				lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
				lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
				lvi.iItem = 0; 
				lvi.iSubItem = 0; 
				lvi.lParam = (LPARAM)nTableID;    // item data 
				ListView_InsertItem(hWndTableMonitorList, &lvi);
				v->tablemonitor[nTableID].fInList = TRUE;
			}

		}
		if (v->tablemonitor[nTableID].nLastDisplayPacketCount != v->tablemonitor[nTableID].nPacketCount
			|| v->tablemonitor[nTableID].lnLastDisplayDelay != v->tablemonitor[nTableID].lnDelay)
		{
			// need to update him
			int nItemCount = ListView_GetItemCount(hWndTableMonitorList);
			int i;

			v->tablemonitor[nTableID].nLastDisplayPacketCount = v->tablemonitor[nTableID].nPacketCount;
			v->tablemonitor[nTableID].lnLastDisplayDelay = v->tablemonitor[nTableID].lnDelay;

			for (i = 0; i < nItemCount; i++)
			{
				LVITEM lvi;
				memset(&lvi, 0, sizeof(lvi));
				lvi.mask = LVIF_PARAM; 
				lvi.iItem = i;
				ListView_GetItem(hWndTableMonitorList, &lvi);
				if (lvi.lParam == nTableID)
				{
					ListView_RedrawItems(GetDlgItem(hDlg, IDC_TABLE_VIEWER_LIST), i, i);
					break;
				}
			}
		}
	}
}

void GetTableMonitorDispInfo(LV_DISPINFO *pnmv) 
{ 
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nTableID = (int)(pnmv->item.lParam);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			if (v->fDecimalPIDs)
				wsprintf(pnmv->item.pszText, "%d", nTableID);
			else
				wsprintf(pnmv->item.pszText, "0x%02x", nTableID);
			break;
		case 1:
			lstrcpy(pnmv->item.pszText, GetTableDescription(nTableID));
			break;
		case 2:
			wsprintf(pnmv->item.pszText, "%d", v->tablemonitor[nTableID].nPacketCount);
			break;
		case 3:
			if (v->dDisplayMuxRate)
			{
				if (v->tablemonitor[nTableID].lnDelayItems)
				{
					__int64 nValue = (v->tablemonitor[nTableID].lnDelay / v->tablemonitor[nTableID].lnDelayItems) / (__int64)27000;
					wsprintf(pnmv->item.pszText, "%d", (int)nValue);
				}
				else
					pnmv->item.pszText[0] = '\0';
			}
			else
				lstrcpy(pnmv->item.pszText, "n/a");
			break;
		case 4:
			if (v->dDisplayMuxRate)
			{
				if (v->tablemonitor[nTableID].lnDelayMin != 0xffffffff)
				{
					__int64 nValue = v->tablemonitor[nTableID].lnDelayMin / (__int64)27000;
					wsprintf(pnmv->item.pszText, "%d", (int)nValue);
				}
				else
					pnmv->item.pszText[0] = '\0';
			}
			else
				lstrcpy(pnmv->item.pszText, "n/a");
			break;
		case 5:
			if (v->dDisplayMuxRate)
			{
				if (v->tablemonitor[nTableID].lnDelayMax)
				{
					__int64 nValue = v->tablemonitor[nTableID].lnDelayMax / (__int64)27000;
					wsprintf(pnmv->item.pszText, "%d", (int)nValue);
				}
				else
					pnmv->item.pszText[0] = '\0';
			}
			else
				lstrcpy(pnmv->item.pszText, "n/a");
			break;
		}
	}
}

#ifdef PRO
void AddRecordTable(HWND hDlg)
{
	int nPIDIndex = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETCURSEL, 0, 0);
	int nPID;
	int nStartTable, nEndTable;
	int nTableIndex;
	char * szHyphen;
	char szTableNumbers[256];
	char szPID[64];
	
	// Get the PID
	if (nPIDIndex == LB_ERR)
	{
		MessageBox(hDlg, "Please select the PID containing the table(s) you want to record", gszAppName, MB_ICONINFORMATION);
		SetFocus(GetDlgItem(hDlg, IDC_IPDVB_PID_LIST));
		return;
	}
	SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_GETTEXT, (WPARAM)nPIDIndex, (LPARAM)szPID);
	if (v->fDecimalPIDs == FALSE)
		sscanf(&szPID[2], "%x", &nPID);
	else
		sscanf(szPID, "%d", &nPID);

	// Get the table range
	GetDlgItemText(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER, szTableNumbers, sizeof(szTableNumbers));
	if (lstrlen(szTableNumbers) == 0)
	{
		MessageBox(hDlg, "Please enter the table number or range you want to record", gszAppName, MB_ICONINFORMATION);
		SetFocus(GetDlgItem(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER));
		return;
	}
	szHyphen = strstr(szTableNumbers, "-");
	if (szHyphen != NULL)
		*szHyphen = '\0';
	if (v->fDecimalPIDs == FALSE)
		sscanf(szTableNumbers, "%x", &nStartTable);
	else
		sscanf(szTableNumbers, "%d", &nStartTable);
	if (szHyphen == NULL)
		nEndTable = nStartTable;
	else
	{
		if (v->fDecimalPIDs == FALSE)
			sscanf(&szHyphen[1], "%x", &nEndTable);
		else
			sscanf(&szHyphen[1], "%d", &nEndTable);
	}

	if (nStartTable == 0xff || nEndTable == 0xff)
	{
		if (v->fDecimalPIDs)
			MessageBox(hDlg, "Table 255 cannot be used - this is reserved for stuffing", gszAppName, MB_ICONWARNING);
		else
			MessageBox(hDlg, "Table 0xff cannot be used - this is reserved for stuffing", gszAppName, MB_ICONWARNING);
		SetFocus(GetDlgItem(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER));
		return;
	}

	// Make sure this isn't already in the list and add it if not
	for (nTableIndex = 0; nTableIndex < MAX_RECORD_TABLES; nTableIndex++)
	{
		if (v->record_tables[nTableIndex].nPID == nPID)
		{
			if (v->record_tables[nTableIndex].nStartTable == nStartTable)
			{
				if (v->record_tables[nTableIndex].nEndTable == nEndTable)
				{
					MessageBox(hDlg, "This combination of PID and table(s) is already defined", gszAppName, MB_ICONWARNING);
					SetFocus(GetDlgItem(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER));
					return;
				}
			}
		}
		else if (v->record_tables[nTableIndex].nPID == -1)
		{
			LV_ITEM lvi; 

			v->record_tables[nTableIndex].nPID = nPID;
			v->record_tables[nTableIndex].nStartTable = nStartTable;
			v->record_tables[nTableIndex].nEndTable = nEndTable;

			memset(&lvi, 0, sizeof(lvi));
			lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
			lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
			lvi.iItem = nTableIndex; 
			ListView_InsertItem(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST), &lvi);

			SetDlgItemText(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER, "");
			SetFocus(GetDlgItem(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER));
			return;
		}
	}
}

void RemoveRecordTable(HWND hDlg)
{
	int nCount = ListView_GetItemCount(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST));
	int i;

	for (i = 0; i < nCount; i++)
	{
		LV_ITEM lvItem;

		lvItem.mask = LVIF_STATE | LVIF_PARAM;
		lvItem.iItem = i;
		lvItem.iSubItem = 0;
		lvItem.stateMask = LVIS_SELECTED;
		ListView_GetItem(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST), &lvItem);
		if ((lvItem.state & LVIS_SELECTED) > 0)
		{
			int nItemIndex;

			ListView_DeleteItem(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST), i);
			for (nItemIndex = i; nItemIndex < MAX_RECORD_TABLES - 1; nItemIndex++)
				memcpy(&v->record_tables[nItemIndex], &v->record_tables[nItemIndex + 1], sizeof(v->record_tables[nItemIndex]));
			v->record_tables[nItemIndex].nPID = -1;
			return;
		}
	}
	
	MessageBox(hDlg, "Please select the PID/table(s) to delete first", gszAppName, MB_ICONINFORMATION);
}

void GetRecordTableListDispInfo(LV_DISPINFO *pnmv) 
{ 
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nItem = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:	// PID
			if (v->fDecimalPIDs)
				wsprintf(pnmv->item.pszText, "%d", v->record_tables[nItem].nPID);
			else
				wsprintf(pnmv->item.pszText, "0x%04x", v->record_tables[nItem].nPID);
			break;
		case 1:	// Tables
			if (v->fDecimalPIDs)
			{
				if (v->record_tables[nItem].nStartTable == v->record_tables[nItem].nEndTable)
					wsprintf(pnmv->item.pszText, "%d", v->record_tables[nItem].nStartTable);
				else
					wsprintf(pnmv->item.pszText, "%d-%d", v->record_tables[nItem].nStartTable, v->record_tables[nItem].nEndTable);
			}
			else
			{
				if (v->record_tables[nItem].nStartTable == v->record_tables[nItem].nEndTable)
					wsprintf(pnmv->item.pszText, "0x%x", v->record_tables[nItem].nStartTable);
				else
					wsprintf(pnmv->item.pszText, "0x%02x-0x%02x", v->record_tables[nItem].nStartTable, v->record_tables[nItem].nEndTable);
			}
			break;
		}
	}
}

BOOL CALLBACK RecordTablesDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int i;
			int nColumnPosition = 0;
			int nTableIndex;
			int nItemCount;
			HWND hWndPIDTableList = GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST);
			LV_COLUMN lvc; 
			char szTemp[128];

			if (v->fDecimalPIDs)
				SetDlgItemText(hDlg, IDC_RECORD_TABLE_TABLE_CAPTION, "(single number or range like 80-95)");

			SetDlgItemText(hDlg, IDC_RECORD_TABLES_OUTPUT_LOCATION, v->szRecordTablesFolder);

			// Setup PID list
			for (i = 0; i < 8191; i++)
			{
				if (v->pc[i].lnPackets)
				{
					int nIndex;
					int j = 0;
					char szPID[64];

					wsprintf(szPID, "%s", FormatTooltipPID(v->pc[i].nPID));			
					nIndex = SendDlgItemMessage(hDlg, IDC_IPDVB_PID_LIST, LB_ADDSTRING, 0, (LPARAM)szPID);
				}
			}

			// Setup options
			SetDlgItemText(hDlg, IDC_RECORD_TABLES_OUTPUT_LOCATION, v->szRecordTablesFolder);
			if (v->fRecordTablesHexASCII)
				CheckDlgButton(hDlg, IDC_RECORD_TABLES_HEX_ASCII, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_RECORD_TABLES_RAW, BST_CHECKED);

			// Setup list view columns. 
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			lvc.pszText = szTemp; 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 50; 
			lstrcpy(szTemp, TEXT("PID"));
			ListView_InsertColumn(hWndPIDTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_LEFT; 
			lvc.cx = 80; 
			lstrcpy(szTemp, TEXT("Table(s)"));
			ListView_InsertColumn(hWndPIDTableList, nColumnPosition++, &lvc); 

			ListView_SetExtendedListViewStyle(hWndPIDTableList, LVS_EX_FULLROWSELECT);

			// Setup existing items
			nItemCount = 0;
			for (nTableIndex = 0; nTableIndex < MAX_RECORD_TABLES; nTableIndex++)
			{
				if (v->record_tables[nTableIndex].nPID != -1)
				{
					LV_ITEM lvi; 

					memset(&lvi, 0, sizeof(lvi));
					lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
					lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
					lvi.iItem = nItemCount++; 
					ListView_InsertItem(GetDlgItem(hDlg, IDC_RECORD_TABLES_PID_TABLE_LIST), &lvi);
				}
			}

			SetFocus(GetDlgItem(hDlg, IDC_RECORD_TABLE_TABLE_NUMBER));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				int nItemIndex;
				char szOutputLocation[MAX_PATH];
				char szOutputFilename[MAX_PATH];

				GetDlgItemText(hDlg, IDC_RECORD_TABLES_OUTPUT_LOCATION, szOutputLocation, sizeof(szOutputLocation));
				if (lstrlen(szOutputLocation) == 0)
				{
					MessageBox(hDlg, "Please set the output location", gszAppName, MB_ICONINFORMATION);
					SetFocus(GetDlgItem(hDlg, IDC_RECORD_TABLES_OUTPUT_LOCATION));
					break;
				}
				lstrcpy(v->szRecordTablesFolder, szOutputLocation);

				v->fRecordTablesHexASCII = IsDlgButtonChecked(hDlg, IDC_RECORD_TABLES_HEX_ASCII);
				for (nItemIndex = 0; nItemIndex < MAX_RECORD_TABLES; nItemIndex++)
				{
					char szTemp[128];

					v->nFillPtr[BUFFER_RECORD_TABLES1 + nItemIndex] = 0;

					if (v->record_tables[nItemIndex].nPID != -1)
					{
						if (v->fDecimalPIDs)
						{
							if (v->record_tables[nItemIndex].nStartTable == v->record_tables[nItemIndex].nEndTable)
								wsprintf(szTemp, "PID %d Table %d", 
								         v->record_tables[nItemIndex].nPID,
										 v->record_tables[nItemIndex].nStartTable);
							else
								wsprintf(szTemp, "PID %d Tables %d-%d", 
								         v->record_tables[nItemIndex].nPID,
										 v->record_tables[nItemIndex].nStartTable,
										 v->record_tables[nItemIndex].nEndTable);
						}
						else
						{
							if (v->record_tables[nItemIndex].nStartTable == v->record_tables[nItemIndex].nEndTable)
								wsprintf(szTemp, "PID 0x%04x Table 0x%02x",
								         v->record_tables[nItemIndex].nPID,
										 v->record_tables[nItemIndex].nStartTable);
							else
								wsprintf(szTemp, "PID 0x%04x Tables 0x%02x-0x%02x",
								         v->record_tables[nItemIndex].nPID,
										 v->record_tables[nItemIndex].nStartTable,
										 v->record_tables[nItemIndex].nEndTable);
						}
						if (v->fRecordTablesHexASCII)
							lstrcat(szTemp, ".txt");
						else
							lstrcat(szTemp, ".bin");
						lstrcpy(szOutputFilename, szOutputLocation);
						if (szOutputFilename[lstrlen(szOutputFilename) - 1] != '\\')
							lstrcat(szOutputFilename, "\\");
						lstrcat(szOutputFilename, szTemp);
						v->record_tables[nItemIndex].dwPriorTickCount = 0;
						v->record_tables[nItemIndex].nTableCount = 0;
						v->record_tables[nItemIndex].hFile = CreateFile(szOutputFilename, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
					}
				}
				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_RECORD_PID_LOAD:
			LoadPIDListDialog(hDlg);
			break;
		case IDC_RECORD_PID_SAVE:
			SavePIDListDialog(hDlg);
			break;
		case IDC_RECORD_TABLES_ADD:
			AddRecordTable(hDlg);
			break;
		case IDC_RECORD_TABLES_LOAD:
			LoadTableListDialog(hDlg);
			break;
		case IDC_RECORD_TABLE_SAVE:
			SaveTableListDialog(hDlg);
			break;
		case IDC_RECORD_TABLES_OUTPUT_LOCATION_BROWSE:
			{
				LPITEMIDLIST ItemID;
				BROWSEINFO BrowsingInfo;
				char DirPath[MAX_PATH];
				char FolderName[MAX_PATH];

				lstrcpy(FolderName, v->szRecordTablesFolder);
				memset(&BrowsingInfo, 0, sizeof(BROWSEINFO));
				memset(DirPath, 0, MAX_PATH);

				BrowsingInfo.hwndOwner      = hDlg;
				BrowsingInfo.pszDisplayName = FolderName;
				BrowsingInfo.lpszTitle      = "Select output folder for recorded file(s)";
				BrowsingInfo.ulFlags = BIF_NEWDIALOGSTYLE;
				ItemID = SHBrowseForFolder(&BrowsingInfo);
				if (ItemID)
				{
					SHGetPathFromIDList(ItemID, DirPath);
					SetDlgItemText(hDlg, IDC_RECORD_TABLES_OUTPUT_LOCATION, DirPath);
				} 
			}
			break;
		case IDC_RECORD_TABLES_REMOVE:
			RemoveRecordTable(hDlg);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetRecordTableListDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		}
		break;
	}
	
	return FALSE;
}
#endif PRO

BOOL CALLBACK PluginSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTemp[128];

			CheckDlgButton(hDlg, IDC_PLUGINS_PREFERED_CA_ID_ENABLED, v->fUsePreferedCAID);
			wsprintf(szTemp, "%x", v->nPrefereredCAID);
			SetDlgItemText(hDlg, IDC_PLUGINS_PREFERED_CA_ID, szTemp);
			EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_PREFERED_CA_ID), v->fUsePreferedCAID);
			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fUsePreferedCAID = IsDlgButtonChecked(hDlg, IDC_PLUGINS_PREFERED_CA_ID_ENABLED);
			if (v->fUsePreferedCAID)
			{
				char szTemp[128];
				
				GetDlgItemText(hDlg, IDC_PLUGINS_PREFERED_CA_ID, szTemp, sizeof(szTemp));
				sscanf(szTemp, "%x", &v->nPrefereredCAID);
			}
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_PLUGINS_PREFERED_CA_ID_ENABLED:
			EnableWindow(GetDlgItem(hDlg, IDC_PLUGINS_PREFERED_CA_ID), IsDlgButtonChecked(hDlg, IDC_PLUGINS_PREFERED_CA_ID_ENABLED));
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}
	
	return FALSE;
}


BOOL CALLBACK EmailSetupDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemText(hDlg, IDC_ARCHIVE_EMAIL, v->szEmailDestination);
			SetDlgItemText(hDlg, IDC_ARCHIVE_SMTP_SERVER, v->szSMTPServer);
			SetDlgItemText(hDlg, IDC_ARCHIVE_EMAIL_FROM, v->szEmailFrom);

			CheckDlgButton(hDlg, IDC_ARCHIVE_SMTP_AUTHENTICATE, v->fSMTPNeedsAuthentication);
			SetDlgItemText(hDlg, IDC_ARCHIVE_SMTP_USERNAME, v->szSMTPUsername);
			SetDlgItemText(hDlg, IDC_ARCHIVE_SMTP_PASSWORD, v->szSMTPPassword);
			EnableWindow(GetDlgItem(hDlg, IDC_ARCHIVE_SMTP_USERNAME), v->fSMTPNeedsAuthentication);
			EnableWindow(GetDlgItem(hDlg, IDC_ARCHIVE_SMTP_PASSWORD), v->fSMTPNeedsAuthentication);

			SetFocus(GetDlgItem(hDlg, IDOK));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				GetDlgItemText(hDlg, IDC_ARCHIVE_EMAIL, v->szEmailDestination, sizeof(v->szEmailDestination));
				GetDlgItemText(hDlg, IDC_ARCHIVE_SMTP_SERVER, v->szSMTPServer, sizeof(v->szSMTPServer));
				GetDlgItemText(hDlg, IDC_ARCHIVE_EMAIL_FROM, v->szEmailFrom, sizeof(v->szEmailFrom));

				v->fSMTPNeedsAuthentication = IsDlgButtonChecked(hDlg, IDC_ARCHIVE_SMTP_AUTHENTICATE);
				GetDlgItemText(hDlg, IDC_ARCHIVE_SMTP_USERNAME, v->szSMTPUsername, sizeof(v->szSMTPUsername));
				GetDlgItemText(hDlg, IDC_ARCHIVE_SMTP_PASSWORD, v->szSMTPPassword, sizeof(v->szSMTPPassword));
				
				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_ARCHIVE_SMTP_AUTHENTICATE:
			EnableWindow(GetDlgItem(hDlg, IDC_ARCHIVE_SMTP_USERNAME), IsDlgButtonChecked(hDlg, IDC_ARCHIVE_SMTP_AUTHENTICATE));
			EnableWindow(GetDlgItem(hDlg, IDC_ARCHIVE_SMTP_PASSWORD), IsDlgButtonChecked(hDlg, IDC_ARCHIVE_SMTP_AUTHENTICATE));
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}


BOOL CALLBACK EITServerSettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton(hDlg, IDC_EIT_SERVER_ENABLED, v->fEITServerEnabled);
			SetDlgItemInt(hDlg, IDC_EIT_SERVER_PORT, v->nEITServerPort, FALSE);
			SetFocus(GetDlgItem(hDlg, IDC_EIT_SERVER_ENABLED));
			PostMessage(hDlg, WM_COMMAND, IDC_EIT_SERVER_REFRESH_CONNECTIONS, 0);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				BOOL fFlag = FALSE;

				if (IsDlgButtonChecked(hDlg, IDC_EIT_SERVER_ENABLED) != (UINT)v->fEITServerEnabled)
					fFlag = TRUE;
				if (GetDlgItemInt(hDlg, IDC_EIT_SERVER_PORT, NULL, FALSE) != (UINT)v->nEITServerPort)
					fFlag = TRUE;
				v->fEITServerEnabled = IsDlgButtonChecked(hDlg, IDC_EIT_SERVER_ENABLED);
				v->nEITServerPort = GetDlgItemInt(hDlg, IDC_EIT_SERVER_PORT, NULL, FALSE);

				if (fFlag == TRUE)
					MessageBox(hDlg, "You need to quit and relaunch TSReader for this change to take effect.", gszAppName, MB_ICONINFORMATION);

				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_EIT_SERVER_REFRESH_CONNECTIONS:
			{
				int nConnectionNumber;

				while (SendDlgItemMessage(hDlg, IDC_EIT_SERVER_CONNECTIONS, LB_DELETESTRING, 0, 0) != LB_ERR)
					;

				if (!v->fEITServerInitialized)
					break;
				EnterCriticalSection(&v->csEITConnection);
				for (nConnectionNumber = 0; nConnectionNumber < MAX_EIT_CONNECTIONS; nConnectionNumber++)
				{
					if (v->EITConnection[nConnectionNumber].socket != INVALID_SOCKET)
					{
						char szTemp[256];

						wsprintf(szTemp, "Channel %d requested by %d.%d.%d.%d",
							     v->EITConnection[nConnectionNumber].nChannel,
								 v->EITConnection[nConnectionNumber].acc_sin.sin_addr.S_un.S_un_b.s_b1,
								 v->EITConnection[nConnectionNumber].acc_sin.sin_addr.S_un.S_un_b.s_b2,
								 v->EITConnection[nConnectionNumber].acc_sin.sin_addr.S_un.S_un_b.s_b3,
								 v->EITConnection[nConnectionNumber].acc_sin.sin_addr.S_un.S_un_b.s_b4);
						SendDlgItemMessage(hDlg, IDC_EIT_SERVER_CONNECTIONS, LB_ADDSTRING, 0, (LPARAM)szTemp);
					}
				}
				LeaveCriticalSection(&v->csEITConnection);
			}
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

#ifndef LITE
BOOL CALLBACK TableViewerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hWndTableList = GetDlgItem(hDlg, IDC_TABLE_VIEWER_LIST);
			int nColumnPosition = 0;
			LV_COLUMN lvc; 
			char szTemp[128];

			// Setup list view columns. 
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
			lvc.pszText = szTemp; 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 40; 
			lstrcpy(szTemp, TEXT("Table"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_LEFT; 
			lvc.cx = 265; 
			lstrcpy(szTemp, TEXT("Description"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 60; 
			lstrcpy(szTemp, TEXT("Sections"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 70; 
			lstrcpy(szTemp, TEXT("Cycle (avg)"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 70; 
			lstrcpy(szTemp, TEXT("Cycle (min)"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			lvc.fmt = LVCFMT_CENTER; 
			lvc.cx = 70; 
			lstrcpy(szTemp, TEXT("Cycle (max)"));
			ListView_InsertColumn(hWndTableList, nColumnPosition++, &lvc); 

			ListView_SetExtendedListViewStyle(hWndTableList, LVS_EX_FULLROWSELECT);
			
			if (v->fDecimalPIDs)
			{
				wsprintf(szTemp, "%d", v->nTableMonitorPID);
				SetDlgItemText(hDlg, IDC_TABLE_VIEWER_PID_FORMAT, gszDecimalString);
			}
			else
			{
				wsprintf(szTemp, "%x", v->nTableMonitorPID);
				SetDlgItemText(hDlg, IDC_TABLE_VIEWER_PID_FORMAT, gszHexString);
			}
			SetDlgItemText(hDlg, IDC_TABLE_VIEWER_PID, szTemp);

#ifdef PRO
			v->nTableMonitorSectionTable = -1;
			SendDlgItemMessage(hDlg, IDC_TABLE_VIEWER_SECTION_DISPLAY, WM_SETFONT, (WPARAM)v->hCourierNew, MAKELONG(TRUE, 0));
			v->hTableViewerSectionDisplayWindow = GetDlgItem(hDlg, IDC_TABLE_VIEWER_SECTION_DISPLAY);
#endif PRO
			SetFocus(GetDlgItem(hDlg, IDC_TABLE_VIEWER_PID));
		}
		return TRUE;
	case WM_CLOSE:
		if (v->fTableMonitorRunning)
			SendMessage(hDlg, WM_COMMAND, IDOK, 0);
		EndDialog(hDlg, FALSE);
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			if (v->fTableMonitorRunning == FALSE)
			{
				int nMonitorPID = -1;
				int i;
				char szTemp[128];

				GetDlgItemText(hDlg, IDC_TABLE_VIEWER_PID, szTemp, sizeof(szTemp));
				if (szTemp[0] == '0' && szTemp[1] == 'x')
					sscanf(&szTemp[2], "%x", &nMonitorPID);
				else
				{
					if (v->fDecimalPIDs == TRUE)
						sscanf(szTemp, "%d", &nMonitorPID);
					else
						sscanf(szTemp, "%x", &nMonitorPID);
				}
				if (nMonitorPID < 0 || nMonitorPID > 8190)
				{
					char szTemp2[256];
					wsprintf(szTemp2, "TSReader: Invalid PID value '%s' -> 0x%08x\n", szTemp, nMonitorPID);
					OutputDebugString(szTemp2);

					if (v->fDecimalPIDs)
						wsprintf(szTemp, "Invalid PID value %d (0 - 8192 accepted)", nMonitorPID);
					else
						wsprintf(szTemp, "Invalid PID value 0x%x (0 - 1ffe accepted)", nMonitorPID);
					MessageBox(hDlg, szTemp, gszAppName, MB_ICONSTOP);
					SetFocus(GetDlgItem(hDlg, IDC_TABLE_VIEWER_PID));
					break;
				}
				for (i = 0; i < 256; i++)
				{
					memset(&v->tablemonitor[i], 0, sizeof(TABLEMONITOR));
					v->tablemonitor[i].lnDelayMin = 0xffffffff;
				}

				ListView_DeleteAllItems(GetDlgItem(hDlg, IDC_TABLE_VIEWER_LIST));
				v->nTableMonitorFillPtr = 0;
				v->nTableMonitorPID = nMonitorPID;
				v->fTableMonitorRunning = TRUE;
				SetDlgItemText(hDlg, IDOK, "Stop");
				SetTimer(hDlg, 1, 250, NULL);
			}
			else
			{
				v->fTableMonitorRunning = FALSE;
				SetDlgItemText(hDlg, IDOK, "Start");
				KillTimer(hDlg, 1);
			}
			break;
		case IDCANCEL:
			if (v->fTableMonitorRunning)
				SendMessage(hDlg, WM_COMMAND, IDOK, 0);
			EndDialog(hDlg, FALSE);
			break;
#ifdef PRO
		case IDC_TABLE_VIEWER_SHOW_SECTIONS:
			if (!v->fTableMonitorRunning)
			{
				MessageBox(hDlg, "Please start the table monitor first", gszAppName, MB_ICONWARNING);
				break;
			}
			if (v->nTableMonitorSectionTable == -1)
			{
				CheckDlgButton(hDlg, IDC_TABLE_VIEWER_SHOW_SECTIONS, FALSE);
				MessageBox(hDlg, "Please select the table from which you wish to view sections", gszAppName, MB_ICONWARNING);
			}
			v->fTableMonitorSectionDisplayEnabled = IsDlgButtonChecked(hDlg, IDC_TABLE_VIEWER_SHOW_SECTIONS);
			break;
#endif PRO
		}
		break;
	case WM_TIMER:
		UpdateTableMonitorStats(hDlg);
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetTableMonitorDispInfo((LV_DISPINFO *) lParam);
			}
			break;
#ifdef PRO
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
				{
					v->nTableMonitorSectionTable = pnmv->lParam;
				}
			}
			break;
#endif PRO
		}
		break;
	}

	return FALSE;
}
#endif LITE

BOOL IsFileSource()
{
	if (strstr(v->szSourceName, "_File") != NULL)
		return TRUE;
	return FALSE;
}

void SetPCTimeToStreamTime(HWND hWnd)
{

	SYSTEMTIME stNew, stSet;

	memset(&stNew, 0, sizeof(stNew));
	if (v->nTDTRightClickIndex == 0)
	{
		stNew.wYear = v->dvbtdt.nYear;
		stNew.wMonth = v->dvbtdt.nMonth;
		stNew.wDay = v->dvbtdt.nDay;
		stNew.wHour = v->dvbtdt.nHour;
		stNew.wMinute = v->dvbtdt.nMinute;
		stNew.wSecond = v->dvbtdt.nSecond;
	}
	else
	{
		stNew.wYear = v->dvbtot.nYear;
		stNew.wMonth = v->dvbtot.nMonth;
		stNew.wDay = v->dvbtot.nDay;
		stNew.wHour = v->dvbtot.nHour;
		stNew.wMinute = v->dvbtot.nMinute;
		stNew.wSecond = v->dvbtot.nSecond;
	}
	if (SetSystemTime(&stNew) == FALSE)
	{
		int nGLE = GetLastError();
		nGLE = nGLE;
	}
	GetSystemTime(&stSet);
}

LRESULT APIENTRY MainDlgSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
	switch(uMsg)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		PostMessage(v->hWndMainWindow, uMsg, wParam, lParam);
	default:
		{
			int i;

			for (i = 0; i < 10; i++)
			{
				if (hWnd == v->hWndMainDlgSubclass[i])
					return CallWindowProc(v->wpSubclassOrigProc[i], hWnd, uMsg, wParam, lParam); 
			}
		}
		break;
	}

	return FALSE;
}

#ifdef PRO
BOOL CALLBACK ChartSaveDataDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_SAVE_CHART_DATA_ENABLED, v->fSaveChartDataEnabled);
		SetDlgItemText(hDlg, IDC_SAVE_CHART_DATA_FOLDER, v->szSaveChartDataFolder);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			v->fSaveChartDataEnabled = IsDlgButtonChecked(hDlg, IDC_SAVE_CHART_DATA_ENABLED);
			GetDlgItemText(hDlg, IDC_SAVE_CHART_DATA_FOLDER, v->szSaveChartDataFolder, sizeof(v->szSaveChartDataFolder));
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}

	return FALSE;
}

BOOL CALLBACK MDIIndexDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static __int64 nTotalBitrate;
	static int nTotalBitrateSamples;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{			
			InitializeCriticalSection(&v->csMDI);
			mdi = mdiStreamInit(188);
			SetTimer(hDlg, 1, 1000, NULL);

			SendDlgItemMessage(hDlg, IDC_MDI_DEBUG, WM_SETFONT, (WPARAM)v->hCourierNew, MAKELONG(TRUE, 0));

			nTotalBitrate = 0;
			nTotalBitrateSamples = 0;

			v->fMDIIndexActive = TRUE;

			SetFocus(GetDlgItem(hDlg, IDCANCEL));
		}
		break;
	case WM_DESTROY:
		{
			KillTimer(hDlg, 1);
			v->fMDIIndexActive = FALSE;
			Sleep(250);
			mdiStreamStop(mdi);
			DeleteCriticalSection(&v->csMDI);
			v->hDlgMDIIndex = NULL;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hDlg);
		break;
	case WM_TIMER:
		{
			tMDISAMPLE sample;
			MDITIME ts;
			signed __int64 lnCurrentCounterValue;
			double dnsPerTick;
			char szTemp[128];

			memset(&sample, 0, sizeof(sample));

			dnsPerTick = 1000000000.0 / (double)v->lnTicksPerSecond;
			QueryPerformanceCounter((LARGE_INTEGER *)&lnCurrentCounterValue);
			ts = lnCurrentCounterValue * (__int64)dnsPerTick;

			EnterCriticalSection(&v->csMDI);
			mdiSample(ts, mdi, &sample);
			LeaveCriticalSection(&v->csMDI);			
			sprintf(szTemp, "MDI: df=%8.3f lc=%5u br=%9u  ts=%.0f\n", sample.delayFactor / 1000, sample.lossCount, sample.bitrate, (double)ts);
			SendDlgItemMessage(hDlg, IDC_MDI_DEBUG, LB_INSERTSTRING, 0, (LPARAM)szTemp);

			nTotalBitrate += (__int64)sample.bitrate;
			nTotalBitrateSamples++;
			wsprintf(szTemp, "%d", nTotalBitrate / nTotalBitrateSamples);
			SetDlgItemText(hDlg, IDC_MDI_AVERAGE_BR, szTemp);
		}
		break;
	}

	return FALSE;
}
#endif PRO

#ifndef LITE
typedef struct _tagNetworkToINIList
{
	int nNetworkID;
	int nDVBSignals;
	int nADVSignals;
	HANDLE hFile;
	char szFilename[MAX_PATH];
} NETWORKTOINILIST, *PNETWORKTOINILIST;
#define MAX_NETWORK_TO_INI_LIST 32

void AddNetworkToINI(PNITENTRY pNITData, PNETWORKTOINILIST pNetworkToINIList)
{
	int nRollOff = pNITData->dvbs.nModulation >> 3;
	int nModulationSystem = pNITData->dvbs.nModulation >> 2;
	int nModulationType = pNITData->dvbs.nModulation & 3;
	int fADVMux = FALSE;
	char szINIString[256] = {""};
	char szFEC[32] = {""};
	char cPolarity;

	switch(pNITData->dvbs.nPolarization)
	{
	case 0:
		cPolarity = 'H';
		break;
	case 1:
		cPolarity = 'V';
		break;
	case 2:
		cPolarity = 'L';
		break;
	case 3:
		cPolarity = 'R';
		break;
	}

	switch(nModulationType)
	{
	case 0:
	case 1:		// QPSK
		switch(pNITData->dvbs.nFEC)
		{
		case 1:
			lstrcpy(szFEC, "12");
			break;
		case 2:
			lstrcpy(szFEC, "23");
			break;
		case 3:
			lstrcpy(szFEC, "34");
			break;
		case 4:
			lstrcpy(szFEC, "56");
			break;
		case 5:
			lstrcpy(szFEC, "78");
			break;
		default:
			lstrcpy(szFEC, "00");
			break;
		}
		wsprintf(szINIString, "%d,%c,%d,%s,%s",
				 pNITData->nFrequency / 100,
				 cPolarity,
				 pNITData->dvbs.nSymbolRate / 10,
				 szFEC,
				 pNITData->szNetworkName);
		break;
	case 2:		// 8PSK
		if (v->fExportNITINITurbo)
		{
			int nModulation = 2;
			int nFEC = 0;
			wsprintf(szINIString, "%d,%c,%d,%d,%d,%s",
					 pNITData->nFrequency / 100,
					 cPolarity,
					 pNITData->dvbs.nSymbolRate / 10,
					 nFEC,
					 nModulation,
					 pNITData->szNetworkName);
			fADVMux = TRUE;
		}
		else
		{
			// todo - DVB-S2
		}
		break;
	case 3:		// 16QAM
		if (v->fExportNITINITurbo)
		{
			int nModulation = 1;
			int nFEC = 2;
			wsprintf(szINIString, "%d,%c,%d,%d,%d,%s",
					 pNITData->nFrequency / 100,
					 cPolarity,
					 pNITData->dvbs.nSymbolRate / 10,
					 nFEC,
					 nModulation,
					 pNITData->szNetworkName);
			fADVMux = TRUE;
		}
		else
		{
			// todo - DVB-S2
		}
		break;
	}
	if (lstrlen(szINIString))
	{
		char szMuxCount[32];

		if (!fADVMux)
		{
			pNetworkToINIList->nDVBSignals++;
			wsprintf(szMuxCount, "%d", pNetworkToINIList->nDVBSignals);
			WritePrivateProfileString("DVB", szMuxCount, szINIString, pNetworkToINIList->szFilename);
		}
		else
		{
			pNetworkToINIList->nADVSignals++;
			wsprintf(szMuxCount, "%d", pNetworkToINIList->nADVSignals);
			WritePrivateProfileString("ADV", szMuxCount, szINIString, pNetworkToINIList->szFilename);
		}
	}
}

int ExportNITAsINIFiles(HWND hWnd)
{
	int nNITIndex;
	int i;
	int nTotalMuxesWritten = 0;
	PNETWORKTOINILIST pNetworkToINIList = (PNETWORKTOINILIST)LocalAlloc(LPTR, sizeof(NETWORKTOINILIST) * MAX_NETWORK_TO_INI_LIST);
	char szExportNITINIFolder[MAX_PATH];

	lstrcpy(szExportNITINIFolder, v->szExportNITINIFolder);
	if (szExportNITINIFolder[lstrlen(szExportNITINIFolder) - 1] != '\\')
		lstrcat(szExportNITINIFolder, "\\");

	for (i = 0; i < MAX_NETWORK_TO_INI_LIST; i++)
		pNetworkToINIList[i].nNetworkID = -1;

	for (nNITIndex = 0; nNITIndex < MAX_TS_ENTRIES; nNITIndex++)
	{
		if (v->pNITData[nNITIndex] != NULL)
		{
			BOOL fIgnoredNetwork = FALSE;

			if (!v->fExportNITINIIncludeIgnored)
			{
				for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
				{
					if (v->nIgnoredNetworks[i] == 0)
						break;
					if (v->nIgnoredNetworks[i] == v->pNITData[nNITIndex]->nNetworkID)
					{
						fIgnoredNetwork = TRUE;
						break;
					}
				}
			}
			if (!fIgnoredNetwork)
			{
				if (v->pNITData[nNITIndex]->nType == NIT_DVBS)
				{
					for (i = 0; i < MAX_NETWORK_TO_INI_LIST; i++)
					{
						if (v->pNITData[nNITIndex]->nNetworkID == pNetworkToINIList[i].nNetworkID)
						{
							// existing network
							AddNetworkToINI(v->pNITData[nNITIndex], &pNetworkToINIList[i]);
							break;
						}
						if (pNetworkToINIList[i].nNetworkID == -1)
						{
							int nOrbitalLocation;
							char szTempFile[MAX_PATH];

							// firt time on the network
							pNetworkToINIList[i].nNetworkID = v->pNITData[nNITIndex]->nNetworkID;
							if (v->pNITData[nNITIndex]->dvbs.fEastern)
								nOrbitalLocation = v->pNITData[nNITIndex]->dvbs.nOrbitalPosition;
							else
								nOrbitalLocation = 3600 - v->pNITData[nNITIndex]->dvbs.nOrbitalPosition;
							
							// Make a BAK file in case the INI file was already there
							wsprintf(pNetworkToINIList[i].szFilename, "%s%04d.ini", szExportNITINIFolder, nOrbitalLocation);
							wsprintf(szTempFile, "%s%04d.bak", szExportNITINIFolder, nOrbitalLocation);
							DeleteFile(szTempFile);
							MoveFile(pNetworkToINIList[i].szFilename, szTempFile);
							pNetworkToINIList[i].hFile = CreateFile(pNetworkToINIList[i].szFilename,
											  GENERIC_WRITE,
											  0,
											  (LPSECURITY_ATTRIBUTES) NULL,
											  CREATE_ALWAYS,
											  FILE_ATTRIBUTE_NORMAL,
											  (HANDLE) NULL);
							if (pNetworkToINIList[i].hFile != INVALID_HANDLE_VALUE)
							{
								DWORD dwWritten;
								char szSATTypeHeader[256];

								wsprintf(szSATTypeHeader, "[SATTYPE]\r\n1=%04d\r\n2=%s\r\n\r\n",
									     nOrbitalLocation,
										 v->pNITData[nNITIndex]->szNetworkName);
								WriteFile(pNetworkToINIList[i].hFile, szSATTypeHeader, lstrlen(szSATTypeHeader), &dwWritten, NULL);
								CloseHandle(pNetworkToINIList[i].hFile);
								AddNetworkToINI(v->pNITData[nNITIndex], &pNetworkToINIList[i]);
							}
							break;
						}
					}
				}
			}
		}
	}

	for (i = 0; i < MAX_NETWORK_TO_INI_LIST; i++)
	{
		char szTemp[64];
		
		if (pNetworkToINIList[i].nNetworkID == -1)
			break;
		if (pNetworkToINIList[i].nDVBSignals)
		{
			wsprintf(szTemp, "%d", pNetworkToINIList[i].nDVBSignals);
			WritePrivateProfileString("DVB", "0", szTemp, pNetworkToINIList[i].szFilename);
		}
		if (pNetworkToINIList[i].nADVSignals)
		{
			wsprintf(szTemp, "%d", pNetworkToINIList[i].nADVSignals);
			WritePrivateProfileString("ADV", "0", szTemp, pNetworkToINIList[i].szFilename);
		}
		nTotalMuxesWritten += pNetworkToINIList[i].nDVBSignals;
		nTotalMuxesWritten += pNetworkToINIList[i].nADVSignals;
	}

	LocalFree(pNetworkToINIList);
	return nTotalMuxesWritten;
}

BOOL CALLBACK ExportNITAsINIDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SetDlgItemText(hDlg, IDC_EXPORT_INI_FOLDER, v->szExportNITINIFolder);
			if (v->fExportNITINITurbo)
				CheckDlgButton(hDlg, IDC_EXPORT_NIT_TURBO, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_EXPORT_NIT_DVBS2, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_EXPORT_NIT_INCLUDE_IGNORED, v->fExportNITINIIncludeIgnored);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			GetDlgItemText(hDlg, IDC_EXPORT_INI_FOLDER, v->szExportNITINIFolder, sizeof(v->szExportNITINIFolder));
			v->fExportNITINITurbo = IsDlgButtonChecked(hDlg, IDC_EXPORT_NIT_TURBO);
			v->fExportNITINIIncludeIgnored = IsDlgButtonChecked(hDlg, IDC_EXPORT_NIT_INCLUDE_IGNORED);
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}
	return FALSE;
}

void ToggleNetworkIgnore(HWND hWnd)
{
	BOOL fAlreadyIgnored = FALSE;
	int i;
	int nNITIndex;

	// See if the network is ignored
	for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
	{
		if (v->nIgnoredNetworks[i] == 0)
			break;
		if (v->nIgnoredNetworks[i] == v->pNITData[v->nNITRightClickIndex]->nNetworkID)
		{
			fAlreadyIgnored = TRUE;
			break;
		}
	}
	if (fAlreadyIgnored)
	{
		// Ignored - remove from list
		int nNewIgnoreList[MAX_IGNORED_NETWORKS];
		int nOutputIndex = 0;

		memset(nNewIgnoreList, 0, sizeof(nNewIgnoreList));
		for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
		{
			if (v->nIgnoredNetworks[i] == 0)
				break;
			if (v->nIgnoredNetworks[i] != v->pNITData[v->nNITRightClickIndex]->nNetworkID)
				nNewIgnoreList[nOutputIndex++] = v->nIgnoredNetworks[i];
		}
		memcpy(v->nIgnoredNetworks, nNewIgnoreList, sizeof(v->nIgnoredNetworks));
	}
	else
	{
		// Not ignored - add to list
		for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
		{
			if (v->nIgnoredNetworks[i] == 0)
			{
				v->nIgnoredNetworks[i] = v->pNITData[v->nNITRightClickIndex]->nNetworkID;
				break;
			}
		}
	}

	// Go through each NIT entry and update it's icon
	for (nNITIndex = 0 ; nNITIndex < MAX_TS_ENTRIES; nNITIndex++)
	{
		if (v->pNITData[nNITIndex] != NULL)
		{
			BOOL fIgnoredNetwork = FALSE;
			int nIcon = 9;
			TVITEM tvi;

			memset(&tvi, 0, sizeof(tvi));
			for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
			{
				if (v->nIgnoredNetworks[i] == 0)
					break;
				if (v->nIgnoredNetworks[i] == v->pNITData[nNITIndex]->nNetworkID)
				{
					fIgnoredNetwork = TRUE;
					break;
				}
			}
			if (fIgnoredNetwork)
			{
				nIcon = 27;
			}
			else
			{
				if (!v->pNITData[nNITIndex]->fThisTS)
					nIcon = 25;
			}
			tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
			tvi.hItem = v->pNITData[nNITIndex]->hNITTreeItem;
			tvi.iSelectedImage = tvi.iImage = nIcon;
			TreeView_SetItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), &tvi);
		}
	}

	// Now go through any existing entries and remove them if they're ignored now
	if (!fAlreadyIgnored)
	{
		int nSDTIndex;

		for (nSDTIndex = 0; nSDTIndex < MAX_EIT_CHANNEL_DATA; nSDTIndex++)
		{
			BOOL fRemoveThisChannel = FALSE;

			if (v->pChannelData[nSDTIndex] != NULL)
			{
				EnterCriticalSection(&v->csEIT);
				for (i = 0; i < MAX_IGNORED_NETWORKS; i++)
				{
					if (v->nIgnoredNetworks[i] == 0)
						break;
					if (v->nIgnoredNetworks[i] == v->pChannelData[nSDTIndex]->nOriginalNetworkID)
					{
						fRemoveThisChannel = TRUE;
						break;
					}
				}
				if (fRemoveThisChannel)
				{
					int j;

					for (j = 0; j < MAX_SDT_EXTRA_DESCRIPTORS; j++)
					{
						if (v->pChannelData[nSDTIndex]->pExtraDescriptors[j] != NULL)
						{
							LocalFree(v->pChannelData[nSDTIndex]->pExtraDescriptors[j]);
							v->pChannelData[nSDTIndex]->pExtraDescriptors[j] = NULL;
						}
					}
					TreeView_DeleteItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), v->pChannelData[nSDTIndex]->hSDTTreeItem);
					LocalFree(v->pChannelData[nSDTIndex]);
					v->pChannelData[nSDTIndex] = NULL;

					if (v->pEvents[nSDTIndex] != NULL)
					{
						PEITEVENT pCurrent;
						
						EnterCriticalSection(&v->csEIT);
						pCurrent = v->pEvents[nSDTIndex];
						do
						{
							PEITEVENT pNext = (PEITEVENT)pCurrent->dwNextEvent;
							for (j = 0; j < MAX_EIT_EXTRA_DESCRIPTORS; j++)
							{
								if (pCurrent->pExtraDescriptors[j] != NULL)
								{
									LocalFree(pCurrent->pExtraDescriptors[j]);
									pCurrent->pExtraDescriptors[j] = NULL;
								}
							}
							if (pCurrent->szShortEventDescription != NULL)
								LocalFree(pCurrent->szShortEventDescription);
							if (pCurrent->szLongEventDescription != NULL)
								LocalFree(pCurrent->szLongEventDescription);
							LocalFree(pCurrent);
							pCurrent = pNext;
						} while (pCurrent != NULL);
						v->pEvents[nSDTIndex] = NULL;
						TreeView_DeleteItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), v->hEITTreeItem[nSDTIndex]);
						v->hEITTreeItem[nSDTIndex] = NULL;								
						LeaveCriticalSection(&v->csEIT);
					}
				}
				LeaveCriticalSection(&v->csEIT);
			}
		}
	}
}
#endif LITE

BOOL CALLBACK SIParserDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL fFirstTime;

	switch(uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hRealDC = BeginPaint(hDlg, &ps);

			if (v->hThumbnailDC != NULL)
			{
				int xStart, yStart, xWidth, yHeight;

				GetVideoArea(&xStart, &yStart, &xWidth, &yHeight);
				BitBlt(hRealDC,
				       xStart, yStart, 
					   xWidth, yHeight,
					   v->hThumbnailDC,
					   0, 0, SRCCOPY);
			}			
			EndPaint(hDlg, &ps);
		}
		break;
	case WM_INITDIALOG:
		{
			static int nSubclassItemList[] = {IDC_SI_TREE, IDC_SI_TEXT, IDC_PID_CHART_DISABLE, IDC_SORT_DECENDING, IDC_SORT_RATE, IDC_SORT_PID, 0};
			int i, j;
			int nMonitorWidth, nMonitorHeight;
			RECT rcTSReader;

			v->fSourceInitFailed = FALSE;

			v->hDialogIcon = LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVBSMALL_LOGO));
			GetWindowRect(hDlg, &rcTSReader);
			nMonitorWidth = GetSystemMetrics(SM_CXFULLSCREEN);
			nMonitorHeight = GetSystemMetrics(SM_CYFULLSCREEN);
			/*SetWindowPos(hDlg, HWND_BOTTOM,
				         (nMonitorWidth / 2) - ((rcTSReader.right - rcTSReader.left) / 2),
				         (nMonitorHeight / 2) - ((rcTSReader.bottom - rcTSReader.top) / 2),
						 0, 0,
						 SWP_NOSIZE | SWP_NOZORDER | SWP_NOSENDCHANGING | SWP_NOOWNERZORDER);*/
			
			//UpdateMainStatusText("");
			InitSITreeViewImageLists(GetDlgItem(hDlg, IDC_SI_TREE));

			v->ss.hWndTSReader = v->hDlgSIParser = hDlg;
			v->hNITRootTreeItem = v->hSDTRootTreeItem = v->hEITRootTreeItem = NULL;
			v->nMinimumSDTChannel = 65536;
			v->nMinimumEITChannel = 65536;

			CursorWait(hDlg);

			ResetParserPackets();
			ResetParserCRCs();
#ifdef PRO
			ResetParserTableErrors();
			ResetTableTimes();
			ResetTableTimingErrors();
#endif PRO
			SetupFonts(hDlg);
			SetupServiceBitmaps(hDlg, TRUE);

			if (Init(&v->ss) == FALSE)
			{
				v->fSourceInitFailed = TRUE;
#ifndef PRO
				MessageBox(hDlg, "The source module you selected didn't initialize. The device may be missing or not installed correctly.\n\nTo change source module, launch TSReader with the Ctrl key held down.", gszAppName, MB_OK | MB_ICONSTOP);
#else PRO
				MessageBox(hDlg, "The source module you selected didn't initialize. The device may be missing or not installed correctly.\n\nUse the profile browser to change source modules.", gszAppName, MB_OK | MB_ICONSTOP);
#endif PRO
				//v->szSourceName[0] = 0;
				DeInit();
				EndDialog(hDlg, FALSE);
				break;
			}

			// Refresh capabilities after Init() as they may have changed
			if (GetDescription != NULL)
				GetDescription(NULL, NULL, NULL, &v->nMaxSourcePIDs, &v->dwSourceCapabilities);
#ifndef LITE
			if ( (v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) && (v->ss.fSerialReceiverControlEnabled == TRUE) )
			{
				if (SourceHelper_InitSerialControl() == FALSE)
					v->ss.fSerialReceiverControlEnabled = FALSE;
			}
#endif LITE

			memset(&v->pat, 0, sizeof(v->pat));
			v->pat.nVersionNumber = -1;
			for (j = 0; j < MAX_TS_BUFFERS; j++)
			{
				v->ss.tsb[j].pData = LocalAlloc(LPTR, TS_BUFFER_SIZE);			
				if (v->ss.fTimestampPackets == TRUE)
					v->ss.tsb[j].pTimestamps = LocalAlloc(LPTR, sizeof(DWORD) * TS_PACKETS_AT_A_TIME);
			}
			
			if (v->fSortChartByPID == FALSE)
				CheckDlgButton(hDlg, IDC_SORT_RATE, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_SORT_PID, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_SORT_DECENDING, v->fSortChartDecending);

			if (v->fXNSInterface)
			{
				if (XNS_SetupSocket(v) == FALSE)
					v->fXNSInterface = FALSE;
			}
			EnableDisablePIDChartButtons(hDlg);
			CheckDlgButton(hDlg, IDC_PID_CHART_DISABLE, v->fPIDChartDisabled);
			fFirstTime = TRUE;

			CreateTooltip(hDlg);
			SetWindowLong(hDlg, GWL_STYLE, GetWindowLong(hDlg, GWL_STYLE) | CS_DBLCLKS);

			for (i = 0; nSubclassItemList[i] != 0; i++)
			{				
				v->hWndMainDlgSubclass[i] = GetDlgItem(hDlg, nSubclassItemList[i]);
				v->wpSubclassOrigProc[i] = (WNDPROC)SetWindowLong(v->hWndMainDlgSubclass[i], GWL_WNDPROC, (LONG)MainDlgSubclassProc); 
			}	
		}
		break;
	case WM_DESTROY:
		{
			int j;

			for (j = 0; v->hWndMainDlgSubclass[j] != NULL; j++)
				SetWindowLong(v->hWndMainDlgSubclass[j], GWL_WNDPROC, (LONG)v->wpSubclassOrigProc[j]); 

			KillTimer(hDlg, 1);
			KillTimer(hDlg, 2);
			DeleteObject(v->hPIDFont);
			DeleteObject(v->hSourceInfoFont);
			DeleteObject(v->hCourierNew);
			ImageList_Destroy(v->hSIParserImageList);
			for (j = 0; j < MAX_TS_BUFFERS; j++)
			{
				if (v->ss.tsb[j].pData != NULL)
					LocalFree(v->ss.tsb[j].pData);
				if (v->ss.tsb[j].pTimestamps != NULL)
					LocalFree(v->ss.tsb[j].pTimestamps);
			}
			if (v->hBr != NULL)
			{
				DeleteObject(v->hBr);
				DeleteObject(v->hBrRed);
				DeleteObject(v->hBrGreen);
				DeleteObject(v->hBrDarkRed);
				DeleteObject(v->hBrDarkGreen);
				DeleteObject(v->hSourceInfoBrush1);
				DeleteObject(v->hSourceInfoBrush2);
				DeleteObject(v->hRedPen);
				DeleteObject(v->hGreenPen);
			}
			DestroyIcon(v->hDialogIcon);
			SetupServiceBitmaps(hDlg, FALSE);
			OutputDebugString("TSReader: WM_DESTROY done for dlg\n");
		}
		break;
	case WM_DROPFILES:
		{
			HDROP hDrop = (HDROP) wParam;
			int nFileCount = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
			if (nFileCount == 1)
			{
				DragQueryFile(hDrop, 0, v->ss.szDropFilename, sizeof(v->ss.szDropFilename));
				RestartTSReader(v->hWndMainWindow);
			}
			DragFinish(hDrop);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_IDRNITPOPUP_IGNORETHISNETWORK:
#ifndef LITE
			if (v->nNITRightClickIndex != -1)		
				ToggleNetworkIgnore(v->hWndMainWindow);
#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
			break;
#ifdef PRO
		case ID_IDPCRPOPUP_USETHISPIDFORMUXRATE:
			{
				int nPID;

				if (v->nSelectedPCRPID == 0)
					break;

				EnterCriticalSection(&v->ss.csPIDCounter);
				for (nPID = 0; nPID < 8192; nPID++)
				{
					v->lnPIDCounter[nPID] = 0;
					v->fPIDActive[nPID] = 0;
					v->dPIDRate[nPID] = 0;
					v->lnPIDRateSamples[nPID] = 0;
					v->lnPIDRateBytes[nPID] = 0;
					v->lnPIDRatePCR[nPID] = 0;
				}
				v->lnTotalTSPackets = 0;
				v->nMuxRatePID = v->nSelectedPCRPID;
				v->dDisplayMuxRate = 0;
				v->nMuxRateCounter = 0;
				v->lnMuxRatePCR = 0;
				v->nMuxRateBytes = 0;
				LeaveCriticalSection(&v->ss.csPIDCounter);
				UpdatePIDChart(v->hDlgSIParser);
				UpdateStatistics(v->hDlgSIParser, FALSE);
			}
			break;
#endif PRO
		case ID_IDRNITPOPUP_RETUNETOTHISMUX:
#ifndef LITE
			RetuneToThisMux(v->hWndMainWindow);
#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
			break;
#ifndef LITE
		case ID_NIT_EXPORT_EXPORT_TO_INI:
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_EXPORT_NIT_INI), hDlg, ExportNITAsINIDlgProc) == TRUE)
			{
				int nMuxes = ExportNITAsINIFiles(hDlg);

				if (!nMuxes)
					MessageBox(hDlg, "No muxes were written - perhaps this network doesn't include satellite muxes?", gszAppName, MB_ICONINFORMATION);
				else
				{
					char szTemp[128];
					wsprintf(szTemp, "%d muxes written to INI files", nMuxes);
					MessageBox(hDlg, szTemp, gszAppName, MB_ICONINFORMATION);
				}
			}
			break;
#endif LITE
		case ID_IDRNITPOPUP_RETUNETOTHISMUX_SDT:
#ifndef LITE
			if (v->nSDTRightClickIndex != -1)
			{
				if (v->pChannelData[v->nSDTRightClickIndex] != NULL)
				{
					if (v->dwSourceCapabilities & CAPABILITIES_TUNE_BY_CHANNEL)
					{
						if (v->fSkyEPG)
							RetuneToThisChannel(v->hWndMainWindow, v->pChannelData[v->nSDTRightClickIndex]->nLogicalChannelNumber);
						else
							RetuneToThisChannel(v->hWndMainWindow, v->pChannelData[v->nSDTRightClickIndex]->nChannelNumber);
					}
					else
					{
						int nNITIndex;

						for (nNITIndex = 0; nNITIndex < MAX_TS_ENTRIES; nNITIndex++)
						{
							if (v->pNITData[nNITIndex] != NULL)
							{
								if (v->pNITData[nNITIndex]->nTransportStreamID == v->pChannelData[v->nSDTRightClickIndex]->nTransportStreamID)
								{
									v->nNITRightClickIndex = nNITIndex;
									RetuneToThisMux(v->hWndMainWindow);
									break;
								}
							}
						}
					}
				}
			}
#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
			break;
#ifndef LITE
		case ID_IDRESPOPUP_REMOVEESBLACKLIST:
			{

				v->pat.pmt[v->nPopupSelectedPMTIndex].es[v->nPopupSelectedESIndex].nBlacklisted = 0;
				UpdateVideoPix(hDlg);
				SetDlgItemText(hDlg, IDC_SI_TEXT, FormatESEntry(v->pat.pmt[v->nPopupSelectedPMTIndex].es[v->nPopupSelectedESIndex].nESPID));
			}
			break;
		case ID_IDRTDTPOPUP_SETPCTIMETOTHISTIME:
			SetPCTimeToStreamTime(v->hWndMainWindow);
			break;
		case ID_IDRBATPOPUP_SELECTTHISBAT:
			if (v->nBATRightClickIndex != -1)
			{
				int nBATIndex;

				if (v->nCurrentBATID != v->bat[v->nBATRightClickIndex].bouquet_id)
				{
					for (nBATIndex = 0; nBATIndex < MAX_BAT_ENTRIES; nBATIndex++)
					{
						TVITEM tvi;

						if (v->bat[nBATIndex].bouquet_id == 0)
							break;

						memset(&tvi, 0, sizeof(tvi));
						tvi.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
						tvi.hItem = v->bat[nBATIndex].hTreeItem;
						if (v->bat[nBATIndex].bouquet_id == v->nCurrentBATID)
						{
							tvi.iSelectedImage = tvi.iImage = 23;	// unselected BAT
							TreeView_SetItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), &tvi);
						}
						else if (v->bat[nBATIndex].bouquet_id == v->bat[v->nBATRightClickIndex].bouquet_id)
						{
							tvi.iSelectedImage = tvi.iImage = 26;	// selected BAT
							TreeView_SetItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), &tvi);
						}
					}
				}
				v->nCurrentBATID = v->bat[v->nBATRightClickIndex].bouquet_id;
				if (v->fSkyEPG)
					UpdateSkyEPGMap(v->nCurrentBATID);
				v->nBATRightClickIndex = -1;
			}
			break;
#endif LITE
		case IDCANCEL:
			PostMessage(v->hWndMainWindow, WM_CLOSE, 0, 0);
			break;
		case IDC_PID_CHART_DISABLE:
			v->fPIDChartDisabled = IsDlgButtonChecked(hDlg, IDC_PID_CHART_DISABLE);
			EnableDisablePIDChartButtons(hDlg);
			if (v->fPIDChartDisabled)
			{
				InvalidateRect(hDlg, NULL, TRUE);
				UpdateWindow(hDlg);
				UpdateVideoPix(hDlg);
				UpdateStatistics(hDlg, FALSE);
				UpdateSourceInfo(hDlg);
			}
			else
				UpdatePIDChart(hDlg);
			break;
		case IDC_SORT_RATE:
			v->fSortChartByPID = FALSE;
			UpdatePIDChart(hDlg);
			break;
		case IDC_SORT_PID:
			v->fSortChartByPID = TRUE;
			UpdatePIDChart(hDlg);
			break;
		case IDC_SORT_DECENDING:
			v->fSortChartDecending = IsDlgButtonChecked(hDlg, IDC_SORT_DECENDING);
			UpdatePIDChart(hDlg);
			break;
		case ID_IDRIPPOPUP_STOPSAVING:
		case ID_IDRIPPOPUP_SAVEPAYLOAD:
		case ID_IDRIPPOPUP_SAVEPAYLOADANDXXXIPHEADER:
		case ID_IDRIPPOPUP_SAVEPAYLOADXXXIPHEADERANDMPEHEADER:
	#ifndef LITE
			if (v->pLastClickedIPEntry != NULL)
			{
				int nIPSaveMode;

				switch(LOWORD(wParam))
				{
				case ID_IDRIPPOPUP_SAVEPAYLOAD:
					nIPSaveMode = IP_SAVE_PAYLOAD;
					break;
				case ID_IDRIPPOPUP_SAVEPAYLOADANDXXXIPHEADER:
					nIPSaveMode = IP_SAVE_PAYLOAD_IP;
					break;
				case ID_IDRIPPOPUP_SAVEPAYLOADXXXIPHEADERANDMPEHEADER:
					nIPSaveMode = IP_SAVE_PAYLOAD_IP_MPE;
					break;
				}
				RecordSelectedIPStream(hDlg, LOWORD(wParam) != ID_IDRIPPOPUP_STOPSAVING, nIPSaveMode);
			}
	#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
	#endif LITE
			break;
		case ID_IDRIPPOPUP_SAVEPAYLOAD_MAC:
		case ID_IDRIPPOPUPMAC_SAVEALLPAYLOADANDXXXIPHEADERSFORMAC:
		case ID_IDRIPPOPUPMAC_SAVEALLPAYLOADXXXIPHEADERANDMPEHEADERSFORMAC:
		case ID_IDRIPPOPUP_STOPSAVING_MAC:
	#ifndef LITE
			if (v->pLastClickedIPMACEntry != NULL)
			{
				int nIPSaveMode = 0;
				switch(LOWORD(wParam))
				{
				case ID_IDRIPPOPUP_SAVEPAYLOAD_MAC:
					nIPSaveMode = IP_SAVE_PAYLOAD;
					break;
				case ID_IDRIPPOPUPMAC_SAVEALLPAYLOADANDXXXIPHEADERSFORMAC:
					nIPSaveMode = IP_SAVE_PAYLOAD_IP;
					break;
				case ID_IDRIPPOPUPMAC_SAVEALLPAYLOADXXXIPHEADERANDMPEHEADERSFORMAC:
					nIPSaveMode = IP_SAVE_PAYLOAD_IP_MPE;
					break;
				}
				RecordSelectedIPMACStream(hDlg, LOWORD(wParam) != ID_IDRIPPOPUP_STOPSAVING_MAC, nIPSaveMode);
			}
	#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
	#endif LITE
			break;
		case ID_IDRIPPOPUP_SAVEPAYLOAD_PID:
		case ID_IDRIPPOPUPPID_SAVEALLPAYLOADANDXXXIPHEADERSONPID:
		case ID_IDRIPPOPUPPID_SAVEALLPAYLOADXXXIPHEADERANDMPEHEADERSONPID:
		case ID_IDRIPPOPUP_STOPSAVING_PID:
	#ifndef LITE
			if (v->pLastClickedIPPIDEntry != NULL)
			{
				int nIPSaveMode = 0;

				switch(LOWORD(wParam))
				{
				case ID_IDRIPPOPUP_SAVEPAYLOAD_PID:
					nIPSaveMode = IP_SAVE_PAYLOAD;
					break;
				case ID_IDRIPPOPUPPID_SAVEALLPAYLOADANDXXXIPHEADERSONPID:
					nIPSaveMode = IP_SAVE_PAYLOAD_IP;
					break;
				case ID_IDRIPPOPUPPID_SAVEALLPAYLOADXXXIPHEADERANDMPEHEADERSONPID:
					nIPSaveMode = IP_SAVE_PAYLOAD_IP_MPE;
					break;
				}
				RecordSelectedIPPIDStream(hDlg, LOWORD(wParam) != ID_IDRIPPOPUP_STOPSAVING_PID, nIPSaveMode);
			}
	#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
	#endif LITE
			break;
		case ID_IDRIPPOPUP_RETRANSMITPAYLOAD:
		case ID_IDRIPPOPUP_STOPTRANSMITTING:
	#ifndef LITE
			if (v->pLastClickedIPEntry != NULL)
			{
				ReTransmitSelectedIPStream(hDlg, LOWORD(wParam) == ID_IDRIPPOPUP_RETRANSMITPAYLOAD);
			}
	#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
	#endif LITE
			break;
		case ID_IDRIPPOPUP_RETRANSMITPAYLOAD_MAC:
		case ID_IDRIPPOPUP_STOPTRANSMITTING_MAC:
	#ifndef LITE
			if (v->pLastClickedIPMACEntry != NULL)
			{
				ReTransmitSelectedIPMACStream(hDlg, LOWORD(wParam) == ID_IDRIPPOPUP_RETRANSMITPAYLOAD_MAC);
			}
	#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
	#endif LITE
			break;
		case ID_IDRIPPOPUP_RETRANSMITPAYLOAD_PID:
		case ID_IDRIPPOPUP_STOPTRANSMITTING_PID:
	#ifndef LITE
			if (v->pLastClickedIPPIDEntry != NULL)
			{
				ReTransmitSelectedIPPIDStream(hDlg, LOWORD(wParam) == ID_IDRIPPOPUP_RETRANSMITPAYLOAD_PID);
			}
	#else LITE
			MessageBox(hDlg, szLiteWarning, gszAppName, MB_ICONSTOP);
	#endif LITE
			break;
		}
		break;
	case WM_TIMER:
		switch(wParam)
		{
		case 1:			// 1Hz constant running
#ifdef BETA
			{
				SYSTEMTIME systime;
					
				GetLocalTime(&systime);
				if (systime.wYear >= BETA_EXPIRE_YEAR)
				{
					if ( (systime.wMonth > BETA_EXPIRE_MONTH) || (systime.wYear > BETA_EXPIRE_YEAR) )
					{
						PostMessage(hDlg, WM_CLOSE, 0, 0);
						break;
					}
				}
			}
#endif BETA

			{
				static HANDLE hTeleTextPlugin = NULL;
				HANDLE hWndCheck = FindWindow("MDTeleText", "MDTeleText");
				if (hWndCheck != NULL)
				{
					if (hTeleTextPlugin == NULL)
					{
						int i;

						hTeleTextPlugin = hWndCheck;
						for (i = 0; i < 16; i++)
						{
							if (v->hPluginTranslateDialog[i] == NULL)
							{
								v->hPluginTranslateDialog[i] = hTeleTextPlugin;
								break;
							}
						}
					}
				}
				else
				{
					if (hTeleTextPlugin != NULL)
					{
						int i;

						for (i = 0; i < 16; i++)
						{
							if (v->hPluginTranslateDialog[i] == hTeleTextPlugin)
							{
								v->hPluginTranslateDialog[i] = NULL;
								break;
							}
						}
						hTeleTextPlugin = NULL;
					}
				}				
			}	
			
			keybd_event(VK_F24, 0, KEYEVENTF_KEYUP,0);		// prevent hibernation
			v->nPMTTimeoutCounter++;
			
			// Update counter for data stop
#ifndef LITE
			if (v->fAutoRestartOnDataStop && v->fDataReceviedInParseIncomingDataThread && !IsFileSource())
			{
				BOOL fRestart = FALSE;

				EnterCriticalSection(&v->csAutoRestartOnDataStopCounter);
				if (v->nAutoRestartOnDataStopCounter++ == v->nAutoRestartOnDataStopDelay)
					fRestart = TRUE;
				LeaveCriticalSection(&v->csAutoRestartOnDataStopCounter);
				if (fRestart)
				{
					if (v->fAutoRestartNoTuneDialog == TRUE)
						v->fAutoRestartNoDialogInProgress = TRUE;				
					PostMessage(v->hWndMainWindow, WM_USER + 10, 0, 0);
					PostMessage(v->hWndMainWindow, WM_COMMAND, ID_FILE_RESTART_SOURCE, 0);
				}
			}
#endif LITE
			// Update counter for remote control
			if (v->nTreeUpdateCounter2 != -1)
			{
				v->nTreeUpdateCounter2++;
				if (v->nTreeUpdateCounter2 == v->nAutoExportDelay)
					v->nTreeUpdateCounter2 = -1;
			}

			// Update counter for auto export
			if (v->nTreeUpdateCounter != -1)
			{
				v->nTreeUpdateCounter++;
				if (v->fAutoXMLExport == TRUE)
				{
					char szTemp[128];

					wsprintf(szTemp, "Auto export counter - %d", v->nTreeUpdateCounter);
					UpdateMainStatusText(szTemp);
				}
				if (v->nTreeUpdateCounter == v->nAutoExportDelay)
				{
					v->nTreeUpdateCounter = -1;
#ifndef LITE
					if (v->fAutoXMLExport == TRUE)
					{
						HANDLE hXMLFile;

						hXMLFile = CreateFile(v->szXMLExportFilename,
											  GENERIC_WRITE,
											  0,
											  (LPSECURITY_ATTRIBUTES) NULL,
											  CREATE_ALWAYS,
											  FILE_ATTRIBUTE_NORMAL,
											  (HANDLE) NULL);
						if (hXMLFile != INVALID_HANDLE_VALUE)					
						{
							if (v->fAutoXMLFormatAsXMLTV == -1)
							{
								UpdateMainStatusText("Auto exporting HTML");
								HTMLExport(hXMLFile, v->nAutoHTMLExportFlags, v->szXMLExportFilename);
							}
							else if (v->fAutoXMLFormatAsXMLTV == FALSE)
							{
								UpdateMainStatusText("Auto exporting XML");
								XMLExport(hDlg, hXMLFile);
							}
							else
							{
								UpdateMainStatusText("Auto exporting XMLTV");
								XMLTVExport(hDlg, hXMLFile);
							}
							CloseHandle(hXMLFile);
						}
						else
						{
							char szTemp[128];
							wsprintf(szTemp, "TSReader: Unable to open HTML/XML file for output - GLE = %d\n", GetLastError());
							OutputDebugString(szTemp);
						}
						PostMessage(v->hWndMainWindow, WM_CLOSE, 0, 0);
					}
#endif LITE
				}
			}

			UpdateStatistics(hDlg, TRUE);
			UpdateSourceInfo(hDlg);
			if (!v->fRealtimeCharting)
				UpdatePIDChart(hDlg);
			
			if 	(v->fAutoRecordFromControlServer == FALSE)
			{
				if (v->nAutoRecordSeconds)
				{
					v->nAutoRecordSecondCounter++;
					if (v->nAutoRecordSecondCounter >= v->nAutoRecordSeconds)
					{
						v->nAutoRecordSecondsReload = v->nAutoRecordSeconds;
						v->nAutoRecordSeconds = 0;
						if (v->nAutoRecord != AUTO_RECORD_NONE && v->nAutoRecord != AUTO_RECORD_VLC)
						{
							if (v->nAutoRecord == AUTO_RECORD_VLC_TIME_LIMITED)
								SendMessage(v->hWndMainWindow, WM_COMMAND, ID_PLAYBACK_VLC_1 + (v->nAutoVLCConfiguration - 1), 0);
							else
								SendMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_RECORD, 0);
							PostMessage(v->hWndMainWindow, WM_CLOSE, 0, 0);
						}
						else
							PostMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_RECORD, 0);
					}
					else
					{
						int nRemainingSeconds = v->nAutoRecordSeconds - v->nAutoRecordSecondCounter;
						int nHours, nMinutes;
						char szTemp[128];

						nHours = nRemainingSeconds / 3600;
						nRemainingSeconds -= nHours * 3600;
						nMinutes = nRemainingSeconds / 60;
						nRemainingSeconds -= nMinutes * 60;
						
						if (v->nAutoRecord != AUTO_RECORD_VLC_TIME_LIMITED)
						{
							if (v->nAutoRecord != AUTO_RECORD_NONE)
								wsprintf(szTemp, "%02d:%02d:%02d remaining in auto-record", nHours, nMinutes, nRemainingSeconds);
							else
								wsprintf(szTemp, "%02d:%02d:%02d remaining in record", nHours, nMinutes, nRemainingSeconds);
						}
						else
							wsprintf(szTemp, "%02d:%02d:%02d remaining in auto-streaming", nHours, nMinutes, nRemainingSeconds);
						UpdateMainStatusText(szTemp);
					}
				}
			}
			if (v->fCompletedESParsing == TRUE)
			{
				v->nESParsingCounter--;
				if (v->nESParsingCounter <= 0 && v->nESParsingCounterReload != 0)
					SendMessage(hDlg, WM_USER + 7, 0, 0);	// actually does the reload
			}
			break;
		case 2:		// 10 Hz for activity counter
			UpdateInputActivityPosition(v->nInputActivityPosition);
#ifdef PRO
			UpdateForwarderActivityPosition(v->nForwarderActivityPosition);
#endif PRO

			// and also for the statusbar
			if (v->hWndST != NULL)
			{
				EnterCriticalSection(&v->csStatusbar);
				if (v->fStatusDirty)
				{
					SendMessage(v->hWndST, SB_SETTEXT, (WPARAM)0, (LPARAM)v->szStatusTextMain);		
					SendMessage(v->hWndST, SB_SETTEXT, (WPARAM)1, (LPARAM)v->szStatusTextSecondary);		
					v->fStatusDirty = FALSE;
					v->nStatusNotDirtyCounter = 0;
				}
				else
				{
					if (v->nStatusNotDirtyCounter++ >= 50)
					{
						v->szStatusTextMain[0] = '\0';
						v->szStatusTextSecondary[0] = '\0';
						v->fStatusDirty = TRUE;
					}
				}
				LeaveCriticalSection(&v->csStatusbar);
			}
			break;
		case 3:		// 1 second delay for autorecord
			KillTimer(hDlg, 3);
			v->fAutoRecordFromControlServer = FALSE;
			switch(v->nAutoRecord)
			{
			case AUTO_RECORD_PROGRAM:
				if (   (lstrcmp(v->szAutoRecordFile, "D-VHS") == 0)
					|| (lstrcmp(v->szAutoRecordFile, "d-vhs") == 0) )
					PostMessage(v->hWndMainWindow, WM_COMMAND, ID_RECORD_RECORDPROGRAMTODVHS, 0);
				else if (   (lstrcmp(v->szAutoRecordFile, "Roku") == 0)
					|| (lstrcmp(v->szAutoRecordFile, "roku") == 0) )
					PostMessage(v->hWndMainWindow, WM_COMMAND, ID_PLAYBACK_ROKUHD1000, 0);
				else
					PostMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_RECORD, 0);
				break;
			case AUTO_RECORD_ALL:
				PostMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_RECORD_ALL, 0);
				break;
			case AUTO_RECORD_VLC:
			case AUTO_RECORD_VLC_TIME_LIMITED:
				SendMessage(v->hWndMainWindow, WM_COMMAND, ID_PLAYBACK_VLC_1 + (v->nAutoVLCConfiguration - 1), 0);
				break;
			}
			break;
		case 4:		// post resize delay
			KillTimer(hDlg, 4);
			PostMessage(hDlg, WM_USER + 3, 0, 0);
			break;
		case 5:		// fires when the automatic Record PIDs is done
			KillTimer(hDlg, 5);
			PostMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_RECORD_PID, 0);
			PostMessage(v->hWndMainWindow, WM_CLOSE, 0, 0);
			break;
		case 6:		// 4 Hz for PID statistics
			UpdatePIDChartingData(hDlg);
			if (v->fRealtimeCharting)
				UpdatePIDChart(hDlg);
			if (v->fRecording && v->fMinimizedFlag && v->fHideWhenMinimized && !v->fRecordDialogStreamOnly)
			{
				HICON hTrayIcon = NULL;

				v->nTrayIconBlinkCounter++;
				if (v->nTrayIconBlinkCounter == 6)
					hTrayIcon = LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVB_LOGO_SMALL_RED));
				else if (v->nTrayIconBlinkCounter == 7)
				{
					hTrayIcon = LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVB_LOGO_SMALL_BLACK));
					v->nTrayIconBlinkCounter = 0;
				}

				if (hTrayIcon != NULL)
				{
					NOTIFYICONDATA tnid; 

					tnid.cbSize = sizeof(NOTIFYICONDATA); 
					tnid.hWnd = v->hWndMainWindow; 
					tnid.uID = 1; 
					tnid.uFlags = NIF_ICON; 
					tnid.hIcon = hTrayIcon; 
					Shell_NotifyIcon(NIM_MODIFY, &tnid); 
					DestroyIcon(hTrayIcon); 
				}
			}
			else 
			{
				if (v->fMinimizedFlag && v->fHideWhenMinimized && v->nTrayIconBlinkCounter)
				{
					HICON hTrayIcon = LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVB_LOGO_SMALL_BLACK));
					NOTIFYICONDATA tnid; 

					tnid.cbSize = sizeof(NOTIFYICONDATA); 
					tnid.hWnd = v->hWndMainWindow; 
					tnid.uID = 1; 
					tnid.uFlags = NIF_ICON; 
					tnid.hIcon = hTrayIcon; 
					Shell_NotifyIcon(NIM_MODIFY, &tnid); 
					DestroyIcon(hTrayIcon); 
					v->nTrayIconBlinkCounter = 0;
				}
			}			
			break;
		case 7:		// 500 ms delay after channel change
			KillTimer(hDlg, 7);
			v->fStradisAutostart = TRUE;
			StreamDecoder(v->hWndMainWindow);
			break;
		}
		break;
	case WM_VSCROLL:
		if ((HWND)lParam == GetDlgItem(hDlg, IDC_PIDS_SCROLL))
			HandlePIDScroll(hDlg, wParam, lParam);
		else
			HandleThumbnailScroll(hDlg, wParam, lParam);
		break;
	case WM_NOTIFY:
		SIParserMsgNotify(hDlg, uMsg, wParam, lParam);
		break;
	case WM_CLOSE:
		{
			int i;
			MSG msg;
		
#ifndef LITE
			if (v->fDirtyManualChannels == TRUE)
			{
				if (MessageBox(hDlg, "Manual channels have not been saved. Would you like to save them now?", gszAppName, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON1) == IDYES)
					SaveManualChannels(hDlg);
			}
#endif LITE
			SaveSettings();

			// Stop recording/streaming
			if (v->fRecording)
			{
				if (v->fStradisActive)
				{
					OutputDebugString("Shutdown stream decoder\n");
					StreamDecoder(v->hWndMainWindow);
				}
				else
				{
					OutputDebugString("Shutdown recording\n");
					RecordStream(v->hWndMainWindow, FALSE, -1);
				}
			}
			v->nGotKeys = GOT_DISABLE;

			// Shut the TS source down
			OutputDebugString("+Stop\n");
			if (v->fRunning)
			{
				Stop();
				v->fRunning = FALSE;
			}
			while (v->fParseThreadRunning == TRUE)
			{
				while (PeekMessage(&msg, hDlg, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				Sleep(10);
			}
			OutputDebugString("-Stop\n");

			// Make sure MPEG-2 decoder is shutdown
			OutputDebugString("TSReader: Waiting for thumbnail decoder thread\n");
			WaitForThumbnailThread(hDlg);

			// Let any outstanding messages from the SI parser thread
			// through before we delete stuff
			while (PeekMessage(&msg, hDlg, WM_USER + 2, WM_USER + 2, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, FALSE, 0);
			v->fDeletingAllTVItems = TRUE;
			TreeView_DeleteAllItems(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE));	
			v->fDeletingAllTVItems = FALSE;
			SendMessage(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), WM_SETREDRAW, TRUE, 0);
			v->fTreeViewSelectedAtLeastOnce = FALSE;

			if (v->fIPDVBMode == FALSE)
				CleanupMPEGParsingThread(hDlg);
			else
				CleanupIPParsingThread(hDlg);

			DeInit();

			for (i = 0; i < 5; i++)
			{
				MD__Unload_External_Dll(i);
			}
			MD__Shutdown();

#ifndef LITE
			if ( (v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) && (v->ss.fSerialReceiverControlEnabled == TRUE) )
				SourceHelper_DeInitSerialControl();
#endif LITE
			DestroyWindow(hDlg);
			break;
		}
		break;
	case WM_SIZE:
		ResizeDialogWindow(hDlg, wParam, lParam);
		break;
	case WM_USER + 2:
		if (v->fIPDVBMode == FALSE)
			HandleWMUSER2MPEG2Mode(hDlg, wParam, lParam);
		else
			HandleWMUSER2IPMode(hDlg, wParam, lParam);
		break;
	case WM_USER + 3:
		if (v->fStopping == TRUE)
			break;
		UpdateVideoPix(hDlg);
		if (lParam == 0)
		{
			UpdatePIDChart(hDlg);
			UpdateStatistics(hDlg, FALSE);
			UpdateSourceInfo(hDlg);
		}
		break;
	case WM_USER + 4:
		{
			char szFailureMessage[512];

			StreamDecoder(hDlg);		// turns off D-VHS recording if the filtergraph failed
			wsprintf(szFailureMessage, "Unable to record to D-VHS\n\n%s", v->szDVHSRecordFailureReason);
			MessageBox(hDlg, szFailureMessage, gszAppName, MB_ICONSTOP);
			lstrcat(szFailureMessage, "\n");
			OutputDebugString(szFailureMessage);
		}
		break;
	case WM_USER + 5:
		if (v->nThumbnailImageCount > 3)
		{
			int nMin, nMax;

			GetScrollRange(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), SB_CTL, &nMin, &nMax);
			if (v->nThumbnailImageCount > nMax)
				SetScrollRange(GetDlgItem(v->hDlgSIParser, IDC_SCROLL_THUMBNAILS), SB_CTL, 0, v->nThumbnailImageCount - 3, TRUE);
		}
		break;
	case WM_USER + 6:
		TreeView_DeleteItem(GetDlgItem(v->hDlgSIParser, IDC_SI_TREE), (HTREEITEM)lParam);
		break;
	case WM_USER + 7:
		{
			int nES;
			int nMaxThreads = 1;

			v->fPostInitialParse = TRUE;
			if (ThumbnailThreadsRunning())
			{
				v->nESParsingCounter++;
				break;
			}

			GetNextESPID(TRUE, 0);
#ifdef PRO
			if (!v->fWaitForCAThumbnail)
				nMaxThreads = v->nMaximumThumbnailThreads;
#endif PRO
			EnterCriticalSection(&v->csNextESPID);
			for (nES = 0; nES < nMaxThreads; nES++)
			{
				tv->nPESLength[nES] = 0;
				tv->nESFillPtr[nES] = 0;
				tv->fBufferSections[nES] = FALSE;
				if (GetNextESPID(FALSE, nES) == 0x1fff)
					break;
			}
			LeaveCriticalSection(&v->csNextESPID);
		}
		break;
	case WM_MOUSEMOVE:
		MouseMoveTooltips(hDlg, wParam, lParam);
		break;
	case WM_LBUTTONDBLCLK:
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (v->fRecording == FALSE && v->nSelectedProgram != -1)
			{
#ifdef PRO
				if (v->fDefaultPlaybackViaStradis)
					PostMessage(v->hWndMainWindow, WM_COMMAND, IDC_SI_PARSER_TO_STRADIS, 0);
				else
#endif PRO
					PostMessage(v->hWndMainWindow, WM_COMMAND, ID_PLAYBACK_VLC_1, 0);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			int nPMTIndex, nESIndex;
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			int nStartPMTIndex = GetPMTOffsetFromThumbnailScrollOffset();
			
			if (nStartPMTIndex == -1)
				nStartPMTIndex = 0;
			for (nPMTIndex = nStartPMTIndex; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
			{
				if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
					break;
				for (nESIndex = 0; nESIndex < MAX_ESLIST_ENTRIES; nESIndex++)
				{
					if (v->pat.pmt[nPMTIndex].es[nESIndex].nESPID == 0)
						break;
					if (v->pat.pmt[nPMTIndex].es[nESIndex].pRGBVideoFrame != NULL)
					{
						if (y >= v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.top
							&& y <= v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.bottom)
						{
							if (x >= v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.left
								&& x <= v->pat.pmt[nPMTIndex].es[nESIndex].rcThumbnail.right)
							{
								HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);
								TreeView_SelectItem(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
								TreeView_SelectSetFirstVisible(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem);
								nPMTIndex = MAX_PAT_ENTRIES;
								break;
							}
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void SetProcessPriority()
{
	BOOL fRetVal = SetPriorityClass(GetCurrentProcess(), v->nProcessPriority);
}

#ifdef _DEBUG
void DecodeSkyLCN()
{
	int nBATIndex;
	HANDLE hDebug;

	hDebug = CreateFile("c:\\tsreader-bat.txt", GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

	for (nBATIndex = 0; nBATIndex < MAX_BAT_ENTRIES; nBATIndex++)
	{
		int nTSIndex;
		int lcn;
		DWORD dwWritten;
		char szTemp[256];
		char szBouquetName[256] = {0};

		memset(v->channel_maps, 0, 65536 * 32 * sizeof(unsigned short));

		if (v->bat[nBATIndex].bouquet_id == 0)
			break;

		// Get the bouquet name
		if (v->bat[nBATIndex].bouquet_descriptors_length)
		{
			int nDescriptorsLength = v->bat[nBATIndex].bouquet_descriptors_length;
			BYTE * pDescriptors = v->bat[nBATIndex].bouquet_descriptors;

			while (nDescriptorsLength)
			{
				int nDescriptor = pDescriptors[0];
				int nDescriptorLength = pDescriptors[1];

				if (nDescriptor == 0x47)
				{
					set_buf(BM_USER_THREAD, pDescriptors, 0, FALSE);
					{
						int descriptor_tag = get_bits(BM_USER_THREAD, 8);
						int descriptor_length = get_bits(BM_USER_THREAD, 8);
						int nOutIndex = 0;

						while (descriptor_length)
						{
							szBouquetName[nOutIndex++] = get_bits(BM_USER_THREAD, 8);
							descriptor_length--;
						}
						szBouquetName[nOutIndex] = '\0';
					}
					break;
				}
				pDescriptors += nDescriptorLength + 2;
				nDescriptorsLength -= nDescriptorLength + 2;
			}
		}

		// Now the channels
		for (nTSIndex = 0; nTSIndex < MAX_BAT_TRANSPORT_ITEMS; nTSIndex++)
		{
			if (v->bat[nBATIndex].batts[nTSIndex].transport_stream_id == 0)
				break;
			if (v->bat[nBATIndex].batts[nTSIndex].transport_descriptors_length)
			{
				int nDescriptorsLength = v->bat[nBATIndex].batts[nTSIndex].transport_descriptors_length;
				BYTE * pDescriptors = v->bat[nBATIndex].batts[nTSIndex].transport_descriptors;

				while (nDescriptorsLength)
				{
					int nDescriptor = pDescriptors[0];
					int nDescriptorLength = pDescriptors[1];

					if (nDescriptor == 0xb1)
					{
						set_buf(BM_USER_THREAD, pDescriptors, 0, FALSE);
						{
							int descriptor_tag = get_bits(BM_USER_THREAD, 8);
							int descriptor_length = get_bits(BM_USER_THREAD, 8);

							int type = get_bits(BM_USER_THREAD, 8);
							descriptor_length--;
							while (descriptor_length > 9)
							{
								int flag = get_bits(BM_USER_THREAD, 8);
								int service_id = get_bits(BM_USER_THREAD, 16);
								int service_type = get_bits(BM_USER_THREAD, 8);
								int epg_id = get_bits(BM_USER_THREAD, 16);
								int lcn = get_bits(BM_USER_THREAD, 16);
								int unknown = get_bits(BM_USER_THREAD, 8);

								descriptor_length -= 9;

								if (lcn != 0xffff)
								{
									int j;

									for (j = 0; j < 32; j++)
									{
										if (v->channel_maps[lcn][j] == 0)
										{
											v->channel_maps[lcn][j] = service_id | (epg_id << 16);
											break;
										}
										else
										{
											if ((int)(v->channel_maps[lcn][j] & 0xffff) == service_id)
												break;
										}
									}
								}
							}
						}
					}
					pDescriptors += nDescriptorLength + 2;
					nDescriptorsLength -= nDescriptorLength + 2;
				}
			}
		}


		// Now we can output
		wsprintf(szTemp, "BAT %s\r\n", szBouquetName);
		WriteFile(hDebug, szTemp, lstrlen(szTemp), &dwWritten, NULL);
		for (lcn = 0; lcn < 65535; lcn++)
		{
			if (v->channel_maps[lcn][0] & 0xffff)
			{
				int j;
				char szTemp[256];
				
				for (j = 0; j < 32; j++)
				{
					char szTemp2[32];
					char szLCNString[4] = {"LCN"};

					if ((v->channel_maps[lcn][j] & 0xffff) == 0)
						break;

					if (j)
						lstrcpy(szLCNString, " + ");
					if (v->pChannelData[v->channel_maps[lcn][j] & 0xffff] != NULL)
						wsprintf(szTemp, "%s %5d (%s):", szLCNString, lcn, v->pChannelData[v->channel_maps[lcn][j] & 0xffff]->szShortName);
					else
						wsprintf(szTemp, "%s %5d:", szLCNString, lcn);
					while (lstrlen(szTemp) < 50)
						lstrcat(szTemp, " ");
					wsprintf(szTemp2, "sn: %05d epg: %05d\r\n", v->channel_maps[lcn][j] & 0xffff, v->channel_maps[lcn][j] >> 16);
					lstrcat(szTemp, szTemp2);
					WriteFile(hDebug, szTemp, lstrlen(szTemp), &dwWritten, NULL);
				}
			}
		}
		lstrcpy(szTemp, "-----------------------------------------------------------------------------------------------------\r\n");
		WriteFile(hDebug, szTemp, lstrlen(szTemp), &dwWritten, NULL);

	}

	CloseHandle(hDebug);
}
#endif _DEBUG

#ifndef LITE
BOOL LoadPIDListFile(char * szInputFile)
{
	HANDLE hInputFile = CreateFile(szInputFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hInputFile == INVALID_HANDLE_VALUE)
		return FALSE;

	do
	{
		int nSelectPID;
		char szInputLine[128];

		if (SourceHelper_ReadLine(hInputFile, szInputLine, sizeof(szInputLine)) == 0)
			break;
		sscanf(szInputLine, "%x", &nSelectPID);
		if (nSelectPID >= 0 && nSelectPID <= 0x1fff)
		{
			EnterCriticalSection(&v->ss.csPIDCounter);
			v->lnPIDCounter[nSelectPID]++;
			LeaveCriticalSection(&v->ss.csPIDCounter);
		}

	} while (TRUE);
	CloseHandle(hInputFile);

	return TRUE;
}
#endif LITE

void LoadManualPIDList(HWND hWnd)
{
#ifdef LITE
	MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
	OPENFILENAME ofn;
	char szInputFile[MAX_PATH] = {0};

	memset( &(ofn), 0, sizeof(ofn));
	ofn.lStructSize	= sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szInputFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("PID List Files (*.pdl)\0*.pdl\0All Files (*.*)\0*.*\0\0");	
	ofn.lpstrTitle = TEXT("Load PID List");
	ofn.lpstrDefExt = TEXT("pdl");
	ofn.Flags =  OFN_HIDEREADONLY;
	ofn.lpstrInitialDir = v->szRecordPIDFolder;				
	ofn.Flags =  OFN_HIDEREADONLY;
	
	if (SourceHelper_myGetOpenFileName(&ofn) == TRUE)
	{
		if (LoadPIDListFile(szInputFile) == FALSE)
			MessageBox(hWnd, "Unable to open PID list file", gszAppName, MB_ICONSTOP);
	}
#endif LITE
}

#ifndef LITE
int GetVideoCompositionPID(HWND hWnd)
{
	int nVideoCompositionPID;
	int i;

	if (v->nSelectedProgram == -1)
	{
		MessageBox(hWnd, "Please select a video program to chart", gszAppName, MB_ICONSTOP);
		return 0;
	}
	nVideoCompositionPID = -1;
	for (i = 0; i < MAX_ESLIST_ENTRIES && nVideoCompositionPID == -1; i++)
	{
		if (v->pat.pmt[v->nSelectedProgram].es[i].nESPID == 0)
			break;
		switch(v->pat.pmt[v->nSelectedProgram].es[i].nStreamType)
		{
		case 0x01:	// MPEG1
		case 0x02:	// MPEG2
			nVideoCompositionPID = v->pat.pmt[v->nSelectedProgram].es[i].nESPID;
			break;
#ifndef LITE
		case 0x1b:	// H264
			nVideoCompositionPID = v->pat.pmt[v->nSelectedProgram].es[i].nESPID | 0x0010000;
			break;
#endif LITE
		case 0x80:	// DCII
			if (v->nNetworkPID != 0x0010)
				nVideoCompositionPID = v->pat.pmt[v->nSelectedProgram].es[i].nESPID;
			break;			
		}
	}
	if (nVideoCompositionPID == -1)
	{
#ifndef LITE
		MessageBox(hWnd, "No MPEG-2/H.264 video stream found in this program", gszAppName, MB_ICONSTOP);
#else LITE
		MessageBox(hWnd, "No MPEG-2 video stream found in this program", gszAppName, MB_ICONSTOP);
#endif LITE
		return 0;
	}
	if (v->fPIDScrambled[nVideoCompositionPID])
	{
		MessageBox(hWnd, "Video stream is scrambled", gszAppName, MB_ICONSTOP);
		return 0;
	}

	return nVideoCompositionPID;
}
#endif LITE

DWORD WINAPI CheckNewVersionThread(LPVOID lpv)
{
	HWND hDlg = (HWND)lpv;
	HINTERNET hInet;
	HINTERNET hHTTP;
	HANDLE hRequest;
	DWORD dwRead;
	DWORD dwSizeLen;
	int nRetVal;
	int nActSize;
	char szTemp[128];
	char szVersionBuffer[1024];

	hInet = InternetOpen(gszAppName, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	if (hInet == NULL)
	{
		PostMessage(hDlg, WM_USER + 1, 1, 0);
		goto CheckNewVersionThread_Exit4;
	}
	PostMessage(hDlg, WM_USER + 1, 2, 0);

	hHTTP = InternetConnect(hInet, "www.coolstf.com", INTERNET_DEFAULT_HTTP_PORT, 0, 0, INTERNET_SERVICE_HTTP, 0, 0); 
	if (hHTTP == NULL)
	{
		PostMessage(hDlg, WM_USER + 1, 3, 0);
		goto CheckNewVersionThread_Exit3;
	}
	PostMessage(hDlg, WM_USER + 1, 4, 0);

#ifdef PRO
	hRequest = HttpOpenRequest(hHTTP, "GET", "/tsreader/versions/pro/", NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0); 
#else PRO
 #ifndef LITE
	hRequest = HttpOpenRequest(hHTTP, "GET", "/tsreader/versions/std/", NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);
 #else LITE
	{
		char szLiteExe[MAX_PATH];
		if (VERSION_SUB_EDIT != '\0')
			wsprintf(szLiteExe, "/tsreader/TSReaderLite%d.%d.%d%c.exe", VERSION_MAJOR, VERSION_MINOR, VERSION_EDIT, VERSION_SUB_EDIT);
		else
			wsprintf(szLiteExe, "/tsreader/TSReaderLite%d.%d.%d.exe", VERSION_MAJOR, VERSION_MINOR, VERSION_EDIT);
		hRequest = HttpOpenRequest(hHTTP, "GET", szLiteExe, NULL, NULL, NULL, INTERNET_FLAG_KEEP_CONNECTION, 0);
	}
 #endif LITE
#endif PRO
	if (hRequest == NULL)
	{
		PostMessage(hDlg, WM_USER + 1, 5, 0);
		goto CheckNewVersionThread_Exit2;
	}
	PostMessage(hDlg, WM_USER + 1, 6, 0);

	nRetVal = HttpSendRequest(hRequest, NULL, 0, NULL, 0); 
	if (nRetVal == FALSE)
	{
		PostMessage(hDlg, WM_USER + 1, 7, 0);		
		goto CheckNewVersionThread_Exit1;
	}
	PostMessage(hDlg, WM_USER + 1, 8, 0);		

	memset(szTemp, 0, sizeof(szTemp));
	dwSizeLen = sizeof(szTemp);
	nRetVal = HttpQueryInfo(hRequest, HTTP_QUERY_CONTENT_LENGTH, szTemp, &dwSizeLen, NULL); 
#ifndef LITE
	if (nRetVal != 0)
		nActSize = atol(szTemp); 
	else
		nActSize = 20;
#else LITE
	if (nRetVal != 0)
	{
		nActSize = atol(szTemp); 
		if (nActSize > sizeof(szVersionBuffer))
			nActSize = sizeof(szVersionBuffer);
	}
	else
		nActSize = sizeof(szVersionBuffer);
#endif LITE
	PostMessage(hDlg, WM_USER + 1, 10, 0);		

	memset(szVersionBuffer, 0, sizeof(szVersionBuffer));
	nRetVal = InternetReadFile(hRequest, szVersionBuffer, nActSize, &dwRead); 
	if (nRetVal != FALSE)
	{
#ifndef LITE
		// We get the proper version for Standard and Lite
		char * szLineBreak;
		char szMyVersion[128];
		
		szLineBreak = strstr(szVersionBuffer, "\r");
		if (szLineBreak != NULL)
			*szLineBreak = '\0';
		wsprintf(szMyVersion, "%d.%d.%d%c", 
				 VERSION_MAJOR, VERSION_MINOR, VERSION_EDIT, VERSION_SUB_EDIT);
		if (strcmp(szMyVersion, szVersionBuffer) == 0)
			PostMessage(hDlg, WM_USER + 1, 12, 0);
		else
			PostMessage(hDlg, WM_USER + 1, 13, 0);
		goto CheckNewVersionThread_Exit1;
#else LITE
		// For Lite we check for a HTTP 404 error - that means we have a new version
		// because this version's setup util couldn't be found. If this is the latest
		// we get the first 1KB of the setup util
		char * szError404;

		szError404 = strstr(szVersionBuffer, "<TITLE>404");
		if (szError404 != NULL)
			PostMessage(hDlg, WM_USER + 1, 13, 0);
		else
			PostMessage(hDlg, WM_USER + 1, 12, 0);
		goto CheckNewVersionThread_Exit1;
#endif LITE
	}
	PostMessage(hDlg, WM_USER + 1, 11, 0);


CheckNewVersionThread_Exit1:
	InternetCloseHandle(hRequest);
CheckNewVersionThread_Exit2:
	InternetCloseHandle(hHTTP);
CheckNewVersionThread_Exit3:
	InternetCloseHandle(hInet);
CheckNewVersionThread_Exit4:

	v->fCheckNewVersionThreadRunning = FALSE;

	return 0;
}

BOOL CALLBACK CheckNewVersionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			DWORD dwThreadID;

			v->hCheckNewVersionStatusIconBad = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_UPDATE_BAD), IMAGE_ICON, 32, 32, 0);
			v->hCheckNewVersionStatusIconGood = LoadImage(v->hInstance, MAKEINTRESOURCE(IDI_UPDATE_GOOD), IMAGE_ICON, 32, 32, 0);
					
			v->fCheckNewVersionThreadRunning = TRUE;
			v->hCheckNewVersionThread = CreateThread(NULL, 0, CheckNewVersionThread, (LPVOID)hDlg, 0, &dwThreadID);
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			}
		}
		break;
	case WM_DESTROY:
		{
			if (v->fCheckNewVersionThreadRunning == TRUE)
			{
				TerminateThread(v->hCheckNewVersionThread, -1);
				v->fCheckNewVersionThreadRunning = FALSE;
			}
			CloseHandle(v->hCheckNewVersionThread);
			DestroyIcon(v->hCheckNewVersionStatusIconBad);
			DestroyIcon(v->hCheckNewVersionStatusIconGood);
		}
		break;
	case WM_USER + 1:
		switch(wParam)
		{
		case 1:		// InternetOpen failed
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_1, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconBad);
			PostMessage(hDlg, WM_USER + 1, 11, 0);
			break;
		case 2:		// InternetOpen good
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_1, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconGood);
			break;
		case 3:		// InternetConnect failed
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_2, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconBad);
			PostMessage(hDlg, WM_USER + 1, 11, 0);
			break;
		case 4:		// Internet Open good
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_2, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconGood);
			break;
		case 5:		// HttpOpenRequest failed
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_3, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconBad);
			PostMessage(hDlg, WM_USER + 1, 11, 0);
			break;
		case 6:		// HttpOpenRequest Open good
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_3, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconGood);
			break;
		case 7:		// HttpSendRequest failed
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_4, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconBad);
			PostMessage(hDlg, WM_USER + 1, 11, 0);
			break;
		case 8:		// HttpSendRequest Open good
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_4, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconGood);
			break;
//		case 9:		// HttpQueryInfo failed
//			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_5, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconBad);
//			PostMessage(hDlg, WM_USER + 1, 11, 0);
//			break;
		case 10:	// HttpQueryInfo Open good
			SendDlgItemMessage(hDlg, IDC_UPDATE_STEP_5, STM_SETIMAGE, IMAGE_ICON, (LPARAM)v->hCheckNewVersionStatusIconGood);
			break;
		case 11:	// Something fatal happened
			MessageBox(hDlg, "Unable to retreive current version number. Check your Internet connection or try later", gszAppName, MB_ICONSTOP);
			EndDialog(hDlg, FALSE);
			break;
		case 12:	// Got the current version
			{
				char szTemp[256];

				wsprintf(szTemp, "You already have the most current version of %s", gszAppName); 
				MessageBox(hDlg, szTemp, gszAppName, MB_ICONINFORMATION);
				EndDialog(hDlg, FALSE);
			}
			break;
		case 13:	// New version
#ifndef LITE
			if (MessageBox(hDlg, "A new version available. Would you like to login in order to download it now?", gszAppName, MB_ICONINFORMATION | MB_YESNO) == IDYES)
				ShellExecute(NULL, "open", "https://tsreader.co.uk", NULL, NULL, SW_SHOW);
#else LITE
			if (MessageBox(hDlg, "A new version of TSReader Lite is available. Would you like to go to the download page now?", gszAppName, MB_ICONINFORMATION | MB_YESNO) == IDYES)
				ShellExecute(NULL, "open", "https://tsreader.co.uk", NULL, NULL, SW_SHOW);
#endif LITE
			EndDialog(hDlg, FALSE);
			break;
		}
		break;
	}
	
	return FALSE;
}

void ProcessMain_WM_COMMAND(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	default:
		if ( (wParam >= 40000) && (wParam < 50000) )
			MD__Send_External_DLL_Menu_Cmd(wParam);
		break;
#ifdef PRO
	case ID_RECORD_RECORDTABLES:
		if (v->fRecordTablesActive == FALSE)
		{
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_RECORD_TABLES), hWnd, RecordTablesDlgProc) == TRUE)
			{
				v->fRecordTablesActive = TRUE;
				CheckMenuItem(GetMenu(hWnd), ID_RECORD_RECORDTABLES, MF_CHECKED | MF_BYCOMMAND);
			}
		}
		else
		{
			int nItemIndex;

			v->fRecordTablesActive = FALSE;
			Sleep(100);
			for (nItemIndex = 0; nItemIndex < MAX_RECORD_TABLES; nItemIndex++)
			{
				if (v->record_tables[nItemIndex].hFile != 0)
				{
					CloseHandle(v->record_tables[nItemIndex].hFile);
					v->record_tables[nItemIndex].hFile = 0;
				}
			}
			CheckMenuItem(GetMenu(hWnd), ID_RECORD_RECORDTABLES, MF_UNCHECKED | MF_BYCOMMAND);
		}
		break;
#endif PRO
	case ID_SETTINGS_PLUGINSETTINGS:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_PLUGIN_SETTINGS), hWnd, PluginSettingsDlgProc);
		break;
	case ID_FILE_LOADPIDLIST:
		LoadManualPIDList(hWnd);
		break;
	case ID_VIEW_TABLELIST:
#ifndef LITE
#ifndef PRO
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_TABLE_VIEWER), hWnd, TableViewerDlgProc);
#else PRO
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_TABLE_VIEWER_PRO), hWnd, TableViewerDlgProc);
#endif PRO
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_EPGGRID:
#ifndef LITE
		DialogBoxParam(v->hInstance, MAKEINTRESOURCE(IDD_EPG_GRID_SETTINGS), hWnd, EPGGridSettingsDlgProc, FALSE);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_CHART_SETTINGS_REFRESHRATE:
#ifndef LITE
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_GRAPH_REFRESH), hWnd, GraphRefreshRateDlgProc);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_CHART_SETTINGS_PIDCHARTCOLORS:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_PID_CHART_COLORS), hWnd, PIDChartColorsDlgProc);
		break;
	case ID_VIEW_CHARTSETTINGS_STYLE_NOSTYLE:							// 0
	case ID_VIEW_CHARTSETTINGS_STYLE_LIGHTCOLORSWITHINSETBORDER:		// 1
	case ID_VIEW_CHARTSETTINGS_STYLE_LIGHTCOLORSWITHSHADOWBORDER:		// 2
	case ID_VIEW_CHARTSETTINGS_STYLE_LIGHTCOLORSWITHLINEBORDER:			// 3
	case ID_VIEW_CHARTSETTINGS_STYLE_LIGHTCOLORSWITHNOBORDER:			// 4
	case ID_VIEW_CHARTSETTINGS_STYLE_MEDIUMCOLORSWITHINSETBORDER:		// 5
	case ID_VIEW_CHARTSETTINGS_STYLE_MEDIUMCOLORSWITHSHADOWBORDER:		// 6
	case ID_VIEW_CHARTSETTINGS_STYLE_MEDIUMCOLORSWITHLINEBORDER:		// 7
	case ID_VIEW_CHARTSETTINGS_STYLE_MEDIUMCOLORSWITHNOBORDER:			// 8
	case ID_VIEW_CHARTSETTINGS_STYLE_DARKCOLORSWITHINSETBORDER:			// 9
	case ID_VIEW_CHARTSETTINGS_STYLE_DARKCOLORSWITHSHADOWBORDER:		// 10
	case ID_VIEW_CHARTSETTINGS_STYLE_DARKCOLORSWITHLINEBORDER:			// 11
	case ID_VIEW_CHARTSETTINGS_STYLE_DARKCOLORSWITHNOBORDER:			// 12
		{
			int nChartIndex = 0;
			void Charting__UpdateQuickStyle(int nChartIndex);

			v->nChartStyle = LOWORD(wParam) - ID_VIEW_CHARTSETTINGS_STYLE_NOSTYLE;
			CheckMenuRadioItem(GetMenu(hWnd),
							   ID_VIEW_CHARTSETTINGS_STYLE_NOSTYLE,
							   ID_VIEW_CHARTSETTINGS_STYLE_DARKCOLORSWITHNOBORDER,
							   v->nChartStyle + ID_VIEW_CHARTSETTINGS_STYLE_NOSTYLE,
							   MF_BYCOMMAND);
			for (nChartIndex = 0; nChartIndex < MAX_CHARTS; nChartIndex++)
			{			
				if (v->m_hPE[nChartIndex] != NULL)
				{
					Charting__UpdateQuickStyle(nChartIndex);
					InvalidateRect(v->m_hPE[nChartIndex], NULL, FALSE);
				}
			}
		}
		break;			
	case ID_VIEW_PMTTREE_EXPANDALLPMTS:
	case ID_VIEW_PMTTREE_CONTRACTALLPMTS:
		{
			int nPMTIndex;
			UINT uOption = TVE_EXPAND;
			HWND hWndTV = GetDlgItem(v->hDlgSIParser, IDC_SI_TREE);

			if (LOWORD(wParam) == ID_VIEW_PMTTREE_CONTRACTALLPMTS)
				uOption = TVE_COLLAPSE;

			SendMessage(hWndTV, WM_SETREDRAW, FALSE, 0);
			for (nPMTIndex = 0; nPMTIndex < MAX_PAT_ENTRIES; nPMTIndex++)
			{
				if (v->pat.pmt[nPMTIndex].nPMTPID == 0)
					break;
				if (v->pat.pmt[nPMTIndex].hPMTTreeItem != NULL)
					TreeView_Expand(hWndTV, v->pat.pmt[nPMTIndex].hPMTTreeItem, uOption);
			}
			SendMessage(hWndTV, WM_SETREDRAW, TRUE, 0);
		}
		break;
	case ID_FILE_CICAMMENU:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_CAM_MENU), hWnd, CAMMenuDlgProc);
		break;
	case ID_FILE_IMPORTSATELLITELISTS_IMPORTSDX:
		SourceHelper_ImportSatelliteList(hWnd, 0);
		break;
	case ID_FILE_IMPORTSATELLITELISTS_IMPORTCSV:
		SourceHelper_ImportSatelliteList(hWnd, 1);
		break;
	case ID_VIEW_PIDPIECHART:
		CreateChartWindow(hWnd, PIDPieChartWindowProc, TRUE, LOWORD(wParam));
		break;
	case ID_VIEW_PIDUSAGE2DPIECHART:
		CreateChartWindow(hWnd, PIDPieChartWindowProc, FALSE, LOWORD(wParam));
		break;
	case ID_VIEW_VIDEOBITRATECHART:
	case ID_VIEW_VIDEOBITRATEAREACHART:
		{
			int nVideoStreamCount = GetVideoStreamCount();
			if (nVideoStreamCount == 0)
			{
				MessageBox(hWnd, "This mux doesn't appear to contain any video streams", gszAppName, MB_ICONSTOP);
				break;
			}
			CreateChartWindow(hWnd, VideoBitrateChartWindowProc, LOWORD(wParam) == ID_VIEW_VIDEOBITRATEAREACHART, LOWORD(wParam));
		}
		break;
	case ID_VIEW_MUXUSAGELINECHART:
	case ID_VIEW_MUXUSAGEAREACHART:
		{
			int nProgramCount = GetProgramCount();
			if (nProgramCount == 0)
			{
				MessageBox(hWnd, "This mux doesn't appear to contain any programs", gszAppName, MB_ICONSTOP);
				break;
			}

			CreateChartWindow(hWnd, MuxrateChartWindowProc, LOWORD(wParam) == ID_VIEW_MUXUSAGEAREACHART, LOWORD(wParam));
			break;
		}
		break;
	case ID_VIEW_ACTIVEPIDSBYRATE:
		CreateChartWindow(hWnd, ActivePIDsChartWindowProc, FALSE, LOWORD(wParam));
		break;
	case ID_VIEW_ACTIVEPIDSBYPID:
		CreateChartWindow(hWnd, ActivePIDsChartWindowProc, TRUE, LOWORD(wParam));
		break;
	case ID_VIEW_CHART_VIDEOCOMPOSITIONCHART:
#ifndef LITE
		{
			int nMonitorPID = GetVideoCompositionPID(hWnd);
			if (nMonitorPID)
			{
				int nChartIndex;

				nChartIndex = CreateChartWindow(hWnd, VideoCompositionChartWindowProc, nMonitorPID, LOWORD(wParam));
				v->nVideoCompositionPID[nChartIndex] = nMonitorPID;
			}
		}
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_CHART_SIGNALCHART:
#ifndef LITE
		CloseExistingChart(hWnd, ID_VIEW_CHART_SIGNALCHART);
		{
			char * szCompareSignal;
			char szSignal[64];

			GetSourceInfoLine(2, szSignal);
			szCompareSignal = strstr(szSignal, " ") + 1;
			if (lstrcmp(szCompareSignal, "n/a") == 0)
			{
				MessageBox(hWnd, "The current source doesn't report signal strength, so this function cannot be used.", gszAppName, MB_ICONINFORMATION);
				break;
			}
			CreateChartWindow(hWnd, SignalChartWindowProc, FALSE, LOWORD(wParam));
		}
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_CHART_PROGRAMUSAGESTACKEDBARCHART:
#ifndef LITE
		CreateChartWindow(hWnd, ProgramUsageChartWindowProc, FALSE, LOWORD(wParam));
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
#ifdef PRO
	case ID_VIEW_CHART_PIDUSAGEVBRSTACKEDAREA:
		if (!v->fRealtimeCharting)
			MessageBox(hWnd, "This chart requires TSReader operate in real-time charting mode. Please select this mode first", gszAppName, MB_ICONSTOP);
		else
			CreateChartWindow(hWnd, PIDUsageVBRStackedAreaWindowProc, TRUE, LOWORD(wParam));
		break;
#endif PRO

	case ID_SETTINGS_IPDVBSETTINGS:
#ifndef LITE
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_IP_SETTINGS), hWnd, IPSettingsDlgProc);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_HELP_SHOWPIDLIST:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_PID_LIST), hWnd, PIDListDlgProc);
		break;
	case ID_SETTINGS_CONTROLSERVERSETTINGS:
#ifndef LITE
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_CONTROL_SERVER), hWnd, ControlServerDlgProc);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_BUFFERSIZES:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_BUFFER_SIZES), hWnd, BufferSizesDlgProc);
		break;
	case ID_EXPORT_SAVETHUMBNAILS:
		SaveThumbnails(hWnd, FALSE);
		break;
	case ID_EXPORT_SAVEALLTHUMBNAILS:
		SaveThumbnails(hWnd, TRUE);
		break;
	case IDC_SUPPORT:
#ifdef INEOQUEST
		ShellExecute(NULL, "open", "http://www.ineoquest.com", NULL, NULL, SW_SHOW);
#else INEOQUEST
 #ifdef ZANALYZER
		ShellExecute(NULL, "open", "http://www.zanalyzer.com", NULL, NULL, SW_SHOW);
 #else ZANALYZER
  #ifdef DTVSENTINEL
		ShellExecute(NULL, "open", "http://www.mediatechnologysolutions.net", NULL, NULL, SW_SHOW);
  #else DTVSENTINEL
   #ifndef LITE
		{
			char szSupportHTMLFile[MAX_PATH];

			SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szSupportHTMLFile, sizeof(szSupportHTMLFile));
			lstrcat(szSupportHTMLFile, "\\documentation\\support.html");
			ShellExecute(NULL, "open", szSupportHTMLFile, NULL, NULL, SW_SHOW);
		}
    #else LITE
		ShellExecute(NULL, "open", "https://tsreader.co.uk", NULL, NULL, SW_SHOW);
   #endif LITE
  #endif DTVSENTINEL
 #endif ZANALYZER
#endif INEOQUEST
		break;
	case ID_FILE_SELECTSOURCE:
		{
			char szCurrentSource[MAX_PATH];

			lstrcpy(szCurrentSource, v->szSourceName);
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_SOURCE), hWnd, SourceDlgProc) == TRUE)
			{
				if (lstrcmp(szCurrentSource, v->szSourceName) != 0)
				{
#ifndef LITE
					RestartTSReader_Stop(hWnd);

					// Switch source if we need to
					FreeLibrary(v->hSource);
					SourceHelper_ResetFirstTimeFlag();
					LoadSource(hWnd);						
					EnableDisableSourceMenuItems(hWnd);
					RestartTSReader_Start(hWnd);
#else LITE
					MessageBox(hWnd, "You will now need to close and restart TSReader for the change to take effect", gszAppName, MB_ICONINFORMATION);
#endif LITE
				}
			}
		}
		break;
	case ID_FILE_RESTART_SOURCE:
		RestartTSReader(hWnd);
		break;
	case ID_FILE_EXIT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case IDC_SI_PARSER_EXPORT_EIT:
#ifndef LITE
		StartXMLExport(hWnd, FALSE);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_EXPORT_XMLTVEXPORT:
#ifndef LITE
		StartXMLExport(hWnd, TRUE);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case IDC_SI_PARSER_EXPORT:
		SIParserExport(hWnd);
		break;
	case IDC_SI_PARSER_TO_STRADIS:
		v->nStreamTo = STREAM_TO_STRADIS;
		StreamDecoder(hWnd);
		break;
	case ID_PLAYBACK_STRADISMPEG2DECODER_SETTINGS:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_STRADIS_SETTINGS), hWnd, StradisSettingsDlgProc);
		break;
	case ID_PLAYBACK_DIRECTSHOW:
		v->nStreamTo = STREAM_TO_DIRECTSHOW;
		StreamDecoder(hWnd);
		break;
	case ID_PLAYBACK_XNSSERVER:
		v->nStreamTo = STREAM_TO_XNS;
		StreamDecoder(hWnd);
		break;
	case ID_SETTINGS_EITLANGUAGE:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_EIT_LANGUAGE_SETTINGS), hWnd, EITLanguageSettingsDlgProc);
		break;
#ifdef PRO
	case ID_EXPORT_GPSSIGNALEXPORT:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_GPS_SIGNAL_EXPORT), hWnd, GPSSignalExportDlgProc);
		break;
	case ID_VIEW_CHART_SETTINGS_SAVECHARTDATA:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_CHART_SAVE_DATA), hWnd, ChartSaveDataDlgProc);
		break;
	case ID_VIEW_MDIINDEX:
		if (v->hDlgMDIIndex == NULL)
		{
			v->hDlgMDIIndex = CreateDialog(v->hInstance, MAKEINTRESOURCE(IDD_MDI_INDEX), hWnd, MDIIndexDlgProc);
			ShowWindow(v->hDlgMDIIndex, SW_SHOW);
		}
		else
			SetForegroundWindow(v->hDlgMDIIndex);		
		break;
	case ID_VIEW_VIDEOMOSAIC:
		if (v->hWndVideoMosaic == NULL)
			ShowVideoMosaic(hWnd);
		else
			SetForegroundWindow(v->hWndVideoMosaic);
		break;
	case ID_VIEW_STREAMMONITOR:
		if (v->nPMTPID != v->nNullPID && v->pat.hPATTreeItem == NULL)
		{
			MessageBox(hWnd, "Please wait for PMT parsing to complete", gszAppName, MB_ICONWARNING);
			break;
		}
		if (IsWindow(v->hDlgStreamMonitor))
			SetForegroundWindow(v->hDlgStreamMonitor);
		else
		{
			v->hDlgStreamMonitor = CreateDialog(v->hInstance, MAKEINTRESOURCE(IDD_STREAM_MONITOR), hWnd, StreamMonitorDlgProc);
			ShowWindow(v->hDlgStreamMonitor, SW_SHOW);
		}
		break;
	case ID_EXPORT_SAVEEPGDATA:
		if (!v->fEPGSaveEnabled)
		{
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_SAVE_EPG), hWnd, SaveEPGDataDlgProc) == TRUE)
				CheckMenuItem(GetMenu(hWnd), ID_EXPORT_SAVEEPGDATA, MF_CHECKED | MF_BYCOMMAND);
		}
		else
		{
			v->fEPGSaveEnabled = FALSE;
			CheckMenuItem(GetMenu(hWnd), ID_EXPORT_SAVEEPGDATA, MF_UNCHECKED | MF_BYCOMMAND);
		}
		break;
	case ID_SETTINGS_EITSERVER:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_EIT_SERVER), hWnd, EITServerSettingsDlgProc);
		break;
	case ID_SETTINGS_EMAILSETTINGS:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_EMAIL_SETUP), hWnd, EmailSetupDlgProc);
		break;
	case ID_VIEW_CLOSEDCAPTIONS:
		ClosedCaptionDecoderToggle(hWnd);
		break;
	case ID_VIEW_ARCHIVEDFILES:
		ViewArchivedFiles(hWnd);
		break;
	case ID_VIEW_DESCRIPTORUSAGE:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_DESCRIPTOR_USAGE), hWnd, DescriptorUsageDlgProc);
		break;
	case ID_RECORD_RECORDALLPROGRAMS:
		if (!v->fArchiveRunning)
		{
			if (v->nPMTPID != v->nNullPID && v->pat.hPATTreeItem == NULL)
			{
				MessageBox(hWnd, "Please wait for PMT parsing to complete", gszAppName, MB_ICONWARNING);
				break;
			}
			if (StartArchivePrograms(hWnd) == TRUE)
				CheckMenuItem(GetMenu(hWnd), ID_RECORD_RECORDALLPROGRAMS, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			StopArchivePrograms(hWnd);
			CheckMenuItem(GetMenu(hWnd), ID_RECORD_RECORDALLPROGRAMS, MF_BYCOMMAND | MF_UNCHECKED);
		}
		break;
	case ID_FORWARD_FORWARDTOUDP:
		if (!v->fForwarderEnabled)
		{
			if (v->nPMTPID != v->nNullPID && v->pat.hPATTreeItem == NULL)
			{
				MessageBox(hWnd, "Please wait for PMT parsing to complete", gszAppName, MB_ICONWARNING);
				break;
			}
			if (StartUDPForwarder(hWnd) == TRUE)
				CheckMenuItem(GetMenu(hWnd), ID_FORWARD_FORWARDTOUDP, MF_BYCOMMAND | MF_CHECKED);
		}
		else
		{
			StopUDPForwarder(hWnd);
			CheckMenuItem(GetMenu(hWnd), ID_FORWARD_FORWARDTOUDP, MF_BYCOMMAND | MF_UNCHECKED);
		}
		break;
	case ID_FORWARD_SETTINGS:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_FORWARD_SETTINGS), hWnd, FwdSettingsDlgProc);
		break;
	case ID_FORWARD_DLL_BASE + 0:
	case ID_FORWARD_DLL_BASE + 1:
	case ID_FORWARD_DLL_BASE + 2:
	case ID_FORWARD_DLL_BASE + 3:
	case ID_FORWARD_DLL_BASE + 4:
	case ID_FORWARD_DLL_BASE + 5:
	case ID_FORWARD_DLL_BASE + 6:
	case ID_FORWARD_DLL_BASE + 7:
	case ID_FORWARD_DLL_BASE + 8:
	case ID_FORWARD_DLL_BASE + 9:
	case ID_FORWARD_DLL_BASE + 10:
	case ID_FORWARD_DLL_BASE + 11:
	case ID_FORWARD_DLL_BASE + 12:
	case ID_FORWARD_DLL_BASE + 13:
	case ID_FORWARD_DLL_BASE + 14:
	case ID_FORWARD_DLL_BASE + 15:
		{
			int nIndex = LOWORD(wParam) - ID_FORWARD_DLL_BASE;
			ForwarderModuleStartStop(hWnd, nIndex);
		}
		break;
#endif PRO
	case ID_PLAYBACK_ROKUHD1000:
		{
			BOOL fAbort = FALSE;

			v->nStreamTo = STREAM_TO_ROKU;
			while ( (lstrlen(v->szRokuIP) == 0) || (lstrlen(v->szRokuUsername) == 0) || (lstrlen(v->szRokuMpegPSPlayLocation) == 0) )
			{
				if (MessageBox(hWnd, "Either the Roku HD1000 IP, username, or MpegPSPlay location is not set.\n\nPlease set on the following dialog.", gszAppName, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
				{
					fAbort = TRUE;
					break;
				}
				if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_ROKU1000_SETTINGS), hWnd, RokuHD1000Settings) == FALSE)
				{
					fAbort = TRUE;
					break;
				}
			} (TRUE);
			if (!fAbort)
				StreamDecoder(hWnd);
		}
		break;
	case ID_SETTINGS_SPLITRECORDFILENAMES:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_FILE_NAME_FORMAT), hWnd, FileNameFormatDlgProc);
		break;				
	case ID_RECORD_RECORDPROGRAMTODVHS:
		v->nStreamTo = STREAM_TO_DVHS;
		StreamDecoder(hWnd);
		break;
	case ID_PLAYBACK_VLC_1:
	case ID_PLAYBACK_VLC_2:
	case ID_PLAYBACK_VLC_3:
	case ID_PLAYBACK_VLC_4:
	case ID_PLAYBACK_VLC_5:
	case ID_PLAYBACK_VLC_6:
	case ID_PLAYBACK_VLC_7:
	case ID_PLAYBACK_VLC_8:
	case ID_PLAYBACK_VLC_9:
	case ID_PLAYBACK_VLC_10:
	case ID_PLAYBACK_VLC_11:
	case ID_PLAYBACK_VLC_12:
	case ID_PLAYBACK_VLC_13:
	case ID_PLAYBACK_VLC_14:
	case ID_PLAYBACK_VLC_15:
	case ID_PLAYBACK_VLC_16:
		v->nVLCPlaybackConfig = LOWORD(wParam) - ID_PLAYBACK_VLC_1;
		while (!lstrlen(v->szVLCExeLocation))
		{
			if (MessageBox(hWnd, "The VLC executable location is not set. Please set on the following dialog", gszAppName, MB_ICONWARNING | MB_OKCANCEL) == IDCANCEL)
				return;
			DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_VLC_SETTINGS), hWnd, VLCSettingsDlgProc);
		}
		if (!lstrlen(v->szVLCConfigCommand[v->nVLCPlaybackConfig]))
		{
			MessageBox(hWnd, "VLC Command not set - defaulting to playback", gszAppName, MB_ICONINFORMATION);
			lstrcpy(v->szVLCConfigCommand[v->nVLCPlaybackConfig], "<IP>");
		}
		if (v->fVLCNoWarn == FALSE && v->fVLCControl == FALSE && v->fRecording == FALSE)
		{
			DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_VLC_WARNING), hWnd, VLCWarningDlgProc);
		}
		v->nStreamTo = STREAM_TO_VLC;
		StreamDecoder(hWnd);
		break;
	case IDC_SI_PARSER_RECORD:
		v->nStreamTo = 0;
		RecordStream(hWnd, FALSE, -1);
		break;
	case IDC_SI_PARSER_RECORD_ALL:
#ifdef LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
		if (   v->nMaxSourcePIDs != 8192
			&& v->fIgnoreRecordAllPIDLimitationWarning == FALSE
			&& v->nAutoRecord == AUTO_RECORD_NONE)
		{
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_PID_WARNING), hWnd, PIDWarningDlgProc) == FALSE)
				break;
		}
		RecordStream(hWnd, TRUE, -1);
#endif LITE
		break;
	case IDC_SI_PARSER_RECORD_PID:
		{				
			if (v->fRecording == TRUE)
			{
				int nPID;

				v->fRecording = FALSE;
				for (nPID = 0; nPID < 8192; nPID++)
				{
					if (v->hRecordPIDFile[nPID] != INVALID_HANDLE_VALUE)
					{
						CloseHandle(v->hRecordPIDFile[nPID]);
						v->hRecordPIDFile[nPID] = INVALID_HANDLE_VALUE;
					}
				}
				CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_PID, MF_BYCOMMAND | MF_UNCHECKED);
			}
			else
			{
				v->fRecordPIDMode = TRUE;
				if (DialogBoxParam(v->hInstance, MAKEINTRESOURCE(IDD_RECORD_PID_SELECT), hWnd, IPDVBPIDSelectDlgProc, TRUE) == TRUE)
				{
					//
					int nPID;

					if (v->fRecordPIDsOneFile == FALSE)
					{
						// Record the PIDs to individual files
						for (nPID = 0; nPID < 8192; nPID++)
						{
							if (v->nIPMonitorPID[nPID] != -1)
							{
								char szFileName[MAX_PATH] = {0};
								char szTemp[128];

								if (lstrlen(v->szRecordPIDFolder))
								{
									lstrcpy(szFileName, v->szRecordPIDFolder);
									if (szFileName[lstrlen(szFileName) - 1] != '\\')
										lstrcat(szFileName, "\\");
								}
								wsprintf(szTemp, "PID_%s.bin", FormatTooltipPID(nPID));
								lstrcat(szFileName, szTemp);
								v->hRecordPIDFile[nPID] = INVALID_HANDLE_VALUE;
								if (v->fRecordPIDsAppend == TRUE)
								{
									v->hRecordPIDFile[nPID] = CreateFile(szFileName,
																		 GENERIC_WRITE | GENERIC_READ,
																		 FILE_SHARE_READ,
																		 (LPSECURITY_ATTRIBUTES)NULL,
																		 OPEN_EXISTING, 
																		 FILE_ATTRIBUTE_NORMAL, 
																		 (HANDLE)NULL);
									if (v->hRecordPIDFile[nPID] != INVALID_HANDLE_VALUE)
										SetFilePointer(v->hRecordPIDFile[nPID], 0, NULL, FILE_END);
								}
								if (v->hRecordPIDFile[nPID] == INVALID_HANDLE_VALUE)									
									v->hRecordPIDFile[nPID] = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
								if (v->hRecordPIDFile[nPID] == INVALID_HANDLE_VALUE)
								{
									char szTemp[MAX_PATH];

									wsprintf(szTemp, "Unable to open PID record file %s\n\nPID %s will not be recorded", szFileName, FormatTooltipPID(nPID));
									MessageBox(hWnd, szTemp, gszAppName, MB_ICONSTOP);
								}
							}
							else
								v->hRecordPIDFile[nPID] = INVALID_HANDLE_VALUE;
						}
					}
					else
					{
						// Record the PIDs to one file
						v->hRecordPIDFile[0] = CreateFile(v->szRecordPIDFolder, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
						if (v->hRecordPIDFile[0] == INVALID_HANDLE_VALUE)
						{
							char szTemp[MAX_PATH];

							wsprintf(szTemp, "Unable to open PID(s) record file %s", v->szRecordPIDFolder);
							MessageBox(hWnd, szTemp, gszAppName, MB_ICONSTOP);
							break;
						}
					}
					v->dTotalRecorded = 0;
					v->dwRecordTickCounter = GetTickCount();
					v->fRecording = TRUE;
					CheckMenuItem(GetMenu(hWnd), IDC_SI_PARSER_RECORD_PID, MF_BYCOMMAND | MF_CHECKED);
				}
			}
		}
		break;
	case IDC_SI_PARSER_IP_DVB_MODE:
		ToggleIPDVBMode(hWnd);
		break;
	case IDC_SI_PARSER_EXPORT_NIT:
		SIParserExportNIT(hWnd);
		break;
	case IDC_SI_PARSER_ABOUT:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_ABOUT), hWnd, AboutDlgProc);
		break;
	case ID_HELP_CHECKFORNEWVERSION:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_CHECK_NEW_VERSION), hWnd, CheckNewVersionDlgProc);
		break;
#ifdef LITE
	case IDC_LITE_WARNING:
		MessageBox(hWnd, "This version of TSReader limits recording and streaming to one minute", gszAppName, MB_ICONINFORMATION);
		break;
#endif LITE
	case ID_SETTINGS_MAINPROCESSPRIORITY_REALTIME:
		v->nProcessPriority = REALTIME_PRIORITY_CLASS;
		SetProcessPriority();
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_MAINPROCESSPRIORITY_HIGH:
		v->nProcessPriority = HIGH_PRIORITY_CLASS;
		SetProcessPriority();
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_MAINPROCESSPRIORITY_ABOVENORMAL:
		v->nProcessPriority = ABOVE_NORMAL_PRIORITY_CLASS;
		SetProcessPriority();
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_MAINPROCESSPRIORITY_NORMAL:
		v->nProcessPriority = NORMAL_PRIORITY_CLASS;
		SetProcessPriority();
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_MAINPROCESSPRIORITY_BELOWNORMAL:
		v->nProcessPriority = BELOW_NORMAL_PRIORITY_CLASS;
		SetProcessPriority();
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_MAINPROCESSPRIORITY_LOW:
		v->nProcessPriority = IDLE_PRIORITY_CLASS;
		SetProcessPriority();
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_DATAINPUTTHEADPRIORITY_NORMAL:
		v->ss.nInputThreadPriority = 0;
		SourceHelper_SetWorkerThreadPriorities(FALSE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_DATAINPUTTHEADPRIORITY_HIGH:
		v->ss.nInputThreadPriority = 1;
		SourceHelper_SetWorkerThreadPriorities(FALSE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_DATAINPUTTHEADPRIORITY_LOW:
		v->ss.nInputThreadPriority = 2;
		SourceHelper_SetWorkerThreadPriorities(FALSE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_DATAINPUTTHEADPRIORITY_CRITICAL:
		v->ss.nInputThreadPriority = 3;
		SourceHelper_SetWorkerThreadPriorities(FALSE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_NORMAL:
		v->nStreamProcessingThreadPriority = 0;
		SourceHelper_SetWorkerThreadPriorities(TRUE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_HIGH:
		v->nStreamProcessingThreadPriority = 1;
		SourceHelper_SetWorkerThreadPriorities(TRUE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_STREAMPROCESSINGTHREADPRIORITY_LOW:
		v->nStreamProcessingThreadPriority = 2;
		SourceHelper_SetWorkerThreadPriorities(TRUE);
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_THUMBNAILTHREADPRIORITY_NORMAL:
		v->nThumbnailProcessingThreadPriority = 0;
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_THUMBNAILTHREADPRIORITY_HIGH:
		v->nThumbnailProcessingThreadPriority = 1;
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_THUMBNAILTHREADPRIORITY_LOW:
		v->nThumbnailProcessingThreadPriority = 2;
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_SETTINGS_THUMBNAILTHREADPRIORITY_DISABLED:
		v->nThumbnailProcessingThreadPriority = 3;
		UpdateThreadDialogPriorities(hWnd);
		break;
	case ID_HELP_PURCHASETSREADER:
		// Purchase link removed - this is a memorial build, no commercial version available
		break;
	case ID_HELP_RESETALL:
#ifdef LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
		SendMessage(hWnd, WM_COMMAND, ID_HELP_RESETCONTINUITYCOUNTERS, 0);
		SendMessage(hWnd, WM_COMMAND, ID_HELP_RESETTEICOUNTERS, 0);
		SendMessage(hWnd, WM_COMMAND, ID_HELP_RESETCRCCOUNTERS, 0);
		SendMessage(hWnd, WM_COMMAND, ID_HELP_RESETSECTIONCOUNTERS, 0);
		SendMessage(hWnd, WM_COMMAND, ID_HELP_RESETPIDCHART, 0);
		if (GetSyncLossCount != NULL)
			GetSyncLossCount(TRUE);
		if (GetRetuneCount != NULL)
			GetRetuneCount(TRUE);
		break;
#endif LITE
	case ID_HELP_RESETCONTINUITYCOUNTERS:
		{
			int i;

			EnterCriticalSection(&v->ss.csPIDCounter);
			for (i = 0; i < 8192; i++)
			{
				v->nPIDContinuity[i] = -1;
				v->nPIDHasContinuityErrors[i] = 0;
			}
			v->nContinuityErrors = 0;
			LeaveCriticalSection(&v->ss.csPIDCounter);
		}
		break;
	case ID_HELP_RESETTEICOUNTERS:
		{
			int i;

			for (i = 0; i < 8192; i++)
				v->nPIDTEICount[i] = 0;
			v->nTEIErrors = 0;
		}
		break;
	case ID_HELP_RESETCRCCOUNTERS:
		ResetParserCRCs();
		break;
	case ID_HELP_RESETSECTIONCOUNTERS:
		ResetParserPackets();
		break;
	case ID_HELP_RESETPIDCHART:
		ResetPIDChart(hWnd);
		break;
	case ID_SETTINGS_AUTORESTART:
#ifndef LITE
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_AUTO_RESTART), hWnd, AutoRestartDlgProc);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_ALWAYSONTOP:
		ToggleMenuOption(hWnd, ID_VIEW_ALWAYSONTOP, &v->fAlwaysOnTop);
		ToggleAlwaysOnTop();
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_REFRESHNOW:
#ifndef LITE
		SendMessage(v->hDlgSIParser, WM_USER + 7, 0, 0);
#endif LITE
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_ENABLEAUDIOTHUMBNAILS:
#ifndef LITE
#ifndef PRO
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_ENABLEAUDIOTHUMBNAILS, &v->fAudioThumbnails);
		UpdateVideoPix(hWnd);
#else PRO
		if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_AUDIO_THUMBNAIL_SETTINGS), hWnd, AudioThumbnailSettingsDlgProc) == TRUE)
			UpdateVideoPix(hWnd);
#endif PRO
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_CHART_SETTINGS_STYLE_GRADIENTBITMAPBACKGROUND:
		ToggleMenuOption(hWnd, ID_SETTINGS_SHOWNONVIDEOPCRICONS, &v->fChartGradientBitmap);
		break;
	case ID_SETTINGS_SHOWNONVIDEOPCRICONS:
		ToggleMenuOption(hWnd, ID_SETTINGS_SHOWNONVIDEOPCRICONS, &v->fShowNonVideoPCR);
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_DISPLAYFULLTHUMBNAILSONLY:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_DISPLAYFULLTHUMBNAILSONLY, &v->fFullThumbnails);
		UpdateVideoPix(hWnd);
		break;
	case ID_SETTINGS_SDTONLYFORCURRENTMUX:
		ToggleMenuOption(hWnd, ID_SETTINGS_SDTONLYFORCURRENTMUX, &v->fSDTOnlyForCurrentMux);
		break;
	case ID_SETTINGS_PLAINCADESCRIPTORDECODING:
		ToggleMenuOption(hWnd, ID_SETTINGS_PLAINCADESCRIPTORDECODING, &v->fPlainCADescriptors);
		break;
	case ID_SETTINGS_WARNINGBEFOREOVERWRITNGRECORDINGS:
		ToggleMenuOption(hWnd, ID_SETTINGS_WARNINGBEFOREOVERWRITNGRECORDINGS, &v->fWarnBeforeOverwritingRecordings);
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_HIDETHUMBNAILICONS:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_HIDETHUMBNAILICONS, &v->fHideThumbnailIcons);
		UpdateVideoPix(hWnd);
		break;
	case ID_SETTINGS_REALTIMECHARTING:
#ifndef LITE
		if (v->nMaxSourcePIDs < 8192)
		{
			MessageBox(hWnd, "This function can only be used full-transport stream interfaces.", gszAppName, MB_ICONSTOP);
			break;
		}
		ToggleMenuOption(hWnd, ID_SETTINGS_REALTIMECHARTING, &v->fRealtimeCharting);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL:
	case ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE:
	case ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_NORMAL:
#ifndef LITE
		if (LOWORD(wParam) == ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_SMALL)
			v->nThumbnailSize = 1;
		else if (LOWORD(wParam) == ID_SETTINGS_THUMBNAILTHREAD_THUMBNAILSIZE_LARGE)
			v->nThumbnailSize = 2;
		else
			v->nThumbnailSize = 0;
		{
			HDC hDC = GetDC(v->hDlgSIParser);
			SetupChannelFont(hDC);
			ReleaseDC(v->hDlgSIParser, hDC);
		}
		CheckThumbnailSizeMenu(GetMenu(hWnd));
		PostMessage(v->hDlgSIParser, WM_USER + 3, 0, 1);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_RIGHTTOLEFT:
	case ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_TOPDOWN:
#ifndef LITE
		if (LOWORD(wParam) == ID_SETTINGS_THUMBNAILTHREAD_DISPLAYORDER_RIGHTTOLEFT)
			v->fThumbnailsRightToLeft = TRUE;
		else
			v->fThumbnailsRightToLeft = FALSE;
		CheckThumbnailDisplayOrderMenu(GetMenu(hWnd));
		PostMessage(hWnd, WM_USER + 3, 0, 1);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_TIMESTAMPPACKETS:
#ifndef LITE
		ToggleMenuOption(hWnd, ID_SETTINGS_TIMESTAMPPACKETS, &v->ss.fTimestampPackets);
		MessageBox(hWnd, gszRestartNeeded, gszAppName, MB_ICONINFORMATION);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_DECIMALPIDS:
		ToggleMenuOption(hWnd, ID_SETTINGS_DECIMALPIDS, &v->fDecimalPIDs);
		if (v->fDecimalPIDs)
			lstrcpy(v->szOutputPIDFlags, "%d");
		else
			lstrcpy(v->szOutputPIDFlags, "0x%04x");
		UpdatePIDChart(v->hDlgSIParser);
		MessageBox(hWnd, gszRestartNeeded, gszAppName, MB_ICONINFORMATION);
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_WAITFORCABEFOREPICTURE:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_WAITFORCABEFOREPICTURE, &v->fWaitForCAThumbnail);
		break;
	case ID_SETTINGS_HIDEWHENMINIMIZED:
		ToggleMenuOption(hWnd, ID_SETTINGS_HIDEWHENMINIMIZED, &v->fHideWhenMinimized);
		break;
	case ID_SETTINGS_COUNTCONTINUITYERRORS:
#ifndef LITE
		ToggleMenuOption(hWnd, ID_SETTINGS_COUNTCONTINUITYERRORS, &v->fCountContinuityErrors);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_KEEPPASTEITDATA:
		ToggleMenuOption(hWnd, ID_SETTINGS_KEEPPASTEITDATA, &v->fKeepPastEITData);
		break;
	case ID_SETTINGS_CONTROLDVHSDECK:
		ToggleMenuOption(hWnd, ID_SETTINGS_CONTROLDVHSDECK, &v->fControlDVHSDeck);
		break;
	case ID_SETTINGS_DVHSCONTROL_AUTOPOWERONOFF:
		ToggleMenuOption(hWnd, ID_SETTINGS_DVHSCONTROL_AUTOPOWERONOFF, &v->fPowerCycleDVHSDeck);
		break;
	case ID_SETTINGS_IGNOREEIT:
		ToggleMenuOption(hWnd, ID_SETTINGS_IGNOREEIT, &v->fIgnoreEIT);
		break;
	case ID_SETTINGS_IGNOREPMTSABOVECH65500:
		ToggleMenuOption(hWnd, ID_SETTINGS_IGNOREPMTSABOVECH65500, &v->fIgnorePMT65500);
		break;
	case ID_SETTINGS_IGNOREDCIIPMTCH80:
		ToggleMenuOption(hWnd, ID_SETTINGS_IGNOREDCIIPMTCH80, &v->fIgnorePMT800x0ff6);
		break;
	case ID_SETTINGS_THUMBNAILTHREADPRIORITY_ANIMATED:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREADPRIORITY_ANIMATED, &v->fThumbnailThreadAnimated);
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_SAVEALLUSESSAMENAME:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_SAVEALLUSESSAMENAME, &v->fSaveAllThumbnailsSameName);
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_FULLSIZESAVEDTHUMBNAILS:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_FULLSIZESAVEDTHUMBNAILS, &v->fSavedThumbnailsFullSize);
		break;
	case ID_SETTINGS_THUMBNAILTHREAD_SHOWSCRAMBLEDCHANNELS:
		ToggleMenuOption(hWnd, ID_SETTINGS_THUMBNAILTHREAD_SHOWSCRAMBLEDCHANNELS, &v->fShowScrambledChannels);
		break;
	case ID_SETTINGS_AUTOEXPANDPMTS:
		ToggleMenuOption(hWnd, ID_SETTINGS_AUTOEXPANDPMTS, &v->fAutoExpandPMTs);
		break;
	case ID_SETTINGS_AUTOEXPANDIPS:
		ToggleMenuOption(hWnd, ID_SETTINGS_AUTOEXPANDIPS, &v->fAutoExpandIPs);
		break;
	case ID_SETTINGS_IGNORETABLECRCERRORS:
		ToggleMenuOption(hWnd, ID_SETTINGS_IGNORETABLECRCERRORS, &v->fIgnoreTableCRCErrors);
		break;
	case ID_SETTINGS_DVHS_FORCEPIDSTOBEATSCCOMPATIBLE:
		ToggleMenuOption(hWnd, ID_SETTINGS_DVHS_FORCEPIDSTOBEATSCCOMPATIBLE, &v->fDVHSForceATSC);
		break;
	case ID_SETTINGS_KEEPSPECIALXMLCHARACTERS:
		ToggleMenuOption(hWnd, ID_SETTINGS_KEEPSPECIALXMLCHARACTERS, &v->fKeepSpecialXMLCharacters);
		break;
	case ID_SETTINGS_RELOADMANUALCHANNELSAFTERRESTART:
#ifndef LITE
		ToggleMenuOption(hWnd, ID_SETTINGS_RELOADMANUALCHANNELSAFTERRESTART, &v->fReloadManualChannels);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_SETTINGS_THUMBNAILTHREADPRIORITY_REFRESHRATE:
#ifdef PRO
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_THUMBNAIL_REFRESH_RATE_PRO), hWnd, ThumbnailRefreshRateDlgProc);
#else PRO
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_THUMBNAIL_REFRESH_RATE), hWnd, ThumbnailRefreshRateDlgProc);
#endif PRO
		break;
	case ID_SETTINGS_ROKUHD1000:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_ROKU1000_SETTINGS), hWnd, RokuHD1000Settings);
		break;
	case ID_FILE_DEFINEMANUALCHANNELS:
		DefineManualChannels(hWnd);
		break;
	case ID_FILE_LOADMANUALCHANNELS:
		LoadManualChannelsMenu(hWnd);
		break;
	case ID_FILE_SAVEMANUALCHANNELS:
		SaveManualChannels(hWnd);
		break;
	case ID_SETTINGS_XNSSERVER:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_XNS_SERVER), hWnd, XNSServerDlgProc);
		break;
	case ID_SETTINGS_VLC:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_VLC_SETTINGS), hWnd, VLCSettingsDlgProc);
		break;
	case ID_FILE_STOPSOURCE:
#ifdef LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
		Stop();
#endif LITE
		break;
	case ID_FILE_DISEQCPOSITIONER:
#ifdef LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#else LITE
		SourceHelper_DiSEqCPositionerDialog(hWnd);
#endif LITE
		break;
	case ID_SETTINGS_SERIALRECEVIERCONTROL:
#ifndef LITE
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_SERIAL_CONTROL), hWnd, SerialControlDlgProc);
#else LITE
		MessageBox(hWnd, szLiteWarning, gszAppName, MB_ICONSTOP);
#endif LITE
		break;
	case ID_VIEW_SHOWEPGGRID:
		ShowEPGGrid(hWnd);
		break;
	}
}

#ifndef LITE
BOOL CALLBACK AutoRecordNoProgramDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		MessageBeep(0);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_DONT_WARN:
			v->fDontWarnAboutInccorectAutoRecordProgram = IsDlgButtonChecked(hDlg, IDC_DONT_WARN);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}
	
	return FALSE;
}
#endif LITE

#ifdef PRO
void LoadForwardDLLs(HWND hWnd)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	HMENU hTopMenu = GetMenu(hWnd);
	HMENU hForwardMenu = GetSubMenu(hTopMenu, 5);
	char szCurrentDir[MAX_PATH];
	char szCurrentFile[MAX_PATH * 2];

	v->fwd.nForwarderDLLCount = 0;

	// Find receiver control modules
	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	sprintf(szCurrentFile, "%s\\Forwarders\\*.dll", szCurrentDir);
	hFind = FindFirstFile(szCurrentFile, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			wsprintf(szCurrentFile, "%s\\Forwarders\\%s", szCurrentDir, fd.cFileName);
			v->fwd.hDLL[v->fwd.nForwarderDLLCount] = LoadLibrary(szCurrentFile);
			if (v->fwd.hDLL[v->fwd.nForwarderDLLCount] != NULL)
			{
				char szForwarderDescription[MAX_PATH];

				v->fwd.functions[v->fwd.nForwarderDLLCount].Fwd_Init = (void *)GetProcAddress(v->fwd.hDLL[v->fwd.nForwarderDLLCount], "TSReader_Fwd_Init");
				v->fwd.functions[v->fwd.nForwarderDLLCount].Fwd_DeInit = (void *)GetProcAddress(v->fwd.hDLL[v->fwd.nForwarderDLLCount], "TSReader_Fwd_DeInit");
				v->fwd.functions[v->fwd.nForwarderDLLCount].Fwd_Data = (void *)GetProcAddress(v->fwd.hDLL[v->fwd.nForwarderDLLCount], "TSReader_Fwd_Data");
				v->fwd.functions[v->fwd.nForwarderDLLCount].Fwd_GetDescription = (void *)GetProcAddress(v->fwd.hDLL[v->fwd.nForwarderDLLCount], "TSReader_Fwd_GetDescription");

				v->fwd.functions[v->fwd.nForwarderDLLCount].Fwd_GetDescription(szForwarderDescription);
				AppendMenu(hForwardMenu, MF_BYPOSITION | MF_POPUP | MF_ENABLED, (unsigned int)ID_FORWARD_DLL_BASE + v->fwd.nForwarderDLLCount, (LPCTSTR)szForwarderDescription);

				v->fwd.nForwarderDLLCount++;
				if (v->fwd.nForwarderDLLCount == MAX_FWD_DLLS)
				{
					MessageBox(NULL, "Too many forwarders DLLs - please email support@tsreader.co.uk and tell me you saw this!", gszAppName, MB_ICONSTOP);
					break;
				}
			}
		} while (FindNextFile(hFind, &fd) == TRUE);
		FindClose(hFind);
	}
}
#endif PRO

LRESULT FAR PASCAL MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL fFirstTime;

	switch (uMsg)
	{
	case WM_CREATE:
		{
			fFirstTime = TRUE;
			v->hDlgSIParser = CreateDialog(v->hInstance, MAKEINTRESOURCE(IDD_SI_PARSER), hWnd, SIParserDlgProc);
			if (v->fSourceInitFailed)
			{
				DestroyWindow(hWnd);
				break;
			}
			ShowWindow(v->hDlgSIParser, SW_SHOWNORMAL);
			CreateStatusBar(hWnd);
			SetupMenu(hWnd);
			SetupVLCMenuNames(hWnd);
			UpdateThreadDialogPriorities(hWnd);
			SetInitialMenuStates(hWnd);
#ifdef PRO
			LoadForwardDLLs(hWnd);
#endif PRO
			if (v->fDisableStreamParsing == TRUE)
			{
				EnableMenuItem(GetMenu(hWnd), ID_SETTINGS_THUMBNAILTHREADPRIORITY_HIGH, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), ID_SETTINGS_THUMBNAILTHREADPRIORITY_NORMAL, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), ID_SETTINGS_THUMBNAILTHREADPRIORITY_LOW, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), ID_SETTINGS_THUMBNAILTHREADPRIORITY_DISABLED, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
				EnableMenuItem(GetMenu(hWnd), ID_SETTINGS_THUMBNAILTHREADPRIORITY_REFRESHRATE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND);
			}
			EnableDisableSourceMenuItems(hWnd);

            v->dwTaskbarRestartMessage = RegisterWindowMessage(TEXT("TaskbarCreated"));

			if (v->nAutoRecord != AUTO_RECORD_NONE || v->fAutoXMLExport == TRUE)
				PostMessage(hWnd, WM_ACTIVATE, 0, 0);

			/*if (v->fAllowResizing)
			{
				LONG lState = GetWindowLong(hWnd, GWL_STYLE);
				SetWindowLong(hWnd, GWL_STYLE, lState | WS_THICKFRAME | WS_MAXIMIZEBOX);
			}*/

		}
		break;
	case WM_DESTROY:
		DeleteStatusBar(hWnd);
		PostQuitMessage(0);
		break;
	case WM_COMMAND:
		ProcessMain_WM_COMMAND(hWnd, wParam, lParam);
		break;
	case WM_CLOSE:
#ifdef PRO
		{
			int nConnectionCount = 0;
			int i;

			if (v->fArchiveRunning == TRUE)
			{
				SendMessage(v->hWndArchiveRun, WM_CLOSE, 0, 0);
				if (v->fArchiveTerminate == FALSE)
					return 0;
			}
			if (v->fForwarderEnabled == TRUE)
			{
				SendMessage(v->hWndForwarderRun, WM_CLOSE, 0, 0);
			}
			for (i = 0; i < MAX_FWD_DLLS; i++)
			{
				if (v->fwd.fActive[i])
					ForwarderModuleStartStop(hWnd, i);
			}

			if (v->fMonitorRunning)
				SendMessage(v->hDlgStreamMonitor, WM_CLOSE, 0, 0);
			nConnectionCount = GetEITConnectionCount();
			if (nConnectionCount)
			{
				char szTemp[128];
				wsprintf(szTemp, "There are %d EIT Server connection(s) - are you want to quit?", nConnectionCount);
				if (MessageBox(hWnd, szTemp, gszAppName, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDNO)
					return 0;
			}
		}
#endif PRO

		SendMessage(v->hDlgSIParser, WM_CLOSE, 0, 0);
		if (v->fHideWhenMinimized)
		{
			if (v->fCurrentMinimized == TRUE)
			{
				NOTIFYICONDATA tnid;

				v->fCurrentMinimized = FALSE;

 				tnid.cbSize = sizeof(NOTIFYICONDATA); 
				tnid.hWnd = hWnd; 
				tnid.uID = 1; 
				Shell_NotifyIcon(NIM_DELETE, &tnid); 
				v->fBalloonQueued = FALSE;
			}
		}
#ifndef LITE
		if (!v->fMaximizedFlag && !v->fMinimizedFlag)
		{
			RECT rc;
			
			GetWindowRect(hWnd, &rc);
			v->nMainWindowSizeX = rc.right - rc.left;
			v->nMainWindowSizeY = rc.bottom - rc.top;
			v->nMainWindowPositionX = rc.left;
			v->nMainWindowPositionY = rc.top;
		}
#endif LITE
		DestroyWindow(hWnd);
		break;
	case WM_ACTIVATE:
		if (fFirstTime == TRUE)
		{
			fFirstTime = FALSE;
			PostMessage(hWnd, WM_USER + 5, 0, 0);
		}
		UpdatePIDChart(v->hDlgSIParser);
		UpdateStatistics(v->hDlgSIParser, FALSE);
		UpdateSourceInfo(v->hDlgSIParser);
//		InvalidateThumbnails();
		break;
	case WM_SIZE:
		v->fMinimizedFlag = wParam == SIZE_MINIMIZED;
		v->fMaximizedFlag = wParam == SIZE_MAXIMIZED;

		if (v->fHideWhenMinimized)
		{
			if (wParam == SIZE_RESTORED)
			{
				if (v->fCurrentMinimized == TRUE)
				{
					NOTIFYICONDATA tnid; 

					v->fCurrentMinimized = FALSE;

 					tnid.cbSize = sizeof(NOTIFYICONDATA); 
					tnid.hWnd = hWnd; 
					tnid.uID = 1; 
					Shell_NotifyIcon(NIM_DELETE, &tnid); 
					v->fBalloonQueued = FALSE;
				}
			}
			else if (wParam == SIZE_MINIMIZED)
			{
				if (v->fCurrentMinimized == FALSE)
				{
					ShowWindow(hWnd, SW_HIDE);
					v->fCurrentMinimized = TRUE;
					AddTaskbarIcon(hWnd);
				}
			}
		}
		SetWindowPos(v->hWndST, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOMOVE);
		ResizeStatusbar(v->hWndST);

		SendMessage(v->hDlgSIParser, WM_SIZE, wParam, lParam);
		break;
	case WM_USER:
		if (wParam && v->fMDPluginsLoaded == TRUE)
			MD__ExternCommandDispatch(hWnd, uMsg, wParam, lParam);
		break;
	case WM_USER + 5:
		{
			BOOL fAborted = FALSE;
			BOOL fLocked = FALSE;
			BOOL fOKToChangeQuietMode = FALSE;

			do
			{
				int nTuneLoop;
				BOOL fTuneDialogRequired = TRUE;

				if (v->fAutoRestartNoTuneDialog == TRUE && v->fAutoRestartNoDialogInProgress == TRUE)
				{
					fTuneDialogRequired = FALSE;
					v->fAutoRestartNoDialogInProgress = FALSE;
				}
				if (fTuneDialogRequired == TRUE)
				{
					CursorNormal();
					if (TuneDialog(hWnd) == FALSE)
					{
						OutputDebugString("TSReader: TuneDialog() returned FALSE - returning to source selection\n");
#ifndef LITE
						if ( (v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) && (v->ss.fSerialReceiverControlEnabled == TRUE) )
							SourceHelper_DeInitSerialControl();
#endif LITE

						// Cancelling the tune dialog takes us back to the source selection
						// dialog instead of exiting the app. If the user also cancels there,
						// then we exit.
						if (v->hSource)
						{
							FreeLibrary(v->hSource);
							v->hSource = NULL;
						}
						v->szSourceName[0] = 0;
						SourceHelper_ResetFirstTimeFlag();

						if (LoadSource(hWnd) == FALSE)
						{
							PostMessage(hWnd, WM_CLOSE, 0, 0);
							fAborted = TRUE;
							if (v->fQuietFromCommandLine)
								nTSReaderReturnValue = 1;
							break;
						}
						EnableDisableSourceMenuItems(hWnd);
						// Plugins keep their PSOURCESTRUCT pointer in a static set
						// by TSReader_Init. LoadSource only rebinds host-side
						// function pointers, so we must still call Init on the
						// fresh plugin before its TuneDialog runs — otherwise it
						// dereferences a NULL ss and faults at offset ~0x1851
						// (seen with TSReader_UDPMulticast.dll v2.6.0.41).
						Init(&v->ss);
						continue;		// retry with the newly-chosen source
					}
					CursorWait(hWnd);
				}

				if (!v->ss.fQuietMode)
					fOKToChangeQuietMode = TRUE;

				if (v->nDiSEqCPosition != -1)
					SourceHelper_DiSEqCPositionerDialog(hWnd);

				for (nTuneLoop = 0; nTuneLoop < v->nTunerLoops; nTuneLoop++)
				{
					char szTemp[128];

					if (fOKToChangeQuietMode == TRUE)
					{
						if (nTuneLoop == v->nTunerLoops - 1)
							v->ss.fQuietMode = FALSE;
						else
							v->ss.fQuietMode = TRUE;
					}
					fLocked = Tune();
					wsprintf(szTemp, "TSReader: TuneLoop = %d fLocked = %d v->ss.fQuietMode = %d\n",
						nTuneLoop, fLocked, v->ss.fQuietMode);
					OutputDebugString(szTemp);
					if (fLocked)
						break;
					if (!fLocked && v->fQuietFromCommandLine)
					{
						fAborted = TRUE;
						nTSReaderReturnValue = 1;
						PostMessage(hWnd, WM_CLOSE, 0, 0);
						break;
					}
				}
				if (!fLocked && v->fTuneFromControlServer)
				{
					v->fTuneFromControlServer = FALSE;
					return FALSE;
				}
			} while (!fLocked);
			v->fTuneFromControlServer = FALSE;
			OutputDebugString("TSReader: Past tune section - ready to run\n");
			CursorNormal();
			if (fAborted == FALSE)
			{
				UpdateMainStatusText("Reading PAT");
#ifndef LITE
				if (lstrlen(v->szAutoLoadManualChannelFilename))
				{
					LoadManualChannels(hWnd, v->szAutoLoadManualChannelFilename);
					if (v->nAutoRecord != AUTO_RECORD_NONE)
						v->nPMTTimeoutCounter = PMT_TIMEOUT;
				}
#endif LITE
				LoadPlugins(hWnd);
				StartIncomingTSDataThread();
#ifndef LITE
				ReadPersistantEPG();
#endif LITE
				Start();
				v->fRunning = TRUE;
				SetTimer(v->hDlgSIParser, 1, 1000, NULL);
				SetTimer(v->hDlgSIParser, 2, 100, NULL);
				SetTimer(v->hDlgSIParser, 6, v->nPIDDataRefreshRate, NULL);
				OutputDebugString("TSReader: Running!\n");
#ifndef LITE
				if (lstrlen(v->szAutoLoadPIDListFilename))
				{
					LoadPIDListFile(v->szAutoLoadPIDListFilename);
					v->szAutoLoadPIDListFilename[0] = '\0';
				}
#endif LITE

				if (v->fRunHidden)
				{
					v->fBlockResizeMessage = FALSE;
					SendMessage(hWnd, WM_SIZE, SIZE_MINIMIZED, 0);
				}
				{
					SYSTEMTIME stSystemTime;
					FILETIME ftNow;

					GetSystemTime(&stSystemTime);
					SystemTimeToFileTime(&stSystemTime, &ftNow);
					memcpy(&v->lnStartTime, &ftNow, sizeof(DWORD64));
				}
				InvalidateRect(v->hWndST, NULL, TRUE);
			}
		}
		break;
#ifdef LITE
	case WM_USER + 7:
		if (v->fRecording)
		{
			if (v->fStradisActive)
				StreamDecoder(hWnd);
			else
				RecordStream(hWnd, FALSE, -1);
			if (v->fWarnedAboutLite == FALSE)
			{
				v->fWarnedAboutLite = TRUE;
				PostMessage(hWnd, WM_COMMAND, IDC_LITE_WARNING, 0);
			}
		}
		break;
#endif LITE
#ifndef NIN_BALLOONSHOW
#define NIN_BALLOONSHOW WM_USER + 2
#define NIN_BALLOONHIDE WM_USER + 3
#define NIN_BALLOONTIMEOUT WM_USER + 4
#define NIN_BALLOONUSERCLICK WM_USER + 5
#endif NIN_BALLOONSHOW
#ifndef NIIF_NOSOUND
#define NIIF_NOSOUND 0x10
#endif NIIF_NOSOUND
	case WM_USER + 8:
		{
			UINT uMouseMsg = (UINT)lParam;

			if ((uMouseMsg == WM_LBUTTONUP) || (uMouseMsg == WM_RBUTTONUP))
			{
				ShowWindow(hWnd, SW_SHOW);
				ShowWindow(hWnd, SW_RESTORE);
			}
			else
			{
				if (v->nAutoRecord == AUTO_RECORD_PROGRAM)
				{
					if (v->fBalloonQueued == FALSE)
					{
						int nRemainingSeconds = v->nAutoRecordSeconds - v->nAutoRecordSecondCounter;
						int nHours, nMinutes;
						NOTIFYICONDATA IconData = {0};
						char szProgram[128];
						char szTemp[256 + MAX_PATH];
						char szTruncatedFilename[MAX_PATH];

						nHours = nRemainingSeconds / 3600;
						nRemainingSeconds -= nHours * 3600;
						nMinutes = nRemainingSeconds / 60;
						nRemainingSeconds -= nMinutes * 60;						
						
						if (v->pChannelData[v->pat.pmt[v->nSelectedProgram].nProgramNumber] == NULL)
							wsprintf(szProgram, "%d", v->pat.pmt[v->nSelectedProgram].nProgramNumber);
						else
							wsprintf(szProgram, "%d (%s)", v->pat.pmt[v->nSelectedProgram].nProgramNumber, 
							         v->pChannelData[v->pat.pmt[v->nSelectedProgram].nProgramNumber]->szShortName);

						lstrcpy(szTruncatedFilename, v->szAutoRecordFile);
						do
						{
							wsprintf(szTemp, "Recording program %s\nRecording to: %s\n%02d:%02d:%02d remaining",
								szProgram, szTruncatedFilename, nHours, nMinutes, nRemainingSeconds);
							if (lstrlen(szTemp) < sizeof(IconData.szInfo))
								break;
							memcpy(&szTruncatedFilename[0], &szTruncatedFilename[1], lstrlen(szTruncatedFilename) - 1);
						} while (lstrlen(szTruncatedFilename));
						lstrcpy(IconData.szInfo, szTemp);
												
						IconData.cbSize = sizeof(IconData);
						IconData.hWnd = hWnd;
						IconData.uFlags = NIF_INFO;
						IconData.uID = 1; 
						IconData.uTimeout = 10000; // in milliseconds
						IconData.dwInfoFlags =  NIIF_INFO | NIIF_NOSOUND;
				
						lstrcpy(IconData.szInfoTitle, gszAppName);
						Shell_NotifyIcon(NIM_MODIFY, &IconData);
						v->fBalloonQueued = TRUE;
					}
					else
					{
						if (uMouseMsg == NIN_BALLOONTIMEOUT || uMouseMsg == NIN_BALLOONUSERCLICK)
							v->fBalloonQueued = FALSE;
					}
				}
			}
		}
		break;
	case WM_USER + 9:
		switch(wParam)
		{
		case 0:
			{
				int nChartIndex = lParam;
				char szClassName[64];

				SetupChartClassName(szClassName, nChartIndex);
				UnregisterClass(szClassName, v->hInstance);	
			}
			break;
		case 1:
			UnregisterClass(gszEPGGridClass, v->hInstance);
			break;
		}
		break;
#ifndef LITE
	case WM_USER + 10:
		MessageBeep(0);
		break;
#endif LITE
#ifdef PRO
		// This message is from the archive monitor app - we get it's hWnd
		// to close it if the archiver is stopped
	case WM_USER + 11:
		v->hWndArchiveMonitor = (HWND)lParam;
		break;
	case WM_USER + 12:
		return v->fMinimizedFlag;
	case WM_USER + 13:
		MessageBox(hWnd, "A plugin has provided a descrambling key but no CSA plugin has been located by TSReader.\nSearch the Internet for CSA.DLL or preferably FFDecsa_64_MMX.dll and put it into the\nTSReader folder.\n\nTSReader does not include its own CSA code since this is patented.\n\nThis warning is displayed each time TSReader is run and no CSA decoder can be located.\nYou can stop it by removing the plugin or installing either of these files. FFDecsa_64_MMX.dll\nhas a significantly lower impact on CPU load and is highly recommended.", gszAppName, MB_ICONINFORMATION);
		break;
#endif PRO
#ifndef LITE
	case WM_USER + 14:
		DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_AUTO_RECORD_NO_PROGRAM), hWnd, AutoRecordNoProgramDlgProc);
		break;
#endif LITE
	default:
		if (uMsg == v->dwTaskbarRestartMessage)
		{
			if (v->fCurrentMinimized == TRUE)
				AddTaskbarIcon(hWnd);
		}
		else
			return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}

	return 0;
}

void InitVariables(HINSTANCE hInstance, int nCmdShow)
{
	int i;

	srand( (unsigned)time( NULL ) );
#ifdef _DEBUG
	{
		char szTemp[128];
		wsprintf(szTemp, "TSReader: Variable storage required = %d\n", sizeof(VARIABLES));
		OutputDebugString(szTemp);
	}
#endif _DEBUG

	// Setup variables
	v = LocalAlloc(LPTR, sizeof(VARIABLES));
	v->hEPGSaveHandle = INVALID_HANDLE_VALUE;
	v->nSavedCmdShow = nCmdShow;
	v->nMemoryUsageItems = 1000;
	v->memu = LocalAlloc(LPTR, v->nMemoryUsageItems * sizeof(MEMORYUSAGE));
	v->ss.hTSReaderInst = v->hInstance = hInstance;
	lstrcpy(v->szOutputPIDFlags, "0x%04x");
	memset(&v->nHighlightPIDs, -1, sizeof(v->nHighlightPIDs));
	v->nSelectedProgram	= -1;
	v->pat.nVersionNumber = -1;
	v->cat.nVersionNumber = -1;
	v->nCaptionPID = -1;
	for (i = 0; i < MAX_CHARTS; i++)
		v->nVideoCompositionPID[i] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_BAT] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_SDT] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_NIT] = -1;
	v->nSIParserVersionNumbers[SI_PARSER_STATS_EIT] = -1;
	v->nBATRightClickIndex = -1;
	v->nAutoExportDelay = 10;
	v->nPIDUsageStackedAreaChartIndex = -1;
	v->nDiSEqCPosition = -1;
	v->nNetworkPID = -1;
	v->nNITRightClickIndex = -1;
	v->nSDTRightClickIndex = -1;
	for (i = 0; i < MAX_ES_PARSERS; i++)
	{
		v->nESParsePID[i] = 0x1fff;
		InitializeCriticalSection(&v->esparserinfo[i].csThreadSignal);
	}
	v->nMuxRatePID = 0x1fff;
	v->nEITPID = 0x0012;
	v->nLastPATHighestVersionNumber = -1;
	v->nDCIIECMDescriptorPID = -1;
	v->nSkyEPGPIDs[0] = 0x36;
	v->nSkyEPGPIDs[1] = 0x46;

	lstrcpy(v->szHTTPContentType, "application/octet-stream");

	//lstrcpy(v->szRecordFile, "c:\\mpeg\\test.mpg");
#ifdef _DEBUG
	v->hDebugFile = CreateFile("c:\\tsreader.h264", GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES)NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
#endif _DEBUG
#ifdef INCLUDE_CSA
	v->fUseInternalCSA = TRUE;
#endif INCLUDE_CSA
	for (i = 0; i < 64; i++)
		v->nATSCEITPID[i] = v->nATSCETTPID[i] = 0x1fff;
	v->nATSCCETTPID = -1;
	v->hRegistryRoot = HKEY_CURRENT_USER;

	for (i = 0; i < 8192; i++)
		v->nPIDContinuity[i] = -1;
	for (i = 0; i < MAX_MGT_ENTRIES; i++)
		v->mgt[i].nTableType = -1;
	for (i = 0; i < MAX_SATELLITES; i++)
		v->sats[i].nOrbitalPosition = -1;
	for (i = 0; i < MAX_RECORD_BUFFERS; i++)
		v->nAutoRecordPIDsPID[i] = 0x1fff;
	for (i = 0; i < MAX_CVCT_ENTRIES; i++)
	{
		int j;

		v->cvct[i].transport_stream_id = -1;
		for (j = 0; j < MAX_CVCT_CHANNEL_ENTRIES; j++)
			v->cvct[i].CVCTEntry[j].major_channel_number = v->cvct[i].CVCTEntry[j].minor_channel_number = -1;
	}

	InitializeCriticalSection(&v->csEIT);
	InitializeCriticalSection(&v->ss.csPIDCounter);
	InitializeCriticalSection(&v->ss.csTSBuffersInUse);
	InitializeCriticalSection(&v->csThumbnails);	
	InitializeCriticalSection(&v->csPipeBytes);		
	InitializeCriticalSection(&v->csAutoRestartOnDataStopCounter);		
	InitializeCriticalSection(&v->csNextESPID);			
	InitializeCriticalSection(&v->csStatusbar);

#ifdef PRO
	InitializeCriticalSection(&v->csActualRecordFilename);
	InitializeCriticalSection(&v->csXMLLog);
	InitializeCriticalSection(&v->csH264VideoChart);
	{
		int nTableIndex;
		for (nTableIndex = 0; nTableIndex < MAX_RECORD_TABLES; nTableIndex++)
			v->record_tables[nTableIndex].nPID = -1;
	}
#endif PRO
	QueryPerformanceFrequency((LARGE_INTEGER *)&v->lnTicksPerSecond);

	SourceHelper_Init(v);

	// Load the scrambled channel bitmap
	{
		HISSRC hSourceObject;
		char szPictureName[MAX_PATH];

		SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szPictureName, sizeof(szPictureName));
		lstrcat(szPictureName, "\\scrambled.bmp");
		hSourceObject = _ISOpenFileSource(szPictureName);
		if (hSourceObject != NULL)
		{
			v->hScrambledPicture[0] = _ISReadBMPToRGB(hSourceObject, &v->dwScrambledPictureWidth[0], &v->dwScrambledPictureHeight[0]);
			_ISCloseSource(hSourceObject);

			v->dwScrambledPictureWidth[1] = (v->dwScrambledPictureWidth[0] * 2) / 3;
			v->dwScrambledPictureHeight[1] = (v->dwScrambledPictureHeight[0] * 2) / 3;
			v->hScrambledPicture[1] = GlobalAlloc(GPTR, v->dwScrambledPictureWidth[1] * v->dwScrambledPictureHeight[1] * 3);
			_ISDecimateRGB(v->hScrambledPicture[0],
				           v->dwScrambledPictureWidth[0], v->dwScrambledPictureHeight[0],
						   v->hScrambledPicture[1],
				           v->dwScrambledPictureWidth[1], v->dwScrambledPictureHeight[1]);

			v->dwScrambledPictureWidth[2] = v->dwScrambledPictureWidth[0] * 2;
			v->dwScrambledPictureHeight[2] = v->dwScrambledPictureHeight[0] * 2;
			v->hScrambledPicture[2] = GlobalAlloc(GPTR, v->dwScrambledPictureWidth[2] * v->dwScrambledPictureHeight[2] * 3);
			_ISResizeRGB(v->hScrambledPicture[0],
				           v->dwScrambledPictureWidth[0], v->dwScrambledPictureHeight[0],
						   v->hScrambledPicture[2],
				           v->dwScrambledPictureWidth[2], v->dwScrambledPictureHeight[2]);
		}
	}

	memset(v->out_pat, 0xff, sizeof(v->out_pat));
	memset(v->out_pmt, 0xff, sizeof(v->out_pmt));
	memset(v->out_sdt, 0xff, sizeof(v->out_sdt));

#ifndef LITE
	{
		HMODULE hWPCAP = LoadLibrary("wpcap.dll");
		if (hWPCAP != NULL)
		{
			FreeLibrary(hWPCAP);
			v->hUDPSender = LoadLibrary("TSReader_UDPSender.dll");
			if (v->hUDPSender != NULL)
			{
				UDPSender_GetDevices = (td_UDPSender_GetDevices)GetProcAddress(v->hUDPSender, "UDPSender_GetDevices");
				UDPSender_OpenDevice = (td_UDPSender_OpenDevice)GetProcAddress(v->hUDPSender, "UDPSender_OpenDevice");
				UDPSender_CloseDevice = (td_UDPSender_CloseDevice)GetProcAddress(v->hUDPSender, "UDPSender_CloseDevice");
				UDPSender_SendPacket = (td_UDPSender_SendPacket)GetProcAddress(v->hUDPSender, "UDPSender_SendPacket");
			}
		}
	}
#endif LITE
	i = 0;
	v->wChartMenuItems[i++] = ID_VIEW_PIDPIECHART;
	v->wChartMenuItems[i++] = ID_VIEW_PIDUSAGE2DPIECHART;
	v->wChartMenuItems[i++] = ID_VIEW_VIDEOBITRATECHART;
	v->wChartMenuItems[i++] = ID_VIEW_VIDEOBITRATEAREACHART;
	v->wChartMenuItems[i++] = ID_VIEW_MUXUSAGELINECHART;
	v->wChartMenuItems[i++] = ID_VIEW_MUXUSAGEAREACHART;
	v->wChartMenuItems[i++] = ID_VIEW_ACTIVEPIDSBYRATE;
	v->wChartMenuItems[i++] = ID_VIEW_ACTIVEPIDSBYPID;
	v->wChartMenuItems[i++] = ID_VIEW_CHART_PROGRAMUSAGESTACKEDBARCHART;
	v->wChartMenuItems[i++] = ID_VIEW_CHART_VIDEOCOMPOSITIONCHART;
	v->wChartMenuItems[i++] = ID_VIEW_CHART_SIGNALCHART;
}

void DeInitVariables()
{
	int nIndex;

	if (v->hThumbnailDC != NULL)
	{
		DeleteObject(v->memThumbnailBM);
		DeleteDC(v->hThumbnailDC);
	}

#ifdef PRO
#ifdef PRO
	if (v->fStreamingXMLMode)
	{
		if (v->XMLLog != NULL)
			LocalFree(v->XMLLog);
	}
#endif PRO
#endif PRO
#ifndef LITE
	if (v->hUDPSender != NULL)
		FreeLibrary(v->hUDPSender);
#endif LITE

	for (nIndex = 0; nIndex < 3; nIndex++)
	{
		if (v->hScrambledPicture[nIndex] != NULL)
			GlobalFree(v->hScrambledPicture[nIndex]);
	}

	DeleteCriticalSection(&v->csEIT);
	DeleteCriticalSection(&v->ss.csPIDCounter);
	DeleteCriticalSection(&v->ss.csTSBuffersInUse);
	DeleteCriticalSection(&v->csThumbnails);			
	DeleteCriticalSection(&v->csPipeBytes);			
	DeleteCriticalSection(&v->csAutoRestartOnDataStopCounter);			
	DeleteCriticalSection(&v->csNextESPID);			
	DeleteCriticalSection(&v->csStatusbar);
#ifdef PRO
	DeleteCriticalSection(&v->csActualRecordFilename);
	DeleteCriticalSection(&v->csXMLLog);
	DeleteCriticalSection(&v->csH264VideoChart);
#endif PRO
	
	for (nIndex = 0; nIndex < MAX_ES_PARSERS; nIndex++)
		DeleteCriticalSection(&v->esparserinfo[nIndex].csThreadSignal);

	for (nIndex = 0; nIndex < MAX_SATELLITES; nIndex++)
	{
		if (v->sats[nIndex].nOrbitalPosition != -1)
		{
			if (v->sats[nIndex].mux != NULL)
				LocalFree(v->sats[nIndex].mux);
		}
	}
#ifdef _DEBUG
	CloseHandle(v->hDebugFile);
#endif _DEBUG
	LocalFree(v->memu);
	LocalFree(v);
}

#ifndef LITE
BOOL GetCommandLineFilename(char * szCommandLinePtr, char * szOutputName, int * nOffset)
{
	if (*szCommandLinePtr == '\"')
	{
		char * szNextQuote = strstr(szCommandLinePtr + 1, "\"");
		if (szNextQuote == NULL)
			return FALSE;
		*szNextQuote = '\0';
		lstrcpy(szOutputName, szCommandLinePtr + 1);
		*nOffset = lstrlen(szOutputName) + 2;
		if (*(szNextQuote + 1) == ' ')
			*nOffset = (*nOffset) + 1;
		return TRUE;
	}

	{
		char * szNextSpace = strstr(szCommandLinePtr, " ");
		if (szNextSpace != NULL)
			*szNextSpace = '\0';
		lstrcpy(szOutputName, szCommandLinePtr);
		*nOffset = lstrlen(szOutputName);
		if (szNextSpace != NULL)
			*nOffset = (*nOffset) + 1;
	}
	return TRUE;
}

char * SkipCommandLineSwitch(char * szCmdLinePtr)
{
	do
	{
		if (*szCmdLinePtr == ' ')
		{
			szCmdLinePtr++; //past the space
			break;
		}
		if (*szCmdLinePtr == '\0')
			break;
		szCmdLinePtr++;
	} while (TRUE);

	return szCmdLinePtr;
}

char * ParseTSReaderCommandLine(char * szCmdLinePtr, BOOL * fResult)
{
	*fResult = TRUE;

	do
	{
		if (*szCmdLinePtr != '-')
			return szCmdLinePtr;

		switch(*(szCmdLinePtr + 1))
		{
		default:
			{
				char szTemp[128];
				wsprintf(szTemp, "Invalid switch -%c", *(szCmdLinePtr + 1));
				MessageBox(NULL, szTemp, gszAppName, MB_ICONSTOP);
			}
			return NULL;
		case '1':		// force uniprocessor
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fUniprocessorMode = TRUE;
			break;
		case 'a':		// record all
		case 'A':
			{
				int nConversionCount;
				int nNewOffset;
				char * szNextSpace;
				char * szNextSpace2;
				char szInvalidRecordParameters[] = {
					"Invalid -a parameters\n\n"
					"{file-size = MB to record to each file - with -A switch}\n"
					"service = service number to select\n"
					"file = filename to record to\n"
					"length = length of recording in seconds\n"};

				v->fSplitRecord = FALSE;
				if (*(szCmdLinePtr + 1) == 'A')
					v->fSplitRecord = TRUE;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);

				if (v->fSplitRecord == TRUE)
				{
					nConversionCount = sscanf(szCmdLinePtr, "%d", &v->nSplitFileSize);
					if ( (nConversionCount != 1) || (v->nSplitFileSize == 0) )
					{
						MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
						*fResult = FALSE;
						return NULL;
					}
					szNextSpace = strstr(szCmdLinePtr, " ") + 1;
					szCmdLinePtr = szNextSpace;
					v->fSplitSeconds = FALSE;
				}

				nConversionCount = sscanf(szCmdLinePtr, "%d", &v->nAutoRecordProgram);
				if ( (nConversionCount != 1) || (v->nAutoRecordProgram < 0) )
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace == NULL)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace++;
				if (GetCommandLineFilename(szNextSpace, v->szAutoRecordFile, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace += nNewOffset;
				nConversionCount = sscanf(szNextSpace, "%d", &v->nAutoRecordSeconds);
				if (nConversionCount == 0)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace2 = strstr(szNextSpace + 1, " ");
				if (szNextSpace2 != NULL)
					szCmdLinePtr = szNextSpace2 + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);

				v->nAutoRecord = AUTO_RECORD_ALL;
				v->fRecordLimit = TRUE;
			}
			break;
		case 'b':	// single thumbnail channel
			{
				char * szNextSpace;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					*szNextSpace = '\0';
				sscanf(szCmdLinePtr, "%d", &v->nSingleThumbnailChannel);
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);				
			}
			break;
		case 'B':	// disable blacklisting
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fDisableBlacklisting = TRUE;
			break;
		case 'c':	// remote control enable
			{
				char * szNextSpace;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					*szNextSpace = '\0';
				sscanf(szCmdLinePtr, "%d", &v->nControlServerPort);
				v->fControlServerEnabled = TRUE;
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);				
			}
			break;
		case 'C':	// receiver remote Control enable
			{
				int nIndex;
				int nNewOffset = 0;
				char * szNextSpace;
				char * szNextSpace2;
				char szReceiverType[128];
				char szSerialPort[32];

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = szCmdLinePtr;
				if (GetCommandLineFilename(szNextSpace, szReceiverType, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid receiver-type", gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace += nNewOffset;
				szNextSpace2 = strstr(szNextSpace, " ");
				if (szNextSpace2 != NULL)
					*szNextSpace2 = '\0';
				lstrcpy(szSerialPort, szNextSpace);
				strupr(szSerialPort);
				if (szNextSpace2 != NULL)
					szCmdLinePtr = szNextSpace2 + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);

				// Check for turn off
				if (lstrcmp(szReceiverType, "OFF") == 0)
				{
					v->ss.fSerialReceiverControlEnabled = FALSE;
					break;
				}

				// Check receiver type
				for (nIndex = 0; nIndex < v->nSerialReceiverControlIndex; nIndex++)
				{
					typedef char * (* td_GetReceiverName) ();
					char * (* GetReceiverName) ();
				
					GetReceiverName = (td_GetReceiverName)GetProcAddress(v->hSerialReceiverControl[nIndex], "GetReceiverName");
					if (GetReceiverName != NULL)
					{
						if (lstrcmp(GetReceiverName(), szReceiverType) == 0)
						{
							lstrcpy(v->szSerialReceiverType, GetReceiverName());
							break;
						}
					}
				}
				
				if (nIndex == v->nSerialReceiverControlIndex)
				{
					MessageBox(NULL, "Invalid serial receiver type", gszAppName, MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				lstrcpy(v->szSerialReceiverPort, szSerialPort);
				v->ss.fSerialReceiverControlEnabled = TRUE;
			}
			break;
		case 'd':	// disable stream parsing
			{
				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				v->fDisableStreamParsing = TRUE;
				v->nESParsingCounterReload = 0;
				v->nThumbnailProcessingThreadPriority = 3;
			}
			break;
		case 'D':	// force network type mode
			{
				v->fForcedNetworkType = TRUE;
				switch(szCmdLinePtr[2])
				{
				case 'a':		// ATSC mode
				case 'A':
					v->fIgnorePSIP = FALSE;				
					v->nNetworkPID = 0x1ffb;
					break;
				case 'd':		// DVB mode
				case 'D':
					v->fIgnorePSIP = TRUE;				
					v->nNetworkPID = 0x0010;
					v->fISDB = FALSE;
					break;
				case 'i':		// ISDB mode
				case 'I':
					v->fIgnorePSIP = TRUE;				
					v->nNetworkPID = 0x0010;
					v->fISDB = TRUE;
					break;
				case 'm':		// DCII mode
				case 'M':
					v->fIgnorePSIP = TRUE;				
					v->nNetworkPID = 0x0ffe;
					break;
				default:
					v->fForcedNetworkType = TRUE;
					break;
				}
				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			}
			break;
#ifdef PRO
		case 'e':	// EPG Save all (Pro)
			{
				int nNewOffset;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				if (GetCommandLineFilename(szCmdLinePtr, v->szEPGSaveFolder, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid -H filename", gszAppName, MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				v->hEPGSaveHandle = INVALID_HANDLE_VALUE;
				memset(&v->stEPGSaveCurrentDate, 0, sizeof(v->stEPGSaveCurrentDate));
				v->fEPGSaveFirstTime = TRUE;
				v->fEPGSaveEnabled = TRUE;
				szCmdLinePtr += nNewOffset;
			}
			break;
#endif PRO
		case 'E':	// extended EPGs
			{
				char * szNextSpace;

				szCmdLinePtr += 2; // skip '-E'
				switch(*szCmdLinePtr)
				{
				case 's':		// Sky EEPG
					v->fSkyEPG = TRUE;
					break;
				case 'd':		// Dish Network EEPG
					v->nEITPID = 0x0300;
					break;
				case 'b':		// BEV EEPG
					v->nEITPID = 0x0440;
					break;
				}
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);
			}
			break;
#ifdef PRO
		case 'f':	// Automatic forwarding
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fAutomaticForwarding = TRUE;
			break;
#endif PRO
		case 'h':	// Display chart
			{
				szCmdLinePtr += 2;	// skip '-h'
				v->nCommandLineChart = (int)*szCmdLinePtr - '0' + 1;
				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			}
			break;
		case 'H':	// HTML export
			{
				int nNewOffset;

				szCmdLinePtr += 2; // skip '-H'
				if (*szCmdLinePtr == '+')
				{
					char * szNextSpace = strstr(szCmdLinePtr, " ");
					if (szNextSpace == NULL)
					{
						MessageBox(NULL, "Invalid -H+ switch - a space is required", gszAppName, MB_ICONSTOP);
						*fResult = FALSE;
						return NULL;
					}
					*szNextSpace = '\0';
					szCmdLinePtr++;	// past the + to the start of the parameter
					sscanf(szCmdLinePtr, "%x", &v->nAutoHTMLExportFlags);
					*szNextSpace = ' ';
					szCmdLinePtr = szNextSpace + 1;
				}
				else
				{
					v->nAutoHTMLExportFlags = -1;
					szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				}

				if (GetCommandLineFilename(szCmdLinePtr, v->szXMLExportFilename, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid -H filename", gszAppName, MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr += nNewOffset;
				v->ss.fQuietMode = v->fAutoXMLExport = TRUE;
				v->fAutoXMLFormatAsXMLTV = -1;
			}
			break;
		case 'i':
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fSingleInstance = TRUE;
			break;
		case 'I':		// record PIDs
			{
				int nNewOffset;
				int nAutoRecordPIDIndex;
				char * szNextSpace;
				char szInvalidRecordParameters[] = {"Invalid -I parameters"};

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				if (GetCommandLineFilename(szCmdLinePtr, v->szAutoRecordPIDsFile, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr += nNewOffset;
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace == NULL)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				sscanf(szCmdLinePtr, "%d", &v->nAutoRecordPIDsDuration);
				szCmdLinePtr = szNextSpace + 1;
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace == NULL)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				sscanf(szCmdLinePtr, "%d", &v->nAutoRecordPIDsOptions);
				szCmdLinePtr = szNextSpace + 1;
				if (*szCmdLinePtr != '\"')
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr++;
				nAutoRecordPIDIndex = 0;
				while (*szCmdLinePtr != '\"' && nAutoRecordPIDIndex < 16)
				{
					if (*szCmdLinePtr == '0' && *(szCmdLinePtr + 1) == 'x')
						sscanf(szCmdLinePtr + 2, "%x", &v->nAutoRecordPIDsPID[nAutoRecordPIDIndex]);
					else
						sscanf(szCmdLinePtr, "%d", &v->nAutoRecordPIDsPID[nAutoRecordPIDIndex]);
					nAutoRecordPIDIndex++;
					do
					{
						if (*szCmdLinePtr == '\"' || *szCmdLinePtr == '\0')
							break;
						szCmdLinePtr++;
						if (*szCmdLinePtr == ' ')
						{
							szCmdLinePtr++;
							break;
						}
					} while (TRUE);
				}
				szCmdLinePtr++;
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);
			}
			break;
#ifdef PRO
		case 'l':
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fStreamingXMLMode = TRUE;
			break;
		case 'L':	// Profile select (Pro)
			{
				int nNewOffset;
				char szTemp[MAX_PATH];
				char szProfileName[MAX_PATH] = {0};

				szCmdLinePtr += 2; // skip '-L'
				if (*szCmdLinePtr != '\0')
				{
					szCmdLinePtr++;
					if (*szCmdLinePtr != '-')
					{
						if (GetCommandLineFilename(szCmdLinePtr, szProfileName, &nNewOffset) == FALSE)
						{
							MessageBox(NULL, "Invalid profile parameters", gszAppName, MB_OK | MB_ICONSTOP);
							*fResult = FALSE;
							return NULL;
						}
						szCmdLinePtr += nNewOffset;
						lstrcpy(szTemp, szProfileName);
						strlwr(szTemp);
						if (strcmp(szTemp, "default") == 0)
						{
							MessageBox(NULL, "The profile name 'Default' is reserved - to use this profile, remove the -L option", gszAppName, MB_ICONSTOP);
							*fResult = FALSE;
							return NULL;
						}
						if (strcmp(szTemp, "shell") == 0)
						{
							szProfileName[0] = '\0';
							v->fProfileBrowserShellMode = TRUE;
						}
					}
				}
				if (lstrlen(szProfileName) == 0)
					v->fShowProfileBrowser = TRUE;
				else
					lstrcpy(v->szProfileName, szProfileName);
			}
			break;
#endif PRO
		case 'm':	// load manual channel
			{
				int nNewOffset;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				if (GetCommandLineFilename(szCmdLinePtr, v->szAutoLoadManualChannelFilename, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid -m filename", gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr += nNewOffset;
			}
			break;
		case 'M':	// run minimized
			{
				if (szCmdLinePtr[2] == 'm')
				{
					v->fHideWhenMinimized = TRUE;
					v->fHideWhenMinimizedTemporary = TRUE;
					szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				}
				else
					szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				v->fRunHidden = TRUE;
			}
			break;
#ifdef PRO
		case 'n':
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fAutomaticRecordAll = TRUE;
			break;
		case 'N':
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fAutomaticStreamMonitor = TRUE;
			break;
#endif PRO
		case 'p':	// force program stream recording
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fRecordProgramStream = TRUE;	
			break;
		case 'P':	// go to DiSEqC position
			{
				int nPositionWest = -1;
				char * szNextSpace;
				char * szCmdLineTemp;
				char cSaveEastWest;

				if (*(szCmdLinePtr + 1) == 'U')
				{
					szCmdLinePtr++;
					v->fDiSEqCPostionIsUSALS = TRUE;
				}
				else
					v->fDiSEqCPostionIsUSALS = FALSE;
				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);

				// Find out if it's east or west
				if (*szCmdLinePtr == ' ')
					szCmdLinePtr++;
				szCmdLineTemp = szCmdLinePtr;
				while (*szCmdLineTemp != ' ' && *szCmdLineTemp != '\0')
				{
					if (*szCmdLineTemp == 'e' || *szCmdLineTemp == 'E')
					{
						nPositionWest = FALSE;
						break;
					}
					if (*szCmdLineTemp == 'w' || *szCmdLineTemp == 'W')
					{
						nPositionWest = TRUE;
						break;
					}
					szCmdLineTemp++;
				}
				if (nPositionWest == -1)
				{
					MessageBox(NULL, "Invalid positioner parameters - position needs east/west indicator", gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				cSaveEastWest = *szCmdLineTemp;
				*szCmdLineTemp = '\0';
				sscanf(szCmdLinePtr, "%d", &v->nDiSEqCPosition);
				if (nPositionWest == TRUE)
					v->nDiSEqCPosition |= 0x40000000;
				*szCmdLineTemp = cSaveEastWest; 
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace == NULL)
				{
					MessageBox(NULL, "Invalid positioner parameters - position and delay are required", gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr = szNextSpace + 1;
				sscanf(szCmdLinePtr, "%d", &v->nDiSEqCDelay);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);
			}
			break;
		case 'q':	// quiet mode
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->ss.fQuietMode = TRUE;
			v->fQuietFromCommandLine = TRUE;
			break;
		case 'r':
		case 'R':	// record program
			{
				int nConversionCount;
				int nNewOffset;
				char * szNextSpace;
				char * szNextSpace2;
				char szInvalidRecordParameters[] = {
					"Invalid -r parameters\n\n"
					"{file-size = MB to record to each file - with -R switch}\n"
					"service = service number to record\n"
					"file = filename to record to\n"
					"length = length of recording in seconds\n"};

				v->fSplitRecord = FALSE;
				if (*(szCmdLinePtr + 1) == 'R')
					v->fSplitRecord = TRUE;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);

				if (v->fSplitRecord == TRUE)
				{
					nConversionCount = sscanf(szCmdLinePtr, "%d", &v->nSplitFileSize);
					if ( (nConversionCount != 1) || (v->nSplitFileSize == 0) )
					{
						MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
						*fResult = FALSE;
						return NULL;
					}
					szNextSpace = strstr(szCmdLinePtr, " ") + 1;
					szCmdLinePtr = szNextSpace;
					v->fSplitSeconds = FALSE;
				}

				nConversionCount = sscanf(szCmdLinePtr, "%d", &v->nAutoRecordProgram);
				if ( (nConversionCount != 1) || (v->nAutoRecordProgram == 0) )
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace == NULL)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace++;
				if (GetCommandLineFilename(szNextSpace, v->szAutoRecordFile, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace += nNewOffset;

				nConversionCount = sscanf(szNextSpace, "%d", &v->nAutoRecordSeconds);
				if (nConversionCount == 0)
				{
					MessageBox(NULL, szInvalidRecordParameters, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace2 = strstr(szNextSpace + 1, " ");
				if (szNextSpace2 != NULL)
					szCmdLinePtr = szNextSpace2 + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);

				v->nAutoRecord = AUTO_RECORD_PROGRAM;
				v->fRecordProgramStream = FALSE;
				v->fRecordLimit = TRUE;
			}
			break;
		case 's':	// specify source name
			{
				char * szNextSpace;
				char szNewSourceName[128];

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					*szNextSpace = '\0';
				lstrcpy(szNewSourceName, szCmdLinePtr);
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szNewSourceName);

				SourceHelper_GetTSReaderEXEDirectory(v->hInstance, v->szSourceName, sizeof(v->szSourceName));
				lstrcat(v->szSourceName, "\\Sources\\");
				lstrcat(v->szSourceName, szNewSourceName);
			}
			break;
		case 'S':	// Source device number
			{
				char * szNextSpace;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					*szNextSpace = '\0';
				sscanf(szCmdLinePtr, "%d", &v->ss.nSourceIndex);
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);				
			}
			break;
		case 't':		// select audio track
			{
				char * szNextSpace;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					*szNextSpace = '\0';
				sscanf(szCmdLinePtr, "%i", &v->nAutoRecordAudioTrack);
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);				
			}
			break;
		case 'T':		// load PID list
			{
				int nNewOffset;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				if (GetCommandLineFilename(szCmdLinePtr, v->szAutoLoadPIDListFilename, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid -T filename", gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr += nNewOffset;
			}
			break;
		case 'u':	// auto record/stream subtitles
			szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
			v->fAutoRecordSubtitles = TRUE;
			break;
		case 'v':	// auto stream to VLC
		case 'V':	// auto stream to VLC, time limited
			{
				int nConversionCount;
				BOOL fTimeLimited = FALSE;
				char * szNextSpace;
				char * szErrorMessage;
				char szInvalidStreamParameters[] = {
					"Invalid -v parameters\n\n"
					"service = service number to stream\n"
					"config = TSReader's VLC configuration number (1-16)\n"};
				char szInvalidStreamParametersTimeLimited[] = {
					"Invalid -V parameters\n\n"
					"service = service number to stream\n"
					"config = TSReader's VLC configuration number (1-16)\n"
					"seconds = number of seconds to stream\n"};


				if (*(szCmdLinePtr + 1) == 'V')
				{
					fTimeLimited = TRUE;
					szErrorMessage = szInvalidStreamParametersTimeLimited;
				}
				else
					szErrorMessage = szInvalidStreamParameters;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);

				nConversionCount = sscanf(szCmdLinePtr, "%d", &v->nAutoRecordProgram);
				if (nConversionCount != 1)
				{
					MessageBox(NULL, szErrorMessage, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}

				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace == NULL)
				{
					MessageBox(NULL, szErrorMessage, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szNextSpace++;
				nConversionCount = sscanf(szNextSpace, "%d", &v->nAutoVLCConfiguration);
				if (nConversionCount != 1)
				{
					MessageBox(NULL, szErrorMessage, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				if (v->nAutoVLCConfiguration < 1 || v->nAutoVLCConfiguration > 16)
				{
					MessageBox(NULL, szErrorMessage, gszAppName, MB_OK | MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}

				// Time limited VLC parameter
				if (fTimeLimited)
				{
					szNextSpace = strstr(szNextSpace + 1, " ");
					if (szNextSpace == NULL)
					{
						MessageBox(NULL, szErrorMessage, gszAppName, MB_OK | MB_ICONSTOP);
						*fResult = FALSE;
						return NULL;
					}
					szNextSpace++;
					nConversionCount = sscanf(szNextSpace, "%d", &v->nAutoRecordSeconds);
					if (nConversionCount != 1)
					{
						MessageBox(NULL, szErrorMessage, gszAppName, MB_OK | MB_ICONSTOP);
						*fResult = FALSE;
						return NULL;
					}
				}

				szNextSpace = strstr(szNextSpace + 1, " ");
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);

				if (fTimeLimited == TRUE)
					v->nAutoRecord = AUTO_RECORD_VLC_TIME_LIMITED;
				else
					v->nAutoRecord = AUTO_RECORD_VLC;
			}
			break;
		case 'x':	// XML export
			{
				int nNewOffset;

				v->ss.fQuietMode = v->fAutoXMLExport = TRUE;
				v->fAutoXMLFormatAsXMLTV = FALSE;
				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);

				if (GetCommandLineFilename(szCmdLinePtr, v->szXMLExportFilename, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid -x filename", gszAppName, MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr += nNewOffset;
			}
			break;
		case 'X':	// XMLTV export
			{
				int nNewOffset;

				v->ss.fQuietMode = v->fAutoXMLExport = TRUE;		
				v->fAutoXMLFormatAsXMLTV = TRUE;
				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);

				if (GetCommandLineFilename(szCmdLinePtr, v->szXMLExportFilename, &nNewOffset) == FALSE)
				{
					MessageBox(NULL, "Invalid -X filename", gszAppName, MB_ICONSTOP);
					*fResult = FALSE;
					return NULL;
				}
				szCmdLinePtr += nNewOffset;
			}
			break;
		case 'y':	// Delay for auto export.
			{
				char * szNextSpace;

				szCmdLinePtr = SkipCommandLineSwitch(szCmdLinePtr);
				szNextSpace = strstr(szCmdLinePtr, " ");
				if (szNextSpace != NULL)
					*szNextSpace = '\0';
				sscanf(szCmdLinePtr, "%d", &v->nAutoExportDelay);
				if (szNextSpace != NULL)
					szCmdLinePtr = szNextSpace + 1;
				else
					szCmdLinePtr += lstrlen(szCmdLinePtr);				
			}
			break;
		}
	}
	while (TRUE);

	return szCmdLinePtr;
}
#endif LITE

void LoadOutputModules()
{
	// Try loading StradisDecoder.dll
	HMODULE hDecoder = LoadLibrary("StradisDecoder.dll");
	if (hDecoder != NULL)
	{
		FreeLibrary(hDecoder);
		v->fStradisInterface = TRUE;
	}
	hDecoder = LoadLibrary("TSReader_DS.dll");
	if (hDecoder != NULL)
	{
		FreeLibrary(hDecoder);
		v->fDSInterface = TRUE;
	}
	hDecoder = LoadLibrary("TSReader_DVHS.dll");
	if (hDecoder != NULL)
	{
		FreeLibrary(hDecoder);
		v->fDVHSInterface = TRUE;
	}
	hDecoder = LoadLibrary("TSReader_VLC.dll");
	if (hDecoder != NULL)
	{
		FreeLibrary(hDecoder);
		v->fVLCInterface = TRUE;
	}
}

void LoadOtherModules()
{
	WSADATA WSAData;
	
	// Load CSA code
	ptr_set_cws = NULL;
	ptr_decrypt = NULL;
	v->hCSA = LoadLibrary("csa.dll");
	if (v->hCSA == NULL)
	{
		char szTemp[MAX_PATH];

		// Try the plugins folder just incase
		SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szTemp, sizeof(szTemp));
		lstrcat(szTemp, "\\MDPlugins\\csa.dll");
		v->hCSA = LoadLibrary(szTemp);
	}

	if (v->hCSA != NULL)
	{
		ptr_set_cws = (td_ptr_set_cws)GetProcAddress(v->hCSA, "set_cws");
		ptr_decrypt = (td_ptr_decrypt)GetProcAddress(v->hCSA, "decrypt");
	}

	// Load FFDecsa_64_MMX.dll if there
	get_keyset_size = NULL;
	get_internal_parallelism = NULL;
	set_control_words = NULL;
	decrypt_packets = NULL;
	v->hFFCSA = LoadLibrary("FFDecsa_64_MMX.dll");
	if (v->hFFCSA == NULL)
	{
		char szTemp[MAX_PATH];

		// Try the plugins folder just incase
		SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szTemp, sizeof(szTemp));
		lstrcat(szTemp, "\\MDPlugins\\FFDecsa_64_MMX.dll");
		v->hFFCSA = LoadLibrary(szTemp);
	}
	if (v->hFFCSA != NULL)
	{
		int nTemp;

		get_keyset_size = (td_get_keyset_size)GetProcAddress(v->hFFCSA, "get_keyset_size");
		get_internal_parallelism = (td_get_internal_parallelism)GetProcAddress(v->hFFCSA, "get_parallelism");
		set_control_words = (td_set_control_words)GetProcAddress(v->hFFCSA, "set_control_words");
		decrypt_packets = (td_decrypt_packets)GetProcAddress(v->hFFCSA, "decrypt_packets");

		v->pFFKeySpaceBase = LocalAlloc(LPTR, (get_keyset_size() * 2) + 4);
		v->pFFKeySpace = v->pFFKeySpaceBase;
		while (((int)v->pFFKeySpace & 3) != 0)
			v->pFFKeySpace++;
		nTemp = get_internal_parallelism();
		v->nFFCSAPackets = nTemp + (nTemp / 10);
		if (v->nFFCSAPackets < nTemp + 5)
			v->nFFCSAPackets = nTemp + 5;
	}

	if (v->hCSA == NULL && v->hFFCSA == NULL)
		v->fWarnAboutCSA = TRUE;
#ifdef INCLUDE_CSA
	else
	{
		if (v->fUseInternalCSA)
		{
			ptr_set_cws = set_cws;
			ptr_decrypt = decrypt;
		}
	}
#endif INCLUDE_CSA

	// Winsock
	WSAStartup(0x0202, &WSAData);
	v->hXNSInterface = LoadLibrary("TSReader_XNS.dll");
	if (v->hXNSInterface != NULL)
	{
		XNS_SetupSocket = (td_XNS_SetupSocket)GetProcAddress(v->hXNSInterface, "SetupSocket");
		XNS_ShutdownSocket = (td_XNS_ShutdownSocket)GetProcAddress(v->hXNSInterface, "ShutdownSocket");
		v->fXNSInterface = TRUE;
	}
}

VOID UnloadOtherModules()
{
	if (v->hXNSInterface != NULL)
	{
		XNS_ShutdownSocket(v);
		FreeLibrary(v->hXNSInterface);
	}
	WSACleanup();

	if (v->hCSA != NULL)
		FreeLibrary(v->hCSA);

	if (v->hFFCSA != NULL)
	{
		LocalFree(v->pFFKeySpaceBase);
		FreeLibrary(v->hFFCSA);
	}

#ifdef PRO
	{
		int i;
		
		for (i = 0; i < v->fwd.nForwarderDLLCount; i++)
		{
			if (v->fwd.hDLL[i] != NULL)
			{
				FreeLibrary(v->fwd.hDLL[i]);
				v->fwd.hDLL[i] = NULL;
			}
		}
		v->fwd.nForwarderDLLCount = 0;
	}
#endif PRO
}

BOOL NEAR InitApplication()
{
	WNDCLASS wndclass;

	// register window class
	wndclass.style =         CS_DBLCLKS;
	wndclass.lpfnWndProc =   MainWndProc;
	wndclass.cbClsExtra =    0;
	wndclass.cbWndExtra =    0;
	wndclass.hInstance =     v->hInstance;
	wndclass.hIcon =         LoadIcon(v->hInstance, MAKEINTRESOURCE(IDI_DVBSMALL_LOGO));
	wndclass.hCursor =       LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) (COLOR_BACKGROUND);
	wndclass.lpszMenuName =  MAKEINTRESOURCE(IDR_MAIN);
	wndclass.lpszClassName = gszMainClass;

	return(RegisterClass(&wndclass));

}

HWND NEAR InitInstance(int nCmdShow)
{
	int nInitialXPos, nInitialYPos;
	int nInitialXSize, nInitialYSize;
	BOOL fMaximizedFlag;
	DWORD dwStyle;
	char szTitle[256] = {"\0"};
	char szProfileName[256] = {"Default"};

#ifdef PRO
	if (lstrlen(v->szProfileName))
		lstrcpy(szProfileName, v->szProfileName);
	wsprintf(szTitle, "%s -- %s %s", szProfileName, gszAppName, GetTSRVersion(NULL));
#else PRO
	wsprintf(szTitle, "%s -- %s", gszAppName, GetTSRVersion(NULL));
#endif PRO
#ifdef BETA
	lstrcat(szTitle, " BETA");
#endif BETA
#ifdef LITE
	lstrcat(szTitle, " - not for commerical use");
#endif LITE
	fMaximizedFlag = v->fMaximizedFlag; // save as it gets reset on the first WM_SIZE
	v->hAccel = LoadAccelerators(v->hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR_TOP));

	// create the window
#ifndef LITE
	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_SIZEBOX;
#define TSREADER_INITIAL_X 1000
#define TSREADER_INITIAL_Y 720
#else LITE
	dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU;
#define TSREADER_INITIAL_X 640
#define TSREADER_INITIAL_Y 480
#endif LITE

	nInitialXPos = 0;
	nInitialYPos = 0;
	nInitialXSize = TSREADER_INITIAL_X;
	nInitialYSize = TSREADER_INITIAL_Y;
	v->hWndMainWindow = CreateWindow(gszMainClass, szTitle,
						   dwStyle,
						   nInitialXPos, nInitialYPos,
						   nInitialXSize, nInitialYSize,
						   NULL, NULL, v->hInstance, NULL);

	if (NULL == v->hWndMainWindow)
		return (NULL);

#ifdef LITE
	{
		int xExtras, yExtras;
		RECT rcClient, rcStatusbar;

		xExtras = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXSIZEFRAME);
		yExtras = GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYSIZEFRAME);
		GetClientRect(v->hDlgSIParser, &rcClient);
		GetClientRect(v->hWndST, &rcStatusbar);
		SetWindowPos(v->hWndMainWindow, NULL,
			         0, 0,
					 rcClient.right + xExtras, rcClient.bottom + rcStatusbar.bottom + yExtras,
					 SWP_NOMOVE);
	}
#else LITE
	{
		if (v->nMainWindowPositionX < 0 || v->nMainWindowPositionY < 0)
			v->nMainWindowPositionX = v->nMainWindowPositionY = 0;
		if (v->nMainWindowSizeX <= 0 || v->nMainWindowSizeY <= 0)
		{
			v->nMainWindowSizeX = 640;
			v->nMainWindowSizeY = 480;
		}

		SetWindowPos(v->hWndMainWindow,
					 HWND_TOP,
					 v->nMainWindowPositionX, v->nMainWindowPositionY,
					 v->nMainWindowSizeX, v->nMainWindowSizeY,
					 0);
	}
#endif LITE
	v->fMaximizedFlag = fMaximizedFlag;
	if (v->fRunHidden)
	{
		v->fBlockResizeMessage = TRUE;
		ShowWindow(v->hWndMainWindow, SW_SHOWMINIMIZED);
	}
	else
	{
		if (v->fMaximizedFlag == TRUE)
			nCmdShow = SW_MAXIMIZE;
		ShowWindow(v->hWndMainWindow, nCmdShow);
		UpdateWindow(v->hWndMainWindow);
	}

	return (v->hWndMainWindow);
}

BOOL CALLBACK BombWarningDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		MessageBeep(0);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(hDlg, TRUE);
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_BOMB_DONT_TELL:
			v->fDontShowBombDialog = IsDlgButtonChecked(hDlg, IDC_BOMB_DONT_TELL);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

void DoNormalTSReaderWindow(char * szCmdLinePtr)
{
	BOOL fBombed = FALSE;

	if (LoadSource(NULL) == TRUE)
	{
#ifndef LITE
		v->szSourceParametersPtr = szCmdLinePtr;
		if (ParseSourceModuleCommandLine(&v->ss, szCmdLinePtr, FALSE) == TRUE)
#endif LITE
		{
			if (DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_LICENSE), NULL, LicenseDlgProc) == TRUE)
			{
				LoadOtherModules();
				LoadOutputModules();
#ifndef LITE
				if (v->fControlServerEnabled)
					StartControlServer();
#ifdef PRO
				if (v->fEITServerEnabled)
				{
					if (StartEITServer() == FALSE)
						MessageBox(NULL, "The EIT server failed to start", gszAppName, MB_ICONSTOP);
				}
#endif PRO
#endif LITE
				if (InitApplication() != FALSE)
				{
					if (InitInstance(v->nSavedCmdShow) != NULL)
					{
						MSG msg;

						ToggleAlwaysOnTop();
						SetProcessPriority();
#ifndef _DEBUG
 #define CATCH_ERRORS
#endif _DEBUG
#ifdef CATCH_ERRORS
						__try
						{
#endif CATCH_ERRORS
							while (GetMessage(&msg, NULL, 0, 0) == TRUE)
							{
								if (msg.hwnd == v->hWndMainWindow)
								{
									if (!(TranslateAccelerator(msg.hwnd, v->hAccel, &msg)))
										TranslateMessage(&msg);
								}
								{
									int i;
									for (i = 0; i < 16; i++)
									{
										if (IsWindow(v->hPluginTranslateDialog[i]))
										{
											if (v->hPluginTranslateDialog[i] == msg.hwnd)
												IsDialogMessage(v->hPluginTranslateDialog[i], &msg);
										}
									}
								}
								DispatchMessage(&msg);
							}
#ifdef CATCH_ERRORS
						} __except(EXCEPTION_EXECUTE_HANDLER )
						{
							// main loop crashed
							fBombed = TRUE;
						}
#endif CATCH_ERRORS
					}
				}
#ifndef LITE
				TerminateControlServer();
#ifdef PRO
				TerminateEITServer();
#endif PRO
#endif LITE
				UnloadOtherModules();
			}
		}
		FreeLibrary(v->hSource);
	}
	if (fBombed)
	{
		OutputDebugString("TSReader: Main GetMessage() loop crashed\n");
		//if (v->fDontShowBombDialog == FALSE)
		//	DialogBox(v->hInstance, MAKEINTRESOURCE(IDD_BOMB_WARNING), NULL, BombWarningDlgProc);
	}
}

void LoadSerialReceiverDLLs()
{
	HANDLE hFind;
	char szCurrentDir[MAX_PATH];
	char szCurrentFile[MAX_PATH * 2];
	WIN32_FIND_DATA fd;

	v->nSerialReceiverControlIndex = 0;

	// Find receiver control modules
	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	sprintf(szCurrentFile, "%s\\Sources\\ReceiverControl\\*.dll", szCurrentDir);
	hFind = FindFirstFile(szCurrentFile, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			wsprintf(szCurrentFile, "%s\\Sources\\ReceiverControl\\%s", szCurrentDir, fd.cFileName);
			v->hSerialReceiverControl[v->nSerialReceiverControlIndex] = LoadLibrary(szCurrentFile);
			v->nSerialReceiverControlIndex++;
			if (v->nSerialReceiverControlIndex == MAX_SERIAL_RECEIVERS)
			{
				MessageBox(NULL, "Too many receiver control modules - please email support@tsreader.co.uk and tell me you saw this!", gszAppName, MB_ICONSTOP);
				break;
			}
		} while (FindNextFile(hFind, &fd) == TRUE);
		FindClose(hFind);
	}
}

void UnloadSerialReceiverDLLs()
{
	int nIndex;

	for (nIndex = 0; nIndex < v->nSerialReceiverControlIndex; nIndex++)
		FreeLibrary(v->hSerialReceiverControl[nIndex]);
}

#ifdef PRO
BOOL CheckHASP()
{
	hasp_handle_t handle = 0;
	hasp_status_t status;
	const hasp_feature_t feature = HASP_PROGNUM_DEFAULT_FID | HASP_PROGNUM_OPT_NO_REMOTE;
	unsigned char vendor_code[] =
	"yndP2uWijYQMGG+hZCdCOXmTNSo7dJouB9ZIva4AVZj5pqY/F3iThD208T7zMp/l7hVXybbl/ujO2Xf/"
	"0g6RX/2gJdUtQJFUDB0Ygx7ASLNQ1i/ISh6O88HoZqKGTDAmoRuRYOe16G03QQEsDQkY+BYeOLEqj+LF"
	"Qx2m05v4zeWqvd+ReoW0XBgMih5vjCb+DplooTsMoXvgTkIag2EZTU6OIYkrZZ2p3EOHp3SwNbHgqZrl"
	"k1MxZ/yJWCrCB26neFQx2QB23WoaDds+vKPMwrTAAxmCPkivKURRVL/LbDnegsIjqF+himXporeZ+FQK"
	"OhN7r4VZSUFKu4dA4YTz18G9E+pemP7NUkBH5wcNXH7c6AqyKQmAynfmfwJju+dkdVz/XU342966UcQJ"
	"aKR67ZGRk/uZHJbzLVchwNZE58sZZjKQEncpyBgTZ20yY4etFXjJtNgUZ8tOqp6ooJONKIN4VuQWl/YL"
	"Rpvh6ow3EH5zUDF6WXOA59wxaRnXqjrGknJFHrUOUXSDfHFT1BkI9k02L1+MqDywHJh7a4vd7SfdbEY7"
	"Q7hDOnKcMteAQyX1DRmIKllFGsNy25x+OL0PsiGp8GNovipQr0R2lOdVXM9lQ6N6+nmu2jeJceqVc34r"
	"Cc1/AdvpZhL/RD4kfRaVZlQbSz7HWNsupp5tNG2mCaDGLK2pm5JXPhx5S92XV2AfFCIi9CBDyxoASB8P"
	"6pfvAXklCaXH";

	// login - if we're normal unprotected TSR this'll fail
	// but if we're protected and the key isn't there we'll
	// never run anyway
	status = hasp_login(feature, vendor_code, &handle);
	if (status == HASP_STATUS_OK)
	{
		hasp_status_t memSize = 0;

		status = hasp_get_size(handle, HASP_FILEID_MAIN, &memSize);
		if (status == HASP_STATUS_OK)
		{
			if (memSize != 0)
			{	
				// We have a time key - let's see if this has been inited
				hasp_time_t time;
				unsigned char data[16];

				status = hasp_read(handle, HASP_FILEID_MAIN, 0, 16, data);
				status = hasp_decrypt(handle, data, 16);
				if (data[15] == 0xde)
				{
					// This key in virgin
					status = hasp_get_rtc(handle, &time);
					time += 28 * 24 * 60 * 60;
					memcpy(&data[0], &time, sizeof(time));
					data[15] = 0xdb;
					status = hasp_encrypt(handle, data, 16);
					status = hasp_write(handle, HASP_FILEID_MAIN, 0, 16, data);
				}
				else if (data[15] == 0xdb)
				{
					hasp_time_t expire_time;

					memcpy(&expire_time, &data[0], sizeof(expire_time));
					status = hasp_get_rtc(handle, &time);
					if (time > expire_time)
					{
						char szError[256];

						// Expired
						hasp_logout(handle);
						wsprintf(szError, "This demo version of %s has reached it's 28 day limit.\n\nPlease contact the organization you received the key from for instructions\non how to contuine. Do NOT discard the USB key - it must be returned.", gszAppName);
						MessageBox(NULL, szError, gszAppName, MB_ICONSTOP);
						return FALSE;
					}
				}
				else
				{
					// Unknown data in the key - let's just quit
					hasp_logout(handle);
					return FALSE;
				}
			}
		}

		// All done with the key - logout
		status = hasp_logout(handle);
	}

	return TRUE;
}

#endif PRO

// Memorial splash screen for Rod Hewitt KG6TTD (G6TTD), defined in splash.c
extern void ShowMemorialSplash(HINSTANCE hInstance);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	BOOL fOK;
	char * szCmdLinePtr = lpszCmdLine;
	char szTemp[128];
	char szVersion[32];

	// In memory of Rod Hewitt KG6TTD (G6TTD) - shows for 5 seconds, click/key to dismiss
	ShowMemorialSplash(hInstance);

	nTSReaderReturnValue = 0;
	wsprintf(szTemp, "%s: Version %s Built %s %s\n", gszAppName, GetTSRVersion(szVersion), __DATE__, __TIME__);
	OutputDebugString(szTemp);

	lstrcat(gszRestartNeeded, gszAppName);

#ifndef LITE
	{
		char szTemp2[1024];
		wsprintf(szTemp2, "TSReader: Commandline: %s\n", lpszCmdLine);
		OutputDebugString(szTemp2);
	}
#endif LITE
#ifdef BETA
	{
		SYSTEMTIME systime;
			
		GetLocalTime(&systime);
		if (systime.wYear >= BETA_EXPIRE_YEAR)
		{
			if ( (systime.wMonth > BETA_EXPIRE_MONTH) || (systime.wYear > BETA_EXPIRE_YEAR) )
			{
				MessageBox(NULL, TEXT("This beta release has expired.\n\nPlease visit https://tsreader.co.uk to obtain an up to\ndate version of this product"), gszAppName, MB_OK | MB_ICONSTOP);
				return 0;
			}
		}
	}
#endif BETA

	// Some setup stuff
	_ISInitialize("{4E3E4954-F9B5-11d2-A085-00500402F30B}");
	SourceHelper_CRC_Init();
	{
		INITCOMMONCONTROLSEX iccex; 

		iccex.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_COOL_CLASSES | ICC_DATE_CLASSES ;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCommonControlsEx(&iccex);
	}
	InitVariables(hInstance, nCmdShow);
	LoadSerialReceiverDLLs();
	LoadSettings();
	if (v->fDecimalPIDs)
		lstrcpy(v->szOutputPIDFlags, "%d");
	else
		lstrcpy(v->szOutputPIDFlags, "0x%04x");

	// OEM versions need to check for the correct command-line parameters
#ifdef INEOQUEST
	{
		char szGUID[] = "{C0F5E3B4-5CF2-4275-8938-DDE7381812EF}";
		if (memcmp(lpszCmdLine, szGUID, sizeof(szGUID) - 1) != 0)
			return 0;
		szCmdLinePtr += sizeof(szGUID) - 1;
	}
#endif INEOQUEST

#ifdef LITE
	if (lstrlen(szCmdLinePtr))
		MessageBox(NULL, "Command-line parameters are not supported by this version of TSReader. Parameters ignored.", gszAppName, MB_ICONWARNING);
	fOK = TRUE;
#else LITE
	szCmdLinePtr = ParseTSReaderCommandLine(szCmdLinePtr, &fOK);
#ifdef PRO
	if (lstrlen(v->szProfileName))
	{
		LoadSettings();
		if (v->fHideWhenMinimizedTemporary == TRUE)
			v->fHideWhenMinimized = TRUE;
	}
	if (fOK && v->fShowProfileBrowser == TRUE)
	{
		if (ShowProfileBrowser() == FALSE)
			goto PreDeInitVariables;
		LoadSettings();		// profile may have changed
		if (v->fDecimalPIDs)
			lstrcpy(v->szOutputPIDFlags, "%d");
		else
			lstrcpy(v->szOutputPIDFlags, "0x%04x");
	}
#endif PRO
#endif LITE
	if (fOK == TRUE)
	{
#ifdef PRO
		if (lstrlen(v->szProfileName))
		{
			lstrcat(gszMainClass, ".");
			lstrcat(gszMainClass, v->szProfileName);
		}

		if (CheckHASP() == FALSE)
			goto PreDeInitVariables;
#endif PRO

		if (v->fSingleInstance)
		{
			BOOL fSentClose = FALSE;

			do
			{
				HWND hWnd = FindWindow(gszMainClass, NULL);
				if (hWnd == NULL)
					break;
				if (fSentClose == FALSE)
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					fSentClose = TRUE;
				}
				Sleep(50);
			} while (TRUE);

			if (fSentClose == TRUE)
				Sleep(500);
		}

		DoNormalTSReaderWindow(szCmdLinePtr);
	}

	SaveSettings();
PreDeInitVariables:
	UnloadSerialReceiverDLLs();
	DeInitVariables();

	return nTSReaderReturnValue;

	goto PreDeInitVariables; // stop compiler complaining
}

// LNB Power: Vertical = 12v Horizontal = 18v
// Horizontal = LHCP
// Veritcal = RHCP
// low-density parity check (LDPC)

