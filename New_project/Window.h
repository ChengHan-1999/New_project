#pragma once
#define NOMINMAX
#include<Windows.h>
#include<iostream>

#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)LOWORD(lp))
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
class Window
{
public:
	HWND hwnd;
	HINSTANCE hinstance;
	std::string name;
	int WindowWidth;
	int WindowHeight;
	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];
	Window(HINSTANCE hInst, const char* className,const char* windowTitle,int width,int height,WNDPROC WndProc);
	

	void updateMouse(int x, int y)
	{
		mousex = x;
		mousey = y;
	}
	void processMessages();
};

