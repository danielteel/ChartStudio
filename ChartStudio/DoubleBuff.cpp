#include "stdafx.h"
#include "DoubleBuff.h"
#include <math.h>

#pragma float_control(except, off)  // disable exception semantics
#pragma fenv_access(off)            // disable environment sensitivity
#pragma float_control(precise, off) // disable precise semantics
#pragma fp_contract(on)             // enable contractions


//only do in 90 degree increments, need to trouble shoot why it gets fucky inbetween
HBITMAP DoubleBuff::getRotatedBitmap(HBITMAP bitmap, int degrees, COLORREF backGroundColor) {
	if (bitmap == 0) return NULL;

	BITMAP dcBmp;
	GetObject(bitmap, sizeof(BITMAP), &dcBmp);

	double vsin = sin(0.017453*(double)degrees);
	double vcos = cos(0.017453*(double)degrees);

	double x1 = (double)dcBmp.bmHeight * vsin;
	double y1 = (double)dcBmp.bmHeight * vcos;
	double x2 = (double)dcBmp.bmWidth * vcos + (double)dcBmp.bmHeight * vsin;
	double y2 = (double)dcBmp.bmHeight * vcos - (double)dcBmp.bmWidth * vsin;
	double x3 = (double)dcBmp.bmWidth * vcos;
	double y3 = (double)-dcBmp.bmWidth * vsin;

	double minx = min(0, min(x1, min(x2, x3)));
	double miny = min(0, min(y1, min(y2, y3)));
	double maxx = max(0, max(x1, max(x2, x3)));
	double maxy = max(0, max(y1, max(y2, y3)));
	double w = maxx - minx;
	double h = maxy - miny;

	HDC tempDC = CreateCompatibleDC(this->hdc);
	HBITMAP newBmp = CreateCompatibleBitmap(this->hdc, int(w), int(h));
	HBITMAP tempBmp = (HBITMAP)SelectObject(tempDC, newBmp);

	HDC sourceDC = CreateCompatibleDC(this->hdc);
	HBITMAP sourceBmp = (HBITMAP)SelectObject(sourceDC, bitmap);

	RECT fr = { 0,0,int(w + 2),int(h + 2)};
	HBRUSH backColorBrush = CreateSolidBrush(backGroundColor);
	FillRect(tempDC, &fr, backColorBrush);
	DeleteObject(backColorBrush);

	SetGraphicsMode(tempDC, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = float(vcos);
	xform.eM12 = float(-vsin);
	xform.eM21 = float(vsin);
	xform.eM22 = float(vcos);
	xform.eDx = float(-minx);
	xform.eDy = float(-miny);
	SetWorldTransform(tempDC, &xform);

	BitBlt(tempDC, 0, 0, dcBmp.bmWidth, dcBmp.bmHeight, sourceDC, 0, 0, SRCCOPY);
	SetGraphicsMode(tempDC, GM_COMPATIBLE);
	SelectObject(sourceDC, sourceBmp);
	SelectObject(tempDC, tempBmp);
	DeleteDC(sourceDC);
	DeleteDC(tempDC);
	return newBmp;
}


DoubleBuff::DoubleBuff(HWND hwnd) {
	this->scale = 1.0f;
	this->offsetX = 0;
	this->offsetY = 0;

	this->hwnd = hwnd;
	this->w = GetSystemMetrics(SM_CXSCREEN);
	this->h = GetSystemMetrics(SM_CYSCREEN);
	this->hdc = GetDC(hwnd);
	this->memDC = CreateCompatibleDC(this->hdc);
	this->bitmap = CreateCompatibleBitmap(this->hdc, this->w, this->h);
	this->oldBitmap = (HBITMAP)SelectObject(this->memDC, this->bitmap);
	this->oldBrush = (HBRUSH)SelectObject(this->memDC, CreateSolidBrush(0));
	this->oldPen = (HPEN)SelectObject(this->memDC, CreatePen(PS_SOLID, 1, RGB(255, 255, 255)));
	this->oldFont = (HFONT)SelectObject(this->memDC, CreateFontA(12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr));
	this->flip();
}


DoubleBuff::~DoubleBuff() {
	DeleteObject(SelectObject(this->memDC, this->oldBrush));
	DeleteObject(SelectObject(this->memDC, this->oldPen));
	DeleteObject(SelectObject(this->memDC, this->oldFont));
	SelectObject(this->memDC, this->oldBitmap);
	DeleteObject(this->bitmap);
	DeleteDC(this->memDC);
	ReleaseDC(this->hwnd, this->hdc);
}

void DoubleBuff::setLineColor(COLORREF color) {
	this->lineColor = color;
}
void DoubleBuff::setFillColor(COLORREF color) {
	this->fillColor = color;
}

void DoubleBuff::setPen(int style, int size, COLORREF color) {
	DeleteObject(SelectObject(this->memDC, CreatePen(style, size, color)));
}
void DoubleBuff::setBrush(COLORREF color) {
	DeleteObject(SelectObject(this->memDC, CreateSolidBrush(color)));
}

void DoubleBuff::setFont(int height, int weight, bool italic, bool underline, const char * fontName) {
	this->fontHeight = height;
	this->fontWeight = weight;
	this->fontItalic = italic;
	this->fontUnderline = underline;
	if (fontName) {
		this->fontName = fontName;
	} else {
		this->fontName = "";
	}
}

void DoubleBuff::setTextColor(COLORREF textColor, COLORREF bgColor, bool bgTransparent) {
	this->textColor = textColor;
	this->bgColor = bgColor;
	this->bgTransparent = bgTransparent;
}

void DoubleBuff::drawHorizantalArrow(double x, double y, double width, double length, double lineWidth, bool pointsRight, bool leadingIsAtXY, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	double direction = pointsRight ? -1.0f : 1.0f;

	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
	}

	if (drawSize != DRAWSIZE::NONE) {
		lineWidth *= this->scale;
		width *= this->scale;
		length *= this->scale;
	}

	if (!leadingIsAtXY && !pointsRight) {
		x = x - length;
	}
	if (!leadingIsAtXY && pointsRight) {
		x = x + length;
	}
	if (leadingIsAtXY) {
		x = x + direction * (lineWidth / 2);
	} else {
		x = x - direction * (lineWidth / 2);
	}

	this->setPen(PS_SOLID, int(fmax(lround(lineWidth), 1)), this->lineColor);
	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x+length*direction), int(y));
	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x + width * direction), int(y+width*direction));
	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x + width * direction), int(y - width * direction));
}

