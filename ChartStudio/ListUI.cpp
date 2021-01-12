#include "stdafx.h"
#include "ButtonBar.h"
#include "ListUI.h"

ListUI::ListUI(string title, HWND parent, int x, int y, int w, int h, LISTUIDRAWITEMPROC drawItemProc, LISTUISELCHANGE* selChangeProc, LISTUIDBLCLK* dblClkProc, LISTUIRIGHTCLK* rightClkProc) {
	ListUI::Initialize();
	this->title = title;
	this->parent = parent;
	this->dblClkProc = dblClkProc;
	this->drawItemProc = drawItemProc;
	this->selChangeProc = selChangeProc;
	this->rightClkProc = rightClkProc;
	this->titleHeight = 20;
	HINSTANCE hinst = GetModuleHandleA(NULL);
	this->hwnd = CreateWindowExA(0, ListUI::ClassName, "", WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN, x, y, w, h, parent, NULL, hinst, this);
	if (this->hwnd) {
		this->buttonBar = new ButtonBar(this->hwnd, 0, 0, w, 4, RGB(128, 128, 128));
		this->hListbox = CreateWindowEx(0, WC_LISTBOXA, "Listbox", LBS_WANTKEYBOARDINPUT | LBS_NOTIFY | WS_VSCROLL | WS_CHILD | LBS_OWNERDRAWFIXED | WS_VISIBLE | LBS_NOINTEGRALHEIGHT | WS_CLIPCHILDREN, 0, 40, w, h - 40, this->hwnd, 0, hinst, NULL);
		SendMessage(hListbox, LB_SETITEMHEIGHT, 0, 18);
		SetWindowLongPtr(this->hListbox, GWL_USERDATA, GetWindowLongPtr(this->hListbox, GWL_WNDPROC));
		SetWindowLongPtr(this->hListbox, GWL_WNDPROC, (LONG)ListUI::ListboxSubclass);

		this->itemBackFillEvensBrush = CreateSolidBrush(itemBackFillEvens);
	}
}


ListUI::~ListUI() {
	if (this->buttonBar) delete this->buttonBar;
	if (this->hwnd) DestroyWindow(this->hwnd);
	DeleteObject(this->itemBackFillEvensBrush);
	for (auto & column : this->columns) {
		if (column) delete column;
	}
}


//STATIC MEMBERS
bool ListUI::IsInitialized=false;
const char ListUI::ClassName[]="ListUIClass";

//STATIC METHODS
void ListUI::Initialize() {
	if (IsInitialized == false) {
		IsInitialized = true;

		WNDCLASSEXA wndClass;
		wndClass.cbSize = sizeof(WNDCLASSEXA);
		wndClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = ListUI::WndProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = sizeof(ListUI*);
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = NULL;
		wndClass.hInstance = GetModuleHandleA(nullptr);
		wndClass.hIcon = NULL;
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = ListUI::ClassName;
		wndClass.hIconSm = NULL;
		RegisterClassExA(&wndClass);
	}
}


void ListUI::addString(const char* string) {
	SendMessage(this->hListbox, LB_ADDSTRING, 0, (LPARAM)string);
}

void ListUI::clearContents() {
	SendMessage(this->hListbox, LB_RESETCONTENT, 0, 0);
}

void ListUI::deleteString(char* data) {
	void* itemData = nullptr;
	bool hadADelete;
	do {
		hadADelete = false;
		int items = SendMessage(this->hListbox, LB_GETCOUNT, 0, 0);
		for (int i = 0; i < items; i++) {
			itemData = (void*)SendMessage(this->hListbox, LB_GETITEMDATA, i, 0);
			if (itemData == data) {
				SendMessage(this->hListbox, LB_DELETESTRING, i, 0);
				hadADelete = true;
				break;
			}
		}
	} while (hadADelete == true);
}

