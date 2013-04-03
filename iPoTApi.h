//=============================================================================
// T-PoT - Total Commander file system plug-in for iPod and iPhone devices
//-----------------------------------------------------------------------------
// File:			iPoTApi.h
// Purpose:			iPod/iPhone devices USB file interface API class.
// Limitations:		Based on undocumented features of iTunes.
//					Tested with iTunes 7.4 and iPod firmware 1.1.1 / 1.1.2.
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

#ifndef IPOTAPI_H
#define IPOTAPI_H

#include "MobileDevice.h"

// Styles used in CoreFoundation routines
#ifdef WIN32
	#define kPathStyle		kCFURLWindowsPathStyle
	#define kStrEncoding	kCFStringEncodingWindowsLatin1
#else
	#define kPathStyle		kCFURLPOSIXPathStyle
	#define kStrEncoding	kCFStringEncodingUTF8
#endif

int PngConv(char *srcFilename, char *dstFilename);

typedef mach_error_t t_MachError;
typedef struct am_device t_AMDevice;
typedef struct am_device_notification_callback_info t_AMDeviceNotificationInfo;
typedef struct afc_connection t_AFCConnection;
typedef struct am_device_notification t_AMDeviceNotification;
typedef struct afc_directory t_AFCDirectory;

typedef int (__stdcall *tf_Progress)(
	char* SourceName,
    char* TargetName,
    int PercentDone
);

typedef struct {
	WIN32_FIND_DATA findData;
	t_AFCDirectory *pHandle;
	char *remotePath;
	bool appFolder;
} t_iPodFileInfo;

typedef enum {
	IPOD_IFIFO = 0,
	IPOD_IFCHR,
	IPOD_IFDIR,
	IPOD_IFREG,
	IPOD_IFBLK,
	IPOD_IFSOCK,
	IPOD_IFLNK,
	IPOD_IFBLNK		// Broken link
} t_iPodFileType;

typedef struct {
	t_iPodFileType iType;
	unsigned int iSize;
	unsigned int iBlocks;
	unsigned int iLinks;
	char LinkTarget[MAX_PATH];
} t_iPodExtraInfo;

typedef enum {
	IPOD_ERR_OK				= 0,
	IPOD_ERR_DLL_NOT_FOUND	= 1,
	IPOD_IPOD_NOT_FOUND		= 2,
	IPOD_COULD_NOT_CONNECT	= 3,
} t_iPodError;

typedef enum {
	IPOD_STATE_NO_DLL = 0,		// DLL could not be loaded by Attach()
	IPOD_STATE_UNCONNECTED,		// DLL loaded but iPod not connected
	IPOD_STATE_CONNECTED		// iPod connected
} t_iPodState;

typedef enum {
	API_STATE_UNAVAILABLE = 0,	// No session
	API_STATE_AVAILABLE			// Session open
} t_ApiState;


class CiPoTApi {

	// ------------------------------------------------------------------------
	// Static part
	// ------------------------------------------------------------------------
	private:
		// Global device status and handle
		static volatile t_iPodState m_iPodState;
		static volatile t_AMDevice *m_iPodDev;
		static volatile unsigned int m_GlobalConnectionID;
		static bool bCanTranslatePLIST;

		// Dll dynamic loading data
		static char *m_piTunesMobileDevicePath;
		static HINSTANCE m_iTunesDll;
		static char *m_pCoreFoundationPath;
		static HINSTANCE m_CoreFoundationDll;

		// Dll routines
		static tf_AMDeviceNotificationSubscribe	            AMDeviceNotificationSubscribe;
		static tf_AMDeviceConnect				            AMDeviceConnect;
		static tf_AMDeviceDisconnect			            AMDeviceDisconnect;
		static tf_AMDeviceIsPaired				            AMDeviceIsPaired;
		static tf_AMDeviceValidatePairing		            AMDeviceValidatePairing;
		static tf_AMDeviceStartSession			            AMDeviceStartSession;
		static tf_AMDeviceStartService			            AMDeviceStartService;
		static tf_AMDeviceStopSession			            AMDeviceStopSession;
		static tf_AFCConnectionOpen				            AFCConnectionOpen;
		static tf_AFCDeviceInfoOpen				            AFCDeviceInfoOpen;
		static tf_AFCDirectoryOpen				            AFCDirectoryOpen;
		static tf_AFCDirectoryRead				            AFCDirectoryRead;
		static tf_AFCDirectoryClose				            AFCDirectoryClose;
		static tf_AFCFileInfoOpen				            AFCFileInfoOpen;
		static tf_AFCKeyValueRead				            AFCKeyValueRead;
		static tf_AFCKeyValueClose				            AFCKeyValueClose;
		static tf_AFCFileRefOpen				            AFCFileRefOpen;
		static tf_AFCFileRefClose				            AFCFileRefClose;
		static tf_AFCFileRefRead				            AFCFileRefRead;
		static tf_AFCFileRefWrite				            AFCFileRefWrite;
		static tf_AFCRemovePath					            AFCRemovePath;
		static tf_AFCDirectoryCreate			            AFCDirectoryCreate;
		static tf_AFCRenamePath					            AFCRenamePath;

