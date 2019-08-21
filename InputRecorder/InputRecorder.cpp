// InputRecorder.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "InputRecorder.h"
#include "KeystrokeCaptureData.hpp"
#include "FeatureExtractor.hpp"
#include <time.h>
#include "Fingerprint.hpp"

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream

#include <sys/timeb.h>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HWND _hWnd;

KeystrokeCaptureData kcdata;

TCHAR recordToFile[MAX_PATH] = { "" };

Fingerprint _fingerprint_model;

Fingerprint _fingerprint_data;

HANDLE hFile = 0;

void printf_log(LPCTSTR szData, ...)
{

	if (hFile == 0)
	{
		TCHAR my_path[500] = "c:\\inputrec.txt";
	
		hFile = CreateFile(my_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		

		DWORD dwSize = GetFileSize(hFile, NULL);
		if (dwSize > (25 * 100 * 1024)) // Size Checking.
		{
			SetEndOfFile(hFile);
			//CloseHandle(hFile);
			//DeleteFile(my_path);
			//hFile = CreateFile( my_path, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);

		}


	}

	SetFilePointer(hFile, 0, NULL, FILE_END);

	TCHAR Buffer[1000] = { 0 };
	va_list pArgList;

	va_start(pArgList, szData);
	// The last argument to wvsprintf points to the arguments
	_vsntprintf(Buffer, sizeof(Buffer), szData, pArgList);
	va_end(pArgList);

#ifdef _X64
	_tcscat(Buffer, _T(" (x64)"));
#endif

	_tcscat_s(Buffer, 900, _T("\r\n"));

	if (hFile == INVALID_HANDLE_VALUE)
	{
		//printf(Buffer);
		return;
	}

	//CTime currtime = CTime::GetCurrentTime();
	//CString time = currtime.Format("%H:%M:%S  ");

	time_t t = time(NULL);
	tm time2;
	TCHAR timestr[100] = { "00:00:00 " };
	strftime(timestr, 80, "%H:%M:%S  ", localtime( &t));

	DWORD unBytesWritten;
	WriteFile(hFile, (LPCTSTR)timestr, _tcslen(timestr), &unBytesWritten, NULL);
	WriteFile(hFile, Buffer, _tcslen(Buffer), &unBytesWritten, NULL);
	FlushFileBuffers(hFile);

	//CloseHandle(hFile);


}


void print_Score()
{
	FeatureExtractor fe = FeatureExtractor();
	kcdata.feed(fe);
	auto features = fe.extract_features();
	Fingerprint fingerprint = Fingerprint(features);

	double score = fingerprint.canberra_distance(_fingerprint_model);
	double score2 = fingerprint.euclid_distance(_fingerprint_model);
	double score3 = fingerprint.manhattan_distance(_fingerprint_model);
	//ResultViewController *result = [[ResultViewController alloc] initWithResult:fingerprint.canberra_distance(_geniuneFingerprint)];


	printf_log("%d chars, Score = %f %f %f" , kcdata.size(), score, score2, score3);

}






void load_Model(LPCTSTR fileName)
{

	std::ifstream is(fileName);
	KeystrokeCaptureData kscd = KeystrokeCaptureData(is);
	FeatureExtractor fe = FeatureExtractor();
	kscd.feed(fe);
	auto features = fe.extract_features();
	Fingerprint fingerprint(features);

	_fingerprint_model = fingerprint;

	printf_log("load_Model OK: %d chars (from %s)", kscd.size(), fileName);

}


bool load_DataFile(LPCTSTR fileName)
{

	std::ifstream is(fileName);
	if (!is.is_open())
		return false;
	KeystrokeCaptureData kscd = KeystrokeCaptureData(is);
	FeatureExtractor fe = FeatureExtractor();
	kscd.feed(fe);
	auto features = fe.extract_features();
	Fingerprint fingerprint(features);

	_fingerprint_data = fingerprint;

	printf_log("load_DataFile OK: %d chars from %s", kscd.size(), fileName);

	return true;

}

bool play_DataFile(LPCTSTR fileName)
{

	std::ifstream is(fileName);
	if (!is.is_open())
		return false;
	KeystrokeCaptureData kscd = KeystrokeCaptureData(is);
	EventPlayer player = EventPlayer();
	printf_log("play_DataFile...");
	kscd.feed(player);
	
	printf_log("play_DataFile OK: %d chars done!", kscd.size());

	return true;

}

/*
void get_Score()
{
	FeatureExtractor fe = FeatureExtractor();
	kcdata.feed(fe);
	auto features = fe.extract_features();
	Fingerprint fingerprint = Fingerprint(features);

	float score = fingerprint.canberra_distance(_fingerprint_model);

	printf_log("Score %f", score);


}*/

void save_Keylog()
{
	printf_log("save_Keylog %d chars to %s", kcdata.size(), recordToFile);

	std::ofstream is; // ("c:\\file_log.txt", std::ifstream::out || std::ifstream::app);
	is.open( recordToFile);

	kcdata.print_log(is);

	is.close();
}


void RegisterInput() {

	RAWINPUTDEVICE rid[2];
	int nDevs = 0;

	// keyboard
	rid[nDevs].dwFlags = RIDEV_INPUTSINK;// RIDEV_INPUTSINK;	// ignore legacy messages and receive system wide keystrokes	
	rid[nDevs].usUsagePage = 1;							// raw keyboard data only
	rid[nDevs].usUsage = 6; //keyboard
	rid[nDevs].hwndTarget = _hWnd;
	nDevs = 1;
	
	// registere to get WM_INPUT messages
	if (RegisterRawInputDevices(rid, nDevs, sizeof(RAWINPUTDEVICE)))
	{
		printf_log("RegisterInput OK %d", nDevs);
		//InitKbdExt();
	}
	else
	{
		printf_log("RegisterInput ERR %X, devs:%d", GetLastError(), nDevs);
	}

}

/*************************************************************************
 * CommandLineToArgvA            [SHELL32.@]
 *
 * MODIFIED FROM https://www.winehq.org/ project
 * We must interpret the quotes in the command line to rebuild the argv
 * array correctly:
 * - arguments are separated by spaces or tabs
 * - quotes serve as optional argument delimiters
 *   '"a b"'   -> 'a b'
 * - escaped quotes must be converted back to '"'
 *   '\"'      -> '"'
 * - consecutive backslashes preceding a quote see their number halved with
 *   the remainder escaping the quote:
 *   2n   backslashes + quote -> n backslashes + quote as an argument delimiter
 *   2n+1 backslashes + quote -> n backslashes + literal quote
 * - backslashes that are not followed by a quote are copied literally:
 *   'a\b'     -> 'a\b'
 *   'a\\b'    -> 'a\\b'
 * - in quoted strings, consecutive quotes see their number divided by three
 *   with the remainder modulo 3 deciding whether to close the string or not.
 *   Note that the opening quote must be counted in the consecutive quotes,
 *   that's the (1+) below:
 *   (1+) 3n   quotes -> n quotes
 *   (1+) 3n+1 quotes -> n quotes plus closes the quoted string
 *   (1+) 3n+2 quotes -> n+1 quotes plus closes the quoted string
 * - in unquoted strings, the first quote opens the quoted string and the
 *   remaining consecutive quotes follow the above rule.
 */

LPSTR* WINAPI CommandLineToArgvA(LPSTR lpCmdline, int* numargs)
{
	DWORD argc;
	LPSTR  *argv;
	LPSTR s;
	LPSTR d;
	LPSTR cmdline;
	int qcount, bcount;

	if (!numargs || *lpCmdline == 0)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	/* --- First count the arguments */
	argc = 1;
	s = lpCmdline;
	/* The first argument, the executable path, follows special rules */
	if (*s == '"')
	{
		/* The executable path ends at the next quote, no matter what */
		s++;
		while (*s)
			if (*s++ == '"')
				break;
	}
	else
	{
		/* The executable path ends at the next space, no matter what */
		while (*s && *s != ' ' && *s != '\t')
			s++;
	}
	/* skip to the first argument, if any */
	while (*s == ' ' || *s == '\t')
		s++;
	if (*s)
		argc++;

	/* Analyze the remaining arguments */
	qcount = bcount = 0;
	while (*s)
	{
		if ((*s == ' ' || *s == '\t') && qcount == 0)
		{
			/* skip to the next argument and count it if any */
			while (*s == ' ' || *s == '\t')
				s++;
			if (*s)
				argc++;
			bcount = 0;
		}
		else if (*s == '\\')
		{
			/* '\', count them */
			bcount++;
			s++;
		}
		else if (*s == '"')
		{
			/* '"' */
			if ((bcount & 1) == 0)
				qcount++; /* unescaped '"' */
			s++;
			bcount = 0;
			/* consecutive quotes, see comment in copying code below */
			while (*s == '"')
			{
				qcount++;
				s++;
			}
			qcount = qcount % 3;
			if (qcount == 2)
				qcount = 0;
		}
		else
		{
			/* a regular character */
			bcount = 0;
			s++;
		}
	}

	/* Allocate in a single lump, the string array, and the strings that go
	 * with it. This way the caller can make a single LocalFree() call to free
	 * both, as per MSDN.
	 */
	argv = (LPSTR*)LocalAlloc(LMEM_FIXED, (argc + 1) * sizeof(LPSTR) + (strlen(lpCmdline) + 1) * sizeof(char));
	if (!argv)
		return NULL;
	cmdline = (LPSTR)(argv + argc + 1);
	strcpy(cmdline, lpCmdline);

	/* --- Then split and copy the arguments */
	argv[0] = d = cmdline;
	argc = 1;
	/* The first argument, the executable path, follows special rules */
	if (*d == '"')
	{
		/* The executable path ends at the next quote, no matter what */
		s = d + 1;
		while (*s)
		{
			if (*s == '"')
			{
				s++;
				break;
			}
			*d++ = *s++;
		}
	}
	else
	{
		/* The executable path ends at the next space, no matter what */
		while (*d && *d != ' ' && *d != '\t')
			d++;
		s = d;
		if (*s)
			s++;
	}
	/* close the executable path */
	*d++ = 0;
	/* skip to the first argument and initialize it if any */
	while (*s == ' ' || *s == '\t')
		s++;
	if (!*s)
	{
		/* There are no parameters so we are all done */
		argv[argc] = NULL;
		*numargs = argc;
		return argv;
	}

	/* Split and copy the remaining arguments */
	argv[argc++] = d;
	qcount = bcount = 0;
	while (*s)
	{
		if ((*s == ' ' || *s == '\t') && qcount == 0)
		{
			/* close the argument */
			*d++ = 0;
			bcount = 0;

			/* skip to the next one and initialize it if any */
			do {
				s++;
			} while (*s == ' ' || *s == '\t');
			if (*s)
				argv[argc++] = d;
		}
		else if (*s == '\\')
		{
			*d++ = *s++;
			bcount++;
		}
		else if (*s == '"')
		{
			if ((bcount & 1) == 0)
			{
				/* Preceded by an even number of '\', this is half that
				 * number of '\', plus a quote which we erase.
				 */
				d -= bcount / 2;
				qcount++;
			}
			else
			{
				/* Preceded by an odd number of '\', this is half that
				 * number of '\' followed by a '"'
				 */
				d = d - bcount / 2 - 1;
				*d++ = '"';
			}
			s++;
			bcount = 0;
			/* Now count the number of consecutive quotes. Note that qcount
			 * already takes into account the opening quote if any, as well as
			 * the quote that lead us here.
			 */
			while (*s == '"')
			{
				if (++qcount == 3)
				{
					*d++ = '"';
					qcount = 0;
				}
				s++;
			}
			if (qcount == 2)
				qcount = 0;
		}
		else
		{
			/* a regular character */
			*d++ = *s++;
			bcount = 0;
		}
	}
	*d = '\0';
	argv[argc] = NULL;
	*numargs = argc;

	return argv;
}


int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_INPUTRECORDER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

   
	int argc=0;
	LPSTR *argv = CommandLineToArgvA(lpCmdLine, &argc);

	if (argc==1 && argv[0][0] != '-')
	{
		//  play file 
		Sleep(1000);
		play_DataFile(argv[0]);
		return 0;
	}

	if (argc == 2 && argv[0][0] == '-')
	{
		//  save input to data file
		_tcscpy(recordToFile, argv[1]);
	} else
		if (argc == 2 && argv[0][0] != '-')
		{
			//  compare to files
			
			load_Model(argv[0]);
			
			if (load_DataFile(argv[1]))
			{
				double score = _fingerprint_data.canberra_distance(_fingerprint_model);
				double score2 = _fingerprint_data.euclid_distance(_fingerprint_model);
				double score3 = _fingerprint_data.manhattan_distance(_fingerprint_model);
				//ResultViewController *result = [[ResultViewController alloc] initWithResult:fingerprint.canberra_distance(_geniuneFingerprint)];


				printf_log("Compare score  = %f %f %f", score, score2, score3);


				return 0;
			}

		}
		else
		{
			printf_log("Usage");
			MessageBox(0, "Usage: inputRec.exe - file.txt", "How to use", 0);
			return 0;
		}

	printf_log("Recording input to save to %s", recordToFile);


	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}


	
	RegisterInput();
	

	

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_INPUTRECORDER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INPUTRECORDER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_INPUTRECORDER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   _hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!_hWnd)
   {
      return FALSE;
   }

   ShowWindow(_hWnd, nCmdShow);
   UpdateWindow(_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
				
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
		save_Keylog();
		//print_Score();
        PostQuitMessage(0);
        break;

	case WM_INPUT:
	{

		BYTE buff[2000] = { 0 };
		UINT dwSize = 1900;


		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buff, &dwSize, sizeof(RAWINPUTHEADER)))
		{
			PRAWINPUT raw = (PRAWINPUT)buff;
			//UINT Event;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				//if (_LogKeyStrokes)
				//  WM_KEYUP
				// KeyLog_LogRawKey(raw->data.keyboard.Message, raw->data.keyboard.VKey, raw->data.keyboard.MakeCode, 0);

				EventType type;
				time_t timestamp = time(0);

				switch (raw->data.keyboard.Message) {
				case WM_KEYDOWN:
					type = KEY_DOWN;
					break;
				case WM_KEYUP:
					type = KEY_UP;
					break;
					/*case NSFlagsChanged:
						if ([e modifierFlags] & NSShiftKeyMask && keycode == kVK_Shift) {
							type = KEY_DOWN;
						}
						else if (keycode == kVK_Shift) {
							type = KEY_UP;
						}
						break;*/
				default:
					break;
				}

				 __timeb64 timebuffer;
				_ftime64_s(&timebuffer);
				//timebuffer.millitm; //milli seconds
				//timebuffer.time; //the same like struct time_t
				
				//VK_SHIFT

				timestamp = timebuffer.time * 1000 + timebuffer.millitm;
				int keycode = int(raw->data.keyboard.VKey);
				kcdata.on_key(keycode, type, timestamp);

			//	printf_log("Key %c", keycode);
			}

		}
	}

	break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;

		// process raw input. (see RegisterRawInputDevices() )
		// 
	
	}
    
    return (INT_PTR)FALSE;
}
