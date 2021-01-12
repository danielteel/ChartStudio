#pragma once
#include "stdafx.h"
#include "TRect.h"
#include <string>
using namespace std;

enum class DRAWPOSITION {
	NONE,
	SCALEANDOFFSET,
};

enum class DRAWSIZE {
	NONE,
	SCALEENTIRE,
	SCALEFILLONLY
};

enum class DRAWXALIGN {
	CENTER,
	LEFT,
	RIGHT
};

enum class DRAWYALIGN {
	CENTER,
	TOP,
	BOTTOM
};

class DoubleBuff {
public:
	DoubleBuff(HWND hwnd);
	~DoubleBuff();

	void setLineColor(COLORREF color);
	void setFillColor(COLORREF color);
	void setFont(int height, int weight, bool italic, bool underline, const char* fontName);
	void setTextColor(COLORREF textCOlor, COLORREF bgColor, bool bgTransparent);

	void drawHorizantalArrow(double x, double y, double width, double length, double lineWidth, bool pointsRight, bool leadingIsAtXY, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE);
	void drawVerticalArrow(double x, double y, double width, double length, double lineWidth, bool pointsUp, bool leadingIsAtXY, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE);

	void drawHorizantalLine(double y, double lineWidth, DRAWPOSITION drawPos=DRAWPOSITION::NONE, DRAWSIZE drawSize=DRAWSIZE::NONE);
	void drawVerticalLine(double x, double lineWidth, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE);
	void drawLine(double x, double y, double x2, double y2, double lineWidth, DRAWPOSITION drawPos=DRAWPOSITION::NONE, DRAWSIZE drawSize=DRAWSIZE::NONE);
	void drawEllipse(double x, double y, double w, double h, double thickness, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE, DRAWXALIGN drawXAlign = DRAWXALIGN::LEFT, DRAWYALIGN drawYAlign = DRAWYALIGN::TOP);
	void drawCircle(double x, double y, double radius, double lineWidth, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE, DRAWXALIGN drawXAlign = DRAWXALIGN::LEFT, DRAWYALIGN drawYAlign = DRAWYALIGN::TOP);
	void drawRect(double x, double y, double x2, double y2, double lineWidth, bool fill, DRAWPOSITION drawPos, DRAWSIZE drawSize);
	void drawRect(TRect rect, double lineWidth, bool fill, DRAWPOSITION drawPos, DRAWSIZE drawSize);
	void drawText(string text, double x, double y, double* out_width, double* out_height, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE, DRAWXALIGN drawXAlign = DRAWXALIGN::LEFT, DRAWYALIGN drawYAlign = DRAWYALIGN::TOP);
	void drawBitmap(HBITMAP bitmap, double x, double y, DRAWPOSITION drawPos = DRAWPOSITION::NONE, DRAWSIZE drawSize = DRAWSIZE::NONE, DRAWXALIGN drawXAlign = DRAWXALIGN::LEFT, DRAWYALIGN drawYAlign = DRAWYALIGN::TOP);

	void setClippingRect(double x, double y, double x2, double y2, DRAWPOSITION rectPos = DRAWPOSITION::NONE);
	void clearClippingRect();

	void clear(COLORREF color);

	void flip(const RECT* clipRect=nullptr);
	HBITMAP getRotatedBitmap(HBITMAP bitmap, int degrees, COLORREF backGroundColor);

	void setScale(double scale);
	double getScale();

	void setOffset(double x, double y);
	void getOffset(double& x, double& y);

	HWND hwnd;
	HDC hdc;
	HDC memDC;
	int w;
	int h;

	bool smoothBitmap = false;

private:
	void setPen(int style, int size, COLORREF color);
	void setBrush(COLORREF color);
	HBITMAP oldBitmap;
	HBITMAP bitmap;
	HBRUSH oldBrush;
	HPEN oldPen;
	HFONT oldFont;


	int fontHeight=12;
	int fontWeight=FW_NORMAL;
	bool fontItalic=false;
	bool fontUnderline=false;
	string fontName="Segoe UI";
	COLORREF textColor = RGB(0, 0, 0);
	COLORREF bgColor = RGB(255, 255, 255);
	bool bgTransparent = false;

	COLORREF lineColor = RGB(0, 0, 0);
	COLORREF fillColor = RGB(128, 128, 128);

	double scale;
	double offsetX;
	double offsetY;
};

