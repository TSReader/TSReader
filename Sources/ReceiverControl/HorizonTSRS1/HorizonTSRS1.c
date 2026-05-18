// Serial receiver control module for
//  Horizon TSR-S1

#include <windows.h>
#include <stdio.h>
#include "..\..\..\SampleSource\Inc\sources.h"

BOOL fDoneInit = FALSE;
BOOL fPollThreadTerminated, fTerminatePollThread;
CRITICAL_SECTION csStringProtection;
char szLastSignal[128] = {"n/a"};
char gszAppName[] = {"Horizon TSR-S1"};

void SendHorizonCommand(char * szCommand)
{
	char szLocalCommand[64];
	char szTemp[16];
	int i, cs;

	szLocalCommand[0] = 2; //STX
	lstrcpy(&szLocalCommand[1], szCommand);
	/*do
	{
		if (lstrlen(&szLocalCommand[1]) == 22)
			break;
		lstrcat(&szLocalCommand[1], " ");
	} while (TRUE);*/
	cs = 0;
	for (i = 0; i < lstrlen(&szLocalCommand[1]) + 1; i++)
		cs ^= szLocalCommand[i + 1];
	cs = 0 - cs;
	cs &= 0xff;
	wsprintf(szTemp, "%02X", cs);
	lstrcat(&szLocalCommand[1], szTemp);
	lstrcat(&szLocalCommand[1], "\003");

	SourceHelper_SendReceiverSerial(szLocalCommand, strlen(szLocalCommand));
}

unsigned char * ReadHorizonString()
{
	static unsigned char szResponseBuffer[128];
	int nPtr = 0;
	BOOL fGotACK = FALSE;
	BOOL fGotResponse = FALSE;

	do
	{
		int nTimeout = 1000;
		BYTE c = SourceHelper_ReceiveReceiverSerial(&nTimeout);
		if (nTimeout == 0)
			break;
		if (fGotACK == FALSE)
		{
			if (c == 6)	// ACK
				fGotACK = TRUE;
		}
		else
		{
			if (c == 3)
			{
				szResponseBuffer[nPtr] = 0;
				fGotResponse = TRUE;
			}
			else
				szResponseBuffer[nPtr++] = c;
		}
	} while (fGotResponse == FALSE);

	return szResponseBuffer;
}

DWORD ReadHorizonRegister(DWORD dwRegister, int nBytes)
{
	int nTimeout = 5;
	DWORD dwRetVal;
	char szCommand[128];

	wsprintf(szCommand, "R%02X%01X%02X",
		     0xd0, // I2C address of link chip
			 nBytes,
			 dwRegister);
	SendHorizonCommand(szCommand);
	do
	{
		char * szResponse;

		szResponse = ReadHorizonString();
		switch(szResponse[0])
		{
		case 'r':
			szResponse[6 + (nBytes * 2)] = '\0';
			sscanf(&szResponse[6], "%x", &dwRetVal);
			/*{
				char szTemp[128];
				wsprintf(szTemp, "%s = %02x\n", szCommand, dwRetVal);
				OutputDebugString(szTemp);
			}*/
			return dwRetVal;
		case 'v':
			break;
		case '?':
			SendHorizonCommand(szCommand);
			nTimeout--;
			break;
		}
	} while (nTimeout);

	return 0;
}

void WriteHorizonRegister(DWORD dwRegister, DWORD dwValue)
{
	int nTimeout = 5;
	char szCommand[128];

	
	wsprintf(szCommand, "WD02%02X%02X", dwRegister, dwValue);
	/*{
		char szTemp[128];
		wsprintf(szTemp, "%s\n", szCommand);
		OutputDebugString(szTemp);
	}*/
	SendHorizonCommand(szCommand);
	do
	{
		char * szResponse;

		szResponse = ReadHorizonString();
		switch(szResponse[0])
		{
		case 'w':
			return;
		case 'v':
			break;
		default:
			OutputDebugString("r");
			SendHorizonCommand(szCommand);
			nTimeout--;
			break;
		}
	} while (nTimeout);
}

void WriteHorizonTuner(BYTE * pData)
{
	int nTimeout = 5;
	int i;
	char szCommand[128];
	char szTemp[16];

	lstrcpy(szCommand, "WC04");
	for (i = 0; i < 4; i++)
	{
		wsprintf(szTemp, "%02X", pData[i]);
		lstrcat(szCommand, szTemp);
	}
	/*{
		char szTemp[128];
		wsprintf(szTemp, "%s\n", szCommand);
		OutputDebugString(szTemp);
	}*/

	SendHorizonCommand(szCommand);
	do
	{
		char * szResponse;

		szResponse = ReadHorizonString();
		switch(szResponse[0])
		{
		case 'w':
			return;
		case 'v':
			break;
		case '?':
			SendHorizonCommand(szCommand);
			nTimeout--;
			break;
		}
	} while (nTimeout);
}

