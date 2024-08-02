#include <windows.h>
#include <stdint.h>
#include <xinput.h>

#define  local_persist static
#define  global_variable static
#define  internal static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;



struct win32_offscreen_buffer
{
	// NOTE: Pixels are always 32-bits wide, Memory order BB GG RR XX
	BITMAPINFO Info;
	void *Memory;
	int Width;
	int Height;
	int Pitch;
};

struct win32_window_dimension
{
	int Width;
	int Height;
};

// TODO: This is a global for now.
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

// NOTE: XInputGetState 
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserInput, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return (0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

// NOTE: XInputSetState 
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return (0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void Win32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(XInputLibrary)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");

	}
}

internal win32_window_dimension Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;

	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right  - ClientRect.left;
	Result.Height = ClientRect.top = ClientRect.bottom;

	return(Result);
}


internal void RenderWeirdGradient(win32_offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
	//TODO: Let's see what the optimizer do.
	uint8 *Row = (uint8 *)Buffer->Memory; 
	for(int Y = 0; Y < Buffer->Height; ++Y)
	{
		uint32 *Pixel = (uint32 *)Row;
		for(int X = 0; X < Buffer->Width; ++X)
		{
			/*
			   Pixel in memory: xx RR GG BB
			*/
			uint8 Blue = (uint8)(X + BlueOffset);
			uint8 Green = (uint8)(Y + GreenOffset);
			*Pixel++ = ((Green << 8) | Blue);
		}
		Row += Buffer->Pitch;
	}
}

//device independent bitmap
internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	// TODO: Bulletproof this
	// Maybe dont't free first. free after, then free first if that fails.
	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}

	Buffer->Width = Width;
	Buffer->Height = Height;
	int BytesPerPixcel = 4;

	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	// height it negative to make bitmap origin top-left
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixcel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	
	// TODO: Probably clear to black;
	Buffer->Pitch = Width*BytesPerPixcel;
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext, int WindowWidth, int WindowHeight, win32_offscreen_buffer *Buffer, int X, int Y)
{	
	// TODO: Aspect ratio correction.
	// TODO: Play with stretch modes.
	StretchDIBits(DeviceContext, 0, 0, WindowWidth, WindowHeight, 0, 0, Buffer->Width, Buffer->Height, Buffer->Memory, &Buffer->Info, DIB_RGB_COLORS, SRCCOPY);	
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
		case WM_DESTROY:
			{
				GlobalRunning = false;
				OutputDebugStringA("WM_DESTROY\n");
			} break;
		case WM_CLOSE:
			{
				// TODO: Handle this with a message to the user?
				GlobalRunning = false;
				// PostQuitMessage(0);
				// OutputDebugStringA("WM_CLOSE\n");
			} break;
		case WM_ACTIVATEAPP:
			{
				OutputDebugStringA("WM_ACTIVATEAPP\n");
			} break;
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			uint32 VKCode = WParam;
			bool WasDown = ((LParam & (1 << 30)) != 0);
			bool IsDown = ((LParam & (1 << 31)) == 0);

			if(VKCode == 'W')
			{
				OutputDebugStringA("W pressed");
			}
			else if(VKCode == 'A')
			{
			}
			else if(VKCode == 'S')
			{
			}
			else if(VKCode == 'D')
			{
			}
			else if(VKCode == 'Q')
			{
			}
			else if(VKCode == 'E')
			{
			}
			else if(VKCode == VK_UP)
			{
			}
			else if(VKCode == VK_DOWN)
			{
			}
			else if(VKCode == VK_LEFT)
			{
			}
			else if(VKCode == VK_RIGHT)
			{
			}
			else if(VKCode == VK_ESCAPE)
			{
			}
			else if(VKCode == VK_SPACE)
			{

			}

		} break;
		case WM_PAINT:
			{
				PAINTSTRUCT Paint;
				HDC DeviceContext =  BeginPaint(Window, &Paint);
				int X = Paint.rcPaint.left;
				int Y = Paint.rcPaint.top;
				int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
				int Width = Paint.rcPaint.right - Paint.rcPaint.left;

				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, &GlobalBackbuffer, X, Y);
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
	Win32LoadXInput();
	WNDCLASSA WindowClass = {};

	Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);
	// Tells windows to paint the whole window on resize instead of just the needed part.
	WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	if(RegisterClass(&WindowClass))
	{
		HWND Window = 
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
		if(Window)
		{
			// NOTE: Since we specified CS_OWNDC, we can just
			// get one device context and use it forever because
			// we are not sharing it with anyone.
			HDC DeviceContext = GetDC(Window);
			int BlueOffset = 0;
			int GreenOffset = 0;
			GlobalRunning = true;	
			while(GlobalRunning)
			{
				MSG Message;
				// BOOL MessageResult = PeekMessageA(&Message, 0, 0, 0, PM_REMOVE);
				while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
				{
					if(Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				
				// TODO: Should we poll this more frequently
				for(DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex)
				{
					XINPUT_STATE ControllerState;
					if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
					{
						// NOTE: The controller is plugged in.
						// TODO: See if ControllerState.dwPacketNumber increments too rapidly.
						XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
						bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
						bool Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
						bool LeftShoulder = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RightShoulder = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
						bool AButton = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool BButtin = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool XButton = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool YButtin = (Pad->wButtons & XINPUT_GAMEPAD_Y);

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;

					}
					else 
					{
						// NOTE: The controller is not available
					}

				}

//				XINPUT_VIBRATION Vibration;
//				Vibration.wLeftMotorSpeed = 60000;
//				Vibration.wRightMotorSpeed = 60000;
//				XInputSetState(0, &Vibration);

				RenderWeirdGradient(&GlobalBackbuffer, BlueOffset, GreenOffset);

				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, &GlobalBackbuffer,0, 0); 

				++BlueOffset;
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
