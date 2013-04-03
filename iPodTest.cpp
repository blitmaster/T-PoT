// iPodTest.cpp : Defines the entry point for the console application.
//

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#pragma warning(disable:4996)

#include <stdio.h>
#include <tchar.h>
#include <afxwin.h>         // MFC core and standard components

#include "MobileDevice.h"

typedef enum { IPOD_STATE_UNCONNECTED = 0, IPOD_STATE_CONNECTED, IPOD_STATE_READY } t_iPodState;

// State variables
volatile t_iPodState iPodState = IPOD_STATE_UNCONNECTED;
volatile struct am_device *iPodDev = NULL;
CFStringRef iPodAFCName;
struct afc_connection *iPodAFC;
struct afc_connection *iPodConnection;

// Dll dynamic loading data
char *piTunesMobileDevicePath = NULL;
HINSTANCE iTunesDll = NULL;

// Dll routines
tf_AMDeviceNotificationSubscribe	AMDeviceNotificationSubscribe;
tf_AMDeviceConnect				AMDeviceConnect;
tf_AMDeviceDisconnect			AMDeviceDisconnect;
tf_AMDeviceIsPaired				AMDeviceIsPaired;
tf_AMDeviceValidatePairing		AMDeviceValidatePairing;
tf_AMDeviceStartSession			AMDeviceStartSession;
tf_AMDeviceStartService			AMDeviceStartService;
tf_AMDeviceStopSession			AMDeviceStopSession;
tf_AFCConnectionOpen				AFCConnectionOpen;
tf_AFCDeviceInfoOpen				AFCDeviceInfoOpen;
tf_AFCDirectoryOpen				AFCDirectoryOpen;
tf_AFCDirectoryRead				AFCDirectoryRead;
tf_AFCDirectoryClose				AFCDirectoryClose;
tf_AFCFileInfoOpen				AFCFileInfoOpen;
tf_AFCKeyValueRead				AFCKeyValueRead;
tf_AFCKeyValueClose				AFCKeyValueClose;
tf_AFCFileRefOpen				AFCFileRefOpen;
tf_AFCFileRefClose				AFCFileRefClose;
tf_AFCFileRefRead				AFCFileRefRead;
tf_AFCFileRefWrite				AFCFileRefWrite;
tf_AFCRemovePath					AFCRemovePath;
tf_AFCDirectoryCreate			AFCDirectoryCreate;
tf_AFCRenamePath					AFCRenamePath;
tf_AFCGetFileInfo					AFCGetFileInfo;

