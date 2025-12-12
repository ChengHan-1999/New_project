#pragma once
#define NOMINMAX
#include<Windows.h>
#include<iostream>

#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
class Window
{
public:
	HWND hwnd;
	HINSTANCE hinstance;
	std::string name;
	int width;  //这是窗口的宽度
	int height;
	bool keys[256];
	int mousex;
	int mousey;
	bool mouseButtons[3];
	bool useMouseClip;
	float invZoom;
	int mouseWheel;
	/*Window(HINSTANCE hInst, const char* className,const char* windowTitle,int width,int height,WNDPROC WndProc);*/
	

	void updateMouse(int x, int y)
	{
		mousex = x;
		mousey = y;
	}
	void processMessages();
	void create(int window_width, int window_height, const std::string window_name, float zoom = 1.0f, bool window_fullscreen = false, int window_x = 0, int window_y = 0);
	void checkInput()
	{
		if (useMouseClip)
		{
			clipMouseToWindow();
		}
		processMessages();
	}
	bool keyPressed(int key)
	{
		return keys[key];
	}
	int getMouseInWindowX()
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		RECT rect;
		GetClientRect(hwnd, &rect);
		p.x = p.x - rect.left;
		p.x = p.x * invZoom;
		return p.x;
	}
	void initmouse()
	{
		RECT rc;
		GetClientRect(hwnd, &rc);   // rc 是 client 区，左上(0,0)，右下(width,height)

		POINT center;
		center.x = (rc.left + rc.right) / 2;
		center.y = (rc.top + rc.bottom) / 2;
		ClientToScreen(hwnd, &center);  //这里的center是已经经过转化的位置，不再表示窗口,而是表示屏幕坐标
		SetCursorPos(center.x, center.y);  //把鼠标设置到窗口中心位置
	}
	int getMouseInWindowY()
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(hwnd, &p);
		RECT rect;
		GetClientRect(hwnd, &rect);
		p.y = p.y - rect.top;
		p.y = p.y * invZoom;
		return p.y;
	}
	void clipMouseToWindow()
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		POINT ul;
		ul.x = rect.left;
		ul.y = rect.top;
		POINT lr;
		lr.x = rect.right;
		lr.y = rect.bottom;
		MapWindowPoints(hwnd, nullptr, &ul, 1);
		MapWindowPoints(hwnd, nullptr, &lr, 1);
		rect.left = ul.x;
		rect.top = ul.y;
		rect.right = lr.x;
		rect.bottom = lr.y;
		ClipCursor(&rect);
	}
	~Window()
	{
		ShowCursor(true);
		ClipCursor(NULL);
	}
};

