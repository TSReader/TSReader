// Stuff for libmpeg
#include <stdio.h>
#include <stdlib.h>
#include "inttypes.h"
#include "mpeg2.h"
#include "convert.h"
#include "softcsa.h"
#include "sources.h"

// Stuff for imglib
#include <_ISource.h>

#define VERSION_MAJOR 2
#define VERSION_MINOR 8
#define VERSION_EDIT 53
#define VERSION_SUB_EDIT 'h'
//#define BETA
#ifdef BETA
#define BETA_EXPIRE_MONTH 2
#define BETA_EXPIRE_YEAR 2008
#endif BETA

#define EIT_FORMAT_PLAIN 0
#define EIT_FORMAT_XML 1
#define EIT_FORMAT_XMLTV 2

#define SI_PARSER_NOP  0x00000000
#define SI_PARSER_PAT  0x10000000
#define SI_PARSER_EIT  0x20000000
#define SI_PARSER_SDT  0x30000000
#define SI_PARSER_NIT  0x40000000
#define SI_PARSER_CAT  0x50000000
#define SI_PARSER_PMT  0x60000000
#define SI_PARSER_ES   0x70000000
#define SI_PARSER_BAT  0x80000000
#define SI_PARSER_VCT  0x90000000
#define SI_PARSER_MMT  0xa0000000
#define SI_PARSER_CDT  0xb0000000		// Carrier Definition Table in DCII
#define SI_PARSER_RRT  0xb0000000		// Region Rating Table in ATSC
#define SI_PARSER_SIT  0xc0000000
#define SI_PARSER_TDT  0xd0000000		// Transponder Definition Table in DCII
#define SI_PARSER_MGT  0xe0000000
#define SI_PARSER_CVCT 0xf0000000

#define SI_PARSER_IP_PID 0x10000000
#define SI_PARSER_IP_MAC 0x20000000
#define SI_PARSER_IP_IP  0x30000000

// Time and Date Table in DVB
#define SI_PARSER_RST 0xe0000000

enum SI_PARSER_STATS
{
	SI_PARSER_STATS_PAT = 0,
	SI_PARSER_STATS_PMT,
	SI_PARSER_STATS_CAT,
	SI_PARSER_STATS_NIT,
	SI_PARSER_STATS_SDT,
	SI_PARSER_STATS_EIT,
	SI_PARSER_STATS_PSIP,
	SI_PARSER_STATS_RST,
	SI_PARSER_STATS_TDT,
	SI_PARSER_STATS_BAT
};

enum
{
	GPS_STATE_NO_DATA = 0,
	GPS_STATE_DATA_NO_NMEA,
	GPS_STATE_DATA_NMEA,
	GPS_STATE_DATA_NO_FIX,
	GPS_STATE_DATA_FIX
};

#define MAX_PAT_ENTRIES 255
#define MAX_ESLIST_ENTRIES 255
#define MAX_EIT_CHANNEL_DATA 65536
#define MAX_TS_ENTRIES 1024
#define MAX_SDT_EXTRA_DESCRIPTORS 64
#define MAX_EIT_EXTRA_DESCRIPTORS 64
#define MAX_NIT_EXTRA_DESCRIPTORS 64
#define MAX_CAT_DESCRIPTORS 256
#define MAX_MMT_ENTRIES 256
#define MAX_CDT_ENTRIES 256
#define MAX_SIT_ENTRIES 1024
#define MAX_TDT_ENTRIES 1920
#define MAX_MGT_ENTRIES 512
#define MAX_SATELLITES 512
#define MAX_TERRESTRIAL_MUXES 64
#define MAX_CABLE_MUXES 128
#define MAX_DECODE_NO_PID_COUNTER 10000
#define MAX_DECODE_NO_PES_COUNTER 150
#define MAX_VLC_CONFIGURATIONS 16
#define MAX_SOURCE_MODULES 512
#define MAX_CVCT_ENTRIES 256
#define MAX_CVCT_CHANNEL_ENTRIES 32
#define MAX_BAT_TRANSPORT_ITEMS 128
#define MAX_BAT_ENTRIES 256
#define MAX_AUDIO_STREAMS 8
#define MAX_GOP_SIZE 128
#define MAX_OTHER_STREAMS 16
#define MAX_SERIAL_RECEIVERS 32
#define MAX_CA_PIDS 32
#define MAX_IGNORED_NETWORKS 16
#define MAX_CHART_GOP_LENGTH 512
#define MAX_SWITCH_PARAMETERS 20

enum SIGNAL_CHART_MODE
{
	SIGNAL_CHART_MODE_UNKNOWN = 0,
	SIGNAL_CHART_MODE_SNR,
	SIGNAL_CHART_MODE_BER,
	SIGNAL_CHART_MODE_QUALITY,
	SIGNAL_CHART_MODE_QUALITY_DBM
};

#ifndef PRO
 #define MAX_CHARTS 1
 #define MAX_ES_PARSERS 1
#else PRO
 #define MAX_CHARTS 16
 #define MAX_ES_PARSERS 32
#endif PRO
#define REAL_MAX_CHARTS 32
#define REAL_MAX_ES_PARSERS 32

#define IP_UDP_ID 17
#define PMT_TIMEOUT 15

enum PSI_BUFFERS
{
	BUFFER_EIT = 0,
	BUFFER_SDT,
	BUFFER_NETWORK,
	BUFFER_PAT_PMT,
	BUFFER_CAT,
	BUFFER_PSIP,
	BUFFER_RST,
	BUFFER_TDT,
	BUFFER_EIT0,
	BUFFER_EIT1,
	BUFFER_EIT2,
	BUFFER_EIT3,
	BUFFER_EIT4,
	BUFFER_EIT5,
	BUFFER_EIT6,
	BUFFER_EIT7,
	BUFFER_EIT8,
	BUFFER_EIT9,
	BUFFER_EIT10,
	BUFFER_EIT11,
	BUFFER_EIT12,
	BUFFER_EIT13,
	BUFFER_EIT14,
	BUFFER_EIT15,
	BUFFER_EIT16,
	BUFFER_EIT17,
	BUFFER_EIT18,
	BUFFER_EIT19,
	BUFFER_EIT20,
	BUFFER_EIT21,
	BUFFER_EIT22,
	BUFFER_EIT23,
	BUFFER_EIT24,
	BUFFER_EIT25,
	BUFFER_EIT26,
	BUFFER_EIT27,
	BUFFER_EIT28,
	BUFFER_EIT29,
	BUFFER_EIT30,
	BUFFER_EIT31,
	BUFFER_EIT32,
	BUFFER_EIT33,
	BUFFER_EIT34,
	BUFFER_EIT35,
	BUFFER_EIT36,
	BUFFER_EIT37,
	BUFFER_EIT38,
	BUFFER_EIT39,
	BUFFER_EIT40,
	BUFFER_EIT41,
	BUFFER_EIT42,
	BUFFER_EIT43,
	BUFFER_EIT44,
	BUFFER_EIT45,
	BUFFER_EIT46,
	BUFFER_EIT47,	
	BUFFER_EIT48,	
	BUFFER_EIT49,	
	BUFFER_EIT50,	
	BUFFER_EIT51,	
	BUFFER_EIT52,	
	BUFFER_EIT53,	
	BUFFER_EIT54,	
	BUFFER_EIT55,	
	BUFFER_EIT56,	
	BUFFER_EIT57,	
	BUFFER_EIT58,	
	BUFFER_EIT59,	
	BUFFER_EIT60,	
	BUFFER_EIT61,	
	BUFFER_EIT62,	
	BUFFER_EIT63,	
	BUFFER_ETT0,
	BUFFER_ETT1,
	BUFFER_ETT2,
	BUFFER_ETT3,
	BUFFER_ETT4,
	BUFFER_ETT5,
	BUFFER_ETT6,
	BUFFER_ETT7,
	BUFFER_ETT8,
	BUFFER_ETT9,
	BUFFER_ETT10,
	BUFFER_ETT11,
	BUFFER_ETT12,
	BUFFER_ETT13,
	BUFFER_ETT14,
	BUFFER_ETT15,
	BUFFER_ETT16,
	BUFFER_ETT17,
	BUFFER_ETT18,
	BUFFER_ETT19,
	BUFFER_ETT20,
	BUFFER_ETT21,
	BUFFER_ETT22,
	BUFFER_ETT23,
	BUFFER_ETT24,
	BUFFER_ETT25,
	BUFFER_ETT26,
	BUFFER_ETT27,
	BUFFER_ETT28,
	BUFFER_ETT29,
	BUFFER_ETT30,
	BUFFER_ETT31,
	BUFFER_ETT32,
	BUFFER_ETT33,
	BUFFER_ETT34,
	BUFFER_ETT35,
	BUFFER_ETT36,
	BUFFER_ETT37,
	BUFFER_ETT38,
	BUFFER_ETT39,
	BUFFER_ETT40,
	BUFFER_ETT41,
	BUFFER_ETT42,
	BUFFER_ETT43,
	BUFFER_ETT44,
	BUFFER_ETT45,
	BUFFER_ETT46,
	BUFFER_ETT47,
	BUFFER_ETT48,	
	BUFFER_ETT49,	
	BUFFER_ETT50,	
	BUFFER_ETT51,	
	BUFFER_ETT52,	
	BUFFER_ETT53,	
	BUFFER_ETT54,	
	BUFFER_ETT55,	
	BUFFER_ETT56,	
	BUFFER_ETT57,	
	BUFFER_ETT58,	
	BUFFER_ETT59,	
	BUFFER_ETT60,	
	BUFFER_ETT61,	
	BUFFER_ETT62,	
	BUFFER_ETT63,
	BUFFER_CETT,
	BUFFER_SKY_EPG_PID1,
	BUFFER_SKY_EPG_PID2,
	BUFFER_SKY_EPG_PID3,
	BUFFER_SKY_PID_80,
	BUFFER_SKY_PID_81,
	BUFFER_SKY_PID_85,
	BUFFER_INT,
	BUFFER_RECORD_TABLES1,
	BUFFER_RECORD_TABLES2,
	BUFFER_RECORD_TABLES3,
	BUFFER_RECORD_TABLES4,
	BUFFER_RECORD_TABLES5,
	BUFFER_RECORD_TABLES6,
	BUFFER_RECORD_TABLES7,
	BUFFER_RECORD_TABLES8,
	BUFFER_RECORD_TABLES9,
	BUFFER_RECORD_TABLES10,
	BUFFER_RECORD_TABLES11,
	BUFFER_RECORD_TABLES12,
	BUFFER_RECORD_TABLES13,
	BUFFER_RECORD_TABLES14,
	BUFFER_RECORD_TABLES15,
	BUFFER_RECORD_TABLES16,
	BUFFER_BIT,
	MAX_SECTION_BUFFERS 
} SECTION_BUFFERS;

