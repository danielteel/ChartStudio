#include "stdafx.h"
#include "ButtonBar.h"
#include "helpers.h"


vector<HWND> ButtonBar::StaticButtonArray;
bool ButtonBar::SetupComplete = false;

ButtonBar::ButtonBar(HWND parent, int x, int y, int w, int seperatorWidth, COLORREF seperatorColor) {
	this->parent = parent;
	this->x = x;
	this->y = y;
	this->w = w;
	this->h = 0;
	this->seperatorWidth = seperatorWidth;
	this->seperatorColor = seperatorColor;
	this->largestImageHeight = 0;
	
	if (ButtonBar::SetupComplete == false) {
		ButtonBar::SetupComplete = true;
		WNDCLASSEXA wcex;
		wcex.cbSize = sizeof(WNDCLASSEXA);
		wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = ButtonBar::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandleA(NULL);
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = NULL;
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = "ButtonBarBacking";
		wcex.hIconSm = NULL;
		RegisterClassExA(&wcex);
	}
	this->backing = CreateWindowExA(0, "ButtonBarBacking", "", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, x, y, w, 20, parent, NULL, GetModuleHandleA(NULL), 0);
	SetWindowLongPtr(this->backing, GWLP_USERDATA, (LONG)this);
}


ButtonBar::~ButtonBar() {
	for (auto & button : this->buttons) {
		if (button) {
			DestroyWindow(button);
			ButtonBar::removeButtonFromStaticArray(button);
		}
	}
	for (auto & bitmap : this->bitmaps) {
		DeleteObject(bitmap);
	}
	DestroyWindow(this->backing);
}



HWND CreateToolTip(HWND control, const char* text) {
	if (!control || !text) {
		return FALSE;
	}
	HWND parent = GetParent(control);

	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		parent, NULL,
		GetModuleHandleA(NULL), NULL);

	if (!hwndTip) {
		return (HWND)NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { 0 };
	toolInfo.cbSize = sizeof(toolInfo);
	toolInfo.hwnd = parent;
	toolInfo.uFlags = TTF_IDISHWND | TTF_CENTERTIP | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)control;
	toolInfo.lpszText = (LPSTR)text;
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	return hwndTip;
}

LRESULT CALLBACK ButtonBar::Subclassproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (uMsg) {
	case WM_LBUTTONDOWN:
		SetFocus(hWnd);
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	case WM_KEYDOWN:
	case WM_COMMAND:
	case WM_KEYUP:
	case WM_CHAR:
		SetFocus(GetParent(hWnd));
		SendMessage(GetParent(hWnd), uMsg, wParam, lParam);
		return 1;
	default:
		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK ButtonBar::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN:
	case WM_COMMAND:
	case WM_KEYUP:
	case WM_CHAR:
		SetFocus(GetParent(hWnd));
		SendMessage(GetParent(hWnd), message, wParam, lParam);
		return 1;

	case WM_PAINT:
		{
			ButtonBar* thisBar =  reinterpret_cast<ButtonBar*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (thisBar) {
				RECT rect;
				GetClientRect(hWnd, &rect);
				rect.bottom++;
				rect.right++;
				HDC hdc = GetDC(hWnd);
				HBRUSH brush = CreateSolidBrush(thisBar->seperatorColor);
				FillRect(hdc, &rect, brush);
				DeleteObject(brush);
				ReleaseDC(hWnd, hdc);
			}
			ValidateRect(hWnd, nullptr);
		}
		return 0;
	case WM_SIZE:
		{
			ButtonBar* thisBar = reinterpret_cast<ButtonBar*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (thisBar) {
				thisBar->updateButtonSizes();
			}
		}
		break;
	case WM_DRAWITEM:
		return ButtonBar::messageHandler(hWnd, message, wParam, lParam);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void ButtonBar::updateButtonSizes() {
	double x = 0;
	double y = 0;
	if (this->buttons.size() > 0) {
		int buttonsLeft = this->buttons.size() - this->seperators;
		w -= this->seperators*this->seperatorWidth;

		for (auto & button : this->buttons) {
			if (button) {
				if (buttonsLeft > 0) {
					int buttonWidth = w / buttonsLeft;
					MoveWindow(button, int(x), int(y), buttonWidth, this->h + 1, true);
					x += buttonWidth;
					w -= buttonWidth;
					buttonsLeft--;
				}
			} else {
				x += this->seperatorWidth;
			}
		}

	}
}

void ButtonBar::move(int x, int y, int w) {
	MoveWindow(this->backing, x, y, w, this->h, true);
	this->x = x;
	this->y = y;
	this->w = w;
	this->updateButtonSizes();
}

void ButtonBar::addButton(HBITMAP bitmap, int buttonID, const char* toolTipText) {
	DWORD style = BS_CENTER | BS_OWNERDRAW | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	if (bitmap) {
		style = style | BS_BITMAP;
	}

	HINSTANCE hInst = GetModuleHandleA(nullptr);
	HWND button = CreateWindowExA(0, "button", "", style, 0, 0, 10, 10, this->backing, (HMENU)buttonID, GetModuleHandleA(nullptr), 0);
	CreateToolTip(button, toolTipText);
	SetWindowSubclass(button, ButtonBar::Subclassproc, 1, 0);
	if (bitmap) {
		BITMAP bitmapProps;
		GetObject(bitmap, sizeof(BITMAP), &bitmapProps);
		if (bitmapProps.bmHeight > this->largestImageHeight) {
			this->largestImageHeight = bitmapProps.bmHeight;
			this->h = this->largestImageHeight + 3;
		}
		SendMessage(button, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)bitmap);
	}

	this->updateButtonSizes();
	this->buttons.push_back(button);
	this->buttonIDs.push_back(buttonID);
	ButtonBar::addButtonToStaticArray(button);
}

