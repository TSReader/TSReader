// Serial receiver control module for
//  Tandberg Alteia Plus
//    and
//  Tandberg TT1260

#include <windows.h>
#include <stdio.h>
#include "..\..\..\SampleSource\Inc\sources.h"

#ifndef TT1260
	char gszAppName[] = {"Tandberg Alteia Plus Serial Control Module"};
#else TT1260
	char gszAppName[] = {"Tandberg TT1260 Serial Control Module"};
#endif TT1260
char gszLocalModeError[] = {"Receiver is in local mode - switch to remote mode"};

void SendAlteia(char * szBuffer, DWORD dwBytesToWrite)
{
	DWORD i;
	DWORD dwCRC = 0;
	char szTemp[256];

	for (i = 0; i < dwBytesToWrite; i++)
		dwCRC += szBuffer[i];
	wsprintf(szTemp, "$%02X\r", dwCRC & 0xff);
	lstrcat(szBuffer, szTemp);
	dwBytesToWrite += lstrlen(szTemp);
	SourceHelper_SendReceiverSerial(szBuffer, dwBytesToWrite);
}

char * ReadAlteiaResponse()
{
	
	int nOffsetPointer = 0;
	BYTE c;
	static BYTE szResponseBuffer[256];

	do
	{
		int nTimeout = 1000;
		c = SourceHelper_ReceiveReceiverSerial(&nTimeout);
		if (nTimeout == 0)
			break;

		if (c == '\r')
		{
			szResponseBuffer[nOffsetPointer] = 0;
			break;
		}
		else
		{
			szResponseBuffer[nOffsetPointer++] = c;
		}
	} while (TRUE);

	return szResponseBuffer;
}

BOOL GetAlteiaTunerStatus()
{
	char szCommand[128];
	DWORD dwReturnValue;
	char * szResponse, * szValue;
	int i;
	DWORD dwMask;

	// The bloddy Alteia reports crap occasionally for status
	// so we query four times and only compare the last
	for (i = 0; i < 4; i++)
	{
		wsprintf(szCommand, "\\000:SYS;SRQ_");
		SendAlteia(szCommand, lstrlen(szCommand));
		szResponse = ReadAlteiaResponse();
		szValue = strstr(szResponse, "=");
		sscanf(szValue + 1, "%04x", &dwReturnValue);
	}
	dwMask = 0x8000 | 0x2000 | 0x0020;
	return ((dwReturnValue & dwMask) == 0);
}

// Function to tell our receiver name
char * GetReceiverName()
{
#ifndef TT1260
	return "Tandberg Alteia Plus";
#else TT1260
	return "Tandberg TT1260";
#endif TT1260
}

// Parameters for the serial port
void GetSerialParameters(int *nBaudRate, int *nDataBits, int *nParity, int *nStopBits, BOOL *fDTR)
{
	*nBaudRate = 9600;
	*nDataBits = 8;
	*nParity = NOPARITY;
	*nStopBits = ONESTOPBIT;
	*fDTR = TRUE;
}

