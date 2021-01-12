#include "stdafx.h"
#include "Toolbar.h"


Toolbar::Toolbar(HWND hParent, const char* toolbarImage, int bitmapItemWH, int numOfBitmaps) {
	HINSTANCE hInstance = GetModuleHandleW(nullptr);

	this->hParent = hParent;
	DWORD style = TBSTYLE_FLAT | WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_TOOLTIPS | WS_BORDER | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	this->hToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, style,
									0, 0, 0, 0, hParent, NULL, hInstance, NULL);
	SendMessage(this->hToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(this->hToolbar, TB_SETMAXTEXTROWS, 0, 0);

	this->toolbarBitmap = LoadImage(hInstance, toolbarImage, IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
	SendMessage(this->hToolbar, TB_SETBITMAPSIZE, 0, MAKELPARAM(bitmapItemWH, bitmapItemWH));

	TBADDBITMAP addBmp;
	addBmp.hInst = NULL;
	addBmp.nID = (UINT_PTR)this->toolbarBitmap;
	SendMessage(this->hToolbar, TB_ADDBITMAP, numOfBitmaps, (LPARAM)&addBmp);

	SendMessage(hToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(bitmapItemWH+8, bitmapItemWH+8));
	SendMessage(hToolbar, TB_AUTOSIZE, 0, 0);
	ShowWindow(hToolbar, TRUE);

}

int Toolbar::getHeight() {
	RECT tbarRect;
	GetWindowRect(this->hToolbar, &tbarRect);

	return tbarRect.bottom - tbarRect.top;
}

void Toolbar::addButton(int bitmapIndex, int id, const char* tooltipText) {
	TBBUTTON button;
	button.iBitmap = bitmapIndex;
	button.idCommand = id;
	button.fsState = TBSTATE_ENABLED;
	button.fsStyle = TBSTYLE_BUTTON;
	button.dwData = 0;
	button.iString = (INT_PTR)tooltipText;
	SendMessage(this->hToolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);
}

void Toolbar::autosize() {
	SendMessage(this->hToolbar, TB_AUTOSIZE, 0, 0);
}

void Toolbar::addSeperator(int sepWidth) {
	TBBUTTON button;
	button.iBitmap = sepWidth;
	button.idCommand = 0;
	button.fsState = 0;
	button.fsStyle = BTNS_SEP;
	button.dwData = 0;
	button.iString = 0;
	SendMessage(this->hToolbar, TB_ADDBUTTONS, 1, (LPARAM)&button);
}

void Toolbar::setEnabled(int id, bool enabled) {
	SendMessage(this->hToolbar, TB_ENABLEBUTTON, id, enabled);
}

Toolbar::~Toolbar() {
	DestroyWindow(this->hToolbar);
	DeleteObject(this->toolbarBitmap);
	this->hToolbar = NULL;
}
