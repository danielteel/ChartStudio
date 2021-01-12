#include "stdafx.h"
#include "TPoint.h"
#include "TRect.h"
#include "ChartImage.h"
#include "helpers.h"


TRect ChartImage::getRect() {
	return TRect(position, position + size);
}

ChartImage::ChartImage(string name, HBITMAP bitmap, TPoint position) {
	this->position = position;
	this->bitmap = NULL;
	this->setHBitmap(bitmap);
	this->name = name;
}

ChartImage::~ChartImage() {
	DeleteObject(bitmap);
}

TPoint ChartImage::getPosition() {
	return position;
}

TPoint ChartImage::getSize() {
	return size;
}

void ChartImage::move(TPoint newXY, TPoint newSize) {
	this->position = newXY;
	this->size = newSize;
}

void ChartImage::setHBitmap(HBITMAP newBitmap) {
	if (newBitmap) {
		if (this->bitmap) {
			DeleteObject(this->bitmap);
			this->bitmap = NULL;
		}
		this->bitmap = newBitmap;
		BITMAP bmp;
		GetObject(newBitmap, sizeof(BITMAP), &bmp);
		this->size.x = (double)bmp.bmWidth;
		this->size.y = (double)bmp.bmHeight;
	}
}

HBITMAP ChartImage::getHBitmap() {
	return this->bitmap;
}

bool ChartImage::isLess(const ChartImage* than) {
	if (!than) return false;
	return _stricmp(this->name.c_str(), than->name.c_str()) > 0;
}

string ChartImage::toString(char** imageData, size_t* imageDataLen, ULONG quality) {
	string retString = "image('" + encodeString(this->name) + "'," + trimmedDoubleToString(position.x) + "," + trimmedDoubleToString(position.y) + ",";
	retString += trimmedDoubleToString(size.x) + "," + trimmedDoubleToString(size.y) + ", ";
	*imageData = nullptr;
	*imageDataLen = 0;
	char tempPath[MAX_PATH + 1];
	if (GetTempPathA(sizeof(tempPath), tempPath)) {

		char tempFileName[MAX_PATH + 1];
		if (GetTempFileNameA(tempPath, "img", 0, tempFileName)) {
			string tempFileString = tempFileName;

			Gdiplus::Bitmap* chartImage = new Gdiplus::Bitmap(this->bitmap, NULL);
			if (chartImage) {
				//TODO determine what encoder I want to use. A Jpeg at 100% quality is half the size of the lossless png format.
				CLSID pngClsid;
				Gdiplus::EncoderParameters encoderParameters;
				encoderParameters.Count = 1;
				encoderParameters.Parameter[0].Guid = Gdiplus::EncoderQuality;
				encoderParameters.Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
				encoderParameters.Parameter[0].NumberOfValues = 1;
				encoderParameters.Parameter[0].Value = &quality;

				if (GetEncoderClsid(L"image/jpeg", &pngClsid) != -1) {

					wstring file = wstring(tempFileString.begin(), tempFileString.end());

					if (chartImage->Save(file.c_str(), &pngClsid, &encoderParameters) == Gdiplus::Ok) {

						FILE* fImgIn = NULL;

						if (!fopen_s(&fImgIn, tempFileString.c_str(), "rb")) {
							fseek(fImgIn, 0, SEEK_END);
							*imageDataLen = ftell(fImgIn);
							fseek(fImgIn, 0, SEEK_SET);

							*imageData = new char[*imageDataLen + 1];
							if (*imageData) {
								fread(*imageData, *imageDataLen, 1, fImgIn);
							} else {
								*imageDataLen = 0;
							}
							fclose(fImgIn);
							remove(tempFileString.c_str());
						}
					}
				}
				delete chartImage;
			}
		}
	}

	retString += to_string(*imageDataLen) + ":";
	return retString;
}