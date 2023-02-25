#include "stdafx.h"
#include "SyntaxColoredEditor.h"
#include "resource.h"
#include "helpers.h"
#include "ButtonBar.h"
#include "dialogs.h"
#include "ListUI.h"
#include "UIData.h"
#include "ChartProject.h"
#include "ProjectContainer.h"
#include "ChartImage.h"

#include "CChartObject.h"
#include "CConstant.h"
#include "CInput.h"
#include "CChartBase.h"
#include "CPolyChart.h"
#include "CClampChart.h"
#include "CLinearChart.h"
#include "CTrendChart.h"
#include "CScript.h"
#include "CTable.h"


CChartObject* gChartObjectToEdit = nullptr;
CChartObject* gVariableToEdit = nullptr;
CTableEntry* gTableItemToEdit = nullptr;
ChartImage* gImageToEdit = nullptr;
SyntaxColoredEditor* gSCEditor = nullptr;


// About box
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


//Chart settings
void addInputsToCombo(HWND combobox, ChartProject* chartProject, CChartObject* theChart) {
	if (chartProject) {
		chartProject->makeObjectNamesUnique();
		SendMessage(combobox, CB_ADDSTRING, 0, 0);//add empty one so user can erase the input if needed
		//add constants
		for (auto & constant : chartProject->constants) {
			SendMessage(combobox, CB_ADDSTRING, 0, (LPARAM)constant);
		}

		//add input variables
		for (auto & inputVars : chartProject->inputs) {
			SendMessage(combobox, CB_ADDSTRING, 0, (LPARAM)inputVars);
		}

		//add charts as outputs
		for (size_t i = 0; i < chartProject->objects.size(); i++) {
			if (chartProject->objects[i] == theChart) {
				break;
			}
			SendMessage(combobox, CB_ADDSTRING, 0, (LPARAM)chartProject->objects[i]);
		}
	}
}

CChartObject* getSelectedInput(HWND hDlg, int id) {
	int sel = SendDlgItemMessageA(hDlg, id, CB_GETCURSEL, 0, 0);
	CChartObject* selectedInput = nullptr;
	if (sel != CB_ERR) {
		selectedInput = (CChartObject*)SendDlgItemMessageA(hDlg, id, CB_GETITEMDATA, sel, 0);
	}
	return selectedInput;
}

bool tryAndSetObjectName(HWND hDlg, ChartProject* chartProject, CChartObject* chartObject, string newName) {
	if (!chartProject || !chartObject) return false;

	string newChartName = CChartObject::getFormattedName(newName);

	CChartObject* existingObject = chartProject->getObjectByName(newChartName);
	if (existingObject != nullptr && existingObject != chartObject) {
		string msg = "The name '" + newChartName + "' already exists, please enter a unique name.";
		MessageBox(hDlg, msg.c_str(), "Name needs to be unique", MB_ICONWARNING);
		return false;
	}

	chartObject->setName(newChartName);
	return true;
}

bool isChecked(HWND hDlg, int checkboxID) {
	return SendDlgItemMessageA(hDlg, checkboxID, BM_GETCHECK, 0, 0) == BST_CHECKED;
}

