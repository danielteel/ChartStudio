// ChartStudio.cpp : Defines the entry point for the application.
//
#include <string>
#include "stdafx.h"
#include "ProjectContainer.h"
#include "ChartProject.h"
#include "ChartStudio.h"
#include "dialogs.h"
#include "Canvas.h"
#include "CChartObject.h"
#include "ButtonBar.h"
#include "TabControl.h"
#include "ListUI.h"
#include "Toolbar.h"
#include "UIData.h"
#include "CInput.h"
#include "TLine.h"
#include "TPoint.h"
#include "CConstant.h"
#include "CChartBase.h"
#include "CTrendChart.h"
#include "CPolyChart.h"
#include "CClampChart.h"
#include "CLinearChart.h"
#include "CTable.h"
#include "CScript.h"
#include "ChartImage.h"
#include "helpers.h"

#include "Tokenizer.h"
#include "Parser.h"

HINSTANCE hInst;
char szTitle[] = "Chart Studio";
char szWindowClass[] = "ChartStudioClass";

ProjectContainer* gProject = nullptr;

HBITMAP gExportBitmap = NULL;
HBITMAP gLinearBitmap = NULL;
HBITMAP gTrendBitmap = NULL;
HBITMAP gClampBitmap = NULL;
HBITMAP gPolyBitmap = NULL;
HBITMAP gTableBitmap = NULL;
HBITMAP gScriptBitmap = NULL;
HBITMAP gBadBitmap = NULL;

