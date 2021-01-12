#pragma once
#include <vector>
#include <string>
using namespace std;

class TPoint;
class TSegment;

class TLine {
public:
	friend class CChartBase;
	TLine(double value = 0, vector<TPoint*> points = vector<TPoint*>{});
	TLine(vector<TPoint*> points);
	~TLine();

	TLine* copyLine();

	void addNode(TPoint* node);
	void insertNode(TPoint* newNode, TPoint* insertBeforeThisNode);
	void deleteNode(TPoint* node);

	void flipPoints();
	void reverseOrder();

	bool isLinear(bool yIsInputAxis);
	bool findSegment(TSegment& returnSegment, double x);

	vector<TPoint*> points;

	double getValue();

	string toString();

protected:
	double value;
};

