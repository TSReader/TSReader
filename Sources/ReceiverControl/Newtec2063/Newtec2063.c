#include <windows.h>
#include "..\..\..\SampleSource\Inc\sources.h"

char gszAppName[] = {"Newtec 2063 Serial Control Module"};

void SendNewtec(char * szBuffer, DWORD dwBytesToWrite)
{
	int nOutputPos = 0;
	DWORD i;
	int nCRC = 0;
	BYTE szTemp[256];

	szTemp[nOutputPos++] = 2;	//stx
	szTemp[nOutputPos++] = 100;	//device address
	for (i = 0; i < dwBytesToWrite; i++)
		szTemp[nOutputPos++] = szBuffer[i];
	szTemp[nOutputPos++] = 3;	// etx

	for (i = 0; i < (DWORD)nOutputPos; i++)
		nCRC ^= szTemp[i];
	szTemp[nOutputPos++] = nCRC;
	SourceHelper_SendReceiverSerial(szTemp, nOutputPos);
}

BYTE * ReadNewtecResponse()
{
	BOOL fFoundACK = FALSE;
	BOOL fWaitingCRC = FALSE;
	int nOffsetPointer = 0;
	int nNoDataCounter = 0;
	BYTE c;
	static BYTE szResponseBuffer[256];

	do
	{
		int nTimeout = 1000;
		c = SourceHelper_ReceiveReceiverSerial(&nTimeout);
		if (nTimeout == 0)
			break;

		if (fFoundACK == FALSE)
		{
			if (c == 6)
				fFoundACK = TRUE;
		}
		else
		{
			if (fWaitingCRC == TRUE)
			{
				szResponseBuffer[nOffsetPointer++] = 0;
				break;
			}
			else
			{
				if (c == 3)
					fWaitingCRC = TRUE;
				else
					szResponseBuffer[nOffsetPointer++] = c;
			}
		}
	} while (TRUE);

	return szResponseBuffer;
}

// Function to tell our receiver name
char * GetReceiverName()
{
	return "Newtec 2063";
}

// Parameters for the serial port
void GetSerialParameters(int *nBaudRate, int *nDataBits, int *nParity, int *nStopBits, BOOL *fDTR)
{
	*nBaudRate = 9600;
	*nDataBits = 7;
	*nParity = EVENPARITY;
	*nStopBits = ONESTOPBIT;
	*fDTR = TRUE;
}

// The main function - tune the receiver and wait for a lock
BOOL TuneReceiver(PSOURCESTRUCT ss, char * szTunerString)
{
	int nLBand;
	int nRFInput = -1;
	int nNewStatus = 0;
	DWORD dwTimeoutTime = 0;
	BOOL fRetVal = FALSE;
	char szFECAndModulation[16] = {""};
	char szCommand[256];

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
			lstrcpy(szFECAndModulation, "11");
			break;
		case 1:
			lstrcpy(szFECAndModulation, "12");
			break;
		case 2:
			lstrcpy(szFECAndModulation, "13");
			break;
		case 3:
			lstrcpy(szFECAndModulation, "15");
			break;
		case 4:
			lstrcpy(szFECAndModulation, "17");
			break;
		default:
			MessageBox(ss->hWndTSReader, "Unsupported code rate", gszAppName, MB_ICONSTOP);
			return FALSE;
		}
		break;
	case ADV_MOD_TURBO_8PSK:
		switch(ss->nCodeRate)
		{
		case 0:
			lstrcpy(szFECAndModulation, "82");
			break;
		case 3:
			lstrcpy(szFECAndModulation, "85");
			break;
		case 4:
			lstrcpy(szFECAndModulation, "89");
			break;
		default:
			MessageBox(ss->hWndTSReader, "Unsupported code rate", gszAppName, MB_ICONSTOP);
			return FALSE;
		}
		break;
	case ADV_MOD_TURBO_16QAM:
		switch(ss->nCodeRate)
		{
		case 0:
			lstrcpy(szFECAndModulation, "63");
			break;
		default:
			MessageBox(ss->hWndTSReader, "Unsupported code rate", gszAppName, MB_ICONSTOP);
			return FALSE;
		}
		break;

	case ADV_MOD_DCII_C_QPSK:
	case ADV_MOD_DCII_I_QPSK:
	case ADV_MOD_DCII_Q_QPSK:
	case ADV_MOD_DCII_C_OQPSK:
	case ADV_MOD_TURBO_QPSK:
		MessageBox(ss->hWndTSReader, "The currently selected receiver doesn't support this modulation mode", gszAppName, MB_ICONSTOP);
		return FALSE;
	}

	// Translate DiSEqC options
	if (ss->nDiSEqCInput > 0 && ss->nDiSEqCInput < 3)
		nRFInput = ss->nDiSEqCInput;
	
	if (ss->n22KHz)
		nNewStatus |= 1;
	switch(ss->nPolarity)
	{
	case 0:		// vertical
		// nothing to set
		break;
	case 1:		// horizontal
		nNewStatus |= 2;
		break;
	default:
		break;	// leave as is
	}

	wsprintf(szCommand, "1XBm%d", nNewStatus);	// port A
	SendNewtec(szCommand, lstrlen(szCommand));
	ReadNewtecResponse();
	wsprintf(szCommand, "2XBm%d", nNewStatus);	// port B
	SendNewtec(szCommand, lstrlen(szCommand));
	ReadNewtecResponse();
	

	// RF Input
	switch(nRFInput)
	{
	case 1:		// port a
	case 2:		// port b
		wsprintf(szCommand, "XBi%d", nRFInput);
		SendNewtec(szCommand, lstrlen(szCommand));
		ReadNewtecResponse();
		break;
	}

	// Frequency
	wsprintf(szCommand, "RFf%011d", nLBand * 1000000);
	SendNewtec(szCommand, lstrlen(szCommand));
	ReadNewtecResponse();

	// Modulation and FEC rate
	wsprintf(szCommand, "RMx%s", szFECAndModulation);
	SendNewtec(szCommand, lstrlen(szCommand));
	ReadNewtecResponse();

	//Symbol rate
	wsprintf(szCommand, "RRs%09d", ss->nSymbolRate * 1000);
	SendNewtec(szCommand, lstrlen(szCommand));
	ReadNewtecResponse();
	
	// Wait for a lock
	Sleep(2000);
	dwTimeoutTime = GetTickCount() + (30 * 1000);
	do
	{
		BYTE * pResponse;

		lstrcpy(szCommand, "Rss?");
		SendNewtec(szCommand, lstrlen(szCommand));
		pResponse = ReadNewtecResponse();
		if (memcmp(&pResponse[1], "Rss1", 4) == 0)
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

// Note that since the Newtec is a demod only, there's no need for a SetChannel
// function in this module. TSReader will see this function doesn't exist and will not
// attempt to select channel