INT_PTR CALLBACK ChartSettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{

			gChartObjectToEdit = reinterpret_cast<CChartObject*>((CChartObject*)lParam);
			if (!gProject || !gProject->chartProject || !gChartObjectToEdit) {
				EndDialog(hDlg, false);
				return false;
			}
			CTable* tableObject = gChartObjectToEdit->toTable();
			CChartBase* chartBase = gChartObjectToEdit->toChartBase();

			gProject->chartProject->validateInputs();

			//init dialog items
			SendDlgItemMessageA(hDlg, IDC_CHARTNAME, WM_SETTEXT, 0, (LPARAM)gChartObjectToEdit->getName().c_str());

			if (gChartObjectToEdit->exportResult) {
				SendDlgItemMessageA(hDlg, IDC_EXPORTRESULT, BM_SETCHECK, BST_CHECKED, 0);
			} else {
				SendDlgItemMessageA(hDlg, IDC_EXPORTRESULT, BM_SETCHECK, BST_UNCHECKED, 0);
			}

			string whyIsBad;
			if (gChartObjectToEdit->checkIsBad(gProject->chartProject, whyIsBad)) {
				whyIsBad = "Error check: " + whyIsBad;
			} else {
				whyIsBad = "No errors";
			}
			SetDlgItemTextA(hDlg, IDC_ERRORTEXT, whyIsBad.c_str());

			EnableWindow(GetDlgItem(hDlg, IDC_REVERSE_INTERPOLATE), false);

			if (tableObject) {
				EnableWindow(GetDlgItem(hDlg, IDC_INPUTAXISISX), false);
				EnableWindow(GetDlgItem(hDlg, IDC_INPUTAXISISY), false);
				EnableWindow(GetDlgItem(hDlg, IDC_INPUT2LIST), false);
				EnableWindow(GetDlgItem(hDlg, IDC_INPUT3LIST), false);
				addInputsToCombo(GetDlgItem(hDlg, IDC_INPUT1LIST), gProject->chartProject, tableObject);
				SendDlgItemMessageA(hDlg, IDC_INPUT1LIST, CB_SELECTSTRING, -1, (LPARAM)tableObject->inputVariable);
				SendDlgItemMessageA(hDlg, IDC_INPUT1TEXT, WM_SETTEXT, 0, (LPARAM)"Value");
			} else if (chartBase) {
				addInputsToCombo(GetDlgItem(hDlg, IDC_INPUT1LIST), gProject->chartProject, chartBase);
				addInputsToCombo(GetDlgItem(hDlg, IDC_INPUT2LIST), gProject->chartProject, chartBase);
				addInputsToCombo(GetDlgItem(hDlg, IDC_INPUT3LIST), gProject->chartProject, chartBase);

				CPolyChart* polyChart = chartBase->toPolyChart();
				CTrendChart* trendChart = chartBase->toTrendChart();
				CLinearChart* linearChart = chartBase->toLinearChart();
				CClampChart* clampChart = chartBase->toClampChart();
				if (polyChart) {
					SendDlgItemMessageA(hDlg, IDC_INPUT1TEXT, WM_SETTEXT, 0, (LPARAM)"X in");
					SendDlgItemMessageA(hDlg, IDC_INPUT2TEXT, WM_SETTEXT, 0, (LPARAM)"Y in");
					SendDlgItemMessageA(hDlg, IDC_INPUT3TEXT, WM_SETTEXT, 0, (LPARAM)"n/a");
					EnableWindow(GetDlgItem(hDlg, IDC_INPUTAXISISX), false);
					EnableWindow(GetDlgItem(hDlg, IDC_INPUTAXISISY), false);
					EnableWindow(GetDlgItem(hDlg, IDC_INPUT3LIST), false);
					SendDlgItemMessageA(hDlg, IDC_INPUT1LIST, CB_SELECTSTRING, -1, (LPARAM)polyChart->inputVariable1);
					SendDlgItemMessageA(hDlg, IDC_INPUT2LIST, CB_SELECTSTRING, -1, (LPARAM)polyChart->inputVariable2);
				} else if (trendChart) {
					SendDlgItemMessageA(hDlg, IDC_INPUT1TEXT, WM_SETTEXT, 0, (LPARAM)"X/Y");
					SendDlgItemMessageA(hDlg, IDC_INPUT2TEXT, WM_SETTEXT, 0, (LPARAM)"Entry");
					SendDlgItemMessageA(hDlg, IDC_INPUT3TEXT, WM_SETTEXT, 0, (LPARAM)"Exit");
					SendDlgItemMessageA(hDlg, IDC_INPUT1LIST, CB_SELECTSTRING, -1, (LPARAM)trendChart->inputVariable1);
					SendDlgItemMessageA(hDlg, IDC_INPUT2LIST, CB_SELECTSTRING, -1, (LPARAM)trendChart->entryPointInput);
					SendDlgItemMessageA(hDlg, IDC_INPUT3LIST, CB_SELECTSTRING, -1, (LPARAM)trendChart->inputVariable2);
				} else if (linearChart) {
					SendDlgItemMessageA(hDlg, IDC_INPUT1TEXT, WM_SETTEXT, 0, (LPARAM)"X/Y");
					SendDlgItemMessageA(hDlg, IDC_INPUT2TEXT, WM_SETTEXT, 0, (LPARAM)"Z");
					SendDlgItemMessageA(hDlg, IDC_INPUT3TEXT, WM_SETTEXT, 0, (LPARAM)"n/a");
					SendDlgItemMessageA(hDlg, IDC_INPUT1LIST, CB_SELECTSTRING, -1, (LPARAM)linearChart->inputVariable1);
					SendDlgItemMessageA(hDlg, IDC_INPUT2LIST, CB_SELECTSTRING, -1, (LPARAM)linearChart->inputVariable2);
					EnableWindow(GetDlgItem(hDlg, IDC_INPUT3LIST), false);
					EnableWindow(GetDlgItem(hDlg, IDC_REVERSE_INTERPOLATE), true);
					if (linearChart->reverseInterpolate) {
						SendDlgItemMessageA(hDlg, IDC_REVERSE_INTERPOLATE, BM_SETCHECK, BST_CHECKED, 0);
					}else {
						SendDlgItemMessageA(hDlg, IDC_REVERSE_INTERPOLATE, BM_SETCHECK, BST_UNCHECKED, 0);
					}
				} else if (clampChart) {
					SendDlgItemMessageA(hDlg, IDC_INPUT1TEXT, WM_SETTEXT, 0, (LPARAM)"X/Y");
					SendDlgItemMessageA(hDlg, IDC_INPUT2TEXT, WM_SETTEXT, 0, (LPARAM)"Value");
					SendDlgItemMessageA(hDlg, IDC_INPUT3TEXT, WM_SETTEXT, 0, (LPARAM)"n/a");
					SendDlgItemMessageA(hDlg, IDC_INPUT1LIST, CB_SELECTSTRING, -1, (LPARAM)clampChart->inputVariable1);
					SendDlgItemMessageA(hDlg, IDC_INPUT2LIST, CB_SELECTSTRING, -1, (LPARAM)clampChart->inputVariable2);
					EnableWindow(GetDlgItem(hDlg, IDC_INPUT3LIST), false);
				}
				if (trendChart || linearChart || clampChart) {
					if (chartBase->yIsInputAxis) {
						SendDlgItemMessageA(hDlg, IDC_INPUTAXISISY, BM_SETCHECK, BST_CHECKED, 0);
					} else {
						SendDlgItemMessageA(hDlg, IDC_INPUTAXISISX, BM_SETCHECK, BST_CHECKED, 0);
					}
				}

				SendDlgItemMessageA(hDlg, IDC_INPUT1LIST, CB_SETMINVISIBLE, 15, 0);
				SendDlgItemMessageA(hDlg, IDC_INPUT2LIST, CB_SETMINVISIBLE, 15, 0);
				SendDlgItemMessageA(hDlg, IDC_INPUT3LIST, CB_SETMINVISIBLE, 15, 0);

			}
		}
		return (INT_PTR)TRUE;
	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* pdis = (DRAWITEMSTRUCT*)lParam;
			if (pdis->CtlType == ODT_COMBOBOX) {
				if (pdis->itemID == -1) {
					break;
				}
				CChartObject* object = reinterpret_cast<CChartObject*>(pdis->itemData);

				COLORREF clrBackground;
				COLORREF clrForeground;
				TEXTMETRIC tm;
				int x, y;

				clrForeground = SetTextColor(pdis->hDC,
					GetSysColor(pdis->itemState & ODS_SELECTED ?
						COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

				clrBackground = SetBkColor(pdis->hDC,
					GetSysColor(pdis->itemState & ODS_SELECTED ?
						COLOR_HIGHLIGHT : COLOR_WINDOW));

				// Calculate the vertical and horizontal position.
				GetTextMetrics(pdis->hDC, &tm);
				y = (pdis->rcItem.bottom + pdis->rcItem.top - tm.tmHeight) / 2;
				x = 5;

				if (object) {
					ExtTextOut(pdis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &pdis->rcItem, object->getName().c_str(), object->getName().length(), NULL);
				} else {
					string emptyString = "<empty>";
					ExtTextOut(pdis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &pdis->rcItem, emptyString.c_str(), emptyString.length(), NULL);
				}
				// Restore the previous colors.
				SetTextColor(pdis->hDC, clrForeground);
				SetBkColor(pdis->hDC, clrBackground);

				if (pdis->itemState & ODS_FOCUS) {
					DrawFocusRect(pdis->hDC, &pdis->rcItem);
				}
				return 1;

			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			{
				if (gChartObjectToEdit && gProject && gProject->chartProject) {
					if (tryAndSetObjectName(hDlg, gProject->chartProject, gChartObjectToEdit, CChartObject::getFormattedName(getDlgItemString(hDlg, IDC_CHARTNAME))) == false) {
						break;
					}

					gChartObjectToEdit->exportResult = isChecked(hDlg, IDC_EXPORTRESULT);

					CTable* tableObj = gChartObjectToEdit->toTable();
					if (tableObj) {
						tableObj->inputVariable = getSelectedInput(hDlg, IDC_INPUT1LIST);
					}

					CChartBase* chartBase = gChartObjectToEdit->toChartBase();
					if (chartBase) {
						CTrendChart* trendChart = chartBase->toTrendChart();
						CLinearChart* linearChart = chartBase->toLinearChart();
						if (trendChart) {
							trendChart->inputVariable1 = getSelectedInput(hDlg, IDC_INPUT1LIST);
							trendChart->entryPointInput = getSelectedInput(hDlg, IDC_INPUT2LIST);
							trendChart->inputVariable2 = getSelectedInput(hDlg, IDC_INPUT3LIST);
						} else {
							chartBase->inputVariable1 = getSelectedInput(hDlg, IDC_INPUT1LIST);
							chartBase->inputVariable2 = getSelectedInput(hDlg, IDC_INPUT2LIST);
						}
						if (linearChart) {
							linearChart->reverseInterpolate = isChecked(hDlg, IDC_REVERSE_INTERPOLATE);
						}

						chartBase->yIsInputAxis = isChecked(hDlg, IDC_INPUTAXISISY);
					}

					gProject->chartProject->makeObjectNamesUnique();
					gProject->chartProject->validateInputs();
					gProject->needsRedraw();
					gProject->updateObjectsList();
				}
			}
			EndDialog(hDlg, true);
			return true;

		case IDCANCEL:
			EndDialog(hDlg, false);
			return (INT_PTR)TRUE;
		}

		break;

	case WM_CLOSE:
		break;
	}
	return (INT_PTR)FALSE;
}


