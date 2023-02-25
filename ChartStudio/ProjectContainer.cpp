#include "stdafx.h"
#include "Exporter.h"
#include "TLine.h"
#include "TPoint.h"
#include "TSegment.h"
#include "TRect.h"
#include "UIData.h"
#include "ButtonBar.h"
#include "TabControl.h"
#include "ListUI.h"
#include "Canvas.h"
#include "resource.h"
#include "CChartObject.h"
#include "CChartBase.h"
#include "CClampChart.h"
#include "CLinearChart.h"
#include "CPolyChart.h"
#include "CTrendChart.h"
#include "CScript.h"
#include "CTable.h"
#include "CConstant.h"
#include "CInput.h"
#include "ChartImage.h"
#include "ChartProject.h"
#include "DoubleBuff.h"
#include "ProjectContainer.h"
#include <algorithm>


ProjectContainer::ProjectContainer(UIData* ui) {
	this->ui = ui;
	this->ui->clearUI();
	this->chartDB = this->ui->chartCanvas->db;


	this->crossHairCursor = LoadCursorA(NULL, IDC_CROSS);
	this->handCursor = LoadCursorA(NULL, IDC_HAND);
	this->moveCursor = LoadCursorA(NULL, IDC_SIZEALL);

	setChartProject(new ChartProject());
}


ProjectContainer::~ProjectContainer() {
	this->ui->clearUI();
	delete this->chartProject;
}

void ProjectContainer::setChartProject(ChartProject* chartProj) {
	this->ui->clearUI();
	setChartState(CHARTSTATE::NOTHING);
	setSelectedImage(nullptr);
	setSelectedObject(nullptr);
	if (this->chartProject) {
		delete this->chartProject;
		this->chartProject = nullptr;
	}
	this->chartProject = chartProj;
	this->needsRedraw();
	this->needsUIUpdate();
	//redraw charts
	//update menus

	fillListboxes();
}


void ProjectContainer::updateObjectsList() {
	if (!ui || !ui->objectsList) return;
	int topIndex = ui->objectsList->getTopIndex();
	ui->objectsList->pauseDrawing();
		ui->objectsList->clearContents();

		if (chartProject) {
			for (auto& object : chartProject->objects) {
				ui->objectsList->addString((char*)object);
			}
		}

		ui->objectsList->setTopIndex(topIndex);
		if (selectedObject) ui->objectsList->setCurSelItemData((char*)selectedObject);
	ui->objectsList->resumeDrawing();
}

void ProjectContainer::updateLinesList() {
	if (!ui || !ui->linesList) return;
	int topIndex = ui->linesList->getTopIndex();
	ui->linesList->pauseDrawing();
	ui->linesList->clearContents();
	CChartBase* chart = selectedObject ? selectedObject->toChartBase() : nullptr;
	if (chart) {
		for (auto & line : chart->lines) {
			ui->linesList->addString((char*)line);
		}
	}
	ui->linesList->setTopIndex(topIndex);
	if (selectedLine) ui->linesList->setCurSelItemData((char*)selectedLine);
	ui->linesList->resumeDrawing();
}

void ProjectContainer::updateTableList() {
	if (!ui || !ui->tableList) return;
	int topIndex = ui->tableList->getTopIndex();
	CTableEntry* sel = (CTableEntry*)ui->tableList->getCurSelItemData();
	ui->tableList->pauseDrawing();
	ui->tableList->clearContents();


	CTable* table = selectedObject ? selectedObject->toTable() : nullptr;
	if (table) {
		for (auto & entry : table->table) {
			ui->tableList->addString((char*)entry);
		}
	}
	ui->tableList->setTopIndex(topIndex);
	if (sel) ui->tableList->setCurSelItemData((char*)sel);
	ui->tableList->resumeDrawing();
}

void ProjectContainer::updateConstantsList() {
	if (!ui || !ui->constantsList) return;
	int topIndex = ui->constantsList->getTopIndex();
	CConstant* sel = (CConstant*)ui->constantsList->getCurSelItemData();
	ui->constantsList->pauseDrawing();
	ui->constantsList->clearContents();
	if (chartProject) {
		for (auto & constant : chartProject->constants) {
			ui->constantsList->addString((char*)constant);
		}
	}
	ui->constantsList->setTopIndex(topIndex);
	if (sel) ui->constantsList->setCurSelItemData((char*)sel);
	ui->constantsList->resumeDrawing();
}

void ProjectContainer::updateInputsList() {
	if (!ui || !ui->inputsList) return;
	int topIndex = ui->inputsList->getTopIndex();
	CInput* sel = (CInput*)ui->inputsList->getCurSelItemData();
	ui->inputsList->pauseDrawing();
	ui->inputsList->clearContents();
	if (chartProject) {
		for (auto & input : chartProject->inputs) {
			ui->inputsList->addString((char*)input);
		}
	}
	ui->inputsList->setTopIndex(topIndex);
	if (sel) ui->inputsList->setCurSelItemData((char*)sel);
	ui->inputsList->resumeDrawing();
}

void ProjectContainer::updateImagesList() {
	if (!ui || !ui->imagesList) return;
	int topIndex = ui->imagesList->getTopIndex();
	ui->imagesList->pauseDrawing();
	ui->imagesList->clearContents();
	if (chartProject) {
		for (auto & image : chartProject->chartImages) {
			ui->imagesList->addString((char*)image);
		}
	}
	ui->imagesList->setTopIndex(topIndex);
	if (selectedImage) ui->imagesList->setCurSelItemData((char*)selectedImage);
	ui->imagesList->resumeDrawing();
}


void ProjectContainer::fillListboxes() {
	updateObjectsList();
	updateInputsList();
	updateConstantsList();
	updateImagesList();
}


int ProjectContainer::GetSaveFileDlg(HWND parent, const char* strFilter, DWORD flags, char* strFile) {
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.hwndOwner = parent;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = strFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = flags;
	return GetSaveFileName(&ofn);
}

int ProjectContainer::GetOpenFileDlg(HWND parent, const char* strFilter, DWORD flags, char* strFile) {
	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.hwndOwner = parent;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = strFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = flags;
	return GetOpenFileName(&ofn);
}

vector<string> ProjectContainer::GetOpenMultipleFiles(HWND parent, const char* strFilter) {
	OPENFILENAMEA ofn;
	vector<string> filePaths;
	char strFile[2048] = { 0 };

	memset(&ofn, 0, sizeof(OPENFILENAMEA));
	ofn.hwndOwner = parent;
	ofn.lStructSize = sizeof(OPENFILENAMEA);
	ofn.lpstrFile = strFile;
	ofn.nMaxFile = sizeof(strFile);
	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 0;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	if (GetOpenFileName(&ofn) == TRUE) {
		char *p = ofn.lpstrFile;
		string path = p;
		p += path.size() + 1;
		if (*p == 0) {
			// there is only one string, being the full path to the file
			filePaths.push_back(path.c_str());
		} else {
			// multiple files follow the directory
			for (; *p != 0; ) {
				string fileName = p;
				filePaths.push_back(path + "\\" + fileName.c_str());
				p += fileName.size() + 1;
			}
		}
	}

	return filePaths;
}

