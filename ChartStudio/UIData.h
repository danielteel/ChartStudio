#pragma once

class ListUI;
class Toolbar;
class DoubleBuff;
class TabControl;
class Canvas;

class UIData {
public:
	UIData();
	~UIData();

	void clearUI();

	int chartTabIndex;

	int objectsTab;
	int inputsTab;
	int imagesTab;


	HWND hwnd;

	HMENU menu;

	Toolbar* toolbar;
	HWND statusbar;

	TabControl* mainTab;
	TabControl* chartsTab;
	Canvas* chartCanvas;
	ListUI* objectsList;
	ListUI* inputsList;
	ListUI* constantsList;
	ListUI* imagesList;
	ListUI* linesList;
	ListUI* tableList;
};