//Project settings
COLORREF gSelLineColor;
COLORREF gSelNodeColor;
COLORREF gNotSelLineColor;
COLORREF gNotSelNodeColor;
COLORREF gInactiveLineColor;
COLORREF gInactiveNodeColor;

void drawColorButton(DRAWITEMSTRUCT* pdis, COLORREF c) {
	HBRUSH color = CreateSolidBrush(c);
	HBRUSH oldBrush = (HBRUSH)SelectObject(pdis->hDC, color);

	RECT dt = pdis->rcItem;
	dt.left = dt.left + 15;

	if (pdis->itemState & ODS_SELECTED) {
		DrawFrameControl(pdis->hDC, &pdis->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH | DFCS_PUSHED);
		Rectangle(pdis->hDC, pdis->rcItem.right - 28, pdis->rcItem.top + 5, pdis->rcItem.right - 5, pdis->rcItem.bottom - 3);
		dt.left += 2;
		dt.top += 2;
	} else {
		DrawFrameControl(pdis->hDC, &pdis->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);
		Rectangle(pdis->hDC, pdis->rcItem.right - 30, pdis->rcItem.top + 3, pdis->rcItem.right - 5, pdis->rcItem.bottom - 3);
	}
	int len = SendMessage(pdis->hwndItem, WM_GETTEXTLENGTH, 0, 0) + 1;
	char* text = new char[len];
	if (text) {
		SendMessage(pdis->hwndItem, WM_GETTEXT, len, (LPARAM)text);
		DrawText(pdis->hDC, text, len, &dt, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	delete[]  text;
	SelectObject(pdis->hDC, oldBrush);
	DeleteObject(color);
}

COLORREF pickColor(HWND parent, COLORREF initColor) {
	CHOOSECOLOR ccs;
	static COLORREF custColors[16];
	memset(&ccs, 0, sizeof(CHOOSECOLOR));
	ccs.lStructSize = sizeof(CHOOSECOLOR);
	ccs.hwndOwner = parent;
	ccs.hInstance = NULL;
	ccs.rgbResult = initColor;
	ccs.lpCustColors = custColors;
	ccs.Flags = CC_ANYCOLOR | CC_FULLOPEN | CC_RGBINIT | CC_SOLIDCOLOR;
	if (ChooseColor(&ccs)) {
		return ccs.rgbResult;
	}
	return initColor;
}

INT_PTR CALLBACK ProjectSettingsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			if (!gProject || !gProject->chartProject) {
				EndDialog(hDlg, false);
				return false;
			} else {
				SetDlgItemInt(hDlg, IDC_SELLINEWIDTH, UINT(gProject->chartProject->selectedSettings.lineSize), true);
				SetDlgItemInt(hDlg, IDC_SELNODESIZE, UINT(gProject->chartProject->selectedSettings.nodeSize), true);
				gSelLineColor = gProject->chartProject->selectedSettings.lineColor;
				gSelNodeColor = gProject->chartProject->selectedSettings.nodeColor;

				SetDlgItemInt(hDlg, IDC_UNSELLINEWIDTH, UINT(gProject->chartProject->notSelectedSettings.lineSize), true);
				SetDlgItemInt(hDlg, IDC_UNSELNODESIZE, UINT(gProject->chartProject->notSelectedSettings.nodeSize), true);
				gNotSelLineColor = gProject->chartProject->notSelectedSettings.lineColor;
				gNotSelNodeColor = gProject->chartProject->notSelectedSettings.nodeColor;

				SetDlgItemInt(hDlg, IDC_INACTIVELINEWIDTH, UINT(gProject->chartProject->notActiveSettings.lineSize), true);
				SetDlgItemInt(hDlg, IDC_INACTIVENODESIZE, UINT(gProject->chartProject->notActiveSettings.nodeSize), true);
				gInactiveLineColor = gProject->chartProject->notActiveSettings.lineColor;
				gInactiveNodeColor = gProject->chartProject->notActiveSettings.nodeColor;
			}
		}
		return (INT_PTR)TRUE;

	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* pdis = (DRAWITEMSTRUCT*)lParam;
			switch (wParam) {
			case IDC_SELLINECOLOR:
				drawColorButton(pdis, gSelLineColor);
				break;
			case IDC_SELNODECOLOR:
				drawColorButton(pdis, gSelNodeColor);
				break;
			case IDC_UNSELLINECOLOR:
				drawColorButton(pdis, gNotSelLineColor);
				break;
			case IDC_UNSELNODECOLOR:
				drawColorButton(pdis, gNotSelNodeColor);
				break;
			case IDC_INACTIVELINECOLOR:
				drawColorButton(pdis, gInactiveLineColor);
				break;
			case IDC_INACTIVENODECOLOR:
				drawColorButton(pdis, gInactiveNodeColor);
				break;
			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			//Save settings
			if (gProject && gProject->chartProject) {
				int selLineSize = max(1, int(GetDlgItemInt(hDlg, IDC_SELLINEWIDTH, nullptr, true)));
				int selNodeSize = max(1, int(GetDlgItemInt(hDlg, IDC_SELNODESIZE, nullptr, true)));
				int notSelLineSize = max(1, int(GetDlgItemInt(hDlg, IDC_UNSELLINEWIDTH, nullptr, true)));
				int notSelNodeSize = max(1, int(GetDlgItemInt(hDlg, IDC_UNSELNODESIZE, nullptr, true)));
				int inactiveLineSize = max(1, int(GetDlgItemInt(hDlg, IDC_INACTIVELINEWIDTH, nullptr, true)));
				int inactiveNodeSize = max(1, int(GetDlgItemInt(hDlg, IDC_INACTIVENODESIZE, nullptr, true)));
				gProject->chartProject->selectedSettings= LineDisplaySettings(selLineSize, selNodeSize, gSelLineColor, gSelNodeColor);
				gProject->chartProject->notSelectedSettings= LineDisplaySettings(notSelLineSize, notSelNodeSize, gNotSelLineColor, gNotSelNodeColor);
				gProject->chartProject->notActiveSettings= LineDisplaySettings(inactiveLineSize, inactiveNodeSize, gInactiveLineColor, gInactiveNodeColor);
				gProject->needsRedraw();
			}
			EndDialog(hDlg, false);
			return (INT_PTR)TRUE;

		case IDC_SELLINECOLOR:
			gSelLineColor = pickColor(hDlg, gSelLineColor);
			break;
		case IDC_SELNODECOLOR:
			gSelNodeColor = pickColor(hDlg, gSelNodeColor);
			break;
		case IDC_UNSELLINECOLOR:
			gNotSelLineColor = pickColor(hDlg, gNotSelLineColor);
			break;
		case IDC_UNSELNODECOLOR:
			gNotSelNodeColor = pickColor(hDlg, gNotSelNodeColor);
			break;
		case IDC_INACTIVELINECOLOR:
			gInactiveLineColor = pickColor(hDlg, gInactiveLineColor);
			break;
		case IDC_INACTIVENODECOLOR:
			gInactiveNodeColor = pickColor(hDlg, gInactiveNodeColor);
			break;
		}
		InvalidateRect(hDlg, nullptr, true);
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK BoundsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			gChartObjectToEdit = reinterpret_cast<CChartObject*>(lParam);
			if (!gProject || !gProject->chartProject || !gChartObjectToEdit || !gChartObjectToEdit->toChartBase()) {
				EndDialog(hDlg, false);
				return false;
			}

			CChartBase* chartBase = gChartObjectToEdit->toChartBase();
			setDlgItemDouble(hDlg, IDC_RIGHTBOUND, chartBase->bounds.rightValue);
			setDlgItemDouble(hDlg, IDC_LEFTBOUND, chartBase->bounds.leftValue);
			setDlgItemDouble(hDlg, IDC_TOPBOUND, chartBase->bounds.topValue);
			setDlgItemDouble(hDlg, IDC_BOTTOMBOUND, chartBase->bounds.bottomValue);
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (gChartObjectToEdit){
				CChartBase* chartBase = gChartObjectToEdit->toChartBase();

				if (chartBase) {
					chartBase->bounds.rightValue = getDlgItemDouble(hDlg, IDC_RIGHTBOUND);
					chartBase->bounds.leftValue = getDlgItemDouble(hDlg, IDC_LEFTBOUND);
					chartBase->bounds.topValue = getDlgItemDouble(hDlg, IDC_TOPBOUND);
					chartBase->bounds.bottomValue = getDlgItemDouble(hDlg, IDC_BOTTOMBOUND);
				}
				if (gProject && gProject->chartProject) {
					gProject->chartProject->makeObjectNamesUnique();
					gProject->updateObjectsList();
					gProject->needsRedraw();
				}
			}
			EndDialog(hDlg, true);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, false);
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK EditConstantProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			gChartObjectToEdit = reinterpret_cast<CChartObject*>(lParam);

			if (!gProject || !gProject->chartProject || !gChartObjectToEdit || !gChartObjectToEdit->toConstant()) {
				EndDialog(hDlg, false);
				return false;
			}

			gVariableToEdit = gChartObjectToEdit->toConstant();

			SetDlgItemTextA(hDlg, IDC_NAME, gVariableToEdit->getName().c_str());
			setDlgItemDouble(hDlg, IDC_VALUE, gVariableToEdit->result.value_or(0.0f));
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (gVariableToEdit && gProject && gProject->chartProject) {
				if (tryAndSetObjectName(hDlg, gProject->chartProject, gVariableToEdit, CChartObject::getFormattedName(getDlgItemString(hDlg, IDC_NAME))) == false) {
					break;
				}

				gVariableToEdit->result = getDlgItemDouble(hDlg, IDC_VALUE);

				gProject->chartProject->makeObjectNamesUnique();
				gProject->updateConstantsList();
				gProject->updateObjectsList();
				gProject->needsRedraw();
			}
			EndDialog(hDlg, true);
			break;
		case IDCANCEL:
			EndDialog(hDlg, false);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}

