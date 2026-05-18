#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <commctrl.h>
#include <math.h>
#include <shlobj.h>

#include "..\TSReader.h"
#include "resource.h"
#include "serial.h"
#include "atscbandplans.h"
#include "dvbcbandplans.h"
#include "dvbtbandplans.h"
#include "isdbbandplans.h"

PVARIABLES v;
PSOURCESTRUCT ss;
HANDLE hInstance;
BOOL fTuneDialogFirstTime;
int nATSCQAMScanRangeStart, nATSCQAMScanRangeEnd;

static int nSwitchButtons[] = {IDC_DISEQC1, IDC_DISEQC2, IDC_DISEQC3, IDC_DISEQC4, IDC_INPUT_TB_A, IDC_INPUT_TB_B};

#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#define TO_RADS (M_PI / 180.0)
#define TO_DEC (180.0 / M_PI)

#define SOCKET_MODE_UDP_MULTICAST 0
#define SOCKET_MODE_UDP_UNICAST 1
#define SOCKET_MODE_TCP 2

typedef struct _tagSDXBuildList
{
	int nOrbital;
	int nFrequency;
	int nSymbolRate;
	int nCodeRate;
	int nPolarity;
	int nModulationType;
	char szSatelliteName[24];
	char szMuxName[128];
} SDXBUILDLIST, *PSDXBUILDLIST;

typedef struct _tagUDPList
{
	char szAddress[128];
	int nPort;
	char szInterfaceAddress[128];
	char szDescription[256];
} UDPLIST, *PUDPLIST;
#define MAX_UDP_LIST_ENTRIES 1000

// This should be moved into VARIABLES
PUDPLIST pUDPList;
SWITCHPARAMETERS spcopy[MAX_SWITCH_PARAMETERS];
// This should be moved into VARIABLES

BOOL (*SendDiSEqC) (BYTE * bCommand, int nLength);

char szCurrentSatFile[48] = {0};
char szTypeName[4] = {"DVB"};
char gszAppName[] = {"TSReader Source Helper"};

void CursorNormal()
{
	ReleaseCapture();
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void CursorWait(HWND hWnd)
{
	SetCapture(hWnd);
	SetCursor(LoadCursor(NULL, IDC_WAIT));
}
	
BOOL SourceHelper_myGetOpenFileName(LPOPENFILENAME lpofn)
{
	int i;
	char * szFileName = lpofn->lpstrFile;
	char * szInitialDir = (char *)lpofn->lpstrInitialDir;

	if (GetOpenFileName(lpofn) == FALSE)
		return FALSE;

	strcpy(szInitialDir, szFileName);
	for (i = strlen(szInitialDir); i > 0; i--)
	{
		if (szInitialDir[i] == '\\')
			break;
		szInitialDir[i] = 0;
	}
	return TRUE;
}

void SourceHelper_SetWorkerThreadPriorities(BOOL fStreamProcessingThread)
{
	if (!v)
		return;

	if (fStreamProcessingThread)
	{
		switch(v->nStreamProcessingThreadPriority)
		{
		case 0:
			SetThreadPriority(v->hStreamProcessingThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(v->hStreamProcessingThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(v->hStreamProcessingThread, THREAD_PRIORITY_IDLE);
			break;
		}
	}
	else
	{
		switch(v->ss.nInputThreadPriority)
		{
		case 0:
			SetThreadPriority(v->ss.hReadDataThread, THREAD_PRIORITY_NORMAL);
			break;
		case 1:
			SetThreadPriority(v->ss.hReadDataThread, THREAD_PRIORITY_HIGHEST);
			break;
		case 2:
			SetThreadPriority(v->ss.hReadDataThread, THREAD_PRIORITY_IDLE);
			break;
		case 3:
			SetThreadPriority(v->ss.hReadDataThread, THREAD_PRIORITY_TIME_CRITICAL);
			break;
		}
	}
}

void SetupSatelliteListViews(HWND hDlg)
{
	int nColumnPosition = 0;
	HWND hWndSatelliteList = GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST);
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_TUNER_MUX_LIST);
	LV_COLUMN lvc; 
	char szTemp[128];

	// Initialize the LV_COLUMN structure. 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	lvc.pszText = szTemp; 

	// Satellite list first. 
	lvc.fmt = LVCFMT_LEFT; 
	lvc.cx = 50; 
	lstrcpy(szTemp, TEXT("Orbit"));
	ListView_InsertColumn(hWndSatelliteList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_LEFT; 
	lvc.cx = 110; 
	lstrcpy(szTemp, TEXT("Name"));
	ListView_InsertColumn(hWndSatelliteList, nColumnPosition++, &lvc); 

	ListView_SetExtendedListViewStyle(hWndSatelliteList, LVS_EX_FULLROWSELECT);

	// Mux list next
	nColumnPosition = 0;
	lvc.fmt = LVCFMT_LEFT; 
	lvc.cx = 50; 
	lstrcpy(szTemp, TEXT("Freq"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 35; 
	lstrcpy(szTemp, TEXT("Pol"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 50; 
	lstrcpy(szTemp, TEXT("SR"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_LEFT; 
	lvc.cx = 120; 
	lstrcpy(szTemp, TEXT("Name"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	ListView_SetExtendedListViewStyle(hWndMuxList, LVS_EX_FULLROWSELECT);

}

void PopulateSatelliteListView(HWND hDlg)
{
	int nIndex;
	int i;
	HWND hWndSatelliteList = GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST);
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_TUNER_MUX_LIST);
	LV_ITEM lvi; 

	v->nInitialMuxIndex = -1;
	v->nInitialSatelliteIndex = -1;
	v->fBlockFirstSatelliteListSelection = FALSE;
	v->fBlockFirstMuxListSelection = FALSE;

	// Populate satellite list
	for (nIndex = 0; nIndex < v->nSatellites; nIndex++)
	{
		// Initialize item-specific LV_ITEM members. 
		memset(&lvi, 0, sizeof(lvi));
		lvi.state = 0; 
		lvi.stateMask = 0; 
		lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
		lvi.iItem = nIndex; 
		lvi.iSubItem = 0; 
		lvi.lParam = (LPARAM) 0;    // item data 
		ListView_InsertItem(hWndSatelliteList, &lvi);
	}

	// Select the current satellite so we update the muxes
	if (lstrlen(v->szCurrentlySelectedSatellite))
	{
		for (i = 0; i < v->nSatellites; i++)
		{
			if (lstrcmp(v->szCurrentlySelectedSatellite, v->sats[i].szName) == 0)
			{
				v->nInitialSatelliteIndex = i;
				break;
			}
		}
	}
	if (v->nInitialSatelliteIndex != -1)
	{
		v->fBlockFirstSatelliteListSelection = TRUE;
		lvi.mask = LVIF_STATE;
		lvi.iItem = v->nInitialSatelliteIndex;
		lvi.iSubItem = 0;
		lvi.stateMask = LVIS_SELECTED | LVIS_DROPHILITED;
		lvi.state = LVIS_SELECTED | LVIS_DROPHILITED;
		ListView_SetItem(hWndSatelliteList, &lvi);
		ListView_EnsureVisible(hWndSatelliteList, v->nInitialSatelliteIndex, FALSE);

		// Select the current mux if there's a match
		for (i = 0; i < v->sats[v->nInitialSatelliteIndex].nMuxCount; i++)
		{
			if (   (v->sats[v->nInitialSatelliteIndex].mux[i].nFrequency == v->ss.nFrequency)
				&& (v->sats[v->nInitialSatelliteIndex].mux[i].nSymbolRate == v->ss.nSymbolRate) )
			{
				v->nInitialMuxIndex = i;
				break;
			}
		}
		if (v->nInitialMuxIndex != -1)
		{
			v->fBlockFirstMuxListSelection = TRUE;
			lvi.mask = LVIF_STATE;
			lvi.iItem = v->nInitialMuxIndex;
			lvi.iSubItem = 0;
			lvi.stateMask = LVIS_SELECTED | LVIS_DROPHILITED;
			lvi.state = LVIS_SELECTED | LVIS_DROPHILITED;
			ListView_SetItem(hWndMuxList, &lvi);
			ListView_EnsureVisible(hWndMuxList, v->nInitialMuxIndex, FALSE);
		}
	}
}

void GetSatelliteDispInfo(LV_DISPINFO *pnmv) 
{ 
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nSatelliteIndex = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			{
				char szEW[2] = {"E"};
				if (v->sats[nSatelliteIndex].fWest)
					lstrcpy(szEW, "W");
				sprintf(pnmv->item.pszText, "%.1f %s", (double)v->sats[nSatelliteIndex].nOrbitalPosition / 10.0f, szEW);
			}
			break;
		case 1:
			lstrcpy(pnmv->item.pszText, v->sats[nSatelliteIndex].szName);
			break;
		}
	}
}

void GetMuxDispInfo(LV_DISPINFO *pnmv) 
{
	if (v->nCurrentSelectedSatellite == -1)
		return;

    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nMuxIndex = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			wsprintf(pnmv->item.pszText, "%d", v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nFrequency);
			break;
		case 1:
			lstrcpy(pnmv->item.pszText, v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].szPolarity);
			break;
		case 2:
			wsprintf(pnmv->item.pszText, "%d", v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nSymbolRate);
			break;
		case 3:
			lstrcpy(pnmv->item.pszText, v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].szMuxName);
			break;
		}
	}
}

int SetupCodeRateOptions(HWND hDlg, int nADVModulationMode)
{
	int nAutoFECSelect = 0;

	while (SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_DELETESTRING, 0, 0) != CB_ERR)
		;

	switch(nADVModulationMode)
	{
	case ADV_MOD_DVB_QPSK:
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/2");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"2/3");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"5/6");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"7/8");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		nAutoFECSelect = 5;
		break;
	case ADV_MOD_TURBO_QPSK:
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/4");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/2");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4");
#ifdef _DEBUG
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0x80");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0xa0");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0xb0");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0xc0");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0xd0");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0xe0");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"0xf0");
#endif _DEBUG
		break;
	case ADV_MOD_TURBO_8PSK:
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"2/3");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4-I");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4-II");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"5/6");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"8/9");
		break;
	case ADV_MOD_TURBO_16QAM:
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4");
		break;
	case ADV_MOD_DCII_C_QPSK:
	case ADV_MOD_DCII_I_QPSK:
	case ADV_MOD_DCII_Q_QPSK:
	case ADV_MOD_DCII_C_OQPSK:
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"5/11");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/2");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/5");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"2/3");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"4/5");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"5/6");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"7/8");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		nAutoFECSelect = 8;
		break;
	case ADV_MOD_DVBS2:
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/4");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/3");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"2/5");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"1/2");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/5");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"2/3");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"3/4");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"4/5");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"5/6");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"8/9");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"9/10");
		SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_ADDSTRING, 0, (LPARAM)"Auto");
		nAutoFECSelect = 11;
		break;
	}
	return nAutoFECSelect;
}

void SourceHelper_CalculateSwitchParameters(int nFrequency, int nPolarity, int nDiSEqCInput, int * nLOF, BOOL * f22KHz, int * nVoltage)
{
	switch(v->sp[nDiSEqCInput].nLNBType)
	{
	case LNB_TYPE_SINGLE:
		*nLOF = v->sp[nDiSEqCInput].nLOFrequencyLow;
		break;
	case LNB_TYPE_DUAL:
		if (nFrequency >= v->sp[nDiSEqCInput].nSwitchFrequency)
			*nLOF = v->sp[nDiSEqCInput].nLOFrequencyHigh;
		else
			*nLOF = v->sp[nDiSEqCInput].nLOFrequencyLow;			
		break;
	case LNB_TYPE_STACKED:
		if (!nPolarity)
			*nLOF = v->sp[nDiSEqCInput].nLOFrequencyLow;
		else
			*nLOF = v->sp[nDiSEqCInput].nLOFrequencyHigh;			
		break;
	}

	switch(v->sp[nDiSEqCInput].n22KHzTone)
	{
	case LNB_22KHZ_BAND:
		if (nFrequency >= v->sp[nDiSEqCInput].nSwitchFrequency)
			*f22KHz = TRUE;
		else
			*f22KHz = FALSE;
		break;
	case LNB_22KHZ_ON:
		*f22KHz = TRUE;
		break;
	case LNB_22KHZ_OFF:
		*f22KHz = FALSE;
		break;
	}

	switch(v->sp[nDiSEqCInput].nVoltage)
	{
	case LNB_VOLTAGE_POLARITY:
		*nVoltage = nPolarity;
		break;
	case LNB_VOLTAGE_BAND:
		if (nFrequency >= v->sp[nDiSEqCInput].nSwitchFrequency)
			*nVoltage = 1;
		else
			*nVoltage = 0;
		break;
	case LNB_VOLTAGE_OFF:
		*nVoltage = -1;
		break;
	case LNB_VOLTAGE_14V:
		*nVoltage = 0;
		break;
	case LNB_VOLTAGE_18V:
		*nVoltage = 1;
		break;
	}


	/*{
		char szTemp[256];
		wsprintf(szTemp, "SourceHelper: CalculateSwitchParameters(nFrequency = %d nPolarity = %d nDiSEqCInput = %d *nLOF = %d, *f22KHz = %d *nVoltage = %d\n",
			     nFrequency, nPolarity, nDiSEqCInput, *nLOF, *f22KHz, *nVoltage);
		OutputDebugString(szTemp);
	}*/

}

void SetupAutoSwitchParameters(HWND hDlg, BOOL fFromFrequencyChange)
{
	int nCurrentSwitch = -1;
	int nLOF = 0;
	BOOL f22KHz = FALSE;
	int nPolarity = 0;
	int nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
	int nVoltage;
	int nLBand;

	if (fFromFrequencyChange)
	{
		int i;

		for (i = 0; i < MAX_SWITCH_PARAMETERS; i++)
		{
			switch(v->sp[i].nAutoSelect)
			{
			case AUTO_SELECT_LNB_FREQ:
				if (nFrequency >= v->sp[i].nAutoSelectFreqStart && nFrequency <= v->sp[i].nAutoSelectFreqEnd)
				{
					int j;
					for (j = 0; j < 6; j++)
						CheckDlgButton(hDlg, nSwitchButtons[j], FALSE);
					CheckDlgButton(hDlg, IDC_INPUT_NONE, FALSE);
					CheckDlgButton(hDlg, IDC_INPUT_DISH, FALSE);
					CheckDlgButton(hDlg, nSwitchButtons[i], TRUE);
					break;
				}
				break;
			case AUTO_SELECT_LNB_ORBITAL:
			case AUTO_SELECT_LNB_NETWORK:
				break;		// not really applicable here!
			}
		}
	}

	if (IsDlgButtonChecked(hDlg, IDC_DISEQC1))
		nCurrentSwitch = 0;
	else if (IsDlgButtonChecked(hDlg, IDC_DISEQC2))
		nCurrentSwitch = 1;
	else if (IsDlgButtonChecked(hDlg, IDC_DISEQC3))
		nCurrentSwitch = 2;
	else if (IsDlgButtonChecked(hDlg, IDC_DISEQC4))
		nCurrentSwitch = 3;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_TB_A))
		nCurrentSwitch = 4;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_TB_B))
		nCurrentSwitch = 5;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_DISH))
	{
		int nItem = SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_GETCURSEL, 0, 0);
		nCurrentSwitch = 6 + nItem;
	}
	if (nCurrentSwitch == -1)
		return;

	if (IsDlgButtonChecked(hDlg, IDC_HORZ))
		nPolarity = 1;
	SourceHelper_CalculateSwitchParameters(nFrequency, nPolarity, nCurrentSwitch, &nLOF, &f22KHz, &nVoltage);
	v->ss.nPolarity = nVoltage;

	if (nLOF == 0)
		SetDlgItemText(hDlg, IDC_LOF, "zero");
	else
		SetDlgItemInt(hDlg, IDC_LOF, nLOF, FALSE);
	CheckDlgButton(hDlg, IDC_22KHZ, f22KHz);

	if (nFrequency > nLOF)
		nLBand = nFrequency - nLOF;
	else
		nLBand = nLOF - nFrequency;
}

void UpdateSatelliteMux(HWND hDlg, int nMuxIndex)
{
	if (v->fBlockFirstMuxListSelection == TRUE)
	{
		v->fBlockFirstMuxListSelection = FALSE;
		return;
	}

	if (v->nInitialMuxIndex != -1)
	{
		LV_ITEM lvi; 

		lvi.mask = LVIF_STATE;
		lvi.iItem = v->nInitialMuxIndex;
		lvi.iSubItem = 0;
		lvi.stateMask = LVIS_DROPHILITED;
		lvi.state = 0;
		ListView_SetItem(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST), &lvi);
		v->nInitialMuxIndex = -1;
	}

	SetDlgItemInt(hDlg, IDC_FREQUENCY, v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nFrequency, FALSE);
	SetDlgItemInt(hDlg, IDC_SR, v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nSymbolRate, FALSE);
	if (v->nPolarity != -1)
	{
		if (lstrcmp(v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].szPolarity, "H") == 0 ||
			lstrcmp(v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].szPolarity, "L") == 0)
		{
			CheckDlgButton(hDlg, IDC_HORZ, BST_CHECKED);
			CheckDlgButton(hDlg, IDC_VERT, BST_UNCHECKED);
		}
		else
		{
			CheckDlgButton(hDlg, IDC_HORZ, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_VERT, BST_CHECKED);
		}
	}
	if (v->fADVModulation)
	{
		SendDlgItemMessage(hDlg, IDC_MODULATION, CB_SETCURSEL, (WPARAM)v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nModulationMode, 0);
		SetupCodeRateOptions(hDlg, v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nModulationMode);
		if (v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nCodeRate == -1)
		{
			int nCount = SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_GETCOUNT, 0, 0);
			SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_SETCURSEL, (WPARAM)nCount - 1, 0);
		}
		else
			SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_SETCURSEL, (WPARAM)v->sats[v->nCurrentSelectedSatellite].mux[nMuxIndex].nCodeRate, 0);
	}
	SetupAutoSwitchParameters(hDlg, FALSE);
	v->nCurrentlySelectedMux = nMuxIndex;
}

void UpdateMuxTitle(HWND hDlg, char * szFilename)
{
	char szTemp[128];

	wsprintf(szTemp, "Muxes on %s", v->szCurrentlySelectedSatellite);
	if (szFilename != NULL)
	{
		lstrcat(szTemp, " - ");
		lstrcat(szTemp, szFilename);
	}
	SetDlgItemText(hDlg, IDC_TUNER_MUX_TITLE, szTemp);

}

void WriteSatelliteLNBFAndSwitch(HWND hDlg, int nSatellite)
{
	int nDiSEqCInput = 0;
	char szCurrentDir[MAX_PATH];
	char szCurrentINIFile[MAX_PATH];
	char szLOF[32];
	char szDiSEqCInput[32];

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	wsprintf(szCurrentINIFile, "%s\\Satellites\\%s", szCurrentDir, v->sats[nSatellite].szSourceFilename);
	GetDlgItemText(hDlg, IDC_LOF, szLOF, sizeof(szLOF));
	WritePrivateProfileString("TSREADER", "LOF", szLOF, szCurrentINIFile);
	if (IsDlgButtonChecked(hDlg, IDC_DISEQC1))
		nDiSEqCInput = 1;
	else if (IsDlgButtonChecked(hDlg, IDC_DISEQC2))
		nDiSEqCInput = 2;
	else if (IsDlgButtonChecked(hDlg, IDC_DISEQC3))
		nDiSEqCInput = 3;
	else if (IsDlgButtonChecked(hDlg, IDC_DISEQC4))
		nDiSEqCInput = 4;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_TB_A))
		nDiSEqCInput = 5;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_TB_B))
		nDiSEqCInput = 6;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_NONE))
		nDiSEqCInput = 0;
	else if (IsDlgButtonChecked(hDlg, IDC_INPUT_DISH))
		nDiSEqCInput = SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_GETCURSEL, 0, 0) + 7;
	wsprintf(szDiSEqCInput, "%d", nDiSEqCInput);
	WritePrivateProfileString("TSREADER", "DISEQC", szDiSEqCInput, szCurrentINIFile);
}

void ReadSatelliteLNBFAndSwitch(HWND hDlg, int nSatellite)
{
	char szCurrentDir[MAX_PATH];
	char szCurrentINIFile[MAX_PATH];
	char szLOF[32];
	char szDiSEqCInput[32];

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	wsprintf(szCurrentINIFile, "%s\\Satellites\\%s", szCurrentDir, v->sats[nSatellite].szSourceFilename);
	GetPrivateProfileString("TSREADER", "LOF", "", szLOF, sizeof(szLOF), szCurrentINIFile);
	GetPrivateProfileString("TSREADER", "DISEQC", "", szDiSEqCInput, sizeof(szDiSEqCInput), szCurrentINIFile);
	if (lstrlen(szLOF) && lstrlen(szDiSEqCInput))
	{
		int nDiSEqCInput = atoi(szDiSEqCInput);
		SetDlgItemText(hDlg, IDC_LOF, szLOF);

		CheckDlgButton(hDlg, IDC_INPUT_NONE, FALSE);
		CheckDlgButton(hDlg, IDC_DISEQC1, FALSE);
		CheckDlgButton(hDlg, IDC_DISEQC2, FALSE);
		CheckDlgButton(hDlg, IDC_DISEQC3, FALSE);
		CheckDlgButton(hDlg, IDC_DISEQC4, FALSE);
		CheckDlgButton(hDlg, IDC_INPUT_TB_A, FALSE);
		CheckDlgButton(hDlg, IDC_INPUT_TB_B, FALSE);
		CheckDlgButton(hDlg, IDC_INPUT_DISH, FALSE);

		switch(nDiSEqCInput)
		{
		case 0:
			CheckDlgButton(hDlg, IDC_INPUT_NONE, TRUE);
			break;
		case 1:
			CheckDlgButton(hDlg, IDC_DISEQC1, TRUE);
			break;
		case 2:
			CheckDlgButton(hDlg, IDC_DISEQC2, TRUE);
			break;
		case 3:
			CheckDlgButton(hDlg, IDC_DISEQC3, TRUE);
			break;
		case 4:
			CheckDlgButton(hDlg, IDC_DISEQC4, TRUE);
			break;
		case 5:
			CheckDlgButton(hDlg, IDC_INPUT_TB_A, TRUE);
			break;
		case 6:
			CheckDlgButton(hDlg, IDC_INPUT_TB_B, TRUE);
			break;
		default:
			if (nDiSEqCInput >= 7 && nDiSEqCInput <= 20)
			{
				// Dish switch
				CheckDlgButton(hDlg, IDC_INPUT_DISH, TRUE);
				SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_SETCURSEL, nDiSEqCInput - 7, 0);
			}
			break;
		}
	}
}

void UpdateMuxList(HWND hDlg, int nSatelliteIndex)
{
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_TUNER_MUX_LIST);
	int i;

	if (v->nInitialSatelliteIndex != -1 && v->fBlockFirstSatelliteListSelection == FALSE)
	{
		LV_ITEM lvi; 

		lvi.mask = LVIF_STATE;
		lvi.iItem = v->nInitialSatelliteIndex;
		lvi.iSubItem = 0;
		lvi.stateMask = LVIS_DROPHILITED;
		lvi.state = 0;
		ListView_SetItem(GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST), &lvi);
		v->nInitialSatelliteIndex = -1;
	}

	if (v->nCurrentSelectedSatellite != -1)
	{
		// Save switch and LNB frequency for this satellite
		WriteSatelliteLNBFAndSwitch(hDlg, v->nCurrentSelectedSatellite);
	}

	v->nCurrentSelectedSatellite = nSatelliteIndex;
	ReadSatelliteLNBFAndSwitch(hDlg, v->nCurrentSelectedSatellite);
	lstrcpy(v->szCurrentlySelectedSatellite, v->sats[nSatelliteIndex].szName);
	lstrcpy(szCurrentSatFile, v->sats[nSatelliteIndex].szSourceFilename);
	ListView_DeleteAllItems(hWndMuxList);
	UpdateMuxTitle(hDlg, NULL);

	for (i = 0; i < v->sats[nSatelliteIndex].nMuxCount; i++)
	{
		{
			LV_ITEM lvi; 

			if (v->fADVModulation == FALSE && v->sats[nSatelliteIndex].mux[i].fADVModulation == TRUE)
				continue;

			// Initialize item-specific LV_ITEM members. 
			memset(&lvi, 0, sizeof(lvi));
			lvi.state = 0; 
			lvi.stateMask = 0; 
			lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
			lvi.mask = LVIF_TEXT | LVIF_STATE; 
			lvi.iItem = i; 
			lvi.iSubItem = 0; 
			lvi.lParam = (LPARAM)0;    // item data 

			// Add the item. 
			ListView_InsertItem(hWndMuxList, &lvi);
		}
	}
	v->fBlockFirstSatelliteListSelection = FALSE;
}