bool ListUI::getIndexFromData(char* data, int& indexOut) {
	if (data == nullptr) return false;
	int items = SendMessage(this->hListbox, LB_GETCOUNT, 0, 0);
	void* itemData = nullptr;
	for (int i = 0; i < items; i++) {
		itemData = (void*)SendMessage(this->hListbox, LB_GETITEMDATA, i, 0);
		if (itemData == data) {
			indexOut = i;
			return true;
		}
	}
	return false;
}

bool ListUI::getCurSel(int& curSelOut) {
	int curSelIndex = SendMessage(this->hListbox, LB_GETCURSEL, 0, 0);
	if (curSelIndex != LB_ERR) {
		curSelOut = curSelIndex;
		return true;
	}
	return false;
}

void ListUI::setCurSel(int index) {
	SendMessage(this->hListbox, LB_SETCURSEL, index, 0);
}

void ListUI::setCurSelItemData(char* itemData) {
	int index = 0;
	if (this->getIndexFromData(itemData, index)) {
		this->setCurSel(index);
	}
}

char* ListUI::getItemData(int index) {
	long returnVal = SendMessage(this->hListbox, LB_GETITEMDATA, index, 0);
	if (returnVal == -1) returnVal = 0;
	return (char*)returnVal;
}

char* ListUI::getCurSelItemData() {
	int curSel;
	if (this->getCurSel(curSel)) {
		return this->getItemData(curSel);
	}
	return nullptr;
}

int ListUI::getTopIndex() {
	return SendMessage(this->hListbox, LB_GETTOPINDEX, 0, 0);
}

void ListUI::setTopIndex(int item) {
	SendMessage(this->hListbox, LB_SETTOPINDEX, item, 0);
}

void ListUI::pauseDrawing() {
	SendMessage(this->hListbox, WM_SETREDRAW, false, 0);
}