void deleteEditor() {
	if (gSCEditor) {
		delete gSCEditor;
		gSCEditor = nullptr;
	}
}

void setupEditor(HWND hDlg) {
	deleteEditor();
	gSCEditor = new SyntaxColoredEditor(hDlg, 0, 0, 500, 500);
}


const int CheckForModTimerID = 1;
const int CompileTimerID = 2;
INT_PTR CALLBACK EditScriptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{

			CScript* scriptObject = reinterpret_cast<CScript*>(lParam);
			gChartObjectToEdit = scriptObject;
			if (!gProject || !gProject->chartProject || !scriptObject) {
				EndDialog(hDlg, false);
				return false;
			}


			setupEditor(hDlg);

			gProject->chartProject->validateInputs();

			//init dialog items
			SetDlgItemTextA(hDlg, IDC_NAME, scriptObject->getName().c_str());

			string whyItsBad;
			scriptObject->checkIsBad(gProject->chartProject, whyItsBad);
			SetDlgItemTextA(hDlg, IDC_COMPILEERROR, whyItsBad.c_str());

			gSCEditor->setText(scriptObject->getCode());

			SendDlgItemMessageA(hDlg, IDC_EXPORTRESULT, BM_SETCHECK, scriptObject->exportResult ? BST_CHECKED : BST_UNCHECKED, 0);

			//Set timer to recompile and update error box
			SetTimer(hDlg, CheckForModTimerID, 500, nullptr);
		}
		//Fall through on purpose
	case WM_SIZE:
		{
			RECT clientRect;
			RECT nameRect;
			RECT resultRect;
			RECT errorRect;
			RECT nameStaticRect;

			GetClientRect(hDlg, &clientRect);

			GetWindowRect(GetDlgItem(hDlg, IDC_NAME), &nameRect);
			GetWindowRect(GetDlgItem(hDlg, IDC_EXPORTRESULT), &resultRect);
			GetWindowRect(GetDlgItem(hDlg, IDC_COMPILEERROR), &errorRect);
			GetWindowRect(GetDlgItem(hDlg, IDC_NAMESTATIC), &nameStaticRect);
			ScreenToClient(hDlg, (POINT*)&nameRect.left);
			ScreenToClient(hDlg, (POINT*)&nameRect.right);
			ScreenToClient(hDlg, (POINT*)&resultRect.left);
			ScreenToClient(hDlg, (POINT*)&resultRect.right);
			ScreenToClient(hDlg, (POINT*)&errorRect.left);
			ScreenToClient(hDlg, (POINT*)&errorRect.right);
			ScreenToClient(hDlg, (POINT*)&nameStaticRect.left);
			ScreenToClient(hDlg, (POINT*)&nameStaticRect.right);

			nameRect.left = nameStaticRect.right + 1;
			nameRect.right = clientRect.right - 100;
			resultRect.left = nameRect.right + 4;
			resultRect.right = clientRect.right - 4;

			errorRect.top = clientRect.bottom - 50;
			errorRect.left = clientRect.left + 4;
			errorRect.bottom = clientRect.bottom - 4;
			errorRect.right = clientRect.right - 4;
			MoveWindow(GetDlgItem(hDlg, IDC_NAME), nameRect.left, nameRect.top, nameRect.right - nameRect.left, nameRect.bottom - nameRect.top, 1);
			MoveWindow(GetDlgItem(hDlg, IDC_EXPORTRESULT), resultRect.left, resultRect.top, resultRect.right - resultRect.left, resultRect.bottom - resultRect.top, 1);
			MoveWindow(GetDlgItem(hDlg, IDC_COMPILEERROR), errorRect.left, errorRect.top, errorRect.right - errorRect.left, errorRect.bottom - errorRect.top, 1);
			if (gSCEditor) {
				gSCEditor->move(4, nameRect.bottom + 4, clientRect.right - 8, errorRect.top - 4 - nameRect.bottom);
			}
		}
		return (INT_PTR)TRUE;

	case WM_TIMER:
		if (gProject && gChartObjectToEdit) {
			if (wParam == CheckForModTimerID) {
				KillTimer(hDlg, CheckForModTimerID);
				if (gSCEditor->getModified()) {
					gSCEditor->setModified(false);
					SetTimer(hDlg, CompileTimerID, 1000, nullptr);
				} else {
					SetTimer(hDlg, CheckForModTimerID, 500, nullptr);
				}
			} else if (wParam == CompileTimerID) {
				KillTimer(hDlg, CompileTimerID);
				if (gSCEditor->getModified() == false) {
					CScript* scriptObject = reinterpret_cast<CScript*>(gChartObjectToEdit);
					if (scriptObject) {
						string whyItsBad;
						scriptObject->setCode(gSCEditor->getText());
						scriptObject->checkIsBad(gProject->chartProject, whyItsBad);
						SetDlgItemTextA(hDlg, IDC_COMPILEERROR, whyItsBad.c_str());
					}
				}
				SetTimer(hDlg, CheckForModTimerID, 500, nullptr);
			}
		}
		break;

	case WM_CLOSE:
		if (gProject && gProject->chartProject && gChartObjectToEdit) {
			CScript* scriptObject = reinterpret_cast<CScript*>(gChartObjectToEdit);
			if (scriptObject) {
				if (tryAndSetObjectName(hDlg, gProject->chartProject, gChartObjectToEdit, CChartObject::getFormattedName(getDlgItemString(hDlg, IDC_NAME))) == false) {
					break;
				}

				scriptObject->exportResult = isChecked(hDlg, IDC_EXPORTRESULT);

				scriptObject->setCode(gSCEditor->getText());

				gProject->chartProject->makeObjectNamesUnique();
				gProject->chartProject->validateInputs();
				gProject->updateObjectsList();
				gProject->needsRedraw();
			}
		}

		deleteEditor();

		KillTimer(hDlg, CheckForModTimerID);
		KillTimer(hDlg, CompileTimerID);
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK EditInputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			CInput* inputObject = reinterpret_cast<CInput*>(lParam);
			gChartObjectToEdit = inputObject;

			if (!inputObject || !gProject || !gProject->chartProject) {
				EndDialog(hDlg, LOWORD(wParam));
				return false;
			} 

			setupEditor(hDlg);
			gProject->chartProject->validateInputs();

			//init dialog items
			SetDlgItemTextA(hDlg, IDC_NAME, inputObject->getName().c_str());

			string whyItsBad;
			inputObject->checkIsBad(gProject->chartProject, whyItsBad);
			SetDlgItemTextA(hDlg, IDC_COMPILEERROR, whyItsBad.c_str());

			gSCEditor->setText(inputObject->getCode());

			//Set timer to recompile and update error box
			SetTimer(hDlg, CheckForModTimerID, 500, nullptr);
		}
		//Fall through on purpose
	case WM_SIZE:
		{
			RECT clientRect;
			RECT nameRect;
			RECT errorRect;
			RECT nameStaticRect;

			GetClientRect(hDlg, &clientRect);

			GetWindowRect(GetDlgItem(hDlg, IDC_NAME), &nameRect);
			GetWindowRect(GetDlgItem(hDlg, IDC_COMPILEERROR), &errorRect);
			GetWindowRect(GetDlgItem(hDlg, IDC_NAMESTATIC), &nameStaticRect);
			ScreenToClient(hDlg, (POINT*)&nameRect.left);
			ScreenToClient(hDlg, (POINT*)&nameRect.right);
			ScreenToClient(hDlg, (POINT*)&errorRect.left);
			ScreenToClient(hDlg, (POINT*)&errorRect.right);
			ScreenToClient(hDlg, (POINT*)&nameStaticRect.left);
			ScreenToClient(hDlg, (POINT*)&nameStaticRect.right);

			nameRect.left = nameStaticRect.right + 1;
			nameRect.right = clientRect.right - nameStaticRect.right - 5;

			errorRect.top = clientRect.bottom - 50;
			errorRect.left = clientRect.left + 4;
			errorRect.bottom = clientRect.bottom - 4;
			errorRect.right = clientRect.right - 4;
			MoveWindow(GetDlgItem(hDlg, IDC_NAME), nameRect.left, nameRect.top, nameRect.right, nameRect.bottom - nameRect.top, 1);
			MoveWindow(GetDlgItem(hDlg, IDC_COMPILEERROR), errorRect.left, errorRect.top, errorRect.right - errorRect.left, errorRect.bottom - errorRect.top, 1);
			if (gSCEditor) {
				gSCEditor->move(4, nameRect.bottom + 4, clientRect.right - 8, errorRect.top - 4 - nameRect.bottom);
			}
		}
		return (INT_PTR)TRUE;

	case WM_TIMER:
		if (gProject && gChartObjectToEdit) {
			if (wParam == CheckForModTimerID) {
				KillTimer(hDlg, CheckForModTimerID);
				if (gSCEditor->getModified()) {
					gSCEditor->setModified(false);
					SetTimer(hDlg, CompileTimerID, 1000, nullptr);
				} else {
					SetTimer(hDlg, CheckForModTimerID, 500, nullptr);
				}
			} else if (wParam == CompileTimerID) {
				KillTimer(hDlg, CompileTimerID);
				if (gSCEditor->getModified() == false) {
					CInput* inputObject = reinterpret_cast<CInput*>(gChartObjectToEdit);
					if (inputObject) {
						string whyItsBad;
						inputObject->setCode(gSCEditor->getText());
						inputObject->checkIsBad(gProject->chartProject, whyItsBad);
						SetDlgItemTextA(hDlg, IDC_COMPILEERROR, whyItsBad.c_str());
					}
				}
				SetTimer(hDlg, CheckForModTimerID, 500, nullptr);
			}
		}
		break;

	case WM_CLOSE:
		if (gProject && gProject->chartProject && gChartObjectToEdit) {
			CInput* inputObject = reinterpret_cast<CInput*>(gChartObjectToEdit);
			if (inputObject) {
				if (tryAndSetObjectName(hDlg, gProject->chartProject, gChartObjectToEdit, CChartObject::getFormattedName(getDlgItemString(hDlg, IDC_NAME))) == false) {
					break;
				}

				inputObject->setCode(gSCEditor->getText());
			}

			gProject->chartProject->makeObjectNamesUnique();
			gProject->chartProject->validateInputs();
			gProject->updateInputsList();
			gProject->updateObjectsList();
			gProject->needsRedraw();
		}
		deleteEditor();
		KillTimer(hDlg, CheckForModTimerID);
		KillTimer(hDlg, CompileTimerID);
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK EditTableItemProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			gTableItemToEdit = reinterpret_cast<CTableEntry*>(lParam);
			gChartObjectToEdit = nullptr;
			if (gProject && gProject->selectedObject) {
				gChartObjectToEdit = gProject->selectedObject->toTable();
			}
			if (!gProject || !gProject->chartProject || !gChartObjectToEdit || !gTableItemToEdit) {
				EndDialog(hDlg, false);
				return false;
			}

			gProject->chartProject->validateInputs();

			setDlgItemDouble(hDlg, IDC_VALUE, gTableItemToEdit->getValue());

			addInputsToCombo(GetDlgItem(hDlg, IDC_REFERENCE), gProject->chartProject, gChartObjectToEdit);

			SendDlgItemMessageA(hDlg, IDC_REFERENCE, CB_SELECTSTRING, -1, (LPARAM)gTableItemToEdit->chartObject);
			SendDlgItemMessageA(hDlg, IDC_REFERENCE, CB_SETMINVISIBLE, 15, 0);
		}
		return (INT_PTR)TRUE;
	case WM_DRAWITEM:
		{
			DRAWITEMSTRUCT* pdis = (DRAWITEMSTRUCT*)lParam;
			if (pdis->CtlType == ODT_COMBOBOX) {
				if (pdis->itemID == -1) {
					break;
				}
				CChartObject* chart = reinterpret_cast<CChartObject*>(pdis->itemData);

				COLORREF clrBackground;
				COLORREF clrForeground;
				TEXTMETRIC tm;
				int x, y;

				clrForeground = SetTextColor(pdis->hDC,
					GetSysColor(pdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));

				clrBackground = SetBkColor(pdis->hDC,
					GetSysColor(pdis->itemState & ODS_SELECTED ?
						COLOR_HIGHLIGHT : COLOR_WINDOW));

				// Calculate the vertical and horizontal position.
				GetTextMetrics(pdis->hDC, &tm);
				y = (pdis->rcItem.bottom + pdis->rcItem.top - tm.tmHeight) / 2;
				x = 5;

				if (chart) {
					ExtTextOut(pdis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &pdis->rcItem, chart->getName().c_str(), chart->getName().length(), NULL);
				} else {
					string emptyString = "<empty>";
					ExtTextOut(pdis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &pdis->rcItem, emptyString.c_str(), emptyString.length(), NULL);
				}
				// Restore the previous colors.
				SetTextColor(pdis->hDC, clrForeground);
				SetBkColor(pdis->hDC, clrBackground);

				if (pdis->itemState & ODS_FOCUS)
					DrawFocusRect(pdis->hDC, &pdis->rcItem);
				return 1;

			}
		}
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			{
				if (gProject && gProject->chartProject && gChartObjectToEdit && gTableItemToEdit) {
					CTable* tableObject = reinterpret_cast<CTable*>(gChartObjectToEdit);
					if (tableObject) {
						gTableItemToEdit->chartObject = getSelectedInput(hDlg, IDC_REFERENCE);
						tableObject->setItemValue(gTableItemToEdit, getDlgItemDouble(hDlg, IDC_VALUE));
					}
					gProject->chartProject->validateInputs();
					gProject->updateTableList();
					gProject->updateObjectsList();
					gProject->needsRedraw();
				}
			}
			EndDialog(hDlg, true);
			break;
		case IDCANCEL:
			EndDialog(hDlg, false);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, false);
	}
	return (INT_PTR)FALSE;
}