enum ES_PARSER_TYPES
{
	PARSE_ES_TYPE_NONE = 0,
	PARSE_ES_TYPE_MPEG2_VIDEO,
	PARSE_ES_TYPE_MPEG4_VIDEO,
	PARSE_ES_TYPE_H264_VIDEO,
	PARSE_ES_TYPE_VC1_VIDEO,
	PARSE_ES_TYPE_MPEG_AUDIO,
	PARSE_ES_TYPE_AC3_AUDIO,
	PARSE_ES_TYPE_TELETEXT,
	PARSE_ES_TYPE_MPEG2_AAC_AUDIO,
	PARSE_ES_TYPE_MPEG4_AAC_AUDIO,
	PARSE_ES_TYPE_AUDIO_TITLE
};

#define AUTO_RECORD_NONE 0
#define AUTO_RECORD_PROGRAM 1
#define AUTO_RECORD_ALL 2
#define AUTO_RECORD_VLC 3
#define AUTO_RECORD_VLC_TIME_LIMITED 4

#define VBI_SERVICE_422     0x01
#define VBI_SERVICE_CC      0x02
#define VBI_SERVICE_ITXT    0x04
#define VBI_SERVICE_SUB     0x08
#define VBI_SERVICE_TXT     0x10
#define VBI_SERVICE_USER    0x20
#define VBI_SERVICE_VPS     0x40
#define VBI_SERVICE_WSS     0x80
#define VBI_SERVICE_DTVCC   0x100
#define VBI_SERVICE_WSS_43  0x200
#define VBI_SERVICE_WSS_149 0x400
#define VBI_SERVICE_WSS_169 0x800
#define VBI_SERVICE_RC      0x1000
#define VBI_SERVICE_AFD     0x2000

#define IP_SAVE_PAYLOAD 0
#define IP_SAVE_PAYLOAD_IP 1
#define IP_SAVE_PAYLOAD_IP_MPE 2

enum EVENT_ICONS
{
	EVENT_ICON_BODY_ONLY = 0,
	EVENT_ICON_INFORMATION,
	EVENT_ICON_STOP,
	EVENT_ICON_WARNING
};

#define AUDIO_CHARTX 220
#define AUDIO_CHARTTIMES 6
#define AUDIO_SAMPLES_REQUIRED AUDIO_CHARTX * AUDIO_CHARTTIMES * 2

#define DESCRIPTOR_CAT 0
#define DESCRIPTOR_PMT 1
#define DESCRIPTOR_BAT 2
#define DESCRIPTOR_SDT 3
#define DESCRIPTOR_NIT 4
#define DESCRIPTOR_TOT 5
#define DESCRIPTOR_EIT 6
#define DESCRIPTOR_VCT 7
#define DESCRIPTOR_MGT 8
#define MAX_DESCRIPTOR_TAG_ARRAY DESCRIPTOR_MGT + 1

BYTE bDescriptorTagArray[MAX_DESCRIPTOR_TAG_ARRAY][256];

#define MAX_EIT_CONNECTIONS 100
typedef struct _tagConnection
{
	SOCKET socket;
	int nCurrentEventID;
	int nChannel;
	HANDLE hEvent;
	BOOL fUpdatesRequested;
	int nPreviousEventID;
	SOCKADDR_IN acc_sin;
	BOOL fAbort;
} EITCONNECTION, *PEITCONNECTION;

#define MAX_RECORD_TABLES 16
typedef struct _tagRecordTables
{
	int nPID;
	int nStartTable, nEndTable;
	int nTableCount;
	DWORD dwPriorTickCount;
	HANDLE hFile;
} RECORDTABLES, *PRECORDTABLES;

typedef struct _tagEventList
{
	int nEventType;
	SYSTEMTIME stEvent;
	char szShort[128];
	char szLong[1024];
} EVENTLIST, *PEVENTLIST;

typedef struct _tagEventEmailItem
{
	EVENTLIST el;
	char * szMessageBody;
	char szSMTPServer[MAX_PATH];
	char szEmailAddress[MAX_PATH];
	char szEmailFrom[MAX_PATH];
} EVENTEMAILITEM, *PEVENTEMAILITEM;

typedef struct _tagCVCTENTRY
{
	char szShortName[8];

	int major_channel_number;
	int minor_channel_number;
	int modulation_mode;
	int carrier_frequency;
	int channel_TSID;
	int program_number;
	int ETM_location;
	int access_controlled;
	int hidden;
	int path_select;
	int out_of_band;
	int hide_guide;
	int service_type;
	int source_id;
	int descriptors_length;
	BYTE * pDescriptors;

} CVCTENTRY, *PCVCTENTRY;

typedef struct _tagCVCT
{
	int transport_stream_id;
	int version_number;
	int num_channels_in_section;
	CVCTENTRY CVCTEntry[MAX_CVCT_CHANNEL_ENTRIES];
	int additional_descriptors_length;
	BYTE * pAdditionalDescriptors;
} CVCT, *PCVCT;

typedef struct _tagMGT // ATSC MGT
{
	int nTableType;
	int nTablePID;
	int nDescriptorsLength;
	BYTE * pDescriptors;
} MGT, *PMGT;

typedef struct _tagIPClicklParam
{
	int nType;
	DWORD dwPtr;
} IPCLICKLPARAM, *PIPCLICKLPARAM;

typedef struct _tagIPEntry
{
	DWORD dwNext;				// gets cast!
	DWORD dwDestinationAddress;
	DWORD dwProtocol;
	int nPacketCount;
	HTREEITEM hIPItem;
	HTREEITEM hIPMacItem;
	__int64 nByteCount;
	HANDLE hSaveFile;
	BOOL fTransmitting;
	BOOL fGotFirstFragment;
	int nSaveMode;
	int nIPVersion;
	BYTE bDestinationAddressIPv6[16];
} IPENTRY, *PIPENTRY;

typedef struct _tagIPMACEntry
{
	DWORD dwNext;				// gets cast!
	int nPacketCount;
	__int64 nByteCount;
	HTREEITEM hIPMacItem;
	HTREEITEM hIPPIDRootItem;
	PIPENTRY pIPEntries;
	BYTE bMAC[8];
} IPMACENTRY, *PIPMACENTRY;

typedef struct _tagIPPIDEntry
{
	int nPID;
	int nPacketCount;
	__int64 nByteCount;
	HTREEITEM hIPPIDRootItem;
	PIPMACENTRY pMACEntries;
} IPPIDENTRY, *PIPPIDENTRY;

typedef struct _tagEITEvent
{
	DWORD dwNextEvent;			// gets heavily cast!
	int nEventID;
#define EIT_FLAG_SHORT_EVENT 0x01
#define EIT_FLAG_LONG_EVENT 0x02
#define EIT_FLAG_COMPRESSED_SHORT_EVENT 0x04
#define EIT_FLAG_COMPRESSED_LONG_EVENT 0x08
	int nFlags;
	int nRunningStatus;
	int nSource;
	BOOL fFreeCAMode;
	SYSTEMTIME stStartTime;
	SYSTEMTIME stRunTime;
	BYTE * pExtraDescriptors[MAX_EIT_EXTRA_DESCRIPTORS];
	char szEventName[512];
	char * szShortEventDescription;
	char * szLongEventDescription;
} EITEVENT, *PEITEVENT;

typedef struct _tagEITChannelData
{
	int nChannelNumber;
	int nFromTable;
	int nMajorChannelNumber, nMinorChannelNumber;
	int nCarrierFrequency, nModulationMode;
	int nTransportStreamID, nOriginalNetworkID;
	int nSourceID;
	int nRunningStatus;
	int nLogicalChannelNumber;

	BOOL fSetupSDTName;
	BOOL fSetupEITName;
	BOOL fATSC;
	BOOL fEIT_schedule_flag;
	BOOL fEIT_present_following_flag;
	BOOL fFreeCAMode;
	BYTE * pExtraDescriptors[MAX_SDT_EXTRA_DESCRIPTORS];
	HTREEITEM hSDTTreeItem;
	char szShortName[128];
	char szLongName[256];
	char szATSCChannelETT[256];
} EITCHANNELDATA, *PEITCHANNELDATA;

typedef struct _tagDVBTDT
{
	int nYear, nMonth, nDay;
	int nHour, nMinute, nSecond;
	int nGPSOffset;
	int nDaylightSavings;
	BOOL fSTTSeen;
	HTREEITEM hTreeItem;
	HTREEITEM hRootTreeItem;
} DVBTDT, *PDVBTDT;

typedef struct _tagDVBTOT
{
	int nYear, nMonth, nDay;
	int nHour, nMinute, nSecond;
	int nDescriptorsLength;
	BYTE * pDescriptors;
	HTREEITEM hTreeItem;
	HTREEITEM hRootTreeItem;
} DVBTOT, *PDVBTOT;

typedef struct _tagNITDVBSEntry
{
	int nOrbitalPosition;
	int nPolarization;
	int nModulation;
	int nSymbolRate;
	int nFEC;
	BOOL fEastern;
} NITDVBSENTRY, *PNITDVBSENTRY;

typedef struct _tagNITDVBTEntry
{
	int nBandwidth;
	int nConstellation;
	int nHierarchyInformation;
	int nCodeRateHPStream;
	int nCodeRateLPStream;
	int nGuardInterval;
	int nTransmissionMode;
	int nOtherFrequencyFlag;
} NITDVBTENTRY, *PNITDVBTENTRY;

typedef struct _tagNITDVBCEntry
{
	int nFECOuter;
	int nModulation;
	int nSymbolRate;
	int nFECInner;
} NITDVBCENTRY, *PNITDVBCENTRY;

typedef struct _tagNITEntry
{
#define NIT_DVBS 1
#define NIT_DVBT 2
#define NIT_DVBC 3
	int nType;
	int nNetworkID;
	int nTransportStreamID;
	int nOriginalNetworkID;
	int nFrequency;
	int nNetworkDescriptorsLength;
	int nVersionNumber;
	BOOL fThisTS;
	HTREEITEM hNITTreeItem;
	BYTE * pExtraDescriptors[MAX_NIT_EXTRA_DESCRIPTORS];
	BYTE * pNetworkDescriptors;
	NITDVBSENTRY dvbs;
	NITDVBTENTRY dvbt;
	NITDVBCENTRY dvbc;
	char szNetworkName[128];	
} NITENTRY, *PNITENTRY;

typedef struct _tagParsedMPEGVideo
{
	int horizontal_size_value;
	int vertical_size_value;
	int aspect_ratio_information;
	int frame_rate_code;
	int bit_rate_value;
	int marker_bit1;
	int vbv_buffer_size_value;
	int contrained_parameters_flag;
	int load_intra_quantiser_matrix;
	int load_non_intra_quantiser_matrix;

	int extension_start_code_identifier;
	int profile_and_level_indication;
	int progressive_sequence;
	int chroma_format;
	int horizontal_size_extension;
	int vertical_size_extension;
	int bit_rate_extension;
	int marker_bit2;
	int vbv_buffer_size_extension;
	int low_delay;
	int frame_rate_extension_n;
	int frame_rate_extension_d;
} PARSEDMPEGVIDEO, *PPARSEDMPEGVIDEO;