void ListUI::resumeDrawing() {
	SendMessage(this->hListbox, WM_SETREDRAW, true, 0);
	RedrawWindow(this->hListbox, nullptr, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

void ListUI::redraw() {
	RedrawWindow(this->hListbox, nullptr, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

LRESULT CALLBACK ListUI::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
	case WM_CREATE:
		{
			CREATESTRUCTA* cs = (CREATESTRUCTA*)lParam;
			ListUI* thisUI = (ListUI*)cs->lpCreateParams;
			SetWindowLongPtr(hWnd, 0, (LONG)thisUI);
		}
		return 0;

	case WM_DESTROY:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				SetWindowLongPtr(hWnd, 0, NULL);
			}
		}
		return 0;

	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_CHAR:
		//send it to the parent
		return SendMessage(GetParent(hWnd), message, wParam, lParam);

	case WM_PAINT:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			RECT titleRect;
			HDC dc = GetDC(hWnd);
			GetClientRect(hWnd, &titleRect);

			HBRUSH backColorBrush = CreateSolidBrush(RGB(0, 0, 0));
			FillRect(dc, &titleRect, backColorBrush);//fill the whole control with black
			DeleteObject(backColorBrush);

			SetBkMode(dc, TRANSPARENT);

			HFONT titleFont = CreateFontA(thisUI->titleHeight - 2, 0, 0, 0, FW_SEMIBOLD, 0, 0, false,
				DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");
			HFONT oldFont = (HFONT)SelectObject(dc, (HGDIOBJ)titleFont);

			titleRect.bottom = thisUI->titleHeight;
			HBRUSH titleBackColor = CreateSolidBrush(thisUI->titleBackColor);
			FillRect(dc, &titleRect, titleBackColor);
			DeleteObject(titleBackColor);
			//DrawEdge(dc, &titleRect, EDGE_RAISED, BF_RECT| BF_ADJUST);
			//titleRect.top-=2;
			SetTextColor(dc, thisUI->titleTextColor);
			DrawTextA(dc, thisUI->title.c_str(), -1, &titleRect, DT_CENTER  | DT_SINGLELINE);

			SelectObject(dc, oldFont);
			DeleteObject(titleFont);

			SetTextColor(dc, RGB(0, 0, 0));

			if (thisUI) {
				RECT headerRect;
				GetClientRect(hWnd, &headerRect);

				headerRect.top = thisUI->titleHeight + 1;

				int headerH = thisUI->extraHeaderHeight;
				if (thisUI->buttonBar) {
					headerRect.top += thisUI->buttonBar->getHeight();
				}
				if (thisUI->hListbox) {
					headerH += SendMessage(thisUI->hListbox, LB_GETITEMHEIGHT, 0, 0);
					headerRect.bottom = headerRect.top + headerH - 1;
				}
				int textHeight = headerRect.bottom - headerRect.top + thisUI->extraHeaderHeight -1;

				//draw headers;
				FillRect(dc, &headerRect, GetSysColorBrush(COLOR_BTNFACE));

				///adjust width to subtract listbox scrollbar width
				if (thisUI->hListbox) {
					if (GetWindowLong(thisUI->hListbox, GWL_STYLE) & WS_VSCROLL) {
						RECT itemRect;
						if (SendMessage(thisUI->hListbox, LB_GETITEMRECT, 0, (LPARAM)&itemRect) != LB_ERR) {
							headerRect.right = itemRect.right;
						}
					}
				}

				HFONT font = CreateFontA(textHeight, 0, 0, 0, FW_MEDIUM, 0, 0, false,
					DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");
				HFONT oldFont = (HFONT)SelectObject(dc, (HGDIOBJ)font);

				if (thisUI->columns.size() > 0) {
					int w = headerRect.right-headerRect.left;
					RECT thisColumnRect = headerRect;
					for (size_t i=0;i<thisUI->columns.size();i++){
						if (i == thisUI->columns.size() - 1) {
							thisColumnRect.right = headerRect.right;
						} else {
							thisColumnRect.right = thisColumnRect.left + int(float(w)*thisUI->columns[i]->widthPercent);
						}

						DrawTextA(dc, thisUI->columns[i]->title.c_str(), -1, &thisColumnRect, DT_NOCLIP |DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						DrawEdge(dc, &thisColumnRect, EDGE_RAISED, BF_RECT);
						thisColumnRect.left = thisColumnRect.right;
					}
				} else {
					DrawEdge(dc, &headerRect, EDGE_RAISED, BF_RECT);
				}

				SelectObject(dc, oldFont);
				DeleteObject(font);
			}
			ReleaseDC(hWnd, dc);
			ValidateRect(hWnd, nullptr);
		}
		return 0;

	case WM_SIZE:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				RECT thisRect;
				int barH = 0;
				int headerH = 0;
				GetClientRect(hWnd, &thisRect);

				if (thisUI->buttonBar) {
					thisUI->buttonBar->move(0,thisUI->titleHeight, thisRect.right);
					barH = thisUI->buttonBar->getHeight();
				}

				if (thisUI->hListbox) {
					headerH = SendMessage(thisUI->hListbox, LB_GETITEMHEIGHT, 0, 0)+thisUI->extraHeaderHeight;
					MoveWindow(thisUI->hListbox, 0, barH+headerH+ thisUI->titleHeight, thisRect.right, thisRect.bottom - barH - headerH - thisUI->titleHeight, true);
				}
			}
		}
		return 0;

	case WM_CTLCOLORLISTBOX:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				return (LRESULT)thisUI->itemBackFillEvensBrush;
			}
		}
		break;

	case WM_DRAWITEM:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				//Do scrollbar detection
				bool scrollbarVisible = GetWindowLong(thisUI->hListbox, GWL_STYLE) & WS_VSCROLL;
				if (scrollbarVisible != thisUI->isScrollbarVisible) {
					thisUI->isScrollbarVisible = scrollbarVisible;
					SendMessage(thisUI->hwnd, WM_PAINT, 0, 0);
				}


				DRAWITEMSTRUCT* pdis = (DRAWITEMSTRUCT*)lParam;
				if (pdis->CtlType == ODT_LISTBOX && pdis->itemID != -1) {

					int textHeight = pdis->rcItem.bottom - pdis->rcItem.top;
					HFONT standardFont = CreateFontA(textHeight, 0, 0, 0, FW_MEDIUM, 0, 0, false,
						DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH, "Segoe UI");
					HFONT oldFont = (HFONT)SelectObject(pdis->hDC, (HGDIOBJ)standardFont);


					HBRUSH backColorBrush = NULL;
					if (pdis->itemState & ODS_SELECTED) {
						backColorBrush = CreateSolidBrush(thisUI->itemSelectBackFill);
					} else {
						if (!(pdis->itemID % 2)) {
							backColorBrush = CreateSolidBrush(thisUI->itemBackFillEvens);
						} else {
							backColorBrush = CreateSolidBrush(thisUI->itemBackFillOdds);
						}
					}
					FillRect(pdis->hDC, &pdis->rcItem, backColorBrush);
					DeleteObject(backColorBrush);
					backColorBrush = NULL;
					SetBkMode(pdis->hDC, TRANSPARENT);
					SetTextColor(pdis->hDC, thisUI->textColor);


					DRAWITEMSTRUCT columnpdis = *pdis;
					if (thisUI->columns.size() > 0) {
						int w = pdis->rcItem.right - pdis->rcItem.left;
						for (size_t i = 0; i < thisUI->columns.size(); i++) {
							if (thisUI->drawItemProc) {
								if (i == thisUI->columns.size() - 1) {
									columnpdis.rcItem.right = w;
								} else {
									columnpdis.rcItem.right = long(double(columnpdis.rcItem.left) + double(w) * thisUI->columns[i]->widthPercent);
								}
								thisUI->drawItemProc(&columnpdis, i);
								columnpdis.rcItem.left = columnpdis.rcItem.right;
							}
						}
					} else {
						if (thisUI->drawItemProc) {
							thisUI->drawItemProc(&columnpdis, 0);
						}
					}

					if (columnpdis.itemState & ODS_SELECTED) {
						DrawFocusRect(pdis->hDC, &pdis->rcItem);
					}

					SelectObject(pdis->hDC, oldFont);
					DeleteObject(standardFont);
				}
			}
		}
		break;

	case LISTUI_LDBLCLKITEM:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				if (thisUI->dblClkProc) {
					thisUI->dblClkProc(thisUI->getItemData(lParam), lParam);
				}
			}
		}
		return 1;

	case LISTUI_RCLICKITEM:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				if (thisUI->rightClkProc) {
					thisUI->rightClkProc(thisUI->getItemData(lParam), lParam);
				}
			}
		}
		return 1;

	case WM_COMMAND:
		{
			ListUI* thisUI = (ListUI*)GetWindowLongPtr(hWnd, 0);
			if (thisUI) {
				int wmId = LOWORD(wParam);
				int notification = HIWORD(wParam);
				switch (notification) {
					case BN_CLICKED:
						SendMessage(thisUI->parent, WM_COMMAND, wParam, lParam);
						break;
					case LBN_DBLCLK:
						if (thisUI->dblClkProc) {
							HWND listbox = (HWND)lParam;
							char* itemData = nullptr;
							int itemIndex = SendMessage(listbox, LB_GETCURSEL, 0, 0);
							if (itemIndex != LB_ERR) {
								itemData = (char*)SendMessage(listbox, LB_GETITEMDATA, itemIndex, 0);
							}
							thisUI->dblClkProc(itemData, itemIndex);
						}
						break;
					case LBN_SELCANCEL:
					case LBN_SELCHANGE:
						if (thisUI->selChangeProc) {
							HWND listbox = (HWND)lParam;
							char* itemData = nullptr;
							int itemIndex = SendMessage(listbox, LB_GETCURSEL, 0, 0);
							if (itemIndex != LB_ERR) {
								itemData = (char*)SendMessage(listbox, LB_GETITEMDATA, itemIndex, 0);
							}
							thisUI->selChangeProc(itemData, itemIndex);
						}
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}else{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

LRESULT ListUI::ListboxSubclass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_RBUTTONUP:
		{
			POINT mouseClient;
			RECT clientArea;
			GetClientRect(hWnd, &clientArea);
			GetCursorPos(&mouseClient);
			ScreenToClient(hWnd, &mouseClient);
			if (PtInRect(&clientArea, mouseClient)) {
				LPARAM ret = SendMessage(hWnd, LB_ITEMFROMPOINT, 0, MAKELPARAM(mouseClient.x, mouseClient.y));
				if (HIWORD(ret) == 0) {
				} else {
					ret = -1;
				}
				if (SendMessage(hWnd, LB_GETCURSEL, 0, 0) != LOWORD(ret)) {
					SendMessage(hWnd, LB_SETCURSEL, LOWORD(ret), 0);
					SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(0, LBN_SELCHANGE), (LPARAM)hWnd);
				}
				SendMessage(GetParent(hWnd), LISTUI_RCLICKITEM, 0, LOWORD(ret));
			}
		}
		return 0;

	case WM_LBUTTONDBLCLK:
		{
			POINT mouseClient;
			RECT clientArea;
			GetClientRect(hWnd, &clientArea);
			GetCursorPos(&mouseClient);
			ScreenToClient(hWnd, &mouseClient);
			if (PtInRect(&clientArea, mouseClient)) {
				LPARAM ret = SendMessage(hWnd, LB_ITEMFROMPOINT, 0, MAKELPARAM(mouseClient.x, mouseClient.y));
				if (HIWORD(ret) == 0) {
					if (SendMessage(hWnd, LB_GETCURSEL, 0, 0) != LOWORD(ret)) {
						SendMessage(hWnd, LB_SETCURSEL, LOWORD(ret), 0);
						SendMessage(GetParent(hWnd), WM_COMMAND, MAKELONG(0, LBN_SELCHANGE), (LPARAM)hWnd);
					}
					SendMessage(GetParent(hWnd), LISTUI_LDBLCLKITEM, 0, LOWORD(ret));
				}
			}
		}
		return 0;

	case WM_KEYDOWN:
		if (wParam == VK_UP || wParam == VK_DOWN) {
			break;
		}
	case WM_KEYUP:
	case WM_CHAR:
		//send it to the parent of the parent
		return SendMessage(GetParent(GetParent(hWnd)), message, wParam, lParam);
	}
	return CallWindowProc((WNDPROC)GetWindowLongPtr(hWnd, GWL_USERDATA), hWnd, message, wParam, lParam);
}

