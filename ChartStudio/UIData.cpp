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


	constantsList->clearContents();
	inputsList->clearContents();
	imagesList->clearContents();
	objectsList->clearContents();


	linesList->clearContents();
	tableList->clearContents();
}