// The main function - tune the receiver and wait for a lock
BOOL TuneReceiver(PSOURCESTRUCT ss, char * szTunerString)
{
	int nLBand;
	DWORD dwTimeoutTime = 0;
	BOOL fRetVal = FALSE;
	int nMode = 0;		// default to QPSK
	int nRFInput = 1;	// default to input 1
	char szFEC[16] = {0};

	if (ss->nFrequency > ss->nLNBFrequency)
		nLBand = ss->nFrequency - ss->nLNBFrequency;
	else
		nLBand = ss->nLNBFrequency - ss->nFrequency;

	switch(ss->nADVModulationMode)
	{
	case ADV_MOD_DVB_QPSK:
		switch(ss->nCodeRate)
		{
		case 0:
			lstrcpy(szFEC, "1/2");
			break;
		case 1:
			lstrcpy(szFEC, "2/3");
			break;
		case 2:
			lstrcpy(szFEC, "3/4");
			break;
		case 3:
			lstrcpy(szFEC, "5/6");
			break;
		case 4:
			lstrcpy(szFEC, "7/8");
			break;
		default:
#ifdef TT1260
			lstrcpy(szFEC, "AUT");
#else TT1260
			MessageBox(ss->hWndTSReader, "Unsupported code rate", gszAppName, MB_ICONSTOP);
			return FALSE;
#endif TT1260
			break;
		}
		nMode = 0;		// QPSK
		break;
	case ADV_MOD_TURBO_8PSK:
		switch(ss->nCodeRate)
		{
		case 0:
#ifdef TT1260
			lstrcpy(szFEC, "2/3");
#else TT1260
			MessageBox(ss->hWndTSReader, "Unsupported code rate", gszAppName, MB_ICONSTOP);
			return FALSE;
#endif T1260
			break;
		case 3:
			lstrcpy(szFEC, "5/6");
			break;
		case 4:
			lstrcpy(szFEC, "8/9");
			break;
		default:
			MessageBox(ss->hWndTSReader, "Unsupported code rate", gszAppName, MB_ICONSTOP);
			return FALSE;
		}
		nMode = 1;		// 8PSK
		break;
	case ADV_MOD_DCII_C_QPSK:
	case ADV_MOD_DCII_I_QPSK:
	case ADV_MOD_DCII_Q_QPSK:
	case ADV_MOD_DCII_C_OQPSK:
	case ADV_MOD_TURBO_QPSK:
	case ADV_MOD_TURBO_16QAM:
		MessageBox(ss->hWndTSReader, "The currently selected receiver doesn't support this modulation mode", gszAppName, MB_ICONSTOP);
		return FALSE;
	}

	// Now tune
	{
		char szCommand[256];

		// Frequency
		switch(ss->nPolarity)
		{
		case 0:		// vertical
			wsprintf(szCommand, "\\000:TUN;POL_VER_");
			SendAlteia(szCommand, lstrlen(szCommand));
			ReadAlteiaResponse();
			wsprintf(szCommand, "\\000:TUN;PWR=ON_");
			break;
		case 1:		// horizontal
			wsprintf(szCommand, "\\000:TUN;POL_HOR_");
			SendAlteia(szCommand, lstrlen(szCommand));
			ReadAlteiaResponse();
			wsprintf(szCommand, "\\000:TUN;PWR=ON_");
			break;
		default:
			wsprintf(szCommand, "\\000:TUN;PWR=OFF_");
			break;
		}
		SendAlteia(szCommand, lstrlen(szCommand));
		if (memcmp(ReadAlteiaResponse(), "\\000:TUN;FRQ=LCL_", 17) == 0)
		{
			MessageBox(ss->hWndTSReader, gszLocalModeError, gszAppName, MB_ICONSTOP);
			return FALSE;
		}
	}

	{
		char szCommand[256];
		char szModulation[4];

		// RF Input
		// Translate DiSEqC options
		if (ss->nDiSEqCInput > 0 && ss->nDiSEqCInput < 3)
			nRFInput = ss->nDiSEqCInput;		
		wsprintf(szCommand, "\\000:TUN;RFI=NO %1d_", nRFInput);
		SendAlteia(szCommand, lstrlen(szCommand));
		ReadAlteiaResponse();

		// Frequency
		wsprintf(szCommand, "\\000:TUN;FRQ=%06d_", nLBand * 8);
		SendAlteia(szCommand, lstrlen(szCommand));
		if (memcmp(ReadAlteiaResponse(), "\\000:TUN;FRQ=LCL_", 17) == 0)
		{
			MessageBox(ss->hWndTSReader, gszLocalModeError, gszAppName, MB_ICONSTOP);
			return FALSE;
		}

		//Symbol rate
		wsprintf(szCommand, "\\000:DEM;SYM=%06d_", ss->nSymbolRate * 10);
		SendAlteia(szCommand, lstrlen(szCommand));
		ReadAlteiaResponse();

		// FEC
		wsprintf(szCommand, "\\000:DEM;FEC=%s_", szFEC);
		SendAlteia(szCommand, lstrlen(szCommand));
		ReadAlteiaResponse();

		// Modulation
		if (nMode == 1)
			lstrcpy(szModulation, "8PS");
		else
			lstrcpy(szModulation, "QPS");
		wsprintf(szCommand, "\\000:TUN;MOD=%s_", szModulation);
		SendAlteia(szCommand, lstrlen(szCommand));
		ReadAlteiaResponse();
	}

	// Wait for a lock
	Sleep(2000);
	dwTimeoutTime = GetTickCount() + (60 * 1000);
	do
	{
		if (GetAlteiaTunerStatus() == TRUE)
		{
			fRetVal = TRUE;
			break;
		}
		Sleep(100);
	} while (GetTickCount() < dwTimeoutTime);
	if (!fRetVal)
	{
		if (ss->fQuietMode == FALSE)
		{
			MessageBox(ss->hWndTSReader, "Failed to lock signal", gszAppName, MB_ICONWARNING);
			return FALSE;
		}
	}			

	return TRUE;
}

// This function is called when the TSReader user selects a channel in the PMT tree
// It selects the channel on the receiver therefore enabling any CA authorization
// for that channel
BOOL SetChannel(int nSID, int nTSID, int nNID)
{
	char * szResponse;
	char szCommand[256];

	wsprintf(szCommand, "\\000:SER;SID=%05d,%05d,%05d_", nTSID, nNID, nSID);
	SendAlteia(szCommand, lstrlen(szCommand));		
	szResponse = ReadAlteiaResponse();
	
	return TRUE;
}