HMENU gChartBoxMenu = NULL;
HMENU gTableBoxMenu = NULL;
HMENU gImagesBoxMenu = NULL;
HMENU gLinesBoxMenu = NULL;
HMENU gInputsBoxMenu = NULL;
HMENU gConstantsBoxMenu = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow){
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	Tokenizer tokenizer;
	try {
		tokenizer.tokenize("double a=0.5;\n string b='dude';");
	} catch (char e) {
		MessageBox(NULL, tokenizer.errorMsg.c_str(), tokenizer.errorMsg.c_str(), 0);
	}
	Parser parser(tokenizer.tokens);
	try {
		parser.parse({});
	} catch (char e) {
		MessageBox(NULL, parser.errorMsg.c_str(), parser.errorMsg.c_str(), 0);
	}

	hInst = hInstance; // Store instance handle in our global variable

	//Init GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	gLinearBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_LINEAR), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gTrendBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_TREND), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gClampBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_CLAMP), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gPolyBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_POLY), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gScriptBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_SCRIPT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gTableBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_TABLE), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gBadBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_DELETE), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	gExportBitmap = (HBITMAP)LoadImageA(hInst, MAKEINTRESOURCE(IDB_EXPORT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

	gChartBoxMenu = LoadMenuA(hInst, MAKEINTRESOURCE(IDR_CHARTBOXMENU));
	gTableBoxMenu = LoadMenuA(hInst, MAKEINTRESOURCE(IDR_TABLEBOXMENU));
	gImagesBoxMenu = LoadMenuA(hInst, MAKEINTRESOURCE(IDR_IMAGESBOXMENU));
	gLinesBoxMenu = LoadMenuA(hInst, MAKEINTRESOURCE(IDR_LINESBOXMENU));
	gInputsBoxMenu = LoadMenuA(hInst, MAKEINTRESOURCE(IDR_INPUTMENU));
	gConstantsBoxMenu = LoadMenuA(hInst, MAKEINTRESOURCE(IDR_CONSTANTMENU));

    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)){
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHARTSTUDIO));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)){
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	//Shut GDI+ down
	Gdiplus::GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance){
    WNDCLASSEXA wcex;
    wcex.cbSize = sizeof(WNDCLASSEXA);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHARTSTUDIO));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEA(IDC_CHARTSTUDIO);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExA(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow){
   HWND hWnd = CreateWindowA(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd){
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void chartLeftDown(POINT xy) {
	if (gProject) {
		int currentTab = gProject->ui->chartsTab->currentTab();

		if (currentTab == gProject->ui->objectsTab) {
			gProject->chartLButtonDown(xy);
		} else if (currentTab == gProject->ui->imagesTab) {
			gProject->chartImagesLButtonDown(xy);
		}
	}
}
void chartLeftMove(POINT xy) {
	if (gProject) {
		int currentTab = gProject->ui->chartsTab->currentTab();

		if (currentTab == gProject->ui->objectsTab) {
			gProject->chartLButtonMove(xy);
		} else if (currentTab == gProject->ui->imagesTab) {
			gProject->chartImagesLButtonMove(xy);
		}
	}
}
void chartLeftUp(POINT xy) {
	if (gProject) {
		int currentTab = gProject->ui->chartsTab->currentTab();

		if (currentTab == gProject->ui->objectsTab) {
			gProject->chartLButtonUp(xy);
		} else if (currentTab == gProject->ui->imagesTab) {
			gProject->chartImagesLButtonUp(xy);
		}
	}
}
void chartRightDown(POINT xy) {
	if (gProject) gProject->chartRButtonDown(xy);
}
void chartRightMove(POINT xy) {
	if (gProject) gProject->chartRButtonMove(xy);
}
void chartRightUp(POINT xy) {
	if (gProject) gProject->chartRButtonUp(xy);
}
void chartMouseMove(POINT xy) {
	if (gProject) gProject->chartMouseMove(xy);
}
void chartMouseWheel(POINT xy, int amount) {
	if (gProject) gProject->chartMouseWheel(xy, amount);
}

void uiLeftDown(POINT xy) {
	if (gProject) gProject->uiLButtonDown(xy);
}
void uiLeftMove(POINT xy) {
	if (gProject) gProject->uiLButtonMove(xy);
}
void uiLeftUp(POINT xy) {
	if (gProject) gProject->uiLButtonUp(xy);
}
void uiRightDown(POINT xy) {
	if (gProject) gProject->uiRButtonDown(xy);
}
void uiRightMove(POINT xy) {
	if (gProject) gProject->uiRButtonMove(xy);
}
void uiRightUp(POINT xy) {
	if (gProject) gProject->uiRButtonUp(xy);
}
void uiMouseMove(POINT xy) {
	if (gProject) gProject->uiMouseMove(xy);
}
void uiMouseWheel(POINT xy, int amount) {
	if (gProject) gProject->uiMouseWheel(xy, amount);
}


void drawInputsBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!pdis) return;

	CInput* input = reinterpret_cast<CInput*>(pdis->itemData);
	RECT drawRect = pdis->rcItem;
	if (input) {
		switch (column) {
		case 0:
			DrawTextA(pdis->hDC, to_string(pdis->itemID).c_str(), -1, &pdis->rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			break;

		case 1:
			{
				string whyItsBad;
				int bitmapSize = drawRect.bottom - drawRect.top;
				drawRect.left += 4;
				if (gProject) {
					if (input->checkIsBad(gProject->chartProject, whyItsBad)) {
						drawBitmapStretchTransparent(pdis->hDC, gBadBitmap, drawRect.left, drawRect.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
						drawRect.left += bitmapSize + 2;
					}
				}
				DrawTextA(pdis->hDC, input->getName().c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			}
			break;
		}
	}
}
void drawConstantsBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!pdis) return;

	CConstant* constant = reinterpret_cast<CConstant*>(pdis->itemData);
	RECT drawRect = pdis->rcItem;
	if (constant) {
		switch (column) {
		case 0:
			drawRect.left += 5;
			DrawTextA(pdis->hDC, constant->getName().c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			break;
		case 1:
			{
				string displayString = "nil";
				if (constant->result) {
					displayString = trimmedDoubleToString(*constant->result);
				}
				DrawTextA(pdis->hDC, displayString.c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			}
			break;
		}
	}
}

void drawImagesBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!pdis) return;

	ChartImage* image = reinterpret_cast<ChartImage*>(pdis->itemData);
	RECT drawRect = pdis->rcItem;
	if (image) {
		switch (column) {
		case 0:
			{
				drawRect.left += 5;
				DrawTextA(pdis->hDC, image->name.c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			}
			break;
		}
	}
}

void drawChartsBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!pdis) return;

	switch (column) {
	case 0:
		DrawTextA(pdis->hDC, to_string(pdis->itemID).c_str(), -1, &pdis->rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		break;
	case 1:
		{
			CChartObject* chartObject = reinterpret_cast<CChartObject*>(pdis->itemData);
			if (chartObject) {
				CLinearChart* linearChart = chartObject->toLinearChart();
				CTrendChart* trendChart = chartObject->toTrendChart();
				CClampChart* clampChart = chartObject->toClampChart();
				CPolyChart* polyChart = chartObject->toPolyChart();

				CTable* tableObject = chartObject->toTable();
				CScript* scriptObject = chartObject->toScript();

				HBITMAP bitmapToDraw = NULL;
				if (linearChart) {
					bitmapToDraw = gLinearBitmap;
				} else if (trendChart) {
					bitmapToDraw = gTrendBitmap;
				} else if (clampChart) {
					bitmapToDraw = gClampBitmap;
				} else if (polyChart) {
					bitmapToDraw = gPolyBitmap;
				} else if (tableObject) {
					bitmapToDraw = gTableBitmap;
				} else if (scriptObject) {
					bitmapToDraw = gScriptBitmap;
				}
				int bitmapSize = 0;
				if (bitmapToDraw) {
					bitmapSize = pdis->rcItem.bottom - pdis->rcItem.top;
					drawBitmapStretchTransparent(pdis->hDC, bitmapToDraw, pdis->rcItem.left, pdis->rcItem.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
					pdis->rcItem.left += bitmapSize + 2;
				}

				string whyItsBad;
				if (gProject) {
					if (chartObject->checkIsBad(gProject->chartProject, whyItsBad)) {
						drawBitmapStretchTransparent(pdis->hDC, gBadBitmap, pdis->rcItem.left, pdis->rcItem.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
						pdis->rcItem.left += bitmapSize + 2;
					}
				}

				RECT textRect = pdis->rcItem;
				DrawTextA(pdis->hDC, chartObject->getName().c_str(), -1, &textRect, DT_VCENTER | DT_SINGLELINE);

				if (chartObject->exportResult) {
					drawBitmapStretchTransparent(pdis->hDC, gExportBitmap, pdis->rcItem.right - bitmapSize - 1, pdis->rcItem.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
				}
			}

		}
		break;
	}
}


void drawLinesBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!gProject) return;
	if (!gProject->selectedObject) return;

	CChartBase* chartBase = gProject->selectedObject->toChartBase();


	TLine* lineItem = reinterpret_cast<TLine*>(pdis->itemData);
	if (lineItem) {
		CTrendChart* trendChart = chartBase->toTrendChart();
		CLinearChart* linearChart = chartBase->toLinearChart();
		CPolyChart* polyChart = chartBase->toPolyChart();
		CClampChart* clampChart = chartBase->toClampChart();

		RECT drawRect = pdis->rcItem;
		int height = drawRect.bottom - drawRect.top;
		string displayString = "";

		switch (column) {
		case 0:
			if (gProject->chartProject->isLineBad(chartBase, lineItem)) {
				drawBitmapStretchTransparent(pdis->hDC, gBadBitmap, drawRect.left + 1, drawRect.top + 1, height - 2, height - 2, RGB(255, 255, 255));
			}
			drawRect.left += height + 1;
			if (linearChart || polyChart) {
				displayString = trimmedDoubleToString(chartBase->getLineValue(lineItem));
			} else if (clampChart) {
				if (clampChart->maxLine == lineItem) {
					displayString = "Max Line";
				} else {
					displayString = "Min Line";
				}
			} else if (trendChart) {
			}
			DrawTextA(pdis->hDC, displayString.c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			break;
		case 1:
			DrawTextA(pdis->hDC, to_string(lineItem->points.size()).c_str(), -1, &drawRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			break;
		}
	}
}

void drawTableBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!gProject) return;
	if (!gProject->selectedObject) return;
	CTableEntry* entry = reinterpret_cast<CTableEntry*>(pdis->itemData);
	RECT drawRect = pdis->rcItem;
	if (entry) {
		string displayString;
		switch (column) {
		case 0:
			{
				int bitmapSize = drawRect.bottom - drawRect.top;
				drawRect.left += 4;
				if (entry->chartObject == nullptr) {
					drawBitmapStretchTransparent(pdis->hDC, gBadBitmap, drawRect.left, drawRect.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
					drawRect.left += bitmapSize + 2;
				}

				if (entry->chartObject) {
					displayString = entry->chartObject->getName();
				} else {
					displayString = "<empty>";
				}
				DrawTextA(pdis->hDC, displayString.c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			}
			break;
		case 1:
			displayString = trimmedDoubleToString(entry->getValue());
			DrawTextA(pdis->hDC, displayString.c_str(), -1, &drawRect, DT_SINGLELINE | DT_VCENTER);
			break;
		}
	}
}


void selChangeChartsBox(char* item, int id) {
	if (gProject) {
		gProject->objectListChangeSel((CChartObject*)item);
	}
}

void dblClickChartsBox(char* item, int id) {
	if (item) {
		SendMessage(gProject->ui->hwnd, WM_COMMAND, ID_OBJ_EDIT, 0);
	}
}

void rightClickChartsBox(char* item, int id) {
	if (!gProject || !gProject->chartProject || !gProject->ui || id == -1) return;

	CChartObject* activeObject = reinterpret_cast<CChartObject*>(item);
	if (activeObject == nullptr) return;

	POINT mouse;
	GetCursorPos(&mouse);

	HMENU subMenu = GetSubMenu(gChartBoxMenu, 0);

	int menuCount = GetMenuItemCount(subMenu);
	for (int i = 0; i < menuCount; i++) {
		EnableMenuItem(subMenu, i, MF_BYPOSITION | MF_DISABLED);
		CheckMenuItem(subMenu, i, MF_BYPOSITION | MF_UNCHECKED);
	}

	EnableMenuItem(subMenu, ID_OBJ_TOGGLEEXPORT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(subMenu, ID_OBJ_EDIT, MF_BYCOMMAND | MF_ENABLED);
	EnableMenuItem(subMenu, ID_OBJ_DELETE, MF_BYCOMMAND | MF_ENABLED);

	if (activeObject->exportResult) {
		CheckMenuItem(subMenu, ID_OBJ_TOGGLEEXPORT, MF_BYCOMMAND | MF_CHECKED);
	}

	CChartBase* chartBase = activeObject->toChartBase();
	if (chartBase) {
		CPolyChart* polyChart = chartBase->toPolyChart();

		EnableMenuItem(subMenu, ID_OBJ_CENTERVIEW, MF_BYCOMMAND | MF_ENABLED);
		EnableMenuItem(subMenu, ID_OBJ_COPYBOUNDS, MF_BYCOMMAND | MF_ENABLED);

		if (gProject->hasBoundsInClipboard()) {
			EnableMenuItem(subMenu, ID_OBJ_PASTEBOUNDS, MF_BYCOMMAND | MF_ENABLED);
		}

		if (chartBase && !polyChart) {
			EnableMenuItem(subMenu, ID_OBJ_INPUTAXISISY, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(subMenu, ID_OBJ_INPUTAXISISX, MF_BYCOMMAND | MF_ENABLED);

			if (chartBase->yIsInputAxis) {
				CheckMenuItem(subMenu, ID_OBJ_INPUTAXISISY, MF_BYCOMMAND | MF_CHECKED);
			} else {
				CheckMenuItem(subMenu, ID_OBJ_INPUTAXISISX, MF_BYCOMMAND | MF_CHECKED);
			}
		}
	}
	TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, gProject->ui->hwnd, NULL);
}

void selChangeLinesBox(char* item, int id) {
	if (gProject) {
		gProject->linesListChangeSel((TLine*)item);
	}
}

void rightClickLinesBox(char* item, int id) {
	if (!gProject || !gProject->ui || !gProject->chartProject) return;

	POINT mouse;
	GetCursorPos(&mouse);

	HMENU subMenu = GetSubMenu(gLinesBoxMenu, 0);

	int menuCount = GetMenuItemCount(subMenu);
	for (int i = 0; i < menuCount; i++) {
		EnableMenuItem(subMenu, i, MF_BYPOSITION | MF_DISABLED);
		CheckMenuItem(subMenu, i, MF_BYPOSITION | MF_UNCHECKED);
	}

	CChartObject* activeObject = gProject->selectedObject;
	if (activeObject) {
		CChartBase* chartBase = activeObject->toChartBase();
		CTrendChart* trendChart = activeObject->toTrendChart();
		CClampChart* clampChart = activeObject->toClampChart();

		if (item) {
			EnableMenuItem(subMenu, ID_LINES_DELETE, MF_BYCOMMAND | MF_ENABLED);
			EnableMenuItem(subMenu, ID_LINES_COPY, MF_BYCOMMAND | MF_ENABLED);

			if (chartBase && !trendChart) {
				EnableMenuItem(subMenu, ID_LINES_EDIT, MF_BYCOMMAND | MF_ENABLED);
			}
		}
		if (!clampChart && gProject->hasLineInClipboard()) {
			EnableMenuItem(subMenu, ID_LINES_PASTE, MF_BYCOMMAND | MF_ENABLED);
		}
	}

	TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, gProject->ui->hwnd, NULL);
}


void dblClickTableBox(char* item, int id) {
	if (item) {
		SendMessage(gProject->ui->hwnd, WM_COMMAND, ID_TABLE_EDITITEM, 0);
	}
}

void rightClickTableBox(char* item, int id) {
	if (!gProject || !gProject->ui || !gProject->chartProject || item == nullptr) return;

	CChartObject* activeObject = gProject->selectedObject;
	if (activeObject) {
		CTable* activeTable = activeObject->toTable();
		if (activeTable) {
			POINT mouse;
			GetCursorPos(&mouse);

			HMENU subMenu = GetSubMenu(gTableBoxMenu, 0);
			TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, gProject->ui->hwnd, NULL);
		}
	}
}

void dblClickInputsBox(char* item, int id) {
	if (item) {
		SendMessage(gProject->ui->hwnd, WM_COMMAND, ID_INPUT_EDIT, 0);
	}
}

void rightClickInputsBox(char* item, int id) {
	if (!gProject || !gProject->ui || !gProject->chartProject || item == nullptr) return;

	POINT mouse;
	GetCursorPos(&mouse);

	HMENU subMenu = GetSubMenu(gInputsBoxMenu, 0);
	TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, gProject->ui->hwnd, NULL);
}

void dblClickConstantsBox(char* item, int id) {
	if (item) {
		SendMessage(gProject->ui->hwnd, WM_COMMAND, ID_CONSTANT_EDIT, 0);
	}
}

void rightClickConstantsBox(char* item, int id) {
	if (!gProject || !gProject->ui || !gProject->chartProject || item == nullptr) return;

	POINT mouse;
	GetCursorPos(&mouse);

	HMENU subMenu = GetSubMenu(gConstantsBoxMenu, 0);
	TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, gProject->ui->hwnd, NULL);
}

void dblClickImagesBox(char* item, int id) {
	if (gProject && gProject->ui) {
		SendMessage(gProject->ui->hwnd, WM_COMMAND, ID_IMAGE_EDITNAME, 0);
	}
}
void rightClickImagesBox(char* item, int id) {
	if (!gProject || !gProject->ui || !gProject->chartProject || item == nullptr) return;

	POINT mouse;
	GetCursorPos(&mouse);

	HMENU subMenu = GetSubMenu(gImagesBoxMenu, 0);
	TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, mouse.x, mouse.y, 0, gProject->ui->hwnd, NULL);
}