INT_PTR CALLBACK EditImageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			gImageToEdit = reinterpret_cast<ChartImage*>(lParam);

			if (!gProject || !gProject->chartProject || !gImageToEdit) {
				EndDialog(hDlg, false);
				return false;
			} else {
				SetDlgItemTextA(hDlg, IDC_NAME, gImageToEdit->name.c_str());
			}
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (gImageToEdit && gProject && gProject->chartProject) {
				gImageToEdit->name = getDlgItemString(hDlg, IDC_NAME);
				gProject->chartProject->sortChartImages();
				gProject->updateImagesList();
				gProject->needsRedraw();
				EndDialog(hDlg, true);
			}
			break;
		case IDCANCEL:
			EndDialog(hDlg, 0);
			break;
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}


void RunChartsDrawChartBox(DRAWITEMSTRUCT* pdis, int column) {
	if (!gProject || !gProject->chartProject) return;

	switch (column) {
	case 1:
		{
			CChartObject* chartObject = reinterpret_cast<CChartObject*>(pdis->itemData);
			string resultText = "nil";
			if (chartObject) {
				if (chartObject->result) {
					resultText = trimmedDoubleToString(*chartObject->result);
				}
			}
			DrawTextA(pdis->hDC, resultText.c_str(), -1, &pdis->rcItem, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		}
		break;
	case 0:
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
				if (chartObject) {
					if (chartObject->checkIsBad(gProject->chartProject, whyItsBad)) {
						drawBitmapStretchTransparent(pdis->hDC, gBadBitmap, pdis->rcItem.left, pdis->rcItem.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
						pdis->rcItem.left += bitmapSize + 2;
					}

					RECT textRect = pdis->rcItem;
					DrawTextA(pdis->hDC, chartObject->getName().c_str(), -1, &textRect, DT_VCENTER | DT_SINGLELINE);

					if (chartObject->exportResult) {
						drawBitmapStretchTransparent(pdis->hDC, gExportBitmap, pdis->rcItem.right - bitmapSize - 1, pdis->rcItem.top + 1, bitmapSize - 2, bitmapSize - 2, RGB(255, 255, 255));
					}
				}
			}

		}
		break;
	}
}