		static tf_CFWriteStreamCreateWithFile				CFWriteStreamCreateWithFile;
		static tf_CFReadStreamCreateWithFile				CFReadStreamCreateWithFile;
		static tf_CFStringCreateWithCString					CFStringCreateWithCString;
		static tf_CFURLCreateWithFileSystemPath				CFURLCreateWithFileSystemPath;
		static tf_CFReadStreamOpen							CFReadStreamOpen;
		static tf_CFWriteStreamOpen							CFWriteStreamOpen;
		static tf_CFPropertyListCreateFromStream			CFPropertyListCreateFromStream;
		static tf_CFReadStreamClose							CFReadStreamClose;
		static tf_CFPropertyListIsValid						CFPropertyListIsValid;
		static tf_CFPropertyListWriteToStream				CFPropertyListWriteToStream;
		static tf_CFWriteStreamClose						CFWriteStreamClose;
		static tf_CFRelease									CFRelease;
		static tf_CFURLCreateDataAndPropertiesFromResource	CFURLCreateDataAndPropertiesFromResource;
		static tf_CFPropertyListCreateFromXMLData           CFPropertyListCreateFromXMLData;
		static tf_CFPropertyListCreateXMLData               CFPropertyListCreateXMLData;
		static tf_CFURLWriteDataAndPropertiesToResource     CFURLWriteDataAndPropertiesToResource;

	// ------------------------------------------------------------------------

	private:
		static void NotificationHandler(t_AMDeviceNotificationInfo *info);

	public:
		static tf_CFStringMakeConstantString				_CFStringMakeConstantString;
		char m_Serial[64];
		static t_iPodError AttachDLL();			// Should be called before using any instance
		static t_iPodError DetachDLL();			// Should be called on program closure

	// ------------------------------------------------------------------------
	// Non-static part
	// ------------------------------------------------------------------------

	private:

		#define FILEREAD_BUFFER_SIZE	(1<<16)	// transfers split in 64 kB buffer chunks

		t_AMDevice *m_pDev;
		t_ApiState m_ApiState;
		CFStringRef m_iPodAFCName;
		t_AFCConnection *m_iPodAFC;
		t_AFCConnection *m_iPodConnection;
		unsigned int m_ConnectionID;
		tf_Progress m_ProgressCallBack;
		
		void ResetConnection();
		void CheckConnection();
		void GetLinkTargetPath(char *cLinkName, char *cLinkTarget, char *cFullPath);

	public:
		CiPoTApi();
		~CiPoTApi();

		t_ApiState GetApiState();
		t_iPodState GetiPodState() const	{ return m_iPodState; }
		t_iPodError OpenSession();
		t_iPodError CloseSession();
		void SetProgressCallBack(tf_Progress fn);

		t_MachError GetFileInfo(char *remotePath, t_iPodFileInfo *pInfo, t_iPodExtraInfo *pExtra = 0);
		void GetTempFilename(CString &tmpFilename);
		bool FindFirst(char *remotePath, t_iPodFileInfo *pInfo);
		bool FindNext(t_iPodFileInfo *pInfo);
		void FindClose(t_iPodFileInfo *pInfo);
		bool FileExists(char *remotePath);
		int FileRead(char *remotePath, char *localPath);
		int FileWrite(char *remotePath, char *localPath);
		t_MachError Move(char *oldPath, char *newPath);
		t_MachError Remove(char *remotePath);
		t_MachError MakeDir(char *remotePath);
		bool TranslatePNG(char *originalFile, char *newFile);
		bool CanTranslatePLIST() const { return bCanTranslatePLIST; };
		bool TranslatePLIST(char *originalFile, char *newFile);

	// ------------------------------------------------------------------------

	public:

};

#endif