typedef struct _tagParsedMPEGAudio
{
	int layer;
	int bitrate_index;
	int sampling_frequency;
	int mode;
} PARSEDMPEGAUDIO, *PPARSEDMPEGAUDIO;

typedef struct _tagParsedAC3Audio
{
	int fscod;
	int frmsizecod;
	int bsid;
	int bsmod;
	int acmod;
	int cmixlev;
	int surmixlev;
	int dsurmod;
	int lfeon;
	int dialnorm;
} PARSEDAC3AUDIO, *PPARSEDAC3AUDIO;

typedef struct _tagParsedAACAudio
{
	int channels;
	int samplerate;
	int sbr;
	int object_type;
	int header_type;
	int num_front_channels;
	int num_side_channels;
	int num_back_channels;
	int num_lfe_channels;
} PARSEDAACAUDIO, *PPARSEDAACAUDIO;

typedef struct _tagParsedH264Video
{
	int horizontal_size_value;
	int vertical_size_value;
	int interlaced;
} PARSEDH264VIDEO, *PPARSEDH264VIDEO;


typedef struct _tagParsedVC1Video
{
	int horizontal_size_value;
	int vertical_size_value;
	int interlaced;
} PARSEDVC1VIDEO, *PPARSEDVC1VIDEO;

typedef struct _tagParsedMPEG4Video
{
	int horizontal_size_value;
	int vertical_size_value;
} PARSEDMPEG4VIDEO, *PPARSEDMPEG4VIDEO;

enum BLACKLIST
{
	BLACKLIST_NOT_BLACKLISTED = 0,
	BLACKLIST_NO_TRAFFIC,
	BLACKLIST_NO_DATA,
	BLACKLISTED_NO_PES_PACKETS
};
	
typedef struct _tagAudioSamples
{
	int nChannels;
	int nSamples;
	int nSampleData[2][AUDIO_CHARTX];
} AUDIOSAMPLES, *PAUDIOSAMPLES;

typedef struct _tagESLIST
{
	int nStreamType;
	int nESPID;
	int nDescriptorsLength;
	int nParseType;
	int nVideoWidth, nVideoHeight;
	int nTeletextServices;
	int nBlacklisted;	
	BOOL fDecoderCrashed;
	DWORD dwAFDData;
	RECT rcThumbnail;
	BYTE * pDescriptors;
	BYTE * pParsedData;
	BYTE * pRGBVideoFrame;
	HTREEITEM hESTreeItem;
	AUDIOSAMPLES as;
} ESLIST, *PESLIST;

typedef struct _tagPMT
{
	int nProgramNumber;
	int nPMTPID;
	int nPCRPID;
	int nPMTPollCounter;
	int nProgramInfoLength;	
	int nVersionNumber;
	BOOL fSetupSDTName;
	BOOL fCompleted;
	BOOL fPostTreeAddSelect;
	ESLIST es[MAX_ESLIST_ENTRIES];
	HTREEITEM hPMTTreeItem;
	HTREEITEM hPCRTreeItem;
	BYTE * pProgramInfo;
} PMT, *PPMT;

typedef struct _tagPAT
{
	int nTransportStreamID;
	int nVersionNumber;
	PMT pmt[MAX_PAT_ENTRIES];
	HTREEITEM hPATTreeItem;
	BYTE * pRawPAT;
} PAT, *PPAT;

typedef struct _tagPMTListen
{
	BOOL fOutstanding;
	int nPMTPointerKludge;
	int nPID;
	int nProgramNumber;
	int nFillPtr;
	DWORD dwStartTickCount;
	BYTE bSectionBuffer[1024];
} PMTLISTEN, *PPMTLISTEN;

typedef struct _tagCAT
{
	int nVersionNumber;
	BYTE * pDescriptor[MAX_CAT_DESCRIPTORS];
	HTREEITEM hCATTreeItem;
} CAT, *PCAT;

typedef struct _tagMMT
{
	int transmission_system;
	int inner_coding_mode;
	int split_bitstream_mode;
	int modulation_format;
	int symbol_rate;
} MMT, *PMMT;

typedef struct _tagCDT
{
	int number_of_carriers;
	int spacing_unit;
	int frequency_spacing;
	int frequency_unit;
	int first_carrier_frequency;
} CDT, *PCDT;

typedef struct _tagSIT
{
	int satellite_ID;
	int you_are_here;
	int frequency_band;
	int out_of_service;
	int hemisphere;
	int orbital_position;
	int polarization_type;
	int number_of_transponders;
} SIT, *PSIT;

typedef struct _tagTDT
{
	int satellite_ID;
	int transport_type;
	int polarization;
	int transponder_number;
	int CDT_reference;
	int MMT_reference;
	int VCT_ID;
	int root_transponder;
	int wide_bandwidth_video;
	int waveform_standard;
	int wide_bandwidth_audio;
	int companded_audio;
	int matrix_mode;
	int subcarrier_2_offset;
	int subcarrier_1_offset;
} TDT, *PTDT;

typedef struct _tagPIDCounter
{
	BOOL fPIDHasBeenActive;
	BOOL fScrambled;
	int nPID;
	int nPIDHasContinuityErrors;
	int nPIDTEICount;
	__int64 lnPackets;
	double dPercent;
	double dPIDRate;
} PIDCOUNTER, *PPIDCOUNTER;

typedef struct tag_MemoryUsage
{
	HLOCAL buffer;
	unsigned int uSize;
	int nType;
} MEMORYUSAGE, *PMEMORYUSAGE;

#define MUX_TUNE_TYPE_ADV 0
#define MUX_TUNE_TYPE_DVB 1
#define MUX_TUNE_TYPE_DSS 2
typedef struct tag_Mux
{
	int nFrequency;
	int nSymbolRate;
	int nCodeRate;
	int nModulationMode;	// ADV modulation
	BOOL fADVModulation;	// ADV modulation
	int nTuneType;
	char szPolarity[4];
	char szMuxName[128];
} MUX, *PMUX;

typedef struct tag_Satellite
{
	int nOrbitalPosition;
	int nMuxCount;
	BOOL fWest;
	BOOL fDirty;
	int nLastDiSEqC;
	int nLastLOF;
	PMUX mux;
	char szName[48];
	char szSourceFilename[48];
} SATELLITE, *PSATELLITE;

typedef struct tag_TerrestrialMux
{
	int nFrequency;
	BOOL fSpectrumInversion;
	int nBandwidth;
	char szDescription[48];
} TERRESTRIALMUX, *PTERRESTRIALMUX;

typedef struct tag_CableMux
{
	int nFrequency;
	int nSymbolRate;
	int nQAM;
	BOOL fSpectrumInversion;
	int nBandwidth;
} CABLEMUX, *PCABLEMUX;

typedef struct tag_ATSCMux
{
	int nFrequency;
	char szDescription[48];
} ATSCMUX, *PATSCMUX;

typedef struct tag_BAT_TRANSPORT_STREAM
{
	int transport_stream_id;
	int original_network_id;
	int transport_descriptors_length;
	BYTE * transport_descriptors;
} BAT_TRANSPORT_STREAM, *PBAT_TRANSPORT_STREAM;

typedef struct tag_BAT
{
	int bouquet_id;
	int version_number;
	int current_next_indicator;
	int section_number;
	int last_section_number;
	int bouquet_descriptors_length;
	HTREEITEM hTreeItem;
	BYTE * bouquet_descriptors;
	BAT_TRANSPORT_STREAM batts[MAX_BAT_TRANSPORT_ITEMS];
} BAT, *PBAT;

typedef struct tag_RRTRating
{
	char szAbbreviatedRating[12];
	char szRating[256];
} RRTRating, *PRRTRating;

typedef struct tag_RRTDimension
{
	int nRatingsDefined;
	char szRatingDimension[256];
	RRTRating rrtrating[16];
} RRTDIMENSION, *PRRTDIMESION;

typedef struct tag_RRT
{
	int nDimensionsDefined;
	char szRegionName[256];
	RRTDIMENSION rrtdimension[256];
} RRT, *PRRT;

enum LNB_TYPE
{
	LNB_TYPE_SINGLE = 0,
	LNB_TYPE_DUAL,
	LNB_TYPE_STACKED
};

enum LNB_VOLTAGE
{
	LNB_VOLTAGE_POLARITY = 0,
	LNB_VOLTAGE_BAND,
	LNB_VOLTAGE_OFF,
	LNB_VOLTAGE_14V,
	LNB_VOLTAGE_18V
};

enum LNB_22KHZ
{
	LNB_22KHZ_BAND = 0,
	LNB_22KHZ_ON,
	LNB_22KHZ_OFF
};

enum LNB_POSITIONER
{
	LNB_POSITIONER_NONE = 0,
	LNB_POSITIONER_DISEQC
};	

enum AUTO_SELECT_LNB
{
	AUTO_SELECT_LNB_OFF = 0,
	AUTO_SELECT_LNB_FREQ,
	AUTO_SELECT_LNB_ORBITAL,
	AUTO_SELECT_LNB_NETWORK
};

typedef struct tag_SwitchParameters
{
	int nLNBType;
	int nLOFrequencyHigh, nLOFrequencyLow;
	int nSwitchFrequency;
	int nVoltage;
	int n22KHzTone;
	int nPositionerType;
	int nAutoSelect;
	int nAutoSelectFreqStart, nAutoSelectFreqEnd;
	int nAutoSelectOrbital;
	int nAutoSelectNetwork;
	int nAutoSelectPolarity;
} SWITCHPARAMETERS, *PSWITCHPARAMETERS;

typedef struct tag_SourceModules
{
	DWORD dwCapabilities;
	char szFilename[MAX_PATH];
	char szDescription[128];
	char szDisplayName[64];
	char szCommandLineParameters[MAX_PATH];
} SOURCEMODULES, *PSOURCEMODULES;

typedef struct _tagTableMonitor
{
	int nPacketCount;
	__int64 lnLastTime;
	__int64 lnDelay;
	__int64 lnDelayItems;
	__int64 lnDelayMax;
	__int64 lnDelayMin;
	__int64 lnLastDisplayDelay;
	int nLastDisplayPacketCount;
	BOOL fInList;
} TABLEMONITOR, *PTABLEMONITOR;

// EPG Grid storage
typedef struct _tagScreenEvents
{
	int nChannel;
	RECT rc;
	PEITEVENT pEITEvent;
} SCREENEVENTS, *PSCREENEVENTS;

typedef struct _tagEITEventWithPtr
{
	EITEVENT eitevent;
	PEITEVENT original;
} EITEVENTWITHPTR, *PEITEVENTWITHPTR;