void selChangeImagesBox(char* item, int id) {
	if (gProject) {
		gProject->imagesListSelChange((ChartImage*)item);
	}
}

void selChangeUpdateUI(char* item, int id) {
	if (gProject) gProject->needsUIUpdate();
}

void onChartsTabChange(int tabId) {
	if (gProject) {
		gProject->needsRedraw();
	}
}

void beforePaintChartCanvas(Canvas* canvas) {
	if (gProject) {
		gProject->beforePaintCharts();
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message){
	case WM_CREATE:
		{
			UIData* ui = new UIData;

			ui->hwnd = hWnd;

			ui->menu = GetMenu(hWnd);

			ui->toolbar = new Toolbar(hWnd, MAKEINTRESOURCE(IDB_TOOLBAR), 16, 13);
			ui->toolbar->addButton(0, ID_FILE_NEW, "New");
			ui->toolbar->addButton(1, ID_FILE_OPEN, "Open");
			ui->toolbar->addButton(2, ID_FILE_SAVE, "Save");
			ui->toolbar->addSeperator(8);
			ui->toolbar->addButton(4, ID_VIEW_SETTINGS, "Settings");
			ui->toolbar->addSeperator(8);
			ui->toolbar->addButton(11, ID_CHARTS_BOUNDVALUES, "Set bound values");
			ui->toolbar->addButton(7, ID_CHARTS_MARKLEFT, "Set left bound");
			ui->toolbar->addButton(9, ID_CHARTS_MARKTOP, "Set top bound");
			ui->toolbar->addButton(8, ID_CHARTS_MARKRIGHT, "Set right bound");
			ui->toolbar->addButton(10, ID_CHARTS_MARKBOTTOM, "Set bottom bound");

			ui->statusbar = CreateWindowEx(WS_EX_TOPMOST, STATUSCLASSNAME, "", WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0, hWnd, 0, hInst, NULL);
			int partsArray[] = { 150,300,450,-1 };
			SendMessage(ui->statusbar, SB_SETPARTS, 4, (long int)&partsArray);

			ui->mainTab = new TabControl(hWnd, 0, 0, 0, 0, nullptr);
			ui->chartTabIndex = ui->mainTab->addTab("Charts");
			ui->uiTabIndex = ui->mainTab->addTab("UI");

				ui->chartCanvas = new Canvas(ui->mainTab->getHWnd(), 0, 0, 0, 0);
				ui->mainTab->addChildToTab(ui->chartCanvas->hwnd, ui->chartTabIndex);
				ui->chartCanvas->leftDown = chartLeftDown;
				ui->chartCanvas->leftMove = chartLeftMove;
				ui->chartCanvas->leftUp = chartLeftUp;
				ui->chartCanvas->rightDown = chartRightDown;
				ui->chartCanvas->rightMove = chartRightMove;
				ui->chartCanvas->rightUp = chartRightUp;
				ui->chartCanvas->mouseMove = chartMouseMove;
				ui->chartCanvas->mouseWheel = chartMouseWheel;
				ui->chartCanvas->beforePaint = beforePaintChartCanvas;

				ui->chartsTab = new TabControl(ui->mainTab->getHWnd(), 0, 0, 0, 0, onChartsTabChange);
				ui->mainTab->addChildToTab(ui->chartsTab->getHWnd(), ui->chartTabIndex);
				ui->objectsTab = ui->chartsTab->addTab("Objects");
				ui->inputsTab = ui->chartsTab->addTab("Inputs");
				ui->imagesTab = ui->chartsTab->addTab("Images");

				ui->objectsList = new ListUI("Objects", ui->chartsTab->getHWnd(), 0, 0, 0, 0, drawChartsBox, selChangeChartsBox, dblClickChartsBox, rightClickChartsBox);
				ui->chartsTab->addChildToTab(ui->objectsList->getHwnd(), ui->objectsTab);
				ui->objectsList->buttonBar->addButton(IDB_UP, ID_OBJ_UP, "Move up");
				ui->objectsList->buttonBar->addButton(IDB_DOWN, ID_OBJ_DOWN, "Move down");
				ui->objectsList->buttonBar->addSeperator();
				ui->objectsList->buttonBar->addButton(IDB_DELETE, ID_OBJ_DELETE, "Delete");
				ui->objectsList->buttonBar->addSeperator();
				ui->objectsList->buttonBar->addButton(IDB_ADDLINEAR, ID_OBJ_ADDLINEAR, "Add linear chart");
				ui->objectsList->buttonBar->addButton(IDB_ADDCLAMP, ID_OBJ_ADDCLAMP, "Add clamp chart");
				ui->objectsList->buttonBar->addButton(IDB_ADDTREND, ID_OBJ_ADDTREND, "Add trend chart");
				ui->objectsList->buttonBar->addButton(IDB_ADDPOLY, ID_OBJ_ADDPOLY, "Add poly chart");
				ui->objectsList->buttonBar->addSeperator();
				ui->objectsList->buttonBar->addButton(IDB_ADDTABLE, ID_OBJ_ADDTABLE, "Add linear interpolation table");
				ui->objectsList->buttonBar->addButton(IDB_ADDSCRIPT, ID_OBJ_ADDSCRIPT, "Add a script");
				ui->objectsList->addColumn(0.15f, "#");
				ui->objectsList->addColumn(0.85f, "Identity");

				ui->inputsList = new ListUI("Inputs", ui->chartsTab->getHWnd(), 0, 0, 0, 0, drawInputsBox, selChangeUpdateUI, dblClickInputsBox, rightClickInputsBox);
				ui->chartsTab->addChildToTab(ui->inputsList->getHwnd(), ui->inputsTab);
				ui->inputsList->buttonBar->addButton(IDB_UP, ID_INPUT_UP, "Move up");
				ui->inputsList->buttonBar->addButton(IDB_DOWN, ID_INPUT_DOWN, "Move down");
				ui->inputsList->buttonBar->addSeperator();
				ui->inputsList->buttonBar->addButton(IDB_ADD, ID_INPUT_ADD, "Add input");
				ui->inputsList->buttonBar->addSeperator();
				ui->inputsList->buttonBar->addButton(IDB_DELETE, ID_INPUT_DELETE, "Delete input");
				ui->inputsList->addColumn(0.15f, "#");
				ui->inputsList->addColumn(0.85f, "Name");

				ui->constantsList = new ListUI("Constants", ui->chartsTab->getHWnd(), 0, 0, 0, 0, drawConstantsBox, selChangeUpdateUI, dblClickConstantsBox, rightClickConstantsBox);
				ui->chartsTab->addChildToTab(ui->constantsList->getHwnd(), ui->inputsTab);
				ui->constantsList->buttonBar->addButton(IDB_UP, ID_CONSTANT_UP, "Move up");
				ui->constantsList->buttonBar->addButton(IDB_DOWN, ID_CONSTANT_DOWN, "Move down");
				ui->constantsList->buttonBar->addSeperator();
				ui->constantsList->buttonBar->addButton(IDB_ADD, ID_CONSTANT_ADD, "Add constant");
				ui->constantsList->buttonBar->addSeperator();
				ui->constantsList->buttonBar->addButton(IDB_DELETE, ID_CONSTANT_DELETE, "Delete constant");
				ui->constantsList->addColumn(0.65f, "Name");
				ui->constantsList->addColumn(0.35f, "Value");

				ui->linesList = new ListUI("Lines", ui->chartsTab->getHWnd(), 0, 0, 0, 0, drawLinesBox, selChangeLinesBox, nullptr, rightClickLinesBox);
				ui->chartsTab->addChildToTab(ui->linesList->getHwnd(), ui->objectsTab);
				ui->linesList->buttonBar->addButton(IDB_ADDLINE, ID_LINES_NEWLINE, "New line");
				ui->linesList->buttonBar->addSeperator();
				ui->linesList->buttonBar->addButton(IDB_ADDPOINTS, ID_LINES_ADDPOINTS, "Add points");
				ui->linesList->buttonBar->addButton(IDB_REMOVEPOINTS, ID_LINES_REMOVEPOINT, "Remove last point");
				ui->linesList->buttonBar->addSeperator();
				ui->linesList->buttonBar->addButton(IDB_DELETE, ID_LINES_DELETE, "Delete line");
				ui->linesList->addColumn(0.65f, "Value");
				ui->linesList->addColumn(0.35f, "Points");

				ui->tableList = new ListUI("Table", ui->chartsTab->getHWnd(), 0, 0, 0, 0, drawTableBox, selChangeUpdateUI, dblClickTableBox, rightClickTableBox);
				ui->chartsTab->addChildToTab(ui->tableList->getHwnd(), ui->objectsTab);
				ui->tableList->buttonBar->addButton(IDB_ADD, ID_TABLE_ADDENTRY, "Add entry");
				ui->tableList->buttonBar->addButton(IDB_DELETE, ID_TABLE_DELETEENTRY, "Delete entry");
				ui->tableList->addColumn(0.65f, "Object");
				ui->tableList->addColumn(0.35f, "Value");

				ui->imagesList = new ListUI("Images", ui->chartsTab->getHWnd(), 0, 0, 0, 0, drawImagesBox, selChangeImagesBox, dblClickImagesBox, rightClickImagesBox);
				ui->chartsTab->addChildToTab(ui->imagesList->getHwnd(), ui->imagesTab);
				ui->imagesList->buttonBar->addButton(IDB_ADD, ID_IMAGE_ADD, "Add image(s)");
				ui->imagesList->buttonBar->addButton(IDB_DELETE, ID_IMAGE_DELETE, "Delete image");
				ui->imagesList->addColumn(1.0f, "Name");

				ui->uiList = new ListUI("UI Objects", ui->mainTab->getHWnd(), 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
				ui->mainTab->addChildToTab(ui->uiList->getHwnd(), ui->uiTabIndex);

				ui->uiCanvas = new Canvas(ui->mainTab->getHWnd(), 0, 0, 0, 0);
				ui->mainTab->addChildToTab(ui->uiCanvas->hwnd, ui->uiTabIndex);
				ui->uiCanvas->leftDown = uiLeftDown;
				ui->uiCanvas->leftMove = uiLeftMove;
				ui->uiCanvas->leftUp = uiLeftUp;
				ui->uiCanvas->rightDown = uiRightDown;
				ui->uiCanvas->rightMove = uiRightMove;
				ui->uiCanvas->rightUp = uiRightUp;
				ui->uiCanvas->mouseMove = uiMouseMove;
				ui->uiCanvas->mouseWheel = uiMouseWheel;


			gProject = new ProjectContainer(ui);

			SendMessage(hWnd, WM_SIZE, 0, 0);
		}
		break;

	case WM_KEYUP:
		if (gProject) {
			if (gProject->ui) {
				if (gProject->ui->mainTab->currentTab() == gProject->ui->uiTabIndex) {
					gProject->uiKeyDown(wParam);
				} else if (gProject->ui->mainTab->currentTab() == gProject->ui->chartTabIndex) {
					gProject->chartKeyDown(wParam);
				}
			}
		}
		break;

	case WM_CHAR:
		if (gProject){
			if (gProject->ui) {
				if (gProject->ui->mainTab->currentTab() == gProject->ui->uiTabIndex) {
					gProject->uiCharPress(wParam);
				} else if (gProject->ui->mainTab->currentTab() == gProject->ui->chartTabIndex) {
					gProject->chartCharPress(wParam);
				}
			}
		}
		break;

	case WM_EXITSIZEMOVE:
		{
			if (gProject) {
				gProject->needsRedraw();
			}
		}
		break;
	case WM_SIZE:
		{
			RECT rect;
			RECT sbarRect;

			GetClientRect(hWnd, &rect);
			
			if (gProject) {
				UIData* ui = gProject->ui;

				if (ui) {
					int rightPanelW = 250;

					SendMessage(ui->statusbar, WM_SIZE, 0, 0);
					GetClientRect(ui->statusbar, &sbarRect);
					rect.bottom -= sbarRect.bottom;

					ui->toolbar->autosize();
					rect.top = ui->toolbar->getHeight();

					ui->mainTab->move(0, rect.top, rect.right, rect.bottom - rect.top);
					RECT mainTabRect = ui->mainTab->getRect();
					int mainTabW = mainTabRect.right - mainTabRect.left;
					int mainTabH = mainTabRect.bottom - mainTabRect.top;
					ui->chartCanvas->move(mainTabRect.left, mainTabRect.top, mainTabW - rightPanelW, mainTabH);
					ui->chartsTab->move(mainTabRect.right - rightPanelW, mainTabRect.top, rightPanelW, mainTabH);
					RECT chartsTabRect = ui->chartsTab->getRect();
					int chartsTabW = chartsTabRect.right - chartsTabRect.left;
					int chartsTabH = chartsTabRect.bottom - chartsTabRect.top;
					int objectsListH = int(double(chartsTabH) / 1.75f);
					int selectedObjH = chartsTabH - objectsListH;
					ui->objectsList->move(chartsTabRect.left, chartsTabRect.top, chartsTabW, objectsListH);

					ui->linesList->move(chartsTabRect.left, chartsTabRect.top + objectsListH, chartsTabW, 0);
					ui->tableList->move(chartsTabRect.left, chartsTabRect.top + objectsListH, chartsTabW, 0);
					if (gProject) {
						if (gProject->selectedObject) {
							if (gProject->selectedObject->toChartBase()) {
								ui->linesList->move(chartsTabRect.left, chartsTabRect.top + objectsListH, chartsTabW, selectedObjH);
								ui->tableList->move(chartsTabRect.left, chartsTabRect.top + objectsListH, chartsTabW, 0);
							} else if (gProject->selectedObject->toTable()) {
								ui->linesList->move(chartsTabRect.left, chartsTabRect.top + objectsListH, chartsTabW, 0);
								ui->tableList->move(chartsTabRect.left, chartsTabRect.top + objectsListH, chartsTabW, selectedObjH);
							}
						}
					}

					ui->inputsList->move(chartsTabRect.left, chartsTabRect.top, chartsTabW, chartsTabH / 2);
					ui->constantsList->move(chartsTabRect.left, chartsTabRect.top + chartsTabH / 2, chartsTabW, chartsTabH / 2);
					ui->imagesList->move(chartsTabRect.left, chartsTabRect.top, chartsTabW, chartsTabH);

					ui->uiCanvas->move(mainTabRect.left, mainTabRect.top, mainTabW - rightPanelW, mainTabH);
					ui->uiList->move(mainTabRect.right - rightPanelW, mainTabRect.top, rightPanelW, mainTabH);
				}
			}

		}
		if (wParam == SIZE_MAXIMIZED || wParam == SIZE_MAXSHOW){
			if (gProject) gProject->needsRedraw();
		}
		break;

	case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case ID_FILE_NEW:
				if (gProject) gProject->fileNew();
				break;
			case ID_FILE_OPEN:
				if (gProject) gProject->fileOpen();
				break;
			case ID_FILE_SAVE:
				if (gProject) gProject->fileSave();
				break;
			case ID_FILE_SAVEAS:
				if (gProject) gProject->fileSaveAs();
				break;
			case ID_FILE_EXIT:
				DestroyWindow(hWnd);
				break;

			case ID_VIEW_RESETVIEW:
				if (gProject) {
					if (gProject->chartProject) {
						gProject->chartProject->viewState.xy = TPoint(0, 0);
						gProject->chartProject->viewState.zoom = 1.0f;
						gProject->needsRedraw();
						gProject->needsUIUpdate();
					}
				}
				break;
			case ID_VIEW_SETTINGS:
				if (gProject) {
					DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_PROJECTSETTINGS), hWnd, ProjectSettingsProc, 0);
				}
				break;

            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDlgProc);
                break;

			case ID_INPUT_UP:
				if (gProject) gProject->inputMoveUp();
				break;
			case ID_INPUT_DOWN:
				if (gProject) gProject->inputMoveDown();
				break;
			case ID_INPUT_ADD:
				if (gProject) gProject->inputAdd();
				break;
			case ID_INPUT_DELETE:
				if (gProject) gProject->inputDelete();
				break;
			case ID_INPUT_EDIT:
				if (gProject && gProject->ui && gProject->ui->inputsList) {
					CInput* item = (CInput*)gProject->ui->inputsList->getCurSelItemData();
					if (item) {
						DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_EDITINPUT), hWnd, EditInputProc, (LPARAM)item);
					}
				}
				break;

			case ID_CONSTANT_UP:
				if (gProject) gProject->constantMoveUp();
				break;
			case ID_CONSTANT_DOWN:
				if (gProject) gProject->constantMoveDown();
				break;
			case ID_CONSTANT_ADD:
				if (gProject) gProject->constantAdd();
				break;
			case ID_CONSTANT_DELETE:
				if (gProject) gProject->constantDelete();
				break;
			case ID_CONSTANT_EDIT:
				if (gProject && gProject->ui && gProject->ui->constantsList) {
					CConstant* item = (CConstant*)gProject->ui->constantsList->getCurSelItemData();
					if (item) {
						DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_EDITVARIABLE), hWnd, EditConstantProc, (LPARAM)item);
					}
				}
				break;

			case ID_IMAGE_ADD:
				if (gProject) gProject->newChartImages();
				break;
			case ID_IMAGE_DELETE:
				if (gProject) {
					ChartImage* image = (ChartImage*)gProject->ui->imagesList->getCurSelItemData();
					if (image) {
						gProject->deleteChartImage(image);
					}
				}
				break;
			case ID_IMAGE_EDITNAME:
				if (gProject) {
					ChartImage* image = (ChartImage*)gProject->ui->imagesList->getCurSelItemData();
					if (image) {
						DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_EDITIMAGE), hWnd, EditImageProc, (LPARAM)image);
					}
				}
				break;
			case ID_IMAGE_CENTERVIEW:
				if (gProject) {
					ChartImage* image = (ChartImage*)gProject->ui->imagesList->getCurSelItemData();
					if (image) {
						gProject->centerOnImage(image);
					}
				
				}
				break;


			case ID_CHARTS_BOUNDVALUES:
				if (gProject && gProject->selectedObject) {
					CChartBase* activeChart = gProject->selectedObject->toChartBase();
					if (activeChart) {
						DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_BOUNDS), hWnd, BoundsDlgProc, (LONG_PTR)activeChart);
					}
				}
				break;
			case ID_CHARTS_MARKLEFT:
			case ID_CHARTS_MARKTOP:
			case ID_CHARTS_MARKRIGHT:
			case ID_CHARTS_MARKBOTTOM:
				if (gProject) gProject->objectMarkBounds(wmId);
				break;

			case ID_TABLE_ADDENTRY:
				if (gProject) gProject->tableNewEntry();
				break;
			case ID_TABLE_DELETEENTRY:
				if (gProject) gProject->tableDeleteEntry();
				break;
			case ID_TABLE_EDITITEM:
				if (gProject && gProject->ui && gProject->ui->tableList) {
					CTableEntry* item = (CTableEntry*)gProject->ui->tableList->getCurSelItemData();
					if (item) {
						DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_EDITTABLEITEM), hWnd, EditTableItemProc, (LPARAM)item);
					}
				}
				break;

			case ID_LINES_ADDPOINTS:
				if (gProject) gProject->lineAddPoints();
				break;
			case ID_LINES_DELETE:
				if (gProject) gProject->lineDeleteLine();
				break;
			case ID_LINES_REMOVEPOINT:
				if (gProject) gProject->lineRemovePoint();
				break;
			case ID_LINES_NEWLINE:
				if (gProject) gProject->lineNewLine();
				break;
			case ID_LINES_COPY:
				if (gProject && gProject->selectedLine) {
					gProject->copyLine(gProject->selectedLine);
				}
				break;
			case ID_LINES_PASTE:
				if (gProject && gProject->selectedObject) {
					CChartBase* chart = gProject->selectedObject->toChartBase();
					if (chart && !chart->toClampChart()) {
						gProject->pasteLine(chart);
					}
				}
				break;
			

			case ID_OBJ_EDIT:
				if (gProject) {
					if (gProject->selectedObject) {
						CChartBase* chart = gProject->selectedObject->toChartBase();
						CTable* table = gProject->selectedObject->toTable();
						CScript* script = gProject->selectedObject->toScript();
						
						if (chart || table) {
							DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_CHARTSETTINGS), hWnd, ChartSettingsDlgProc, (LONG_PTR)gProject->selectedObject);
						} else if (script) {
							DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_EDITSCRIPT), hWnd, EditScriptProc, (LONG_PTR)gProject->selectedObject);
						}
					}
				}
				break;
			case ID_OBJ_CENTERVIEW:
				if (gProject && gProject->selectedObject) {
					gProject->centerOnChart(gProject->selectedObject->toChartBase());
				}
				break;
			case ID_OBJ_COPYBOUNDS:
				if (gProject && gProject->selectedObject) {
					gProject->copyBounds(gProject->selectedObject->toChartBase());
				}
				break;
			case ID_OBJ_PASTEBOUNDS:
				if (gProject && gProject->selectedObject) {
					gProject->pasteBounds(gProject->selectedObject->toChartBase());
				}
				break;
			case ID_OBJ_INPUTAXISISX:
				if (gProject) {
					if (gProject->selectedObject) {
						CChartBase* chart = gProject->selectedObject->toChartBase();
						chart->yIsInputAxis = false;
						gProject->updateObjectsList();
						gProject->updateLinesList();
						gProject->needsRedraw();
					}
				}
				break;
			case ID_OBJ_INPUTAXISISY:
				if (gProject) {
					if (gProject->selectedObject) {
						CChartBase* chart = gProject->selectedObject->toChartBase();
						chart->yIsInputAxis = true;
						gProject->updateObjectsList();
						gProject->updateLinesList();
						gProject->needsRedraw();
					}
				}
				break;
			case ID_OBJ_TOGGLEEXPORT:
				if (gProject) {
					if (gProject->selectedObject) {
						gProject->selectedObject->exportResult = !gProject->selectedObject->exportResult;
						gProject->updateObjectsList();
					}
				}
				break;

			case ID_OBJ_DELETE:
				if (gProject) gProject->objectDelete();
				break;
			case ID_OBJ_DOWN:
				if (gProject) gProject->objectMoveDown();
				break;
			case ID_OBJ_UP:
				if (gProject) gProject->objectMoveUp();
				break;
			case ID_OBJ_ADDLINEAR:
				if (gProject) gProject->newLinearChart();
				break;
			case ID_OBJ_ADDPOLY:
				if (gProject) gProject->newPolyChart();
				break;
			case ID_OBJ_ADDCLAMP:
				if (gProject) gProject->newClampChart();
				break;
			case ID_OBJ_ADDTREND:
				if (gProject) gProject->newTrendChart();
				break;
			case ID_OBJ_ADDSCRIPT:
				if (gProject) gProject->newScript();
				break;
			case ID_OBJ_ADDTABLE:
				if (gProject) gProject->newTable();
				break;

			case ID_DATA_RUNCHARTS:
				DialogBoxParamA(hInst, MAKEINTRESOURCE(IDD_RUNCHARTS), hWnd, RunChartsProc, (LONG_PTR)gProject->selectedObject);
				break;

			case ID_EXPORT_EXPORTHTML:
				if (gProject) gProject->exportToJS();
				break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_CLOSE:
		if (MessageBoxA(hWnd, "Are you sure you want to close the application?", "Did you save?", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDOK) {
			DestroyWindow(hWnd);
		}
		return 0;

    case WM_DESTROY:
		{
			if (gProject && gProject->ui) {
				delete gProject->ui->linesList;
				delete gProject->ui->tableList;
				delete gProject->ui->chartCanvas;
				delete gProject->ui->constantsList;
				delete gProject->ui->imagesList;
				delete gProject->ui->inputsList;
				delete gProject->ui->uiList;
				delete gProject->ui->objectsList;
				delete gProject->ui->uiCanvas;
				delete gProject->ui->chartsTab;
				delete gProject->ui->mainTab;

				delete gProject->ui;
				gProject->ui = nullptr;
			}
			PostQuitMessage(0);
		}
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