void init()
{
	HKEY hSetting = NULL;
	DWORD length = 0;
	CString path;
	int pos;

	// Adds iTunesMobileDevice.dll folder to the path, from the registry:
	if (::RegCreateKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Apple Inc.\\Apple Mobile Device Support\\Shared"), &hSetting) != ERROR_SUCCESS)
		throw "iTunesMobileDevice library not found";
	if (::RegQueryValueEx(hSetting, _T("iTunesMobileDeviceDLL"), NULL, NULL, NULL, &length) != ERROR_SUCCESS)
		throw "iTunesMobileDevice library not found";
	piTunesMobileDevicePath = new char[length+1];
	::RegQueryValueEx(hSetting, _T("iTunesMobileDeviceDLL"), NULL, NULL, (LPBYTE)piTunesMobileDevicePath, &length);

	// Adds the folder to the current system path:	
	path.GetEnvironmentVariable("PATH");
	path = (path + ";") + piTunesMobileDevicePath;
	pos = path.ReverseFind('\\');
	if (pos >= 0)
		path = path.Left(pos);
	SetEnvironmentVariable("PATH", path);

	// Loads the DLL routines
	iTunesDll = LoadLibrary(piTunesMobileDevicePath);
	//iTunesDll = LoadLibrary("C:\\Program Files\\Common Files\\Apple\\Mobile Device Support\\bin\\MobileDevice.dll");
	if (iTunesDll) {
		AMDeviceNotificationSubscribe = (tf_AMDeviceNotificationSubscribe)GetProcAddress(iTunesDll, "AMDeviceNotificationSubscribe");
		AMDeviceConnect = (tf_AMDeviceConnect)GetProcAddress(iTunesDll, "AMDeviceConnect");
		AMDeviceDisconnect = (tf_AMDeviceDisconnect)GetProcAddress(iTunesDll, "AMDeviceDisconnect");
		AMDeviceIsPaired = (tf_AMDeviceIsPaired)GetProcAddress(iTunesDll, "AMDeviceIsPaired");
		AMDeviceValidatePairing = (tf_AMDeviceValidatePairing)GetProcAddress(iTunesDll, "AMDeviceValidatePairing");
		AMDeviceStartSession = (tf_AMDeviceStartSession)GetProcAddress(iTunesDll, "AMDeviceStartSession");
		AMDeviceStartService = (tf_AMDeviceStartService)GetProcAddress(iTunesDll, "AMDeviceStartService");
		AMDeviceStopSession = (tf_AMDeviceStopSession)GetProcAddress(iTunesDll, "AMDeviceStopSession");
		AFCConnectionOpen = (tf_AFCConnectionOpen)GetProcAddress(iTunesDll, "AFCConnectionOpen");
		AFCDeviceInfoOpen = (tf_AFCDeviceInfoOpen)GetProcAddress(iTunesDll, "AFCDeviceInfoOpen");
		AFCDirectoryOpen = (tf_AFCDirectoryOpen)GetProcAddress(iTunesDll, "AFCDirectoryOpen");
		AFCDirectoryRead = (tf_AFCDirectoryRead)GetProcAddress(iTunesDll, "AFCDirectoryRead");
		AFCDirectoryClose = (tf_AFCDirectoryClose)GetProcAddress(iTunesDll, "AFCDirectoryClose");
		AFCFileInfoOpen = (tf_AFCFileInfoOpen)GetProcAddress(iTunesDll, "AFCFileInfoOpen");
		AFCKeyValueRead = (tf_AFCKeyValueRead)GetProcAddress(iTunesDll, "AFCKeyValueRead");
		AFCKeyValueClose = (tf_AFCKeyValueClose)GetProcAddress(iTunesDll, "AFCKeyValueClose");
		AFCFileRefOpen = (tf_AFCFileRefOpen)GetProcAddress(iTunesDll, "AFCFileRefOpen");
		AFCFileRefClose = (tf_AFCFileRefClose)GetProcAddress(iTunesDll, "AFCFileRefClose");
		AFCFileRefRead = (tf_AFCFileRefRead)GetProcAddress(iTunesDll, "AFCFileRefRead");
		AFCFileRefWrite = (tf_AFCFileRefWrite)GetProcAddress(iTunesDll, "AFCFileRefWrite");
		AFCRemovePath = (tf_AFCRemovePath)GetProcAddress(iTunesDll, "AFCRemovePath");
		AFCDirectoryCreate = (tf_AFCDirectoryCreate)GetProcAddress(iTunesDll, "AFCDirectoryCreate");
		AFCRenamePath = (tf_AFCRenamePath)GetProcAddress(iTunesDll, "AFCRenamePath");
		AFCGetFileInfo = (tf_AFCGetFileInfo)GetProcAddress(iTunesDll, "AFCGetFileInfo");
	} else
		throw "iTunesMobileDevice.dll could not be loaded";
}

void notification(struct am_device_notification_callback_info *info)
{
	unsigned int msg = info->msg;
	
	//  Need more verbosity here.
	printf("[NOTIF] %d\n", msg);
	switch (msg) {
		case ADNCI_MSG_CONNECTED:
			if (iPodState == IPOD_STATE_UNCONNECTED) {
				iPodDev = info->dev;
				iPodState = IPOD_STATE_CONNECTED;
				printf("iPod is connected\n");
			} else {
				// Was in another state before, something bad must have happened
			}
			break;
		case ADNCI_MSG_DISCONNECTED:
			if (iPodState == IPOD_STATE_CONNECTED) {
				iPodState = IPOD_STATE_UNCONNECTED;
				iPodDev = NULL;
				iPodAFC = NULL;
				iPodConnection = NULL;
				printf("iPod is disconnected\n");
			}
			break;
		default:
			break;
	}
}

void connect()
{
	struct am_device_notification *notif; 
	mach_error_t ret;
	int timeout;
	struct am_device *pDev;
	
	printf("Trying to connect iPod...\n");
	ret = AMDeviceNotificationSubscribe(notification, 0, 0, 0, &notif);
	printf("[RET]AMDeviceNotificationSubscribe() = %d\n", ret);
	for (timeout = 0; (timeout < 1000) && (iPodState != IPOD_STATE_CONNECTED); timeout++) {
		Sleep(10);
	}
	if (iPodState != IPOD_STATE_CONNECTED)
		throw "Could not find iPod";
	pDev = (struct am_device *)iPodDev;
	// This part could possibly move to the notification routine:
	ret = AMDeviceConnect(pDev);
	printf("[RET]AMDeviceConnect() = %d\n", ret);
	if (ret) {
		// We don't handle the restore mode
		throw "Could not connect iPod";
	}
	ret = AMDeviceIsPaired(pDev);
	printf("[RET]AMDeviceIsPaired() = %d\n", ret);
	if (!ret)
		throw "Could not pair iPod";
	ret = AMDeviceValidatePairing(pDev);
	printf("[RET]AMDeviceValidatePairing() = %d\n", ret);
	if (ret)
		throw "Could not validate iPod pairing";
	ret = AMDeviceStartSession(pDev);
	printf("[RET]AMDeviceStartSession() = %d\n", ret);
	if (ret)
		throw "Could not start session";
	iPodAFCName = AMSVC_AFC2;
	ret = AMDeviceStartService(pDev, iPodAFCName, &iPodAFC, NULL);
	printf("[RET]AMDeviceStartService() = %d\n", ret);
	if (ret) {
		// Not jailbroken, tries to connect to the standard name
		iPodAFCName = AMSVC_AFC;
		ret = AMDeviceStartService(pDev, iPodAFCName, &iPodAFC, NULL);
		printf("[RET]AMDeviceStartService() = %d\n", ret);
		if (ret)
			throw "Could not start AFC service";
	}
	ret = AFCConnectionOpen(iPodAFC, 0, &iPodConnection);
	printf("[RET]AFCConnectionOpen() = %d\n", ret);
	if (ret)
		throw "Could not start AFC connection";
	printf("Ready\n");
	iPodState = IPOD_STATE_READY;
}

void close()
{
	if (piTunesMobileDevicePath)
		delete piTunesMobileDevicePath;
	if (iTunesDll)
		FreeLibrary(iTunesDll);
	if (iPodState == IPOD_STATE_READY) {
		/* Doesn't work:
		mach_error_t ret;
		int timeout;
		ret = AMDeviceStopSession((struct am_device *)iPodDev);
		printf("[RET]AMDeviceStopSession() = %d\n", ret);
		ret = AMDeviceDisconnect((struct am_device *)iPodDev);
		printf("[RET]AMDeviceDisconnect() = %d\n", ret);
		for (timeout = 0; (timeout < 500) && (iPodState != IPOD_STATE_UNCONNECTED); timeout++) {
			Sleep(10);
		}
		if (iPodState != IPOD_STATE_UNCONNECTED)
			throw "Could not disconnect iPod";
		*/
	}
}

int iPodCmdFileInfo(char *remoteFile)
{
	mach_error_t ret;
	struct afc_dictionary *pInfo;
	char *pKey, *pVal;
	unsigned int size = 0;
	
	//ret = AFCGetFileInfo(iPodConnection, remoteFile, &pInfo, &size);
	//printf("[RET]AFCGetFileInfo() = %d\n[RES]\tpInfo=%08X size=%08X\n", ret, (int)pInfo, size);

	ret = AFCFileInfoOpen(iPodConnection, remoteFile, &pInfo);
	printf("[RET]AFCFileInfoOpen() = %d\n", ret);
	if (ret) {
		printf("%s doesn't exist\n", remoteFile);
		return ret;
	}
	ret = AFCKeyValueRead(pInfo, &pKey, &pVal);
	while(pKey || pVal) {
		printf("[RES]\t%s = %s\n", (pKey ? pKey : "<empty>"), (pVal ? pVal : "<empty>"));
		AFCKeyValueRead(pInfo, &pKey, &pVal);
	}
	AFCKeyValueClose(pInfo);
	return 0;
}

int iPodCmdLs(char *remotePath)
{
	struct afc_directory *pDir;
	char *pEntry;
	mach_error_t ret;
	CString filename;
	
	ret = AFCDirectoryOpen(iPodConnection, remotePath, &pDir);
	printf("[RET]AFCDirectoryOpen() = %d\n", ret);
	if (ret) {
		printf("%s doesn't exist\n", remotePath);
		return ret;
	}
	while(1) {
		ret = AFCDirectoryRead(iPodConnection, pDir, &pEntry);
		if (ret) {
			printf("[RET]AFCDirectoryRead() = %d\n", ret);
			break;
		}
		if (!pEntry)
			break;
		printf("[RES] %s\n", pEntry);
		/*
		filename = remotePath;
		if (filename.Right(1).Compare("/"))
			filename += "/";
		filename += pEntry;
		iPodCmdFileInfo(filename.GetBuffer());
		*/
	}
	ret = AFCDirectoryClose(iPodConnection, pDir);
	printf("[RET]AFCDirectoryClose() = %d\n", ret);
	return ret;
}

int iPodCmdFileRead(char *remoteFile)
{
	mach_error_t ret;
	struct afc_dictionary *pInfo;
	char *pKey, *pVal;
	unsigned int size = 0, total = 0, len;
	afc_file_ref handle;
	unsigned char buffer[16];
	unsigned int i;

	// Gets the file size
	ret = AFCFileInfoOpen(iPodConnection, remoteFile, &pInfo);
	if (ret) {
		printf("%s doesn't exist\n", remoteFile);
		return ret;
	}
	ret = AFCKeyValueRead(pInfo, &pKey, &pVal);
	while(pKey || pVal) {
		printf("\t%s = %s\n", pKey, pVal);
		if (pKey == NULL || pVal == NULL)
			break;
		if (!stricmp(pKey, "st_size")) {
			size = atoi(pVal);
			break;
		}
		AFCKeyValueRead(pInfo, &pKey, &pVal);
	}
	AFCKeyValueClose(pInfo);
	if (size == 0) {
		printf("%s has a null size\n", remoteFile);
		return 1;
	}

	// Opens the file for reading
	ret = AFCFileRefOpen(iPodConnection, remoteFile, AFC_FILEMODE_READ, 0, &handle);
	if (ret != MDERR_OK) {
		printf("[RET]AFCFileRefOpen() = %d\n", ret);
		return ret;
	}
	
	// Gets the contents by chunks
	while (total < size) {
		len = min(size - total, sizeof(buffer));
		ret = AFCFileRefRead(iPodConnection, handle, buffer, &len);
		if (ret != MDERR_OK) {
			printf("[RET]AFCFileRefRead() = %d\n", ret);
			break;
		}
		if (!len)
			break;
		for (i = 0; i < len; i++)
			printf("%02X ", buffer[i]);
		printf("%*s| ", (sizeof(buffer)-len)*3+1, " ");
		for (i = 0; i < len; i++)
			printf("%c", buffer[i] >= 32 ? buffer[i] : ' ');
		putchar('\n');
		total += len;
	}
	AFCFileRefClose(iPodConnection, handle);
	return 0;
}

void test()
{
	char buffer[512];
	while(1) {
		printf("> ");
		gets(buffer);
		if (!*buffer)
			break;
		if (buffer[0] == '?') {
			printf("File info on %s:\n", buffer+1);
			iPodCmdFileInfo(buffer+1);
		} else if (!strncmp(buffer, "r ", 2)) {
			printf("Reading file %s:\n", buffer+2);
			iPodCmdFileRead(buffer+2);
		} else {
			printf("Directory contents of %s:\n", buffer);
			iPodCmdLs(buffer);
		}
	}
}

void main(int argc, char *argv[])
{
	try {
		init();
		connect();
		test();
		close();
	}
	catch(char *message) {
		printf("Error: %s\nAborting.\n", message);
		close();
		exit(1);
	}
}