#define MAX_EPG_MAPS 32
typedef struct _tagEPGGridVariables
{
	BOOL fTimerRunning;
	BOOL fDisplayEPGByLCN;
	BOOL fHideChannelSelectMode;
	BOOL fFoundSomething;
	BOOL fWindowsSchedulerActive;
	
	int nSelectedChannel;
	int nSearchFlag;
	int nHorizontalScrollPos;
	int nVerticalScrollPos;
	int nTimeRangeShown;
	int nMaxEITChannels;
	int nEITChannelsDisplayed;
	int nGotoChannel;
	int nSearchChannel;
	int nSearchProgramIndex;
	int nSelectedEPGChannel;
	int nMapSourceProgram[MAX_EPG_MAPS], nMapDestinationProgram[MAX_EPG_MAPS];	
	
	__int64 nStartDisplayTime;
	__int64 nActualStartTime;

	HFONT hGridTextFont;
	HFONT hGridTextFontBold;
	HFONT hGridSmallTextFont;
	HFONT hTitleEventFontBold;
	HFONT hEventSmallTextFont;
	HBRUSH hCellEntryBackground;
	HBRUSH hCellEntrySelectedBackground;
	HBRUSH hCellEntryRecordBackground;
	HBRUSH hChannelEntryBackground;
	HBRUSH hCellEntryHideModeBackground;
	HPEN hCellEntryPrePostRollPen;
	HICON hTV_14, hTV_G, hTV_MA, hTV_PG, hTV_Y, hTV_Y7, hTV_Y7FV;
	HICON hCC;
	HMODULE hSchedulerDLL;
	HPEN hTimeGridPen;
	HICON hEPGChannelHidden;

	PSCREENEVENTS screenevents;
	PEITEVENT pSelectedEvent;

	BYTE bHiddenChannels[8192];

	char szSearchString[255];

} EPGGRIDVARIABLES, *PEPGGRIDVARIABLES;

typedef struct _tagStreamMonitor
{
	BOOL fDisabled;
	int nStatus;
	__int64 lnLastAlarmTime;
} STREAMMONITOR, *PSTREAMMONITOR;

#define XML_LOG_TYPE_WMUSER2 0
#define XML_LOG_TYPE_THUMBNAIL 1

typedef struct _tagXMLLog
{
	int nXMLLogType;
	BOOL fSent;
	WPARAM wParam;
	LPARAM lParam;
	int nProgram, nESIndex;
	char szFilename[MAX_PATH];
} XMLLOG, *PXMLLOG;

enum MONITOR_LOCATIONS
{
	MONITOR_ETR290_1_1 = 0,
	MONITOR_ETR290_1_2,
	MONITOR_ETR290_1_3,
	MONITOR_ETR290_1_4,
	MONITOR_ETR290_1_5,
	MONITOR_ETR290_1_6,
	MONITOR_ETR290_2_1,
	MONITOR_ETR290_2_2,
	MONITOR_ETR290_2_3,
	MONITOR_ETR290_2_4,
	MONITOR_ETR290_2_5,
	MONITOR_ETR290_2_6,
	MONITOR_ETR290_3_1,
	MONITOR_ETR290_3_2,
	MONITOR_ETR290_3_3,
	MONITOR_ETR290_3_4,
	MONITOR_ETR290_3_5,
	MONITOR_ETR290_3_6,
	MONITOR_ETR290_3_7,
	MONITOR_ETR290_3_8,
	MONITOR_ETR290_3_9,
	MONITOR_ETR290_3_10,
	MONITOR_COUNT,
	MONITOR_ATSC_1 = MONITOR_ETR290_3_1,
	MONITOR_ATSC_2,
	MONITOR_ATSC_3,
	MONITOR_ATSC_4,
	MONITOR_ATSC_5,
	MONITOR_ATSC_6
};

enum SERIAL_RECEIVER_TYPES
{
	SERIAL_RECEIVER_ALTEIA_PLUS = 1,
	SERIAL_RECEIVER_DSR4800,
	SERIAL_RECEIVER_TT1260,
	SERIAL_RECEIVER_NEWTEC_DVB2063,
	SERIAL_RECEIVER_MAX
};

typedef struct _tagStreamMonitorLog
{
	int nAlarmCode;
	int nAlarmSubcode;
	SYSTEMTIME st;
} STREAMMONITORLOG, *PSTREAMMONITORLOG;


// Transport stream buffer
#define MAX_RECORD_BUFFERS 64
#ifdef LITE
 #define ACTUAL_MAX_RECORD_BUFFERS 1
#else LITE
 #ifndef PRO
  #define ACTUAL_MAX_RECORD_BUFFERS 16
 #else PRO
  #define ACTUAL_MAX_RECORD_BUFFERS MAX_RECORD_BUFFERS;
 #endif PRO
#endif LITE

typedef struct _tagESParserInfo
{
	int nES;
	int nProgramNumber;
	CRITICAL_SECTION csThreadSignal;
} ESPARSERINFO, *PESPARSERINFO;

// Forwarder definitions for Pro
typedef struct _tagFwdFunctions
{
	BOOL (* Fwd_Init) (HWND hWnd, int nPacketLength, int nBitRate);
	BOOL (* Fwd_DeInit) ();
	BOOL (* Fwd_Data) (BYTE * pBuffer, int nLength);
	BOOL (* Fwd_GetDescription) (char * szDeviceNameBuffer);
} FWDFUNCTIONS, *PFWDFUNCTIONS;

#define MAX_FWD_DLLS 16
typedef struct _tagFwd
{
	int nForwarderDLLCount;
	int nForwarderModulesActive;
	HANDLE hDLL[MAX_FWD_DLLS];
	FWDFUNCTIONS functions[MAX_FWD_DLLS];
	BOOL fActive[MAX_FWD_DLLS];
	CRITICAL_SECTION csPipeBytes[MAX_FWD_DLLS];
	HANDLE hReadPipe[MAX_FWD_DLLS], hWritePipe[MAX_FWD_DLLS];
	BOOL fTerminateThread[MAX_FWD_DLLS], fThreadTerminated[MAX_FWD_DLLS];
	void * RShandle[MAX_FWD_DLLS];
	BYTE bInterleaver[1122][MAX_FWD_DLLS];
	int nSyncCounter[MAX_FWD_DLLS];
	int nInterleaveRow[MAX_FWD_DLLS];
	int nPBRSReg[MAX_FWD_DLLS];

} FWD, *PFWD;

#define ES_BUFFER_SIZE (65536 * 10)
typedef struct _tagTSParserVariables
{
	BOOL fRecordBufferActive[MAX_RECORD_BUFFERS];
	BOOL fGotAdaptation;
	BOOL fIgnoreContinuity;

	int nPointer;
	int nBufferID;
	int nBufferOffset;
	int nBufferNumber;
	int nPESLength[REAL_MAX_ES_PARSERS], nCCPESLength, nVideoPESLength[REAL_MAX_CHARTS];
	int nPreviousBuffers;
	int nPacketLength;
	int nESFillPtr[REAL_MAX_ES_PARSERS], nCCESFillPtr, nVideoESFillPtr[REAL_MAX_CHARTS];
	int nActivityByteCounter;
	int nIncomingBufferLength;
	int nActualMaxRecordBuffers;

	BOOL fBufferSections[REAL_MAX_ES_PARSERS];

	DWORD dwRecordSize[MAX_RECORD_BUFFERS];

	HANDLE hRecordPIDFile[MAX_RECORD_BUFFERS];

	BYTE * pOutputRecordPackets[MAX_RECORD_BUFFERS];
	BYTE * pIncomingBuffer;
	BYTE * bESBuffer[REAL_MAX_ES_PARSERS], * bCCESBuffer, *bVideoESBuffer[REAL_MAX_CHARTS];
	DWORD * pIncomingTimestamps;

} TSPARSERVARIABLES, *PTSPARSERVARIABLES;

