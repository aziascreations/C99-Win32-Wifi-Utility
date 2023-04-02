#include <stdio.h>

#include <windows.h>

// Declare global variables
const wchar_t g_szClassName[] = L"myWindowClass";
HWND g_hButton;

// Declare function prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void HelloWorld();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPWSTR lpCmdLine, int nCmdShow) {
	// Declare local variables
	HWND hwnd;
	MSG msg;
	WNDCLASSEXW wc;
	
	// Register the window class
	ZeroMemory(&wc, sizeof(WNDCLASSEXW));
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wc.lpszClassName = g_szClassName;
	RegisterClassExW(&wc);
	
	// Create the window
	hwnd = CreateWindowExW(0, g_szClassName, L"Hello World",
						   WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
						   400, 300, NULL, NULL, hInstance, NULL);
	
	// Create the button
	g_hButton = CreateWindowW(L"BUTTON", L"Click me", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
							  150, 100, 100, 30, hwnd, NULL, hInstance, NULL);
	
	// Show the window
	ShowWindow(hwnd, nCmdShow);
	
	// Enter the message loop
	while(GetMessageW(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	
	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_COMMAND:
			if((HWND) lParam == g_hButton) {
				HelloWorld();
			}
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
	
	return 0;
}

void HelloWorld() {
	MessageBoxW(NULL, L"Hello World!", L"Message", MB_OK);
}
