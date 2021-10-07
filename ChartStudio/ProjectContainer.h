#pragma once
#include <vector>
#include <string>
#include "TPoint.h"
using namespace std;

class ChartProject;
struct BoundSettings;
class ListUI;
class Toolbar;
class DoubleBuff;
class TabControl;
class Canvas;
class UIData;
class CChartObject;
class CChartBase;
class TLine;
class TPoint;
class ChartImage;

enum class CHARTSTATE {
	NOTHING,
	NEWLINE,
	ADDPOINT,
	DRAGPOINT,
	MARKLEFT,
	MARKRIGHT,
	MARKTOP,
	MARKBOTTOM,
	DRAGCHART
};


class ProjectContainer {
public:
	ProjectContainer(UIData* ui);
	~ProjectContainer();

	void setChartProject(ChartProject * chartProj);

	void updateObjectsList();
	void updateLinesList();
	void updateTableList();
	void updateConstantsList();
	void updateInputsList();
	void updateImagesList();
	void fillListboxes();

	int GetSaveFileDlg(HWND parent, const char * strFilter, DWORD flags, char * strFile);
	int GetOpenFileDlg(HWND parent, const char * strFilter, DWORD flags, char * strFile);
	vector<string> GetOpenMultipleFiles(HWND parent, const char * strFilter);

	void fileNew();
	void fileOpen();
	void fileSave();
	void fileSaveAs();

	void exportToJS();

	void newChartImages();
	void deleteChartImage(ChartImage * image);


	void validateChartState();
	void setChartState(CHARTSTATE state);

	void newChartObj(CChartObject * newObj);

	void newClampChart();
	void newLinearChart();
	void newPolyChart();
	void newTrendChart();
	void newTable();
	void newScript();

	void constantAdd();
	void constantDelete();
	void constantMoveUp();
	void constantMoveDown();

	void inputAdd();
	void inputDelete();
	void inputMoveUp();
	void inputMoveDown();

	void objectMarkBounds(int id);
	void objectMoveUp();
	void objectMoveDown();
	void objectMoveToTop();
	void objectMoveToBottom();
	void objectDelete();


	void objectListChangeSel(CChartObject * newSel);
	void linesListChangeSel(TLine * newSel);

	void imagesListSelChange(ChartImage * newSel);

	void centerOnImage(ChartImage * chart);
	void centerOnChart(CChartBase * chart);

	void setSelectedImage(ChartImage * image);

	void setSelectedObject(CChartObject * obj);
	void setSelectedLine(TLine * line);
	void setSelectedNode(TPoint * point);

	void deleteNode(TLine * line, TPoint * node);
	void deleteLine(CChartBase * chart, TLine * lineToDelete);
	void deleteObject(CChartObject * object);

	void setMouseCapture(HWND window, HCURSOR cursor, bool leftButton);
	void releaseMouseCapture(bool leftButton);

	void keyPressScratchPad(int key);
	void scratchpadEnter();
	void scratchpadBackspace();

	void lineAddPoints();
	void lineRemovePoint();
	void lineDeleteLine();
	void lineNewLine();

	void tableNewEntry();

	void tableDeleteEntry();

	void chartCharPress(int key);
	void chartKeyDown(int key);
	void chartKeyDownPriority(int key);
	void chartLButtonDown(POINT xy);
	void chartLButtonMove(POINT xy);
	void chartLButtonUp(POINT xy);
	void chartRButtonDown(POINT xy);
	void chartRButtonMove(POINT xy);
	void chartRButtonUp(POINT xye);
	void chartMouseMove(POINT xy);
	void chartMouseWheel(POINT xy, int amount);
	void uiCharPress(int key);
	void uiKeyDown(int key);
	void uiLButtonDown(POINT xy);
	void uiLButtonMove(POINT xy);
	void uiLButtonUp(POINT xy);
	void uiRButtonDown(POINT xy);
	void uiRButtonMove(POINT xy);
	void uiRButtonUp(POINT xy);
	void uiMouseMove(POINT xy);
	void uiMouseWheel(POINT xy, int amount);

	void drawSelectedChartBounds(CChartBase * chartBase);
	void drawChart(CChartBase * chartBase, TPoint& mouseXY, bool isActive, bool addingPoints, TLine* selectedLine, TPoint* selectedPoint);
	TPoint screenToChart(POINT xy, double zoom, TPoint viewOrigin);
	TPoint screenToChart(POINT xy);

	void drawChartCanvas();
	void needsRedraw();
	void beforePaintCharts();

	bool isPointOnSegment(const TPoint * xy, const TPoint * a, const TPoint * b, double padding);
	void whatChartIsAt(TPoint mouseXY, CChartBase *& thisChart);
	void whatLineIsAt(TPoint mouseXY, TLine*& thisLine, TPoint*& thisNode, TPoint*& pointIsBeforeThisNode);

	ChartImage * getChartImageAt(TPoint xy);
	bool isChartImageAt(ChartImage * image, TPoint xy);
	void chartImagesLButtonDown(POINT xy);
	void chartImagesLButtonMove(POINT xy);
	void moveChartImageWithCharts(ChartImage * image, TPoint newPos, vector<CChartBase*> dontMoveThese);
	void reorderImages(vector<CChartBase*> dontMoveThese);
	void chartImagesLButtonUp(POINT xy);

	const float dragChartImageTolerance = 5.0f;
	vector<CChartBase*> dragChartCapturedObjects;
	TPoint dragChartImageStartPos;
	TPoint dragChartImageOffset;
	TPoint dragChartStartPos;


	ChartImage* selectedImage = nullptr;

	double selectionPaddingPixels = 2.0f;

	bool hasBoundsInClipboard();
	BoundSettings* clipboardBounds = nullptr;

	bool hasLineInClipboard();
	void copyBounds(CChartBase * chart);
	void pasteBounds(CChartBase * intoThisChart);
	void pasteLine(CChartBase * intoThisChart);
	void copyLine(TLine * line);
	void needsUIUpdate();
	void updateMenus();
	void updateListUIs();
	void updateStatus();
	void updateUI();
	TLine* clipboardLine = nullptr;

	ChartProject* chartProject = nullptr;

	CHARTSTATE chartState = CHARTSTATE::NOTHING;
	CChartObject* selectedObject = nullptr;
	TLine* selectedLine = nullptr;
	TPoint* selectedNode = nullptr;

	bool leftHasCapture = false;
	bool rightHasCapture = false;
	HCURSOR rightMouseCursor = NULL;
	HCURSOR leftMouseCursor = NULL;
	
	HCURSOR handCursor;
	HCURSOR moveCursor;
	HCURSOR crossHairCursor;

	TPoint dragChartXY;

	UIData* ui = nullptr;
	DoubleBuff* chartDB = nullptr;
	DoubleBuff* uiDB = nullptr;

	bool chartsNeedDrawn = true;

	bool uiNeedsUpdate = true;

	string scratchpad;
	bool scratchpadInUse = false;
};