typedef struct tag_Variables
{
	__int64 lnPIDSecondCounter[60];
	__int64 lnPIDCounter[8192];
	__int64 lnTotalTSPackets;
	__int64 lnCopyTotalTSPackets;
	__int64 lnMuxRatePCR;
	__int64 lnRecordSplitPCR;
	__int64 lnPIDRatePCR[8192];
	__int64 lnESParseStartTime[REAL_MAX_ES_PARSERS];
	__int64 lnPIDRateBytes[8192];
	__int64 lnPIDRateSamples[8192];
	__int64 lnMaxPackets;
	__int64 nDSSRTCOffset;
	__int64 lnTicksPerSecond;
	__int64 nLastEPGDisplayTime;
	__int64 nSIParserPackets[16];
	__int64 nSIParserCRCs[16];

	int nEITEvents;
	int nSITreeIcons[50];
	int nMinimumSDTChannel, nMinimumEITChannel;
	int nSIParserVersionNumbers[16];
	int nSIParserTableErrors[16];
	int nSIParserTimingErrors[16];
	int nExportSITables;
	int nEITChannels;
	int nFillPtr[MAX_SECTION_BUFFERS];
	int nPMTPID;
	int nPMTProgramIndex;
	int nActivePIDCount;
	int nLastSecondByteCounter;
	int nPIDScrollOffset;
	int nPIDChartItemCount;
	int nHighlightPIDs[MAX_ESLIST_ENTRIES];
	int nSelectedProgram, nSelectedVideoDisplayProgram;
	int nRecordAudioESIndex[MAX_AUDIO_STREAMS];
	int nRecordVideoESIndex;
	int nRecordPCRPID, nRecordVideoPID, nRecordPMTPID;
	int nRecordAudioPID[MAX_AUDIO_STREAMS], nRecordAudioType[MAX_AUDIO_STREAMS];
	int nPMTTimeoutCounter, nTreeUpdateCounter, nTreeUpdateCounter2;
	int nPATPointerKludge, nPMTPointerKludge;
	int nNetworkPID;
	int nMaxMMT, nMaxCDT, nMaxSIT, nMaxTDT;
	int nIPMonitorPID[8192];
	int nPIDContinuity[8192];
	int nContinuityErrors, nTEIErrors;
	int nMuxRatePID, nMuxRateBytes;
	int nMuxRateCounter;
	int nSplitFileNumber, nSplitFileSize;
	int nAutoRecordProgram;
	int nAutoRecordSeconds, nAutoRecordSecondsReload;
	int nAutoRecordSecondCounter;
	int nESParseType[REAL_MAX_ES_PARSERS];
	int nESParsePMTIndex[REAL_MAX_ES_PARSERS];
	int nESParseESIndex[REAL_MAX_ES_PARSERS];
	int nESParsePID[REAL_MAX_ES_PARSERS];
	int nPATRecordCounter;
	int nESParsingCounter, nESParsingCounterReload;
	int nDecodeNoPIDTrafficCounter[REAL_MAX_ES_PARSERS], nDecodeNoPESLengthCounter[REAL_MAX_ES_PARSERS];
	int nStreamProcessingThreadPriority, nThumbnailProcessingThreadPriority;
#define STREAM_TO_STRADIS 1
#define STREAM_TO_DIRECTSHOW 2
#define STREAM_TO_XNS 3
#define STREAM_TO_DVHS 4
#define STREAM_TO_VLC 5
#define STREAM_TO_ROKU 6
	int nStreamTo;
	int nMemoryUsageItems;
	int nSatellites;
	int nCurrentSelectedSatellite;
	int nSelectedSource;
	int nThumbnailImageCount, nThumbnailScrollOffset, nThumbnailDisplayCount;
	int nPipeBytes;
	int nXNSServerPort;
	int nTerrestrialMuxes;
	int nATSCMuxes, nISDBTMuxes;
	int nLastPATHighestVersionNumber, nCheckPATCounter;
	int nVLCPort;
	int nMaxSourcePIDs;
	int nProgramPIDCount, nProgramPIDs[8192];
	int nRemoteControlSocket;
	int nVLCPlaybackConfig;
	int nStreamingPipeSize, nThumbnailPipeSize;
	int nCableMuxes;
	int nTSBuffers, nStreamBuffers;
	int nInputActivityPosition, nForwarderActivityPosition;
	int nAutoRecord;
	int nAutoRecordAudioTrack;
	int nEITPID;
	int nPIDHasContinuityErrors[8192];
	int nPIDTEICount[8192];
	int nAutoVLCConfiguration;
	int nNullPID;
	int nATSCEITPID[64], nATSCETTPID[64];
	int nATSCCETTPID;
	int nControlServerPort;
	int nMinimumPATs, nMaximumMPEGPictures, nMaximumDCIIPictures, nMaximumH264Pictures;
	int nRecordProgram;
	int nTransmittingCount;
	int nTunerLoops;
	int nNITRightClickIndex, nSDTRightClickIndex;
	int nDCIIECMPMTIndex;
	int nDCIIECMDescriptorPID;
	int nDCIIECMDescriptorTimeout;
	int nChartStyle;
#define GOT_NO_KEYS 0
#define GOT_EVEN_KEY 1
#define GOT_ODD_KEY 2
#define GOT_BOTH_KEYS 3
#define GOT_DISABLE 4
	int nGotKeys;
	int nDecryptPIDs[32];
	int nDecryptPIDCounter;
	int nEPGHalfHourWidth, nEPGChannelHeight;
	int nFFCSAPackets;
	int nAudioStreams;
	int nDiSEqCPosition, nDiSEqCDelay;
	int nCaptionPID;
	int nSavedCmdShow;
	int nCCPipeBytes;
	int nAutoRecordPIDsDuration, nAutoRecordPIDsOptions;
	int nAutoRecordPIDsPID[MAX_RECORD_BUFFERS];
	int nExtraSerialTuneDelay;
	int nGraphRefreshRate;
	int nTableMonitorPID;
	int fTableMonitorRunning;
	int nTableMonitorFillPtr;
	int nAutoRestartOnDataStopDelay;
	int nAutoRestartOnDataStopCounter;
	int nMainWindowSizeX, nMainWindowSizeY;
	int nMainWindowPositionX, nMainWindowPositionY;
	int nArchiveWindowX, nArchiveWindowY, nArchiveWindowW, nArchiveWindowH;
	int nForwarderWindowX, nForwarderWindowY, nForwarderWindowW, nForwarderWindowH;
	int nEPGWindowX, nEPGWindowY, nEPGWindowW, nEPGWindowH;
	int nChartWindowX, nChartWindowY, nChartWindowW, nChartWindowH;
	int nMosaicWindowX, nMosaicWindowY, nMosaicWindowW, nMosaicWindowH;
	int nEITServerPort;
	int nOtherStreams;
	int nRecordOtherESIndex[MAX_OTHER_STREAMS];
	int nRecordOtherPID[MAX_OTHER_STREAMS];
	int nOutputPMTPackets;
	int nProcessPriority;
	int nPolarity;
	int nRecordPIDsPCRPID, nRecordPIDsPCRContinuity;
	int nProfileBrowserX, nProfileBrowserY;
	int nProfileBrowserWidth, nProfileBrowserHeight;
	int nVideoCompositionPID[REAL_MAX_CHARTS];
	int nARCHIVEDPROGRAMSMax;
	int nARCHIVEDPROGRAMSCount;
	int nSelectedArchiveProgram;
	int nEPGSaveCount;
	int nChartParameters;
	int nBATRightClickIndex, nTDTRightClickIndex;
	int nCurrentBATID;
	int nPrefereredCAID;
	int nCurrentVLCEditConfig;
	int nProfileCount, nProfileMax;
	int nProfileListViewState;
	int nSortProfileColumn;
	int fProfileColumnAscending;
	int nInitialProfileXPos, nInitialProfileYPos, nInitialProfileXSize, nInitialProfileYSize;
	int nSingleThumbnailChannel;
	int nTrayIconBlinkCounter;
	int nGraphHistoricalPoints;
	int nMGTDescriptorsLength;
	int nSkyEPGPIDs[3];
	int nCAPIDs[MAX_CA_PIDS];
	int nAutoExportDelay;
	int nSelectedDescriptorItem;
	int nSyncLossCount, nRetuneCount;

	int nMonitorWindowX, nMonitorWindowY, nMonitorWindowW, nMonitorWindowH;
	int nMonitorSyncLossCount;
	int nMonitorContinuityErrors;
	int nMonitorTEICounter;
	int nMonitorCRCCounter;
	int nMonitorPATTimingErrors;
	int nMonitorPATTableErrors;
	int nMonitorPMTTimingErrors;

	int nStreamMonitorMax;
	int nStreamMonitorCurrent;
	int nSerialReceiverControlIndex;
	int nCaptionChannel;
	int nStradisAPI;
	int nIgnoredNetworks[MAX_IGNORED_NETWORKS];
	int nPictureDataCount[REAL_MAX_CHARTS][MAX_CHART_GOP_LENGTH];
	int nPictureType[REAL_MAX_CHARTS][MAX_CHART_GOP_LENGTH];
	int nPictureIndex[REAL_MAX_CHARTS];
	int nVideoCompositionPoints[REAL_MAX_CHARTS];
	int nSignalChartMode[REAL_MAX_CHARTS];
	int fChartGradientBitmap;
	int nMaxGOPLength[REAL_MAX_CHARTS];
	int nMinGOPLength[REAL_MAX_CHARTS];
	int nTotalGOPLength[REAL_MAX_CHARTS];
	int nGOPLengthSamples[REAL_MAX_CHARTS];
	int nAdvancedRecordRemoveOldLimitGB;
	int nStreamMonitorAlarmTimeout;
	int nStreamMonitorClickIndex;
	int nXMLLogCount, nXMLLogMax;
	int nPIDDataRefreshRate;
	int nINTPID, nINTService;
	int nMaxEPGDisplayChannel;
	int nThumbnailSize;
	int nMaximumThumbnailThreads;
	int nLocationLatitude, nLocationLongitude, nLastOrbital;
	int nSocketDialogMode;
	int nSelectedATSCMux;
	int nSelectedDVBTMux;
	int nUDPListEntries;
	int nSelectedUDPEntry;
	int nCurrentEditSwitch;
	int nSyncThread_Syncword;
	int nSyncThread_PacketLength;
	int nSyncThread_PipeBytes;
	int nSyncThread_SyncLossCount;
	int nCurrentlySelectedMux;
	int nInitialMuxIndex, nInitialSatelliteIndex;
	int nPIDUsageStackedAreaChartIndex;
	int nSelectedPCRPID;
	int nTableMonitorSectionTable;
	int nAutoHTMLExportFlags;
	int nStatusNotDirtyCounter;
	int nPopupSelectedPMTIndex, nPopupSelectedESIndex;
	int SavednEPGHalfHourWidth;
	int SavednEPGChannelHeight;
	int SavedfShowEPGChannelsOnly;
	int SavedfShowEPGThisMuxOnly;
	int SavedfEPGTimeGrid;
	int SavedfEPGTimeGridOnTop;
	int nSchedulerDefaultPreRoll, nSchedulerDefaultPostRoll;
	int nCommandLineChart;
	int nForwarderByteCounter;
	int nGPSSerialWrite;
	int nGPSSerialRead;
	int nGPSSerialBaudRate;
	int nGPSLogSeconds;
	int nGPSErrorLogMilliseconds;
	int nGPSLogTime;
	int nCCLogModeBits;
	int nCCStreamMask;
	int nCCDumpOptions;
	int nEITTreeItemCount, nSDTTreeItemCount, nNITTreeItemCount, nBATTreeItemCount;
	int nSongTitleParserPMT;
	int nSongTitleParserTable;
	int nGPSLineBufferWritePos;
	int nGPSFixQuality;
	int nGPSSatellitesTracked;
	int nGPSReceiveState;
	int nGPSReceiveStateCounter;
	int nGPSReceiverTimeout;
	int nGPSBadPacketCount;
	int nGPSSerialPortCount;
	int nGPSSignalMode;
	int nGPSErrorLogContinuityErrors, nGPSErrorLogContinuityErrorsDelta;
	int nGPSErrorLogTEIErrors, nGPSErrorLogTEIErrorsDelta;
	int nGPSAutoLogHalfSecondCounter;

	DWORD dwSavedEPGEventColor;		
	DWORD dwSavedEPGChannelColor;		
	DWORD dwSavedEPGSelectedColor;		
	DWORD dwSavedEPGMainTextColor;		
	DWORD dwSavedEPGSubTextColor;
	DWORD dwSavedEPGTimeGridColor;
	DWORD dwTaskbarRestartMessage;
	DWORD dwRecordTickCounter;
	DWORD dwSourceCapabilities;
	DWORD dwWaitForCACounter;
	DWORD dwScrambledPictureHeight[3], dwScrambledPictureWidth[3];
	DWORD dwScrambledPIDColor, dwUnscrambledPIDColor;
	DWORD dwScrambledInactivePIDColor, dwUnscrambledInactivePIDColor;
	DWORD dwHighlightedPIDColor;
	DWORD dwEPGEventColor, dwEPGSelectedColor, dwEPGChannelColor;
	DWORD dwEPGMainTextColor, dwEPGSubTextColor, dwEPGTimeGridColor;
	
	BOOL fPIDScrambled[8192];
	BOOL fPIDActive[8192];
	BOOL fRecording;
	BOOL fRecordAllTS;
	BOOL fDiscardNULLPIDs;
	BOOL fIPDVBMode, fIPDVBModeChanged;
	BOOL fDidCAT;
	BOOL fTreeViewSelectedAtLeastOnce;
	BOOL fDeletingAllTVItems;
	BOOL fSortChartByPID;
	BOOL fSortChartDecending;
	BOOL fSplitRecord, fSplitSeconds;
	BOOL fAutoXMLExport, fAutoXMLFormatAsXMLTV;
	BOOL fRecordPIDMode;
	BOOL fParseThreadRunning;
	BOOL fRecordPIDNoTSHeader;
	BOOL fESParseDecodedHeader[REAL_MAX_ES_PARSERS];
	BOOL fESParseDecoderStartedLibMPEG[REAL_MAX_ES_PARSERS];
	BOOL fMPEG2DecoderThreadRunning[REAL_MAX_ES_PARSERS];
	BOOL fCompletedESParsing;
	BOOL fPostInitialParse;
	BOOL fRecordDialogStreamOnly;
	BOOL fAllowStradisAC3;
	BOOL fStradisPAL;
	BOOL fDontLoadStradis;
	BOOL fRecordPIDsOneFile;
	BOOL fStradisActive;
	BOOL fStradisAutostart;
	BOOL fStradisInterface, fDSInterface, fXNSInterface;
	BOOL fESParseDecoderCompletedLibMPEG[REAL_MAX_ES_PARSERS];
	BOOL fKeepPastEITData;
	BOOL fDisableResolutionWarning;
	BOOL fRecordProgramStream, fSavedRecordProgramStream;
	BOOL fDVHSInterface;
	BOOL fControlDVHSDeck, fPowerCycleDVHSDeck;
	BOOL fXNSTerminated;
	BOOL fRunning;
	BOOL fIgnoreEIT;
	BOOL fThumbnailThreadAnimated;
	BOOL fAutoExpandPMTs, fAutoExpandIPs;
	BOOL fIgnoreTableCRCErrors;
	BOOL fDisablePSWarning;
	BOOL fPIDChartDisabled;
	BOOL fForceStradisPAL;
	BOOL fUseInternalCSA;
	BOOL fVLCControl, fVLCInterface, fVLCNoWarn;
	BOOL fATSCRecordMode;
	BOOL fMDPluginsLoaded;
	BOOL fSourceCanBeStopped;
	BOOL fLiteMonitorThreadActive;
	BOOL fWarnedAboutLite;
	BOOL fAgreedToLicense;
	BOOL fSaveThumbnails;
	BOOL fRunHidden;
	BOOL fIgnoreRecordAllPIDLimitationWarning;
	BOOL fSaveAllThumbnailsSameName, fSavedThumbnailsFullSize;
	BOOL fIgnorePMT65500, fIgnorePMT800x0ff6;
	BOOL fDisableLNBFrequencyWarning;
	BOOL fDVHSForceATSC;
	BOOL fCountContinuityErrors;
	BOOL fDisableStreamParsing;
	BOOL fReloadManualChannels;
	BOOL fDirtyManualChannels;
	BOOL fSingleInstance;
	BOOL fHideWhenMinimized, fHideWhenMinimizedTemporary, fCurrentMinimized, fBlockResizeMessage;
	BOOL fSendBogusHTTPSize;
	BOOL fDontShowVLCConnectionDialog;
	BOOL fRecordPIDsAppend;
	BOOL fDataReceviedInParseIncomingDataThread;
	BOOL fTerminateRokuThread;
	BOOL fRokuTraceDisabled;
	BOOL fRokuTraceAutoscroll;
	BOOL fHRCQAM;
	BOOL fWaitForCAThumbnail;
	BOOL fShowScrambledChannels;
	BOOL fControlServerEnabled;
	BOOL fIgnorePSIP;
	BOOL fSerialReceiverControlThreadRunning;
	BOOL fKeepSpecialXMLCharacters;
	BOOL fAutoRecordFromControlServer;
	BOOL fSDTOnlyForCurrentMux;
	BOOL fAudioPMTETSI;
	BOOL fAllowResizing;
	BOOL fCurrentMaximized;
	BOOL fUniprocessorMode;
	BOOL fDecimalPIDs;
	BOOL fHideThumbnailIcons;
	BOOL fRecordLimit;
	BOOL fAutoRestartOnPATVersionChange, fAutoRestartOnDataStop;
	BOOL fPlainCADescriptors;
	BOOL fLiteChartWarning;
	BOOL fFullThumbnails;
	BOOL fStopping;
	BOOL fSourceInitFailed;
	BOOL fCCThreadRunning;
	BOOL fShowEPGChannelsOnly, fShowEPGThisMuxOnly;
	BOOL fAudioThumbnails;
	BOOL fMaximizedFlag;
	BOOL fMinimizedFlag;
	BOOL fAutoRestartNoTuneDialog, fAutoRestartNoDialogInProgress;
	BOOL fAutoRestartNoBeep;
	BOOL fArchiveRunning;
	BOOL fNoDSSSupport;
	BOOL fAlwaysOnTop;
	BOOL fRealtimeCharting;
	BOOL fShowArchiveLocalTime;
	BOOL fEITServerEnabled;
	BOOL fEPGSaveFirstTime;
	BOOL fEPGSaveEnabled;
	BOOL fChartMaximizedFlag, fEPGMaximizedFlag;
	BOOL fChartMinimizedFlag, fEPGMinimizedFlag;
	BOOL fFastPMTParserDisabled, fContinuousPMTParserDisabled;
	BOOL fRecordPIDIncludePCR;
	BOOL fIgnoreTEIErrors;
	BOOL fAutoRecordSubtitles;
	BOOL fShowProfileBrowser;
	BOOL fProfileBrowserMaximized;
	BOOL fSkyEPG;
	BOOL fArchiveViewThumbnailThreadRunning;
	BOOL fTerminateArchiveViewThumbnailThread;
	BOOL fSkyEPGMapComplete;
	BOOL fArchiveTerminate;
	BOOL fUsePreferedCAID;
	BOOL fForwarderEnabled;
	BOOL fProfileBrowserShellMode;
	BOOL fMDIIndexActive;
	BOOL fShowNonVideoPCR;
	BOOL fContinueAfterProfileBrowser;
	BOOL fProfilesMaximized;
	BOOL fProfileLabelEditActive;
	BOOL fTuneFromControlServer;
	BOOL fMosaicMaximizedFlag, fMosaicMinimizedFlag;
	BOOL fSchedulerNoDateTime;
	BOOL fIncludeCAData;
	BOOL fMonitorMinimizedFlag, fMonitorMaximizedFlag;
	BOOL fMonitorRunning;
	BOOL fMonitorTerminateThread;
	BOOL fEPGTimeGrid, fEPGTimeGridOnTop;
	BOOL fBalloonQueued;
	BOOL fSaveChartDataEnabled;
	BOOL fParserDisabled;
	BOOL fSetPoints[REAL_MAX_CHARTS];
	BOOL fActivePIDsByPID[REAL_MAX_CHARTS];
	BOOL fDontShowBombDialog;
	BOOL fSchedulerRequiresLogin;
	BOOL fDVBHMACs;
	BOOL fAdvancedRecordRemoveOldEnabled;
	BOOL fAdvancedRecordUTCTime;
	BOOL fAdvancedRecordDropPID;
	BOOL fStreamingXMLMode;
	BOOL fWarnBeforeOverwritingRecordings;
	BOOL fDontShowSchedulerWarning;
	BOOL fEITLanguageFilterEnabled;
	BOOL fEPGRecordPS;
	BOOL fRecordTablesHexASCII;
	BOOL fEPGUpdateRealtime;
	BOOL fEPGDisplayActive;
	BOOL fRecordTablesActive;
	BOOL fISDB;
	BOOL fStreamMonitorClockSystem;
	BOOL fThumbnailsRightToLeft;
	BOOL fSMTPNeedsAuthentication;
	BOOL fStreamMonitorEmailEnabled;
	BOOL fAutomaticForwarding, fAutomaticStreamMonitor;
	BOOL fDiSEqCPostionIsUSALS;
	BOOL fExportNITINITurbo, fExportNITINIIncludeIgnored;
	BOOL fCtrlDown;
	BOOL fTuneDialogFirstTime;
	BOOL fBlockFirstMuxListSelection;
	BOOL fBlockFirstSatelliteListSelection;
	BOOL fDoingScan;
	BOOL fAbortScan;
	BOOL fADVModulation;
	BOOL fQAMMode;
	BOOL fSyncThread_PipeThreadTerminated;
	BOOL fNewMuxCircular;
	BOOL fAutomaticRecordAll;
	BOOL fManualEPGNoSourceParametersWarning;
	BOOL fForcedNetworkType;
	BOOL fTableMonitorSectionDisplayEnabled;
	BOOL fDisableBlacklisting;
	BOOL fEITServerInitialized;
	BOOL fStatusDirty;
	BOOL fCheckNewVersionThreadRunning;
	BOOL fQuietFromCommandLine;
	BOOL fSchedulerWake;
	BOOL fDefaultPlaybackViaStradis;
	BOOL fWarnAboutCSA;
	BOOL fStopGPSSerialThread;
	BOOL fGPSSerialThreadReady;
	BOOL fGPSSerialThreadRunning;
	BOOL fGPSLogManually, fGPSErrorLogEnabled;
	BOOL fCCLogActive;
	BOOL fLocationSouth, fLocationWest, fLastOrbitalWest;
	BOOL fDontWarnAboutInccorectAutoRecordProgram;
	BOOL fSongTitleParserEnabled;
	BOOL fGPSNorth;
	BOOL fGPSEast;

	DWORD64 lnStartTime;
	DWORD64 lnRecordTime;

	double dTotalRecorded;
	double dThisFileRecorded;
	double dDisplayMuxRate;
	double dPIDRate[8192];
	double dSIParserTableTime[16];
	double dGPSLatitude;
	double dGPSLongitude;

	float fVideoMaxRate[REAL_MAX_CHARTS];
	float fMaxProgramStackedRate[REAL_MAX_CHARTS];

	PIDCOUNTER pc[8192];
	PIDCOUNTER new_pc[8192];

	BYTE * pRGB_422;
	BYTE * pRGB_cc;
	BYTE * pRGB_itxt;
	BYTE * pRGB_sub;
	BYTE * pRGB_txt;
	BYTE * pRGB_user;
	BYTE * pRGB_vps;
	BYTE * pRGB_wss;
	BYTE * pRGB_dtvcc;
	BYTE * pRGB_rc;
	BYTE * pRGB_4x3;
	BYTE * pRGB_14x9;
	BYTE * pRGB_16x9;
	BYTE * pRGB_AFD;
	BYTE * pRGB_MPG2Video, * pRGB_BL_MPG2Video;
	BYTE * pRGB_DCIIVideo, * pRGB_BL_DCIIVideo;
	BYTE * pRGB_MPG4Video, * pRGB_BL_MPG4Video;
	BYTE * pRGB_H264Video, * pRGB_BL_H264Video;
	BYTE * pRGB_VC1Video, * pRGB_BL_VC1Video;
	BYTE * pRGB_MPEGAudio, * pRGB_BL_MPEGAudio;
	BYTE * pRGB_AC3Audio, * pRGB_BL_AC3Audio;
	BYTE * pRGB_AACAudio, * pRGB_BL_AACAudio;

	BYTE * pFFKeySpaceBase;
	BYTE * pFFKeySpace;
	BYTE * pMGTDescriptors;
	char * szSourceParametersPtr;
	BYTE * szGPSSerialBuffer;

	HANDLE hGPSPort;
	HWND hDlgSIParser;
	HINSTANCE hInstance;
	HIMAGELIST hSIParserImageList;
	HTREEITEM hNITRootTreeItem, hSDTRootTreeItem, hEITRootTreeItem;
	HTREEITEM hMMTRootTreeItem, hCDTRootTreeItem, hSITRootTreeItem, hTDTRootTreeItem;
	HTREEITEM hMGTRootTreeItem, hRRTRootTreeItem, hCVCTRootTreeItem;
	HTREEITEM hBATRootTreeItem;
	HFONT hPIDFont;
	HFONT hSourceInfoFont;
	HFONT hCourierNew;
	HFONT hGPSDisplayFont;
	HANDLE hRecordFile, hStradisReadPipe;
	HINSTANCE hThdemul;
	HINSTANCE hThdevice;
	HANDLE hDebugFile;
	HANDLE hRecordPIDFile[8192];
	HMODULE hStradisInterface;
	HANDLE hStreamProcessingThread;
	HANDLE hMPEGDecoderReadPipe[REAL_MAX_ES_PARSERS], hMPEGDecoderWritePipe[REAL_MAX_ES_PARSERS];
	HMODULE hCSA;
	HMODULE hXNSInterface;
	HMODULE hVLCInterface;
	HMODULE hSource;
	HWND hWndST;
	HWND hWndTT;
	HWND hWndChart[REAL_MAX_CHARTS];
	HANDLE hStatusBarIcons[16];
	HGLOBAL hScrambledPicture[3];
	HBRUSH hBr, hBrRed, hBrGreen, hBrDarkRed, hBrDarkGreen;
	HBRUSH hSourceInfoBrush1, hSourceInfoBrush2;
	HANDLE hRegistryRoot;
	HTREEITEM hLastSelectedTreeItem;
	HMODULE hUDPSender;
	HICON hDialogIcon;
	HMODULE hFFCSA;
	HWND hWndEPGGrid;
	HACCEL hAccel;
	HWND hWndMainWindow;
	HANDLE hCCReadPipe, hCCWritePipe;
	HANDLE hArchiveReadPipe, hArchiveWritePipe;
	HANDLE hForwarderReadPipe, hForwarderWritePipe;
	HWND hWndCCDisplay;
	HANDLE hEPGSaveHandle;
	HWND hWndArchiveRun;
	HPEN hRedPen;
	HPEN hGreenPen;
	HWND hWndForwarderRun;
	HWND hDlgMDIIndex;
	HWND hWndProfile;
	HWND hWndProfileLV;
	HWND hWndProfileRB, hWndProfileTB;
	HIMAGELIST himlSmall;		// image list for list view
	HIMAGELIST himlLarge;		// image list for list view
	HWND hWndMainDlgSubclass[32];
	HWND hWndVideoMosaic;
	WNDPROC wpSubclassOrigProc[32];
	HWND hDlgStreamMonitor;
	HWND hPluginTranslateDialog[16];
	HICON hMonitorStatusIcon[6];
	HANDLE hStreamMonitorLog;
	HMODULE hSerialReceiverControl[MAX_SERIAL_RECEIVERS];
	HWND m_hPE[REAL_MAX_CHARTS];
	HANDLE hSaveDataFile[REAL_MAX_CHARTS];
	HDC hThumbnailDC;
	HBITMAP memThumbnailBM;
	RECT rcVideoBorder;
	HANDLE hSyncThread_PipeRead;
	HANDLE hSyncThread_PipeWrite;
	HWND hWndArchiveMonitor;
	HWND hTableViewerSectionDisplayWindow;
	HANDLE hCheckNewVersionThread;
	HICON hCheckNewVersionStatusIconBad, hCheckNewVersionStatusIconGood;
	HANDLE hCCLogFile;

	SOCKET SocketControlBase, SocketControl;
	SOCKET UDPsock;

	struct sockaddr_in UDPserver;
	struct sockaddr_in UDPclient;

	CRITICAL_SECTION csEIT;
	CRITICAL_SECTION csThumbnails;
	CRITICAL_SECTION csPipeBytes;
	CRITICAL_SECTION csCCPipeBytes;
	CRITICAL_SECTION csAutoRestartOnDataStopCounter;
	CRITICAL_SECTION csMDI;
	CRITICAL_SECTION csActualRecordFilename;
	CRITICAL_SECTION csXMLLog;
	CRITICAL_SECTION csSyncThread_PipeBytes;
	CRITICAL_SECTION csNextESPID;
	CRITICAL_SECTION csStatusbar;
	CRITICAL_SECTION csH264VideoChart;
	CRITICAL_SECTION csGPSSerial;

	PEITCHANNELDATA pChannelData[MAX_EIT_CHANNEL_DATA];
	PEITEVENT pEvents[MAX_EIT_CHANNEL_DATA];
	HTREEITEM hEITTreeItem[MAX_EIT_CHANNEL_DATA];
	
	SYSTEMTIME stEPGSaveCurrentDate;
	SYSTEMTIME stStreamMonitorFile;
	SYSTEMTIME stGPS;
	SYSTEMTIME stLastGPSSample;

	BAT bat[MAX_BAT_ENTRIES];
	PAT pat;
	PMT editpmt;
	CAT cat;
	MMT mmt[MAX_MMT_ENTRIES];
	CDT cdt[MAX_CDT_ENTRIES];
	SIT sit[MAX_SIT_ENTRIES];
	TDT tdt[MAX_TDT_ENTRIES];
	MGT mgt[MAX_MGT_ENTRIES];
	PNITENTRY pNITData[MAX_TS_ENTRIES];
	DVBTDT dvbtdt;
	DVBTOT dvbtot;
	PRRT prrt[256];
	PMTLISTEN pmtlisten[MAX_PAT_ENTRIES];
	STREAMMONITORLOG * sml;
	XMLLOG * XMLLog;

	IPPIDENTRY ippid[MAX_RECORD_BUFFERS];
	PIPENTRY pLastClickedIPEntry;
	PIPMACENTRY pLastClickedIPMACEntry;
	PIPMACENTRY pLastClickedIPPIDEntry;

	LOGFONT logfontChannelFont;
	SIZE textsizeChannelFont;

	PMEMORYUSAGE memu;
	TABLEMONITOR tablemonitor[256];

	EITCONNECTION EITConnection[MAX_EIT_CONNECTIONS];
	SOCKET EITBaseSocket;
	CRITICAL_SECTION csEITConnection;

	SATELLITE sats[MAX_SATELLITES];
	TERRESTRIALMUX tmuxes[MAX_TERRESTRIAL_MUXES];
	CABLEMUX cmuxes[MAX_CABLE_MUXES];
	ATSCMUX amuxes[MAX_TERRESTRIAL_MUXES];
	CVCT cvct[MAX_CVCT_ENTRIES];

	SOURCEMODULES sourcemodules[MAX_SOURCE_MODULES];
	SOURCESTRUCT ss;

	SWITCHPARAMETERS sp[MAX_SWITCH_PARAMETERS];

	EPGGRIDVARIABLES epg;

	STREAMMONITOR sm[MONITOR_COUNT];

	RECORDTABLES record_tables[MAX_RECORD_TABLES];

	ESPARSERINFO esparserinfo[REAL_MAX_ES_PARSERS];

	FWD fwd;
	int nPacketSizeFlag;
#define FORWARDER_PACKET_188 0
#define FORWARDER_PACKET_204 1
#define FORWARDER_PACKET_204_RS 2
	int nPacketOptions;
#define FORWARDER_PACKET_INTERLEAVE 1
#define FORWARDER_PACKET_RANDOMIZER 2
#define FORWARDER_PACKET_SYNCINVERT 4

	csakey key;
	BYTE cwkey[16];

	BYTE bSIBuffer[MAX_SECTION_BUFFERS][65536];
	BYTE pNewPicture[640 * 480 * 3];
	BYTE bTableMonitorBuffer[65535];
	BYTE out_pat[188];
	BYTE out_pmt[20 * 188];
	BYTE out_sdt[188];
	BYTE bDescriptorTagArray[MAX_DESCRIPTOR_TAG_ARRAY][256];
	BYTE bAdvancedDropPID[8192];

	char szSIFormatBuffer[1 * 1024 * 1024];
	char szHTMInitialDir[MAX_PATH];
	char szXMLInitialDir[MAX_PATH];
	char szOutputPIDFlags[20];
	char szRecordFile[MAX_PATH];
	char szAutoRecordFile[MAX_PATH];
	char szXMLExportFilename[MAX_PATH];
	char szRecordPIDFolder[MAX_PATH];
	char szManualChannelsInitialDir[MAX_PATH];
	char szRecordTitle[30];
	char szCurrentlySelectedSatellite[48];
	char szDVHSRecordFailureReason[256];
	char szSourceName[128];
	char szSourceModuleDescription[128];
	char szVLCExeLocation[MAX_PATH];
	char szAutoLoadManualChannelFilename[MAX_PATH];
	char szThumbnailBaseFilename[MAX_PATH];
	char szThumbnailInitialDir[MAX_PATH];
	char szVLCConfigDescription[MAX_VLC_CONFIGURATIONS][128];
	char szVLCConfigCommand[MAX_VLC_CONFIGURATIONS][1024];
	char szSplitFormatString[MAX_PATH];
	char szRokuIP[MAX_PATH];
	char szRokuUsername[128];
	char szRokuPassword[128];
	char szRokuMpegPSPlayLocation[MAX_PATH];
	char szSerialReceiverPort[32];
	char szVLCCommand[2048];
	char szIPSaveFolder[MAX_PATH];
	char szDVBTBandplan[64];
	char szDVBCBandplan[64];
	char szNewCommandLine[128];
	char szSDXDefaultFolder[MAX_PATH];
	char szDVBCSVFile[MAX_PATH];
	char szDCIICSVFile[MAX_PATH];
	char szAutoRecordPIDsFile[MAX_PATH];
	char szProfileName[MAX_PATH];
	char szBaseArchiveFolder[MAX_PATH];
	char szEPGSaveFolder[MAX_PATH];
	char szArchiveVLCCommand[MAX_PATH];
	char szAutoLoadPIDListFilename[MAX_PATH];
	char szCopyProfileName[MAX_PATH];
	char szNewProfileSource[MAX_PATH];
	char szNewProfileName[MAX_PATH];
	char szSchedulerUsername[MAX_PATH];
	char szSchedulerPassword[MAX_PATH];
	char szSchedulerDirectory[MAX_PATH];
	char szSerialReceiverType[128];
	char szSaveChartDataFolder[MAX_PATH];
	char szSaveStreamLogFolder[MAX_PATH];
	char szActualRecordFile[MAX_PATH];
	char szEmailDestination[MAX_PATH];
	char szSMTPServer[MAX_PATH];
	char szEmailFrom[MAX_PATH];
	char szSMTPUsername[MAX_PATH];
	char szSMTPPassword[MAX_PATH];
	char szEITLanguageFilterLanguage[4];
	char szRecordTablesFolder[MAX_PATH];
	char szExportNITINIFolder[MAX_PATH];
	char szNewMuxName[64];
	char szStatusTextMain[256];
	char szStatusTextSecondary[256];
	char szHTTPContentType[256];
	char szVersionBuffer[64];
	char szGPSSerialPort[64];
	char szGPSLogFile[MAX_PATH];
	char szRecordCCLogFolder[MAX_PATH];
	char szGPSLastSignal[64];
	char szGPSLineBuffer[256];

	BYTE tempDescriptorBuffer[MAX_EIT_EXTRA_DESCRIPTORS][1024];

	DWORD channel_maps[65536][32];		// sorted by LCN contains LOWORD: service_number HIWORD: epg_id (Sky only)
	WORD epg_map[65535];				// sorted by EPG ID - contains service_number

	WORD wChartMenuItems[32];

	BYTE bSafety[1 * 1024 * 1024];
} VARIABLES, *PVARIABLES;

