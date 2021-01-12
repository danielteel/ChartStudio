#pragma once
#include <Windows.h>
#include <vector>
using namespace std;
class ButtonBar {
public:
	ButtonBar(HWND parent, int x, int y, int w, int seperatorWidth, COLORREF seperatorColor);
	~ButtonBar();

	int seperators = 0;
	int seperatorWidth = 2;
	COLORREF seperatorColor = RGB(0, 0, 0);
	int largestImageHeight;
	vector<HWND> buttons;
	vector<int> buttonIDs;
	vector<HBITMAP> bitmaps;
	HWND parent;
	HWND backing;

	void move(int x, int y, int w);
	void updateButtonSizes();
	void addButton(int bitmapResourceID, int buttonID, const char* toolTipText);
	void addButton(HBITMAP bitmap, int buttonID, const char* toolTipText);
	void addSeperator();
	int getHeight();
	void enableButton(int buttonID, bool enabled);
private:
	int x, y, w, h;


	static bool SetupComplete;
	static vector<HWND> StaticButtonArray;
	static int messageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK Subclassproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static void addButtonToStaticArray(HWND button);
	static void removeButtonFromStaticArray(HWND button);
};