void DoubleBuff::drawVerticalArrow(double x, double y, double width, double length, double lineWidth, bool pointsUp, bool leadingIsAtXY, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	double direction = pointsUp ? -1.0f : 1.0f;

	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
	}

	if (drawSize != DRAWSIZE::NONE) {
		lineWidth *= this->scale;
		width *= this->scale;
		length *= this->scale;
	}

	if (!leadingIsAtXY && !pointsUp) {
		y = y - length;
	}
	if (!leadingIsAtXY && pointsUp) {
		y = y + length;
	}

	if (leadingIsAtXY) {
		y = y + direction * (lineWidth / 2);
	} else {
		y = y - direction * (lineWidth / 2);
	}

	this->setPen(PS_SOLID, int(fmax(lround(lineWidth), 1)), this->lineColor);
	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x), int(y + length * direction));

	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x + width * direction), int(y + width * direction));

	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x - width * direction), int(y + width * direction));
}

void DoubleBuff::drawHorizantalLine(double y, double lineWidth, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		y = y * this->scale + this->offsetY * this->scale;
	}
	if (drawSize != DRAWSIZE::NONE) {
		lineWidth *= this->scale;
	}
	this->setPen(PS_SOLID, int(fmax(lround(lineWidth), 1)), this->lineColor);
	MoveToEx(this->memDC, 0, int(y), nullptr);
	LineTo(this->memDC, this->w, int(y));
}

void DoubleBuff::drawVerticalLine(double x, double lineWidth, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX * this->scale;
	}
	if (drawSize != DRAWSIZE::NONE) {
		lineWidth *= this->scale;
	}
	this->setPen(PS_SOLID, int(fmax(lround(lineWidth), 1)), this->lineColor);
	MoveToEx(this->memDC, int(x), 0, nullptr);
	LineTo(this->memDC, int(x), this->h);
}

void DoubleBuff::drawLine(double x, double y, double x2, double y2, double lineWidth, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
		x2 = x2 * this->scale + this->offsetX*this->scale;
		y2 = y2 * this->scale + this->offsetY*this->scale;
	}
	if (drawSize != DRAWSIZE::NONE) {
		lineWidth *= this->scale;
	}
	
	this->setPen(PS_SOLID, int(fmax(lround(lineWidth),1)), this->lineColor);
	MoveToEx(this->memDC, int(x), int(y), nullptr);
	LineTo(this->memDC, int(x2), int(y2));
}