typedef struct _tagMPEIPPACKET
{
	// MPEG-2 section
	int table_id, section_length;
	int section_syntax_indicator, private_indicator;

	// MPE header
	int current_next_indicator, section_number, last_section_number;
	int LLC_SNAP_flag, payload_scrambling_control, address_scrambling_control;
	BYTE MAC_Address[6];

	// IPv4 Header
	int IPHeader_Version, IPHeader_IHL, IPHeader_TOS;
	int IPHeader_TotalLength, IPHeader_Identification, IPHeader_Flags;
	int IPHeader_FragmentOffset, IPHeader_TTL, IPHeader_Protocol;
	int IPHeader_HeaderChecksum;
	DWORD IPHeader_SourceAddress, IPHeader_DestinationAddress;

	// IPv6 Header
	int IPv6Header_TrafficClass, IPv6Header_FlowLabel;
	int IPv6Header_PayloadLength, IPv6Header_NextHeader, IPv6Header_HopLimit;
	BYTE IPv6Header_SourceAddress[16];
	BYTE IPv6Header_DestinationAddress[16];

} MPEIPPACKET, *PMPEIPPACKET;

typedef struct tagTSIDAssignments
{
	int nNTSCTSID;
	int nNTSCChannel;
	int nATSCChannel;
	char szLocale[64];
} TSIDASSIGNMENTS;

