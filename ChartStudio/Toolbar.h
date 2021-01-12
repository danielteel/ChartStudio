#pragma once
#include <vector>

class Toolbar {
public:
	Toolbar(HWND hParent, const char* toolbarImage, int bitmapItemWH, int numOfBitmaps);
	int getHeight();
	void addButton(int bitmapIndex, int id, const char * tooltipText);
	void addSeperator(int sepWidth);
	void setEnabled(int id, bool enabled);
	void autosize();
	~Toolbar();

	HWND hToolbar = NULL;
	HWND hParent = NULL;

private:
	HANDLE toolbarBitmap = NULL;

};