INT_PTR CALLBACK RunChartsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		{
			if (!gProject || !gProject->chartProject) {
				EndDialog(hDlg, 0);
				return false;
			} else {
				RunChartsData* rcd = new RunChartsData;
				if (rcd) {
					SetWindowLongPtrA(hDlg, GWL_USERDATA, (LONG)rcd);

					rcd->chartResults = new ListUI("Results", hDlg, 0, 0, 0, 0, RunChartsDrawChartBox, nullptr, nullptr, nullptr);
					rcd->chartResults->addColumn(0.6f, "Object");
					rcd->chartResults->addColumn(0.4f, "Result");
					rcd->chartResults->buttonBar->addButton(IDB_SHOW, IDC_RUN_SHOWALLOBJECTS, "Show all objects");
					rcd->chartResults->buttonBar->enableButton(IDC_RUN_SHOWALLOBJECTS, false);
					rcd->chartResults->buttonBar->addButton(IDB_HIDE, IDC_RUN_SHOWEXPORTSONLY, "Show only exports");
					rcd->chartResults->setTitleDetails("Results (all)", RGB(0, 0, 0), RGB(255, 255, 255), 25);
					for (auto & object : gProject->chartProject->objects) {
						rcd->chartResults->addString((char*)object);
					}
					for (auto & input : gProject->chartProject->inputs) {
						rcd->inputObjects.push_back(input);
						HWND newInput = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | WS_BORDER, 0, 0, 0, 0, hDlg, 0, hInst, 0);
						rcd->inputBoxes.push_back(newInput);
						SetWindowTextA(newInput, input->input.c_str());
						HWND newLabel = CreateWindowA("static", input->getName().c_str(), WS_VISIBLE | WS_CHILD | SS_CENTERIMAGE, 0, 0, 0, 0, hDlg, 0, hInst, 0);
						rcd->inputLabels.push_back(newLabel);
						HFONT windowFont = (HFONT)SendMessage(hDlg, WM_GETFONT, 0, 0);
						SendMessage(newInput, WM_SETFONT, (WPARAM)windowFont, true);
						SendMessage(newLabel, WM_SETFONT, (WPARAM)windowFont, true);
					}

					SendMessage(hDlg, WM_SIZE, 0, 0);
					SendMessage(hDlg, WM_COMMAND, MAKELONG(IDC_RUNCHARTS, BN_CLICKED), 0);
				} else {
					EndDialog(hDlg, 0);
				}
			}
		}
		return (INT_PTR)TRUE;

	case WM_SIZE:
		{
			RECT clientRect;
			GetClientRect(hDlg, &clientRect);

			int verticalPadding = 3;
			int inputHeight = 20;
			int labelWidth = 80;
			int inputWidth = 75;
			RunChartsData* rcd = reinterpret_cast<RunChartsData*>(GetWindowLongPtrA(hDlg, GWL_USERDATA));
			if (rcd) {
				SetWindowLongPtrA(hDlg, GWL_USERDATA, (LONG)rcd);
				for (size_t i = 0; i < rcd->inputObjects.size(); i++) {
					MoveWindow(rcd->inputLabels[i], 5, verticalPadding + i * inputHeight + i * verticalPadding, labelWidth, inputHeight, true);
					MoveWindow(rcd->inputBoxes[i], 5 + labelWidth, verticalPadding + i * inputHeight + i * verticalPadding, inputWidth, inputHeight, true);
				}

				MoveWindow(GetDlgItem(hDlg, IDC_RUNCHARTS), 5, verticalPadding + rcd->inputObjects.size() * inputHeight + rcd->inputObjects.size() * verticalPadding,
					labelWidth + inputWidth, 30, true);

				rcd->chartResults->move(inputWidth + labelWidth + 10, 0, clientRect.right - inputWidth - labelWidth - 10, clientRect.bottom);
			}
		}
		break;

	case WM_CTLCOLOREDIT: 
		{
			RunChartsData* rcd = reinterpret_cast<RunChartsData*>(GetWindowLongPtrA(hDlg, GWL_USERDATA));
			HWND editCtrl = reinterpret_cast<HWND>(lParam);
			for (size_t i = 0; i < rcd->inputObjects.size(); i++) {
				if (rcd->inputBoxes[i] == editCtrl) {
					CInput* inputObj = static_cast<CInput*>(rcd->inputObjects[i]);
					if (inputObj && inputObj->isInvalid()) {
						SetTextColor(reinterpret_cast<HDC>(wParam), RGB(255,0,0));
					} else {
					}
					return reinterpret_cast<INT_PTR>(GetStockObject(WHITE_BRUSH));
				}
			}
		}
		break;

	case WM_COMMAND:
		{
			RunChartsData* rcd = reinterpret_cast<RunChartsData*>(GetWindowLongPtrA(hDlg, GWL_USERDATA));
			if (rcd && gProject && gProject->chartProject) {
				switch (LOWORD(wParam)) {
				case IDC_RUN_SHOWALLOBJECTS:
					rcd->chartResults->pauseDrawing();
					rcd->chartResults->clearContents();
					for (auto & object : gProject->chartProject->objects) {
						rcd->chartResults->addString((char*)object);
					}
					rcd->chartResults->resumeDrawing();
					rcd->chartResults->setTitleDetails("Results (all)", RGB(0, 0, 0), RGB(255, 255, 255), 25);
					rcd->chartResults->buttonBar->enableButton(IDC_RUN_SHOWALLOBJECTS, false);
					rcd->chartResults->buttonBar->enableButton(IDC_RUN_SHOWEXPORTSONLY, true);
					break;
				case IDC_RUN_SHOWEXPORTSONLY:
					rcd->chartResults->pauseDrawing();
					rcd->chartResults->clearContents();
					for (auto & object : gProject->chartProject->objects) {
						if (object && object->exportResult) {
							rcd->chartResults->addString((char*)object);
						}
					}
					rcd->chartResults->resumeDrawing();
					rcd->chartResults->setTitleDetails("Results (exports only)", RGB(0, 0, 0), RGB(255, 255, 255), 25);
					rcd->chartResults->buttonBar->enableButton(IDC_RUN_SHOWEXPORTSONLY, false);
					rcd->chartResults->buttonBar->enableButton(IDC_RUN_SHOWALLOBJECTS, true);
					break;
				case IDC_RUNCHARTS:
					{
						//set input values
						for (size_t i = 0; i < rcd->inputObjects.size(); i++) {
							if (rcd->inputObjects[i]->type == ChartObjectType::Input) {
								static_cast<CInput*>(rcd->inputObjects[i])->input = getWindowString(rcd->inputBoxes[i]);
							}
						}

						//run all the calculations
						gProject->chartProject->calculate();

						rcd->chartResults->redraw();
					}
					break;
				}
			}
		}
		break;

	case WM_DESTROY:
		{
			RunChartsData* rcd = reinterpret_cast<RunChartsData*>(GetWindowLongPtrA(hDlg, GWL_USERDATA));
			if (rcd) {
				for (auto & hwnd : rcd->inputBoxes) {
					DestroyWindow(hwnd);
				}
				for (auto & hwnd : rcd->inputLabels) {
					DestroyWindow(hwnd);
				}
				rcd->inputBoxes.clear();
				rcd->inputLabels.clear();
				rcd->inputObjects.clear();
				delete rcd->chartResults;
				delete rcd;
				SetWindowLongPtrA(hDlg, GWL_USERDATA, 0);
			}
		}
		break;

	case WM_CLOSE:
		EndDialog(hDlg, 0);
		break;
	}
	return (INT_PTR)FALSE;
}