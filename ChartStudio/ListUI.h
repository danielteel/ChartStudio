#pragma once
#include <string>
#include <vector>
#include "windows.h"

#define LISTUI_RCLICKITEM (WM_USER+500)
#define LISTUI_LDBLCLKITEM (WM_USER+501)

using namespace std;

class ButtonBar;

typedef void (LISTUIDRAWITEMPROC)(DRAWITEMSTRUCT*, int column);
typedef void (LISTUISELCHANGE)(char*, int);
typedef void (LISTUIDBLCLK)(char*, int);
typedef void (LISTUIRIGHTCLK)(char*, int);

struct ListUIColumns {
	float widthPercent=0;
	string title;
};


class ListUI {
public:
	ListUI(string title, HWND parent, int x, int y, int w, int h,  LISTUIDRAWITEMPROC drawItemProc, LISTUISELCHANGE* selChangeProc, LISTUIDBLCLK* dblClkProc, LISTUIRIGHTCLK* rightClkProc);
	~ListUI();

	//statics
	static void Initialize();
	static bool IsInitialized;
	static const char ClassName[];
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK ListboxSubclass(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//instance
	void addString(const char * string);
	void clearContents();
	void deleteString(char* data);
	bool getIndexFromData(char* data, int & indexOut);
	bool getCurSel(int & curSelOut);
	void setCurSel(int index);
	void setCurSelItemData(char* itemData);
	char* getItemData(int index);
	char* getCurSelItemData();
	int getTopIndex();
	void setTopIndex(int item);
	void pauseDrawing();
	void resumeDrawing();
	void redraw();
	void addColumn(float widthPercent, string title);
	void move(int x, int y, int w, int h);
	void show(bool show);
	void setListColors(COLORREF itemBackFillEvens, COLORREF itemBackFillOdds, COLORREF itemSelectBackFill, COLORREF textColor);
	void setTitleDetails(string title, COLORREF backColor, COLORREF textColor, int height);
	HWND getHwnd() {
		return this->hwnd;
	}
	ButtonBar* buttonBar = nullptr;

private:
	COLORREF itemBackFillEvens = RGB(255,255,255);
	COLORREF itemBackFillOdds = RGB(240, 240, 240);
	COLORREF itemSelectBackFill = RGB(220,220,230);
	COLORREF textColor = RGB(0, 0, 0);
	HBRUSH itemBackFillEvensBrush = NULL;

	COLORREF titleBackColor = RGB(25, 25, 75);
	COLORREF titleTextColor = RGB(255, 255, 255);

	int extraHeaderHeight = 1;
	int titleHeight = 20;

	bool isScrollbarVisible = false;

	HWND hwnd=NULL;
	HWND hListbox = NULL;

	HWND parent=NULL;
	LISTUIDRAWITEMPROC* drawItemProc;
	LISTUISELCHANGE* selChangeProc;
	LISTUIDBLCLK* dblClkProc;
	LISTUIRIGHTCLK* rightClkProc;
	
	string title;
	vector<ListUIColumns*> columns;

};

