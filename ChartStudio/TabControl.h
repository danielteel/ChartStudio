#pragma once
#include <vector>
#include <string>
#include <windows.h>
using namespace std;

struct TabControlChild {
	int parentTab;
	HWND childHWND;
};


typedef void (TABCONTROLCHANGE)(int);

class TabControl {
public:
	TabControl(HWND parent, int x, int y, int w, int h, TABCONTROLCHANGE* onTabSwitch);

	static void Initialize();
	static LRESULT CALLBACK Subclassproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static bool IsInitialized;
	static const char ClassName[];
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	int addTab(string title);
	void addChildToTab(HWND hwnd, int tabIndex);
	void updateChildren();
	void move(int x, int y, int w, int h);
	RECT getRect();
	~TabControl();

	int currentTab();

	HWND getHWnd() {
		return this->tabHWND;
	}

private:
	HWND tabHWND = NULL;
	HWND containerHWND = NULL;
	int tabCount = 0;
	vector<TabControlChild*> children;

	TABCONTROLCHANGE* onTabSwitch = nullptr;
};

