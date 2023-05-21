#pragma once
#include <vector>
#include <random>
#include <functional>
#include <algorithm>
#include <string>
#include <chrono> 
#include <list>
#include <unordered_map>
#include <iostream>

using namespace std;

class Grid {

	struct point {
		int x;
		double y;

		bool operator==(const point& p) const
		{
			return (x == p.x && y == p.y);
		}
	};

	bool PointsContains(point p);
	void PrecomputeErrorSums(int xMax);
	double CalculateError(int x1, int x2);
	double MemoizationHelper(unordered_map<int, double>& OPT, unordered_map<int, list<int>>& partitionings, int i);
	void SortPoints(vector<point>& points, int left, int right);
	void Merge(vector<point>& points, int left, int middle, int right);
	vector<point> points;
	double penalty;
	vector<vector<int>> numPoints;
	vector<double> sumY;
	vector<double> sumYSquare;
	int largestX;

public:
	list<int> partitionLines;

	Grid();
	bool AddPoint(int x, double y);
	void AddRandomPoints(int numberPoints, int maxVal);
	list<int> FindOptimalPartitionings_Tabulation();
	list<int> FindOptimalPartitionings_Memoization();
	double TimeToFindDivisions(bool tabulation = true);
	double GetPenalty();
	void SetPenalty(double newPenalty);
	string GetPointInfo();
	int GetNumPoints();
	int GetMaxXVal();

};
