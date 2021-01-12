#include "stdafx.h"
#include "TLine.h"
#include "TPoint.h"
#include "TSegment.h"
#include "helpers.h"
#include <vector>

TLine::TLine(double value, vector<TPoint*> points) {
	this->value = value;
	this->points = points;
}

TLine::TLine(vector<TPoint*> points) {
	this->value = 0.0f;
	this->points = points;
}


TLine::~TLine() {
	for (auto & element : this->points) {
		delete element;
	}
}

TLine * TLine::copyLine() {
	TLine* copy = new TLine(this->value);
	for (auto & point : this->points) {
		copy->addNode(new TPoint(point->x, point->y));
	}
	return copy;
}

//assumes ownership of the node
void TLine::addNode(TPoint* node) {
	this->points.push_back(node);
}

void TLine::insertNode(TPoint* newNode, TPoint* insertBeforeThisNode) {
	if (insertBeforeThisNode == nullptr) {
		this->points.insert(this->points.begin(), newNode);
	} else {
		for (auto it = this->points.begin(); it != this->points.end(); it++) {
			if (*it == insertBeforeThisNode) {
				it = this->points.insert(it, newNode);
				break;
			}
		}
	}
}

void TLine::deleteNode(TPoint* node) {
	for (auto it = this->points.begin(); it != this->points.end(); it++) {
		if (*it == node) {
			delete *it;
			it = this->points.erase(it);
			break;
		}
	}
}


void TLine::flipPoints() {
	for (auto & element : this->points) {
		element->flip();
	}
}

void TLine::reverseOrder() {
	int size = this->points.size();
	if (size > 1) {
		std::vector<TPoint*> newOrder;
		for (int i = size - 1; i >= 0; i--) {
			newOrder.push_back(this->points[i]);
		}
		this->points = newOrder;
	}
}

bool TLine::isLinear(bool yIsInputAxis) {
	if (this->points.size() < 2) {
		return false;//line doesnt have enough points to even have one segment
	}

	if (compare_float(this->points[1]->x, this->points[0]->x)) {
		return false;//first and last point are on the same x location
	}


	bool goingUp = (yIsInputAxis ? this->points[1]->y : this->points[1]->x) > (yIsInputAxis ? this->points[0]->y : this->points[0]->x);
	double lastX = (yIsInputAxis ? this->points[0]->y : this->points[0]->x);

	for (unsigned int i = 1; i < this->points.size(); i++) {
		if (goingUp) {
			if ((yIsInputAxis ? this->points[i]->y : this->points[i]->x) <= lastX){
				return false;//line x was ascending, but now its going down
			}
		} else {
			if ((yIsInputAxis ? this->points[i]->y : this->points[i]->x) >= lastX) {
				return false;//line x was descending, but now its going up
			}
		}
		lastX = (yIsInputAxis ? this->points[i]->y : this->points[i]->x);
	}
	return true;
}

bool TLine::findSegment(TSegment& returnSegment, double x) {
	if (this->points.size() < 2) {
		return false;//not enough points for even one segment
	}

	TSegment segment;
	double leftX = this->points.front()->x;
	double rightX = this->points.back()->x;


	//not going to be able to find a segment within the line, so we have to extrapolate
	if ((leftX<rightX && x<leftX) || (rightX<leftX && x>leftX)) {
		returnSegment = TSegment(*this->points[0], *this->points[1]);
		return true;
	} else if ((leftX<rightX && x>rightX) || (rightX<leftX && x<rightX))  {
		returnSegment = TSegment(*this->points[this->points.size() - 2], *this->points[this->points.size() - 1]);
		return true;
	}

	for (unsigned int i = 0; i < this->points.size()-1; i++) {
		if ((x >= this->points[i]->x  &&  x <= this->points[i + 1]->x) || (x <= this->points[i]->x  &&  x >= this->points[i + 1]->x)) {
			returnSegment = TSegment(*this->points[i], *this->points[i + 1]);
			return true;//we found a segment in which the x bounds of it included x
		}
	}

	return false;//should be impossible to get here
}

double TLine::getValue() {
	return this->value;
}

string TLine::toString() {
	string retString = "(" + trimmedDoubleToString(this->value)+",";
	for (auto & point : this->points) {
		retString += point->toString();
	}
	retString += ")";
	return retString;
}
