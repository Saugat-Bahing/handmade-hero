#include <windows.h>

#define  local_persist static
#define  global_variable static
#define  internal static

// TODO: This is a global for now.
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;

//device independent bitmap
internal void
Win32ResizeDIBSection(int Width, int Height)
{
	// TODO: Bulletproof this
	// Maybe dont't free first. free after, then free first if that fails.
	if(BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;

	int BytesPerPixcel = 4;
	int BitmapMemorySize = (Width*Height)*BytesPerPixcel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
	StretchDIBits(DeviceContext, X, Y, Width, Height, X, Y, Width, Height, BitmapMemory, &BitmapInfo, DIB_RGB_COLORS, SRCCOPY);	
}


LRESULT CALLBACK 
Win32MainWindowCallback(
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
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int Width = ClientRect.right  - ClientRect.left;
				int Height = ClientRect.top = ClientRect.bottom;
				Win32ResizeDIBSection(Width, Height);
			} break;
		case WM_DESTROY:
			{
				Running = false;
				OutputDebugStringA("WM_DESTROY\n");
			} break;
		case WM_CLOSE:
			{
				// TODO: Handle this with a message to the user?
				Running = false;
				// PostQuitMessage(0);
				// OutputDebugStringA("WM_CLOSE\n");
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
				Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
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
	WNDCLASSA WindowClass = {};
	// TODO: Check if CS_HREDRAW CS_VREDRAW is really needed.
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
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
			Running = true;	
			while(Running)
			{
				BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
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
