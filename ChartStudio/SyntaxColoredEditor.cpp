#include <stdlib.h>
#include "stdafx.h"
#include "helpers.h"
#include "SyntaxColoredEditor.h"

bool SyntaxColoredEditor::IsInitialized = false;

 const char* SyntaxColoredEditor::WORDDEF[] = { "if", "while", "for", "loop", "else", "break", "return","exit","double","string","bool"};
 const int SyntaxColoredEditor::WORDMAX = NELEMS(WORDDEF);
 int SyntaxColoredEditor::WORDLEN[NELEMS(WORDDEF)];
 const char* SyntaxColoredEditor::FUNCDEF[] = { "min","max", "ceil","floor","clamp","abs","lcase","ucase","trim","len","substr" };
 const int SyntaxColoredEditor::FUNCMAX = NELEMS(FUNCDEF);
 int SyntaxColoredEditor::FUNCLEN[NELEMS(FUNCDEF)];
 const char* SyntaxColoredEditor::VARDEF[] = { "true", "false", "null", "this"};
 const int SyntaxColoredEditor::VARMAX = NELEMS(VARDEF);
 int SyntaxColoredEditor::VARLEN[NELEMS(VARDEF)];

void SyntaxColoredEditor::initDictionary() {
	for (int i = 0; i < WORDMAX; i++) {
		WORDLEN[i] = strlen(WORDDEF[i]);
	}
	for (int i = 0; i < FUNCMAX; i++) {
		FUNCLEN[i] = strlen(FUNCDEF[i]);
	}
	for (int i = 0; i < VARMAX; i++) {
		VARLEN[i] = strlen(VARDEF[i]);
	}
}

