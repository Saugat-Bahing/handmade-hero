#include <windows.h>


//typedef struct tagWNDCLASSA {
//  UINT      style; --> set of binary flags. Prop of window UINT -> unsigned int 32bit
//  WNDPROC   lpfnWndProc; -> pointer to a function that we define how window handle events
//  int       cbClsExtra; -> custom bytes
//  int       cbWndExtra; -> custom memory
//  HINSTANCE hInstance; -> which instance is handling the window -> hInstance | get from kernal getModuleHandle(0)
//  HICON     hIcon; -> game icon
//  HCURSOR   hCursor; -> windows cursor
//  HBRUSH    hbrBackground; -> background color
//  LPCSTR    lpszMenuName; -> menubar
//  LPCSTR    lpszClassName; -> name for windowclass
//} WNDCLASSA, *PWNDCLASSA, *NPWNDCLASSA, *LPWNDCLASSA;

// HWND -> handle to a window
// Message -> window messages

LRESULT CALLBACK MainWindowCallback(
  HWND Window,
  UINT Message,
  WPARAM WParam,
  LPARAM LParam
)
{
	LRESULT Result = 0;
	switch(Message)
	{
		case WM_SIZE:
			{
				OutputDebugStringA("WM_SIZE\n");
			} break;
		case WM_DESTROY:
			{
				OutputDebugStringA("WM_DESTROY\n");
			} break;
		case WM_CLOSE:
			{
				OutputDebugStringA("WM_CLOSE\n");
			} break;
		case WM_ACTIVATEAPP:
			{
				OutputDebugStringA("WM_ACTIVATEAPP\n");
			} break;
		case WM_PAINT:
			{
				PAINTSTRUCT Paint;
				HDC DeviceContext =  BeginPaint(Window, &Paint);
				int X = Paint.rcPaint.left;
				int Y = Paint.rcPaint.top;
				int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
				int Width = Paint.rcPaint.right - Paint.rcPaint.left;
				static DWORD Operation = WHITENESS;
				PatBlt(DeviceContext, X, Y, Width, Height, Operation);
				if(Operation == WHITENESS){
					Operation = BLACKNESS;
				}
				else {
					Operation = WHITENESS;
				}
				EndPaint(Window, &Paint);

			} break;
		default:
			{
				Result = DefWindowProc(Window, Message, WParam, LParam);
			} break;
	}
	return(Result);
}

int CALLBACK 
WinMain(HINSTANCE Instance,
		HINSTANCE PrevInstance,
		LPSTR CommandLine,
		int ShowCode)
{
	// MessageBox(0, "This is handmade hero.", "HandmadeHero", MB_OK|MB_ICONINFORMATION);
	WNDCLASS WindowClass = {};
	// TODO: Check if CS_HREDRAW CS_VREDRAW is really needed.
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	if(RegisterClass(&WindowClass))
	{
		HWND WindowHandle = 
			CreateWindowExA(
					0,
					WindowClass.lpszClassName,
					"HandmadeHero",
					WS_OVERLAPPEDWINDOW|WS_VISIBLE,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					CW_USEDEFAULT,
					0,
					0,
					Instance,
					0);
		if(WindowHandle)
		{
			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
				}
			}
		}
		else 
		{
			// TODO: Logging
		}
	}
	else 
	{
		// TODO: Logging
	};
	return 0;
}