void SetupCableListView(HWND hDlg)
{
	int nColumnPosition = 0;
	int nIndex;
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_CABLE_LIST);
	char szTemp[100];
	LV_COLUMN lvc; 
	LV_ITEM lvi; 

	// Initialize the LV_COLUMN structure. 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	lvc.pszText = szTemp; 

	// Satellite list first. 
	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 75; 
	lstrcpy(szTemp, TEXT("Frequency"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 75; 
	lstrcpy(szTemp, TEXT("Symbol Rate"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 70; 
	lstrcpy(szTemp, TEXT("QAM"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 60; 
	lstrcpy(szTemp, TEXT("Inversion"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 70; 
	lstrcpy(szTemp, TEXT("Bandwidth"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	ListView_SetExtendedListViewStyle(hWndMuxList, LVS_EX_FULLROWSELECT);

	// Populate mux list
	for (nIndex = 0; nIndex < v->nCableMuxes; nIndex++)
	{
		// Initialize item-specific LV_ITEM members. 
		memset(&lvi, 0, sizeof(lvi));
		lvi.state = 0; 
		lvi.stateMask = 0; 
		lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
		lvi.iItem = nIndex; 
		lvi.iSubItem = 0; 
		lvi.lParam = (LPARAM) 0;    // item data 
		ListView_InsertItem(hWndMuxList, &lvi);
	}
}

void SetupTerrestrialListView(HWND hDlg, BOOL fInit)
{
	int nColumnPosition = 0;
	int nIndex;
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_TERRESTRIAL_LIST);
	char szTemp[100];
	LV_COLUMN lvc; 
	LV_ITEM lvi; 

	if (fInit)
	{
		// Initialize the LV_COLUMN structure. 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.pszText = szTemp; 

		// Satellite list first. 
		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 55; 
		lstrcpy(szTemp, TEXT("Freq."));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 40; 
		lstrcpy(szTemp, TEXT("Inv."));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 50; 
		lstrcpy(szTemp, TEXT("B/W"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		lvc.fmt = LVCFMT_LEFT; 
		lvc.cx = 165; 
		lstrcpy(szTemp, TEXT("Description"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		ListView_SetExtendedListViewStyle(hWndMuxList, LVS_EX_FULLROWSELECT);

	}

	// Populate mux list
	ListView_DeleteAllItems(hWndMuxList);
	for (nIndex = 0; nIndex < v->nTerrestrialMuxes; nIndex++)
	{
		// Initialize item-specific LV_ITEM members. 
		memset(&lvi, 0, sizeof(lvi));
		lvi.state = 0; 
		lvi.stateMask = 0; 
		lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
		lvi.iItem = nIndex; 
		lvi.iSubItem = 0; 
		lvi.lParam = (LPARAM) 0;    // item data 
		ListView_InsertItem(hWndMuxList, &lvi);
	}
}

void SetupATSCListView(HWND hDlg, BOOL fInit)
{
	int nColumnPosition = 0;
	int nIndex;
	int nMuxIndex = 0;
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_ATSC_LIST);
	char szTemp[100];
	LV_COLUMN lvc; 
	LV_ITEM lvi; 

	if (fInit)
	{
		// Initialize the LV_COLUMN structure. 
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
		lvc.pszText = szTemp; 

		// Satellite list first. 
		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 60; 
		lstrcpy(szTemp, TEXT("Channel"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 70; 
		lstrcpy(szTemp, TEXT("Frequency"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		lvc.fmt = LVCFMT_LEFT; 
		lvc.cx = 130; 
		lstrcpy(szTemp, TEXT("Description"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

		ListView_SetExtendedListViewStyle(hWndMuxList, LVS_EX_FULLROWSELECT);
	}

	// Populate mux list
	ListView_DeleteAllItems(hWndMuxList);
	for (nIndex = 0; nIndex < v->nATSCMuxes; nIndex++)
	{
		// Initialize item-specific LV_ITEM members. 
		memset(&lvi, 0, sizeof(lvi));
		lvi.state = 0; 
		lvi.stateMask = 0; 
		lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
		lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
		lvi.iItem = nIndex; 
		lvi.iSubItem = 0; 
		lvi.lParam = (LPARAM) 0;    // item data 
		ListView_InsertItem(hWndMuxList, &lvi);
	}

	// Select the current mux if there's a match
	for (nIndex = 0; nIndex < v->nATSCMuxes; nIndex++)
	{
		if (v->amuxes[nIndex].nFrequency == v->ss.nFrequency)
		{
			nMuxIndex = nIndex;
			break;
		}
	}
	lvi.mask = LVIF_STATE;
	lvi.iItem = nMuxIndex;
	lvi.iSubItem = 0;
	lvi.stateMask = LVIS_SELECTED;
	lvi.state = LVIS_SELECTED;
	ListView_SetItem(hWndMuxList, &lvi);
	ListView_EnsureVisible(hWndMuxList, nMuxIndex, FALSE);

}

// A **VERY** crude routine to return a line!
int SourceHelper_ReadLine(HANDLE hFile, char * szBuffer, int nMaxLength)
{
	int dwBytesRead;
	char szTemp[2];
	int nOutputPosition = 0;

	do
	{
		ReadFile(hFile, szTemp, 1, &dwBytesRead, NULL);
		if (dwBytesRead != 1)
			return nOutputPosition;
		if (szTemp[0] == 0x0d)
		{
			szBuffer[nOutputPosition] = '\0';
			return nOutputPosition;
		}
		if (szTemp[0] != 0x0a)
			szBuffer[nOutputPosition++] = szTemp[0];
		if (nOutputPosition == nMaxLength - 2)
		{
			szBuffer[nOutputPosition] = '\0';
			return nOutputPosition;
		}
	} while (TRUE);
}

// A **VERY** crude routine to return a line!
int SourceHelper_ReadLineUnix(HANDLE hFile, char * szBuffer, int nMaxLength)
{
	int dwBytesRead;
	char szTemp[2];
	int nOutputPosition = 0;

	do
	{
		ReadFile(hFile, szTemp, 1, &dwBytesRead, NULL);
		if (dwBytesRead != 1)
			return nOutputPosition;
		if (szTemp[0] == 0x0a)
		{
			szBuffer[nOutputPosition] = '\0';
			return nOutputPosition;
		}
		if (szTemp[0] != 0x0d)
			szBuffer[nOutputPosition++] = szTemp[0];
		if (nOutputPosition == nMaxLength - 2)
		{
			szBuffer[nOutputPosition] = '\0';
			return nOutputPosition;
		}
	} while (TRUE);
}

int __cdecl SortCableCompareFunction(const void *elem1, const void *elem2)
{
	PCABLEMUX pMUX1 = (PCABLEMUX)elem1;
	PCABLEMUX pMUX2 = (PCABLEMUX)elem2;

	if (pMUX1->nFrequency < pMUX2->nFrequency)
		return -1;
	if (pMUX1->nFrequency > pMUX2->nFrequency)
		return 1;
	return 0;
}

int __cdecl SortTerresrialCompareFunction(const void *elem1, const void *elem2)
{
	PTERRESTRIALMUX pMUX1 = (PTERRESTRIALMUX)elem1;
	PTERRESTRIALMUX pMUX2 = (PTERRESTRIALMUX)elem2;

	if (pMUX1->nFrequency < pMUX2->nFrequency)
		return -1;
	if (pMUX1->nFrequency > pMUX2->nFrequency)
		return 1;
	return 0;
}

int __cdecl SortATSCCompareFunction(const void *elem1, const void *elem2)
{
	PATSCMUX pMUX1 = (PATSCMUX)elem1;
	PATSCMUX pMUX2 = (PATSCMUX)elem2;

	if (pMUX1->nFrequency < pMUX2->nFrequency)
		return -1;
	if (pMUX1->nFrequency > pMUX2->nFrequency)
		return 1;
	return 0;
}

void LoadATSCQAMFile()
{
	int i;
	HANDLE hList;
    char szListFile[MAX_PATH];

	v->nATSCMuxes = 0;

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
	if (v->fQAMMode)
		lstrcat(szListFile, "\\QAM.lst");
	else
		lstrcat(szListFile, "\\ATSC.lst");

	hList = CreateFile(szListFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hList != INVALID_HANDLE_VALUE)
	{
		char szLine[256];
		char szCommaValue[32][100];

		while (SourceHelper_ReadLine(hList, szLine, sizeof(szLine)))
		{
			char * szCurrent = szLine;
			int nItem = 0;
			int nFrequency;
			char * szNextTab;

			do
			{
				szNextTab = strstr(szCurrent, "\t");
				if (szNextTab != NULL)
					*szNextTab = '\0';
				lstrcpy(szCommaValue[nItem++], szCurrent);
				szCurrent = szNextTab + 1;
			} while (szNextTab != NULL);

			if (nItem)
			{
				if (sscanf(szCommaValue[0], "%d", &nFrequency) == 1)
				{
					for (i = 0; i < v->nATSCMuxes; i++)
					{
						if (nFrequency == v->amuxes[i].nFrequency)
							break;
					}
					if (i == v->nATSCMuxes)
					{
						v->amuxes[v->nATSCMuxes].nFrequency = nFrequency;
						lstrcpy(v->amuxes[v->nATSCMuxes].szDescription, szCommaValue[1]);
						v->nATSCMuxes++;
					}
				}
			}
		}
		CloseHandle(hList);

		if (v->nATSCMuxes)
			qsort(v->amuxes, v->nATSCMuxes, sizeof(ATSCMUX), SortATSCCompareFunction);
	}
}

void LoadCableFile()
{
	int i;
	HANDLE hList;
    char szListFile[MAX_PATH];

	v->nCableMuxes = 0;
	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
	lstrcat(szListFile, "\\DigitalTV-c.lst");
	hList = CreateFile(szListFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hList != INVALID_HANDLE_VALUE)
	{
		char szLine[256];
		char szCommaValue[32][100];

		while (SourceHelper_ReadLine(hList, szLine, sizeof(szLine)))
		{
			char * szCurrent = szLine;
			int nItem = 0;
			int nFrequency;

			do
			{
				char * szNextTab = strstr(szCurrent, "\t");
				if (szNextTab == NULL)
					break;
				*szNextTab = '\0';
				lstrcpy(szCommaValue[nItem++], szCurrent);
				szCurrent = szNextTab + 1;
			} while (TRUE);

			if (nItem)
			{
				if (sscanf(szCommaValue[4], "%d", &nFrequency) == 1)
				{
					for (i = 0; i < v->nCableMuxes; i++)
					{
						if (nFrequency == v->cmuxes[i].nFrequency)
							break;
					}
					if (i == v->nCableMuxes)
					{
						int fSpectrumInversion;

						v->cmuxes[v->nCableMuxes].nFrequency = nFrequency;
						sscanf(szCommaValue[5], "%d", &v->cmuxes[v->nCableMuxes].nQAM);
						sscanf(szCommaValue[6], "%d", &v->cmuxes[v->nCableMuxes].nSymbolRate); v->cmuxes[v->nCableMuxes].nSymbolRate /= 1000;
						sscanf(szCommaValue[7], "%d", &fSpectrumInversion);
						v->cmuxes[v->nCableMuxes].fSpectrumInversion = (~fSpectrumInversion) & 1;
						sscanf(szCommaValue[8], "%d", &v->cmuxes[v->nCableMuxes].nBandwidth);
						v->nCableMuxes++;
					}
				}
			}
		}

		CloseHandle(hList);

		if (v->nCableMuxes)
			qsort(v->cmuxes, v->nCableMuxes, sizeof(CABLEMUX), SortCableCompareFunction);
	}
}

void SaveDVBTFile()
{
	if (v->nTerrestrialMuxes)
	{
		int i;
		HANDLE hList;
		char szListFile[MAX_PATH];

		SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
		lstrcat(szListFile, "\\DVBT.lst");
		hList = CreateFile(szListFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
		if (hList != INVALID_HANDLE_VALUE)
		{
			DWORD dwWritten;
			char szLine[128];

			for (i = 0; i < v->nTerrestrialMuxes; i++)
			{
				wsprintf(szLine, "%d\t%d\t%d\t%s\r\n", v->tmuxes[i].nFrequency, v->tmuxes[i].nBandwidth, v->tmuxes[i].fSpectrumInversion, v->tmuxes[i].szDescription);
				WriteFile(hList, szLine, lstrlen(szLine), &dwWritten, NULL);
			}
			CloseHandle(hList);
		}
	}
}

void LoadDVBTFile()
{
	int i;
	HANDLE hList;
    char szListFile[MAX_PATH];

	v->nTerrestrialMuxes = 0;

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
	lstrcat(szListFile, "\\DVBT.lst");

	hList = CreateFile(szListFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hList != INVALID_HANDLE_VALUE)
	{
		BOOL fNeedToReWriteFile = FALSE;
		char szLine[256];
		char szCommaValue[32][100];

		while (SourceHelper_ReadLine(hList, szLine, sizeof(szLine)))
		{
			char * szCurrent = szLine;
			int nItem = 0;
			int nFrequency;
			char * szNextTab;

			do
			{
				szNextTab = strstr(szCurrent, "\t");
				if (szNextTab != NULL)
					*szNextTab = '\0';
				lstrcpy(szCommaValue[nItem++], szCurrent);
				szCurrent = szNextTab + 1;
			} while (szNextTab != NULL);

			if (nItem)
			{
				if (sscanf(szCommaValue[0], "%d", &nFrequency) == 1)
				{
					// Upgrade to KHz if old file
					if (nFrequency < 10000)
					{
						nFrequency *= 100;
						fNeedToReWriteFile = TRUE;
					}
					for (i = 0; i < v->nTerrestrialMuxes; i++)
					{
						if (nFrequency == v->tmuxes[i].nFrequency)
							break;
					}
					if (i == v->nTerrestrialMuxes)
					{
						
						v->tmuxes[v->nTerrestrialMuxes].nFrequency = nFrequency;
						sscanf(szCommaValue[1], "%d", &v->tmuxes[v->nTerrestrialMuxes].nBandwidth);
						sscanf(szCommaValue[2], "%d", &v->tmuxes[v->nTerrestrialMuxes].fSpectrumInversion);
						lstrcpy(v->tmuxes[v->nTerrestrialMuxes].szDescription, szCommaValue[3]);
						v->nTerrestrialMuxes++;
					}
				}
			}
		}
		CloseHandle(hList);

		if (v->nTerrestrialMuxes)
			qsort(v->tmuxes, v->nTerrestrialMuxes, sizeof(TERRESTRIALMUX), SortTerresrialCompareFunction);

		if (fNeedToReWriteFile)
			SaveDVBTFile();
	}
}

void GetCableDispInfo(LV_DISPINFO *pnmv) 
{
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nMuxIndex = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			sprintf(pnmv->item.pszText, "%.3f", (double)v->cmuxes[nMuxIndex].nFrequency / 1000.0);
			break;
		case 1:
			wsprintf(pnmv->item.pszText, "%d", v->cmuxes[nMuxIndex].nSymbolRate);
			break;
		case 2:
			switch(v->cmuxes[nMuxIndex].nQAM)
			{
			case 0:
				lstrcpy(pnmv->item.pszText, "QAM-16");
				break;
			case 1:
				lstrcpy(pnmv->item.pszText, "QAM-32");
				break;
			case 2:
				lstrcpy(pnmv->item.pszText, "QAM-64");
				break;
			case 3:
				lstrcpy(pnmv->item.pszText, "QAM-128");
				break;
			case 4:
				lstrcpy(pnmv->item.pszText, "QAM-256");
				break;
			}
			break;
		case 3:
			if (v->cmuxes[nMuxIndex].fSpectrumInversion)
				lstrcpy(pnmv->item.pszText, "Yes");
			else
				lstrcpy(pnmv->item.pszText, "No");
			break;
		case 4:
			switch(v->cmuxes[nMuxIndex].nBandwidth)
			{
			case 0:
				lstrcpy(pnmv->item.pszText, "6 MHz");
				break;
			case 1:
				lstrcpy(pnmv->item.pszText, "7 MHz");
				break;
			case 2:
				lstrcpy(pnmv->item.pszText, "8 MHz");
				break;
			}
			break;
		}
	}
}

void GetTerrestrialDispInfo(LV_DISPINFO *pnmv) 
{
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nMuxIndex = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			sprintf(pnmv->item.pszText, "%.3f", (double)v->tmuxes[nMuxIndex].nFrequency / 1000.0);
			break;
		case 1:
			if (v->tmuxes[nMuxIndex].fSpectrumInversion)
				lstrcpy(pnmv->item.pszText, "Yes");
			else
				lstrcpy(pnmv->item.pszText, "No");
			break;
		case 2:
			switch(v->tmuxes[nMuxIndex].nBandwidth)
			{
			case 0:
				lstrcpy(pnmv->item.pszText, "6 MHz");
				break;
			case 1:
				lstrcpy(pnmv->item.pszText, "7 MHz");
				break;
			case 2:
				lstrcpy(pnmv->item.pszText, "8 MHz");
				break;
			}
			break;
		case 3:
			lstrcpy(pnmv->item.pszText, v->tmuxes[nMuxIndex].szDescription);
			break;
		}
	}
}

int SourceHelper_GetQAMChannelFromFrequency(int nFrequency)
{
	int nChannel;

	if (v->fHRCQAM == TRUE)
		nFrequency++;

	for (nChannel = 0; nChannel <= sizeof(nIRCChannelTable) / sizeof(nIRCChannelTable[0]); nChannel++)
	{
		if (nFrequency == nIRCChannelTable[nChannel])
			return nChannel + 1;
	}

	return 0;
}

BOOL SourceHelper_GetQAMHRCStatus()
{
	return v->fHRCQAM;
}


int SourceHelper_GetATSCChannelFromFrequency(int nFrequency)
{
	int nChannel = 0;

	if (nFrequency >= bVHFLowBandOffAirFrequencies[0] &&
		nFrequency <= bVHFLowBandOffAirFrequencies[4])
	{
		// VHF low band
		int i;
		for (i = 0; i < sizeof(bVHFLowBandOffAirFrequencies); i++)
		{
			if (bVHFLowBandOffAirFrequencies[i] == nFrequency)
				return i + 2;
		}
	}
	else if (nFrequency >= 177 && nFrequency <= 213)
	{
		// VHF high band
		nFrequency -= 177;
		nFrequency /= 6;
		nChannel = nFrequency + 7;
	}
	else if (nFrequency >= 473 && nFrequency <= 803)
	{
		// UHF band
		nFrequency -= 473;
		nFrequency /= 6;
		nChannel = nFrequency + 14;
	}
	
	return nChannel;
}

void GetATSCDispInfo(LV_DISPINFO *pnmv) 
{
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nMuxIndex = (int)(pnmv->item.iItem);
		switch(pnmv->item.iSubItem)
		{
		case 0:
			if (v->fQAMMode == FALSE)
				wsprintf(pnmv->item.pszText, "%d", SourceHelper_GetATSCChannelFromFrequency(v->amuxes[nMuxIndex].nFrequency));
			else
				wsprintf(pnmv->item.pszText, "%d", SourceHelper_GetQAMChannelFromFrequency(v->amuxes[nMuxIndex].nFrequency));
			break;
		case 1:
			wsprintf(pnmv->item.pszText, "%d", v->amuxes[nMuxIndex].nFrequency);
			break;
		case 2:
			lstrcpy(pnmv->item.pszText, v->amuxes[nMuxIndex].szDescription);
			break;
		}
	}
}

void UpdateATSCMux(HWND hDlg, int nMuxItem)
{
	if (v->fBlockFirstMuxListSelection == TRUE)
	{
		v->fBlockFirstMuxListSelection = FALSE;
		return;
	}

	SetDlgItemInt(hDlg, IDC_FREQUENCY, v->amuxes[nMuxItem].nFrequency, FALSE);
}

void UpdateTerrestrialMux(HWND hDlg, int nMuxItem)
{
	char szTemp[100];

	if (v->fBlockFirstMuxListSelection == TRUE)
	{
		v->fBlockFirstMuxListSelection = FALSE;
		//return;
	}

	sprintf(szTemp, "%.3f", (double)v->tmuxes[nMuxItem].nFrequency / 1000.0);
	SetDlgItemText(hDlg, IDC_FREQUENCY, szTemp);
	CheckDlgButton(hDlg, IDC_SPECTRUM_INVERSE, (v->tmuxes[nMuxItem].fSpectrumInversion == 1));
	CheckDlgButton(hDlg, IDC_SPECTRUM_NORMAL, (v->tmuxes[nMuxItem].fSpectrumInversion == 0));
	CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, (v->tmuxes[nMuxItem].nBandwidth == 0));
	CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, (v->tmuxes[nMuxItem].nBandwidth == 1));
	CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, (v->tmuxes[nMuxItem].nBandwidth == 2));
}

void UpdateCableMux(HWND hDlg, int nMuxItem)
{
	char szTemp[100];

	if (v->fBlockFirstMuxListSelection == TRUE)
	{
		v->fBlockFirstMuxListSelection = FALSE;
		return;
	}

	sprintf(szTemp, "%.3f", (double)v->cmuxes[nMuxItem].nFrequency / 1000.0);
	SetDlgItemText(hDlg, IDC_FREQUENCY, szTemp);
	SetDlgItemInt(hDlg, IDC_SR, v->cmuxes[nMuxItem].nSymbolRate, FALSE);
	SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_SETCURSEL, (WPARAM)v->cmuxes[nMuxItem].nQAM, 0);
	CheckDlgButton(hDlg, IDC_SPECTRUM_INVERSE, (v->cmuxes[nMuxItem].fSpectrumInversion == 1));
	CheckDlgButton(hDlg, IDC_SPECTRUM_NORMAL, (v->cmuxes[nMuxItem].fSpectrumInversion == 0));
	CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, (v->cmuxes[nMuxItem].nBandwidth == 0));
	CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, (v->cmuxes[nMuxItem].nBandwidth == 1));
	CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, (v->cmuxes[nMuxItem].nBandwidth == 2));
}

BOOL CALLBACK TuneDVBCableDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTemp[128];

			if (fTuneDialogFirstTime == TRUE)
			{
				fTuneDialogFirstTime = FALSE;
				LoadCableFile();
			}
			v->fBlockFirstMuxListSelection = FALSE;

			wsprintf(szTemp, "Tune %s", v->szSourceModuleDescription);
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)szTemp);

			SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_ADDSTRING, 0, (LPARAM)"QAM-16");
			SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_ADDSTRING, 0, (LPARAM)"QAM-32");
			SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_ADDSTRING, 0, (LPARAM)"QAM-64");
			SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_ADDSTRING, 0, (LPARAM)"QAM-128");
			SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_ADDSTRING, 0, (LPARAM)"QAM-256");
			
			sprintf(szTemp, "%.3f", (double)v->ss.nFrequency / 1000.0);
			SetDlgItemText(hDlg, IDC_FREQUENCY, szTemp);
			SetDlgItemInt(hDlg, IDC_SR, v->ss.nSymbolRate, FALSE);
			if (v->ss.fSpectrumInversion)
				CheckDlgButton(hDlg, IDC_SPECTRUM_INVERSE, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_SPECTRUM_NORMAL, BST_CHECKED);
			switch(v->ss.nBandwidth)
			{
			case 0:
				CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, BST_CHECKED);
				break;
			case 1:
				CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, BST_CHECKED);
				break;
			case 2:
				CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, BST_CHECKED);
				break;
			}
			SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_SETCURSEL, (WPARAM)v->ss.nQAM, 0);

			if (v->nCableMuxes)
				SetupCableListView(hDlg);
			else
			{
				RECT rcDialog;
				RECT rcSeperator;
				int nNewX, nNewY, nDifference;

				GetWindowRect(hDlg, &rcDialog);
				GetWindowRect(GetDlgItem(hDlg, IDC_SEPERATOR), &rcSeperator);

				nNewX = rcDialog.right - rcDialog.left;
				nNewY = rcDialog.bottom - rcDialog.top;
				nDifference = rcDialog.bottom - rcSeperator.bottom;
				nNewY -= nDifference;
				SetWindowPos(hDlg, HWND_TOP, 0, 0, nNewX, nNewY, SWP_NOMOVE);
			}
			SendDlgItemMessage(hDlg, IDC_FREQUENCY, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hDlg, IDC_FREQUENCY));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				double dFrequency;
				char szTemp[128];
				char szFreq[128] = {0};

				GetDlgItemText(hDlg, IDC_FREQUENCY, szTemp, sizeof(szTemp));
				dFrequency = atof(szTemp);
				dFrequency *= 1000.0;
				v->ss.nFrequency = (int)dFrequency;
				
				if (IsDlgButtonChecked(hDlg, IDC_SPECTRUM_INVERSE))
					v->ss.fSpectrumInversion = TRUE;
				else
					v->ss.fSpectrumInversion = FALSE;
				if (IsDlgButtonChecked(hDlg, IDC_BANDWIDTH_6MHz))
					v->ss.nBandwidth = 0;
				else if (IsDlgButtonChecked(hDlg, IDC_BANDWIDTH_7MHz))
					v->ss.nBandwidth = 1;
				else if (IsDlgButtonChecked(hDlg, IDC_BANDWIDTH_8MHz))
					v->ss.nBandwidth = 2;

				v->ss.nSymbolRate = GetDlgItemInt(hDlg, IDC_SR, NULL, FALSE);
				v->ss.nQAM = SendDlgItemMessage(hDlg, IDC_QAM_MODE, CB_GETCURSEL, 0, 0);

				EndDialog(hDlg, TRUE);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
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
				GetCableDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
					UpdateCableMux(hDlg, pnmv->iItem);
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

void SetupDVBTBandplanCombo(HWND hDlg)
{
	HWND hCombo = GetDlgItem(hDlg, IDC_DVBT_BANDPLAN);
	int i;
	int nSelectIndex = -1;

	for (i = 0; lstrlen(szDVBTBandplanNames[i]); i++)
	{
		int nIndex = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)szDVBTBandplanNames[i]);
		if (lstrcmp(szDVBTBandplanNames[i], v->szDVBTBandplan) == 0)
			nSelectIndex = nIndex;
	}

	if (nSelectIndex != -1)
		SendMessage(hCombo, CB_SETCURSEL, nSelectIndex, 0);
}

DWORD WINAPI ScanDVBTThread(LPVOID lpv)
{
	BOOL (* Tune) ();
	HWND hDlg = (HWND)lpv;
	int nBandplanTable = -1;
	int nIndex;
	int nPriorFrequency = v->ss.nFrequency;
	int nBandplanChannelCount, nBandplanEnd, nBandplanOffset;
	int nBandplanIndex;
	int nFrequencyOffset = 0;

	// First find the bandplan offset by comparing bandplan names
	for (nIndex = 0; lstrlen(szDVBTBandplanNames[nIndex]); nIndex++)
	{
		if (lstrcmp(szDVBTBandplanNames[nIndex], v->szDVBTBandplan) == 0)
		{
			nBandplanTable = nIndex;
			break;
		}
	}
	if (nBandplanTable == -1)
	{
		OutputDebugString("SourceHelper: Unable to locate bandplan table 1\n");
		goto ScanDVBTThread_Wrapup;
	}

	// Now find the appropriate offset into the bandplan table
	nIndex = 0;
	nBandplanOffset = -1;
	nBandplanIndex = 0;
	do
	{
		if (nBandplanIndex == nBandplanTable)
		{
			nBandplanOffset = nIndex;
			break;
		}
		while (nDVBTBandplans[nIndex++] != 0)
			;
		nBandplanIndex++;
	} while (nDVBTBandplans[nIndex] != 0);
	if (nBandplanOffset == -1)
	{
		OutputDebugString("SourceHelper: Unable to locate bandplan table 2\n");
		goto ScanDVBTThread_Wrapup;
	}

	// Now count how many items there are in this bandplan so we can setup the progresbar
	nBandplanEnd = nBandplanOffset;
	while (nDVBTBandplans[nBandplanEnd++] != 0)
		;
	nBandplanChannelCount = nBandplanEnd - nBandplanOffset;
	SendDlgItemMessage(hDlg, IDC_DVBT_SCAN_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM (nBandplanOffset, nBandplanEnd));

	// Now do the scan
	Tune = (td_Tune)GetProcAddress(v->hSource, "TSReader_Tune");
	v->ss.fQuietMode = TRUE; // this stops the source from displaying error dialogs
	for (nIndex = nBandplanOffset; nIndex < nBandplanEnd - 1; nIndex++)
	{
		int nTuneStatus;
		int nBandwidth;

		for (nBandwidth = 1; nBandwidth < 0x10; nBandwidth = nBandwidth << 1)
		{
ScanDVBTThread_Rescan:
			if (v->fAbortScan == TRUE)
				break;
			if ((nDVBTBandwidths[nBandplanTable] & nBandwidth) == 0)
				continue;
			CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, BST_UNCHECKED);
			switch(nBandwidth)
			{
			case 1:		// 5 MHz
				break;		// not yet supported
			case 2:		// 6 MHz
				CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, BST_CHECKED);
				v->ss.nBandwidth = 0;
				break;
			case 4:		// 7 MHz
				CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, BST_CHECKED);
				v->ss.nBandwidth = 1;
				break;
			case 8:		// 8 MHz
				CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, BST_CHECKED);
				v->ss.nBandwidth = 2;
				break;
			}
			v->ss.nFrequency = nDVBTBandplans[nIndex] + nFrequencyOffset;
			nTuneStatus = Tune();
			if (nTuneStatus == TRUE)
			{
				int i;
				char szTemp[32];
				sprintf(szTemp, "%.3f - locked", v->ss.nFrequency / 1000.0);
				SetDlgItemText(hDlg, IDC_DVBT_SCAN_STATUS, szTemp);
				lstrcat(szTemp, "\n");
				OutputDebugString(szTemp);
				for (i = 0; i < v->nTerrestrialMuxes; i++)
				{
					if (v->tmuxes[i].nFrequency == v->ss.nFrequency)
						break;	// already got it
				}
				if (i == v->nTerrestrialMuxes)
				{
					// New one - add it
					v->tmuxes[i].nFrequency = v->ss.nFrequency;
					v->tmuxes[i].nBandwidth = v->ss.nBandwidth;
					v->tmuxes[i].fSpectrumInversion = v->ss.fSpectrumInversion;
					v->tmuxes[v->nTerrestrialMuxes].szDescription[0] = '\0';
					v->nTerrestrialMuxes++;
				}
			}
			else if (nTuneStatus == -1)
			{
				char szTemp[32];
				sprintf(szTemp, "%.3f - analog", v->ss.nFrequency / 1000.0);
				SetDlgItemText(hDlg, IDC_DVBT_SCAN_STATUS, szTemp);
				lstrcat(szTemp, "\n");
				OutputDebugString(szTemp);
			}
			else
			{
				char szTemp[32];
				sprintf(szTemp, "%.3f", v->ss.nFrequency / 1000.0);
				SetDlgItemText(hDlg, IDC_DVBT_SCAN_STATUS, szTemp);
				lstrcat(szTemp, "\n");
				OutputDebugString(szTemp);
			}
			
			// Handle special cases
			if ( (nDVBTBandwidths[nBandplanTable] & UK_OFFSET_FLAG) && (nTuneStatus != TRUE) )
			{
				// We're doing a UK scan so try +/- 167 KHz
				if (nFrequencyOffset == 0)
				{
					nFrequencyOffset = 167;
					goto ScanDVBTThread_Rescan;
				}
				else if (nFrequencyOffset == 167)
				{
					nFrequencyOffset = -167;
					goto ScanDVBTThread_Rescan;
				}
				else
					nFrequencyOffset = 0;
			}
		}

		// Dump the position bar since we're ready for the next frequency
		SendDlgItemMessage(hDlg, IDC_DVBT_SCAN_PROGRESS, PBM_SETPOS, nIndex, 0);
	}		

ScanDVBTThread_Wrapup:
	SendDlgItemMessage(hDlg, IDC_DVBT_SCAN_PROGRESS, PBM_SETPOS, 0, 0);
	SetDlgItemText(hDlg, IDC_DVBT_SCAN_STATUS, "");
	SetDlgItemText(hDlg, IDC_DVBT_SCAN, "Scan");
	if (v->nTerrestrialMuxes)
	{
		qsort(v->tmuxes, v->nTerrestrialMuxes, sizeof(TERRESTRIALMUX), SortTerresrialCompareFunction);
		SetupTerrestrialListView(hDlg, FALSE);
	}
	v->ss.nFrequency = nPriorFrequency;
	v->ss.fQuietMode = FALSE; // this stops the source from displaying error dialogs
	v->fDoingScan = FALSE;
	return 0;
}

void GetDVBTSelectedBandwidthAndInversion(HWND hDlg)
{
	if (IsDlgButtonChecked(hDlg, IDC_BANDWIDTH_6MHz))
		v->ss.nBandwidth = 0;
	else if (IsDlgButtonChecked(hDlg, IDC_BANDWIDTH_7MHz))
		v->ss.nBandwidth = 1;
	else if (IsDlgButtonChecked(hDlg, IDC_BANDWIDTH_8MHz))
		v->ss.nBandwidth = 2;
	if (IsDlgButtonChecked(hDlg, IDC_SPECTRUM_INVERSE))
		v->ss.fSpectrumInversion = TRUE;
	else
		v->ss.fSpectrumInversion = FALSE;
}

BOOL CALLBACK EditDVBTDescriptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTemp[128];

			SetDlgItemText(hDlg, IDC_DVBT_CHANNEL_DESCRIPTION, v->tmuxes[v->nSelectedDVBTMux].szDescription);
			sprintf(szTemp, "%.3f", v->tmuxes[v->nSelectedDVBTMux].nFrequency / 1000.0);
			SetDlgItemText(hDlg, IDC_DVBT_FREQUENCY, szTemp);
			SendDlgItemMessage(hDlg, IDC_DVBT_CHANNEL_DESCRIPTION, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hDlg, IDC_DVBT_CHANNEL_DESCRIPTION));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_DVBT_CHANNEL_DESCRIPTION, v->tmuxes[v->nSelectedDVBTMux].szDescription, sizeof(v->tmuxes[v->nSelectedDVBTMux].szDescription));
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

