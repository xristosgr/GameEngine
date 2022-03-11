#pragma once

#include "RenderWindow.h"
#include "InputVariables.h"
#include<memory>

class WindowContainer : virtual public InputVariables
{
public:
	WindowContainer();
	LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	RenderWindow render_window;
};