void InitHorizonTuner()
{	
	WriteHorizonRegister(0x01, 0x15);	// RCR
	WriteHorizonRegister(0x02, 0x30);	// MCR
	WriteHorizonRegister(0x03, 0x2a);	// ACR
	WriteHorizonRegister(0x04, 0x7d);	// F22FR
	WriteHorizonRegister(0x05, 0x05);	// I2CRPT
	WriteHorizonRegister(0x06, 0xa2);	// DACR1 (MSB)
	WriteHorizonRegister(0x07, 0x00);	// DACR2 (LSB)
	WriteHorizonRegister(0x08, 0x60);	// DiSEqC

	WriteHorizonRegister(0x29, 0x1e);	// VTH0
	WriteHorizonRegister(0x2a, 0x14);	// VTH1
	WriteHorizonRegister(0x2b, 0x0f);	// VTH2
	WriteHorizonRegister(0x2c, 0x09);	// VTH3
	WriteHorizonRegister(0x2d, 0x05);	// VTH4
	
	WriteHorizonRegister(0x31, 0x1f);	// PR (code rate)
	WriteHorizonRegister(0x32, 0x19);	// VSEARCH
	WriteHorizonRegister(0x33, 0xfd);	// RS
	WriteHorizonRegister(0x34, 0x03);	// ERRCNT - Post Viterbi error count
}

void InitTunerVCO()
{
	// Init the VCO on the tuner
	int nLBand = 950 * 2;
	int i;

	for (i = 0; i < 2; i++)
	{
		BYTE bTunerBytes[4];

		bTunerBytes[0] = (BYTE)((nLBand >> 8) & 0xff);
		bTunerBytes[1] = (BYTE)(nLBand & 0xff);
		bTunerBytes[2] = 0xe1;
		bTunerBytes[3] = 0x00;
		WriteHorizonRegister(0x05, 0x85);	// put link chip into passthrough mode
		WriteHorizonTuner(bTunerBytes);
		WriteHorizonRegister(0x22, 0x00);	// clear offset L
		WriteHorizonRegister(0x23, 0x00);	// clear offset U
		Sleep(50);
		bTunerBytes[2] = 0xe1 | 0x04;
		bTunerBytes[3] = 0x00;
		WriteHorizonRegister(0x05, 0x85);	// put link chip into passthrough mode
		WriteHorizonTuner(bTunerBytes);
		WriteHorizonRegister(0x22, 0x00);	// clear offset L
		WriteHorizonRegister(0x23, 0x00);	// clear offset U
		Sleep(50);
		nLBand = 1450 * 2;
	}
}

void SetSymbolRate(int nSymbolRate)
{
	BYTE bSRH, bSRM, bSRL;
	{
		char szTemp[128];
		wsprintf(szTemp, "Horizon: SetSymbolRate(nSymbolRate = %d)\n", nSymbolRate);
		OutputDebugString(szTemp);
	}

	nSymbolRate = nSymbolRate << 16;
	nSymbolRate = nSymbolRate / 5500;
	nSymbolRate = nSymbolRate << 4;
	bSRH = (nSymbolRate >> 16) & 0x0ff;
	bSRM = (nSymbolRate >> 8) & 0x0ff;
	bSRL = (nSymbolRate) & 0xff;
	WriteHorizonRegister(0x1f, bSRH);		// SRH
	WriteHorizonRegister(0x20, bSRM);		// SRM	
	WriteHorizonRegister(0x21, bSRL);		// SRL
	WriteHorizonRegister(0x22, 0x00);	// clear offset L
	WriteHorizonRegister(0x23, 0x00);	// clear offset U
}

