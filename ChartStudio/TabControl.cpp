#include "stdafx.h"
#include "TabControl.h"

bool TabControl::IsInitialized = false;
const char TabControl::ClassName[] = "TabControlClass";


TabControl::TabControl(HWND parent, int x, int y, int w, int h, TABCONTROLCHANGE* onTabSwitch) {
	TabControl::Initialize();
	this->onTabSwitch = onTabSwitch;
	this->containerHWND = CreateWindowExA(0, TabControl::ClassName, "", WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN, x, y, w, h, parent, NULL, GetModuleHandleA(NULL), this);
	this->tabHWND=CreateWindowExA(0, WC_TABCONTROL, "", WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE, 0, 0, w, h, this->containerHWND, NULL, GetModuleHandleA(NULL), NULL);

	SetWindowSubclass(this->tabHWND, TabControl::Subclassproc, 1, 0);
}

TabControl::~TabControl() {
	for (auto & child : this->children) {
		delete child;
	}
}



//STATIC METHODS
void TabControl::Initialize() {
	if (IsInitialized == false) {
		IsInitialized = true;

		WNDCLASSEXA wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEXA);
		wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = TabControl::WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = sizeof(TabControl*);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
		wndClass.hInstance = GetModuleHandleA(nullptr);
		wndClass.hIcon = NULL;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = TabControl::ClassName;
		wndClass.hIconSm = NULL;
		RegisterClassExA(&wndClass);
	}
}

LRESULT CALLBACK TabControl::Subclassproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	switch (uMsg) {
	case WM_SETFOCUS:
		//prevent tab from getting keyboard focus
		SetFocus(GetParent(hWnd));
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_COMMAND:
		SendMessage(GetParent(hWnd), uMsg, wParam, lParam);
		return 0;
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CALLBACK TabControl::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_CREATE:
		{
			CREATESTRUCTA* cs = (CREATESTRUCTA*)lParam;
			TabControl* thisTab = (TabControl*)cs->lpCreateParams;
			SetWindowLongPtr(hWnd, 0, (LONG)thisTab);
		}
		return 0;

	case WM_DESTROY:
		{
			TabControl* thisTab = (TabControl*)GetWindowLongPtr(hWnd, 0);
			if (thisTab) {
				SetWindowLongPtr(hWnd, 0, NULL);
			}
		}
		return 0;

	case WM_SIZE:
		{
			TabControl* thisTab = (TabControl*)GetWindowLongPtr(hWnd, 0);
			if (thisTab) {
				RECT rc;
				GetClientRect(hWnd, &rc);
				MoveWindow(thisTab->tabHWND,0, 0, rc.right - rc.left, rc.bottom-rc.top, true);
			}
		}
		return 0;

	case WM_NOTIFY:
		{
			LPNMHDR lpnmhdr = (LPNMHDR)lParam;
			TabControl* thisTab = (TabControl*)GetWindowLongPtr(hWnd, 0);
			if (lpnmhdr && thisTab) {
				if (lpnmhdr->hwndFrom == thisTab->tabHWND) {
					switch (lpnmhdr->code) {
					case TCN_SELCHANGE:
						{
							thisTab->updateChildren();
							int currentTab = SendMessage(thisTab->tabHWND, TCM_GETCURSEL, 0, 0);
							if (thisTab->onTabSwitch) {
								thisTab->onTabSwitch(currentTab);
							}
						}
						return 0;
					}
				}
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_CHAR:
	case WM_COMMAND:
		SendMessage(GetParent(hWnd), message, wParam, lParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
	
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//INSTANCE METHODS
int TabControl::addTab(string title) {
	TCITEMA newtab;
	newtab.mask = TCIF_TEXT | TCIF_IMAGE;
	newtab.iImage = -1;
	newtab.pszText = (LPSTR)title.c_str();
	newtab.cchTextMax = title.length();
	SendMessage(this->tabHWND, TCM_INSERTITEM, this->tabCount, (LPARAM)&newtab);
	SendMessage(this->tabHWND, WM_SIZE, 0, 0);
	return this->tabCount++;
}

void TabControl::addChildToTab(HWND hwnd, int tabIndex) {
	TabControlChild* newchild = new TabControlChild();
	newchild->childHWND = hwnd;
	newchild->parentTab = tabIndex;
	children.push_back(newchild);
	this->updateChildren();
}

int TabControl::currentTab() {
	return SendMessage(this->tabHWND, TCM_GETCURSEL, 0, 0);
}

void TabControl::updateChildren() {
	int currentTab = SendMessage(this->tabHWND, TCM_GETCURSEL, 0, 0);
	for (auto & child : this->children) {
		if (child->parentTab != currentTab) {
			ShowWindow(child->childHWND, SW_HIDE);
		}
	}
	for (auto & child : this->children) {
		if (child->parentTab == currentTab) {
			ShowWindow(child->childHWND, SW_SHOW);
			RedrawWindow(child->childHWND, nullptr, NULL, RDW_UPDATENOW | RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
		}
	}
}

void TabControl::move(int x, int y, int w, int h) {
	MoveWindow(this->containerHWND, x, y, w, h, true);
}

RECT TabControl::getRect() {
	RECT rc;
	RECT tabRC;
	GetWindowRect(this->tabHWND, &tabRC);
	GetWindowRect(this->containerHWND, &rc);
	SendMessage(this->tabHWND, TCM_ADJUSTRECT, false, (LPARAM)&tabRC);
	tabRC.right -= tabRC.left;
	tabRC.bottom -= tabRC.top;

	tabRC.left = tabRC.left-rc.left;
	tabRC.top = tabRC.top-rc.top;

	tabRC.bottom += tabRC.top;
	tabRC.right += tabRC.left;
	return tabRC;
}