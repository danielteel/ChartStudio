#pragma once
#include <string>
bool isSpace(int chr);
bool isDigit(int chr);
bool isAlpha(int chr);
bool isAlNum(int chr);

bool compare_float(double x, double y, double epsilon = 0.000001f);
void drawBitmapStretch(HDC destdc, HBITMAP bitmap, int x, int y, int w, int h);
void drawBitmap(HDC destdc, HBITMAP bitmap, int x, int y, bool center);
void drawBitmapTransparent(HDC destdc, HBITMAP bitmap, int x, int y, bool center, COLORREF transColor);
void drawBitmapStretchTransparent(HDC destdc, HBITMAP bitmap, int x, int y, int w, int h, COLORREF transColor);

std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v");
std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v");
std::string& trim(std::string& s, const char* t = " \t\n\r\f\v");
std::string stringToLowerCopy(std::string str);
void stringToLower(std::string& str);
std::string doubleToString(double value, int decimalPlaces);
std::string trimmedDoubleToString(double value, int mostDecimalPlaces);
std::string trimmedDoubleToString(double value);

std::string encodeString(std::string str);
std::string decodeString(std::string str);

std::string stripTrailingNumber(std::string str);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


void setDlgItemDouble(HWND hwnd, int controlID, double value, int precision);
void setDlgItemDouble(HWND hwnd, int controlID, double value);
double getDlgItemDouble(HWND hwnd, int controlID);
std::string getDlgItemString(HWND hwnd, int controlID);
std::string getWindowString(HWND hwnd);
double getWindowDouble(HWND hwnd);