void SetFrequency(int nFrequency, int nSymbolRate)
{
	int nMuxWidth;
	BYTE bTunerBytes[4] = {0x00, 0x00, 0xe1, 0x00};

	{
		char szTemp[128];
		wsprintf(szTemp, "Horizon: SetFrequency(nFrequency = %d nSymbolRate = %d)\n", nFrequency, nSymbolRate);
		OutputDebugString(szTemp);
	}

	if (nFrequency >= 950 && nFrequency <= 970)
		bTunerBytes[3] |= 0xa2;
	else if (nFrequency >= 970 && nFrequency <= 1065)
		bTunerBytes[3] |= 0xc2;
	else if (nFrequency >= 1065 && nFrequency <= 1170)
		bTunerBytes[3] |= 0xe2;
	else if (nFrequency >= 1170 && nFrequency <= 1300)
		bTunerBytes[3] |= 0x20;
	else if (nFrequency >= 1300 && nFrequency <= 1445)
		bTunerBytes[3] |= 0x40;
	else if (nFrequency >= 1445 && nFrequency <= 1607)
		bTunerBytes[3] |= 0x60;
	else if (nFrequency >= 1607 && nFrequency <= 1778)
		bTunerBytes[3] |= 0x80;
	else if (nFrequency >= 1778 && nFrequency <= 1942)
		bTunerBytes[3] |= 0xa0;
	else if (nFrequency >= 1942 && nFrequency <= 2131)
		bTunerBytes[3] |= 0xc0;
	else if (nFrequency >= 2131 && nFrequency <= 2150)
		bTunerBytes[3] |= 0xe0;

	if (nSymbolRate < 12000)
	{
		// 10MHz
		bTunerBytes[2] |= 0x18;		// PD5=1 PD4=1
									// PD3=0 PD2=0
		OutputDebugString("Horizon: LPF at 10 MHz\n");
	}
	else
	{
		nMuxWidth = (nSymbolRate / 1000) + 2;
		/*if (nMuxWidth <= 12)
		{
			// 12MHz
										// PD5=0 PD4=0 
			bTunerBytes[3] |= 0x08;		// PD3=1 PD2=0
			OutputDebugString("Horizon: LPF at 12 MHz\n");
		}
		else if (nMuxWidth <= 14)
		{
			// 14MHz
			bTunerBytes[2] |= 0x10;		// PD5=1 PD4=0 
			bTunerBytes[3] |= 0x08;		// PD3=1 PD2=0
			OutputDebugString("Horizon: LPF at 14 MHz\n");
		}
		else if (nMuxWidth <= 16)
		{
			// 16MHz
			bTunerBytes[2] |= 0x08;		// PD5=0 PD4=1 
			bTunerBytes[3] |= 0x08;		// PD3=1 PD2=0
			OutputDebugString("Horizon: LPF at 16 MHz\n");
		}
		else if (nMuxWidth <= 18)
		{
			// 18MHz
			bTunerBytes[2] |= 0x18;		// PD5=1 PD4=1 
			bTunerBytes[3] |= 0x08;		// PD3=1 PD2=0
			OutputDebugString("Horizon: LPF at 18 MHz\n");
		}
		else if (nMuxWidth <= 20)
		{
			// 20MHz
										// PD5=0 PD4=0 
			bTunerBytes[3] |= 0x04;		// PD3=0 PD2=1
			OutputDebugString("Horizon: LPF at 20 MHz\n");
		}
		else if (nMuxWidth <= 22)
		{
			// 22MHz
			bTunerBytes[2] |= 0x10;		// PD5=1 PD4=0 
			bTunerBytes[3] |= 0x04;		// PD3=0 PD2=1
			OutputDebugString("Horizon: LPF at 22 MHz\n");
		}
		else if (nMuxWidth <= 24)
		{
			// 24MHz
			bTunerBytes[2] |= 0x08;		// PD5=0 PD4=1 
			bTunerBytes[3] |= 0x04;		// PD3=0 PD2=1
			OutputDebugString("Horizon: LPF at 24 MHz\n");
		}
		else if (nMuxWidth <= 26)
		{
			// 26MHz
			bTunerBytes[2] |= 0x18;		// PD5=1 PD4=1 
			bTunerBytes[3] |= 0x04;		// PD3=0 PD2=1
			OutputDebugString("Horizon: LPF at 26 MHz\n");
		}
		else if (nMuxWidth <= 28)
		{
			// 28MHz
										// PD5=0 PD4=0 
			bTunerBytes[3] |= 0x0c;		// PD3=1 PD2=1
			OutputDebugString("Horizon: LPF at 28 MHz\n");
		}
		else*/
		{
			/// 30MHz
			bTunerBytes[2] |= 0x10;		// PD5=1 PD4=0 
			bTunerBytes[3] |= 0x0c;		// PD3=1 PD2=1
			OutputDebugString("Horizon: LPF at 30 MHz\n");
		}	
	}

	WriteHorizonRegister(0x05, 0x85);	// put link chip into passthrough mode
	nFrequency = nFrequency * 2;
	bTunerBytes[0] = (BYTE)((nFrequency >> 8) & 0xff);
	bTunerBytes[1] = (BYTE)(nFrequency & 0xff);
	WriteHorizonTuner(bTunerBytes);
	WriteHorizonRegister(0x22, 0x00);	// clear offset L
	WriteHorizonRegister(0x23, 0x00);	// clear offset U
}

