#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
// Access to std::cout and std::endl
#include <iostream>
#include <fstream>
#include <windows.h>
#include <stdlib.h>

// DECLDIR will perform an export for us
#define DLL_EXPORT
#define BUFSIZE 512

// Include our header, must come after #define DLL_EXPORT
#include "ReadDirectoryChangesW.h"

// Get rid of name mangeling
extern "C"
{
	BOOL runthread;
	char* ACTIONS[] = { "", "Created", "Deleted", "Updated", "Renamed from something", "Renamed to something" }; // File actions

	typedef struct overlapped_struct
	{
		OVERLAPPED overlapped;
		wchar_t* buffer;
	} overlapped_t;

	typedef struct dirinfo_struct
	{
		HANDLE hDirOPPort;
		HANDLE hDirFH;
		overlapped_t* o;
		int len_buffer;
		wchar_t* buffer;
		wchar_t* directory_name;
		ULONG_PTR CompletionKey;
	} dirinfo_t;

	int somekey = 1;

	//--------------------------------------------------------------------------------------
	void dirinfo_init(dirinfo_t* t)
	{
		t->buffer = (wchar_t*)malloc(16777216 * sizeof(wchar_t));
		t->len_buffer = 16777216;
		t->o = (overlapped_t*)calloc(1, sizeof(overlapped_t));
		t->o->buffer = (wchar_t*)calloc(16777216, sizeof(wchar_t));
		memset(t->o->buffer, 0, 16777216);
		memset(t->o, 0, sizeof(OVERLAPPED));
	}

	//--------------------------------------------------------------------------------------
	void dirinfo_free(dirinfo_t* t)
	{
		free(t->buffer);
		free(t->o->buffer);
		free(t->o);
		free(t);
	}

	DWORD WINAPI WaitingThread(void* args)
	{
		DWORD errorcode = 0;    // an error code
		BOOL bResultQ = FALSE;  // obvios=us
		BOOL bResultR = FALSE;
		DWORD NumBytes = 0;
		FILE_NOTIFY_INFORMATION* pInfo = NULL; // the data incoming is a pointer
											   // to this struct.
		int i = 0;
		dirinfo_t* d = (dirinfo_t*)args;      // rescue struct from thread arg.

		while (runthread)
		{
			bResultR = ReadDirectoryChangesW(d->hDirFH, (void*)d->buffer,
				16777216, TRUE,
				FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME |
				FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE |
				FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS |
				FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY,
				NULL,
				&d->o->overlapped,
				NULL);
			bResultQ = GetQueuedCompletionStatus(d->hDirOPPort,
				&NumBytes, &(d->CompletionKey),
				(LPOVERLAPPED*)(d->o), 1000);
			if (bResultQ && bResultR)
			{

				pInfo = (FILE_NOTIFY_INFORMATION*)d->buffer;
				pInfo->FileName[pInfo->FileNameLength / 2] = '\0';
				std::ofstream outfile("new.txt", std::ofstream::app);
				// write to outfile
				char *FileName = new char[256 + 1];
				memset(FileName, 0, 256 + 1);
				wcstombs(FileName, pInfo->FileName, 256);
				outfile << FileName << " " << ACTIONS[pInfo->Action] << std::endl;
				outfile.close();
				memset(d->buffer, 0, 16777216);
			}
			else
			{
				Sleep(500);
			}
		}

		return 0;
	}

	//-----------------------------------------------------------------------
	// Define our 2 functions
	// Add will return the sum of two numbers
	DECLDIR void Files_Change(char* TrapPath)
	{
		//-----------------------------------------------------------------------
		// Convert char * to LPWSTR
		//-----------------------------------------------------------------------
		wchar_t dir_path[20];
		mbstowcs(dir_path, TrapPath, strlen(TrapPath) + 1);//Plus null
		LPWSTR ptr = dir_path;
		//-----------------------------------------------------------------------
		dirinfo_t* d = (dirinfo_t*)malloc(sizeof(dirinfo_t));
		d->CompletionKey = (ULONG_PTR)&somekey;
		dirinfo_init(d);

		/* set up */
		runthread = TRUE;
		d->hDirFH = CreateFile(dir_path,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
			NULL);

		d->hDirOPPort = CreateIoCompletionPort(d->hDirFH, NULL,
			(ULONG_PTR)d->CompletionKey, 1);
		DWORD   dwThreadId;

		HANDLE  hThread = CreateThread(
			NULL,                   // default security attributes
			0,                      // use default stack size  
			WaitingThread,          // thread function name
			d,                      // argument to thread function 
			0,                      // use default creation flags 
			&dwThreadId);          // returns the thread identifier 
								   // Wait until all threads have terminated.
		WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);

		// Close all thread handles and free memory allocations.
		CloseHandle(hThread);
	}
}