void ButtonBar::addButton(int bitmapResourceID, int buttonID, const char* toolTipText) {
	HINSTANCE hInst = GetModuleHandleA(nullptr);
	HBITMAP bitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(bitmapResourceID), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
	if (bitmap)this->bitmaps.push_back(bitmap);
	this->addButton(bitmap, buttonID, toolTipText);
}

void ButtonBar::addSeperator() {
	this->buttons.push_back(NULL);
	this->buttonIDs.push_back(NULL);
	this->seperators++;
}

int ButtonBar::getHeight() {
	return this->h;
}

void ButtonBar::enableButton(int buttonID, bool enabled) {
	for (size_t i = 0; i < this->buttonIDs.size(); i++) {
		if (this->buttonIDs[i] == buttonID) {
			EnableWindow(this->buttons[i], enabled);
		}
	}
}



int ButtonBar::messageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* pdis = (DRAWITEMSTRUCT*)lParam;
			if (pdis->CtlType == ODT_BUTTON) {
				if (pdis->itemID == -1) {
					break;
				}
				for (auto & currentButton : ButtonBar::StaticButtonArray) {
					if (currentButton == pdis->hwndItem) {
						//draw button
						int x = (pdis->rcItem.right - pdis->rcItem.left) / 2;
						int y = (pdis->rcItem.bottom - pdis->rcItem.top) / 2;
						//DWORD buttonState = 0;

						if (pdis->itemState & ODS_DISABLED) {
							COLORREF buttonFace = GetSysColor(COLOR_BTNFACE);
							COLORREF disabledColor = RGB(GetRValue(buttonFace)-30, GetGValue(buttonFace)-30, GetBValue(buttonFace)-30);
							HBRUSH brush = CreateSolidBrush(disabledColor);
							FillRect(pdis->hDC, &pdis->rcItem, brush);
							//DrawFrameControl(pdis->hDC, &pdis->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_TRANSPARENT | DFCS_FLAT);
							DrawEdge(pdis->hDC, &pdis->rcItem, EDGE_RAISED, BF_RECT);
							DeleteObject(brush);
						} else {

							FillRect(pdis->hDC, &pdis->rcItem, GetSysColorBrush(COLOR_BTNFACE));
							if (pdis->itemState & ODS_SELECTED) {
								//buttonState = DFCS_PUSHED;
								x = x + 1;
								y = y + 1;
								DrawEdge(pdis->hDC, &pdis->rcItem, EDGE_SUNKEN, BF_RECT);
							} else {
								DrawEdge(pdis->hDC, &pdis->rcItem, EDGE_RAISED, BF_RECT);

							}

							//DrawFrameControl(pdis->hDC, &pdis->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | buttonState);

							HBITMAP bitmap = (HBITMAP)SendMessage(currentButton, BM_GETIMAGE, 0, 0);
							if (bitmap) {
								drawBitmap(pdis->hDC, bitmap, x, y, true);
							}
						}

						break;
					}
				}
			}
		}
		return 1;
	}
	return 0;
}

void ButtonBar::addButtonToStaticArray(HWND button) {
	ButtonBar::StaticButtonArray.push_back(button);
}

void ButtonBar::removeButtonFromStaticArray(HWND button) {
	for (auto it = ButtonBar::StaticButtonArray.begin(); it != ButtonBar::StaticButtonArray.end(); it++) {
		if (*it == button) {
			ButtonBar::StaticButtonArray.erase(it);
			break;
		}
	}
}
