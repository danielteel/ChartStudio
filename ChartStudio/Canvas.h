#pragma once

class DoubleBuff;
class Canvas;

typedef void (CANVASMOUSEEVENT)(POINT xy);
typedef void (CANVASMOUSEWHEELEVENT)(POINT xy, int amount);
typedef void (CANVASPAINT)(Canvas*);

class Canvas {
public:
	Canvas(HWND hwnd, int x, int y, int w, int h);
	~Canvas();

	void move(int x, int y, int w, int h);

	static POINT getMousePoint(HWND hwnd);

	static const char* ClassName;
	static bool SetupComplete;
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CANVASPAINT* beforePaint = nullptr;

	CANVASMOUSEEVENT* leftDown = nullptr;
	CANVASMOUSEEVENT* leftMove = nullptr;
	CANVASMOUSEEVENT* leftUp = nullptr;
	CANVASMOUSEEVENT* rightDown = nullptr;
	CANVASMOUSEEVENT* rightMove = nullptr;
	CANVASMOUSEEVENT* rightUp = nullptr;
	CANVASMOUSEEVENT* mouseMove = nullptr;

	CANVASMOUSEWHEELEVENT* mouseWheel = nullptr;

	int width();
	int height();


	DoubleBuff* db = nullptr;

	HWND hwnd = NULL;
};