BOOL CALLBACK TuneDVBTerrestrialDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HANDLE hIcon;
			char szTemp[128];

			if (fTuneDialogFirstTime == TRUE)
			{
				fTuneDialogFirstTime = FALSE;
				LoadDVBTFile();
			}
			v->fBlockFirstMuxListSelection = TRUE;
			v->nSelectedDVBTMux = -1;
			
			wsprintf(szTemp, "Tune %s", v->szSourceModuleDescription);
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)szTemp);
			SetDlgItemText(hDlg, IDC_DVBT_SCAN_STATUS, "");

			sprintf(szTemp, "%.3f", (double)v->ss.nFrequency / 1000.0);
			SetDlgItemText(hDlg, IDC_FREQUENCY, szTemp);
			if (v->ss.fSpectrumInversion)
				CheckDlgButton(hDlg, IDC_SPECTRUM_INVERSE, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_SPECTRUM_NORMAL, BST_CHECKED);
			switch(v->ss.nBandwidth)
			{
			case 0:
				CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, BST_CHECKED);
				break;
			case 1:
				CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, BST_CHECKED);
				break;
			case 2:
				CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, BST_CHECKED);
				break;
			}
			if (v->dwSourceCapabilities & CAPABILITIES_ACTIVE_ANTENNA)
				CheckDlgButton(hDlg, IDC_ACTIVE_ANTENNA_POWER, v->ss.nPolarity);
			else
				EnableWindow(GetDlgItem(hDlg, IDC_ACTIVE_ANTENNA_POWER), FALSE);
			
			SetupDVBTBandplanCombo(hDlg);
			SetupTerrestrialListView(hDlg, TRUE);

			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_DELETE), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_DVBT_DELETE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			SendDlgItemMessage(hDlg, IDC_FREQUENCY, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hDlg, IDC_FREQUENCY));
		}
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		/*case CBN_SELCHANGE:
			{
				int nCursel = SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETCURSEL, 0, 0);
				if (nCursel != LB_ERR)
				{
					CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, BST_UNCHECKED);
					CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, BST_UNCHECKED);
					CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, BST_UNCHECKED);

					switch(nDVBTBandwidths[nCursel] & 0xffff)
					{
					case 0:
						CheckDlgButton(hDlg, IDC_BANDWIDTH_6MHz, BST_CHECKED);
						break;
					case 1:
						CheckDlgButton(hDlg, IDC_BANDWIDTH_7MHz, BST_CHECKED);
						break;
					case 2:
						CheckDlgButton(hDlg, IDC_BANDWIDTH_8MHz, BST_CHECKED);
						break;
					}
				}
			}
			break;*/
		case BN_CLICKED:
			switch(LOWORD(wParam))
			{
			case IDOK:
				{
					double dFrequency;
					char szTemp[128];
					char szFreq[128] = {0};

					int nCursel = SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETCURSEL, 0, 0);
					if (nCursel != LB_ERR)
						SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETLBTEXT, nCursel, (LPARAM)v->szDVBTBandplan);

					GetDlgItemText(hDlg, IDC_FREQUENCY, szTemp, sizeof(szTemp));
					dFrequency = atof(szTemp);
					dFrequency *= 1000.0;
					v->ss.nFrequency = (int)dFrequency;
					GetDVBTSelectedBandwidthAndInversion(hDlg);
					if (v->dwSourceCapabilities & CAPABILITIES_ACTIVE_ANTENNA)
						v->ss.nPolarity = IsDlgButtonChecked(hDlg, IDC_ACTIVE_ANTENNA_POWER);
					else
						v->ss.nPolarity = FALSE;
					EndDialog(hDlg, TRUE);
				}
				break;
			case IDCANCEL:
				{
					int nCursel = SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETCURSEL, 0, 0);
					if (nCursel != LB_ERR)
						SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETLBTEXT, nCursel, (LPARAM)v->szDVBTBandplan);
					EndDialog(hDlg, FALSE);
				}
				break;
			case IDC_DVBT_SCAN:
				{
					if (v->fDoingScan == TRUE)
					{
						v->fAbortScan = TRUE;
					}
					else
					{
						DWORD dwThreadID;
						HANDLE hThread;

						int nCursel = SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETCURSEL, 0, 0);
						if (nCursel == LB_ERR)
						{
							MessageBox(hDlg, "Please select a bandplan appropriate for your location first", gszAppName, MB_ICONSTOP);
							break;
						}
						SendDlgItemMessage(hDlg, IDC_DVBT_BANDPLAN, CB_GETLBTEXT, nCursel, (LPARAM)v->szDVBTBandplan);
						GetDVBTSelectedBandwidthAndInversion(hDlg);

						v->fDoingScan = TRUE;
						v->fAbortScan = FALSE;
						SetDlgItemText(hDlg, IDC_DVBT_SCAN, "Stop");

						hThread = CreateThread(NULL, 0, ScanDVBTThread, (LPVOID)hDlg, 0, &dwThreadID);
						CloseHandle(hThread);					
					}
				}
				break;
			case IDC_DVBT_EDIT:
				if (v->nSelectedDVBTMux != -1)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_EDIT_DVBT_DESCRIPTION), hDlg, EditDVBTDescriptionDlgProc) == TRUE)
						ListView_RedrawItems(GetDlgItem(hDlg, IDC_TERRESTRIAL_LIST), v->nSelectedDVBTMux, v->nSelectedDVBTMux);
					SetFocus(GetDlgItem(hDlg, IDC_TERRESTRIAL_LIST));
				}
				else
					MessageBox(hDlg, "Please select the mux who's description you want to change.", gszAppName, MB_ICONSTOP);
				break;
			case IDC_DVBT_DELETE:
				if (v->nSelectedDVBTMux != -1)
				{
					if (MessageBox(hDlg, "Are you sure you want to delete this mux?", gszAppName, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2) == IDYES)
					{
						int i;
						int nOutCounter = 0;
						PTERRESTRIALMUX pNewMuxes = LocalAlloc(LPTR, sizeof(TERRESTRIALMUX) * v->nTerrestrialMuxes - 1);
						
						for (i = 0; i < v->nTerrestrialMuxes; i++)
						{
							if (i != v->nSelectedDVBTMux)
								memcpy(&pNewMuxes[nOutCounter++], &v->tmuxes[i], sizeof(TERRESTRIALMUX));
						}
						memset(v->tmuxes, 0, sizeof(TERRESTRIALMUX) * MAX_TERRESTRIAL_MUXES);
						v->nTerrestrialMuxes--;
						memcpy(v->tmuxes, pNewMuxes, sizeof(TERRESTRIALMUX) * v->nTerrestrialMuxes);
						LocalFree(pNewMuxes);
						ListView_DeleteItem(GetDlgItem(hDlg, IDC_TERRESTRIAL_LIST), v->nSelectedDVBTMux);
						ListView_RedrawItems(GetDlgItem(hDlg, IDC_TERRESTRIAL_LIST), 0, v->nTerrestrialMuxes);
					}
					SetFocus(GetDlgItem(hDlg, IDC_TERRESTRIAL_LIST));
				}
				else
					MessageBox(hDlg, "Please select the mux you want to delete.", gszAppName, MB_ICONSTOP);
				break;
			}
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
				GetTerrestrialDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
				{
					UpdateTerrestrialMux(hDlg, pnmv->iItem);
					v->nSelectedDVBTMux = pnmv->iItem;
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
	case WM_DESTROY:
		SaveDVBTFile();
		break;
	}

	return FALSE;
}


int SourceHelper_GetFrequencyFromQAMChannel(int nChannel)
{
	// Such a pain we just use a table!

	if (nChannel < 1 || nChannel > 134)
		return 0;

	if (v->fHRCQAM == TRUE)
		return nIRCChannelTable[nChannel - 1] - 1;

	return nIRCChannelTable[nChannel - 1];
}

int SourceHelper_GetFrequencyFromATSCChannel(int nChannel)
{
	int nFrequency = 0;

	if (nChannel < 2)
		return 0;

	// Calculate US frequencies from channel number
	if (nChannel < 7 && nChannel > 1)
		nFrequency = bVHFLowBandOffAirFrequencies[nChannel - 2];	// VHF low band
	else if (nChannel < 14)
		nFrequency = 177 + ((nChannel - 7) * 6);					// VHF high band
	else if (nChannel <= 69)
		nFrequency = 473 + ((nChannel - 14) * 6);				// UHF band

	return nFrequency;
}

DWORD WINAPI ScanATSCQAMThread(LPVOID lpv)
{
	BOOL (* Tune) ();
	HWND hDlg = (HWND)lpv;
	int nChannel;
	int nPriorFrequency = v->ss.nFrequency;

	Tune = (td_Tune)GetProcAddress(v->hSource, "TSReader_Tune");
	v->ss.fQuietMode = TRUE; // this stops the source from displaying error dialogs
	ShowWindow(GetDlgItem(hDlg, IDC_ATSC_SCAN_PROGRESS), SW_SHOW);
	SendDlgItemMessage(hDlg, IDC_ATSC_SCAN_PROGRESS, PBM_SETRANGE, 0, MAKELPARAM (nATSCQAMScanRangeStart, nATSCQAMScanRangeEnd));
	for (nChannel = nATSCQAMScanRangeStart; nChannel <= nATSCQAMScanRangeEnd; nChannel++)
	{
		int nTuneStatus;

		if (v->fAbortScan == TRUE)
			break;
		if (v->fQAMMode == FALSE)
			v->ss.nFrequency = SourceHelper_GetFrequencyFromATSCChannel(nChannel);
		else
			v->ss.nFrequency = SourceHelper_GetFrequencyFromQAMChannel(nChannel);
		nTuneStatus = Tune();
		if (v->fAbortScan == TRUE)
			break;
		if (nTuneStatus == TRUE)
		{
			int i;
			char szTemp[32];
			wsprintf(szTemp, "%d - locked", nChannel);
			SetDlgItemText(hDlg, IDC_ATSC_SCAN_STATUS, szTemp);
			lstrcat(szTemp, "\n");
			OutputDebugString(szTemp);
			for (i = 0; i < v->nATSCMuxes; i++)
			{
				if (v->amuxes[i].nFrequency == v->ss.nFrequency)
					break;	// already got it
			}
			if (i == v->nATSCMuxes)
			{
				// New one - add it
				v->amuxes[i].nFrequency = v->ss.nFrequency;
				v->amuxes[v->nATSCMuxes].szDescription[0] = '\0';
				v->nATSCMuxes++;
			}
		}
		else if (nTuneStatus == -1)
		{
			char szTemp[32];
			wsprintf(szTemp, "%d - NTSC", nChannel);
			SetDlgItemText(hDlg, IDC_ATSC_SCAN_STATUS, szTemp);
			lstrcat(szTemp, "\n");
			OutputDebugString(szTemp);
		}
		else
		{
			char szTemp[32];
			wsprintf(szTemp, "%d", nChannel);
			SetDlgItemText(hDlg, IDC_ATSC_SCAN_STATUS, szTemp);
			lstrcat(szTemp, "\n");
			OutputDebugString(szTemp);
		}
		SendDlgItemMessage(hDlg, IDC_ATSC_SCAN_PROGRESS, PBM_SETPOS, nChannel, 0);
	}
	if (!v->fAbortScan)
	{
		ShowWindow(GetDlgItem(hDlg, IDC_ATSC_SCAN_PROGRESS), SW_HIDE);
		SendDlgItemMessage(hDlg, IDC_ATSC_SCAN_PROGRESS, PBM_SETPOS, 0, 0);
		SetDlgItemText(hDlg, IDC_ATSC_SCAN_STATUS, "");
		SetDlgItemText(hDlg, IDC_ATSC_SCAN, "Scan");
		if (v->nATSCMuxes)
		{
			qsort(v->amuxes, v->nATSCMuxes, sizeof(ATSCMUX), SortATSCCompareFunction);
			SetupATSCListView(hDlg, FALSE);
		}
	}
	v->ss.nFrequency = nPriorFrequency;
	v->ss.fQuietMode = FALSE; // this stops the source from displaying error dialogs
	v->fDoingScan = FALSE;
	return 0;
}

BOOL CALLBACK EditATSCDescriptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_ATSC_CHANNEL_DESCRIPTION, v->amuxes[v->nSelectedATSCMux].szDescription);
		if (v->fQAMMode == FALSE)
			SetDlgItemInt(hDlg, IDC_ATSC_CHANNEL, SourceHelper_GetATSCChannelFromFrequency(v->amuxes[v->nSelectedATSCMux].nFrequency), FALSE);
		else
			SetDlgItemInt(hDlg, IDC_ATSC_CHANNEL, SourceHelper_GetQAMChannelFromFrequency(v->amuxes[v->nSelectedATSCMux].nFrequency), FALSE);
		SendDlgItemMessage(hDlg, IDC_ATSC_CHANNEL_DESCRIPTION, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_ATSC_CHANNEL_DESCRIPTION));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_ATSC_CHANNEL_DESCRIPTION, v->amuxes[v->nSelectedATSCMux].szDescription, sizeof(v->amuxes[v->nSelectedATSCMux].szDescription));
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

BOOL CALLBACK AddATSCChannelDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			int nFrequency = GetDlgItemInt(GetParent(hDlg), IDC_FREQUENCY, NULL, FALSE);
			int nChannel;
			char szTemp[128];

			if (lParam == 0)
				nChannel = SourceHelper_GetATSCChannelFromFrequency(nFrequency);
			else
				nChannel = SourceHelper_GetQAMChannelFromFrequency(nFrequency);

			wsprintf(szTemp, "Channel %d Frequency %d MHz", nChannel, nFrequency);
			SetDlgItemText(hDlg, IDC_SIGNAL_PARAMETERS, szTemp);
			SetFocus(GetDlgItem(hDlg, IDC_MUX_NAME));
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_MUX_NAME, v->szNewMuxName, sizeof(v->szNewMuxName));
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

void WaitForScanTermination(HWND hDlg)
{
	v->fAbortScan = TRUE;
	while(v->fDoingScan)
	{
		MSG msg;

		while (PeekMessage(&msg, hDlg, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(50);
	}
	ShowWindow(GetDlgItem(hDlg, IDC_ATSC_SCAN_PROGRESS), SW_HIDE);
	SendDlgItemMessage(hDlg, IDC_ATSC_SCAN_PROGRESS, PBM_SETPOS, 0, 0);
	SetDlgItemText(hDlg, IDC_ATSC_SCAN_STATUS, "");
	SetDlgItemText(hDlg, IDC_ATSC_SCAN, "Scan");
}

BOOL CALLBACK ScanATSCQAMRangeDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		if (v->fQAMMode)
		{
			nATSCQAMScanRangeStart = 2;
			nATSCQAMScanRangeEnd = 135;
		}
		else
		{
			nATSCQAMScanRangeStart = 2;
			nATSCQAMScanRangeEnd = 69;
			if (IsDlgButtonChecked(GetParent(hDlg), IDC_ATSC_SCAN_UHF))
				nATSCQAMScanRangeStart = 14;
		}
		SetDlgItemInt(hDlg, IDD_SCAN_ATSC_QAM_RANGE_START, nATSCQAMScanRangeStart, FALSE);
		SetDlgItemInt(hDlg, IDD_SCAN_ATSC_QAM_RANGE_END, nATSCQAMScanRangeEnd, FALSE);
		SetFocus(GetDlgItem(hDlg, IDOK));
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			nATSCQAMScanRangeStart = GetDlgItemInt(hDlg, IDD_SCAN_ATSC_QAM_RANGE_START, NULL, FALSE);
			nATSCQAMScanRangeEnd = GetDlgItemInt(hDlg, IDD_SCAN_ATSC_QAM_RANGE_END, NULL, FALSE);
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

BOOL FindATSCQAMChannelAndSelectInList(HWND hDlg)
{
	int nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
	int nIndex;

	for (nIndex = 0; nIndex < v->nATSCMuxes; nIndex++)
	{
		if (nFrequency == v->amuxes[nIndex].nFrequency)
		{
			HWND hWndList = GetDlgItem(hDlg, IDC_ATSC_LIST);
			ListView_SetItemState(hWndList, nIndex, LVIS_SELECTED, LVIS_SELECTED);
			//v->nSelectedATSCMux = nIndex;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CALLBACK TuneATSCQAMDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL fBlockNextEN_CHANGE = FALSE;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HANDLE hIcon;
			char szTemp[128];
			
			//v->fBlockFirstMuxListSelection = TRUE;			
			wsprintf(szTemp, "Tune %s", v->szSourceModuleDescription);
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)szTemp);
			SetDlgItemInt(hDlg, IDC_FREQUENCY, v->ss.nFrequency, FALSE);
			SetDlgItemText(hDlg, IDC_ATSC_SCAN_STATUS, "");
			if (fTuneDialogFirstTime == TRUE)
			{
				fTuneDialogFirstTime = FALSE;
				LoadATSCQAMFile();
			}
			SetupATSCListView(hDlg, TRUE);
			v->nSelectedATSCMux = -1;
			if (v->fQAMMode)
			{
				if (v->fHRCQAM == TRUE)
					CheckDlgButton(hDlg, IDC_QAM_TYPE_HRC, BST_CHECKED);
				else
					CheckDlgButton(hDlg, IDC_QAM_TYPE_IRC, BST_CHECKED);
			}

			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_DELETE), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_ATSC_DELETE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ADD), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_ATSC_ADD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
						
			SendDlgItemMessage(hDlg, IDC_ATSC_CHANNEL, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hDlg, IDC_ATSC_CHANNEL));
		}
		break;
	case WM_COMMAND:
		switch(HIWORD(wParam))
		{
		case EN_CHANGE:
			if (fBlockNextEN_CHANGE == TRUE)
			{
				//fBlockNextEN_CHANGE = FALSE;
			}
			else
			{
				if ((HWND)lParam == GetDlgItem(hDlg, IDC_ATSC_CHANNEL))
				{
					// Channel changed - recalculate frequency
					int nChannel = GetDlgItemInt(hDlg, IDC_ATSC_CHANNEL, NULL, FALSE);
					fBlockNextEN_CHANGE = TRUE;
					if (v->fQAMMode == FALSE)
						SetDlgItemInt(hDlg, IDC_FREQUENCY, SourceHelper_GetFrequencyFromATSCChannel(nChannel), FALSE);
					else
						SetDlgItemInt(hDlg, IDC_FREQUENCY, SourceHelper_GetFrequencyFromQAMChannel(nChannel), FALSE);
					fBlockNextEN_CHANGE = FALSE;
				}
				else
				{
					// Frequency changed - recalculate channel
					int nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
					fBlockNextEN_CHANGE = TRUE;
					if (v->fQAMMode == FALSE)
						SetDlgItemInt(hDlg, IDC_ATSC_CHANNEL, SourceHelper_GetATSCChannelFromFrequency(nFrequency), FALSE);
					else
						SetDlgItemInt(hDlg, IDC_ATSC_CHANNEL, SourceHelper_GetQAMChannelFromFrequency(nFrequency), FALSE);
					fBlockNextEN_CHANGE = FALSE;
				}
			}
			break;
		case BN_CLICKED:			
			switch(LOWORD(wParam))
			{
			case IDC_ATSC_EDIT:				
				if (v->nSelectedATSCMux != -1)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_EDIT_ATSC_DESCRIPTION), hDlg, EditATSCDescriptionDlgProc) == TRUE)
						ListView_RedrawItems(GetDlgItem(hDlg, IDC_ATSC_LIST), v->nSelectedATSCMux, v->nSelectedATSCMux);
					SetFocus(GetDlgItem(hDlg, IDC_ATSC_LIST));
				}
				else
				{
					if (FindATSCQAMChannelAndSelectInList(hDlg) == TRUE)
						PostMessage(hDlg, WM_COMMAND, IDC_ATSC_EDIT, 0);
					else
						MessageBox(hDlg, "Please select the mux who's description you want to change.", gszAppName, MB_ICONSTOP);
				}
				break;
			case IDC_ATSC_DELETE:
				if (v->nSelectedATSCMux != -1)
				{
					int i;
					int nOutCounter = 0;
					PATSCMUX pNewMuxes;
					
					if (v->nATSCMuxes)
					{
						pNewMuxes = LocalAlloc(LPTR, sizeof(ATSCMUX) * v->nATSCMuxes - 1);
						for (i = 0; i < v->nATSCMuxes; i++)
						{
							if (i != v->nSelectedATSCMux)
								memcpy(&pNewMuxes[nOutCounter++], &v->amuxes[i], sizeof(ATSCMUX));
						}
						memset(v->amuxes, 0, sizeof(ATSCMUX) * MAX_TERRESTRIAL_MUXES);
						v->nATSCMuxes--;
						memcpy(v->amuxes, pNewMuxes, sizeof(ATSCMUX) * v->nATSCMuxes);
						LocalFree(pNewMuxes);
						ListView_DeleteItem(GetDlgItem(hDlg, IDC_ATSC_LIST), v->nSelectedATSCMux);
						ListView_RedrawItems(GetDlgItem(hDlg, IDC_ATSC_LIST), 0, v->nATSCMuxes);
						ListView_SetItemState(GetDlgItem(hDlg, IDC_ATSC_LIST), v->nSelectedATSCMux, LVIS_SELECTED, LVIS_SELECTED);
					}					
					SetFocus(GetDlgItem(hDlg, IDC_ATSC_LIST));
				}
				else
				{
					if (FindATSCQAMChannelAndSelectInList(hDlg) == TRUE)
						PostMessage(hDlg, WM_COMMAND, IDC_ATSC_DELETE, 0);
					else
						MessageBox(hDlg, "Please select the mux you want to delete.", gszAppName, MB_ICONSTOP);
				}
				break;
			case IDC_ATSC_ADD:
				{
					int nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
					BOOL fContinue = TRUE;
					int nIndex;

					if (nFrequency == 0)
					{
						MessageBox(hDlg, "Please enter a valid channel/frequency first", gszAppName, MB_ICONWARNING);
						SetFocus(GetDlgItem(hDlg, IDC_ATSC_CHANNEL));
						break;
					}
					for (nIndex = 0; nIndex < v->nATSCMuxes; nIndex++)
					{
						if (nFrequency == v->amuxes[nIndex].nFrequency)
						{
							MessageBox(hDlg, "This channel/frequency is already defined", gszAppName, MB_ICONWARNING);
							fContinue = FALSE;
							SetFocus(GetDlgItem(hDlg, IDC_ATSC_CHANNEL));
							break;
						}
					}
					if (!fContinue)
						break;

					if (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_ADD_ATSC), hDlg, AddATSCChannelDlgProc, v->fQAMMode) == TRUE)
					{
						HWND hWndList = GetDlgItem(hDlg, IDC_ATSC_LIST);
						LV_ITEM lvi;

						v->amuxes[v->nATSCMuxes].nFrequency = nFrequency;
						lstrcpy(v->amuxes[v->nATSCMuxes].szDescription, v->szNewMuxName);

						memset(&lvi, 0, sizeof(lvi));
						lvi.state = 0; 
						lvi.stateMask = 0; 
						lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
						lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
						lvi.iItem = v->nATSCMuxes; 
						lvi.iSubItem = 0; 
						lvi.lParam = (LPARAM) 0;    // item data 
						ListView_InsertItem(hWndList, &lvi);
						ListView_SetItemState(hWndList, v->nATSCMuxes, LVIS_SELECTED, LVIS_SELECTED);
						ListView_EnsureVisible(hWndList, v->nATSCMuxes, FALSE);
						SetFocus(hWndList);
						v->nATSCMuxes++;
					}
				}
				break;
			case IDCANCEL:
				if (v->fDoingScan == TRUE)
					WaitForScanTermination(hDlg);
				EndDialog(hDlg, FALSE);
				break;
			case IDOK:
				if (v->fDoingScan == TRUE)
					WaitForScanTermination(hDlg);
				v->ss.nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
				EndDialog(hDlg, TRUE);
				break;
			case IDC_ATSC_SCAN:
				if (v->fDoingScan == TRUE)
				{
					WaitForScanTermination(hDlg);
				}
				else
				{
					DWORD dwThreadID;
					HANDLE hThread;

					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_SCAN_ATSC_QAM_RANGE), hDlg, ScanATSCQAMRangeDlgProc) == TRUE)
					{
						v->fDoingScan = TRUE;
						v->fAbortScan = FALSE;
						SetDlgItemText(hDlg, IDC_ATSC_SCAN, "Stop");

						hThread = CreateThread(NULL, 0, ScanATSCQAMThread, (LPVOID)hDlg, 0, &dwThreadID);
						CloseHandle(hThread);
					}
				}
				break;
			case IDC_QAM_TYPE_IRC:
			case IDC_QAM_TYPE_HRC:
				v->fHRCQAM = IsDlgButtonChecked(hDlg, IDC_QAM_TYPE_HRC);
				InvalidateRect(hDlg, NULL, FALSE);
				break;
			}
			break;	
		}
		break;
	case WM_CLOSE:
		if (v->fDoingScan == TRUE)
			WaitForScanTermination(hDlg);
		EndDialog(hDlg, FALSE);
		break;
	case WM_DESTROY:
		if (v->nATSCMuxes)
		{
			int i;
			HANDLE hList;
			char szListFile[MAX_PATH];

			SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
			if (v->fQAMMode)
				lstrcat(szListFile, "\\QAM.lst");
			else
				lstrcat(szListFile, "\\ATSC.lst");
			hList = CreateFile(szListFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
			if (hList != INVALID_HANDLE_VALUE)
			{
				DWORD dwWritten;
				char szLine[128];

				for (i = 0; i < v->nATSCMuxes; i++)
				{
					wsprintf(szLine, "%d\t%s\r\n", v->amuxes[i].nFrequency, v->amuxes[i].szDescription);
					WriteFile(hList, szLine, lstrlen(szLine), &dwWritten, NULL);
				}
				CloseHandle(hList);
			}
		}
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetATSCDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
				{
					UpdateATSCMux(hDlg, pnmv->iItem);
					v->nSelectedATSCMux = pnmv->iItem;
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

int __cdecl SortMuxCompareFunction(const void *elem1, const void *elem2)
{
	PMUX pMux1 = (PMUX)elem1;
	PMUX pMux2 = (PMUX)elem2;

	if (pMux1->nFrequency < pMux2->nFrequency)
		return -1;
	if (pMux1->nFrequency > pMux2->nFrequency)
		return 1;
	return 0;
}

int __cdecl SortSatelliteCompareFunctionEW(const void *elem1, const void *elem2)
{
	PSATELLITE pSAT1 = (PSATELLITE)elem1;
	PSATELLITE pSAT2 = (PSATELLITE)elem2;
	int nSort1, nSort2;

	nSort1 = pSAT1->nOrbitalPosition;
	if (pSAT1->fWest)
		nSort1 += 1800;
	nSort2 = pSAT2->nOrbitalPosition;
	if (pSAT2->fWest)
		nSort2 += 1800;

	if (nSort1 < nSort2)
		return -1;
	if (nSort1 > nSort2)
		return 1;
	return 0;
}

int __cdecl SortSatelliteCompareFunctionEast(const void *elem1, const void *elem2)
{
	PSATELLITE pSAT1 = (PSATELLITE)elem1;
	PSATELLITE pSAT2 = (PSATELLITE)elem2;
	int nSort1, nSort2;

	nSort1 = pSAT1->nOrbitalPosition;
	nSort2 = pSAT2->nOrbitalPosition;

	if (nSort1 > nSort2)
		return -1;
	if (nSort1 < nSort2)
		return 1;
	return 0;
}

void GetMuxName(char * szMuxLine, char * szMuxName)
{
	int nCommas = 0;
	char * szPtr = szMuxLine + lstrlen(szMuxLine);
	char * szCurrent = szMuxLine;

	// Check comma count - must be at least three
	do
	{
		char * szNext = strstr(szCurrent, ",");
		if (szNext == NULL)
			break;
		nCommas++;
		szCurrent = szNext + 1;
	} while (TRUE);

	if (nCommas > 2)
	{
		while (szPtr != szMuxLine)
		{
			if (*szPtr == ',')
			{
				// Check for lines without a mux name - get the code rate instead
				if (lstrcmp(szPtr + 1, "12") == 0)
					break;
				else if (lstrcmp(szPtr + 1, "23") == 0)
					break;
				else if (lstrcmp(szPtr + 1, "34") == 0)
					break;
				else if (lstrcmp(szPtr + 1, "56") == 0)
					break;
				else if (lstrcmp(szPtr + 1, "78") == 0)
					break;
				else if (lstrcmp(szPtr + 1, "00") == 0)
					break;

				lstrcpy(szMuxName, szPtr + 1);			
				return;
			}
			szPtr--;
		}
	}
	szMuxName[0] = 0;
}

void SortSatelliteList()
{
	int i;

	qsort(v->sats, v->nSatellites, sizeof(SATELLITE), SortSatelliteCompareFunctionEW);

	for (i = 0; i < v->nSatellites; i++)
	{
		if (v->sats[i].fWest)
			break;
	}
	if (i != v->nSatellites)
		qsort(v->sats, i, sizeof(SATELLITE), SortSatelliteCompareFunctionEast);
}

void LoadSatelliteFiles()
{
	HANDLE hFind;
    char szCurrentDir[MAX_PATH];
	char szCurrentFile[MAX_PATH * 2];
	WIN32_FIND_DATA fd;

	v->nSatellites = 0;
	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	wsprintf(szCurrentFile, "%s\\Satellites\\*.ini", szCurrentDir);
    hFind = FindFirstFile(szCurrentFile, &fd);
    if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			char szOrbitalPosition[16];
			char szSatelliteName[48];
			char szDVBMuxCount[16];
			char szADVMuxCount[16] = {0};
			char szLastLOF[16];
			char szLastDiSEqC[16];

			wsprintf(szCurrentFile, "%s\\Satellites\\%s", szCurrentDir, fd.cFileName);
			GetPrivateProfileString("SATTYPE", "1", "", szOrbitalPosition, sizeof(szOrbitalPosition), szCurrentFile);
			GetPrivateProfileString("SATTYPE", "2", "", szSatelliteName, sizeof(szSatelliteName), szCurrentFile);
			if (strstr(szSatelliteName, "dvb-c") != NULL)
				continue;
			GetPrivateProfileString(szTypeName, "0", "", szDVBMuxCount, sizeof(szDVBMuxCount), szCurrentFile);
			if (v->fADVModulation)
				GetPrivateProfileString("ADV", "0", "", szADVMuxCount, sizeof(szADVMuxCount), szCurrentFile);
			GetPrivateProfileString("TSREADER", "LOF", "", szLastLOF, sizeof(szLastLOF), szCurrentFile);
			GetPrivateProfileString("TSREADER", "DISEQC", "", szLastDiSEqC, sizeof(szLastDiSEqC), szCurrentFile);

			if (lstrlen(szOrbitalPosition) && lstrlen(szSatelliteName))
			{
				int nIndex;

				for (nIndex = 0; nIndex < MAX_SATELLITES; nIndex++)
				{
					if (v->sats[nIndex].nOrbitalPosition == -1)
					{
						int nMux;
						int nDVBMuxCount = 0;
						int nADVMuxCount = 0;

						sscanf(szOrbitalPosition, "%d", &v->sats[nIndex].nOrbitalPosition);
						if (v->sats[nIndex].nOrbitalPosition > 1800)
						{
							v->sats[nIndex].nOrbitalPosition = 3600 - v->sats[nIndex].nOrbitalPosition;
							v->sats[nIndex].fWest = TRUE;
						}
						lstrcpy(v->sats[nIndex].szName, szSatelliteName);
						lstrcpy(v->sats[nIndex].szSourceFilename, fd.cFileName);
						v->sats[nIndex].fDirty = FALSE;
						sscanf(szDVBMuxCount, "%d", &nDVBMuxCount);
						if (lstrlen(szADVMuxCount))
							sscanf(szADVMuxCount, "%d", &nADVMuxCount);
						v->sats[nIndex].nLastLOF = 0;
						if (lstrlen(szLastLOF))
							sscanf(szLastLOF, "%d", &v->sats[nIndex].nLastLOF);
						v->sats[nIndex].nLastDiSEqC = 0;
						if (lstrlen(szLastDiSEqC))
							sscanf(szLastDiSEqC, "%d", &v->sats[nIndex].nLastDiSEqC);

						v->sats[nIndex].nMuxCount = nDVBMuxCount + nADVMuxCount;
						if (v->sats[nIndex].nMuxCount)
						{
							v->sats[nIndex].mux = LocalAlloc(LPTR, v->sats[nIndex].nMuxCount * sizeof(MUX));
							for (nMux = 0; nMux < nDVBMuxCount; nMux++)
							{
								char szMuxNumber[16];
								char szMuxLine[256] = {0};
								wsprintf(szMuxNumber, "%d", nMux + 1);
								GetPrivateProfileString(szTypeName, szMuxNumber, "", szMuxLine, sizeof(szMuxLine), szCurrentFile);
								sscanf(szMuxLine, "%d,%c,%d,%d", 
									   &v->sats[nIndex].mux[nMux].nFrequency,
									   v->sats[nIndex].mux[nMux].szPolarity,
									   &v->sats[nIndex].mux[nMux].nSymbolRate,
									   &v->sats[nIndex].mux[nMux].nCodeRate);
								GetMuxName(szMuxLine, v->sats[nIndex].mux[nMux].szMuxName);
								strupr(v->sats[nIndex].mux[nMux].szPolarity);

								// Check for SLT exported INI files
								if (v->sats[nIndex].mux[nMux].nFrequency > 99999)
									v->sats[nIndex].mux[nMux].nFrequency /= 1000;
								if (v->sats[nIndex].mux[nMux].nSymbolRate > 99999)
									v->sats[nIndex].mux[nMux].nSymbolRate /= 1000;

								// Translate the silly MyTheatre FEC rate to our index							
								switch(v->sats[nIndex].mux[nMux].nCodeRate)
								{
								case 12:
									v->sats[nIndex].mux[nMux].nCodeRate = 0;
									break;
								case 23:
									v->sats[nIndex].mux[nMux].nCodeRate = 1;
									break;
								case 34:
									v->sats[nIndex].mux[nMux].nCodeRate = 2;
									break;
								case 56:
									v->sats[nIndex].mux[nMux].nCodeRate = 3;
									break;
								case 78:
									v->sats[nIndex].mux[nMux].nCodeRate = 4;
									break;
								default:
									v->sats[nIndex].mux[nMux].nCodeRate = 5;
									break;
								}
								if (lstrcmp(szTypeName, "DSS") == 0)
									v->sats[nIndex].mux[nMux].nTuneType = MUX_TUNE_TYPE_DSS;
								else
									v->sats[nIndex].mux[nMux].nTuneType = MUX_TUNE_TYPE_DVB;
							}
							if (v->fADVModulation)
							{
								for (nMux = 0; nMux < nADVMuxCount; nMux++)
								{
									char szMuxNumber[10];
									char szMuxLine[100];
									wsprintf(szMuxNumber, "%d", nMux + 1);
									GetPrivateProfileString("ADV", szMuxNumber, "", szMuxLine, sizeof(szMuxLine), szCurrentFile);
									sscanf(szMuxLine, "%d,%c,%d,%d,%d", 
										   &v->sats[nIndex].mux[nDVBMuxCount + nMux].nFrequency,
										   v->sats[nIndex].mux[nDVBMuxCount + nMux].szPolarity,
										   &v->sats[nIndex].mux[nDVBMuxCount + nMux].nSymbolRate,
										   &v->sats[nIndex].mux[nDVBMuxCount + nMux].nCodeRate,
										   &v->sats[nIndex].mux[nDVBMuxCount + nMux].nModulationMode);
									GetMuxName(szMuxLine, v->sats[nIndex].mux[nDVBMuxCount + nMux].szMuxName);
									v->sats[nIndex].mux[nDVBMuxCount + nMux].fADVModulation = TRUE;
									strupr(v->sats[nIndex].mux[nDVBMuxCount + nMux].szPolarity);
									v->sats[nIndex].mux[nDVBMuxCount + nMux].nTuneType = MUX_TUNE_TYPE_ADV;
								}
							}
							if (nDVBMuxCount || nADVMuxCount)
								qsort(v->sats[nIndex].mux, v->sats[nIndex].nMuxCount, sizeof(MUX), SortMuxCompareFunction);
						}
						v->nSatellites++;
						break;
					}
				}
				if (nIndex == MAX_SATELLITES)
					break;
			}
		} while (FindNextFile(hFind, &fd) == TRUE);
		FindClose(hFind);
		if (v->nSatellites)
			SortSatelliteList();
	}
}