void DoubleBuff::drawEllipse(double x, double y, double w, double h, double lineWidth, DRAWPOSITION drawPos, DRAWSIZE drawSize, DRAWXALIGN drawXAlign, DRAWYALIGN drawYAlign) {
	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
	}
	if (drawSize == DRAWSIZE::SCALEENTIRE) {
		lineWidth *= this->scale;
		w *= this->scale;
		h *= this->scale;
	} else if (drawSize == DRAWSIZE::SCALEFILLONLY) {
		w *= this->scale;
		h *= this->scale;
	}

	if (drawXAlign == DRAWXALIGN::CENTER) {
		x -= w / 2;
	} else if (drawXAlign == DRAWXALIGN::RIGHT) {
		x -= w;
	}
	if (drawYAlign == DRAWYALIGN::CENTER) {
		y -= h / 2;
	} else if (drawYAlign == DRAWYALIGN::BOTTOM) {
		y -= h;
	}

	this->setBrush(this->fillColor);
	this->setPen(PS_SOLID, int(fmax(lround(lineWidth), 1)), this->lineColor);

	Ellipse(this->memDC, int(x), int(y), int(x+w), int(y+h));
}

void DoubleBuff::drawCircle(double x, double y, double diameter, double lineWidth, DRAWPOSITION drawPos, DRAWSIZE drawSize, DRAWXALIGN drawXAlign, DRAWYALIGN drawYAlign) {
	this->drawEllipse(x, y, diameter, diameter, lineWidth, drawPos, drawSize, drawXAlign, drawYAlign);
}

void DoubleBuff::drawRect(double x, double y, double x2, double y2, double lineWidth, bool fill, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
		x2 = x2 * this->scale + this->offsetX*this->scale;
		y2 = y2 * this->scale + this->offsetY*this->scale;
	}
	if (drawSize != DRAWSIZE::NONE) {
		lineWidth *= this->scale;
	}
	this->setPen(PS_SOLID, int(fmax(lround(lineWidth), 1)), this->lineColor);
	if (fill) {
		this->setBrush(this->fillColor);
		Rectangle(this->memDC, int(x), int(y), int(x2), int(y2));
	} else {
		MoveToEx(this->memDC, int(x), int(y), nullptr);
		LineTo(this->memDC, int(x2), int(y));
		LineTo(this->memDC, int(x2), int(y2));
		LineTo(this->memDC, int(x), int(y2));
		LineTo(this->memDC, int(x), int(y));
	}

}

void DoubleBuff::drawRect(TRect rect, double lineWidth, bool fill, DRAWPOSITION drawPos, DRAWSIZE drawSize) {
	drawRect(rect.point1.x, rect.point1.y, rect.point2.x, rect.point2.y, lineWidth, fill, drawPos, drawSize);
}

void DoubleBuff::drawText(string text, double x, double y, double* out_width, double* out_height, DRAWPOSITION drawPos, DRAWSIZE drawSize, DRAWXALIGN drawXAlign, DRAWYALIGN drawYAlign) {
	RECT drawRect;
	double drawFontHeight = this->fontHeight;

	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
	}
	if (drawSize != DRAWSIZE::NONE) {
		drawFontHeight = drawFontHeight * this->scale;
	}
	DeleteObject(SelectObject(this->memDC, CreateFontA(int(drawFontHeight), 0, 0, 0, this->fontWeight, this->fontItalic, this->fontUnderline, false,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, this->fontName.c_str())));
	
	SetTextColor(this->memDC, this->textColor);
	SetBkMode(this->memDC, this->bgTransparent ? TRANSPARENT : OPAQUE);
	SetBkColor(this->memDC, this->bgColor);

	drawRect.left = long(x);
	drawRect.top = long(y);
	drawRect.right = long(x);
	drawRect.bottom = long(y);

	DrawTextA(this->memDC, text.c_str(), -1, &drawRect, DT_CALCRECT);
	int w = drawRect.right - drawRect.left;
	int h = drawRect.bottom - drawRect.top;

	if (out_width) {
		*out_width = w;
		if (drawSize != DRAWSIZE::NONE) *out_width /= this->scale;
	}
	if (out_height) {
		*out_height = h;
		if (drawSize != DRAWSIZE::NONE) *out_height /= this->scale;
	}
	if (drawXAlign == DRAWXALIGN::CENTER) {
		drawRect.left -= w / 2;
		drawRect.right -= w / 2;
	} else if (drawXAlign == DRAWXALIGN::RIGHT) {
		drawRect.left -= w;
		drawRect.right -= w;
	}
	if (drawYAlign == DRAWYALIGN::CENTER) {
		drawRect.top -= h / 2;
		drawRect.bottom -= h / 2;
	} else if (drawYAlign == DRAWYALIGN::BOTTOM) {
		drawRect.top -= h;
		drawRect.bottom -= h;
	}
	DrawTextA(this->memDC, text.c_str(), -1, &drawRect, DT_LEFT);
}

