// win32.cpp : Defines the entry point for the application.
//
#include <windows.h>
#include "ucos_ii.h"

#define LCD_X		320
#define LCD_Y		480
#define LCD_LEFT	0
#define LCD_TOP		0

#define IDC_TIMER_REFRESHLCD	0x200
#define DLY_REFRESHLCD			50

// Global Variables:
static HANDLE		winhandle;
	   HANDLE		ucoshandle;
static DWORD		WinThrdID;								//
static DWORD		UcosThrdID;								//
static TCHAR szTitle[] = TEXT("PC_SIM");
static TCHAR szWindowClass[] = TEXT("Windows");

extern void 		*GetFrameBufferAddr();

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
HWND				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

TIMERPROC			RefreshLcd(HWND, UINT, WPARAM, LPARAM);
void				WorkThrd(LPVOID);

HWND MainWindowCreate(void)
{
 	// TODO: Place code here.
	HWND hWnd;
	MSG msg;

	// Initialize global strings
	MyRegisterClass(NULL);

	// Perform application initialization:
	hWnd = InitInstance (NULL, SW_SHOW);
	if (hWnd == NULL) 
		return FALSE;

	// Main message loop:
	while (GetMessage(&msg, hWnd, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return hWnd;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

   hWnd = CreateWindow(szWindowClass, szTitle, 
	   WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
	   CW_USEDEFAULT, CW_USEDEFAULT, LCD_X+4, LCD_Y+25, 
	   HWND_DESKTOP, NULL, 
	   NULL, NULL);
   
   if (!hWnd)
	   return NULL;

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return hWnd;
}

extern int ftk_add_event(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch (message) 
	{
	case WM_CREATE:
		{
			SetTimer(hWnd, IDC_TIMER_REFRESHLCD, DLY_REFRESHLCD, (TIMERPROC)RefreshLcd);
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
		{
			ftk_add_event(hWnd, message, wParam, lParam);
			break;
		}
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code here...
			EndPaint(hWnd, &ps);

		}
		break;
	case WM_DESTROY:
		{
			ftk_add_event(hWnd, message, wParam, lParam);
			KillTimer(hWnd, IDC_TIMER_REFRESHLCD);
			PostQuitMessage(0);
			ReleaseSemaphore(winhandle, 1, NULL); // increase count by one
		}
		break;
	default:                      /* for messages that we don't deal with */
		return DefWindowProc (hWnd, message, wParam, lParam);
	}

	return 0;
}

TIMERPROC RefreshLcd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	INT x, y;
	INT mx, my;
	HDC hDC, hMemDC;
	HBITMAP hBmp;
	BITMAPINFO bi;
	BYTE *fbuf = NULL;
	BYTE LcdLineBuf[LCD_X*3];

	if((fbuf = GetFrameBufferAddr()) == NULL)
		return 0;

	memset(&bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biWidth = LCD_X;
	bi.bmiHeader.biHeight = LCD_Y;
	
	hDC = GetDC(hWnd);
	hMemDC = CreateCompatibleDC(hDC);
	hBmp = CreateCompatibleBitmap(hDC, LCD_X, LCD_Y);
	SelectObject(hMemDC, hBmp);

	for(y=0, my=0; y<LCD_Y; y++, my)
	{
		for(x=0, mx=0; x<LCD_X*3; x+=3, mx+=3)
		{
			LcdLineBuf[x+0] = fbuf[my + mx + 0];
			LcdLineBuf[x+1] = fbuf[my + mx + 1];;
			LcdLineBuf[x+2] = fbuf[my + mx + 2];;
		}
		my += LCD_X * 3;
		SetDIBits(hMemDC, hBmp, LCD_Y-y-1, 1, LcdLineBuf, &bi, DIB_RGB_COLORS);
	}
	BitBlt(hDC, LCD_LEFT, LCD_TOP, LCD_X, LCD_Y, hMemDC, 0, 0, SRCCOPY);

	DeleteObject(hBmp);
	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hDC);

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void WinWorkThrd(LPVOID lpParam)
{
	HWND hWnd;

	Sleep(10);
	hWnd = MainWindowCreate();
}

extern void C_AppMain(void);
void UcosWorkThrd(LPVOID lpParam)
{
	lpParam = lpParam;

	C_AppMain();
}



int main(int argc, char *argv[])
{
	HANDLE handles[2];
	DWORD dwEvent;

	ucoshandle = CreateSemaphore( 
		NULL,	// default security attributes
		0,		// initial count
		1,		// maximum count
		NULL);  // unnamed semaphore
	if (ucoshandle == NULL) 
	{ 
		printf("CreateEvent error: %d\n", GetLastError() ); 
		ExitProcess(0); 
	} 

	winhandle = CreateSemaphore( 
		NULL,	// default security attributes
		0,		// initial count
		1,		// maximum count
		NULL);  // unnamed semaphore
	if (winhandle == NULL) 
	{ 
		printf("CreateEvent error: %d\n", GetLastError() ); 
		ExitProcess(0); 
	} 

	/* first run ,must be*/
	CreateThread(NULL, 0, (void*)UcosWorkThrd, NULL, 0, &UcosThrdID);
	CreateThread(NULL, 0, (void*)WinWorkThrd, NULL, 0, &WinThrdID);
	Sleep(15);

	handles[0] = winhandle;
	handles[1] = ucoshandle;
	dwEvent = WaitForMultipleObjects( 
		2,           // number of objects in array
		handles,     // array of objects
		FALSE,       // wait for any
		INFINITE);   // indefinite wait

	switch (dwEvent) 
	{ 
	case WAIT_OBJECT_0 + 0:
	case WAIT_OBJECT_0 + 1:
		CloseHandle(winhandle);
		CloseHandle(ucoshandle);
		break;
	default: 
		printf("Wait error: %d\n", GetLastError()); 
		ExitProcess(0);
		break;
	}

	return 0;
}