BOOL CALLBACK LNBLOWarningDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		case IDC_DONT_SHOW_AGAIN:
			v->fDisableLNBFrequencyWarning = IsDlgButtonChecked(hDlg, IDC_DONT_SHOW_AGAIN);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}

void StopMoving(HWND hDlg)
{
	BYTE bCommand[] = {0xE0, 0x31, 0x60};
	SendDiSEqC(bCommand, sizeof(bCommand));
	SetDlgItemText(hDlg, IDC_MOVE_EAST, "< East");
	SetDlgItemText(hDlg, IDC_MOVE_WEST, "> West");
}

void SetDlgItemDouble(HWND hDlg, int nControl, double dValue)
{
	char szTemp[128];
	sprintf(szTemp, "%.3f", dValue);
	SetDlgItemText(hDlg, nControl, szTemp);
}

double GetDlgItemDouble(HWND hDlg, int nControl)
{
	char szTemp[128];

	GetDlgItemText(hDlg, nControl, szTemp, sizeof(szTemp));
	return atof(szTemp);	
}

double factorial_div( double value, int x)
{
	if(!x)
		return 1;
	else
	{
		while( x > 1)
		{
			value = value / x--;
		}
	}
	return value;
}

double powerd( double x, int y)
{
	int i=0;
	double ans=1.0;

	if(!y)
		return 1.000;
	else
	{
		while( i < y)
		{
			i++;
			ans = ans * x;
		}
	}
	return ans;
}

double round(double x)
{
	double t;

	return x;

	if (x >= 0.0)
	{
		t = ceil(x);
		if (t - x > 0.5)
			t -= 1.0;
		return t;
	}
	else
	{
		t = ceil(-x);
		if (t + x > 0.5)
			t -= 1.0;
		return -t;
	}
}

double SIN( double x)
{
	int i=0;
	int j=1;
	int sign=1;
	double y1 = 0.0;
	double diff = 1000.0;

	if (x < 0.0)
	{
		x = -1 * x;
		sign = -1;
	}

	while ( x > 360.0*M_PI/180)
	{
		x = x - 360*M_PI/180;
	}

	if( x > (270.0 * M_PI / 180) )
	{
		sign = sign * -1;
		x = 360.0*M_PI/180 - x;
	}
	else if ( x > (180.0 * M_PI / 180) )
	{
		sign = sign * -1;
		x = x - 180.0 *M_PI / 180;
	}
	else if ( x > (90.0 * M_PI / 180) )
	{
		x = 180.0 *M_PI / 180 - x;
	}

	while( powerd( diff, 2) > 1.0E-16 )
	{
		i++;
		diff = j * factorial_div( powerd( x, (2*i -1)) ,(2*i -1));
		y1 = y1 + diff;
		j = -1 * j;
	}
	return ( sign * y1 );
}

double COS(double x)
{
	return SIN(90 * M_PI / 180 - x);
}

double ATAN( double x)
{
	int i=0; /* counter for terms in binomial series */
	int j=1; /* sign of nth term in series */
	int k=0;
	int sign = 1; /* sign of the input x */
	double y = 0.0; /* the output */
	double deltay = 1.0; /* the value of the next term in the series */
	double addangle = 0.0; /* used if arctan > 22.5 degrees */

	if (x < 0.0)
	{
		x = -1 * x;
		sign = -1;
	}

	while( x > 0.3249196962 )
	{
		k++;
		x = (x - 0.3249196962) / (1 + x * 0.3249196962);
	}
	addangle = k * 18.0 *M_PI/180;

	while( powerd( deltay, 2) > 1.0E-16 )
	{
		i++;
		deltay = j * powerd( x, (2*i -1)) / (2*i -1);
		y = y + deltay;
		j = -1 * j;
	}
	return (sign * (y + addangle) );
}

double ASIN(double x)
{
	return 2 * ATAN( x / (1 + sqrt(1.0 - x*x)));
}

double Radians( double number )
{
	return number*M_PI/180;
}

double Deg( double number )
{
	return number*180/M_PI;
}

double Rev( double number )
{
	return number - floor( number / 360.0 ) * 360;
}

double calcElevation(double SatLon, double SiteLat, double SiteLon, int Height_over_ocean)
{
	double a0=0.58804392;
	double a1=-0.17941557;
	double a2=0.29906946E-1;
	double a3=-0.25187400E-2;
	double a4=0.82622101E-4;
	double f = 1.00 / 298.257; // Earth flattning factor
	double r_sat=42164.57; // Distance from earth centre to satellite
	double r_eq=6378.14;  // Earth radius
	double sinRadSiteLat=SIN(Radians(SiteLat));
	double cosRadSiteLat=COS(Radians(SiteLat));
	double Rstation = r_eq / ( sqrt( 1.00 - f*(2.00-f)*sinRadSiteLat*sinRadSiteLat ) );
	double Ra = (Rstation+Height_over_ocean)*cosRadSiteLat;
	double Rz= Rstation*(1.00-f)*(1.00-f)*sinRadSiteLat;
	double alfa_rx=r_sat*COS(Radians(SatLon-SiteLon)) - Ra;
	double alfa_ry=r_sat*SIN(Radians(SatLon-SiteLon));
	double alfa_rz=-Rz;
	double alfa_r_north=-alfa_rx*sinRadSiteLat + alfa_rz*cosRadSiteLat;
	double alfa_r_zenith=alfa_rx*cosRadSiteLat + alfa_rz*sinRadSiteLat;
	double El_geometric=Deg(ATAN( alfa_r_zenith/sqrt(alfa_r_north*alfa_r_north+alfa_ry*alfa_ry)));
	double x = fabs(El_geometric+0.589);
	double refraction=fabs(a0+a1*x+a2*x*x+a3*x*x*x+a4*x*x*x*x);
	double El_observed = 0.00;

	if (El_geometric > 10.2)
		El_observed = El_geometric+0.01617*(COS(Radians(fabs(El_geometric)))/SIN(Radians(fabs(El_geometric))) );
	else
	{
		El_observed = El_geometric+refraction ;
	}

	if (alfa_r_zenith < -3000)
		El_observed=-99;

	return El_observed;
}

double calcAzimuth(double SatLon, double SiteLat, double SiteLon, int Height_over_ocean)
{
	double f = 1.00 / 298.257; // Earth flattning factor
	double r_sat=42164.57; // Distance from earth centre to satellite
	double r_eq=6378.14;  // Earth radius
	double sinRadSiteLat=SIN(Radians(SiteLat));
	double cosRadSiteLat=COS(Radians(SiteLat));
	double Rstation = r_eq / ( sqrt( 1 - f*(2-f)*sinRadSiteLat*sinRadSiteLat ) );
	double Ra = (Rstation+Height_over_ocean)*cosRadSiteLat;
	double Rz = Rstation*(1-f)*(1-f)*sinRadSiteLat;
	double alfa_rx = r_sat*COS(Radians(SatLon-SiteLon)) - Ra;
	double alfa_ry = r_sat*SIN(Radians(SatLon-SiteLon));
	double alfa_rz = -Rz;
	double alfa_r_north = -alfa_rx*sinRadSiteLat + alfa_rz*cosRadSiteLat;
	double Azimuth = 0.00;

	if (alfa_r_north < 0)
		Azimuth = 180+Deg(ATAN(alfa_ry/alfa_r_north));
	else
		Azimuth = Rev(360+Deg(ATAN(alfa_ry/alfa_r_north)));

	return Azimuth;
}

double calcDeclination( double SiteLat, double Azimuth, double Elevation)
{
	return Deg( ASIN(SIN(Radians(Elevation)) *
				SIN(Radians(SiteLat)) +
				COS(Radians(Elevation)) *
				COS(Radians(SiteLat)) +
				COS(Radians(Azimuth))
				)
			);
}

double calcSatHourangle( double Azimuth, double Elevation, double Declination, double Lat )
{
	double a = - COS(Radians(Elevation)) * SIN(Radians(Azimuth));
	DOUBLE b = SIN(Radians(Elevation)) *
						 COS(Radians(Lat)) -
						 COS(Radians(Elevation)) *
						 SIN(Radians(Lat)) *
						 COS(Radians(Azimuth));

// Works for all azimuths (northern & sourhern hemisphere)
	 double returnvalue = 180 + Deg(ATAN(a/b));

	(void)Declination;

	if ( Azimuth > 270 )
	{
		returnvalue = ( (returnvalue-180) + 360 );
		if (returnvalue>360)
			returnvalue = 360 - (returnvalue-360);
  }

	if ( Azimuth < 90 )
		returnvalue = ( 180 - returnvalue );

	return returnvalue;
}

void CalculateUSALSString(HWND hDlg, BYTE * msg)
{
	int RotorCmd;
	static int gotoXTable[10] = { 0x00, 0x02, 0x03, 0x05, 0x06, 0x08, 0x0A, 0x0B, 0x0D, 0x0E };
	double SatLon = (double)v->nLastOrbital / 1000.0;///10.00,
	double SiteLat = v->nLocationLatitude / 1000.0;
	double SiteLon = v->nLocationLongitude / 1000.0;
	double azimuth;
	double elevation;
	double declination;
	double satHourAngle;

	if (v->fLocationSouth)
		SiteLat = -SiteLat;
	if (!v->fLocationWest)
		SiteLon = 360 - SiteLon;
	if (!v->fLastOrbitalWest)
		SatLon = 360 - SatLon;
		
	azimuth = calcAzimuth(SatLon, SiteLat, SiteLon, 0);
	elevation = calcElevation(SatLon, SiteLat, SiteLon, 0);
	declination = calcDeclination( SiteLat, azimuth, elevation );
	satHourAngle = calcSatHourangle( azimuth, elevation, declination, SiteLat );

	if (SiteLat >= 0)
	{
		//
		// Northern Hemisphere
		//
		int tmp=(int)round( fabs( 180 - satHourAngle ) * 10.0 );
		RotorCmd = (tmp/10)*0x10 + gotoXTable[ tmp % 10 ];

		if (satHourAngle < 180)  // the east
			RotorCmd |= 0xE000;
		else                     // west
			RotorCmd |= 0xD000;
	}
	else
	{
		//
		// Southern Hemisphere
		//
		if (satHourAngle < 180)  // the east
		{
			int tmp=(int)round( fabs( satHourAngle ) * 10.0 );
			RotorCmd = (tmp/10)*0x10 + gotoXTable[ tmp % 10 ];
			RotorCmd |= 0xD000;
		}
		else
		{                     // west
			int tmp=(int)round( fabs( 360 - satHourAngle ) * 10.0 );
			RotorCmd = (tmp/10)*0x10 + gotoXTable[ tmp % 10 ];
			RotorCmd |= 0xE000;
		}
	}

	msg[0]=0xE0;
	msg[1]=0x31;
	msg[2]=0x6E;
	msg[3]=((RotorCmd & 0xFF00) / 0x100);
	msg[4]=RotorCmd & 0xFF;

	{
		char szTemp[256];
		sprintf(szTemp, "lat=%.3f lon=%.3f sat=%.3f az=%.3f el=%.3f dec=%.3f angle=%.3f CMD=%02x%02x%02x%02x%02x",
				SiteLat, SiteLon, SatLon, azimuth, elevation, declination, satHourAngle,
				msg[0], msg[1], msg[2], msg[3], msg[4]);
		SetDlgItemText(hDlg, IDC_POSITIONER_DEBUG, szTemp);
	}

}

void GotoUSALSPosition(HWND hDlg)
{
/*    int CMD1=0x00 , CMD2=0x00;        // Bytes sent to motor
    int DecimalLookup[10] = { 0x00, 0x02, 0x03, 0x05, 0x06, 0x08, 0x0A, 0x0B, 0x0D, 0x0E };
    double USALS=0.0;
	double latitude = (double)v->nLocationLatitude / 1000.0;
	double longitude = (double)v->nLocationLongitude / 1000.0;
	double satellite_longitude = (double)v->nLastOrbital / 1000.0;
	double P, Ue, Us, az, x, el, Azimuth;

	if (v->fLocationSouth)
		latitude = -latitude;
	if (v->fLocationWest)
		longitude = 360 - longitude;
	if (v->fLastOrbitalWest)
		satellite_longitude = 360 - satellite_longitude;
	
	P = latitude * TO_RADS;           // Earth Station Latitude
	Ue = longitude * TO_RADS;           // Earth Station Longitude
	Us = satellite_longitude * TO_RADS;          // Satellite Longitude

	az = M_PI + atan( tan (Us-Ue) / sin(P) );
	x = acos( cos(Us-Ue) * cos(P) );
	el = atan( (cos(x) - 0.1513 ) /sin(x) );
	Azimuth = atan((-cos(el)*sin(az))/(sin(el)*cos(P)-cos(el)*sin(P)*cos(az)))* TO_DEC;

	if (Azimuth > 0.0)
		CMD1 = 0xE0;		// East
	else
		CMD1 = 0xD0;		// West

	USALS = fabs(Azimuth);
	while (USALS > 16)
	{
		CMD1++;
		USALS -= 16;
	}
	while (USALS >= 1.0)
	{
		CMD2 += 0x10;
		USALS--;
	}
	CMD2 += DecimalLookup[(int)round(USALS*10)];
*/	
	SendDiSEqC = (td_SendDiSEqC)GetProcAddress(v->hSource, "TSReader_SendDiSEqC");
	if (SendDiSEqC != NULL)
	{
//		BYTE bReset[] = {0xe0, 0x10, 0x00};
//		BYTE bInit[] = {0xe0, 0x10, 0x03};
//		BYTE bUSALS[] = {0xe0, 0x31, 0x6e, CMD1, CMD2};
		BYTE bUSALS[6];
		char szTemp[128];

		CursorWait(hDlg);
		//SendDiSEqC(bInit, 3);
		//Sleep(15);
		//SendDiSEqC(bReset, 3);
		//Sleep(15);
		//SendDiSEqC(bInit, 3);
		//Sleep(15);

		//wsprintf(szTemp, "SourceHelper: USALS old: %02x %02x %02x %02x %02x\n", bUSALS[0], bUSALS[1], bUSALS[2], bUSALS[3], bUSALS[4]);
		//OutputDebugString(szTemp);
		CalculateUSALSString(hDlg, bUSALS);
		wsprintf(szTemp, "SourceHelper: USALS new: %02x %02x %02x %02x %02x\n", bUSALS[0], bUSALS[1], bUSALS[2], bUSALS[3], bUSALS[4]);
		OutputDebugString(szTemp);
		SendDiSEqC(bUSALS, 5);
		Sleep(15);

		bUSALS[0] = 0xe1;	// Committed?
		SendDiSEqC(bUSALS, 5);
		Sleep(100);

		bUSALS[0] = 0xe0;	// Repeat
		SendDiSEqC(bUSALS, 5);
		Sleep(15);

		CursorNormal();
	}
}

void GetUSALSParameters(HWND hDlg)
{
	v->nLocationLatitude = (int)(GetDlgItemDouble(hDlg, IDC_POSITIONER_LAT) * 1000.0);
	v->nLocationLongitude = (int)(GetDlgItemDouble(hDlg, IDC_POSITIONER_LON) * 1000.0);
	v->nLastOrbital = (int)(GetDlgItemDouble(hDlg, IDC_POSITIONER_ORBITAL_LOCATION) * 1000.0);
	v->fLocationSouth = IsDlgButtonChecked(hDlg, IDC_POSITIONER_SOUTH);
	v->fLocationWest = IsDlgButtonChecked(hDlg, IDC_POSITIONER_WEST);
	v->fLastOrbitalWest = IsDlgButtonChecked(hDlg, IDC_POSITIONER_SAT_WEST);
}

BOOL CALLBACK PositionerDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static BOOL fMoving;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			BYTE bCommand[] = {0xE0, 0x31, 0x03}; // power on command

			SendDiSEqC = (td_SendDiSEqC)GetProcAddress(v->hSource, "TSReader_SendDiSEqC");
			if (SendDiSEqC == NULL)
			{
				MessageBox(hDlg, "The currently selected source doesn't support DiSEqC functions - you shouldn't actually see this error!", gszAppName, MB_ICONSTOP);
				EndDialog(hDlg, FALSE);
			}
			fMoving = FALSE;
			SetTimer(hDlg, 1, 1000, NULL);
			SendDiSEqC(bCommand, sizeof(bCommand));
			
			if (v->nDiSEqCPosition != -1)
			{
				if (v->fDiSEqCPostionIsUSALS)
				{
					v->nLastOrbital = v->nDiSEqCPosition & (~0x40000000);
					if (v->nDiSEqCPosition & 0x40000000)
						v->fLastOrbitalWest = TRUE;
					PostMessage(hDlg, WM_COMMAND, IDC_POSITIONER_GOTO_USALS, 0);
				}
				else
				{
					SetDlgItemInt(hDlg, IDC_SAT_POSITION, v->nDiSEqCPosition, FALSE);
					PostMessage(hDlg, WM_COMMAND, IDC_GOTO_POSITION, 0);
				}
			}

			SetDlgItemDouble(hDlg, IDC_POSITIONER_LAT, (double)v->nLocationLatitude / 1000.0);
			if (v->fLocationSouth)
				CheckDlgButton(hDlg, IDC_POSITIONER_SOUTH, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_POSITIONER_NORTH, BST_CHECKED);
			SetDlgItemDouble(hDlg, IDC_POSITIONER_LON, (double)v->nLocationLongitude / 1000.0);
			if (v->fLocationWest)
				CheckDlgButton(hDlg, IDC_POSITIONER_WEST, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_POSITIONER_EAST, BST_CHECKED);
			SetDlgItemDouble(hDlg, IDC_POSITIONER_ORBITAL_LOCATION, (double)v->nLastOrbital / 1000.0);
			if (v->fLastOrbitalWest)
				CheckDlgButton(hDlg, IDC_POSITIONER_SAT_WEST, BST_CHECKED);
			else
				CheckDlgButton(hDlg, IDC_POSITIONER_SAT_EAST, BST_CHECKED);
			SetDlgItemText(hDlg, IDC_POSITIONER_DEBUG, "");
			SetFocus(GetDlgItem(hDlg, IDCANCEL));
		}
		break;
	case WM_DESTROY:
		if (fMoving == TRUE)
		{
			BYTE bCommand[] = {0xE0, 0x31, 0x60};
			SendDiSEqC(bCommand, sizeof(bCommand));
		}
		KillTimer(hDlg, 1);
		v->nDiSEqCPosition = -1;
		GetUSALSParameters(hDlg);
		break;
	case WM_TIMER:
		if (v->nDiSEqCPosition != -1)		// only set when we come from the command-line
		{
			if (v->nDiSEqCDelay == 0)
				EndDialog(hDlg, FALSE);
			else
			{
				char szTemp[128];
				wsprintf(szTemp, "Positioner delay - %d second(s) left", v->nDiSEqCDelay);
				SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)szTemp);
				v->nDiSEqCDelay--;
			}
		}
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
		case IDC_POSITIONER_DISEQC1:
		case IDC_POSITIONER_DISEQC2:
		case IDC_POSITIONER_DISEQC3:
		case IDC_POSITIONER_DISEQC4:
			{
				BYTE bCommand[4] = {0xE0, 0x10, 0x38, 0xff};

				if (LOWORD(wParam) == IDC_POSITIONER_DISEQC1) bCommand[3] = 0xc0;
				else if (LOWORD(wParam) == IDC_POSITIONER_DISEQC2) bCommand[3] = 0xc4;
				else if (LOWORD(wParam) == IDC_POSITIONER_DISEQC3) bCommand[3] = 0xc8;
				else if (LOWORD(wParam) == IDC_POSITIONER_DISEQC4) bCommand[3] = 0xcc;

				SendDiSEqC(bCommand, sizeof(bCommand));
			}
			break;
		case IDC_BUMP_WEST:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			{
				BYTE bCommand[] = {0xE0, 0x31, 0x69, 0xff};
				SendDiSEqC(bCommand, sizeof(bCommand));
			}
			break;
		case IDC_BUMP_EAST:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			{
				BYTE bCommand[] = {0xE0, 0x31, 0x68, 0xff};
				SendDiSEqC(bCommand, sizeof(bCommand));
			}
			break;
		case IDC_MOVE_EAST:
		case IDC_MOVE_WEST:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			else
			{
				if (LOWORD(wParam) == IDC_MOVE_EAST)
				{
					BYTE bCommand[] = {0xE0, 0x31, 0x68, 0x00};
					SendDiSEqC(bCommand, sizeof(bCommand));
				}
				else
				{
					BYTE bCommand[] = {0xE0, 0x31, 0x69, 0x00};
					SendDiSEqC(bCommand, sizeof(bCommand));
				}
				fMoving = TRUE;
				SetDlgItemText(hDlg, IDC_MOVE_EAST, "Stop");
				SetDlgItemText(hDlg, IDC_MOVE_WEST, "Stop");
			}
			break;
		case IDC_GOTO_POSITION:
		case IDC_STORE_POSITION:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			{
				int nPosition = GetDlgItemInt(hDlg, IDC_SAT_POSITION, NULL, FALSE);
				if (nPosition < 0 || nPosition > 255)
					MessageBox(hDlg, "Invalid satellite position", gszAppName, MB_ICONSTOP);
				else
				{
					if (LOWORD(wParam) == IDC_GOTO_POSITION)
					{
						BYTE bCommand[] = {0xE0, 0x31, 0x6B, 0x00};
						bCommand[3] = nPosition;
						SendDiSEqC(bCommand, sizeof(bCommand));
					}
					else
					{
						BYTE bCommand[] = {0xE0, 0x31, 0x6A, 0x00};
						bCommand[3] = nPosition;
						SendDiSEqC(bCommand, sizeof(bCommand));
					}
				}
			}
			break;
		case IDC_SET_EAST_LIMIT:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			{
				BYTE bCommand[] = {0xE0, 0x31, 0x66};
				SendDiSEqC(bCommand, sizeof(bCommand));
			}
			break;
		case IDC_SET_WEST_LIMIT:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			{
				BYTE bCommand[] = {0xE0, 0x31, 0x67};
				SendDiSEqC(bCommand, sizeof(bCommand));
			}
			break;
		case IDC_DISABLE_LIMITS:
			if (fMoving == TRUE)
			{
				StopMoving(hDlg);
				fMoving = FALSE;
			}
			{
				BYTE bCommand[] = {0xE0, 0x31, 0x63};
				SendDiSEqC(bCommand, sizeof(bCommand));
			}
			break;
		case IDC_POSITIONER_GOTO_USALS:
			GetUSALSParameters(hDlg);
			GotoUSALSPosition(hDlg);
			v->nDiSEqCPosition = -1;
			break;
		}
		break;
	}

	return FALSE;
}

void EnableOrDisableHiLOAndSwitchFreq(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, IDC_LNB_B), spcopy[v->nCurrentEditSwitch].nLNBType != LNB_TYPE_SINGLE);
	EnableWindow(GetDlgItem(hDlg, IDC_LNB_SWITCH_FREQ), spcopy[v->nCurrentEditSwitch].nLNBType == LNB_TYPE_DUAL);
	EnableWindow(GetDlgItem(hDlg, IDC_VOLT_BAND), spcopy[v->nCurrentEditSwitch].nLNBType == LNB_TYPE_DUAL);
	EnableWindow(GetDlgItem(hDlg, IDC_TONE_BAND), spcopy[v->nCurrentEditSwitch].nLNBType == LNB_TYPE_DUAL);

	switch(spcopy[v->nCurrentEditSwitch].nLNBType)
	{
	case LNB_TYPE_SINGLE:
		SetDlgItemText(hDlg, IDC_LNB_A_CAPTION, "LNB freq");
		SetDlgItemText(hDlg, IDC_LNB_B_CAPTION, "");
		break;
	case LNB_TYPE_DUAL:
		SetDlgItemText(hDlg, IDC_LNB_A_CAPTION, "LNB low freq");
		SetDlgItemText(hDlg, IDC_LNB_B_CAPTION, "high");
		break;
	case LNB_TYPE_STACKED:
		SetDlgItemText(hDlg, IDC_LNB_A_CAPTION, "LNB H/L freq");
		SetDlgItemText(hDlg, IDC_LNB_B_CAPTION, "V/R");
		break;
	}
}

void EnableOrDisableAutoSelectRange(HWND hDlg)
{
	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_FREQUENCY), spcopy[v->nCurrentEditSwitch].nAutoSelect);
	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_ORBITAL), spcopy[v->nCurrentEditSwitch].nAutoSelect);
	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_NETWORK), spcopy[v->nCurrentEditSwitch].nAutoSelect);

	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_START), spcopy[v->nCurrentEditSwitch].nAutoSelect == AUTO_SELECT_LNB_FREQ);
	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_END), spcopy[v->nCurrentEditSwitch].nAutoSelect  == AUTO_SELECT_LNB_FREQ);

	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_ORBITAL_DEG), spcopy[v->nCurrentEditSwitch].nAutoSelect  == AUTO_SELECT_LNB_ORBITAL);
	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_ORBITAL_EAST), spcopy[v->nCurrentEditSwitch].nAutoSelect  == AUTO_SELECT_LNB_ORBITAL);
	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_ORBITAL_WEST), spcopy[v->nCurrentEditSwitch].nAutoSelect  == AUTO_SELECT_LNB_ORBITAL);

	EnableWindow(GetDlgItem(hDlg, IDC_AUTO_SELECT_NETWORK_ID), spcopy[v->nCurrentEditSwitch].nAutoSelect  == AUTO_SELECT_LNB_NETWORK);
}

int Orbital3600toOrbital1800(int nOrbital3600, BOOL *fWest)
{
	if (nOrbital3600 > 1800)
	{
		*fWest = TRUE;
		return 3600 - nOrbital3600;
	}
	*fWest = FALSE;
	return nOrbital3600;
}

void SetSwitchDialogValues(HWND hDlg)
{
	BOOL fWest;
	double dOrbital;
	char szTemp[64];

	SendDlgItemMessage(hDlg, IDC_LNB_TYPE, CB_SETCURSEL, spcopy[v->nCurrentEditSwitch].nLNBType, 0);
	EnableOrDisableHiLOAndSwitchFreq(hDlg);
	SetDlgItemInt(hDlg, IDC_LNB_A, spcopy[v->nCurrentEditSwitch].nLOFrequencyLow, FALSE);
	SetDlgItemInt(hDlg, IDC_LNB_B, spcopy[v->nCurrentEditSwitch].nLOFrequencyHigh, FALSE);
	SetDlgItemInt(hDlg, IDC_LNB_SWITCH_FREQ, spcopy[v->nCurrentEditSwitch].nSwitchFrequency, FALSE);

	CheckDlgButton(hDlg, IDC_VOLT_POL, spcopy[v->nCurrentEditSwitch].nVoltage == LNB_VOLTAGE_POLARITY);
	CheckDlgButton(hDlg, IDC_VOLT_BAND, spcopy[v->nCurrentEditSwitch].nVoltage == LNB_VOLTAGE_BAND);
	CheckDlgButton(hDlg, IDC_VOLT_OFF, spcopy[v->nCurrentEditSwitch].nVoltage == LNB_VOLTAGE_OFF);
	CheckDlgButton(hDlg, IDC_VOLT_14V, spcopy[v->nCurrentEditSwitch].nVoltage == LNB_VOLTAGE_14V);
	CheckDlgButton(hDlg, IDC_VOLT_18V, spcopy[v->nCurrentEditSwitch].nVoltage == LNB_VOLTAGE_18V);
	
	CheckDlgButton(hDlg, IDC_TONE_BAND, spcopy[v->nCurrentEditSwitch].n22KHzTone == LNB_22KHZ_BAND);
	CheckDlgButton(hDlg, IDC_TONE_ON, spcopy[v->nCurrentEditSwitch].n22KHzTone == LNB_22KHZ_ON);
	CheckDlgButton(hDlg, IDC_TONE_OFF, spcopy[v->nCurrentEditSwitch].n22KHzTone == LNB_22KHZ_OFF);

	CheckDlgButton(hDlg, IDC_POSITIONER_NONE, spcopy[v->nCurrentEditSwitch].nPositionerType == LNB_POSITIONER_NONE);
	CheckDlgButton(hDlg, IDC_POSITIONER_DISEQC, spcopy[v->nCurrentEditSwitch].nPositionerType == LNB_POSITIONER_DISEQC);

	CheckDlgButton(hDlg, IDC_AUTO_SELECT, spcopy[v->nCurrentEditSwitch].nAutoSelect);
	CheckDlgButton(hDlg, IDC_AUTO_SELECT_FREQUENCY, spcopy[v->nCurrentEditSwitch].nAutoSelect == AUTO_SELECT_LNB_FREQ || !spcopy[v->nCurrentEditSwitch].nAutoSelect);
	CheckDlgButton(hDlg, IDC_AUTO_SELECT_ORBITAL, spcopy[v->nCurrentEditSwitch].nAutoSelect == AUTO_SELECT_LNB_ORBITAL);
	CheckDlgButton(hDlg, IDC_AUTO_SELECT_NETWORK, spcopy[v->nCurrentEditSwitch].nAutoSelect == AUTO_SELECT_LNB_NETWORK);
	SetDlgItemInt(hDlg, IDC_AUTO_SELECT_START, spcopy[v->nCurrentEditSwitch].nAutoSelectFreqStart, FALSE);
	SetDlgItemInt(hDlg, IDC_AUTO_SELECT_END, spcopy[v->nCurrentEditSwitch].nAutoSelectFreqEnd, FALSE);
	SetDlgItemInt(hDlg, IDC_AUTO_SELECT_NETWORK_ID, spcopy[v->nCurrentEditSwitch].nAutoSelectNetwork, FALSE);
	
	dOrbital = (double)(Orbital3600toOrbital1800(spcopy[v->nCurrentEditSwitch].nAutoSelectOrbital, &fWest));
	sprintf(szTemp, "%.1f", dOrbital / 10.0);
	SetDlgItemText(hDlg, IDC_AUTO_SELECT_ORBITAL_DEG, szTemp);
	CheckDlgButton(hDlg, IDC_AUTO_SELECT_ORBITAL_WEST, fWest);
	CheckDlgButton(hDlg, IDC_AUTO_SELECT_ORBITAL_EAST, !fWest);
	
	CheckDlgButton(hDlg, IDC_INPUT_POLARITY_VARIABLE, spcopy[v->nCurrentEditSwitch].nAutoSelectPolarity == 0);
	CheckDlgButton(hDlg, IDC_INPUT_POLARITY_FIXEDV, spcopy[v->nCurrentEditSwitch].nAutoSelectPolarity == 1);
	CheckDlgButton(hDlg, IDC_INPUT_POLARITY_FIXEDH, spcopy[v->nCurrentEditSwitch].nAutoSelectPolarity == 2);
	
	EnableOrDisableAutoSelectRange(hDlg);
}

