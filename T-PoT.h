//=============================================================================
// T-PoT - Total Commander file system plug-in for iPod and iPhone devices
//-----------------------------------------------------------------------------
// File:			T-PoT.h
// Purpose:			Main API implementation for Total Commander plug-in.
// Limitations:		Limitations of iTunes interface with iPod/iPhone
//					devices (no file attributes).
// Platform:		Win32
//-----------------------------------------------------------------------------
// Copyright (c) 2007-2009, Scythal
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the distribution.
// * Neither the name of the software nor the names of its contributors may be 
//   used to endorse or promote products derived from this software without 
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY ITS AUTHOR ``AS IS'' AND ANY EXPRESS OR 
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
// NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//=============================================================================

#ifndef TPOT_h
#define TPOT_h

#ifdef __cplusplus
extern "C" {
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TPOT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TPOT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TPOT_EXPORTS
#define TPOT_API __declspec(dllexport)
#else
#define TPOT_API __declspec(dllimport)
#endif

#define CTPOT_API extern "C" TPOT_API

// Root name in Total Commander
#define FSPLUGIN_CAPTION "T-PoT"
#define FSPLUGIN_VERSION MAKELONG(1, 3)
#define FSPLUGIN_SUBVERSION MAKELONG(1, 2)

// ids for FsGetFile
#define FS_FILE_OK 0
#define FS_FILE_EXISTS 1
#define FS_FILE_NOTFOUND 2
#define FS_FILE_READERROR 3
#define FS_FILE_WRITEERROR 4
#define FS_FILE_USERABORT 5
#define FS_FILE_NOTSUPPORTED 6
#define FS_FILE_EXISTSRESUMEALLOWED 7

#define FS_EXEC_OK 0
#define FS_EXEC_ERROR 1
#define FS_EXEC_YOURSELF -1
#define FS_EXEC_SYMLINK -2

#define FS_COPYFLAGS_OVERWRITE 1
#define FS_COPYFLAGS_RESUME 2
#define FS_COPYFLAGS_MOVE 4
#define FS_COPYFLAGS_EXISTS_SAMECASE 8
#define FS_COPYFLAGS_EXISTS_DIFFERENTCASE 16
 
// flags for tRequestProc
#define RT_Other 0
#define RT_UserName 1
#define RT_Password 2
#define RT_Account 3
#define RT_UserNameFirewall 4
#define RT_PasswordFirewall 5
#define RT_TargetDir 6
#define RT_URL 7
#define RT_MsgOK 8
#define RT_MsgYesNo 9
#define RT_MsgOKCancel 10

// flags for tLogProc
#define MSGTYPE_CONNECT 1
#define MSGTYPE_DISCONNECT 2
#define MSGTYPE_DETAILS 3
#define MSGTYPE_TRANSFERCOMPLETE 4
#define MSGTYPE_CONNECTCOMPLETE 5
#define MSGTYPE_IMPORTANTERROR 6
#define MSGTYPE_OPERATIONCOMPLETE 7

// flags for FsStatusInfo
#define FS_STATUS_START 0
#define FS_STATUS_END 1

#define FS_STATUS_OP_LIST 1
#define FS_STATUS_OP_GET_SINGLE 2
#define FS_STATUS_OP_GET_MULTI 3
#define FS_STATUS_OP_PUT_SINGLE 4
#define FS_STATUS_OP_PUT_MULTI 5
#define FS_STATUS_OP_RENMOV_SINGLE 6
#define FS_STATUS_OP_RENMOV_MULTI 7
#define FS_STATUS_OP_DELETE 8
#define FS_STATUS_OP_ATTRIB 9
#define FS_STATUS_OP_MKDIR 10
#define FS_STATUS_OP_EXEC 11
#define FS_STATUS_OP_CALCSIZE 12
#define FS_STATUS_OP_SEARCH 13
#define FS_STATUS_OP_SEARCH_TEXT 14
#define FS_STATUS_OP_SYNC_SEARCH 15
#define FS_STATUS_OP_SYNC_GET 16
#define FS_STATUS_OP_SYNC_PUT 17
#define FS_STATUS_OP_SYNC_DELETE 18

#define FS_ICONFLAG_SMALL 1
#define FS_ICONFLAG_BACKGROUND 2

#define FS_ICON_USEDEFAULT 0
#define FS_ICON_EXTRACTED 1
#define FS_ICON_EXTRACTED_DESTROY 2
#define FS_ICON_DELAYED 3
#define FS_BITMAP_NONE 0
#define FS_BITMAP_EXTRACTED 1
#define FS_BITMAP_EXTRACT_YOURSELF 2
#define FS_BITMAP_EXTRACT_YOURSELF_ANDDELETE 3
#define FS_BITMAP_CACHE 256

typedef struct {
	DWORD SizeLow,SizeHigh;
	FILETIME LastWriteTime;
	int Attr;
} RemoteInfoStruct;

typedef struct {
	int size;
	DWORD PluginInterfaceVersionLow;
	DWORD PluginInterfaceVersionHi;
	char DefaultIniName[MAX_PATH];
} FsDefaultParamStruct;

extern int __stdcall Progress(char *sourceName, char *targetName, int percentDone);

// callback functions
typedef int (__stdcall *tProgressProc)(int PluginNr,char *SourceName, char *TargetName, int PercentDone);
typedef void (__stdcall *tLogProc)(int PluginNr, int MsgType, char *LogString);
typedef BOOL (__stdcall *tRequestProc)(int PluginNr, int RequestType, char *CustomTitle,
                                       char *CustomText, char *ReturnedText, int maxlen);

// Function prototypes
void	__stdcall FsGetDefRootName(char *DefRootName, int maxlen);

int		__stdcall FsInit(int PluginNr, tProgressProc pProgressProc, 
                         tLogProc pLogProc, tRequestProc pRequestProc);
HANDLE	__stdcall FsFindFirst(char *Path, WIN32_FIND_DATA *FindData);
BOOL	__stdcall FsFindNext(HANDLE Hdl, WIN32_FIND_DATA *FindData);
int		__stdcall FsFindClose(HANDLE Hdl);
int		__stdcall FsGetFile(char *RemoteName, char *LocalName, int CopyFlags, 
                            RemoteInfoStruct *ri);
int		__stdcall FsPutFile(char *LocalName, char *RemoteName, int CopyFlags);
BOOL	__stdcall FsMkDir(char *Path);
BOOL	__stdcall FsDeleteFile(char *RemoteName);
BOOL	__stdcall FsRemoveDir(char *RemoteName);
int		__stdcall FsRenMovFile(char *OldName, char *NewName, BOOL Move, 
                               BOOL OverWrite, RemoteInfoStruct *ri);
int		__stdcall FsExecuteFile(HWND MainWin, char *RemoteName, char *Verb);
void	__stdcall FsSetDefaultParams(FsDefaultParamStruct *dps);
int		__stdcall FsExtractCustomIcon(char *RemoteName, int ExtractFlags, HICON *TheIcon);

/* NOT IMPLEMENTED:

BOOL	__stdcall FsDisconnect(char *DisconnectRoot);
BOOL	__stdcall FsSetAttr(char *RemoteName, int NewAttr);
BOOL	__stdcall FsSetTime(char *RemoteName, FILETIME *CreationTime, 
                            FILETIME *LastAccessTime, FILETIME *LastWriteTime);
void	__stdcall FsStatusInfo(char *RemoteDir, int InfoStartEnd, int InfoOperation);

int __stdcall FsGetPreviewBitmap(char* RemoteName,int width,int height,HBITMAP* ReturnedBitmap);
BOOL __stdcall FsLinksToLocalFiles(void);
BOOL __stdcall FsGetLocalName(char* RemoteName,int maxlen);

*/

// ************************** content plugin extension ****************************

// 
#define ft_nomorefields 0
#define ft_numeric_32 1
#define ft_numeric_64 2
#define ft_numeric_floating 3
#define ft_date 4

#define ft_time 5
#define ft_boolean 6
#define ft_multiplechoice 7
#define ft_string 8
#define ft_fulltext 9
#define ft_datetime 10

// for FsContentGetValue
#define ft_nosuchfield -1   // error, invalid field number given
#define ft_fileerror -2     // file i/o error
#define ft_fieldempty -3    // field valid, but empty
#define ft_ondemand -4      // field will be retrieved only when user presses <SPACEBAR>
#define ft_delayed 0        // field takes a long time to extract -> try again in background

// for FsContentSetValue
#define ft_setsuccess 0     // setting of the attribute succeeded

// for FsContentGetSupportedFieldFlags
#define contflags_edit 1
#define contflags_substsize 2
#define contflags_substdatetime 4
#define contflags_substdate 6
#define contflags_substtime 8
#define contflags_substattributes 10
#define contflags_substattributestr 12
#define contflags_substmask 14

// for FsContentSetValue
#define setflags_first_attribute 1     // First attribute of this file

#define setflags_last_attribute  2     // Last attribute of this file
#define setflags_only_date       4     // Only set the date of the datetime value!


#define CONTENT_DELAYIFSLOW 1  // ContentGetValue called in foreground

typedef struct {
    int size;
    DWORD PluginInterfaceVersionLow;
    DWORD PluginInterfaceVersionHi;
    char DefaultIniName[MAX_PATH];
} ContentDefaultParamStruct;

typedef struct {
	WORD wYear;
	WORD wMonth;
	WORD wDay;
} tdateformat,*pdateformat;

typedef struct {
	WORD wHour;

WORD wMinute;
	WORD wSecond;
} ttimeformat,*ptimeformat;

/* NOT IMPLEMENTED:

int __stdcall FsContentGetSupportedField(int FieldIndex, char *FieldName, char *Units, int maxlen);
int __stdcall FsContentGetValue(char *FileName, int FieldIndex, int UnitIndex, void *FieldValue, int maxlen, int flags);

void __stdcall FsContentStopGetValue(char *FileName);
int __stdcall FsContentGetDefaultSortOrder(int FieldIndex);
void __stdcall FsContentPluginUnloading(void);
int __stdcall FsContentGetSupportedFieldFlags(int FieldIndex);

int __stdcall FsContentSetValue(char *FileName, int FieldIndex, int UnitIndex, int FieldType, void *FieldValue, int flags);
BOOL __stdcall FsContentGetDefaultView(char *ViewContents, char *ViewHeaders, char *ViewWidths, char *ViewOptions, int maxlen);

*/

#ifdef __cplusplus
}
#endif

#endif
