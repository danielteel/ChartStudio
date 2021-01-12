#pragma once
#include <Windows.h>
#include <Richedit.h>
#include <string>

#define NELEMS(a)  (sizeof(a) / sizeof((a)[0]))

#define RICH_UNDO 100
#define RICH_REDO 101
#define RICH_CUT  102
#define RICH_COPY 103
#define RICH_PASTE 104
#define RICH_SELECTALL 105

using namespace std;

enum class COLORSTATE {
	CSNONE,
	CSSKIP,
	CSNUM,
	CSIDENT,
	CSSTRING,
	CSCOMMENT,
	CSOP1,
	CSOP2,
};


class SyntaxColoredEditor {
public:
	SyntaxColoredEditor(HWND parent, int x, int y, int w, int h);
	~SyntaxColoredEditor();

	void move(int x, int y, int w, int h);

	bool getModified();
	void setModified(bool flag);

	void setText(string text);

	string getText();

	void setFont(int fontSize, string faceName);
	void setLineNumWidth(int width);
	void setBackColor(COLORREF backcolor);
	void setTextColor(COLORREF textColor);

	char* getRange(unsigned int start, unsigned int end, int * textLen);

	HWND getHWnd() {
		return this->hwnd;
	}

	static bool IsInitialized;
	static LRESULT CALLBACK RichProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	static int fontTwipsToPixelHeight(int fontHeightTwips);
	static int isOperator(char chr);
	static int isOperator2(char chr);
	static int selHitTest(CHARRANGE selRange, int firstChar, int len);
	static void initDictionary();
	static const char* WORDDEF[];
	static const int WORDMAX;
	static int WORDLEN[];
	static const char* FUNCDEF[];
	static const int FUNCMAX;
	static int FUNCLEN[];
	static const char* VARDEF[];
	static const int VARMAX;
	static int VARLEN[];

	void drawText(HDC dc, char * text, int len, int firstChar, CHARRANGE selRange, int fontHeight);
	void drawWithoutTabs(HDC dc, char * text, int len, int lineFirstChar, int offset, CHARRANGE selRange, int fontHeight);
	void updateLineNumbers();


	HWND hwnd = NULL;
	HFONT font = NULL;
	WNDPROC wndProc = nullptr;
	HDC lineNumberDC = NULL;
	HBITMAP lineNumberBitmap = NULL;

	bool findDialogOpen = false;
	HWND findDialogHWnd = NULL;

	int lineNumberWidth = 0;

	COLORREF textColor = RGB(0, 0, 0);
	COLORREF backgroundColor = RGB(255, 255, 255);
	COLORREF lineNumberBkColor = RGB(230, 230, 230);
	COLORREF lineNumberTxtColor = RGB(40,40,40);
	COLORREF lineNumberCurrentColor = RGB(0,0,0);
	COLORREF lineNumberCurrentBkColor = RGB(220,220,220);

	COLORREF commandColor = RGB(0, 0, 255);
	COLORREF functionColor = RGB(180, 20, 180);
	COLORREF variableColor = RGB(20, 75, 20);
	COLORREF userVariableColor = RGB(0, 96, 128);
	COLORREF stringColor = RGB(70, 130, 70);
	COLORREF numberColor = RGB(230,0,0);
	COLORREF commentColor = RGB(90, 90, 100);
	COLORREF operator1Color = RGB(0, 0, 230);
	COLORREF operator2Color = RGB(180, 20, 180);
};