SyntaxColoredEditor::SyntaxColoredEditor(HWND parent, int x, int y, int w, int h) {
	if (IsInitialized == false) {
		IsInitialized = true;
		initDictionary();
		LoadLibraryA("riched20.dll");
	}


	HINSTANCE hinst = (HINSTANCE)GetWindowLongPtr(parent, GWL_HINSTANCE);
	this->hwnd = CreateWindowEx(0, RICHEDIT_CLASS, NULL, ES_MULTILINE | ES_WANTRETURN | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | WS_HSCROLL | WS_VSCROLL | ES_NOHIDESEL, x, y, w, h, parent, NULL, hinst, NULL);
	if (this->hwnd) {
		SetWindowLongPtr(this->hwnd, GWL_USERDATA, (long int)this);

		SetWindowText(this->hwnd, "");//control has to be empty to set TM_PLAINTEXT textmode
		SendMessage(this->hwnd, EM_SETEDITSTYLE, SES_EMULATESYSEDIT, SES_EMULATESYSEDIT);
		SendMessage(this->hwnd, EM_SETTEXTMODE, TM_PLAINTEXT | TM_MULTILEVELUNDO, 0);
		SendMessage(this->hwnd, EM_LIMITTEXT, -1, 0);
		SendMessage(this->hwnd, EM_SETUNDOLIMIT, 50, 0);

		setFont(10, "Segoe UI");
		setLineNumWidth(45);
		setBackColor(RGB(240, 240, 250));

		//After the data is all initialized, do the subclass and force a redraw
		this->wndProc = (WNDPROC)GetWindowLongPtr(this->hwnd, GWL_WNDPROC);
		SetWindowLongPtr(this->hwnd, GWL_WNDPROC, (long int)&SyntaxColoredEditor::RichProc);
		RedrawWindow(this->hwnd, nullptr, nullptr, RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_FRAME | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
}


SyntaxColoredEditor::~SyntaxColoredEditor() {
	DestroyWindow(this->hwnd);
}

void SyntaxColoredEditor::move(int x, int y, int w, int h) {
	MoveWindow(this->hwnd, x, y, w, h, true);
}

bool SyntaxColoredEditor::getModified() {
	return SendMessage(this->hwnd, EM_GETMODIFY, 0, 0);
}
void SyntaxColoredEditor::setModified(bool flag) {
	SendMessage(this->hwnd, EM_SETMODIFY, flag, 0);
}


void SyntaxColoredEditor::setText(string text) {
	SetWindowTextA(this->hwnd, text.c_str());
}

string SyntaxColoredEditor::getText() {
	string returnVal;
	if (this->hwnd) {
		int textLen = GetWindowTextLengthA(this->hwnd) + 1;
		char* buff = new char[textLen];
		if (buff) {
			ZeroMemory(buff, textLen);
			GetWindowTextA(this->hwnd, buff, textLen);
			returnVal = buff;
			delete[] buff;
		}
	}
	return returnVal;
}

void SyntaxColoredEditor::setFont(int fontSize, string faceName) {
	HDC editDC = GetDC(this->hwnd);
	int fontHeight = -MulDiv(fontSize, GetDeviceCaps(editDC, LOGPIXELSY), 72);
	HFONT newFont = CreateFont(fontHeight, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, faceName.c_str());
	ReleaseDC(hwnd, editDC);
	if (newFont) {
		if (this->font) DeleteObject(this->font);
		this->font = newFont;
		SendMessage(hwnd, WM_SETFONT, (WPARAM)this->font, 1);
		this->setTextColor(this->textColor);//gotta reset text color after font change
	}
}

void SyntaxColoredEditor::setLineNumWidth(int width){
		this->lineNumberWidth = width;

		HDC editDC = GetDC(hwnd);
		if (!this->lineNumberDC) {
			this->lineNumberDC = CreateCompatibleDC(editDC);
		}

		this->lineNumberBitmap = CreateCompatibleBitmap(editDC, this->lineNumberWidth, 2160);
		DeleteObject(SelectObject(this->lineNumberDC, this->lineNumberBitmap));
		ReleaseDC(hwnd, editDC);

		SendMessage(this->hwnd, EM_SETMARGINS, EC_LEFTMARGIN, this->lineNumberWidth);
}
void SyntaxColoredEditor::setBackColor(COLORREF backcolor) {
	this->backgroundColor = backcolor;
	SendMessage(hwnd, EM_SETBKGNDCOLOR, 0, this->backgroundColor);
}

void SyntaxColoredEditor::setTextColor(COLORREF textColor) {
	this->textColor = textColor;
	CHARFORMAT2 cf2;
	ZeroMemory(&cf2, sizeof(CHARFORMAT2));
	cf2.cbSize = sizeof(CHARFORMAT2);
	cf2.dwMask = CFM_COLOR;
	cf2.dwEffects = 0;
	cf2.crTextColor = this->textColor;
	SendMessage(hwnd, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf2);
}

char* SyntaxColoredEditor::getRange(unsigned int start, unsigned int end, int* textLen) {
	TEXTRANGE tr;
	if (end > start) {
		tr.chrg.cpMax = end;
		tr.chrg.cpMin = start;
	} else {
		tr.chrg.cpMax = start;
		tr.chrg.cpMin = end;
	}
	if (textLen) *textLen = tr.chrg.cpMax - tr.chrg.cpMin;

	tr.lpstrText = (char*)malloc(tr.chrg.cpMax - tr.chrg.cpMin + 1);
	if (tr.lpstrText) {
		SendMessage(this->hwnd, EM_GETTEXTRANGE, 0, (LPARAM)&tr);
		tr.lpstrText[tr.chrg.cpMax - tr.chrg.cpMin] = 0;
		return tr.lpstrText;
	}
	return NULL;
}


int SyntaxColoredEditor::fontTwipsToPixelHeight(int fontHeightTwips) {
	HDC dc = GetDC(NULL);
	int vertPPI = GetDeviceCaps(dc, LOGPIXELSY);
	ReleaseDC(NULL, dc);
	
	return int((double(fontHeightTwips)*double(vertPPI) / 1440.0f)+0.5f);
}

int inline SyntaxColoredEditor::isOperator(char chr) {
	switch (chr) {
	case '<':
	case '>':
	case '-':
	case '=':
	case '+':
	case '^':
	case '/':
	case '*':
	case '!':
	case '|':
	case '&':
	case '@':
		return 1;
	}
	return 0;
}

int inline SyntaxColoredEditor::isOperator2(char chr) {
	switch (chr) {
	case ',':
	case '(':
	case ')':
	case '{':
	case '}':
	case ';':
		return 1;
	}
	return 0;
}

int SyntaxColoredEditor::selHitTest(CHARRANGE selRange, int firstChar, int len) {
	if (selRange.cpMin == selRange.cpMax) return 0;
	if (firstChar >= selRange.cpMin && firstChar < selRange.cpMax) return 1;
	if (firstChar + len > selRange.cpMin && firstChar + len < selRange.cpMax) return 1;
	if (selRange.cpMin > firstChar && selRange.cpMin < firstChar + len) return 1;
	if (selRange.cpMax > firstChar && selRange.cpMax < firstChar + len) return 1;
	return 0;
}

void SyntaxColoredEditor::drawText(HDC dc, char* text, int len, int firstChar, CHARRANGE selRange, int fontHeight) {
	POINTL  point;
	POINTL  lastPoint;
	if (!selHitTest(selRange, firstChar, len)) {
		SendMessage(this->hwnd, EM_POSFROMCHAR, (WPARAM)&point, firstChar);
		SendMessage(this->hwnd, EM_POSFROMCHAR, (WPARAM)&lastPoint, firstChar+len);

		RECT clipRect;
		clipRect.left = point.x;
		clipRect.top = point.y;
		clipRect.right = lastPoint.x;
		clipRect.bottom = lastPoint.y + fontHeight;
		ExtTextOut(dc, point.x, point.y, ETO_CLIPPED| ETO_OPAQUE, &clipRect, text, len, NULL);
	}
}

void SyntaxColoredEditor::drawWithoutTabs(HDC dc, char* text, int len, int lineFirstChar, int offset, CHARRANGE selRange, int fontHeight) {
	int index = 0;
	int printLen = 0;
	int printStart = 0;
	POINTL  point;
	POINTL  lastPoint;
	while (index <= len) {
		if (index < len && text[index]>32) {
			if (printLen == 0) printStart = index;
			printLen++;
		} else {
			if (printLen) {
				if (!selHitTest(selRange, lineFirstChar + offset + printStart, printLen)) {
					SendMessage(this->hwnd, EM_POSFROMCHAR, (WPARAM)&point, lineFirstChar + offset + printStart);
					SendMessage(this->hwnd, EM_POSFROMCHAR, (WPARAM)&lastPoint, lineFirstChar + offset + printStart+printLen);
					RECT clipRect;
					clipRect.left = point.x;
					clipRect.top = point.y;
					clipRect.right = lastPoint.x;
					clipRect.bottom = lastPoint.y + fontHeight;
					ExtTextOut(dc, point.x, point.y, ETO_CLIPPED| ETO_OPAQUE, &clipRect, text + printStart, printLen, NULL);
				}
				printLen = 0;
			}
		}
		index++;
	}
}

void SyntaxColoredEditor::updateLineNumbers() {
	RECT drawRect;
	POINT point;
	char lineNumberTxt[32];
	CHARRANGE selRange;


	HFONT oldNumFont = (HFONT)SelectObject(this->lineNumberDC, (HGDIOBJ)this->font);

	SendMessage(this->hwnd, EM_GETRECT, 0, (LPARAM)&drawRect);

	SetBkMode(this->lineNumberDC, TRANSPARENT);
	drawRect.left = 0;
	drawRect.top = 0;
	drawRect.right = this->lineNumberWidth;
	HBRUSH lineNumberBackBrush = CreateSolidBrush(this->lineNumberBkColor);
	FillRect(this->lineNumberDC, &drawRect, lineNumberBackBrush);
	DeleteObject(lineNumberBackBrush);

	SendMessage(this->hwnd, EM_EXGETSEL, 0, (LPARAM)&selRange);
	if (selRange.cpMin != selRange.cpMax) selRange.cpMax--;
	int firstSelLine = SendMessage(this->hwnd, EM_LINEFROMCHAR, selRange.cpMin, 0);
	int lastSelLine = SendMessage(this->hwnd, EM_LINEFROMCHAR, selRange.cpMax, 0);


	int lineCount = SendMessage(this->hwnd, EM_GETLINECOUNT, 0, 0);
	int firstLine = SendMessage(this->hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);
	if (firstLine > 0) firstLine--;

	int lastLine = lineCount - 1;

	int lastDrawnLineY = 123456789;
	for (int i = firstLine; i <= lastLine; i++) {
		SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&point, SendMessage(hwnd, EM_LINEINDEX, i, 0));
		if (point.y >= drawRect.bottom) {
			break;
		}


		if (point.y != lastDrawnLineY) {
			lastDrawnLineY = point.y;
			drawRect.top = point.y;
			if (i >= firstSelLine && i <= lastSelLine) {
				snprintf(lineNumberTxt, sizeof(lineNumberTxt), "             %i             ", i + 1);
				SetBkMode(this->lineNumberDC, OPAQUE);
				SetBkColor(this->lineNumberDC, this->lineNumberCurrentBkColor);
				SetTextColor(this->lineNumberDC, this->lineNumberCurrentColor);
			} else {
				snprintf(lineNumberTxt, sizeof(lineNumberTxt), "%i", i + 1);
				SetBkMode(this->lineNumberDC, TRANSPARENT);
				SetTextColor(this->lineNumberDC, this->lineNumberTxtColor);
			}
			DrawText(this->lineNumberDC, lineNumberTxt, strlen(lineNumberTxt), &drawRect, DT_SINGLELINE | DT_CENTER | DT_NOCLIP);
		}
	}

	SelectObject(this->lineNumberDC, oldNumFont);

	HDC editDC = GetDC(this->hwnd);
	BitBlt(editDC, 0, 0, this->lineNumberWidth, drawRect.bottom, this->lineNumberDC, 0, 0, SRCCOPY);
	ReleaseDC(this->hwnd, editDC);
}



LRESULT CALLBACK SyntaxColoredEditor::RichProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	CHARRANGE selRange;
	LRESULT retVal;
	SyntaxColoredEditor* sce=(SyntaxColoredEditor*)GetWindowLongPtr(hwnd, GWL_USERDATA);
	if (!sce) {
		return 0;
	}

	switch (msg) {
	case WM_DESTROY:
		if (sce->font) DeleteObject(sce->font);
		if (sce->lineNumberDC) DeleteDC(sce->lineNumberDC);
		if (sce->lineNumberBitmap) DeleteObject(sce->lineNumberBitmap);
		if (sce->findDialogOpen) {
			sce->findDialogOpen = false;
			DestroyWindow(sce->findDialogHWnd);
		}
		sce->hwnd = NULL;
		SetWindowLongPtr(hwnd, GWL_USERDATA, 0);
		SetWindowLongPtr(hwnd, GWL_WNDPROC, (long int)sce->wndProc);
		return CallWindowProc(sce->wndProc, hwnd, msg, wParam, lParam);
		return 0;

	case WM_MOUSEWHEEL:
		if (!(wParam&MK_CONTROL)) {//Disable zooming
			return CallWindowProc(sce->wndProc, hwnd, msg, wParam, lParam);
		}
		return 0;

	case WM_LBUTTONDOWN:
		retVal = CallWindowProc(sce->wndProc, hwnd, msg, wParam, lParam);

		sce->updateLineNumbers();
		return retVal;

	case WM_KEYDOWN:
		if (wParam == VK_OEM_PLUS && GetKeyState(VK_CONTROL)) {//Prevent superscript/subscript
			return 0;
		}

		if (wParam == VK_TAB) {
			DWORD start, end;
			SendMessage(hwnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
			SendMessage(hwnd, EM_SETSEL, start, start);
			SendMessage(hwnd, EM_REPLACESEL, true, (LPARAM)"\t");
			return 1;
		}
		
		sce->updateLineNumbers();
		retVal = CallWindowProc(sce->wndProc, hwnd, msg, wParam, lParam);
		return retVal;

	case WM_RBUTTONUP: {
			POINT mouse;

			SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM)&selRange);

			GetCursorPos(&mouse);
			HMENU popupMenu = CreatePopupMenu();
			AppendMenu(popupMenu, MF_STRING, RICH_UNDO, "Undo");
			AppendMenu(popupMenu, MF_STRING, RICH_REDO, "Redo");
			AppendMenu(popupMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(popupMenu, MF_STRING, RICH_CUT, "Cut");
			AppendMenu(popupMenu, MF_STRING, RICH_COPY, "Copy");
			AppendMenu(popupMenu, MF_STRING, RICH_PASTE, "Paste");
			AppendMenu(popupMenu, MF_SEPARATOR, 0, NULL);
			AppendMenu(popupMenu, MF_STRING, RICH_SELECTALL, "Select All");
			if (SendMessage(hwnd, EM_CANUNDO, 0, 0) == 0) {
				EnableMenuItem(popupMenu, RICH_UNDO, MF_GRAYED);
			}
			if (SendMessage(hwnd, EM_CANREDO, 0, 0) == 0) {
				EnableMenuItem(popupMenu, RICH_REDO, MF_GRAYED);
			}
			if (selRange.cpMin == selRange.cpMax) {
				EnableMenuItem(popupMenu, RICH_CUT, MF_GRAYED);
				EnableMenuItem(popupMenu, RICH_COPY, MF_GRAYED);
			}
			if (!SendMessage(hwnd, EM_CANPASTE, 0, 0)) {
				EnableMenuItem(popupMenu, RICH_PASTE, MF_GRAYED);
			}

			int clickedItem = TrackPopupMenu(popupMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RETURNCMD | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, hwnd, NULL);
			DestroyMenu(popupMenu);
			switch (clickedItem) {
			case RICH_UNDO:
				SendMessage(hwnd, EM_UNDO, 0, 0);
				break;
			case RICH_REDO:
				SendMessage(hwnd, EM_REDO, 0, 0);
				break;
			case RICH_CUT:
				SendMessage(hwnd, WM_CUT, 0, 0);
				break;
			case RICH_COPY:
				SendMessage(hwnd, WM_COPY, 0, 0);
				break;
			case RICH_PASTE:
				SendMessage(hwnd, WM_PASTE, 0, 0);
				break;
			case RICH_SELECTALL: 
				{
					CHARRANGE selRange;
					selRange.cpMin = 0;
					selRange.cpMax = -1;
					SendMessage(hwnd, EM_EXSETSEL, 0, (LPARAM)&selRange);
				}
				break;
			}
			return 0;
		}

	case WM_PAINT: {
			DWORD numer, denom;
			SendMessage(hwnd, EM_GETZOOM, (WPARAM)&numer, (LPARAM)&denom);
			if (numer != 0 || denom != 0) {
				SendMessage(hwnd, EM_SETZOOM, 0, 0);//Set zoom to 0 since it aint
			}
			
			retVal = CallWindowProc(sce->wndProc, hwnd, msg, wParam, lParam);

			RECT drawRect;
			POINT point, nextLinePoint;
			int leftSide, rightSide;
			//get drawable rect in richedit control
			SendMessage(hwnd, EM_GETRECT, 0, (LPARAM)&drawRect);
			leftSide = drawRect.left;
			rightSide = drawRect.right;
			//Get the max line count, and first visible line.
			int lineCount = SendMessage(hwnd, EM_GETLINECOUNT, 0, 0);
			int firstLine = SendMessage(hwnd, EM_GETFIRSTVISIBLELINE, 0, 0);
			if (firstLine > 0) firstLine--;

			int lastLine = lineCount - 1;

			//setup the DC for drawing 
			HDC editDC = GetDC(hwnd);

			HFONT oldFont = (HFONT)SelectObject(editDC, sce->font);

			//get selection range to know where to not draw
			SendMessage(hwnd, EM_EXGETSEL, 0, (LPARAM)&selRange);

			//setup back color and shit
			SetBkMode(editDC, OPAQUE);
			SetBkColor(editDC, sce->backgroundColor);

			//setup the clipping region
			HRGN clipRegion = CreateRectRgn(drawRect.left, drawRect.top, drawRect.right, drawRect.bottom);
			SelectClipRgn(editDC, clipRegion);


			COLORSTATE state = COLORSTATE::CSNONE;
			for (int i = firstLine; i <= lastLine; i++) {
				int fontHeight = drawRect.bottom;

				SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&point, SendMessage(hwnd, EM_LINEINDEX, i, 0));

				if (point.y >= drawRect.bottom) {
					break;
				}

				if (i < lastLine) {
					int nextLineIndex = SendMessage(hwnd, EM_LINEINDEX, i + 1, 0);
					if (nextLineIndex != -1) {
						SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&nextLinePoint, nextLineIndex);
						if (nextLinePoint.y != point.y) {
							fontHeight = nextLinePoint.y - point.y;
						}
					}
				}


				int leftMostChar = SendMessage(hwnd, EM_LINEINDEX, i, 0);
				point.x = leftSide;
				int firstVisCharIndex = SendMessage(hwnd, EM_CHARFROMPOS, 0, (WPARAM)&point) - leftMostChar;
				point.x = rightSide;
				int rightMostChar = SendMessage(hwnd, EM_CHARFROMPOS, 0, (WPARAM)&point);


				if (rightMostChar - leftMostChar > firstVisCharIndex) {
					int lineLen;
					int charIndex;
					int identStart = 0;
					int identLen = 0;
					char* ident = NULL;
					char* lineText = sce->getRange(leftMostChar, rightMostChar, &lineLen);
					if (lineText && lineLen > 0) {
						state = COLORSTATE::CSNONE;
						charIndex = 0;
						while ((charIndex < lineLen || state != COLORSTATE::CSNONE) && charIndex <= lineLen) {
							if (state == COLORSTATE::CSIDENT) {
								if (!isAlNum(lineText[charIndex]) && lineText[charIndex] != '.' && lineText[charIndex] != '_') {
									state = COLORSTATE::CSNONE;
									ident = lineText + identStart;
									int found;
									found = 0;
									if ((identStart + identLen) >= firstVisCharIndex) {//Is it scrolled so far to the right we cant see it?
										for (int wordIndex = 0; wordIndex < WORDMAX; wordIndex++) {//Check for commands to highlight
											if (identLen == WORDLEN[wordIndex]) {
												if (!strncmp(ident, WORDDEF[wordIndex], WORDLEN[wordIndex])) {
													SetTextColor(editDC, sce->commandColor);
													sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
													found = 1;
													break;
												}
											}
										}
										if (!found && charIndex < lineLen && lineText[charIndex] == '(') {
											for (int wordIndex = 0; wordIndex < FUNCMAX; wordIndex++) {//Check for functions to highlight
												if (identLen == FUNCLEN[wordIndex]) {
													if (!strncmp(ident, FUNCDEF[wordIndex], FUNCLEN[wordIndex])) {
														SetTextColor(editDC, sce->functionColor);
														sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
														found = 1;
														break;
													}
												}
											}
										}
										if (!found) {
											for (int wordIndex = 0; wordIndex < VARMAX; wordIndex++) {//Check for variables to highlight
												if (identLen == VARLEN[wordIndex]) {
													if (!strncmp(ident, VARDEF[wordIndex], VARLEN[wordIndex])) {
														SetTextColor(editDC, sce->variableColor);
														sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
														found = 1;
														break;
													}
												}
											}
										}
										if (!found) {
											SetTextColor(editDC, sce->userVariableColor);
											sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
											found = 1;
										}
									}
								} else {
									identLen++;
								}
							} else if (state == COLORSTATE::CSNUM) {
								if (!isDigit(lineText[charIndex]) && lineText[charIndex] != '.') {
									state = COLORSTATE::CSNONE;
									if ((identStart + identLen) >= firstVisCharIndex) {//Is it scrolled so far to the right we cant see it?
										SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&point, leftMostChar + identStart);
										SetTextColor(editDC, sce->numberColor);
										sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
									}
								} else {
									identLen++;
								}
							} else if (state == COLORSTATE::CSSTRING || (charIndex == lineLen && state == COLORSTATE::CSSTRING)) {
								if (charIndex == lineLen || lineText[charIndex] == '"') {
									state = COLORSTATE::CSSKIP;
									if (charIndex < lineLen) identLen++;
									SetTextColor(editDC, sce->stringColor);
									sce->drawWithoutTabs(editDC, lineText + identStart, identLen, leftMostChar, identStart, selRange, fontHeight);
								} else {
									identLen++;
								}
							} else if (state == COLORSTATE::CSOP1) {
								if (!isOperator(lineText[charIndex])) {
									state = COLORSTATE::CSNONE;
									if ((identStart + identLen) >= firstVisCharIndex) {//Is it scrolled so far to the right we cant see it?
										SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&point, leftMostChar + identStart);
										SetTextColor(editDC, sce->operator1Color);
										sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
									}
								} else {
									identLen++;
								}
							} else if (state == COLORSTATE::CSOP2) {
								if (!isOperator2(lineText[charIndex])) {
									state = COLORSTATE::CSNONE;
									if ((identStart + identLen) >= firstVisCharIndex) {//Is it scrolled so far to the right we cant see it?
										SendMessage(hwnd, EM_POSFROMCHAR, (WPARAM)&point, leftMostChar + identStart);
										SetTextColor(editDC, sce->operator2Color);
										sce->drawText(editDC, lineText + identStart, identLen, leftMostChar + identStart, selRange, fontHeight);
									}
								} else {
									identLen++;
								}
							} 

							if (state == COLORSTATE::CSCOMMENT) {
								state = COLORSTATE::CSNONE;
								charIndex = lineLen;
								SetTextColor(editDC, sce->commentColor);
								sce->drawWithoutTabs(editDC, lineText + identStart, lineLen - identStart, leftMostChar, identStart, selRange, fontHeight);
							}

							if (state == COLORSTATE::CSNONE) {
								identStart = charIndex;
								identLen = 1;
								if (lineText[charIndex] >= 0 && lineText[charIndex] <= 255) {
									if (isAlpha(lineText[charIndex]) || lineText[charIndex] == '_') {
										state = COLORSTATE::CSIDENT;
									} else if (isDigit(lineText[charIndex]) || lineText[charIndex] == '.') {
										state = COLORSTATE::CSNUM;
									} else if (lineText[charIndex] == '"') {
										state = COLORSTATE::CSSTRING;
									} else if (lineText[charIndex] == '/' && charIndex + 1 <= lineLen && lineText[charIndex + 1] == '/') {
										state = COLORSTATE::CSCOMMENT;
									} else if (isOperator(lineText[charIndex])) {
										state = COLORSTATE::CSOP1;
									} else if (isOperator2(lineText[charIndex])) {
										state = COLORSTATE::CSOP2;
									}
								}
							} else if (state == COLORSTATE::CSSKIP) {
								state = COLORSTATE::CSNONE;
							}


							charIndex++;
						}

						free(lineText);
					}
				}
			}

			SelectClipRgn(editDC, NULL);
			DeleteObject(clipRegion);
			SelectObject(editDC, oldFont);
			ReleaseDC(hwnd, editDC);
			sce->updateLineNumbers();
			return retVal;
		}
	}

	return CallWindowProc(sce->wndProc, hwnd, msg, wParam, lParam);
}