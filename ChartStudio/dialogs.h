#pragma once

INT_PTR CALLBACK    AboutDlgProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK ChartSettingsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK ProjectSettingsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK BoundsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditConstantProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditScriptProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditInputProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditTableItemProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditImageProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK RunChartsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class ProjectContainer;
class CChartObject;

extern ProjectContainer* gProject;
extern HINSTANCE hInst;
extern HBITMAP gLinearBitmap;
extern HBITMAP gTrendBitmap;
extern HBITMAP gClampBitmap;
extern HBITMAP gPolyBitmap;
extern HBITMAP gScriptBitmap;
extern HBITMAP gTableBitmap;
extern HBITMAP gBadBitmap;
extern HBITMAP gExportBitmap;

struct RunChartsData {
	vector<HWND> inputBoxes;
	vector<HWND> inputLabels;
	vector<CChartObject*> inputObjects;
	class ListUI* chartResults;
};