void ProjectContainer::fileNew() {
	if (MessageBox(ui->hwnd, "Are you sure you want to start a new project?", "Did you save?", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDOK) {
		return;
	}

	setChartProject(new ChartProject());
	SetWindowText(this->ui->hwnd, "ChartStudio");

}
void ProjectContainer::fileOpen() {
	char szFile[MAX_PATH];
	if (MessageBox(ui->hwnd, "Are you sure you want to open a file?", "Did you save?", MB_OKCANCEL | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDOK) {
		return;
	}

	if (GetOpenFileDlg(ui->hwnd, "Chart Project\0*.chartproject\0", OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, szFile)) {
	} else {
		return;
	}
	setChartProject(new ChartProject(szFile));

	string title = "ChartStudio ";
	title += szFile;
	SetWindowText(this->ui->hwnd, title.c_str());
}

void ProjectContainer::fileSave() {
	if (chartProject) {
		if (chartProject->hasSaveLocation()) {
			chartProject->save();
		} else {
			fileSaveAs();
		}
	}
}

void ProjectContainer::fileSaveAs() {
	if (chartProject) {
		char szFile[MAX_PATH];
		if (GetSaveFileDlg(ui->hwnd, "Chart Project\0*.chartproject\0", OFN_PATHMUSTEXIST, szFile)) {
			string file = szFile;
			if (file.find('.') == string::npos) {
				file += ".chartproject";
			}
			chartProject->saveAs(file);

			string title = "ChartStudio ";
			title += file;
			SetWindowText(this->ui->hwnd, title.c_str());
		}
	}
}

void ProjectContainer::exportToJS() {
	char szFile[MAX_PATH];
	if (!GetSaveFileDlg(ui->hwnd, "JS File\0*.js\0", OFN_PATHMUSTEXIST, szFile)) return;
	string file = szFile;
	if (file.find('.') == string::npos) {
		file += ".js";
	}

	Exporter exporter;
	exporter.exportToJS(this->chartProject, file);
}

void ProjectContainer::newChartImages() {
	vector<string> files = GetOpenMultipleFiles(ui->hwnd, "Img\0*.bmp;*.png;*.jpg;*.jpeg;*.tiff\0");
	if (files.size() > 0) {
		ChartImage* newlyAdded = nullptr;
		for (auto & file : files) {
			newlyAdded = chartProject->addImage(file);
		}
		ui->imagesList->setCurSelItemData((char*)newlyAdded);
		needsRedraw();
		updateImagesList();
		needsUIUpdate();
	}
}
void ProjectContainer::deleteChartImage(ChartImage* image) {
	if (image) {
		if (selectedImage == image) {
			setSelectedImage(nullptr);
		}
		chartProject->deleteImage(image);
		updateImagesList();
		needsRedraw();
		needsUIUpdate();
	}
}

void ProjectContainer::validateChartState() {
	switch (chartState) {
	case CHARTSTATE::DRAGPOINT:
		if (!selectedNode) {
			chartState = CHARTSTATE::NOTHING;
		}
		break;
	case CHARTSTATE::ADDPOINT:
		if (!selectedLine) {
			chartState = CHARTSTATE::NOTHING;
		}
		if (selectedNode) {
			setSelectedNode(nullptr);
		}
		break;
	case CHARTSTATE::MARKBOTTOM:
	case CHARTSTATE::MARKLEFT:
	case CHARTSTATE::MARKRIGHT:
	case CHARTSTATE::MARKTOP:
		if (!selectedObject) {
			chartState = CHARTSTATE::NOTHING;
		}
		break;
	case CHARTSTATE::NEWLINE:
		if (!selectedObject) {
			chartState = CHARTSTATE::NOTHING;
		}
		break;
	case CHARTSTATE::DRAGCHART:
		if (!selectedImage) {
			chartState = CHARTSTATE::NOTHING;
		}
		break;
	}
}

void ProjectContainer::setChartState(CHARTSTATE state) {
	chartState = state;
	validateChartState();
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::newChartObj(CChartObject* newObj) {
	if (chartProject) chartProject->addChart(newObj);
	if (!ui) return;
	updateObjectsList();
	setSelectedObject(newObj);
	needsUIUpdate();
}

void ProjectContainer::newClampChart() {
	newChartObj(new CClampChart("new clamp", true, false, BoundSettings(), nullptr, nullptr));
}
void ProjectContainer::newTrendChart() {
	newChartObj(new CTrendChart("new trend", true, false, BoundSettings(), vector<TLine*>{}));
}
void ProjectContainer::newPolyChart() {
	newChartObj(new CPolyChart("new poly", true, BoundSettings(), vector<TLine*>{}));
}
void ProjectContainer::newLinearChart() {
	newChartObj(new CLinearChart("new linear", true, false, BoundSettings(), vector<TLine*>{}));
}
void ProjectContainer::newTable() {
	newChartObj(new CTable("new table", true));
}
void ProjectContainer::newScript() {
	newChartObj(new CScript("new script", true, "exit null;"));
}



void ProjectContainer::constantAdd() {
	if (chartProject && ui) {
		CConstant* newConstant = chartProject->newConstant("new constant", 0.0f);
		updateConstantsList();
		ui->constantsList->setCurSelItemData((char*)newConstant);
		needsUIUpdate();
	}
}
void ProjectContainer::constantDelete() {
	if (chartProject && ui) {
		CConstant* selConstant = (CConstant*)ui->constantsList->getCurSelItemData();
		chartProject->deleteConstant(selConstant);
		updateInputsList();
		updateConstantsList();
		updateObjectsList();
		updateTableList();
		needsUIUpdate();
	}
}
void ProjectContainer::constantMoveUp() {
	if (chartProject && ui) {
		CConstant* selConstant = (CConstant*)ui->constantsList->getCurSelItemData();
		chartProject->moveConstantUp(selConstant);
		updateConstantsList();
		needsUIUpdate();
	}
}
void ProjectContainer::constantMoveDown() {
	if (chartProject && ui) {
		CConstant* selConstant = (CConstant*)ui->constantsList->getCurSelItemData();
		chartProject->moveConstantDown(selConstant);
		updateConstantsList();
		needsUIUpdate();
	}
}


void ProjectContainer::inputAdd() {
	if (chartProject && ui) {
		CInput* newInput = chartProject->newInput("new input", "0", "exit double(input);");
		updateInputsList();
		ui->inputsList->setCurSelItemData((char*)newInput);
		needsUIUpdate();
	}
}
void ProjectContainer::inputDelete() {
	if (chartProject && ui) {
		CInput* selInput = (CInput*)ui->inputsList->getCurSelItemData();
		chartProject->deleteInput(selInput);
		updateInputsList();
		updateObjectsList();
		updateTableList();
		needsUIUpdate();
	}
}
void ProjectContainer::inputMoveUp() {
	if (chartProject && ui) {
		CInput* selInput = (CInput*)ui->inputsList->getCurSelItemData();
		chartProject->moveInputUp(selInput);
		updateInputsList();
		needsUIUpdate();
	}
}
void ProjectContainer::inputMoveDown() {
	if (chartProject && ui) {
		CInput* selInput = (CInput*)ui->inputsList->getCurSelItemData();
		chartProject->moveInputDown(selInput);
		updateInputsList();
		needsUIUpdate();
	}
}


void ProjectContainer::objectMarkBounds(int id) {
	if (selectedObject && selectedObject->toChartBase()) {
		switch (id) {
		case ID_CHARTS_MARKLEFT:
			setChartState(CHARTSTATE::MARKLEFT);
			break;
		case ID_CHARTS_MARKTOP:
			setChartState(CHARTSTATE::MARKTOP);
			break;
		case ID_CHARTS_MARKRIGHT:
			setChartState(CHARTSTATE::MARKRIGHT);
			break;
		case ID_CHARTS_MARKBOTTOM:
			setChartState(CHARTSTATE::MARKBOTTOM);
			break;
		default:
			return;
		}
		needsRedraw();
		needsUIUpdate();
	}
}

void ProjectContainer::objectDelete() {
	if (!selectedObject) return;
	deleteObject(selectedObject);
	needsUIUpdate();
}

void ProjectContainer::objectMoveUp() {
	if (selectedObject) {
		if (chartProject) {
			if (chartProject->canMoveChartUp(selectedObject)) {
				chartProject->moveChartUp(selectedObject);
				updateObjectsList();
				needsUIUpdate();
				if (selectedObject->toTable()) {
					updateTableList();
				}
			}
		}
	}
}
void ProjectContainer::objectMoveDown() {
	if (selectedObject) {
		if (chartProject) {
			if (chartProject->canMoveChartDown(selectedObject)) {
				chartProject->moveChartDown(selectedObject);
				updateObjectsList();
				needsUIUpdate();
				if (selectedObject->toTable()) {
					updateTableList();
				}
			}
		}
	}
}
void ProjectContainer::objectMoveToTop() {
	if (selectedObject) {
		if (chartProject) {
			while (chartProject->canMoveChartUp(selectedObject)) {
				chartProject->moveChartUp(selectedObject);
			}
			updateObjectsList();
			needsUIUpdate();
			if (selectedObject->toTable()) {
				updateTableList();
			}
		}
	}
}
void ProjectContainer::objectMoveToBottom() {
	if (selectedObject) {
		if (chartProject) {
			while (chartProject->canMoveChartDown(selectedObject)) {
				chartProject->moveChartDown(selectedObject);
			}
			updateObjectsList();
			needsUIUpdate();
			if (selectedObject->toTable()) {
				updateTableList();
			}
		}
	}

}

void ProjectContainer::objectListChangeSel(CChartObject* newSel) {
	setSelectedObject(newSel);
}


void ProjectContainer::linesListChangeSel(TLine* newSel) {
	setSelectedLine(newSel);
}

void ProjectContainer::imagesListSelChange(ChartImage* newSel) {
	setSelectedImage(newSel);
}


void ProjectContainer::centerOnImage(ChartImage* image) {
	if (chartProject) {
		if (image) {
			double screenW = ui->chartCanvas->width();
			double screenH = ui->chartCanvas->height();

			TPoint chartSize = image->getSize();

			double widthZoom = screenW / chartSize.x;
			double heightZoom = screenH / chartSize.y;


			chartProject->viewState.zoom = min(50, min(widthZoom, heightZoom));
			chartProject->viewState.xy = TPoint(0,0)-image->getPosition();
		}
	}
	needsRedraw();
	needsUIUpdate();
}


void ProjectContainer::centerOnChart(CChartBase* chart) {
	if (chartProject) {
		if (chart) {
			double screenW = ui->chartCanvas->width();
			double screenH = ui->chartCanvas->height();

			double chartW = chart->bounds.rightPosition - chart->bounds.leftPosition;
			double chartH = chart->bounds.bottomPosition - chart->bounds.topPosition;

			double widthZoom = screenW / chartW;
			double heightZoom = screenH / chartH;


			chartProject->viewState.zoom = min(50,min(widthZoom, heightZoom));
			chartProject->viewState.xy = TPoint(0 - chart->bounds.leftPosition, 0 - chart->bounds.topPosition);
		}
	}
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::setSelectedImage(ChartImage* image) {
	if (selectedImage == image) return;
	selectedImage = image;
	updateImagesList();
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::setSelectedObject(CChartObject* obj) {
	if (selectedObject == obj) return;
	selectedObject = obj;
	selectedLine = nullptr;
	selectedNode = nullptr;
	setChartState(CHARTSTATE::NOTHING);

	ui->tableList->clearContents();
	if (selectedObject) {
		if (selectedObject->toChartBase()) {
			updateLinesList();
		} else if (selectedObject->toTable()) {
			updateTableList();
		}
	}

	if (obj) {
		ui->objectsList->setCurSelItemData((char*)obj);
	} else {
		ui->objectsList->setCurSel(-1);
	}
	SendMessage(ui->hwnd, WM_SIZE, 0, 0);
	needsRedraw();
	needsUIUpdate();
}
void ProjectContainer::setSelectedLine(TLine* line) {
	if (selectedLine == line) return;
	selectedLine = line;
	selectedNode = nullptr;
	setChartState(CHARTSTATE::NOTHING);

	if (line) {
		ui->linesList->setCurSelItemData((char*)line);
	} else {
		ui->linesList->setCurSel(-1);
	}
	
	needsRedraw();
	needsUIUpdate();
}
void ProjectContainer::setSelectedNode(TPoint* point) {
	if (selectedNode == point) return;
	selectedNode = point;
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::deleteNode(TLine* line, TPoint* node) {
	if (node && line) {
		if (node == selectedNode) {
			setSelectedNode(nullptr);
		}
		line->deleteNode(node);
		updateLinesList();
		updateObjectsList();//incase the updated lines mark an object as bad
		needsRedraw();
		needsUIUpdate();
	}
}

void ProjectContainer::deleteLine(CChartBase* chart, TLine* lineToDelete) {
	if (chart && lineToDelete) {
		if (selectedLine == lineToDelete) {
			setSelectedLine(nullptr);
		}
		chart->deleteLine(lineToDelete);
		updateLinesList();
		updateObjectsList();//incase the updated lines mark an object as bad
		needsUIUpdate();
	}
}

void ProjectContainer::deleteObject(CChartObject* object) {
	if (object) {
		if (selectedObject == object) {
			setSelectedObject(nullptr);
		}
		if (chartProject) chartProject->deleteChart(object);
		updateObjectsList();
		needsUIUpdate();
	}
}

void ProjectContainer::setMouseCapture(HWND window, HCURSOR cursor, bool leftButton) {
	if (leftButton) {
		leftHasCapture = true;
		leftMouseCursor = cursor;
	} else {
		rightHasCapture = true;
		rightMouseCursor = cursor;
	}
	SetCapture(window);
	SetCursor(cursor);
}

void ProjectContainer::releaseMouseCapture(bool leftButton) {
	if (leftButton) {
		leftHasCapture = false;
		leftMouseCursor = NULL;
	} else {
		rightHasCapture = false;
		rightMouseCursor = NULL;
	}
	if (leftHasCapture) {
		SetCursor(leftMouseCursor);
	}
	if (rightHasCapture) {
		SetCursor(rightMouseCursor);
	}
	if (leftHasCapture == false && rightHasCapture == false) {
		ReleaseCapture();
	}
}

void ProjectContainer::keyPressScratchPad(int key) {
	if (!scratchpadInUse) {
		scratchpad.clear();
	}

	bool hasDec = scratchpad.find('.') != string::npos;
	if ((key == '.' && !hasDec) || key != '.') {
		if ((scratchpad.length() == 0 && key == '-') || key != '-') {
			scratchpad.push_back(key);
			scratchpadInUse = true;
		}
	}

	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::scratchpadEnter() {
	//apply scratchpad number to whatever is selected
	if (scratchpadInUse == false) return;
	CChartBase* selChart = selectedObject ? selectedObject->toChartBase() : nullptr;
	if (selChart) {
		if (!scratchpad.empty()) {
			double value = strtod(scratchpad.c_str(), nullptr);
			switch (chartState) {
			case CHARTSTATE::MARKLEFT:
				selChart->bounds.leftValue = value;
				scratchpadInUse = false;
				break;
			case CHARTSTATE::MARKRIGHT:
				selChart->bounds.rightValue = value;
				scratchpadInUse = false;
				break;
			case CHARTSTATE::MARKTOP:
				selChart->bounds.topValue = value;
				scratchpadInUse = false;
				break;
			case CHARTSTATE::MARKBOTTOM:
				selChart->bounds.bottomValue = value;
				scratchpadInUse = false;
				break;
			default:
				if (!selChart->toClampChart() && selectedLine) {
					selChart->setLineValue(selectedLine, value);
					updateLinesList();
					scratchpadInUse = false;
				}
				break;
			}
		}
	}
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::scratchpadInsert() {
	if (scratchpadInUse == false) return;
	CLinearChart* selLinearChart = selectedObject ? selectedObject->toLinearChart() : nullptr;
	if (selLinearChart) {
		if (!scratchpad.empty()) {
			double value = strtod(scratchpad.c_str(), nullptr);
			TLine* createdLine = nullptr;
			if (selLinearChart->autoGenerateLine(value, &createdLine)) {
				scratchpadInUse = false;
				setSelectedLine(createdLine);
				updateLinesList();
			}
		}
	}
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::scratchpadBackspace() {
	if (scratchpad.length() > 0) {
		scratchpad.pop_back();
	}

	needsRedraw();
}
void ProjectContainer::lineAddPoints() {
	if (chartState == CHARTSTATE::ADDPOINT) {
		if (selectedLine) {
			selectedLine->reverseOrder();
		}
	} else {
		setChartState(CHARTSTATE::ADDPOINT);
	}
	needsRedraw();
}

void ProjectContainer::lineRemovePoint() {
	if (selectedLine && chartState == CHARTSTATE::ADDPOINT && !leftHasCapture) {
		if (selectedLine->points.size() > 0) {
			deleteNode(selectedLine, selectedLine->points.back());
		}
	}
	needsRedraw();
}

void ProjectContainer::lineDeleteLine() {
	if (selectedLine && selectedObject) {
		deleteLine(selectedObject->toChartBase(), selectedLine);
	}
}

void ProjectContainer::lineNewLine() {
	setChartState(CHARTSTATE::NEWLINE);
}

void ProjectContainer::tableNewEntry() {
	if (!selectedObject) return;
	CTable* table = selectedObject->toTable();
	if (table) {
		CTableEntry* entry = new CTableEntry(nullptr, 0);
		table->addTableEntry(entry);
		updateTableList();
		ui->tableList->setCurSelItemData((char*)entry);
		updateObjectsList();
		needsUIUpdate();
	}
}

void ProjectContainer::tableDeleteEntry() {
	if (!selectedObject) return;
	CTable* table = selectedObject->toTable();
	if (table) {
		CTableEntry* selEntry = (CTableEntry*)ui->tableList->getCurSelItemData();
		if (selEntry) {
			table->deleteTableEntry(selEntry);
			updateTableList();
			updateObjectsList();
			needsUIUpdate();
		}
	}
}

void ProjectContainer::chartCharPress(int key) {
	switch (key) {
	case 'a':
		lineAddPoints();
		break;

	case 'n':
		lineNewLine();
		break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
	case '.':
		keyPressScratchPad(key);
		break;
	}

	needsUIUpdate();
}

void ProjectContainer::chartKeyDownPriority(int key) {
	switch (key) {
	case VK_NEXT:
		objectMoveDown();
		break;

	case VK_PRIOR:
		objectMoveUp();
		break;
	}
}

void ProjectContainer::chartKeyDown(int key) {
	switch (key) {
	case VK_DELETE:
		if (selectedNode && selectedLine) {
			deleteNode(selectedLine, selectedNode);
		} else if (selectedLine && selectedObject) {
			deleteLine(selectedObject->toChartBase(), selectedLine);
		} else if (selectedObject) {
			deleteObject(selectedObject);
		}
		break;

	case VK_ESCAPE:
		if (scratchpadInUse) {
			scratchpadInUse = false;
		} else if (chartState != CHARTSTATE::NOTHING) {
			if (leftHasCapture) {
				releaseMouseCapture(true);
			}
			setChartState(CHARTSTATE::NOTHING);
		} else if (selectedNode) {
			setSelectedNode(nullptr);
		} else if (selectedLine) {
			setSelectedLine(nullptr);
		} else if (selectedObject) {
			setSelectedObject(nullptr);
		}
		break;

	case VK_BACK:
		if (scratchpadInUse) {
			scratchpadBackspace();
		} else if (selectedLine && chartState == CHARTSTATE::ADDPOINT && !leftHasCapture) {
			lineRemovePoint();
		}
		break;

	case VK_RETURN:
		scratchpadEnter();
		break;
	case VK_INSERT:
		scratchpadInsert();
		break;
	}
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::chartLButtonDown(POINT xy) {
	TPoint mouseXY = screenToChart(xy);
	CChartBase* selChart = selectedObject ? selectedObject->toChartBase() : nullptr;

	switch (chartState) {
	case CHARTSTATE::MARKLEFT:
		if (selChart) {
			selChart->bounds.leftPosition = mouseXY.x;
			setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
		}
		break;
	case CHARTSTATE::MARKTOP:
		if (selChart) {
			selChart->bounds.topPosition = mouseXY.y;
			setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
		}
		break;
	case CHARTSTATE::MARKRIGHT:
		if (selChart) {
			selChart->bounds.rightPosition = mouseXY.x;
			setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
		}
		break;
	case CHARTSTATE::MARKBOTTOM:
		if (selChart) {
			selChart->bounds.bottomPosition = mouseXY.y;
			setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
		}
		break;

	case CHARTSTATE::NEWLINE:
		if (selChart) {
			if (!selChart->toClampChart()) {
				TLine* newLine = new TLine();
				selChart->addLine(newLine);
				setSelectedLine(newLine);
				updateLinesList();
				setChartState(CHARTSTATE::ADDPOINT);
			}
		} else {
			setChartState(CHARTSTATE::NOTHING);
			break;
		}
		//fall through on purpose
	case CHARTSTATE::ADDPOINT:
		if (selectedLine) {
			setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
		}
		break;


	case CHARTSTATE::NOTHING:
		if (selChart) {
			TLine* clickedLine = nullptr;
			TPoint* clickedLineBeforeThis = nullptr;
			TPoint* clickedPoint = nullptr;

			whatLineIsAt(mouseXY, clickedLine, clickedPoint, clickedLineBeforeThis);

			if (selectedNode && selectedNode == clickedPoint) {
				//drag node
				setChartState(CHARTSTATE::DRAGPOINT);
				setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);

			} else if (selectedLine && selectedLine == clickedLine) {
				if (clickedPoint) {
					//select and drag node
					setSelectedNode(clickedPoint);
					setChartState(CHARTSTATE::DRAGPOINT);
					setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
				} else {
					//add node
					TPoint* newNode = new TPoint(mouseXY.x, mouseXY.y);
					selectedLine->insertNode(newNode, clickedLineBeforeThis);
					setSelectedNode(newNode);
					setChartState(CHARTSTATE::DRAGPOINT);
					setMouseCapture(ui->chartCanvas->hwnd, crossHairCursor, true);
				}
			} else {
				if (clickedLine) {
					//a different line was clicked
					setSelectedLine(clickedLine);
				} else if (selectedLine) {
					//click occured off the selected line
					if (selectedNode) {
						//node was selected, now deselect it
						setSelectedNode(nullptr);
					} else {
						//no line was selected, now deselect any currently selected line
						setSelectedLine(nullptr);
					}
				} else {
					if (selChart) {
						if (!selChart->boundPosToRect().isPointInside(mouseXY)) {
							CChartBase* clickedChart = nullptr;
							whatChartIsAt(mouseXY, clickedChart);
							setSelectedObject(clickedChart);
						}
					} else {
						CChartBase* clickedChart = nullptr;
						whatChartIsAt(mouseXY, clickedChart);
						if (clickedChart) {
							setSelectedObject(clickedChart);
						}
					}
				}
			}
		} else {
			CChartBase* clickedChart = nullptr;
			whatChartIsAt(mouseXY, clickedChart);
			if (clickedChart) {
				setSelectedObject(clickedChart);
			}
		}
		break;
	}
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::chartLButtonMove(POINT xy) {
	if (chartState == CHARTSTATE::NOTHING) return;

	CChartBase* selChart = selectedObject ? selectedObject->toChartBase() : nullptr;

	TPoint mouseXY = screenToChart(xy);

	switch (chartState) {
	case CHARTSTATE::MARKLEFT:
		if (selChart) selChart->bounds.leftPosition = mouseXY.x;
		break;
	case CHARTSTATE::MARKTOP:
		if (selChart) selChart->bounds.topPosition = mouseXY.y;
		break;
	case CHARTSTATE::MARKRIGHT:
		if (selChart) selChart->bounds.rightPosition = mouseXY.x;
		break;
	case CHARTSTATE::MARKBOTTOM:
		if (selChart) selChart->bounds.bottomPosition = mouseXY.y;
		break;

	case CHARTSTATE::DRAGPOINT:
		if (selectedNode) {
			selectedNode->x = mouseXY.x;
			selectedNode->y = mouseXY.y;
		}
		break;
	}

	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::chartLButtonUp(POINT xy) {
	TPoint mouseXY = screenToChart(xy);

	if (chartState == CHARTSTATE::DRAGPOINT) {
		updateLinesList();
		updateObjectsList();//incase the updated lines mark an object as bad
	}

	if (chartState != CHARTSTATE::ADDPOINT) {
		setChartState(CHARTSTATE::NOTHING);
	} else if (chartState == CHARTSTATE::ADDPOINT) {
		TPoint* newNode = new TPoint(mouseXY);
		selectedLine->addNode(newNode);
		updateLinesList();
		updateObjectsList();//incase the updated lines mark an object as bad
	}

	releaseMouseCapture(true);
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::chartRButtonDown(POINT xy) {
	setMouseCapture(ui->chartCanvas->hwnd, moveCursor, false);
	dragChartXY = screenToChart(xy);
	needsUIUpdate();
}

void ProjectContainer::chartRButtonMove(POINT xy) {
	TPoint newXY = this->screenToChart(xy, chartProject->viewState.zoom, dragChartXY);
	chartProject->viewState.xy = newXY;
	needsRedraw();
	needsUIUpdate();
}

void ProjectContainer::chartRButtonUp(POINT xy) {
	releaseMouseCapture(false);
	needsUIUpdate();
}

void ProjectContainer::chartMouseMove(POINT xy) {
	if (chartState == CHARTSTATE::ADDPOINT) {
		needsRedraw();
	}
	needsUIUpdate();
}

void ProjectContainer::chartMouseWheel(POINT xy, int amount) {
	double oldZoom = chartProject->viewState.zoom;
	double zoomStep = 0.1*amount;

	double newZoom = oldZoom + zoomStep;
	newZoom = fmin(fmax(0.05f, newZoom), 20.0f);

	chartProject->viewState.zoom = newZoom;

	TPoint oldChartCoords = screenToChart(xy, oldZoom, chartProject->viewState.xy);

	TPoint newOrigin = screenToChart(xy, newZoom, oldChartCoords);

	chartProject->viewState.xy = newOrigin;

	needsRedraw();
	needsUIUpdate();
}

ChartImage* ProjectContainer::getChartImageAt(TPoint xy) {
	ChartImage* returnImage = nullptr;
	if (chartProject) {
		for (auto& image : chartProject->chartImages) {
			if (image->getRect().isPointInside(xy)){
				returnImage = image;
			}
		}
	}
	return returnImage;
}

bool ProjectContainer::isChartImageAt(ChartImage* image, TPoint xy) {
	if (!image) return false;
	if (image->getRect().isPointInside(xy)) {
		return true;
	}
	return false;
}

void ProjectContainer::chartImagesLButtonDown(POINT xy) {
	TPoint mouseXY = screenToChart(xy);
	if (selectedImage) {
		if (!isChartImageAt(selectedImage, mouseXY)) {
			setSelectedImage(nullptr);
		}
	}
	if (!selectedImage) {
		setSelectedImage(getChartImageAt(mouseXY));
	}

	if (selectedImage) {
		setMouseCapture(ui->chartCanvas->hwnd, handCursor, true);
	}
	needsUIUpdate();
}

void ProjectContainer::chartImagesLButtonMove(POINT xy) {
	TPoint mouseXY = screenToChart(xy);

	if (selectedImage) {
		if (chartState != CHARTSTATE::DRAGCHART) {
			if (dragChartStartPos.distance(mouseXY) > (dragChartImageTolerance / chartProject->viewState.zoom)) {
				setChartState(CHARTSTATE::DRAGCHART);
			}
		}

		if (chartState == CHARTSTATE::DRAGCHART) {
			needsRedraw();
		}
	}
	needsUIUpdate();
}



void ProjectContainer::chartImagesLButtonUp(POINT xy) {
	if (chartState == CHARTSTATE::DRAGCHART) {

	}
	setChartState(CHARTSTATE::NOTHING);
	releaseMouseCapture(true);
	needsUIUpdate();
}


void ProjectContainer::uiCharPress(int key) {
}

void ProjectContainer::uiKeyDown(int key) {
}

void ProjectContainer::uiLButtonDown(POINT xy) {
}

void ProjectContainer::uiLButtonMove(POINT xy) {
}

void ProjectContainer::uiLButtonUp(POINT xy) {
}

void ProjectContainer::uiRButtonDown(POINT xy) {
}

void ProjectContainer::uiRButtonMove(POINT xy) {
}

void ProjectContainer::uiRButtonUp(POINT xy) {
}

void ProjectContainer::uiMouseMove(POINT xy) {
}

void ProjectContainer::uiMouseWheel(POINT xy, int amount) {

}

void ProjectContainer::drawSelectedChartBounds(CChartBase* chartBase) {
	chartDB->setLineColor(RGB(0, 0, 0));
	chartDB->drawRect(chartBase->bounds.leftPosition, chartBase->bounds.topPosition, chartBase->bounds.rightPosition, chartBase->bounds.bottomPosition, 3, false, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
	chartDB->setLineColor(RGB(255,255,255));
	chartDB->drawRect(chartBase->bounds.leftPosition, chartBase->bounds.topPosition, chartBase->bounds.rightPosition, chartBase->bounds.bottomPosition, 1, false, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);

	chartDB->setFont(14, FW_MEDIUM, false, false, "Segoe UI");
	chartDB->setTextColor(RGB(0, 0, 0), RGB(128, 225, 255), false);

	double middleX = (chartBase->bounds.leftPosition + chartBase->bounds.rightPosition) / 2;
	double middleY = (chartBase->bounds.bottomPosition + chartBase->bounds.topPosition) / 2;

	double topHeight;
	double leftWidth;
	double rightWidth;
	double bottomHeight;

	//Draw bound values
	chartDB->drawText(trimmedDoubleToString(chartBase->bounds.bottomValue), middleX, chartBase->bounds.bottomPosition, nullptr, &bottomHeight,
		DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE, DRAWXALIGN::CENTER, DRAWYALIGN::TOP);

	chartDB->drawText(trimmedDoubleToString(chartBase->bounds.topValue), middleX, chartBase->bounds.topPosition, nullptr, &topHeight,
		DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE, DRAWXALIGN::CENTER, DRAWYALIGN::BOTTOM);

	chartDB->drawText(trimmedDoubleToString(chartBase->bounds.leftValue), chartBase->bounds.leftPosition, middleY, &leftWidth, nullptr,
		DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE, DRAWXALIGN::RIGHT, DRAWYALIGN::CENTER);

	chartDB->drawText(trimmedDoubleToString(chartBase->bounds.rightValue), chartBase->bounds.rightPosition, middleY, &rightWidth, nullptr,
		DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE, DRAWXALIGN::LEFT, DRAWYALIGN::CENTER);


	//Draw in/out arrows
	if (!chartBase->toPolyChart()) {
		CTrendChart* trendChart = chartBase->toTrendChart();
		bool reverseIn = chartBase->bounds.rightPosition > chartBase->bounds.leftPosition;
		bool reverseOut = chartBase->bounds.topPosition < chartBase->bounds.bottomPosition;
		if (!chartBase->yIsInputAxis) {
			reverseIn = !reverseIn;
			reverseOut = !reverseOut;
		}

		chartDB->setLineColor(RGB(0, 0, 0));

		if (!trendChart || (chartBase->yIsInputAxis && trendChart)) {
			chartDB->drawHorizantalArrow(chartBase->bounds.leftPosition - leftWidth / chartDB->getScale(), middleY, 10, 30, 5, reverseIn, chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
			chartDB->drawHorizantalArrow(chartBase->bounds.rightPosition + rightWidth / chartDB->getScale(), middleY, 10, 30, 5, !reverseIn, chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
		}

		if (!trendChart || (!chartBase->yIsInputAxis && trendChart)) {
			chartDB->drawVerticalArrow(middleX, chartBase->bounds.topPosition - topHeight / chartDB->getScale(), 10, 30, 5, !reverseOut, !chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
			chartDB->drawVerticalArrow(middleX, chartBase->bounds.bottomPosition + bottomHeight / chartDB->getScale(), 10, 30, 5, reverseOut, !chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
		}


		if (chartBase->yIsInputAxis) {
			chartDB->setLineColor(RGB(0, 200, 0));
		} else {
			chartDB->setLineColor(RGB(0, 128, 200));
		}

		if (!trendChart || (chartBase->yIsInputAxis && trendChart)) {
			chartDB->drawHorizantalArrow(chartBase->bounds.leftPosition - leftWidth / chartDB->getScale(), middleY, 10, 30, 3, reverseIn, chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
			chartDB->drawHorizantalArrow(chartBase->bounds.rightPosition + rightWidth / chartDB->getScale(), middleY, 10, 30, 3, !reverseIn, chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
		}

		if (!chartBase->yIsInputAxis) {
			chartDB->setLineColor(RGB(0, 200, 0));
		} else {
			chartDB->setLineColor(RGB(0, 128, 200));
		}

		if (!trendChart || (!chartBase->yIsInputAxis && trendChart)) {
			chartDB->drawVerticalArrow(middleX, chartBase->bounds.topPosition - topHeight / chartDB->getScale(), 10, 30, 3, !reverseOut, !chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
			chartDB->drawVerticalArrow(middleX, chartBase->bounds.bottomPosition + bottomHeight / chartDB->getScale(), 10, 30, 3, reverseOut, !chartBase->yIsInputAxis, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
		}
	}
}

void ProjectContainer::drawChart(CChartBase * chartBase, TPoint& mouseXY, bool isActive, bool addingPoints, TLine* selLine, TPoint* selPoint) {
	if (chartBase == nullptr) return;
	if (chartProject == nullptr) return;

	if (addingPoints && !isActive) {
		addingPoints = false;
	}

	//draw bounds
	if (isActive && chartState == CHARTSTATE::MARKLEFT) {
		chartDB->setLineColor(RGB(0, 0, 0));
		chartDB->drawVerticalLine(chartBase->bounds.leftPosition, 3, DRAWPOSITION::SCALEANDOFFSET);
		chartDB->setLineColor(RGB(255, 255, 255));
		chartDB->drawVerticalLine(chartBase->bounds.leftPosition, 1, DRAWPOSITION::SCALEANDOFFSET);
	}
	if (isActive && chartState == CHARTSTATE::MARKTOP) {
		chartDB->setLineColor(RGB(0, 0, 0));
		chartDB->drawHorizantalLine(chartBase->bounds.topPosition, 3, DRAWPOSITION::SCALEANDOFFSET);
		chartDB->setLineColor(RGB(255, 255, 255));
		chartDB->drawHorizantalLine(chartBase->bounds.topPosition, 1, DRAWPOSITION::SCALEANDOFFSET);
	}
	if (isActive && chartState == CHARTSTATE::MARKRIGHT) {
		chartDB->setLineColor(RGB(0, 0, 0));
		chartDB->drawVerticalLine(chartBase->bounds.rightPosition, 3, DRAWPOSITION::SCALEANDOFFSET);
		chartDB->setLineColor(RGB(255, 255, 255));
		chartDB->drawVerticalLine(chartBase->bounds.rightPosition, 1, DRAWPOSITION::SCALEANDOFFSET);
	}
	if (isActive && chartState == CHARTSTATE::MARKBOTTOM) {
		chartDB->setLineColor(RGB(0, 0, 0));
		chartDB->drawHorizantalLine(chartBase->bounds.bottomPosition, 3, DRAWPOSITION::SCALEANDOFFSET);
		chartDB->setLineColor(RGB(255, 255, 255));
		chartDB->drawHorizantalLine(chartBase->bounds.bottomPosition, 1, DRAWPOSITION::SCALEANDOFFSET);
	}

	if (isActive) {
		drawSelectedChartBounds(chartBase);
	} else {
		chartDB->setLineColor(RGB(0, 0, 0));
		chartDB->drawRect(chartBase->bounds.leftPosition, chartBase->bounds.topPosition, chartBase->bounds.rightPosition, chartBase->bounds.bottomPosition, 3, false, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
		chartDB->setLineColor(chartProject->notActiveSettings.lineColor);
		chartDB->drawRect(chartBase->bounds.leftPosition, chartBase->bounds.topPosition, chartBase->bounds.rightPosition, chartBase->bounds.bottomPosition, 1, false, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);

		chartDB->setClippingRect(chartBase->bounds.leftPosition, chartBase->bounds.topPosition, chartBase->bounds.rightPosition, chartBase->bounds.bottomPosition, DRAWPOSITION::SCALEANDOFFSET);
	}

	//draw lines/nodes
	bool isPoly = chartBase->toPolyChart();

	COLORREF lineColor = 0;
	COLORREF nodeColor = 0;
	double lineSize = 0;
	double nodeSize = 0;

	for (auto & line : chartBase->lines) {
		if (isActive) {
			if (line == selLine) {
				lineColor = chartProject->selectedSettings.lineColor;
				nodeColor = chartProject->selectedSettings.nodeColor;
				lineSize = chartProject->selectedSettings.lineSize;
				nodeSize = chartProject->selectedSettings.nodeSize;
			} else {
				lineColor = chartProject->notSelectedSettings.lineColor;
				nodeColor = chartProject->notSelectedSettings.nodeColor;
				lineSize = chartProject->notSelectedSettings.lineSize;
				nodeSize = chartProject->notSelectedSettings.nodeSize;
			}
		} else {
			lineColor = chartProject->notActiveSettings.lineColor;
			nodeColor = chartProject->notActiveSettings.nodeColor;
			lineSize = chartProject->notActiveSettings.lineSize;
			nodeSize = chartProject->notActiveSettings.nodeSize;
		}

		chartDB->setLineColor(lineColor);

		if (line->points.size() > 1) {
			TPoint* lastPoint = nullptr;
			for (auto* point : line->points) {
				if (lastPoint != nullptr) {
					chartDB->drawLine(lastPoint->x, lastPoint->y, point->x, point->y, lineSize, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::SCALEENTIRE);
				}
				lastPoint = point;
			}
			if (isPoly && !(addingPoints && selectedLine==line)) {
				chartDB->drawLine(line->points.front()->x, line->points.front()->y, line->points.back()->x, line->points.back()->y, lineSize, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::SCALEENTIRE);
			}
		}
		if (line->points.size() > 0) {
			if (addingPoints && line == selectedLine) {
				chartDB->drawLine(mouseXY.x, mouseXY.y, line->points.back()->x, line->points.back()->y, lineSize, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::SCALEENTIRE);
			}
		}

		if (isActive) {
			chartDB->setLineColor(RGB(0, 0, 0));
			chartDB->setFillColor(nodeColor);
			int nodeOutlineSize = 1;
			for (auto* point : line->points) {
				if (point == selPoint) {
					nodeOutlineSize = 3;
				} else {
					nodeOutlineSize = 1;
				}
				chartDB->drawCircle(point->x, point->y, nodeSize, nodeOutlineSize, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::SCALEFILLONLY, DRAWXALIGN::CENTER, DRAWYALIGN::CENTER);
			}
		}
	}

	chartDB->clearClippingRect();
}

TPoint ProjectContainer::screenToChart(POINT xy, double zoom, TPoint viewOrigin) {
	return TPoint((double)xy.x / zoom - viewOrigin.x, (double)xy.y / zoom - viewOrigin.y);
}
TPoint ProjectContainer::screenToChart(POINT xy) {
	return this->screenToChart(xy, chartProject->viewState.zoom, chartProject->viewState.xy);
}

void ProjectContainer::drawChartCanvas() {
	//draw charts
	chartDB->setOffset(chartProject->viewState.xy.x, chartProject->viewState.xy.y);
	chartDB->setScale(chartProject->viewState.zoom);
	chartDB->clear(RGB(230, 220, 220));
	if (chartProject) {
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(ui->chartCanvas->hwnd, &cursorPos);
		TPoint mouseXY=screenToChart(cursorPos);

		//Draw the chart images
		chartDB->smoothBitmap = false;
		for (auto & image : chartProject->chartImages) {
			TPoint imgPos = image->getPosition();
			chartDB->drawBitmap(image->getHBitmap(), imgPos.x, imgPos.y, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::SCALEENTIRE);
		}

		//Draw the charts
		for (auto & obj : chartProject->objects) {
			if (obj != selectedObject  ||  ui->chartsTab->currentTab() != ui->chartTabIndex) {
				drawChart(obj->toChartBase(), mouseXY, false, false, nullptr, nullptr);
			}
		}
		if (selectedObject && ui->chartsTab->currentTab()==ui->chartTabIndex) {
			drawChart(selectedObject->toChartBase(), mouseXY, true, chartState == CHARTSTATE::ADDPOINT, selectedLine, selectedNode);
		}

		if (selectedImage && ui->chartsTab->currentTab() == ui->imagesTab) {
			chartDB->setLineColor(RGB(0, 255, 0));
			chartDB->drawRect(selectedImage->getRect(), 3, false, DRAWPOSITION::SCALEANDOFFSET, DRAWSIZE::NONE);
		}
	}

	if (scratchpadInUse) {
		chartDB->setFont(40, FW_MEDIUM, false, false, nullptr);
		chartDB->setTextColor(RGB(255, 255, 255), RGB(0, 0, 0), false);
		chartDB->drawText(scratchpad, 10, 10, nullptr, nullptr, DRAWPOSITION::NONE, DRAWSIZE::NONE);
	}

	string selectionString;
	if (selectedObject) {
		if (selectedObject->toChartBase()) {
			selectionString += selectedObject->getName();
			if (selectedLine) {
				selectionString += " > Line";
				if (selectedNode) {
					selectionString += " > Node";
				}
			}
			chartDB->setFont(20, FW_BOLD, false, false, "Segoe UI");
			chartDB->setTextColor(RGB(255, 255, 255), RGB(0,0,0), false);
			chartDB->drawText(selectionString, 5, ui->chartCanvas->height()-5, nullptr, nullptr, DRAWPOSITION::NONE, DRAWSIZE::NONE, DRAWXALIGN::LEFT, DRAWYALIGN::BOTTOM);
		}
	}

	if (chartState != CHARTSTATE::NOTHING) {
		string chartStateString;
		switch (chartState) {
		case CHARTSTATE::ADDPOINT:
			chartStateString = "Add point";
			break;
		case CHARTSTATE::DRAGPOINT:
			chartStateString = "Dragging point";
			break;
		case CHARTSTATE::MARKBOTTOM:
			chartStateString = "Mark bottom";
			break;
		case CHARTSTATE::MARKTOP:
			chartStateString = "Mark top";
			break;
		case CHARTSTATE::MARKLEFT:
			chartStateString = "Mark left";
			break;
		case CHARTSTATE::MARKRIGHT:
			chartStateString = "Mark right";
			break;
		case CHARTSTATE::NEWLINE:
			chartStateString = "Click to add new line";
			break;
		}
		chartDB->setFont(20, FW_BOLD, false, false, "Segoe UI");
		chartDB->setTextColor(RGB(255, 255, 255), RGB(0, 0, 0), false);
		chartDB->drawText(chartStateString, ui->chartCanvas->width() - 5, ui->chartCanvas->height() - 5, nullptr, nullptr, DRAWPOSITION::NONE, DRAWSIZE::NONE, DRAWXALIGN::RIGHT, DRAWYALIGN::BOTTOM);
	}

	chartDB->flip();
}

void ProjectContainer::needsRedraw() {
	if (chartsNeedDrawn==false) InvalidateRect(ui->chartCanvas->hwnd, nullptr, true);
	chartsNeedDrawn = true;
}

void ProjectContainer::beforePaintCharts() {
	if (uiNeedsUpdate){
		updateUI();
		uiNeedsUpdate = false;
	}
	if (chartsNeedDrawn) {
		drawChartCanvas();
		chartsNeedDrawn = false;
	}
}	


void ProjectContainer::whatChartIsAt(TPoint mouseXY, CChartBase*& thisChart) {
	thisChart = nullptr;

	for (auto * object : chartProject->objects) {
		CChartBase* chart = object->toChartBase();
		if (chart) {
			if (chart->boundPosToRect().isPointInside(mouseXY)) {
				thisChart = chart;
			}
		}
	}
}

bool ProjectContainer::isPointOnSegment(const TPoint* xy, const TPoint* a, const TPoint* b, double padding) {
	TRect rect(*a, *b);
	if (rect.isPointInside(*xy, padding)) {
		TSegment seg(*a, *b);
		TSegment oppositeSeg = seg.getOppositeSegmentAt(*xy);
		TPoint intersect;

		if (seg.getSegmentIntersection(intersect, oppositeSeg, false)) {
			if (intersect.distance(*xy) <= padding) {
				return true;
			}
		}
	}
	return false;
}

void ProjectContainer::whatLineIsAt(TPoint mouseXY, TLine*& thisLine, TPoint*& thisNode, TPoint*& pointIsBeforeThisNode) {
	if (!selectedObject) return;
	if (!chartProject) return;
	CChartBase* chart = selectedObject->toChartBase();
	bool isPoly = selectedObject->toPolyChart();
	if (!chart) return;

	for (auto * line : chart->lines) {//loop through lines
		TPoint* lastPoint = nullptr;
		double lineHalfWidth = 0;
		double nodeHalfWidth = 0;

		if (line == selectedLine) {
			lineHalfWidth = chartProject->selectedSettings.lineSize / 2;
			nodeHalfWidth = chartProject->selectedSettings.nodeSize / 2;
		} else {
			lineHalfWidth = chartProject->notSelectedSettings.lineSize / 2;
			nodeHalfWidth = chartProject->notSelectedSettings.nodeSize / 2;
		}

		lineHalfWidth += selectionPaddingPixels / chartProject->viewState.zoom;
		nodeHalfWidth += selectionPaddingPixels / chartProject->viewState.zoom;

		for (auto * point : line->points) {
			if (mouseXY.distance(*point) <= nodeHalfWidth) {
				if (line == selectedLine) {
					thisNode = point;
				}
				thisLine = line;
				return;
			}

			if (lastPoint) {
				if (isPointOnSegment(&mouseXY, lastPoint, point, lineHalfWidth)) {
					pointIsBeforeThisNode = point;
					thisLine = line;
					return;
				}
			}
			lastPoint = point;
		}
		if (isPoly && line->points.size()>2u) {
			if (isPointOnSegment(&mouseXY, line->points.back(), line->points.front(), lineHalfWidth)) {
				pointIsBeforeThisNode = line->points.front();
				thisLine = line;
				return;
			}
		}
	}
}


bool ProjectContainer::hasBoundsInClipboard() {
	return clipboardBounds!=nullptr;
}

bool ProjectContainer::hasLineInClipboard() {
	return clipboardLine!=nullptr;
}

void ProjectContainer::copyBounds(CChartBase* chart) {
	if (clipboardBounds) {
		delete clipboardBounds;
		clipboardBounds = nullptr;
	}
	if (chart) {
		clipboardBounds = new BoundSettings(chart->bounds);
	}
}

void ProjectContainer::pasteBounds(CChartBase* intoThisChart) {
	if (clipboardBounds) {
		if (intoThisChart) {
			intoThisChart->bounds = *clipboardBounds;
			updateObjectsList();
			needsRedraw();
		}
	}
}

void ProjectContainer::pasteLine(CChartBase* intoThisChart) {
	if (intoThisChart && clipboardLine) {
		intoThisChart->addLine(clipboardLine->copyLine());
		updateObjectsList();
		updateLinesList();
		needsRedraw();
	}
}

void ProjectContainer::copyLine(TLine* line) {
	if (clipboardLine) {
		delete clipboardLine;
		clipboardLine = nullptr;
	}
	if (line) {
		clipboardLine = line->copyLine();
	}
}


void ProjectContainer::needsUIUpdate() {
	if (uiNeedsUpdate==false) InvalidateRect(ui->chartCanvas->hwnd, nullptr, true);
	uiNeedsUpdate = true;
}

void ProjectContainer::updateMenus() {
}

void ProjectContainer::updateListUIs() {
	//update objects box
	ui->objectsList->buttonBar->enableButton(ID_OBJ_DELETE, selectedObject);
	ui->objectsList->buttonBar->enableButton(ID_OBJ_UP, chartProject->canMoveChartUp(selectedObject));
	ui->objectsList->buttonBar->enableButton(ID_OBJ_DOWN, chartProject->canMoveChartDown(selectedObject));

	//update lines box
	ui->linesList->buttonBar->enableButton(ID_LINES_DELETE, selectedLine);
	ui->linesList->buttonBar->enableButton(ID_LINES_ADDPOINTS, selectedLine);
	ui->linesList->buttonBar->enableButton(ID_LINES_REMOVEPOINT, selectedLine && !selectedLine->points.empty() && chartState == CHARTSTATE::ADDPOINT);

	//update table box
	ui->tableList->buttonBar->enableButton(ID_TABLE_DELETEENTRY, ui->tableList->getCurSelItemData());

	//update contants
	CConstant* selConst = (CConstant*)ui->constantsList->getCurSelItemData();
	ui->constantsList->buttonBar->enableButton(ID_CONSTANT_DELETE, selConst);
	ui->constantsList->buttonBar->enableButton(ID_CONSTANT_UP, chartProject->canMoveConstantUp(selConst));
	ui->constantsList->buttonBar->enableButton(ID_CONSTANT_DOWN, chartProject->canMoveConstantDown(selConst));

	//update inputs
	CInput* selInput = (CInput*)ui->inputsList->getCurSelItemData();
	ui->inputsList->buttonBar->enableButton(ID_INPUT_UP, chartProject->canMoveInputUp(selInput));
	ui->inputsList->buttonBar->enableButton(ID_INPUT_DOWN, chartProject->canMoveInputDown(selInput));
	ui->inputsList->buttonBar->enableButton(ID_INPUT_DELETE, selInput);

	//update images
	ui->imagesList->buttonBar->enableButton(ID_IMAGE_DELETE, ui->imagesList->getCurSelItemData());
}

void ProjectContainer::updateStatus() {
	char buffer[32];
	snprintf(buffer, sizeof(buffer) - 1, "\t\t%g%%          ", chartProject->viewState.zoom*100);
	SendMessage(ui->statusbar, SB_SETTEXT, 3, reinterpret_cast<LPARAM>(buffer));


	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(ui->chartCanvas->hwnd, &cursorPos);
	TPoint mouseXY = screenToChart(cursorPos);

	snprintf(buffer, sizeof(buffer) - 1, "%.1f, %.1f", mouseXY.x, mouseXY.y);
	SendMessage(ui->statusbar, SB_SETTEXT, 1, reinterpret_cast<LPARAM>(buffer));

	if (this->selectedObject) {
		CChartBase* chartBase = this->selectedObject->toChartBase();
		if (chartBase) {
			chartBase->screenToChart(&mouseXY);
			snprintf(buffer, sizeof(buffer) - 1, "%.1f, %.1f", mouseXY.x, mouseXY.y);
			SendMessage(ui->statusbar, SB_SETTEXT, 2, reinterpret_cast<LPARAM>(buffer));
		} else {
			snprintf(buffer, sizeof(buffer) - 1, "%.1f, %.1f", mouseXY.x, mouseXY.y);
			SendMessage(ui->statusbar, SB_SETTEXT, 2, reinterpret_cast<LPARAM>(""));
		}
	} else {
		SendMessage(ui->statusbar, SB_SETTEXT, 2, reinterpret_cast<LPARAM>(""));
	}
}

void ProjectContainer::updateUI() {
	if (!ui || !chartProject) return;

	updateStatus();
	updateMenus(); 
	updateListUIs();
}