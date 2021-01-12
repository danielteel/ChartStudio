#include "stdafx.h"
#include "DoubleBuff.h"
#include "Canvas.h"

bool Canvas::SetupComplete = false;
const char* Canvas::ClassName = "CanvasClass";

Canvas::Canvas(HWND parent, int x, int y, int w, int h) {
	if (Canvas::SetupComplete == false) {
		Canvas::SetupComplete = true;
		WNDCLASSEXA wcex;
		wcex.cbSize = sizeof(WNDCLASSEXA);
		wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = Canvas::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandleA(NULL);
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = Canvas::ClassName;
		wcex.hIconSm = NULL;
		RegisterClassExA(&wcex);
	}

	this->hwnd = CreateWindowExA(0, Canvas::ClassName, "", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, x, y, w, 20, parent, NULL, GetModuleHandleA(NULL), 0);
	SetWindowLongPtr(this->hwnd, GWLP_USERDATA, (LONG)this);

	this->db = new DoubleBuff(this->hwnd);
	this->db->clear(RGB(200, 190, 190));
	this->db->flip();
}


Canvas::~Canvas() {
	delete this->db;
	this->db = nullptr;
	DestroyWindow(this->hwnd);
	this->hwnd = NULL;
}

void Canvas::move(int x, int y, int w, int h) {
	MoveWindow(this->hwnd, x, y, w, h, true);
}

POINT Canvas::getMousePoint(HWND hwnd) {
	POINT xy;
	GetCursorPos(&xy);
	ScreenToClient(hwnd, &xy);
	return xy;
}

LRESULT CALLBACK Canvas::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	Canvas* thisCanvas = reinterpret_cast<Canvas*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	switch (message) {
	case WM_LBUTTONDOWN:
		if (thisCanvas) {
			if (thisCanvas->leftDown) thisCanvas->leftDown(Canvas::getMousePoint(hWnd));
		}
		break;
	case WM_LBUTTONUP:
		if (thisCanvas) {
			if (thisCanvas->leftUp) thisCanvas->leftUp(Canvas::getMousePoint(hWnd));
		}
		break;
	case WM_RBUTTONDOWN:
		if (thisCanvas) {
			if (thisCanvas->rightDown) thisCanvas->rightDown(Canvas::getMousePoint(hWnd));
		}
		break;
	case WM_RBUTTONUP:
		if (thisCanvas) {
			if (thisCanvas->rightUp) thisCanvas->rightUp(Canvas::getMousePoint(hWnd));
		}
		break;
	case WM_MOUSEMOVE:
		if (thisCanvas) {
			if (wParam&MK_LBUTTON) {
				if (thisCanvas->leftMove) thisCanvas->leftMove(Canvas::getMousePoint(hWnd));
			}
			if (wParam&MK_RBUTTON) {
				if (thisCanvas->rightMove) thisCanvas->rightMove(Canvas::getMousePoint(hWnd));
			}
			if (!(wParam&MK_LBUTTON) && !(wParam&MK_RBUTTON)) {
				if (thisCanvas->mouseMove) thisCanvas->mouseMove(Canvas::getMousePoint(hWnd));
			}
		}
		break;

	case WM_MOUSEWHEEL:
		if (thisCanvas) {
			int amount = ((short int)HIWORD(wParam)) / 120;
			if (thisCanvas->mouseWheel) thisCanvas->mouseWheel(Canvas::getMousePoint(hWnd), amount);
		}
		break;

	case WM_PAINT:
		if (thisCanvas){
			if (IsWindowVisible(hWnd)) {
				if (thisCanvas->beforePaint) {
					thisCanvas->beforePaint(thisCanvas);
				}
				if (thisCanvas->db) {
					thisCanvas->db->flip();
				}
			}
			ValidateRect(hWnd, nullptr);
		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int Canvas::width() {
	RECT rect;
	GetClientRect(hwnd, &rect);
	return rect.right - rect.left;
}

int Canvas::height() {
	RECT rect;
	GetClientRect(hwnd, &rect);
	return rect.bottom - rect.top;
}