void SelectDiSEqCPort(int nInput)
{
	BYTE bPositionByte[] = {0xc0, 0xc4, 0xc8, 0xcc};
	BYTE bInputString[] = {0xe0, 0x10, 0x38, 0x00};

	nInput--;
	if ((nInput >= 0) && (nInput <= 3) )
	{
		int i;
		int nTimeout;

		bInputString[3] = bPositionByte[nInput];
		WriteHorizonRegister(0x08, 0x06);		// DiSEqC - DiSEqC mode
		Sleep(15);
		for (i = 0; i < 4; i++)
		{
			//while ((ReadHorizonRegister(0x0a, 1) & 1) == 1)
			//	Sleep(1);	// stall if FIFO full
			WriteHorizonRegister(0x09, bInputString[i]);
		}
		nTimeout = 100;
		while ((ReadHorizonRegister(0x0a, 1) & 2) == 2 && nTimeout-- > 0)
			Sleep(1);		// stall until FIFO empty
		Sleep(15);
		WriteHorizonRegister(0x08, 0x04);		// DiSEqC - tone off
		Sleep(15);
	}
}

// Function to tell our receiver name
char * GetReceiverName()
{
	return "Horizon TSR-S1";
}

// Parameters for the serial port
void GetSerialParameters(int *nBaudRate, int *nDataBits, int *nParity, int *nStopBits, BOOL *fDTR)
{
	*nBaudRate = 115200;
	*nDataBits = 8;
	*nParity = NOPARITY;
	*nStopBits = ONESTOPBIT;
	*fDTR = FALSE;
}

void SetupLastTune(PSOURCESTRUCT ss, char * szTunerString)
{
	char szPolarity[4] = {"H/L"};

	if (ss->nPolarity == 0)
		lstrcpy(szPolarity, "V/R");

	wsprintf(szTunerString, "%d MHz %s %d DVB", ss->nFrequency, szPolarity, ss->nSymbolRate);
}