void GetSwitchDialogValues(HWND hDlg)
{
	char * szDecimalPtr;
	char szTemp[64];

	spcopy[v->nCurrentEditSwitch].nLNBType = SendDlgItemMessage(hDlg, IDC_LNB_TYPE, CB_GETCURSEL, 0, 0);
	spcopy[v->nCurrentEditSwitch].nLOFrequencyLow = GetDlgItemInt(hDlg, IDC_LNB_A, NULL, FALSE);
	spcopy[v->nCurrentEditSwitch].nLOFrequencyHigh = GetDlgItemInt(hDlg, IDC_LNB_B, NULL, FALSE);
	spcopy[v->nCurrentEditSwitch].nSwitchFrequency = GetDlgItemInt(hDlg, IDC_LNB_SWITCH_FREQ, NULL, FALSE);

	if (IsDlgButtonChecked(hDlg, IDC_VOLT_POL))  spcopy[v->nCurrentEditSwitch].nVoltage = LNB_VOLTAGE_POLARITY;
	if (IsDlgButtonChecked(hDlg, IDC_VOLT_BAND)) spcopy[v->nCurrentEditSwitch].nVoltage = LNB_VOLTAGE_BAND;
	if (IsDlgButtonChecked(hDlg, IDC_VOLT_OFF))  spcopy[v->nCurrentEditSwitch].nVoltage = LNB_VOLTAGE_OFF;
	if (IsDlgButtonChecked(hDlg, IDC_VOLT_14V))  spcopy[v->nCurrentEditSwitch].nVoltage = LNB_VOLTAGE_14V;
	if (IsDlgButtonChecked(hDlg, IDC_VOLT_18V))  spcopy[v->nCurrentEditSwitch].nVoltage = LNB_VOLTAGE_18V;

	if (IsDlgButtonChecked(hDlg, IDC_TONE_BAND)) spcopy[v->nCurrentEditSwitch].n22KHzTone = LNB_22KHZ_BAND;
	if (IsDlgButtonChecked(hDlg, IDC_TONE_ON))   spcopy[v->nCurrentEditSwitch].n22KHzTone = LNB_22KHZ_ON;
	if (IsDlgButtonChecked(hDlg, IDC_TONE_OFF))  spcopy[v->nCurrentEditSwitch].n22KHzTone = LNB_22KHZ_OFF;

	if (IsDlgButtonChecked(hDlg, IDC_POSITIONER_NONE))   spcopy[v->nCurrentEditSwitch].nPositionerType = LNB_POSITIONER_NONE;
	if (IsDlgButtonChecked(hDlg, IDC_POSITIONER_DISEQC)) spcopy[v->nCurrentEditSwitch].nPositionerType = LNB_POSITIONER_DISEQC;

	spcopy[v->nCurrentEditSwitch].nAutoSelect = IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT);
	if (spcopy[v->nCurrentEditSwitch].nAutoSelect)
	{
		if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_FREQUENCY)) 
			spcopy[v->nCurrentEditSwitch].nAutoSelect = AUTO_SELECT_LNB_FREQ;
		else if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_ORBITAL)) 
			spcopy[v->nCurrentEditSwitch].nAutoSelect = AUTO_SELECT_LNB_ORBITAL;
		else if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_NETWORK)) 
			spcopy[v->nCurrentEditSwitch].nAutoSelect = AUTO_SELECT_LNB_NETWORK;
	}
	spcopy[v->nCurrentEditSwitch].nAutoSelectFreqStart = GetDlgItemInt(hDlg, IDC_AUTO_SELECT_START, NULL, FALSE);
	spcopy[v->nCurrentEditSwitch].nAutoSelectFreqEnd = GetDlgItemInt(hDlg, IDC_AUTO_SELECT_END, NULL, FALSE);
	spcopy[v->nCurrentEditSwitch].nAutoSelectNetwork = GetDlgItemInt(hDlg, IDC_AUTO_SELECT_NETWORK_ID, NULL, FALSE);

	GetDlgItemText(hDlg, IDC_AUTO_SELECT_ORBITAL_DEG, szTemp, sizeof(szTemp));
	szDecimalPtr = strstr(szTemp, ".");
	if (szDecimalPtr != NULL)
		*szDecimalPtr = '\0';
	sscanf(szTemp, "%d", &spcopy[v->nCurrentEditSwitch].nAutoSelectOrbital);
	spcopy[v->nCurrentEditSwitch].nAutoSelectOrbital *= 10;
	if (szDecimalPtr != NULL)
		spcopy[v->nCurrentEditSwitch].nAutoSelectOrbital += (int)(*(szDecimalPtr + 1) - '0');
	if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_ORBITAL_WEST))
		spcopy[v->nCurrentEditSwitch].nAutoSelectOrbital = 3600 - spcopy[v->nCurrentEditSwitch].nAutoSelectOrbital;

	if (IsDlgButtonChecked(hDlg, IDC_INPUT_POLARITY_VARIABLE))	spcopy[v->nCurrentEditSwitch].nAutoSelectPolarity = 0;
	if (IsDlgButtonChecked(hDlg, IDC_INPUT_POLARITY_FIXEDV))	spcopy[v->nCurrentEditSwitch].nAutoSelectPolarity = 1;
	if (IsDlgButtonChecked(hDlg, IDC_INPUT_POLARITY_FIXEDH))	spcopy[v->nCurrentEditSwitch].nAutoSelectPolarity = 2;
}

void LoadDishSwitchCombo(HWND hDlg, int nID)
{
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW21 Dish 1");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW21 Dish 2");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW42 Dish 1");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW42 Dish 2");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW44 Dish 2");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW64 Dish 1A");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW64 Dish 1B");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW64 Dish 2A");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW64 Dish 2B");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW64 Dish 3A");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"SW64 Dish 3B");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"Twin LNB 1");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"Twin LNB 2");
	SendDlgItemMessage(hDlg, nID, CB_ADDSTRING, 0, (LPARAM)"Quad LNB 2");
}

BOOL CALLBACK InputSetupDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HWND hDlgParent = GetParent(hDlg);

			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_ADDSTRING, 0, (LPARAM)"DiSEqC Port 1");
			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_ADDSTRING, 0, (LPARAM)"DiSEqC Port 2");
			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_ADDSTRING, 0, (LPARAM)"DiSEqC Port 3");
			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_ADDSTRING, 0, (LPARAM)"DiSEqC Port 4");
			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_ADDSTRING, 0, (LPARAM)"Toneburst A");
			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_ADDSTRING, 0, (LPARAM)"Toneburst B");
			LoadDishSwitchCombo(hDlg, IDC_INPUT_SETUP_COMBO);

			// this order MUST match the LNB_TYPE enum in tsreader.h
			SendDlgItemMessage(hDlg, IDC_LNB_TYPE, CB_ADDSTRING, 0, (LPARAM)"Single band");
			SendDlgItemMessage(hDlg, IDC_LNB_TYPE, CB_ADDSTRING, 0, (LPARAM)"Multi band");
			SendDlgItemMessage(hDlg, IDC_LNB_TYPE, CB_ADDSTRING, 0, (LPARAM)"Stacked");

			
			if (IsDlgButtonChecked(hDlgParent, IDC_DISEQC1))
				v->nCurrentEditSwitch = 0;
			else if (IsDlgButtonChecked(hDlgParent, IDC_DISEQC2))
				v->nCurrentEditSwitch = 1;
			else if (IsDlgButtonChecked(hDlgParent, IDC_DISEQC3))
				v->nCurrentEditSwitch = 2;
			else if (IsDlgButtonChecked(hDlgParent, IDC_DISEQC4))
				v->nCurrentEditSwitch = 3;
			else if (IsDlgButtonChecked(hDlgParent, IDC_INPUT_TB_A))
				v->nCurrentEditSwitch = 4;
			else if (IsDlgButtonChecked(hDlgParent, IDC_INPUT_TB_B))
				v->nCurrentEditSwitch = 5;
			else if (IsDlgButtonChecked(hDlgParent, IDC_INPUT_DISH))
			{
				v->nCurrentEditSwitch = SendDlgItemMessage(hDlgParent, IDC_DISH_COMBO, CB_GETCURSEL, 0, 0) + 6;
			}
			else
				v->nCurrentEditSwitch = 0;
			SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_SETCURSEL, v->nCurrentEditSwitch, 0);
			memcpy(spcopy, v->sp, sizeof(v->sp));
			SetSwitchDialogValues(hDlg);
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
		case CBN_SELCHANGE:
			if ((HWND)lParam == GetDlgItem(hDlg, IDC_LNB_TYPE))
			{
				spcopy[v->nCurrentEditSwitch].nLNBType = SendDlgItemMessage(hDlg, IDC_LNB_TYPE, CB_GETCURSEL, 0, 0);
				EnableOrDisableHiLOAndSwitchFreq(hDlg);
			}
			else
			{
				GetSwitchDialogValues(hDlg);
				v->nCurrentEditSwitch = SendDlgItemMessage(hDlg, IDC_INPUT_SETUP_COMBO, CB_GETCURSEL, 0, 0);
				SetSwitchDialogValues(hDlg);
			}
			break;
		case BN_CLICKED:
			switch(LOWORD(wParam))
			{
			case IDOK:
				GetSwitchDialogValues(hDlg);
				memcpy(v->sp, spcopy, sizeof(v->sp));
				EndDialog(hDlg, TRUE);
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			case IDC_AUTO_SELECT:
			case IDC_AUTO_SELECT_FREQUENCY:
			case IDC_AUTO_SELECT_ORBITAL:
			case IDC_AUTO_SELECT_NETWORK:
				spcopy[v->nCurrentEditSwitch].nAutoSelect = IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT);
				if (spcopy[v->nCurrentEditSwitch].nAutoSelect)
				{
					if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_FREQUENCY)) 
						spcopy[v->nCurrentEditSwitch].nAutoSelect = AUTO_SELECT_LNB_FREQ;
					else if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_ORBITAL)) 
						spcopy[v->nCurrentEditSwitch].nAutoSelect = AUTO_SELECT_LNB_ORBITAL;
					else if (IsDlgButtonChecked(hDlg, IDC_AUTO_SELECT_NETWORK)) 
						spcopy[v->nCurrentEditSwitch].nAutoSelect = AUTO_SELECT_LNB_NETWORK;
				}
				EnableOrDisableAutoSelectRange(hDlg);
				break;
			}
			break;
		}
		break;
	}

	return FALSE;
}

void EnableOrDisableInputs(HWND hDlg)
{
	if (    ((v->dwSourceCapabilities & CAPABILITIES_DISEQC) == 0) 
		 && ((v->dwSourceCapabilities && CAPABILITIES_SERIAL_CONTROL) == 0) )
	{
		EnableWindow(GetDlgItem(hDlg, IDC_DISEQC1), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_DISEQC2), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_DISEQC3), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_DISEQC4), FALSE);
	}
	if ((v->dwSourceCapabilities & CAPABILITIES_DISEQC_POSITIONER) == 0)
		EnableWindow(GetDlgItem(hDlg, IDC_POSITIONER), FALSE);		

	if ((v->dwSourceCapabilities & CAPABILITIES_TONEBURST) == 0)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_INPUT_TB_A), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_INPUT_TB_B), FALSE);
	}
	if ((v->dwSourceCapabilities & CAPABILITIES_DISH_SWITCH) == 0)
	{
		EnableWindow(GetDlgItem(hDlg, IDC_INPUT_DISH), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_DISH_COMBO), FALSE);
	}
}

BOOL SourceHelper_DiSEqCPositionerDialog(HWND hDlg)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_POSITIONER), hDlg, PositionerDlgProc);
	return TRUE;
}

void SetupUDPListControl(HWND hDlg)
{
	int nColumnPosition = 0;
	HWND hWndMuxList = GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED);
	char szTemp[128];
	LV_COLUMN lvc; 

	// Initialize the LV_COLUMN structure. 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	lvc.pszText = szTemp; 

	if (v->nSocketDialogMode == SOCKET_MODE_UDP_MULTICAST || v->nSocketDialogMode == SOCKET_MODE_TCP)
	{
		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 75; 
		lstrcpy(szTemp, TEXT("Address"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 
	}

	lvc.fmt = LVCFMT_CENTER; 
	lvc.cx = 45; 
	lstrcpy(szTemp, TEXT("Port"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	switch(v->nSocketDialogMode)
	{
	case SOCKET_MODE_UDP_MULTICAST:
		lvc.fmt = LVCFMT_CENTER; 
		lvc.cx = 75; 
		lstrcpy(szTemp, TEXT("Interface"));
		ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 
		lvc.cx = 150; 
		break;
	case SOCKET_MODE_UDP_UNICAST:
		lvc.cx = 300; 
		break;
	case SOCKET_MODE_TCP:
		lvc.cx = 220; 
		break;
	}
	lvc.fmt = LVCFMT_LEFT; 
	lstrcpy(szTemp, TEXT("Description"));
	ListView_InsertColumn(hWndMuxList, nColumnPosition++, &lvc); 

	ListView_SetExtendedListViewStyle(hWndMuxList, LVS_EX_FULLROWSELECT);
}

void LoadUDPList(HWND hDlg)
{
	int i;
	HANDLE hList;
    char szListFile[MAX_PATH];

	v->nUDPListEntries = 0;

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
	switch(v->nSocketDialogMode)
	{
	case SOCKET_MODE_UDP_MULTICAST:
		lstrcat(szListFile, "\\UDPMulticast.lst");
		break;
	case SOCKET_MODE_TCP:
		lstrcat(szListFile, "\\TCP.lst");
		break;
	case SOCKET_MODE_UDP_UNICAST:
		lstrcat(szListFile, "\\UDPUnicast.lst");
		break;
	}

	hList = CreateFile(szListFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hList != INVALID_HANDLE_VALUE)
	{
		char szLine[256];
		char szCommaValue[32][128];

		while (SourceHelper_ReadLine(hList, szLine, sizeof(szLine)))
		{
			char * szCurrent = szLine;
			int nItem = 0;
			char * szNextTab;

			do
			{
				szNextTab = strstr(szCurrent, "\t");
				if (szNextTab != NULL)
					*szNextTab = '\0';
				lstrcpy(szCommaValue[nItem++], szCurrent);
				szCurrent = szNextTab + 1;
			} while (szNextTab != NULL);

			if (nItem)
			{
				switch(v->nSocketDialogMode)
				{
				case SOCKET_MODE_UDP_MULTICAST:
					lstrcpy(pUDPList[v->nUDPListEntries].szAddress, szCommaValue[0]);
					sscanf(szCommaValue[1], "%d", &pUDPList[v->nUDPListEntries].nPort);
					lstrcpy(pUDPList[v->nUDPListEntries].szInterfaceAddress, szCommaValue[2]);
					lstrcpy(pUDPList[v->nUDPListEntries].szDescription, szCommaValue[3]);
					break;
				case SOCKET_MODE_UDP_UNICAST:
					sscanf(szCommaValue[0], "%d", &pUDPList[v->nUDPListEntries].nPort);
					lstrcpy(pUDPList[v->nUDPListEntries].szDescription, szCommaValue[1]);
					break;
				case SOCKET_MODE_TCP:
					lstrcpy(pUDPList[v->nUDPListEntries].szAddress, szCommaValue[0]);
					sscanf(szCommaValue[1], "%d", &pUDPList[v->nUDPListEntries].nPort);
					lstrcpy(pUDPList[v->nUDPListEntries].szDescription, szCommaValue[2]);
					break;
				}
				v->nUDPListEntries++;
				if (v->nUDPListEntries == MAX_UDP_LIST_ENTRIES)
				{
					MessageBox(hDlg, "Out of space for TCP/UDP list entries - tell rod@coolstf.com this happened!", gszAppName, MB_ICONSTOP);
					break;
				}
			}
		}
		CloseHandle(hList);
	}

	if (v->nUDPListEntries)
	{
		HWND hWndMuxList = GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED);
		for (i = 0; i < v->nUDPListEntries; i++)
		{
			// Initialize item-specific LV_ITEM members. 
			LV_ITEM lvi; 
		
			memset(&lvi, 0, sizeof(lvi));
			lvi.state = 0; 
			lvi.stateMask = 0; 
			lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
			lvi.mask = LVIF_TEXT | LVIF_PARAM | LVIF_STATE; 
			lvi.iItem = i; 
			lvi.iSubItem = 0; 
			lvi.lParam = (LPARAM) 0;    // item data 
			ListView_InsertItem(hWndMuxList, &lvi);
		}
	}

}

void SaveUDPList(HWND hDlg)
{
	int i;
	DWORD dwWritten;
	HANDLE hList;
    char szListFile[MAX_PATH];
	char szTemp[MAX_PATH];
	char szDescription[128];

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szListFile, sizeof(szListFile));
	switch(v->nSocketDialogMode)
	{
	case SOCKET_MODE_UDP_MULTICAST:
		lstrcat(szListFile, "\\UDPMulticast.lst");
		break;
	case SOCKET_MODE_UDP_UNICAST:
		lstrcat(szListFile, "\\UDPUnicast.lst");
		break;
	case SOCKET_MODE_TCP:
		lstrcat(szListFile, "\\TCP.lst");
		break;
	}

	hList = CreateFile(szListFile, GENERIC_WRITE, 0, (LPSECURITY_ATTRIBUTES) NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	GetDlgItemText(hDlg, IDC_UDP_DESCRIPTION, szDescription, sizeof(szDescription));
	switch(v->nSocketDialogMode)
	{
	case SOCKET_MODE_UDP_MULTICAST:
		wsprintf(szTemp, "%s\t%d\t%s\t%s\r\n", v->ss.szUDPMulticastAddress, v->ss.nUDPMulticastPort, v->ss.szUDPMulticastInterface, szDescription);
		break;
	case SOCKET_MODE_UDP_UNICAST:
		wsprintf(szTemp, "%d\t%s\r\n", v->ss.nUDPMulticastPort, szDescription);
		break;
	case SOCKET_MODE_TCP:
		wsprintf(szTemp, "%s\t%d\t%s\r\n", v->ss.szUDPMulticastAddress, v->ss.nUDPMulticastPort, szDescription);
		break;
	}
	WriteFile(hList, szTemp, lstrlen(szTemp), &dwWritten, NULL);

	for (i = 0; i < v->nUDPListEntries; i++)
	{
		switch(v->nSocketDialogMode)
		{
		case SOCKET_MODE_UDP_MULTICAST:
			if  (  (strcmp(v->ss.szUDPMulticastAddress, pUDPList[i].szAddress) != 0)
				|| (v->ss.nUDPMulticastPort != pUDPList[i].nPort)
				|| (strcmp(v->ss.szUDPMulticastInterface, pUDPList[i].szInterfaceAddress) != 0) )
			{
				wsprintf(szTemp, "%s\t%d\t%s\t%s\r\n", pUDPList[i].szAddress, pUDPList[i].nPort, pUDPList[i].szInterfaceAddress, pUDPList[i].szDescription);
				WriteFile(hList, szTemp, lstrlen(szTemp), &dwWritten, NULL);
			}
			break;
		case SOCKET_MODE_UDP_UNICAST:
			if (v->ss.nUDPMulticastPort != pUDPList[i].nPort)
			{
				wsprintf(szTemp, "%d\t%s\r\n", pUDPList[i].nPort, pUDPList[i].szDescription);
				WriteFile(hList, szTemp, lstrlen(szTemp), &dwWritten, NULL);
			}
			break;
		case SOCKET_MODE_TCP:
			if  (  (strcmp(v->ss.szUDPMulticastAddress, pUDPList[i].szAddress) != 0)
				|| (v->ss.nUDPMulticastPort != pUDPList[i].nPort) )
			{
				wsprintf(szTemp, "%s\t%d\t%s\r\n", pUDPList[i].szAddress, pUDPList[i].nPort, pUDPList[i].szDescription);
				WriteFile(hList, szTemp, lstrlen(szTemp), &dwWritten, NULL);
			}
			break;
		}
	}

	CloseHandle(hList);
}

void GetUDPItemDispInfo(LV_DISPINFO *pnmv) 
{
    // Provide the item or subitem's text, if requested. 
    if (pnmv->item.mask & LVIF_TEXT)
	{ 
        int nMuxIndex = (int)(pnmv->item.iItem);
		int nSubItem = pnmv->item.iSubItem;

		switch(v->nSocketDialogMode)
		{
		case SOCKET_MODE_UDP_UNICAST:
			switch(nSubItem)
			{
			case 0:
				nSubItem = 1;
				break;
			case 1:
				nSubItem = 3;
				break;
			}
			break;
		case SOCKET_MODE_TCP:
			if (nSubItem == 2)
				nSubItem = 3;
			break;
		}

		switch(nSubItem)
		{
		case 0:
			lstrcpy(pnmv->item.pszText, pUDPList[nMuxIndex].szAddress);
			break;
		case 1:
			wsprintf(pnmv->item.pszText, "%d", pUDPList[nMuxIndex].nPort);
			break;
		case 2:
			lstrcpy(pnmv->item.pszText, pUDPList[nMuxIndex].szInterfaceAddress);
			break;
		case 3:
			lstrcpy(pnmv->item.pszText, pUDPList[nMuxIndex].szDescription);
			break;
		}
	}
}

void UpdateUDPItem(HWND hDlg, int nItem)
{
	v->nSelectedUDPEntry = nItem;
	if (!v->nSocketDialogMode)
	{
		int nComboItem = 0;

		SetDlgItemText(hDlg, IDC_UDP_MULTICAST_ADDRESS, pUDPList[nItem].szAddress);
		while (TRUE)
		{			
			char szTemp[128];
			int nRetVal = SendDlgItemMessage(hDlg, IDC_INTERFACE_ADDRESS, CB_GETLBTEXT, nComboItem, (LPARAM)szTemp);
			if (nRetVal == CB_ERR)
				break;
			if (lstrcmp(szTemp, pUDPList[nItem].szInterfaceAddress) == 0)
			{
				SendDlgItemMessage(hDlg, IDC_INTERFACE_ADDRESS, CB_SETCURSEL, nComboItem, 0);
				break;
			}
			nComboItem++;
		}
		//SetDlgItemText(hDlg, IDC_UDP_MULTICAST_INTERFACE, pUDPList[nItem].szInterfaceAddress);
	}
	SetDlgItemInt(hDlg, IDC_UDP_MULTICAST_PORT, pUDPList[nItem].nPort, FALSE);
	SetDlgItemText(hDlg, IDC_UDP_DESCRIPTION, pUDPList[nItem].szDescription);
}

BOOL CALLBACK EditUDPDescriptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		switch(v->nSocketDialogMode)
		{
		case SOCKET_MODE_UDP_MULTICAST:
		case SOCKET_MODE_TCP:
			SetDlgItemText(hDlg, IDC_UDP_ADDRESS, pUDPList[v->nSelectedUDPEntry].szAddress);
			break;
		case SOCKET_MODE_UDP_UNICAST:
			SetDlgItemText(hDlg, IDC_UDP_ADDRESS_CAPTION, "Port");
			SetDlgItemInt(hDlg, IDC_UDP_ADDRESS, pUDPList[v->nSelectedUDPEntry].nPort, FALSE);
			break;
		}
		SetDlgItemText(hDlg, IDC_UDP_STREAM_DESCRIPTION, pUDPList[v->nSelectedUDPEntry].szDescription);
		SendDlgItemMessage(hDlg, IDC_UDP_STREAM_DESCRIPTION, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_UDP_STREAM_DESCRIPTION));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_UDP_STREAM_DESCRIPTION, pUDPList[v->nSelectedUDPEntry].szDescription, sizeof(pUDPList[v->nSelectedUDPEntry].szDescription));
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

void SetupUDPInterfaceCombo(HWND hDlg)
{
	HOSTENT *pHost; 
	HWND hCombo = GetDlgItem(hDlg, IDC_INTERFACE_ADDRESS);
	int  nAdapter = 0; 
	char szHostname[MAX_PATH]; 

	gethostname(szHostname, sizeof(szHostname)); 
	pHost = gethostbyname(szHostname); 
	while (pHost->h_addr_list[nAdapter]) 
	{
		int nItem;
		char szTemp[128];

		wsprintf(szTemp, "%d.%d.%d.%d", 
			(BYTE)pHost->h_addr_list[nAdapter][0],
			(BYTE)pHost->h_addr_list[nAdapter][1],
			(BYTE)pHost->h_addr_list[nAdapter][2],
			(BYTE)pHost->h_addr_list[nAdapter][3]);
		nItem = SendMessage(hCombo, CB_ADDSTRING, 0, (LPARAM)szTemp);
		if (lstrcmp(szTemp, v->ss.szUDPMulticastInterface) == 0)
			SendMessage(hCombo, CB_SETCURSEL, nItem, 0);
		nAdapter++;
	}
}