void ListUI::addColumn(float widthPercent, string title) {
	ListUIColumns* column = new ListUIColumns();
	column->title = title;
	column->widthPercent = widthPercent;
	this->columns.push_back(column);
}

void ListUI::move(int x, int y, int w, int h) {
	MoveWindow(this->hwnd, x, y, w, h, true);
}

void ListUI::show(bool show) {
	if (show) {
		ShowWindow(this->getHwnd(), SW_SHOW);
	} else {
		ShowWindow(this->getHwnd(), SW_HIDE);
	}
}

void ListUI::setListColors(COLORREF itemBackFillEvens, COLORREF itemBackFillOdds, COLORREF itemSelectBackFill, COLORREF textColor) {
	this->itemBackFillEvens = itemBackFillEvens;
	this->itemBackFillOdds = itemBackFillOdds;
	this->itemSelectBackFill = itemSelectBackFill;
	this->textColor = textColor;
	if (this->itemBackFillEvensBrush) {
		DeleteObject(this->itemBackFillEvensBrush);
	}
	this->itemBackFillEvensBrush = CreateSolidBrush(itemBackFillEvens);

	RedrawWindow(this->hListbox, nullptr, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
}

void ListUI::setTitleDetails(string title, COLORREF backColor, COLORREF textColor, int height) {
	this->title = title;
	this->titleBackColor = backColor;
	this->titleTextColor = textColor;
	this->titleHeight = height;
	SendMessage(this->hwnd, WM_PAINT, 0, 0);
}