// The main function - tune the receiver and wait for a lock
BOOL TuneReceiver(PSOURCESTRUCT ss, char * szTunerString)
{
	BOOL fInverted = FALSE;
	int nInversionCounter = 0;
	int nLBand;
	int nFrequencyOffset = 0;
	int nSymbolRateOffset = 0;
	int nSymbolRateCounter = 0;
	int nLockCount = 0;
	DWORD dwLockTimeout = 10000;
	DWORD dwLockTime;

	SetupLastTune(ss, szTunerString);

	if (ss->nFrequency > ss->nLNBFrequency)
		nLBand = ss->nFrequency - ss->nLNBFrequency;
	else
		nLBand = ss->nLNBFrequency - ss->nFrequency;

	if (!fDoneInit)
	{
		fDoneInit = TRUE;
		SourceHelper_SendReceiverSerial("p", 1);		// turn on tuner power
		Sleep(10);
		InitHorizonTuner();
		InitTunerVCO();
	}

	if (ss->nPolarity)
		SourceHelper_SendReceiverSerial("h", 1);
	else
		SourceHelper_SendReceiverSerial("v", 1);
	
	SelectDiSEqCPort(ss->nDiSEqCInput);

	if (ss->n22KHz)
		WriteHorizonRegister(0x08, 0x03);		// DiSEqC - tone on
	else
		WriteHorizonRegister(0x08, 0x04);		// DiSEqC - tone off

	WriteHorizonRegister(0x28, 0x00);	// FECM (DVB QPSK parallel)
	WriteHorizonRegister(0x14, 0x55);	// BCLC (QPSK2)
	WriteHorizonRegister(0x33, 0xfd);	// RS
	WriteHorizonRegister(0x0c, 0xf1);	// IOCFG (not inverted)

	SetSymbolRate(ss->nSymbolRate + nSymbolRateOffset);
	SetFrequency(nLBand + nFrequencyOffset, ss->nSymbolRate);
	dwLockTime = GetTickCount();	
	while (TRUE)
	{
		DWORD dwCountNow;
		DWORD dwVStatus;

		dwVStatus = ReadHorizonRegister(0x1b, 1);
		if ((dwVStatus & 0x98) == 0x98)
		{
			nLockCount++;
			if (nLockCount > 10)
			{
				if (nFrequencyOffset || nSymbolRateOffset)
				{
					char szTemp[128];
					wsprintf(szTemp, "Horizon: Locked Freq %d SR %d\n", ss->nFrequency + nFrequencyOffset, ss->nSymbolRate + nSymbolRateOffset);
					OutputDebugString(szTemp);
				}
				return TRUE;		// we're locked
			}
		}
		else
			nLockCount = 0;


		dwCountNow = GetTickCount() - dwLockTime;
		if (dwCountNow > dwLockTimeout)
		{
			OutputDebugString("Horizon: Tune Timeout\n");
			break;
		}
		Sleep(1);
		if (nInversionCounter++ == 10)
		{
			nInversionCounter = 0;
			fInverted = ~fInverted;
			if (fInverted == FALSE)
				WriteHorizonRegister(0x0c, 0xf1);	// IOCFG (not inverted)
			else
				WriteHorizonRegister(0x0c, 0xf0);	// IOCFG (inverted)
#define ZIGZAG
#ifdef ZIGZAG
			nSymbolRateCounter++;
			if (nSymbolRateCounter > 2)
			{
				nSymbolRateCounter = 0;
				if (nSymbolRateOffset == 0)
				{
					nSymbolRateOffset = 1;
				}
				else
				{
					if (nSymbolRateOffset > 0)
						nSymbolRateOffset = -nSymbolRateOffset;
					else
					{
						nSymbolRateOffset = -nSymbolRateOffset;
						nSymbolRateOffset++;
						if (nSymbolRateOffset >  3)
						{
							nSymbolRateOffset = 0;
							if (nFrequencyOffset == 0)
								nFrequencyOffset = 1;
							else if (nFrequencyOffset == 1)
								nFrequencyOffset = -1;
							else if (nFrequencyOffset == -1)
								nFrequencyOffset = 0;
							SetFrequency(nLBand + nFrequencyOffset, ss->nSymbolRate + nSymbolRateOffset);
						}
					}
				}
				SetSymbolRate(ss->nSymbolRate + nSymbolRateOffset);
			}
#endif ZIGZAG
		}	
	}

	if (ss->fQuietMode == FALSE)
		MessageBox(ss->hWndTSReader, "Failed to lock signal", gszAppName, MB_ICONWARNING);
	return FALSE;
}

// This function is called when the TSReader user selects a channel in the PMT tree
// It selects the channel on the receiver therefore enabling any CA authorization
// for that channel
BOOL SetChannel(int nSID, int nTSID, int nNID)
{
	// No set channel - just a demodulator
	return TRUE;
}

DWORD WINAPI PollSignal(LPVOID lpv)
{
	while (!fTerminatePollThread)
	{
		int nErrCnt;
		double dBER;
		double dDivisor = 2097152.0;
		char szLockStatus[16];

		nErrCnt = ReadHorizonRegister(0x1d, 2);// << 8 | ReadSharpRegister(0x1e);
		if (nErrCnt > 8192)
			nErrCnt = 8192;
		dBER = (double)nErrCnt / dDivisor;

		if ((ReadHorizonRegister(0x1b, 1) & 0x98) == 0x98)
			lstrcpy(szLockStatus, "Locked");
		else
			lstrcpy(szLockStatus, "Unlocked");

		EnterCriticalSection(&csStringProtection);			
		sprintf(szLastSignal, "%s: BER %0.1E", szLockStatus, dBER);
		LeaveCriticalSection(&csStringProtection);
		
		for (nErrCnt = 0; nErrCnt < 10 && !fTerminatePollThread; nErrCnt++)
			Sleep(50);
	}

	fPollThreadTerminated = TRUE;
	return 0;
}

void StartSerial(PSOURCESTRUCT ss)
{
	HANDLE hThread;
	DWORD dwThreadID;

	InitializeCriticalSection(&csStringProtection);
	fPollThreadTerminated = fTerminatePollThread = FALSE;
	hThread = CreateThread(NULL, 0, PollSignal, (LPVOID)0, 0, &dwThreadID);
	CloseHandle(hThread);
}

void StopSerial(PSOURCESTRUCT ss)
{
	fTerminatePollThread = TRUE;
	while (!fPollThreadTerminated)
		Sleep(50);
	DeleteCriticalSection(&csStringProtection);
}

BOOL GetSignal(char * szSignal)
{
	EnterCriticalSection(&csStringProtection);
	lstrcpy(szSignal, szLastSignal);
	LeaveCriticalSection(&csStringProtection);
	return TRUE;
}