BOOL CALLBACK TuneSocketDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HICON hIcon;

			v->nSelectedUDPEntry = -1;
			v->nSocketDialogMode = lParam;
			pUDPList = LocalAlloc(LPTR, sizeof(UDPLIST) * MAX_UDP_LIST_ENTRIES);

			SetupUDPListControl(hDlg);
			LoadUDPList(hDlg);
			if (v->nSocketDialogMode == SOCKET_MODE_UDP_MULTICAST)
				SetupUDPInterfaceCombo(hDlg);
			if (v->nSocketDialogMode == SOCKET_MODE_UDP_MULTICAST || v->nSocketDialogMode == SOCKET_MODE_TCP)
				SetDlgItemText(hDlg, IDC_UDP_MULTICAST_ADDRESS, v->ss.szUDPMulticastAddress);
			if (v->ss.nUDPMulticastPort)
			{
				int i;

				SetDlgItemInt(hDlg, IDC_UDP_MULTICAST_PORT, v->ss.nUDPMulticastPort, FALSE);
				for (i = 0; i < v->nUDPListEntries; i++)
				{
					if (v->ss.nUDPMulticastPort == pUDPList[i].nPort)
					{
						SetDlgItemText(hDlg, IDC_UDP_DESCRIPTION, pUDPList[i].szDescription);
						break;
					}
				}
			}
			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_DELETE), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_UDP_DELETE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			if (v->nSocketDialogMode == SOCKET_MODE_UDP_MULTICAST || v->nSocketDialogMode == SOCKET_MODE_TCP)
			{
				SendDlgItemMessage(hDlg, IDC_UDP_MULTICAST_ADDRESS, EM_SETSEL, 0, -1);
				SetFocus(GetDlgItem(hDlg, IDC_UDP_MULTICAST_ADDRESS));
			}
			else
			{
				SendDlgItemMessage(hDlg, IDC_UDP_MULTICAST_PORT, EM_SETSEL, 0, -1);
				SetFocus(GetDlgItem(hDlg, IDC_UDP_MULTICAST_PORT));
			}
		}
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
			if (v->nSocketDialogMode == SOCKET_MODE_UDP_MULTICAST || v->nSocketDialogMode == SOCKET_MODE_TCP)
				GetDlgItemText(hDlg, IDC_UDP_MULTICAST_ADDRESS, v->ss.szUDPMulticastAddress, sizeof(v->ss.szUDPMulticastAddress));
			if (v->nSocketDialogMode == SOCKET_MODE_UDP_MULTICAST)
			{
				int nItem;

				nItem = SendDlgItemMessage(hDlg, IDC_INTERFACE_ADDRESS, CB_GETCURSEL, 0, 0);
				if (nItem == CB_ERR)
					v->ss.szUDPMulticastInterface[0] = '\0';
				else
				{
					int nRetVal = SendDlgItemMessage(hDlg, IDC_INTERFACE_ADDRESS, CB_GETLBTEXT, nItem, (LPARAM)v->ss.szUDPMulticastInterface);
				}
			}
			v->ss.nUDPMulticastPort = GetDlgItemInt(hDlg, IDC_UDP_MULTICAST_PORT, NULL, FALSE);
			SaveUDPList(hDlg);
			EndDialog(hDlg, TRUE);
			break;
		case IDC_UDP_EDIT:
			if (v->nSelectedUDPEntry != -1)
			{
				if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_EDIT_UDP_DESCRIPTION), hDlg, EditUDPDescriptionDlgProc) == TRUE)
				{
					SetDlgItemText(hDlg, IDC_UDP_DESCRIPTION, pUDPList[v->nSelectedUDPEntry].szDescription);
					ListView_RedrawItems(GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED), v->nSelectedUDPEntry, v->nSelectedUDPEntry);
				}
				SetFocus(GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED));
			}
			else
				MessageBox(hDlg, "Please select the item who's description you want to change.", gszAppName, MB_ICONSTOP);
			break;
		case IDC_UDP_DELETE:
			if (v->nSelectedUDPEntry != -1)
			{
				int i;
				int nOutCounter = 0;
				PUDPLIST pNewUDPList = LocalAlloc(LPTR, sizeof(UDPLIST) * MAX_UDP_LIST_ENTRIES);
				
				for (i = 0; i < v->nUDPListEntries; i++)
				{
					if (i != v->nSelectedUDPEntry)
						memcpy(&pNewUDPList[nOutCounter++], &pUDPList[i], sizeof(UDPLIST));
				}
				v->nUDPListEntries--;
				memset(pUDPList, 0, sizeof(UDPLIST) * MAX_UDP_LIST_ENTRIES);
				memcpy(pUDPList, pNewUDPList, sizeof(UDPLIST) * v->nUDPListEntries);
				LocalFree(pNewUDPList);
				ListView_DeleteItem(GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED), v->nSelectedUDPEntry);
				ListView_RedrawItems(GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED), 0, v->nUDPListEntries);
				ListView_SetItemState(GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED), v->nSelectedUDPEntry, LVIS_SELECTED, LVIS_SELECTED);				
				SetFocus(GetDlgItem(hDlg, IDC_UDP_RECENTLY_USED));
			}
			else
				MessageBox(hDlg, "Please select the item you want to delete.", gszAppName, MB_ICONSTOP);
			break;
		}
		break;
	case WM_DESTROY:
		LocalFree(pUDPList);
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				GetUDPItemDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
				if (pnmv->uNewState & LVIS_SELECTED)
				{
					UpdateUDPItem(hDlg, pnmv->iItem);
					v->nSelectedDVBTMux = pnmv->iItem;
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

void EnableOrDisableAutoLNBSettings(HWND hDlg, BOOL fEnabled)
{
	EnableWindow(GetDlgItem(hDlg, IDC_LOF), fEnabled);
	EnableWindow(GetDlgItem(hDlg, IDC_22KHZ), fEnabled);
	EnableWindow(GetDlgItem(hDlg, IDC_NO_LNB_POWER), fEnabled);
}

BOOL CALLBACK AddSatelliteDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		CheckDlgButton(hDlg, IDC_SATELLITE_ORBITAL_EAST, BST_CHECKED);
		SetFocus(GetDlgItem(hDlg, IDC_SATELLITE_NAME));
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				BOOL fNewSatelliteWest;
				BOOL fCloseDialog = TRUE;
				int nIndex;
				int nOrbital, nOrbitalDecimal, nOrbitalININumber;
				char szNewSatelliteName[64];
				char szNewSatellitePosition[32];

				// Get new satellite info
				GetDlgItemText(hDlg, IDC_SATELLITE_NAME, szNewSatelliteName, sizeof(szNewSatelliteName));
				if (!lstrlen(szNewSatelliteName))
				{
					MessageBox(hDlg, "Please provide a satellite name", gszAppName, MB_ICONINFORMATION);
					SetFocus(GetDlgItem(hDlg, IDC_SATELLITE_NAME));
					break;
				}
				GetDlgItemText(hDlg, IDC_SATELLITE_ORBITAL, szNewSatellitePosition, sizeof(szNewSatellitePosition));
				if (!lstrlen(szNewSatellitePosition))
				{
					MessageBox(hDlg, "Please provide the orbital position", gszAppName, MB_ICONINFORMATION);
					SetFocus(GetDlgItem(hDlg, IDC_SATELLITE_ORBITAL));
					break;
				}
				fNewSatelliteWest = IsDlgButtonChecked(hDlg, IDC_SATELLITE_ORBITAL_WEST);

				// Make sure it doesn't already exist
				nOrbital = nOrbitalDecimal = 0;
				sscanf(szNewSatellitePosition, "%d.%d", &nOrbital, &nOrbitalDecimal);
				nOrbitalININumber = nOrbital = (nOrbital * 10) + nOrbitalDecimal;
				if (fNewSatelliteWest)
					nOrbitalININumber = 3600 - nOrbitalININumber;
				for (nIndex = 0; nIndex < MAX_SATELLITES; nIndex++)
				{
					if (v->sats[nIndex].nOrbitalPosition != -1)
					{
						if (v->sats[nIndex].nOrbitalPosition == nOrbitalININumber)
						{
							char szTemp[128];
							char szEastWest[2] = {"E"};

							if (v->sats[nIndex].fWest)
								szEastWest[0] = 'W';
							sprintf(szTemp, "The satellite \"%s\" is already defined at %.1f %s",
								    v->sats[nIndex].szName,
									(double)v->sats[nIndex].nOrbitalPosition / 10.0,
									szEastWest);
							MessageBox(hDlg, szTemp, gszAppName, MB_ICONINFORMATION);
							fCloseDialog = FALSE;
							break;
						}
					}
				}
				if (fCloseDialog)
				{
					for (nIndex = 0; nIndex < MAX_SATELLITES; nIndex++)
					{
						if (v->sats[nIndex].nOrbitalPosition == -1)
						{
							memset(&v->sats[nIndex], 0, sizeof(SATELLITE));
							v->sats[nIndex].fWest = fNewSatelliteWest;
							v->sats[nIndex].fDirty = TRUE;
							v->sats[nIndex].nOrbitalPosition = nOrbital;
							lstrcpy(v->sats[nIndex].szName, szNewSatelliteName);
							wsprintf(v->sats[nIndex].szSourceFilename, "%04d.ini", nOrbitalININumber);
							v->nSatellites++;
							break;
						}
					}
					if (nIndex == MAX_SATELLITES)
						MessageBox(hDlg, "Out of room for satellites - please tell rod@coolstf.com you saw this", gszAppName, MB_ICONSTOP);
					else
					{
						ListView_DeleteAllItems(GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST));
						ListView_DeleteAllItems(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST));
						SortSatelliteList();
						PopulateSatelliteListView(hDlg);
					}
					EndDialog(hDlg, TRUE);
				}
			}
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

void WriteSatelliteINIFile(int nSatelliteIndex)
{
	int nDVBCount = 0, nDSSCount = 0, nADVCount = 0;
	int nDVBCurrent = 0, nDSSCurrent = 0, nADVCurrent = 0;
	int nOrbital = v->sats[nSatelliteIndex].nOrbitalPosition;
	char szCurrentDir[MAX_PATH];
	char szINIFile[MAX_PATH];
	char szTemp[256];

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
	wsprintf(szINIFile, "%s\\Satellites\\%s", szCurrentDir, v->sats[nSatelliteIndex].szSourceFilename);

	if (v->sats[nSatelliteIndex].fWest)
		nOrbital = 3600 - nOrbital;
	wsprintf(szTemp, "%04d", nOrbital);
	WritePrivateProfileString("SATTYPE", "1", szTemp, szINIFile);
	WritePrivateProfileString("SATTYPE", "2", v->sats[nSatelliteIndex].szName, szINIFile);

	if (v->sats[nSatelliteIndex].nLastDiSEqC && v->sats[nSatelliteIndex].nLastLOF)
	{
		wsprintf(szTemp, "%d", v->sats[nSatelliteIndex].nLastDiSEqC);
		WritePrivateProfileString("TSREADER", "DISEQC", szTemp, szINIFile);
		wsprintf(szTemp, "%d", v->sats[nSatelliteIndex].nLastLOF);
		WritePrivateProfileString("TSREADER", "LOF", szTemp, szINIFile);
	}

	if (v->sats[nSatelliteIndex].nMuxCount)
	{
		int nMuxIndex;

		for (nMuxIndex = 0; nMuxIndex < v->sats[nSatelliteIndex].nMuxCount; nMuxIndex++)
		{
			switch(v->sats[nSatelliteIndex].mux[nMuxIndex].nTuneType)
			{
			case MUX_TUNE_TYPE_ADV:
				{
					char szKeyName[16];

					nADVCount++;
					wsprintf(szTemp, "%d,%s,%d,%d,%d,%s",
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nFrequency,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].szPolarity,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nSymbolRate,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nCodeRate,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nModulationMode,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].szMuxName);
					wsprintf(szKeyName, "%d", nADVCount);
					WritePrivateProfileString("ADV", szKeyName, szTemp, szINIFile);
				}
				break;
			case MUX_TUNE_TYPE_DVB:
				{
					char szFEC[4];
					char szKeyName[16];

					nDVBCount++;
					switch(v->sats[nSatelliteIndex].mux[nMuxIndex].nCodeRate)
					{
					case 0:
						lstrcpy(szFEC, "12");
						break;
					case 1:
						lstrcpy(szFEC, "23");
						break;
					case 2:
						lstrcpy(szFEC, "34");
						break;
					case 3:
						lstrcpy(szFEC, "56");
						break;
					case 4:
						lstrcpy(szFEC, "78");
						break;
					default:
						lstrcpy(szFEC, "00");
						break;
					}
					wsprintf(szTemp, "%d,%s,%d,%s,%s",
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nFrequency,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].szPolarity,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nSymbolRate,
							 szFEC,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].szMuxName);
					wsprintf(szKeyName, "%d", nDVBCount);
					WritePrivateProfileString("DVB", szKeyName, szTemp, szINIFile);
				}
				break;
			case MUX_TUNE_TYPE_DSS:
				{
					char szKeyName[16];

					nDSSCount++;
					wsprintf(szTemp, "%d,%s,%d,00,%s",
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nFrequency,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].szPolarity,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].nSymbolRate,
							 v->sats[nSatelliteIndex].mux[nMuxIndex].szMuxName);
					wsprintf(szKeyName, "%d", nDSSCount);
					WritePrivateProfileString("DSS", szKeyName, szTemp, szINIFile);
				}
				break;
			}
		}	
		if (nDVBCount)
		{
			wsprintf(szTemp, "%d", nDVBCount);
			WritePrivateProfileString("DVB", "0", szTemp, szINIFile);
		}
		if (nDSSCount)
		{
			wsprintf(szTemp, "%d", nDSSCount);
			WritePrivateProfileString("DSS", "0", szTemp, szINIFile);
		}
		if (nADVCount)
		{
			wsprintf(szTemp, "%d", nADVCount);
			WritePrivateProfileString("ADV", "0", szTemp, szINIFile);
		}
	}

	v->sats[nSatelliteIndex].fDirty = FALSE;
}

void FlushDirtySatellites()
{
	if (v->nSatellites)
	{
		int nIndex;

		for (nIndex = 0; nIndex < MAX_SATELLITES; nIndex++)
		{
			if (v->sats[nIndex].nOrbitalPosition != -1)
			{
				if (v->sats[nIndex].fDirty)
					WriteSatelliteINIFile(nIndex);
			}
		}
	}
}

BOOL CALLBACK EditMuxDescriptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTemp[64];

			wsprintf(szTemp, "%d MHz %s %d", 
				     v->sats[v->nCurrentSelectedSatellite].mux[v->nCurrentlySelectedMux].nFrequency,
					 v->sats[v->nCurrentSelectedSatellite].mux[v->nCurrentlySelectedMux].szPolarity,
					 v->sats[v->nCurrentSelectedSatellite].mux[v->nCurrentlySelectedMux].nSymbolRate);
			SetDlgItemText(hDlg, IDC_SIGNAL_PARAMETERS, szTemp);
			SetDlgItemText(hDlg, IDC_MUX_NAME, v->sats[v->nCurrentSelectedSatellite].mux[v->nCurrentlySelectedMux].szMuxName);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_MUX_NAME, v->sats[v->nCurrentSelectedSatellite].mux[v->nCurrentlySelectedMux].szMuxName, sizeof(v->sats[v->nCurrentSelectedSatellite].mux[v->nCurrentlySelectedMux].szMuxName));
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

void UpdateNewMuxText(HWND hDlg, BOOL fCircular)
{
	char szTemp[64];
	char szPolarity[2] = {""};

	if (!fCircular)
	{
		if (IsDlgButtonChecked(GetParent(hDlg), IDC_HORZ))
			szPolarity[0] = 'H';
		else
			szPolarity[0] = 'V';
	}
	else
	{
		if (IsDlgButtonChecked(GetParent(hDlg), IDC_HORZ))
			szPolarity[0] = 'L';
		else
			szPolarity[0] = 'R';
	}
	wsprintf(szTemp, "%d MHz %s %d", 
			 GetDlgItemInt(GetParent(hDlg), IDC_FREQUENCY, NULL, FALSE),
			 szPolarity,
			 GetDlgItemInt(GetParent(hDlg), IDC_SR, NULL, FALSE));
	SetDlgItemText(hDlg, IDC_SIGNAL_PARAMETERS, szTemp);

}

BOOL CALLBACK AddMuxDescriptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			v->fNewMuxCircular = FALSE;
			UpdateNewMuxText(hDlg, v->fNewMuxCircular);
			CheckDlgButton(hDlg, IDC_ADD_MUX_LINEAR, BST_CHECKED);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_MUX_NAME, v->szNewMuxName, sizeof(v->szNewMuxName));
			EndDialog(hDlg, TRUE);
			break;
		case IDC_ADD_MUX_LINEAR:
			v->fNewMuxCircular = FALSE;
			UpdateNewMuxText(hDlg, v->fNewMuxCircular);
			break;
		case IDC_ADD_MUX_CIRCULAR:
			v->fNewMuxCircular = TRUE;
			UpdateNewMuxText(hDlg, v->fNewMuxCircular);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, FALSE);
		break;
	}

	return FALSE;
}


BOOL CALLBACK EditSatelliteDescriptionDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char szTemp[64];
			char szEastWest[2] = {"E"};

			if (v->sats[v->nCurrentSelectedSatellite].fWest)
				szEastWest[0] = 'W';
			sprintf(szTemp, "%.1f %s", (double)v->sats[v->nCurrentSelectedSatellite].nOrbitalPosition / 10.0, szEastWest);
			SetDlgItemText(hDlg, IDC_ORBITAL_POSITION, szTemp);
			SetDlgItemText(hDlg, IDC_SATELLITE_NAME, v->sats[v->nCurrentSelectedSatellite].szName);
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDOK:
			GetDlgItemText(hDlg, IDC_SATELLITE_NAME, v->sats[v->nCurrentSelectedSatellite].szName, sizeof(v->sats[v->nCurrentSelectedSatellite].szName));
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

void AddNewMux(HWND hDlg, int nSatelliteIndex)
{
	int nMuxCount = v->sats[nSatelliteIndex].nMuxCount;
	LV_ITEM lvi; 
	PMUX newmuxes = LocalAlloc(LPTR, (nMuxCount + 1) * sizeof(MUX));
	
	memcpy(newmuxes, v->sats[nSatelliteIndex].mux, nMuxCount * sizeof(MUX));
	LocalFree(v->sats[nSatelliteIndex].mux);
	v->sats[nSatelliteIndex].mux = newmuxes;
	
	v->sats[nSatelliteIndex].mux[nMuxCount].nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
	v->sats[nSatelliteIndex].mux[nMuxCount].nSymbolRate = GetDlgItemInt(hDlg, IDC_SR, NULL, FALSE);
	if (!v->fNewMuxCircular)
	{
		if (IsDlgButtonChecked(hDlg, IDC_HORZ))
			lstrcpy(v->sats[nSatelliteIndex].mux[nMuxCount].szPolarity, "H");
		else
			lstrcpy(v->sats[nSatelliteIndex].mux[nMuxCount].szPolarity, "V");
	}
	else
	{
		if (IsDlgButtonChecked(hDlg, IDC_HORZ))
			lstrcpy(v->sats[nSatelliteIndex].mux[nMuxCount].szPolarity, "L");
		else
			lstrcpy(v->sats[nSatelliteIndex].mux[nMuxCount].szPolarity, "R");
	}
	lstrcpy(v->sats[nSatelliteIndex].mux[nMuxCount].szMuxName, v->szNewMuxName);
	v->sats[nSatelliteIndex].mux[nMuxCount].nTuneType = MUX_TUNE_TYPE_DVB;
	if (v->fADVModulation)
	{
		v->sats[nSatelliteIndex].mux[nMuxCount].nModulationMode = SendDlgItemMessage(hDlg, IDC_MODULATION, CB_GETCURSEL, 0, 0);
		if (v->sats[nSatelliteIndex].mux[nMuxCount].nModulationMode)
		{
			v->sats[nSatelliteIndex].mux[nMuxCount].fADVModulation = TRUE;
			v->sats[nSatelliteIndex].mux[nMuxCount].nTuneType = MUX_TUNE_TYPE_ADV;
		}
		v->sats[nSatelliteIndex].mux[nMuxCount].nCodeRate = SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_GETCURSEL, 0, 0);
	}
	else
	{
		if (lstrcmp(szTypeName, "DSS") == 0)
			v->sats[nSatelliteIndex].mux[nMuxCount].nTuneType = MUX_TUNE_TYPE_DSS;
	}

	memset(&lvi, 0, sizeof(lvi));
	lvi.state = LVIS_SELECTED; 
	lvi.stateMask = LVIS_SELECTED; 
	lvi.pszText = LPSTR_TEXTCALLBACK;   // app. maintains text 
	lvi.mask = LVIF_TEXT | LVIF_STATE; 
	lvi.iItem = nMuxCount; 
	lvi.iSubItem = 0; 
	lvi.lParam = (LPARAM)0;    // item data 
	ListView_InsertItem(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST), &lvi);
	ListView_EnsureVisible(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST), nMuxCount, FALSE);

	v->sats[nSatelliteIndex].nMuxCount++;
	v->sats[nSatelliteIndex].fDirty = TRUE;
	FlushDirtySatellites();
}

void DeleteMux(HWND hDlg, int nSatelliteIndex, int nMuxIndex)
{
	int nCopyIndex;
	int nOutputIndex = 0;
	PMUX newmuxes = LocalAlloc(LPTR, (v->sats[nSatelliteIndex].nMuxCount - 1) * sizeof(MUX));

	for (nCopyIndex = 0; nCopyIndex < v->sats[nSatelliteIndex].nMuxCount; nCopyIndex++)
	{
		if (nCopyIndex != nMuxIndex)
		{
			memcpy(&newmuxes[nOutputIndex], &v->sats[nSatelliteIndex].mux[nCopyIndex], sizeof(MUX));
			nOutputIndex++;
		}
	}
	LocalFree(v->sats[nSatelliteIndex].mux);
	v->sats[nSatelliteIndex].mux = newmuxes;
	ListView_DeleteItem(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST), nMuxIndex);
	v->sats[nSatelliteIndex].nMuxCount--;
	v->sats[nSatelliteIndex].fDirty = TRUE;
}

BOOL CALLBACK TuneDVBSatelliteDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			HANDLE hIcon;
			char szTemp[256];

			SetDlgItemText(hDlg, IDC_TUNER_MUX_TITLE, "");

			if (fTuneDialogFirstTime == TRUE)
			{
				fTuneDialogFirstTime = FALSE;
				LoadSatelliteFiles();
			}
			v->nCurrentSelectedSatellite = -1;
			v->nCurrentlySelectedMux = -1;

			wsprintf(szTemp, "Tune %s", v->szSourceModuleDescription);
			if (v->ss.fSerialReceiverControlEnabled)
			{
				char szTemp2[128];
				wsprintf(szTemp2, " plus %s", v->szSerialReceiverType);
				lstrcat(szTemp, szTemp2);
			}
			SendMessage(hDlg, WM_SETTEXT, 0, (LPARAM)szTemp);
			SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_SETCURSEL, 0, 0);
			SetDlgItemInt(hDlg, IDC_FREQUENCY, v->ss.nFrequency, FALSE);
			switch(v->nPolarity)
			{
			case 0:
				CheckDlgButton(hDlg, IDC_VERT, TRUE);
				break;
			case 1:
				CheckDlgButton(hDlg, IDC_HORZ, TRUE);
				break;
			case -1:
				CheckDlgButton(hDlg, IDC_NO_LNB_POWER, TRUE);
				break;
			}
			SetDlgItemInt(hDlg, IDC_SR, v->ss.nSymbolRate, FALSE);
			SetDlgItemInt(hDlg, IDC_LOF, v->ss.nLNBFrequency, FALSE);
			CheckDlgButton(hDlg, IDC_22KHZ, v->ss.n22KHz);

			LoadDishSwitchCombo(hDlg, IDC_DISH_COMBO);
			SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_SETCURSEL, 0, 0);
			EnableOrDisableInputs(hDlg);
			switch(v->ss.nDiSEqCInput)
			{
			case 0:
				CheckDlgButton(hDlg, IDC_INPUT_NONE, TRUE);
				break;
			case 1:
				CheckDlgButton(hDlg, IDC_DISEQC1, TRUE);
				break;
			case 2:
				CheckDlgButton(hDlg, IDC_DISEQC2, TRUE);
				break;
			case 3:
				CheckDlgButton(hDlg, IDC_DISEQC3, TRUE);
				break;
			case 4:
				CheckDlgButton(hDlg, IDC_DISEQC4, TRUE);
				break;
			case 5:
				CheckDlgButton(hDlg, IDC_INPUT_TB_A, TRUE);
				break;
			case 6:
				CheckDlgButton(hDlg, IDC_INPUT_TB_B, TRUE);
				break;
			default:
				if (v->ss.nDiSEqCInput >= 7 && v->ss.nDiSEqCInput <= 20)
				{
					// Dish switch
					CheckDlgButton(hDlg, IDC_INPUT_DISH, TRUE);
					SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_SETCURSEL, v->ss.nDiSEqCInput - 7, 0);
				}
				break;
			}
			EnableOrDisableAutoLNBSettings(hDlg, v->ss.nDiSEqCInput == 0);

			if (v->fADVModulation)
			{
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"DVB-S QPSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"Turbo QPSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"8PSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"16QAM");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"DCII-C QPSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"DCII-I QPSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"DCII-Q QPSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"DCII-C OQPSK");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_ADDSTRING, 0, (LPARAM)"DVB-S2");
				SendDlgItemMessage(hDlg, IDC_MODULATION, CB_SETCURSEL, (WPARAM)v->ss.nADVModulationMode, 0);
				SetupCodeRateOptions(hDlg, v->ss.nADVModulationMode);
				SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_SETCURSEL, (WPARAM)v->ss.nCodeRate, 0);
			}

			if (v->nSatellites)
			{
				SetupSatelliteListViews(hDlg);
				PopulateSatelliteListView(hDlg);
			}
			else
			{
				RECT rcDialog;
				RECT rcSeperator;
				int nNewX, nNewY, nDifference;

				GetWindowRect(hDlg, &rcDialog);
				GetWindowRect(GetDlgItem(hDlg, IDC_SEPERATOR), &rcSeperator);

				nNewX = rcDialog.right - rcDialog.left;
				nNewY = rcDialog.bottom - rcDialog.top;
				nDifference = rcDialog.bottom - rcSeperator.bottom;
				nNewY -= nDifference;
				SetWindowPos(hDlg, HWND_TOP, 0, 0, nNewX, nNewY, SWP_NOMOVE);
			}

			v->fCtrlDown = FALSE;
			SetTimer(hDlg, 1, 100, NULL);
			SendDlgItemMessage(hDlg, IDC_FREQUENCY, EM_SETSEL, 0, -1);
			SetFocus(GetDlgItem(hDlg, IDC_FREQUENCY));
			SetupAutoSwitchParameters(hDlg, TRUE);

			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_DELETE), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_SATELLITE_DELETE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			SendDlgItemMessage(hDlg, IDC_MUX_DELETE, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_ADD), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_SATELLITE_ADD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			SendDlgItemMessage(hDlg, IDC_MUX_ADD, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);

			hIcon = LoadImage(hInstance, MAKEINTRESOURCE(IDI_USALS), IMAGE_ICON, 16, 16, 0);
			SendDlgItemMessage(hDlg, IDC_SATELLITE_USALS, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
			if ((v->dwSourceCapabilities & CAPABILITIES_DISEQC_POSITIONER) == 0)
				EnableWindow(GetDlgItem(hDlg, IDC_SATELLITE_USALS), FALSE);				
		}
		break;
	case WM_DESTROY:
		KillTimer(hDlg, 1);
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
			case IDC_SATELLITE_USALS:
				if (v->nCurrentSelectedSatellite != -1)
				{
					double dOrbital;

					if (v->nLocationLatitude == 0 && v->nLocationLongitude == 0)
					{
						MessageBox(hDlg, "Please enter your latitude and longitude by pressing the Positioner button", gszAppName, MB_ICONSTOP);
						break;
					}
					dOrbital = (double)v->sats[v->nCurrentSelectedSatellite].nOrbitalPosition / 10.0;
					v->fLastOrbitalWest = v->sats[v->nCurrentSelectedSatellite].fWest;
					v->nLastOrbital = (int)(dOrbital * 1000.0);
					GotoUSALSPosition(hDlg);
				}
				else
					MessageBox(hDlg, "Select the satellite you wish to move to first", gszAppName, MB_ICONWARNING);
				break;
			case IDC_SATELLITE_DELETE:
				if (v->nCurrentSelectedSatellite != -1)
				{
					char szTemp[256];
					wsprintf(szTemp, "Are you sure you want to remove satellite \"%s\"?", v->sats[v->nCurrentSelectedSatellite].szName);
					if (MessageBox(hDlg, szTemp, gszAppName, MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
					{
						int nIndex;
						char szCurrentDir[MAX_PATH];
						char szINIFile[MAX_PATH];

						SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szCurrentDir, sizeof(szCurrentDir));
						wsprintf(szINIFile, "%s\\Satellites\\%s", szCurrentDir, v->sats[v->nCurrentSelectedSatellite].szSourceFilename);
						DeleteFile(szINIFile);
						
						v->nSatellites--;
						for (nIndex = v->nCurrentSelectedSatellite; nIndex < v->nSatellites; nIndex++)
							memcpy(&v->sats[nIndex], &v->sats[nIndex + 1], sizeof(SATELLITE));
						memset(&v->sats[nIndex], 0, sizeof(SATELLITE));
						v->sats[nIndex].nOrbitalPosition = -1;

						ListView_DeleteAllItems(GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST));
						ListView_DeleteAllItems(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST));
						SortSatelliteList();
						PopulateSatelliteListView(hDlg);
						v->nCurrentSelectedSatellite = -1;
					}
				}
				else
					MessageBox(hDlg, "Select the satellite you wish to delete first", gszAppName, MB_ICONWARNING);
				break;
			case IDC_MUX_DELETE:
				if (v->nCurrentlySelectedMux != -1)
				{
					DeleteMux(hDlg, v->nCurrentSelectedSatellite, v->nCurrentlySelectedMux);
					FlushDirtySatellites();
				}
				else
					MessageBeep(0);
				break;
			case IDC_SATELLITE_ADD:
				if (v->nCurrentSelectedSatellite != -1)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_ADD_SATELLITE), hDlg, AddSatelliteDlgProc) == TRUE)
						FlushDirtySatellites();
				}
				else
					MessageBeep(0);
				break;
			case IDC_MUX_ADD:
				if (v->nCurrentSelectedSatellite != -1)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_ADD_MUX), hDlg, AddMuxDescriptionDlgProc) == TRUE)
						AddNewMux(hDlg, v->nCurrentSelectedSatellite);
				}
				else
					MessageBeep(0);
				break;
			case IDC_SATELLITE_EDIT:
				if (v->nCurrentSelectedSatellite != -1)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_EDIT_SATELLITE_DESCRIPTION), hDlg, EditSatelliteDescriptionDlgProc) == TRUE)
					{
						lstrcpy(v->szCurrentlySelectedSatellite, v->sats[v->nCurrentSelectedSatellite].szName);
						ListView_RedrawItems(GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST), v->nCurrentSelectedSatellite, v->nCurrentSelectedSatellite);
						v->sats[v->nCurrentSelectedSatellite].fDirty = TRUE;
						FlushDirtySatellites();
					}
				}
				else
					MessageBeep(0);
				break;
			case IDC_MUX_EDIT:
				if (v->nCurrentlySelectedMux != -1)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_EDIT_MUX_DESCRIPTION), hDlg, EditMuxDescriptionDlgProc) == TRUE)
					{
						ListView_RedrawItems(GetDlgItem(hDlg, IDC_TUNER_MUX_LIST), v->nCurrentlySelectedMux, v->nCurrentlySelectedMux);
						v->sats[v->nCurrentSelectedSatellite].fDirty = TRUE;
						FlushDirtySatellites();
					}
				}
				break;
			case IDC_INPUT_NONE:
				EnableOrDisableAutoLNBSettings(hDlg, TRUE);
				break;
			case IDC_DISEQC1:
			case IDC_DISEQC2:
			case IDC_DISEQC3:
			case IDC_DISEQC4:
			case IDC_INPUT_TB_A:
			case IDC_INPUT_TB_B:
			case IDC_INPUT_DISH:
				EnableOrDisableAutoLNBSettings(hDlg, FALSE);
				SetupAutoSwitchParameters(hDlg, FALSE);
				break;
			case IDC_POSITIONER:
				{
					int nOriginalPolarity = v->ss.nPolarity;
					int nOriginal22KHz = v->ss.n22KHz;
									
					if (IsDlgButtonChecked(hDlg, IDC_HORZ))
						v->ss.nPolarity = 1;
					else if (IsDlgButtonChecked(hDlg, IDC_VERT))
						v->ss.nPolarity = 0;
					else if (IsDlgButtonChecked(hDlg, IDC_NO_LNB_POWER))
						v->ss.nPolarity = -1;
					v->ss.n22KHz = IsDlgButtonChecked(hDlg, IDC_22KHZ);
					SourceHelper_DiSEqCPositionerDialog(hDlg);
					v->ss.nPolarity = nOriginalPolarity;
					v->ss.n22KHz = nOriginal22KHz;
				}
				break;
			case IDC_HORZ:
			case IDC_VERT:
				SetupAutoSwitchParameters(hDlg, FALSE);
				break;
			case IDC_INPUT_SETUP:
				if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_INPUT_SETUP), hDlg, InputSetupDlgProc) == TRUE)
					SetupAutoSwitchParameters(hDlg, TRUE);
				break;
			case IDOK:
				v->ss.nFrequency = GetDlgItemInt(hDlg, IDC_FREQUENCY, NULL, FALSE);
				if (IsDlgButtonChecked(hDlg, IDC_HORZ))
					v->nPolarity = 1;
				else if (IsDlgButtonChecked(hDlg, IDC_VERT))
					v->nPolarity = 0;
				else if (IsDlgButtonChecked(hDlg, IDC_NO_LNB_POWER))
					v->nPolarity = -1;
				if (v->ss.nDiSEqCInput == 0 || v->ss.nDiSEqCInput > 6)
					v->ss.nPolarity = v->nPolarity;
				v->ss.nSymbolRate = GetDlgItemInt(hDlg, IDC_SR, NULL, FALSE);
				v->ss.nLNBFrequency = GetDlgItemInt(hDlg, IDC_LOF, NULL, FALSE);
				if (v->ss.nLNBFrequency == 0 && v->fDisableLNBFrequencyWarning == FALSE)
				{
					if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_LNB_LO_WARNING), hDlg, LNBLOWarningDlgProc) == FALSE)
						break;
				}
				v->ss.n22KHz = IsDlgButtonChecked(hDlg, IDC_22KHZ);
				if (IsDlgButtonChecked(hDlg, IDC_DISEQC1))					v->ss.nDiSEqCInput = 1;
				else if (IsDlgButtonChecked(hDlg, IDC_DISEQC2))				v->ss.nDiSEqCInput = 2;
				else if (IsDlgButtonChecked(hDlg, IDC_DISEQC3))				v->ss.nDiSEqCInput = 3;
				else if (IsDlgButtonChecked(hDlg, IDC_DISEQC4))				v->ss.nDiSEqCInput = 4;
				else if (IsDlgButtonChecked(hDlg, IDC_INPUT_TB_A))			v->ss.nDiSEqCInput = 5;
				else if (IsDlgButtonChecked(hDlg, IDC_INPUT_TB_B))			v->ss.nDiSEqCInput = 6;
				else if (IsDlgButtonChecked(hDlg, IDC_INPUT_NONE))			v->ss.nDiSEqCInput = 0;
				else if (IsDlgButtonChecked(hDlg, IDC_INPUT_DISH))
					v->ss.nDiSEqCInput = SendDlgItemMessage(hDlg, IDC_DISH_COMBO, CB_GETCURSEL, 0, 0) + 7;

				if (v->fADVModulation)
				{
					v->ss.nADVModulationMode = SendDlgItemMessage(hDlg, IDC_MODULATION, CB_GETCURSEL, 0, 0);
					v->ss.nCodeRate = SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_GETCURSEL, 0, 0);
				}
				WriteSatelliteLNBFAndSwitch(hDlg, v->nCurrentSelectedSatellite);
				FlushDirtySatellites();
				EndDialog(hDlg, TRUE);
				break;
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				break;
			}
			break;
		case CBN_SELCHANGE:
			switch(LOWORD(wParam))
			{
			case IDC_MODULATION:
				{
					int nSelection = SendDlgItemMessage(hDlg, IDC_MODULATION, CB_GETCURSEL, 0, 0);
					int nCodeRateIndex = SetupCodeRateOptions(hDlg, nSelection);
					SendDlgItemMessage(hDlg, IDC_CODE_RATE, CB_SETCURSEL, nCodeRateIndex, 0);
				}
				break;
			case IDC_DISH_COMBO:
				if (IsDlgButtonChecked(hDlg, IDC_INPUT_DISH))
				{
					EnableOrDisableAutoLNBSettings(hDlg, FALSE);
					SetupAutoSwitchParameters(hDlg, FALSE);
				}
				break;
			}
			break;
		case EN_CHANGE:
			switch(LOWORD(wParam))
			{
			case IDC_FREQUENCY:
				SetupAutoSwitchParameters(hDlg, TRUE);
				break;
			}
			break;
		}
		break;
	case WM_NOTIFY: 
		switch (((LPNMHDR) lParam)->code)
		{ 
		case LVN_GETDISPINFO:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				if (pnmv->hdr.hwndFrom == GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST))
					GetSatelliteDispInfo((LV_DISPINFO *) lParam);
				else if (pnmv->hdr.hwndFrom == GetDlgItem(hDlg, IDC_TUNER_MUX_LIST))
					GetMuxDispInfo((LV_DISPINFO *) lParam);
			}
			break;
		case LVN_ITEMCHANGED:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				if (pnmv->hdr.hwndFrom == GetDlgItem(hDlg, IDC_TUNER_SATELLITE_LIST))
				{
					LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
					if (pnmv->uNewState & LVIS_SELECTED)
					{
						UpdateMuxList(hDlg, pnmv->iItem);
						v->nCurrentlySelectedMux = -1;
					}
				}
				else if (pnmv->hdr.hwndFrom == GetDlgItem(hDlg, IDC_TUNER_MUX_LIST))
				{
					LPNMLISTVIEW pnmv = (LPNMLISTVIEW)lParam;
					if (pnmv->uNewState & LVIS_SELECTED)
						UpdateSatelliteMux(hDlg, pnmv->iItem);
				}
			}
			break;
		case NM_DBLCLK:
			{
				NMLVDISPINFO * pnmv = (NMLVDISPINFO *)lParam;
				if (pnmv->hdr.hwndFrom == GetDlgItem(hDlg, IDC_TUNER_MUX_LIST))
					PostMessage(hDlg, WM_COMMAND, IDOK, 0);
			}
			break;
		}
		break;
	case WM_TIMER:
		if (v->fCtrlDown == FALSE)
		{
			if (GetKeyState(VK_CONTROL) & 0x8000)
			{
				v->fCtrlDown = TRUE;
				UpdateMuxTitle(hDlg, szCurrentSatFile);
			}
		}
		else
		{
			if (!(GetKeyState(VK_CONTROL) & 0x8000))
			{
				v->fCtrlDown = FALSE;
				UpdateMuxTitle(hDlg, NULL);
			}
		}
		break;
	}

	return FALSE;
}

