#include "stdafx.h"
#include "Canvas.h"
#include "ListUI.h"
#include "DoubleBuff.h"
#include "UIData.h"


UIData::UIData() {
}


UIData::~UIData() {
}

void UIData::clearUI() {
	chartCanvas->db->clear(RGB(230, 220, 220));
	chartCanvas->db->flip();

	uiCanvas->db->clear(RGB(230, 220, 220));
	uiCanvas->db->flip();

	constantsList->clearContents();
	inputsList->clearContents();
	imagesList->clearContents();
	objectsList->clearContents();

	uiList->clearContents();

	linesList->clearContents();
	tableList->clearContents();
}