void DoubleBuff::drawBitmap(HBITMAP bitmap, double x, double y, DRAWPOSITION drawPos, DRAWSIZE drawSize, DRAWXALIGN drawXAlign, DRAWYALIGN drawYAlign) {
	BITMAP bmp;
	GetObject(bitmap, sizeof(BITMAP), &bmp);

	double drawW = bmp.bmWidth;
	double drawH = bmp.bmHeight;
	if (drawPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
	}
	if (drawSize != DRAWSIZE::NONE) {
		drawW = drawW * this->scale;
		drawH = drawH * this->scale;
	}
	if (drawXAlign == DRAWXALIGN::CENTER) {
		x -= drawW / 2;
	} else if (drawXAlign == DRAWXALIGN::RIGHT) {
		x -= drawW;
	}
	if (drawYAlign == DRAWYALIGN::CENTER) {
		y -= drawH / 2;
	} else if (drawYAlign == DRAWYALIGN::BOTTOM) {
		y -= drawH;
	}

	if (x  > this->w || y  > this->h) {
		return;
	}
	if (x + drawW < 0 || y + drawH < 0) {
		return;
	}

	HDC tempDC = CreateCompatibleDC(this->hdc);
	HBITMAP oldBmp = (HBITMAP)SelectObject(tempDC, bitmap);

	if (this->smoothBitmap) {
		SetStretchBltMode(this->memDC, HALFTONE);
		SetBrushOrgEx(this->memDC, 0, 0, NULL);
	} else {
		SetStretchBltMode(this->memDC, STRETCH_DELETESCANS);
	}

	StretchBlt(this->memDC, int(x), int(y), int(drawW), int(drawH), tempDC, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);

	SelectObject(tempDC, oldBmp);
	DeleteDC(tempDC);
}

void DoubleBuff::setClippingRect(double x, double y, double x2, double y2, DRAWPOSITION rectPos) {
	if (rectPos == DRAWPOSITION::SCALEANDOFFSET) {
		x = x * this->scale + this->offsetX*this->scale;
		y = y * this->scale + this->offsetY*this->scale;
		x2 = x2 * this->scale + this->offsetX*this->scale;
		y2 = y2 * this->scale + this->offsetY*this->scale;
	}
	HRGN clipRgn = CreateRectRgn(int(x), int(y), int(x2), int(y2));
	if (clipRgn) {
		SelectClipRgn(this->memDC, clipRgn);
		DeleteObject(clipRgn);
	}
}
void DoubleBuff::clearClippingRect() {
	SelectClipRgn(this->memDC, NULL);
}

void DoubleBuff::clear(COLORREF color) {
	this->setBrush(color);
	this->setPen(PS_SOLID, 1, color);
	Rectangle(this->memDC, 0, 0, this->w, this->h);
}

void DoubleBuff::flip(const RECT* clipRect) {
	RECT winRect;
	HRGN clipRegion = NULL;
	GetClientRect(this->hwnd, &winRect);
	if (clipRect) {
		clipRegion=CreateRectRgn(clipRect->left, clipRect->top, clipRect->right, clipRect->bottom);
		SelectClipRgn(this->hdc, clipRegion);
		DeleteObject(clipRegion);
	}
	BitBlt(this->hdc, 0, 0, winRect.right, winRect.bottom, this->memDC, 0, 0, SRCCOPY);
	if (clipRect) {
		SelectClipRgn(this->hdc, NULL);
	}
}

void DoubleBuff::setScale(double scale) {
	this->scale=scale;
}

double DoubleBuff::getScale() {
	return this->scale;
}

void DoubleBuff::setOffset(double x, double y) {
	this->offsetX = x;
	this->offsetY = y;
}

void DoubleBuff::getOffset(double & x, double & y) {
	x = this->offsetX;
	y = this->offsetY;
}