// Routines exported from TSReader_SourceHelper
#ifdef  __cplusplus
extern "C" {
#endif

void __cdecl SourceHelper_Init(PVARIABLES pv);
void __cdecl SourceHelper_CheckContinuity(BYTE * pBuffer, int nLength);
int __cdecl  SourceHelper_ReadLine(HANDLE hFile, char * szBuffer, int nMaxLength);
BOOL __cdecl SourceHelper_InitSerialControl();
BOOL __cdecl SourceHelper_DeInitSerialControl();
BOOL __cdecl SourceHelper_SetChannelSerialControl(int nSID, int nTSID, int nNID);
void __cdecl SourceHelper_ResetFirstTimeFlag();
BOOL __cdecl SourceHelper_DiSEqCPositionerDialog(HWND hDlg);
void __cdecl SourceHelper_ImportSatelliteList(HWND hDlg, int nImportFormat);
void __cdecl SourceHelper_CalculateSwitchParameters(int nFrequency, int nPolarity, int nDiSEqCInput, int * nLOF, BOOL * f22KHz, int * nVoltage);

BOOL __cdecl TSReader_SendDiSEqC(BYTE * bCommand, int nLength);

BOOL __cdecl SourceHelper_CRC_Check(BYTE * pU8section, DWORD U16length);
void __cdecl SourceHelper_CRC_Init();
DWORD __cdecl SourceHelper_CRC_Calc(BYTE * pU8section, DWORD U16length);

void __cdecl SourceHelper_GetTSReaderEXEDirectory(HINSTANCE hInstance, char * szCurrentDir, int nCurrentDirLength);

#ifdef  __cplusplus
}
#endif