void SourceHelper_ConvertPolarity(char * lpszCmdLine)
{
	char * szSpace = strstr(lpszCmdLine, " ");
	if (szSpace != NULL)
	{
		szSpace++;
		switch(*szSpace)
		{
		case 'H':
		case 'h':
		case 'L':
		case 'l':
			*szSpace = '1';
			break;
		case 'V':
		case 'v':
		case 'R':
		case 'r':
			*szSpace = '0';
			break;
		}
	}
}

BOOL SourceHelper_DSSTuneDialog(HWND hWnd)
{
	v->fADVModulation = FALSE;
	lstrcpy(szTypeName, "DSS");
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_DSS), hWnd, TuneDVBSatelliteDlgProc));
}

BOOL SourceHelper_DVBSTuneDialog(HWND hWnd)
{
	v->fADVModulation = FALSE;
	lstrcpy(szTypeName, "DVB");
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_DVBS), hWnd, TuneDVBSatelliteDlgProc));
}

BOOL SourceHelper_DVBTTuneDialog(HWND hWnd)
{
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_DVBT), hWnd, TuneDVBTerrestrialDlgProc));
}

BOOL SourceHelper_ATSCTuneDialog(HWND hWnd)
{
	v->fQAMMode = FALSE;
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_ATSC), hWnd, TuneATSCQAMDlgProc));
}

BOOL SourceHelper_ISDBTTuneDialog(HWND hWnd)
{
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_ISDBT), hWnd, TuneATSCQAMDlgProc));
}

BOOL SourceHelper_QAMTuneDialog(HWND hWnd)
{
	v->fQAMMode = TRUE;
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_QAM), hWnd, TuneATSCQAMDlgProc));
}

BOOL SourceHelper_DVBCTuneDialog(HWND hWnd)
{
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_DVBC), hWnd, TuneDVBCableDlgProc));
}

BOOL SourceHelper_ADVTuneDialog(HWND hWnd)
{
	v->fADVModulation = TRUE;
	lstrcpy(szTypeName, "DVB");
	return (DialogBox(hInstance, MAKEINTRESOURCE(IDD_TUNE_ADV), hWnd, TuneDVBSatelliteDlgProc));
}

BOOL SourceHelper_UDPMulticastTuneDialog(HWND hWnd)
{
	return (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_TUNE_UDP_MULTICAST), hWnd, TuneSocketDlgProc, SOCKET_MODE_UDP_MULTICAST));
}

BOOL SourceHelper_UDPUnicastTuneDialog(HWND hWnd)
{
	return (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_TUNE_UDP_UNICAST), hWnd, TuneSocketDlgProc, SOCKET_MODE_UDP_UNICAST));
}

BOOL SourceHelper_TCPTuneDialog(HWND hWnd)
{
	return (DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_TUNE_TCP), hWnd, TuneSocketDlgProc, SOCKET_MODE_TCP));
}

#ifdef _DEBUG
int nContinuity[8192];
int nPID;
int nContinuityErrors = 0;
int nPackets = 0;
int nPreviousContinuityErrors = -1;
double dMBReceived = 0.0;
double dNextMBReceivedDisplay = 0.0;
#endif _DEBUG

void SourceHelper_Init(PVARIABLES pv)
{
#ifdef _DEBUG
	int nPID;

	for (nPID = 0; nPID < 8192; nPID++)
		nContinuity[nPID] = -1;
#endif _DEBUG
	v = pv;
}

void SourceHelper_CheckContinuity(BYTE * pBuffer, int nLength)
{
#ifdef _DEBUG
	int nBufferOffset;

	dMBReceived += (double)nLength / 1024.0 / 1024.0;
	nPackets += nLength / 188;

	for (nBufferOffset = 0; nBufferOffset < (int)nLength; nBufferOffset += v->nSyncThread_PacketLength)
	{
		int nPID = (pBuffer[nBufferOffset + 1] << 8 | pBuffer[nBufferOffset + 2]) & 0x1fff;
		if (nPID != 0x1fff)
		{
			int nCurrentContinuity = pBuffer[nBufferOffset + 3] & 0x0f;
			if (nContinuity[nPID] == -1)
				nContinuity[nPID] = nCurrentContinuity;

			if (nContinuity[nPID] != nCurrentContinuity)
			{
				if ((pBuffer[nBufferOffset + 3] & 0x20) != 0x20)
				{
					if (nContinuity[nPID] != nCurrentContinuity)
					{
						nContinuityErrors++;
						nContinuity[nPID] = nCurrentContinuity;
					}
				}
			}
			nContinuity[nPID]++;
			nContinuity[nPID] &= 0x0f;
		}			
	}
	if (dMBReceived > dNextMBReceivedDisplay)
	{
		char szTemp[100];
		sprintf(szTemp, "%d continuity errors %d packets length = %d %.3f MB received\n", nContinuityErrors, nPackets, nLength, dMBReceived);
		OutputDebugString(szTemp);
		nPreviousContinuityErrors = nContinuityErrors;
		dNextMBReceivedDisplay = dMBReceived + 5.0;
	}
#endif _DEBUG
}

int FindSerialModuleIndex()
{
	int nIndex;

	for (nIndex = 0; nIndex < v->nSerialReceiverControlIndex; nIndex++)
	{
		typedef char * (* td_GetReceiverName) ();
		char * (* GetReceiverName) ();

		GetReceiverName = (td_GetReceiverName)GetProcAddress(v->hSerialReceiverControl[nIndex], "GetReceiverName");
		if (GetReceiverName != NULL)
		{
			if (lstrcmp(GetReceiverName(), v->szSerialReceiverType) == 0)
				return nIndex;
		}
	}

	return -1;
}

BOOL SourceHelper_InitSerialControl()
{
	int nSerialIndex = FindSerialModuleIndex();
	
	typedef void (* td_GetSerialParameters) (int *nBaudRate, int *nDataBits, int *nParity, int *nStopBits, BOOL *fDTR);
	void (* GetSerialParameters) (int *nBaudRate, int *nDataBits, int *nParity, int *nStopBits, BOOL *fDTR);

	OutputDebugString("SourceHelper: SourceHelper_InitSerialControl()\n");
	if (nSerialIndex == -1)
	{
		MessageBox(NULL, "Unable to locate serial receiver module", gszAppName, MB_ICONSTOP);
		return FALSE;
	}
	GetSerialParameters = (td_GetSerialParameters)GetProcAddress(v->hSerialReceiverControl[nSerialIndex], "GetSerialParameters");
	if (GetSerialParameters != NULL)
	{
		int nBaudRate, nDataBits, nParity, nStopBits;
		BOOL fDTR;

		GetSerialParameters(&nBaudRate, &nDataBits, &nParity, &nStopBits, &fDTR);
		if (nBaudRate == 0)
			return TRUE;
		if (OpenSerialPort(nBaudRate, nDataBits, nParity, nStopBits, fDTR) == TRUE)
			return TRUE;
	}
	MessageBox(NULL, "Unable to open serial port for receiver control", gszAppName, MB_ICONSTOP);
	return FALSE;
}

BOOL SourceHelper_DeInitSerialControl()
{
	OutputDebugString("SourceHelper: SourceHelper_DeInitSerialControl()\n");
	if (v->fSerialReceiverControlThreadRunning == TRUE)
		CloseSerialPort();
	return TRUE;
}

BOOL SourceHelper_SetChannelSerialControl(int nSID, int nTSID, int nNID)
{
	typedef BOOL (* td_SetChannel) (int nSID, int nTSID, int nNID);
	BOOL (* SetChannel) (int nSID, int nTSID, int nNID);

	int nSerialIndex = FindSerialModuleIndex();
	char szTemp[256];
	
	wsprintf(szTemp, "SourceHelper: SourceHelper_SetChannelSerialControl(%d,%d,%d) with nSerialReceiverType = %s\n", nSID, nTSID, nNID, v->szSerialReceiverType);
	OutputDebugString(szTemp);

	if (nSerialIndex == -1)
	{
		MessageBox(NULL, "Unable to locate serial receiver module", gszAppName, MB_ICONSTOP);
		return FALSE;
	}
	SetChannel = (td_SetChannel)GetProcAddress(v->hSerialReceiverControl[nSerialIndex], "SetChannel");
	if (SetChannel != NULL)
		return SetChannel(nSID, nTSID, nNID);

	return FALSE;
}

BOOL SourceHelper_TuneSerialControl(char * szTunerString)
{
	typedef BOOL (* td_TuneReceiver) (PSOURCESTRUCT ss, char * szTunerString);
	BOOL (* TuneReceiver) (PSOURCESTRUCT ss, char * szTunerString);

	int nSerialIndex = FindSerialModuleIndex();

	if (nSerialIndex == -1)
	{
		MessageBox(NULL, "Unable to locate serial receiver module", gszAppName, MB_ICONSTOP);
		return FALSE;
	}
	TuneReceiver = (td_TuneReceiver)GetProcAddress(v->hSerialReceiverControl[nSerialIndex], "TuneReceiver");
	if (TuneReceiver != NULL)
		return TuneReceiver(&v->ss, szTunerString);

	return FALSE;
}

void SourceHelper_SerialControlStart()
{
	typedef void (* td_StartSerial) (PSOURCESTRUCT ss);
	void (* StartSerial) (PSOURCESTRUCT ss);

	int nSerialIndex = FindSerialModuleIndex();
	if (nSerialIndex == -1)
	{
		MessageBox(NULL, "Unable to locate serial receiver module", gszAppName, MB_ICONSTOP);
		return;
	}
	StartSerial = (td_StartSerial)GetProcAddress(v->hSerialReceiverControl[nSerialIndex], "StartSerial");
	if (StartSerial != NULL)
		StartSerial(&v->ss);
	return;
}

void SourceHelper_SerialControlStop()
{
	typedef void (* td_StopSerial) (PSOURCESTRUCT ss);
	void (* StopSerial) (PSOURCESTRUCT ss);

	int nSerialIndex = FindSerialModuleIndex();
	if (nSerialIndex == -1)
	{
		MessageBox(NULL, "Unable to locate serial receiver module", gszAppName, MB_ICONSTOP);
		return;
	}
	StopSerial = (td_StopSerial)GetProcAddress(v->hSerialReceiverControl[nSerialIndex], "StopSerial");
	if (StopSerial != NULL)
		StopSerial(&v->ss);
	return;
}

BOOL SourceHelper_SerialControlGetSignal(char * szSignalString)
{
	typedef BOOL (* td_GetSignal) (char * szSignalString);
	BOOL (* GetSignal) (char * szSignalString);

	int nSerialIndex = FindSerialModuleIndex();
	if (nSerialIndex == -1)
	{
		MessageBox(NULL, "Unable to locate serial receiver module", gszAppName, MB_ICONSTOP);
		return FALSE;
	}
	GetSignal = (td_GetSignal)GetProcAddress(v->hSerialReceiverControl[nSerialIndex], "GetSignal");
	if (GetSignal != NULL)
		return GetSignal(szSignalString);
	return FALSE;
}

void SourceHelper_ResetFirstTimeFlag()
{
	int nIndex;

	fTuneDialogFirstTime = TRUE;
	for (nIndex = 0; nIndex < MAX_SATELLITES; nIndex++)
	{
		if (v->sats[nIndex].nOrbitalPosition != -1)
		{
			if (v->sats[nIndex].mux != NULL)
				LocalFree(v->sats[nIndex].mux);
			memset(&v->sats[nIndex], 0, sizeof(SATELLITE));
			v->sats[nIndex].nOrbitalPosition = -1;
		}
	}

}

BOOL SourceHelper_ValidateSourceContainer(PSOURCESTRUCT pss)
{
	int i;

	char szContainerName[MAX_PATH] = {0};
	char szExecutableName[MAX_PATH] = {0};

	GetModuleFileName(pss->hTSReaderInst, szContainerName, sizeof(szContainerName));
	for (i = lstrlen(szContainerName); i > 0; i--)
	{
		if (szContainerName[i] == '\\')
		{
			lstrcpy(szExecutableName, &szContainerName[i + 1]);
			break;
		}
	}

	strlwr(szExecutableName);
	if (lstrcmp(szExecutableName, "tsreader.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "tsreaderlite.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "dvbapps.viewer.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "tsreaderineoquest.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "tsreaderpro.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "tsreaderpro-protected.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "zanalyzer.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "mpeg2output.exe") == 0)
		return TRUE;
	if (lstrcmp(szExecutableName, "dtvsentinel.exe") == 0)
		return TRUE;

	return FALSE;
}

void GetSatcoDXsz(char * szInputLine, int nStartCol, int nEndCol, char * szOutput)
{
	int nLength;

	memcpy(szOutput, &szInputLine[nStartCol - 1], nEndCol - nStartCol + 1);
	szOutput[nEndCol - nStartCol + 1] = '\0';
	nLength = lstrlen(szOutput) - 1;
	while(szOutput[nLength] == ' ' && nLength >= 0)
	{
		szOutput[nLength] = '\0';
		nLength--;
	}
}

int GetSatcoDXint(char * szInputLine, int nStartCol, int nEndCol)
{
	int nRetVal = 0;
	char szTemp[128];

	GetSatcoDXsz(szInputLine, nStartCol, nEndCol, szTemp);
	sscanf(szTemp, "%d", &nRetVal);

	return nRetVal;

}

int __cdecl SortSDXBySatelliteCompareFunction(const void *elem1, const void *elem2)
{
	PSDXBUILDLIST pSDX1 = (PSDXBUILDLIST)elem1;
	PSDXBUILDLIST pSDX2 = (PSDXBUILDLIST)elem2;

	if (pSDX1->nOrbital < pSDX2->nOrbital)
		return -1;
	if (pSDX1->nOrbital > pSDX2->nOrbital)
		return 1;
	return 0;
}

int __cdecl SortSDXByFrequencyCompareFunction(const void *elem1, const void *elem2)
{
	PSDXBUILDLIST pSDX1 = (PSDXBUILDLIST)elem1;
	PSDXBUILDLIST pSDX2 = (PSDXBUILDLIST)elem2;

	if (pSDX1->nFrequency < pSDX2->nFrequency)
		return -1;
	if (pSDX1->nFrequency > pSDX2->nFrequency)
		return 1;
	return 0;
}

void RemoveOldFiles(char * szOutputDirectory, PSDXBUILDLIST pSDX, int nItemCount)
{	
	int nIndex;
	int nCurrentOrbital = -1;

	for (nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		if (nCurrentOrbital != pSDX[nIndex].nOrbital)
		{
			char szKillName[MAX_PATH];
			char szLOF[16];
			char szDiSEqCInput[16];

			nCurrentOrbital = pSDX[nIndex].nOrbital;
			wsprintf(szKillName, "%s\\%04d.ini", szOutputDirectory, nCurrentOrbital);

			// See if we have a TSREADER section - if we do, preserve
			GetPrivateProfileString("TSREADER", "LOF", "", szLOF, sizeof(szLOF), szKillName);
			GetPrivateProfileString("TSREADER", "DISEQC", "", szDiSEqCInput, sizeof(szDiSEqCInput), szKillName);

			// Now we can delete
			DeleteFile(szKillName);

			// Recreate TSREADER section if present
			if (lstrlen(szLOF) && lstrlen(szDiSEqCInput))
			{
				WritePrivateProfileString("TSREADER", "LOF", szLOF, szKillName);
				WritePrivateProfileString("TSREADER", "DISEQC", szDiSEqCInput, szKillName);
			}
		}
	}
}

void WriteMuxCounts(int nDVBMuxCount, int nDSSMuxCount, int nADVMuxCount, char * szCurrentINIFile)
{
	char szMuxCount[32];

	if (nDVBMuxCount)
	{
		wsprintf(szMuxCount, "%d", nDVBMuxCount);
		WritePrivateProfileString("DVB", "0", szMuxCount, szCurrentINIFile);
	}
	if (nADVMuxCount)
	{
		wsprintf(szMuxCount, "%d", nADVMuxCount);
		WritePrivateProfileString("ADV", "0", szMuxCount, szCurrentINIFile);
	}
	if (nDSSMuxCount)
	{
		wsprintf(szMuxCount, "%d", nDSSMuxCount);
		WritePrivateProfileString("DSS", "0", szMuxCount, szCurrentINIFile);
	}
}

void WriteINIFiles(HWND hDlg, PSDXBUILDLIST pSDX, int nItemCount)
{
	int nIndex;
	int nCurrentOrbital = -1;
	int nStartItem = 0;
	int nSortCount = 0;
	int nDVBMuxCount;
	int nADVMuxCount;
	int nDSSMuxCount;
	char szOutputDirectory[MAX_PATH];
	char szTSReaderDir[MAX_PATH];
	char szCurrentINIFile[MAX_PATH];

	if (MessageBox(hDlg, "Warning: This will overwrite all existing data for the satellites being imported.\n\nAre you sure you want to do this?", gszAppName, MB_ICONWARNING | MB_YESNO) == IDNO)
		return;

	SourceHelper_GetTSReaderEXEDirectory(v->hInstance, szTSReaderDir, sizeof(szTSReaderDir));
	wsprintf(szOutputDirectory, "%s\\Satellites", szTSReaderDir);

	// Now sort the list - first by satellite and then by frequency
	SetDlgItemText(hDlg, IDC_STATUS, "Sorting - phase 1");
	qsort(pSDX, nItemCount, sizeof(SDXBUILDLIST), SortSDXBySatelliteCompareFunction);

	SetDlgItemText(hDlg, IDC_STATUS, "Sorting - phase 2");
	for (nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		if (nCurrentOrbital != pSDX[nIndex].nOrbital)
		{
			if (nIndex)
			{
				qsort(&pSDX[nStartItem], nSortCount, sizeof(SDXBUILDLIST), SortSDXByFrequencyCompareFunction);
				nSortCount = 0;
				nStartItem = nIndex;
			}
			nCurrentOrbital = pSDX[nIndex].nOrbital;
		}
		nSortCount++;
	}
	if (nSortCount)
		qsort(&pSDX[nStartItem], nSortCount, sizeof(SDXBUILDLIST), SortSDXByFrequencyCompareFunction);

	// Now we can write the INI files
	SetDlgItemText(hDlg, IDC_STATUS, "Removing old INI files");
	RemoveOldFiles(szOutputDirectory, pSDX, nItemCount);
	nCurrentOrbital = -1;
	for (nIndex = 0; nIndex < nItemCount; nIndex++)
	{
		char szPolarity[2];
		char szCodeRate[4];
		char szMuxCount[24];
		char szMuxData[128];

		if (nCurrentOrbital != pSDX[nIndex].nOrbital)
		{
			char szOrbitalPosition[8];

			if (nIndex != 0)
			{
				WriteMuxCounts(nDVBMuxCount, nDSSMuxCount, nADVMuxCount, szCurrentINIFile);
			}
			nCurrentOrbital = pSDX[nIndex].nOrbital;
			nDVBMuxCount = nADVMuxCount = nDSSMuxCount = 0;
			wsprintf(szCurrentINIFile, "%s\\%04d.ini", szOutputDirectory, nCurrentOrbital);
			{
				char szTemp[MAX_PATH];
				wsprintf(szTemp, "Writing %s\n", szCurrentINIFile);
				SetDlgItemText(hDlg, IDC_STATUS, szTemp);
			}

			wsprintf(szOrbitalPosition, "%04d", pSDX[nIndex].nOrbital);
			WritePrivateProfileString("SATTYPE", "1", szOrbitalPosition, szCurrentINIFile);
			WritePrivateProfileString("SATTYPE", "2", pSDX[nIndex].szSatelliteName, szCurrentINIFile);
		}

		switch(pSDX[nIndex].nPolarity)
		{
		case 0:
			lstrcpy(szPolarity, "V");
			break;
		case 1:
			lstrcpy(szPolarity, "H");
			break;
		case 2:
			lstrcpy(szPolarity, "L");
			break;
		case 3:
			lstrcpy(szPolarity, "R");
			break;
		default:
			szPolarity[0] = '\0';
		}

		switch(pSDX[nIndex].nCodeRate)
		{
		case 0:
			lstrcpy(szCodeRate, "00");
			break;
		case 1:
			lstrcpy(szCodeRate, "12");
			break;
		case 2:
			lstrcpy(szCodeRate, "23");
			break;
		case 3:
			lstrcpy(szCodeRate, "34");
			break;
		case 5:
			lstrcpy(szCodeRate, "56");
			break;
		case 7:
			lstrcpy(szCodeRate, "78");
			break;
		default:
			szCodeRate[0] = '\0';
		}

		switch(pSDX[nIndex].nModulationType & 0xff)
		{
		case 0:	// DVB
			wsprintf(szMuxData, "%d,%s,%d,%s,%s", 
					 pSDX[nIndex].nFrequency / 1000, 
					 szPolarity,
					 pSDX[nIndex].nSymbolRate,
					 szCodeRate,
					 pSDX[nIndex].szMuxName);
			wsprintf(szMuxCount, "%d", ++nDVBMuxCount);
			WritePrivateProfileString("DVB", szMuxCount, szMuxData, szCurrentINIFile);
			break;
		case 1:	// DCII
			{
				int nModeValue = 0;
				int nCodeRateValue = 8;	// auto
				int nModulationMode = pSDX[nIndex].nModulationType >> 8;
				
				if (nModulationMode == 0)
				{
					if (pSDX[nIndex].nSymbolRate < 19510)
						nModeValue = 7;		// DCII OQPSK
					else if (pSDX[nIndex].nSymbolRate == 19510)
						nModeValue = 4;		// DCII combo QPSK
					else nModeValue = 5;	// DCII split QPSK-I
				}
				else
				{
					nModeValue = nModulationMode;
					nCodeRateValue = pSDX[nIndex].nCodeRate;
				}

				wsprintf(szMuxData, "%d,%s,%d,%d,%d,%s",
						 pSDX[nIndex].nFrequency / 1000, 
						 szPolarity,
						 pSDX[nIndex].nSymbolRate,
						 nCodeRateValue,
						 nModeValue,
						 pSDX[nIndex].szMuxName);
				wsprintf(szMuxCount, "%d", ++nADVMuxCount);
				WritePrivateProfileString("ADV", szMuxCount, szMuxData, szCurrentINIFile);
			}
			break;
		case 2:	// DSS
			wsprintf(szMuxData, "%d,%s,%d,%s,%s", 
					 pSDX[nIndex].nFrequency / 1000, 
					 szPolarity,
					 pSDX[nIndex].nSymbolRate,
					 szCodeRate,
					 pSDX[nIndex].szMuxName);
			wsprintf(szMuxCount, "%d", ++nDSSMuxCount);
			WritePrivateProfileString("DSS", szMuxCount, szMuxData, szCurrentINIFile);
			break;
		}
	}
	WriteMuxCounts(nDVBMuxCount, nDSSMuxCount, nADVMuxCount, szCurrentINIFile);
}

// DCII Satellite	Band	Frequency	Link Frequency	Polarity	Generic	VC	Description	SR	FEC	Service Number	Bitstream	Source	Updated	Updater	Encryption	RotateLNB
// DVB  Satellite	Position	Band	DL Frequency	Polarity	Description	SR	Modulation	FEC	VPID	UM	Spectrum Inversion	APID	PCR	Transport ID	 Network ID	Service ID	4:2:2 Video	AC-3 Audio	Source	Updated	Updater	Encryption

#define DVB_SATELLITE 0
#define DVB_POSITION 1
#define DVB_BAND 2
#define DVB_FREQUENCY 3
#define DVB_POLARITY 4
#define DVB_DESCRIPTION 5
#define DVB_SR 6
#define DVB_MOD 7
#define DVB_FEC 8

#define DCII_SATELLITE 0
#define DCII_BAND 1
#define DCII_FREQUENCY 2
#define DCII_LBAND 3
#define DCII_POLARITY 4
#define DCII_GENERIC 5
#define DCII_VC 6
#define DCII_DESCRIPTION 7
#define DCII_SR 8
#define DCII_FEC 9
#define DCII_SERVICE_NUMBER 10
#define DCII_BITSTREAM 11

DWORD WINAPI ImportCSVThread(LPVOID lpv)
{
	HWND hDlg = (HWND)lpv;
	HANDLE hDVBFile, hDCIIFile = INVALID_HANDLE_VALUE;
	BOOL fFirstLine;
	int nSDXCount;
	BOOL fComplete = FALSE;
	char szLine[1024];
	PSDXBUILDLIST pSDX;
	
	hDVBFile = CreateFile(v->szDVBCSVFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
	if (hDVBFile == INVALID_HANDLE_VALUE)
	{
		MessageBox(hDlg, "Unable to open DVB file specified", gszAppName, MB_ICONSTOP);
		goto Windup_ImportCSV;
	}
	if (lstrlen(v->szDCIICSVFile))
	{
		hDCIIFile = CreateFile(v->szDCIICSVFile, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
		if (hDCIIFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hDVBFile);
			MessageBox(hDlg, "Unable to open DCII file specified", gszAppName, MB_ICONSTOP);
			goto Windup_ImportCSV;
		}
	}

	pSDX = LocalAlloc(LPTR, sizeof(SDXBUILDLIST) * 20000);
	nSDXCount = 0;

	// Read the DVB file first
	SetDlgItemText(hDlg, IDC_STATUS, "Reading DVB CSV file");
	fFirstLine = TRUE;
	while (SourceHelper_ReadLineUnix(hDVBFile, szLine, sizeof(szLine)))
	{
		BOOL fGotThisOne = FALSE;
		int nIndex;
		int nPolarity = 0;
		int nCodeRate = 0;
		int nSymbolRate = 0;
		int nItem = 0;
		char * szCurrent = szLine;
		char * szNextComma;
		char szCommaValue[32][100];

		if (fFirstLine)
		{
			fFirstLine = FALSE;
			continue;
		}

		do
		{
			szNextComma = strstr(szCurrent, ",");
			if (szNextComma != NULL)
				*szNextComma = '\0';
			lstrcpy(szCommaValue[nItem++], szCurrent);
			szCurrent = szNextComma + 1;
		} while (szNextComma != NULL);

		if (nItem)
		{
			int nOrbital;

			nSymbolRate = atoi(szCommaValue[DVB_SR]);
			if (nSymbolRate == 0)
				continue;

			strupr(szCommaValue[DVB_POLARITY]);
			if (szCommaValue[DVB_POLARITY][0] == 'V')
				nPolarity = 0;
			else if (szCommaValue[DVB_POLARITY][0] == 'H')
				nPolarity = 1;
			else if (szCommaValue[DVB_POLARITY][0] == 'L')
				nPolarity = 2;
			else if (szCommaValue[DVB_POLARITY][0] == 'R')
				nPolarity = 3;

			if (strstr(szCommaValue[DVB_FEC], "1/2") != NULL)
				nCodeRate = 1;
			else if (strstr(szCommaValue[DVB_FEC], "2/3") != NULL)
				nCodeRate = 2;
			else if (strstr(szCommaValue[DVB_FEC], "3/4") != NULL)
				nCodeRate = 3;
			else if (strstr(szCommaValue[DVB_FEC], "5/6") != NULL)
				nCodeRate = 5;
			else if (strstr(szCommaValue[DVB_FEC], "7/8") != NULL)
				nCodeRate = 7;

			nOrbital = (360 - atoi(szCommaValue[DVB_POSITION])) * 10;
			for (nIndex = 0; nIndex < nSDXCount; nIndex++)
			{
				if (pSDX[nIndex].nFrequency == 0)
					break;	// end of the list - no point comparing
				if (nOrbital == pSDX[nIndex].nOrbital)
				{
					if (atoi(szCommaValue[DVB_FREQUENCY]) * 1000 == pSDX[nIndex].nFrequency)
					{
						if (nPolarity == pSDX[nIndex].nPolarity && nSymbolRate == pSDX[nIndex].nSymbolRate)
						{
							fGotThisOne = TRUE;
							break;
						}
					}
				}
			}
			if (!fGotThisOne)
			{
				pSDX[nIndex].nCodeRate = nCodeRate;
				pSDX[nIndex].nFrequency = atoi(szCommaValue[DVB_FREQUENCY]) * 1000;
				pSDX[nIndex].nOrbital = nOrbital;
				pSDX[nIndex].nPolarity = nPolarity;
				pSDX[nIndex].nSymbolRate = nSymbolRate;
				lstrcpy(pSDX[nIndex].szSatelliteName, szCommaValue[DVB_SATELLITE]);
				lstrcpy(pSDX[nIndex].szMuxName, szCommaValue[DVB_DESCRIPTION]);
				nSDXCount++;
			}
		}
	}

	// Now do the DCII file
	if (hDCIIFile != INVALID_HANDLE_VALUE)
	{
		SetDlgItemText(hDlg, IDC_STATUS, "Reading DCII CSV file");
		fFirstLine = TRUE;
		while (SourceHelper_ReadLineUnix(hDCIIFile, szLine, sizeof(szLine)))
		{
			BOOL fGotThisOne = FALSE;
			int nIndex;
			int nPolarity = 0;
			int nCodeRate = 8;		// default to auto code rate

			char * szCurrent = szLine;
			int nItem = 0;
			char * szNextComma;
			char szCommaValue[32][100];

			if (fFirstLine)
			{
				fFirstLine = FALSE;
				continue;
			}

			do
			{
				szNextComma = strstr(szCurrent, ",");
				if (szNextComma != NULL)
					*szNextComma = '\0';
				lstrcpy(szCommaValue[nItem++], szCurrent);
				szCurrent = szNextComma + 1;
			} while (szNextComma != NULL);

			if (nItem)
			{
				int nOrbital;
				int nModulationMode = 0;

				strupr(szCommaValue[DCII_POLARITY]);
				if (szCommaValue[DCII_POLARITY][0] == 'V')
					nPolarity = 0;
				else if (szCommaValue[DCII_POLARITY][0] == 'H')
					nPolarity = 1;
				else if (szCommaValue[DCII_POLARITY][0] == 'L')
					nPolarity = 2;
				else if (szCommaValue[DCII_POLARITY][0] == 'R')
					nPolarity = 3;

				if (strstr(szCommaValue[DCII_FEC], "5/11") != NULL)
					nCodeRate = 0;
				else if (strstr(szCommaValue[DCII_FEC], "1/2") != NULL)
					nCodeRate = 1;
				else if (strstr(szCommaValue[DCII_FEC], "3/5") != NULL)
					nCodeRate = 2;
				else if (strstr(szCommaValue[DCII_FEC], "2/3") != NULL)
					nCodeRate = 3;
				else if (strstr(szCommaValue[DCII_FEC], "3/4") != NULL)
					nCodeRate = 4;
				else if (strstr(szCommaValue[DCII_FEC], "4/5") != NULL)
					nCodeRate = 5;
				else if (strstr(szCommaValue[DCII_FEC], "5/6") != NULL)
					nCodeRate = 6;
				else if (strstr(szCommaValue[DCII_FEC], "7/8") != NULL)
					nCodeRate = 7;

				strupr(szCommaValue[DCII_BITSTREAM]);
				if ( (szCommaValue[DCII_BITSTREAM][0] == 'B') && (atoi(szCommaValue[DCII_SR]) >= 19510) )
					nModulationMode = 0x400; // DCII QPSK C
				else if (szCommaValue[DCII_BITSTREAM][0] == 'I')
					nModulationMode = 0x500; // DCII QPSK I
				else if (szCommaValue[DCII_BITSTREAM][0] == 'Q')
					nModulationMode = 0x600; // DCII QPSK Q
				else if ( (szCommaValue[DCII_BITSTREAM][0] == 'B') && (atoi(szCommaValue[DCII_SR]) < 19510) )
					nModulationMode = 0x700; // DCII OQPSK C

				nOrbital = -1;
				for (nIndex = 0; nIndex < nSDXCount; nIndex++)
				{
					if (lstrcmp(szCommaValue[DCII_SATELLITE], pSDX[nIndex].szSatelliteName) == 0)
					{
						nOrbital = pSDX[nIndex].nOrbital;
						break;
					}
				}
				if (nOrbital == -1)
					continue;

				for (nIndex = 0; nIndex < nSDXCount; nIndex++)
				{
					if (pSDX[nIndex].nFrequency == 0)
						break;	// end of the list - no point comparing
					if (nOrbital == pSDX[nIndex].nOrbital)
					{
						if (atoi(szCommaValue[DCII_FREQUENCY]) * 1000 == pSDX[nIndex].nFrequency)
						{
							if (nPolarity == pSDX[nIndex].nPolarity)
							{
								if (nModulationMode)
								{
									if (nModulationMode == (pSDX[nIndex].nModulationType & 0xff00))
									{
										fGotThisOne = TRUE;
										break;
									}
								}
								else
								{
									fGotThisOne = TRUE;
									break;
								}
							}
						}
					}
				}
				if (!fGotThisOne)
				{
					pSDX[nIndex].nCodeRate = nCodeRate;
					pSDX[nIndex].nFrequency = atoi(szCommaValue[DCII_FREQUENCY]) * 1000;
					pSDX[nIndex].nOrbital = nOrbital;
					pSDX[nIndex].nPolarity = nPolarity;
					pSDX[nIndex].nSymbolRate = atoi(szCommaValue[DCII_SR]);
					pSDX[nIndex].nModulationType = nModulationMode | 1;	// DCII signal + mod mode if exists
					lstrcpy(pSDX[nIndex].szSatelliteName, szCommaValue[DCII_SATELLITE]);
					lstrcpy(pSDX[nIndex].szMuxName, szCommaValue[DCII_DESCRIPTION]);
					nSDXCount++;
				}
			}
		}
	}

	WriteINIFiles(hDlg, pSDX, nSDXCount);

	LocalFree(pSDX);
	CloseHandle(hDVBFile);
	if (hDCIIFile != INVALID_HANDLE_VALUE)
		CloseHandle(hDCIIFile);
	fComplete = TRUE;

Windup_ImportCSV:
	EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
	if (fComplete)
		PostMessage(hDlg, WM_CLOSE, 0, 0);
	return 0;
}

BOOL CALLBACK ImportCSVDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_DVB_FILENAME, v->szDVBCSVFile);
		SetDlgItemText(hDlg, IDC_DCII_FILENAME, v->szDCIICSVFile);
		SetDlgItemText(hDlg, IDC_STATUS, "");
		SendDlgItemMessage(hDlg, IDC_DVB_FILENAME, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_DVB_FILENAME));
		break;
	case WM_CLOSE:
		if (IsWindowEnabled(GetDlgItem(hDlg, IDCANCEL)) == TRUE)
			EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				HANDLE hThread;
				DWORD dwThreadID;

				GetDlgItemText(hDlg, IDC_DVB_FILENAME, v->szDVBCSVFile, sizeof(v->szDVBCSVFile));
				GetDlgItemText(hDlg, IDC_DCII_FILENAME, v->szDCIICSVFile, sizeof(v->szDCIICSVFile));
				if (lstrlen(v->szDCIICSVFile) > 0 && lstrlen(v->szDVBCSVFile) == 0)
				{
					MessageBox(hDlg, "If you import a DCII file, you must also import a DVB file", gszAppName, MB_ICONSTOP);
					break;
				}
				EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
				hThread = CreateThread(NULL, 0, ImportCSVThread, (LPVOID)hDlg, 0, &dwThreadID);
				CloseHandle(hThread);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_BROWSE_DVB:
		case IDC_BROWSE_DCII:
			{
				int i;
				OPENFILENAME ofn;
				char szInputFile[MAX_PATH] = {0};
				char szInitialDir[MAX_PATH] = {0};

				memset( &(ofn), 0, sizeof(ofn));
				ofn.lStructSize	= sizeof(ofn);
				ofn.hwndOwner = hDlg;
				ofn.lpstrFile = szInputFile;
				ofn.nMaxFile = MAX_PATH;
				ofn.lpstrFilter = TEXT("CSV Files(*.csv)\0*.csv\0All Files (*.*)\0*.*\0\0");	
				ofn.lpstrDefExt = TEXT("csv");
				ofn.Flags =  OFN_HIDEREADONLY;
				if (LOWORD(wParam) == IDC_BROWSE_DVB)
				{
					ofn.lpstrTitle = TEXT("Select DVB CSV file");
					lstrcpy(szInitialDir, v->szDVBCSVFile);
				}
				else
				{
					ofn.lpstrTitle = TEXT("Select DCII CSV file");
					lstrcpy(szInitialDir, v->szDCIICSVFile);
				}
				for (i = lstrlen(szInitialDir); i > 0; i--)
				{
					if (szInitialDir[i] == '\\')
					{
						szInitialDir[i] = '\0';
						break;
					}
				}
				ofn.lpstrInitialDir = szInitialDir;			
				if (SourceHelper_myGetOpenFileName(&ofn) == TRUE)
				{
					if (LOWORD(wParam) == IDC_BROWSE_DVB)
						SetDlgItemText(hDlg, IDC_DVB_FILENAME, szInputFile);
					else
						SetDlgItemText(hDlg, IDC_DCII_FILENAME, szInputFile);
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

DWORD WINAPI ImportSDXThread(LPVOID lpv)
{
	HWND hDlg = (HWND)lpv;
	BOOL fComplete = FALSE;
	int nItemCount = 0;
	int nIndex;
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	PSDXBUILDLIST pSDX = LocalAlloc(LPTR, sizeof(SDXBUILDLIST) * 20000);
	char szSearchName[MAX_PATH];

	lstrcpy(szSearchName, v->szSDXDefaultFolder);
	lstrcat(szSearchName, "\\*.sdx");
	hFind = FindFirstFile(szSearchName, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			HANDLE hSDXFile;
			char szSDXFilename[MAX_PATH];

			if ((fd.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				continue;				
			lstrcpy(szSDXFilename, v->szSDXDefaultFolder);
			lstrcat(szSDXFilename, "\\");
			lstrcat(szSDXFilename, fd.cFileName);
			hSDXFile = CreateFile(szSDXFilename, GENERIC_READ, 0, (LPSECURITY_ATTRIBUTES) NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE) NULL);
			if (hSDXFile != INVALID_HANDLE_VALUE)
			{
				char szLine[512];

				wsprintf(szLine, "Processing %s\n", szSDXFilename);
				SetDlgItemText(hDlg, IDC_STATUS, szLine);

				while (SourceHelper_ReadLine(hSDXFile, szLine, sizeof(szLine)))
				{
					int nSettingsPage, nVersionNumber;
					int nFrequency, nPolarity, nSymbolRate, nCodeRate, nOrbital;
					BOOL fGotThisOne = FALSE;
					char szHeader[8];
					char szSatelliteName[24];
					char szTypeOfChannel[2];
					char szBroadcastType[8];
					char szChannelName[12];
					
					GetSatcoDXsz(szLine, 1, 7, szHeader);
					nSettingsPage = GetSatcoDXint(szLine, 8, 8);
					nVersionNumber = GetSatcoDXint(szLine, 9, 10);
					if (nVersionNumber != 3)
					{
						char szTemp[128 + MAX_PATH];
						wsprintf(szTemp, "Unable to prcocess file %s since it contains a non-version 3 SDX record", szSDXFilename);
						MessageBox(hDlg, szTemp, gszAppName, MB_ICONWARNING);
						goto Windup_ImportSDX;
					}
					GetSatcoDXsz(szLine, 11, 28, szSatelliteName);
					GetSatcoDXsz(szLine, 29, 29, szTypeOfChannel);
					GetSatcoDXsz(szLine, 30, 33, szBroadcastType);
					nFrequency = GetSatcoDXint(szLine, 34, 42);
					nPolarity = GetSatcoDXint(szLine, 43, 43);
					GetSatcoDXsz(szLine, 44, 51, szChannelName);
					nOrbital = GetSatcoDXint(szLine, 52, 55);
					nSymbolRate = GetSatcoDXint(szLine, 70, 74);
					nCodeRate = GetSatcoDXint(szLine, 75, 75);

					if (szTypeOfChannel[0] != '_')
					{
						if (   (lstrcmp(szBroadcastType, "ADR_") == 0)
							|| (lstrcmp(szBroadcastType, "BMAC") == 0)
							|| (lstrcmp(szBroadcastType, "D2MC") == 0)
							|| (lstrcmp(szBroadcastType, "MUSE") == 0)
							|| (lstrcmp(szBroadcastType, "NTSC") == 0)
							|| (lstrcmp(szBroadcastType, "PAL_") == 0)
							|| (lstrcmp(szBroadcastType, "SECM") == 0) )
						{
							continue;
						}
					}
					if (nSymbolRate == 0)
						continue;

					// See if we have this one
					for (nIndex = 0; nIndex < nItemCount; nIndex++)
					{
						if (pSDX[nIndex].nFrequency == 0)
							break;	// end of the list - no point comparing
						if (nOrbital == pSDX[nIndex].nOrbital)
						{
							if (nFrequency == pSDX[nIndex].nFrequency)
							{
								if (nPolarity == pSDX[nIndex].nPolarity)
								{
									fGotThisOne = TRUE;
									break;
								}
							}
						}
					}
					if (!fGotThisOne)
					{
						pSDX[nIndex].nCodeRate = nCodeRate;
						pSDX[nIndex].nFrequency = nFrequency;
						pSDX[nIndex].nOrbital = nOrbital;
						pSDX[nIndex].nPolarity = nPolarity;
						pSDX[nIndex].nSymbolRate = nSymbolRate;
						lstrcpy(pSDX[nIndex].szSatelliteName, szSatelliteName);

						if (lstrcmp(szBroadcastType, "DIC2") == 0)
						{
							pSDX[nIndex].nModulationType = 1;
						}
						else if (lstrcmp(szBroadcastType, "MPEG") == 0)
						{
							pSDX[nIndex].nModulationType = 2;
						}									
						else
						{
							char szTemp[128];
							lstrcpy(szTemp, szChannelName);
							strupr(szTemp);
							if (lstrcmp(szTemp, "DIRECTV") == 0)
								pSDX[nIndex].nModulationType = 2;
							else if (lstrcmp(szTemp, "DIRECTTV") == 0)
								pSDX[nIndex].nModulationType = 2;
							else
							{
								lstrcpy(szTemp, szSatelliteName);
								strupr(szTemp);
								if (strstr(szTemp, "DIRECTV") != NULL)
									pSDX[nIndex].nModulationType = 2;
							}
						}
						nItemCount++;
					}
				}
				CloseHandle(hSDXFile);
			}
		} while (FindNextFile(hFind, &fd) != FALSE);
		FindClose(hFind);

		WriteINIFiles(hDlg, pSDX, nItemCount);
	}
	fComplete = TRUE;
	LocalFree(pSDX);

Windup_ImportSDX:
	EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
	if (fComplete)
		PostMessage(hDlg, WM_CLOSE, 0, 0);
	return 0;

}

BOOL CALLBACK ImportSDXDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_SDX_FOLDER, v->szSDXDefaultFolder);
		SetDlgItemText(hDlg, IDC_STATUS, "");
		SendDlgItemMessage(hDlg, IDC_SDX_FOLDER, EM_SETSEL, 0, -1);
		SetFocus(GetDlgItem(hDlg, IDC_SDX_FOLDER));
		break;
	case WM_CLOSE:
		if (IsWindowEnabled(GetDlgItem(hDlg, IDCANCEL)) == TRUE)
			EndDialog(hDlg, FALSE);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				HANDLE hThread;
				DWORD dwThreadID;

				GetDlgItemText(hDlg, IDC_SDX_FOLDER, v->szSDXDefaultFolder, sizeof(v->szSDXDefaultFolder));
				EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
				hThread = CreateThread(NULL, 0, ImportSDXThread, (LPVOID)hDlg, 0, &dwThreadID);
				CloseHandle(hThread);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			break;
		case IDC_BROWSE_SDX:
			{
				BROWSEINFO BrowsingInfo;
				LPITEMIDLIST ItemID;
				char szDirPath[MAX_PATH];
				char szFolderName[MAX_PATH];

				GetDlgItemText(hDlg, IDC_SDX_FOLDER, v->szSDXDefaultFolder, sizeof(v->szSDXDefaultFolder));
				lstrcpy(szFolderName, v->szSDXDefaultFolder);
				memset(&BrowsingInfo, 0, sizeof(BROWSEINFO));
				memset(szDirPath, 0, MAX_PATH);
				BrowsingInfo.hwndOwner      = hDlg;
				BrowsingInfo.pszDisplayName = szFolderName;
				BrowsingInfo.lpszTitle      = "Select folder containing SDX files";
				BrowsingInfo.ulFlags = 0;//BIF_USENEWUI;
				ItemID = SHBrowseForFolder(&BrowsingInfo);
				if (ItemID)
				{
					SHGetPathFromIDList(ItemID, szDirPath);
					SetDlgItemText(hDlg, IDC_SDX_FOLDER, szDirPath);
				}
			}
			break;
		}
		break;
	}

	return FALSE;
}

void SourceHelper_ImportSatelliteList(HWND hDlg, int nImportType)
{
	switch(nImportType)
	{
	case 0:		// SDX
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_IMPORT_SDX), hDlg, ImportSDXDialogProc);
		break;
	case 1:		// CSV
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_IMPORT_CSV), hDlg, ImportCSVDialogProc);
		break;
	}
}

BOOL SourceHelper_GetTSReaderVersion(int * nMajor, int * nMinor, int * nBuild)
{
	*nMajor = VERSION_MAJOR;
	*nMinor = VERSION_MINOR;
	*nBuild = VERSION_EDIT;

	return TRUE;
}

int ReadFromPipe(BYTE * pBuffer, int nLength)
{
	int nRequestedLength = nLength;
	DWORD dwRead;

	while (nLength)
	{
		ReadFile(v->hSyncThread_PipeRead, pBuffer, nLength, &dwRead, NULL);
		if (dwRead == 0)
			return 0;
		EnterCriticalSection(&v->csSyncThread_PipeBytes);
		v->nSyncThread_PipeBytes -= dwRead;
		LeaveCriticalSection(&v->csSyncThread_PipeBytes);
		pBuffer += dwRead;
		nLength -= dwRead;
	}

	return nRequestedLength;
}

#define SYNCSIZE 8192

DWORD WINAPI ReadPipeThread(LPVOID lpv)
{
	BOOL fAbort = FALSE;
	int nTSBufferIndex = 0;
	int nSync = 0;
	int nRead;
	int nFirstBuffer = 0;
	int nOutputBufferSize = TS_BUFFER_SIZE / 4;
	BYTE syncbuffer[SYNCSIZE];

	OutputDebugString("SourceHelper: +ReadPipeThread\n");
	v->fSyncThread_PipeThreadTerminated = FALSE;
	v->nSyncThread_SyncLossCount = 0;
	do
	{
		if (nSync == 0)
		{
			// Not in sync
			while (nSync == 0 && !fAbort)
			{
				int nOffset = 0;
				int nRead;

				nRead = ReadFromPipe(syncbuffer, SYNCSIZE);
				if (nRead == 0)
				{
					fAbort = TRUE;
					break;
				}
				while (nOffset < SYNCSIZE && !fAbort)
				{
					if (syncbuffer[nOffset] == v->nSyncThread_Syncword)
					{
						int nSecondOffset = nOffset + v->nSyncThread_PacketLength;
						while (nSecondOffset < SYNCSIZE)
						{
							if (syncbuffer[nSecondOffset] == v->nSyncThread_Syncword)
							{
								int nThirdOffset = nSecondOffset + v->nSyncThread_PacketLength;
								
								while (nThirdOffset < SYNCSIZE && !fAbort)
								{
									if (syncbuffer[nThirdOffset] == v->nSyncThread_Syncword)
									{
										int nLength1 = nSecondOffset - nOffset;
										int nLength2 = nThirdOffset - nSecondOffset;

										if (nLength1 == nLength2)
										{
											int nPackets;
											int nCompletePacketBytes;
											int nRemainder;
											int nJunkBytes;

											nSync = nLength1;
											nPackets = (SYNCSIZE - nOffset) / nSync;
											nCompletePacketBytes = nSync * nPackets;
											nRemainder = (SYNCSIZE - nOffset) - nCompletePacketBytes;
											nJunkBytes = nSync - nRemainder;
											ReadFromPipe(syncbuffer, nJunkBytes);
											{
												char szTemp[128];
												wsprintf(szTemp, "SourceHelper: Sync with %d byte packets\n", nSync);
												OutputDebugString(szTemp);
											}
											break;
										}
										else
											nThirdOffset++;
									}
									else
										nThirdOffset++;
								}
								if (nSync)
									break;
								nSecondOffset++;
							}
							else
								nSecondOffset++;
						}
						if (nSync)
							break;
						nOffset++;
					}
					else
						nOffset++;
				}
				if (nSync)
					break;
				if (nOffset == SYNCSIZE)
					continue;	// no SYNC in entire packet
			}
		}
		if (nSync)
		{
			while ((nOutputBufferSize - (ss->tsb[nTSBufferIndex].nSize)) >= v->nSyncThread_PacketLength)
			{
				nRead = ReadFromPipe(ss->tsb[nTSBufferIndex].pData + ss->tsb[nTSBufferIndex].nSize, v->nSyncThread_PacketLength);
				if (nRead == 0)
				{
					fAbort = TRUE;
					break;
				}
				if (nRead != v->nSyncThread_PacketLength)
					OutputDebugString("SourceHelper: read error 5\n");
				if (*(ss->tsb[nTSBufferIndex].pData + ss->tsb[nTSBufferIndex].nSize) != v->nSyncThread_Syncword)
				{
					nSync = 0;
					v->nSyncThread_SyncLossCount++;
					nFirstBuffer = 0;
#ifdef _DEBUG
					OutputDebugString("SourceHelper: Lost sync\n");
#endif _DEBUG
					break;
				}
				ss->tsb[nTSBufferIndex].nSize += nRead;
				if (nSync > v->nSyncThread_PacketLength)
				{
					nRead = ReadFromPipe(syncbuffer, nSync - v->nSyncThread_PacketLength);
					if (nRead == 0)
					{
						fAbort = TRUE;
						break;
					}
					if (nRead != nSync - v->nSyncThread_PacketLength)
						OutputDebugString("SourceHelper: read error 6\n");
				}
			}		 
			
			if (fAbort == FALSE)
			{
				if (nFirstBuffer < 10)
				{
					ss->tsb[nTSBufferIndex].nSize = 0;
					nFirstBuffer++;
				}
				else
				{
					EnterCriticalSection(&ss->csPIDCounter);
					ss->nLastSecondByteCounter += ss->tsb[nTSBufferIndex].nSize;
					LeaveCriticalSection(&ss->csPIDCounter);
					nTSBufferIndex++;
					if (nTSBufferIndex == MAX_TS_BUFFERS)
						nTSBufferIndex = 0;
					EnterCriticalSection(&ss->csTSBuffersInUse);
					ss->nTSBuffersInUse++;
					LeaveCriticalSection(&ss->csTSBuffersInUse);
					ss->tsb[nTSBufferIndex].nSize = 0;
				}
			}
		}
	} while (fAbort == FALSE);

	CloseHandle(v->hSyncThread_PipeRead);
	OutputDebugString("SourceHelper: -ReadPipeThread\n");
	v->fSyncThread_PipeThreadTerminated = TRUE;
	return 0;
}

BOOL SourceHelper_StartSyncThread(PSOURCESTRUCT pss, BOOL fDSSMode)
{
	DWORD dwThreadID;
	HANDLE hPipeReadThread;

	ss = pss;
	InitializeCriticalSection(&v->csSyncThread_PipeBytes);
	v->nSyncThread_PipeBytes = 0;
	switch(fDSSMode)
	{
	case 0:
		v->nSyncThread_Syncword = 0x47;
		v->nSyncThread_PacketLength = 188;
		break;
	case 1:
		v->nSyncThread_Syncword = 0x1d;
		v->nSyncThread_PacketLength = 131;
		break;
	}

	CreatePipe(&v->hSyncThread_PipeRead, &v->hSyncThread_PipeWrite, NULL, 15 * 1024 * 1024);
	hPipeReadThread = CreateThread(NULL, 0, ReadPipeThread, (LPVOID)0, CREATE_SUSPENDED, &dwThreadID);
	switch(ss->nInputThreadPriority)
	{
	case 0:
		SetThreadPriority(hPipeReadThread, THREAD_PRIORITY_NORMAL);
		break;
	case 1:
		SetThreadPriority(hPipeReadThread, THREAD_PRIORITY_HIGHEST);
		break;
	case 2:
		SetThreadPriority(hPipeReadThread, THREAD_PRIORITY_IDLE);
		break;
	case 3:
		SetThreadPriority(hPipeReadThread, THREAD_PRIORITY_TIME_CRITICAL);
		break;
	}
	ResumeThread(hPipeReadThread);
	CloseHandle(hPipeReadThread);

	return TRUE;
}

BOOL SourceHelper_StopSyncThread()
{
	CloseHandle(v->hSyncThread_PipeWrite);
	while (v->fSyncThread_PipeThreadTerminated == FALSE)
		Sleep(10);
	DeleteCriticalSection(&v->csSyncThread_PipeBytes);
	return TRUE;
}

BOOL SourceHelper_SyncData(BYTE * pData, int nLength)
{
	DWORD dwWritten;

	WriteFile(v->hSyncThread_PipeWrite, pData, nLength, &dwWritten, NULL);
	EnterCriticalSection(&v->csSyncThread_PipeBytes);
	v->nSyncThread_PipeBytes += dwWritten;
	LeaveCriticalSection(&v->csSyncThread_PipeBytes);

	return TRUE;
}

int SourceHelper_GetSyncLossCount(BOOL fReset)
{
	if (fReset)
		v->nSyncThread_SyncLossCount = 0;		
	return v->nSyncThread_SyncLossCount;
}

void SourceHelper_GetTSReaderEXEDirectory(HINSTANCE hInstance, char * szCurrentDir, int nCurrentDirLength)
{
	int i;

	GetModuleFileName(hInstance, szCurrentDir, nCurrentDirLength);
	for (i = lstrlen(szCurrentDir); i > 0; i--)
	{
		if (szCurrentDir[i] == '\\')
		{
			szCurrentDir[i] = 0;
			break;
		}
	}
}

BOOL SourceHelper_GetProfileName(char * szBuffer)
{
#ifndef PRO
	return FALSE;
#else PRO
	lstrcpy(szBuffer, v->szProfileName);
	return TRUE;
#endif PRO
}

// =====================================================================
// Stubs for SourceHelper functions added between 2008 and 2018 that
// the in-repo source predates. The prebuilt 2019-2022 source plugins
// (notably the TBS BDA tuner DLLs in Sources_Archive/) import these
// by ordinal and need them to exist for LoadLibrary to succeed.
// Returning FALSE/0/NULL leaves the new functionality silently
// disabled; fully restoring each function would require access to
// Rod's post-2008 SourceHelper source. All stubs are __cdecl (C
// default on x86), so even argument-count mismatches between the
// plugin's call site and our stub are absorbed by the caller-cleans-
// the-stack convention.
// =====================================================================

BOOL SourceHelper_DVBC2TuneDialog(HWND hWnd)
{
	(void)hWnd;
	return FALSE;
}

HANDLE SourceHelper_GetSourceBufferEventHandle(void)
{
	return NULL;
}

void SourceHelper_LogRTPLoss(int nLoss, int nTotal)
{
	(void)nLoss; (void)nTotal;
}

void SourceHelper_OutputDebugString(const char * sz)
{
	if (sz != NULL)
		OutputDebugStringA(sz);
}

BOOL SourceHelper_Parse_CommandLine_ATSC(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

BOOL SourceHelper_Parse_CommandLine_QAM(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

BOOL SourceHelper_Parse_CommandLine_DVBS(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

BOOL SourceHelper_Parse_CommandLine_DVBT(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

BOOL SourceHelper_Parse_CommandLine_DVBC(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

BOOL SourceHelper_Parse_CommandLine_ADV(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

BOOL SourceHelper_Parse_CommandLine_DVBC2(char * szCmd)
{
	(void)szCmd;
	return FALSE;
}

int SourceHelper_ReadLineW(HANDLE hFile, wchar_t * szBuffer, int nMaxLength)
{
	(void)hFile; (void)szBuffer; (void)nMaxLength;
	return 0;
}

BOOL SourceHelper_myGetOpenFileNameW(void * lpofn)
{
	(void)lpofn;
	return FALSE;
}

BOOL SourceHelper_RunningOnWine(void)
{
	return FALSE;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch( ul_reason_for_call )
	{
    case DLL_PROCESS_ATTACH:
		hInstance = hModule;
		fTuneDialogFirstTime = TRUE;
		break;
    case DLL_PROCESS_DETACH:
		break;
    }
    return TRUE;
}

/*
 function GetUSALSPar(Lat, Lg, LgSat: double): WORD;
    var
        angle: double;
        tenth: integer;
    begin
        Lat :=(Pi/180)*Lat;
        Lg :=(Pi/180)*Lg;
        LgSat :=(Pi/180)*LgSat;

        angle := (180/Pi)*ArcTan2((r_earth+sat_orbit)*sin(LgSat-Lg), (r_earth+sat_orbit)*cos(LgSat-Lg)-r_earth*cos(Lat));

        Result:=Round(abs(angle)*10);
        tenth:=Result mod 10;
        Result:=((Result div 10) * 16) + UsalsFract[tenth];

        if(angle<0) then begin
            if(Lat>0)
                then Result := Result or $E000
                else Result := Result or $D000;
        end else begin
            if (Lat>0)
                then Result := Result or $D000
                else Result := Result or $E000;
        end;
    end;

var
    UsalsFract: array[0..9] of integer = ($00,$02,$03,$05,$06,$08,$0A,$0B,$0D,$0E);
const
    r_earth =6378.16;
    sat_orbit =35786.43;

  South -, East -, North +, West +


  */