typedef int (* td_UDPSender_GetDevices) (int nIndex, char * szName, char * szDescription);
typedef int (* td_UDPSender_OpenDevice) (char * szName);
typedef int (* td_UDPSender_CloseDevice) ();
typedef int (* td_UDPSender_SendPacket) (BYTE * pData, int nLength);
int (*UDPSender_GetDevices) (int nIndex, char * szName, char * szDescription);
int (*UDPSender_OpenDevice) (char * szName);
int (*UDPSender_CloseDevice) ();
int (*UDPSender_SendPacket) (BYTE * pData, int nLength);

typedef BOOL (* td_Init) (PSOURCESTRUCT pss);
typedef BOOL (* td_DeInit) ();
typedef BOOL (* td_Start) ();
typedef BOOL (* td_Stop) ();
typedef BOOL (* td_TuneDialog) (HWND hWnd);
typedef BOOL (* td_Tune) ();
typedef BOOL (* td_GetDescription) (char * szDescription, char * szCommandLineParameters, BOOL * fCanBeStopped, int * nMaxPIDs, DWORD * dwCapabilities);
typedef BOOL (* td_ParseCommandLine) (PSOURCESTRUCT pss, char * szCommandLine, BOOL fQuiet);
typedef BOOL (* td_PIDManagement) (BOOL fAdd, int nPID, BOOL fTemporary);
typedef BOOL (* td_IsPIDActive) (int nPID);
typedef BOOL (* td_GetTunerString) (char * szString);
typedef BOOL (* td_GetSignalString) (char * szString);
typedef int  (* td_GetSyncLossCount) (BOOL fReset);
typedef int  (* td_GetRetuneCount) (BOOL fReset);
typedef BOOL (* td_SendDiSEqC) (BYTE * bCommand, int nLength);



//////
struct AC3frmsize
{
	int bit_rate;
	int frm_size[3];
};

// H.264 parser stuff

//Appendix E. Table E-1 ­ Meaning of sample aspect ratio indicator
#define SAR_Unspecified  0           // Unspecified
#define SAR_1_1        1             //  1:1
#define SAR_12_11      2             // 12:11
#define SAR_10_11      3             // 10:11
#define SAR_16_11      4             // 16:11
#define SAR_40_33      5             // 40:33
#define SAR_24_11      6             // 24:11
#define SAR_20_11      7             // 20:11
#define SAR_32_11      8             // 32:11
#define SAR_80_33      9             // 80:33
#define SAR_18_11     10             // 18:11
#define SAR_15_11     11             // 15:11
#define SAR_64_33     12             // 64:33
#define SAR_160_99    13             // 160:99
		                             // 14..254           Reserved
#define SAR_Extended      255        // Extended_SAR

#define SH_SLICE_TYPE_P        0        // P (P slice)
#define SH_SLICE_TYPE_B        1        // B (B slice)
#define SH_SLICE_TYPE_I        2        // I (I slice)
#define SH_SLICE_TYPE_SP       3        // SP (SP slice)
#define SH_SLICE_TYPE_SI       4        // SI (SI slice)

#define MPEG2_PICTURE_I			1
#define MPEG2_PICTURE_P			2
#define MPEG2_PICTURE_B			3

typedef struct
{
	uint8_t* start;
	uint8_t* p;
	uint8_t* end;
	int bits_left;
} bs_t;

typedef struct
{
	int profile_idc;
	int constraint_set0_flag;
	int constraint_set1_flag;
	int constraint_set2_flag;
	int constraint_set3_flag;
	int reserved_zero_4bits;
	int level_idc;
	int seq_parameter_set_id;
	int chroma_format_idc;
	int residual_colour_transform_flag;
	int bit_depth_luma_minus8;
	int bit_depth_chroma_minus8;
	int qpprime_y_zero_transform_bypass_flag;
	int seq_scaling_matrix_present_flag;
	  int seq_scaling_list_present_flag[8];
	  int* ScalingList4x4[6];
	  int UseDefaultScalingMatrix4x4Flag[6];
	  int* ScalingList8x8[2];
	  int UseDefaultScalingMatrix8x8Flag[2];
	int log2_max_frame_num_minus4;
	int pic_order_cnt_type;
	  int log2_max_pic_order_cnt_lsb_minus4;
	  int delta_pic_order_always_zero_flag;
	  int offset_for_non_ref_pic;
	  int offset_for_top_to_bottom_field;
	  int num_ref_frames_in_pic_order_cnt_cycle;
	  int offset_for_ref_frame[256];
	int num_ref_frames;
	int gaps_in_frame_num_value_allowed_flag;
	int pic_width_in_mbs_minus1;
	int pic_height_in_map_units_minus1;
	int frame_mbs_only_flag;
	int mb_adaptive_frame_field_flag;
	int direct_8x8_inference_flag;
	int frame_cropping_flag;
	  int frame_crop_left_offset;
	  int frame_crop_right_offset;
	  int frame_crop_top_offset;
	  int frame_crop_bottom_offset;
	int vui_parameters_present_flag;
	
	struct
	{
		int aspect_ratio_info_present_flag;
		  int aspect_ratio_idc;
		    int sar_width;
		    int sar_height;
		int overscan_info_present_flag;
		  int overscan_appropriate_flag;
		int video_signal_type_present_flag;
		  int video_format;
		  int video_full_range_flag;
		  int colour_description_present_flag;
		    int colour_primaries;
			int transfer_characteristics;
			int matrix_coefficients;
		int chroma_loc_info_present_flag;
		  int chroma_sample_loc_type_top_field;
		  int chroma_sample_loc_type_bottom_field;
		int timing_info_present_flag;
		  int num_units_in_tick;
		  int time_scale;
		  int fixed_frame_rate_flag;
		int nal_hrd_parameters_present_flag;
		int vcl_hrd_parameters_present_flag;
		  int low_delay_hrd_flag;
		int pic_struct_present_flag;
		int bitstream_restriction_flag;
		  int motion_vectors_over_pic_boundaries_flag;
		  int max_bytes_per_pic_denom;
		  int max_bits_per_mb_denom;
		  int log2_max_mv_length_horizontal;
		  int log2_max_mv_length_vertical;
		  int num_reorder_frames;
		  int max_dec_frame_buffering;
	} vui;

	struct
	{
		int cpb_cnt_minus1;
		int bit_rate_scale;
		int cpb_size_scale;
		  int bit_rate_value_minus1[32]; // up to cpb_cnt_minus1, which is <= 31
		  int cpb_size_value_minus1[32];
		  int cbr_flag[32];
		int initial_cpb_removal_delay_length_minus1;
		int cpb_removal_delay_length_minus1;
		int dpb_output_delay_length_minus1;
		int time_offset_length;
	} hrd;

} sps_t;

char * GetTSRVersion(char * szBuffer);