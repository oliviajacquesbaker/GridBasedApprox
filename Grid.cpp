#include "Grid.h"

//this file handles all point set functionality - creating a point set, inspecting it, and finding the optimal partitioning of it

Grid::Grid() {
	penalty = 10.0;
	largestX = 1;
}

//return results of search for point in set - helper function for adding points
bool Grid::PointsContains(point p) {
	if (find(points.begin(), points.end(), p) != points.end()) return true;
	else return false;
}

//manually add a point to the set
//points can share x or y values, but not both (aka no duplicate points)
bool Grid::AddPoint(int x, double y) {
	point newPoint = { x, y };
	if (PointsContains(newPoint)) return false;
	points.push_back(newPoint);
	if (newPoint.x > largestX) largestX = newPoint.x;
	return true;
}

//randomly add points to the set
//numberPoints is the number of points to add, maxVal is the largest x value to be generated (the m in the interval [1,m])
void Grid::AddRandomPoints(int numberPoints, int maxVal) {
	int x = 0; double y = 0;  point newPoint = {};
	unsigned int seed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
	default_random_engine generator(seed);
	uniform_int_distribution<int> distributionInt(1, maxVal);
	uniform_real_distribution<double> distributionReal(-1.0 * maxVal, static_cast<double>(maxVal));
	auto randInt = bind(distributionInt, generator);
	auto randReal = bind(distributionReal, generator);
	while (static_cast<int>(points.size()) < numberPoints) {
		x = randInt();
		y = randReal();
		newPoint = { x, y };
		if (!PointsContains(newPoint)) points.push_back(newPoint);
	}
	largestX = maxVal;
}

//precompute the errors for the point subsets p_1,p_2,...,p_n for every n
void Grid::PrecomputeErrorSums(int xMax) {
	sumY = vector<double>(xMax + 1, 0);
	sumYSquare = vector<double>(xMax + 1, 0);
	sumY[0] = 0.0; sumYSquare[0] = 0.0;

	for (int i = 1; i <= xMax; i++) {
		for (int j = 1; j <= i; j++) {
			sumY[i] += points[j - 1].y;
			sumYSquare[i] += (points[j - 1].y * points[j - 1].y);
		}
	}
}

//using precomputed sums, calculate the error of a specific interval
//precomputed sums all start at x=1, so to get the error of an interval element starting at a > 1 and ending at b >= a,
//calculate sum from a to b minus the sum from 1 to a
double Grid::CalculateError(int x1, int x2) {
	int pointsInCell = x2 - x1;
	double sumYInCell = (sumY[x2] - sumY[x1]);
	double average = (pointsInCell == 0) ? 0 : sumYInCell / pointsInCell;
	double error = (sumYSquare[x2] - sumYSquare[x1]) - pointsInCell * average * average;
	return abs(error);
}

//find the optimal partitioning of the interval [1,m] using a tabulation / bottom-up approach
list<int> Grid::FindOptimalPartitionings_Tabulation() {
	//set up - we want to sort points so that the first j points in the set are the first j points relative to x values
	//then precompute the error sums so that CalculateError can be used to find the error of any interval [a,b] in constant time
	int numPoints = static_cast<int>(points.size());
	SortPoints(points, 0, numPoints - 1);
	PrecomputeErrorSums(numPoints);
	//base case, error for 0 points = 0
	vector<double> OPT(numPoints + 1);
	OPT[0] = 0.0;
	vector<list<int>> partitionings(numPoints + 1);
	int tempPartitioning = 1;
	double tempMinimum = 0; double minimumVal = 0; int lastDivision = 0;
	//find the minimum cost for the first i points by finding an optimum "last segment" 
	//and adding its error to the cost of the points up to the start of the "last segment" (+penalty for adding another interval)
	for (int i = 1; i <= numPoints; i++) {
		minimumVal = CalculateError(0, i);
		for (int j = 1; j <= i; j++) {
			tempMinimum = CalculateError(j-1, i) + OPT[j-1] + penalty;
			if (tempMinimum <= minimumVal) { 
				minimumVal = tempMinimum;
				tempPartitioning = j;
			}
		}
		//update partitionings alongside OPT so there's no need to post-process / backtrack for a solution after OPT(n) is found
		OPT[i] = minimumVal;
		if(tempPartitioning > 0) partitionings[i] = partitionings[tempPartitioning - 1];
		if (tempPartitioning - 1 > 0 && points[tempPartitioning - 1].x != lastDivision && points[tempPartitioning - 1].x != points[0].x) {
			partitionings[i].push_back(points[tempPartitioning - 1].x);
			lastDivision = points[tempPartitioning - 1].x;
		}
	}
	return partitionings[numPoints];
}

//find the optimal partitioning of the interval [1,m] using a memoization / top-down approach
list<int> Grid::FindOptimalPartitionings_Memoization() {
	//setup for the actual recursive/top down call
	//same logic as bottom-up for sorting + precomputing error sums
	int numPoints = static_cast<int>(points.size());
	SortPoints(points, 0, numPoints - 1);
	PrecomputeErrorSums(numPoints);
	unordered_map<int, double> OPT;
	unordered_map<int, list<int>> partitionings;
	//the actual recursive/top down call
	MemoizationHelper(OPT, partitionings, numPoints);
	list<int> toReturn = partitionings[numPoints];
	return toReturn;
}

//the actual recursive function that finds OPT(n)
double Grid::MemoizationHelper(unordered_map<int, double> &OPT, unordered_map<int, list<int>> &partitionings, int i) {
	//same Bellman equation as in bottom-up method, but only calculate a value if it's deemed necessary instead of starting at the bottom + doing everything
	if (OPT.find(i) == OPT.end()) {
		if (i < 1) {
			OPT[i] = 0.0;
			partitionings[i] = { {}, {}, 0 };
		}
		else {
			double minimumVal = CalculateError(0, i);
			double tempMinimum = 0.0; int tempPartitioning = 0;
			for (int j = 1; j <= i; j++) {
				tempMinimum = CalculateError(j - 1, i) + MemoizationHelper(OPT, partitionings, j - 1) + penalty;
				if (tempMinimum <= minimumVal) {
					minimumVal = tempMinimum;
					tempPartitioning = j;
				}
			}
			OPT[i] = minimumVal;
			//no need to make a sepaarte function call for partitionings since it's set alongside OPT, and OPT(tempPartitioning -1) was called above
			list<int> lastSet;
			if (tempPartitioning - 1 > 0) lastSet = partitionings[tempPartitioning - 1];
			if (tempPartitioning > 0) partitionings[i] = lastSet;
			if (tempPartitioning - 1 > 0 && points[tempPartitioning - 1].x != points[0].x && find(lastSet.begin(), lastSet.end(), points[tempPartitioning - 1].x) == lastSet.end())
				partitionings[i].push_back(points[tempPartitioning - 1].x);
		}
	}
	return OPT[i];
}

//merge sort - used to sort the points based on their x coordinates
void Grid::SortPoints(vector<point>& points, int left, int right) {
	if (left < right) {
		int middle = static_cast<int>((left + right) / 2);
		SortPoints(points, left, middle);
		SortPoints(points, middle + 1, right);
		Merge(points, left, middle, right);
	}
}

//merge for merge sort
void Grid::Merge(vector<point>& points, int left, int middle, int right) {
	int l = 0; int r = 0; int merge = left;
	int leftSubMax = middle - left + 1;
	int rightSubMax = right - middle;
	vector<point> leftSubArray(leftSubMax); vector<point> rightSubArray(rightSubMax);
	for (int i = 0; i < leftSubMax; i++) {
		leftSubArray[i] = points[left + i];
	}
	for (int i = 0; i < rightSubMax; i++) {
		rightSubArray[i] = points[middle + 1 + i];
	}
	while (l < leftSubMax && r < rightSubMax) {
		if (leftSubArray[l].x <= rightSubArray[r].x) {
			points[merge] = leftSubArray[l];
			l++;
		}
		else {
			points[merge] = rightSubArray[r];
			r++;
		}
		merge++;
	}
	while (l < leftSubMax) {
		points[merge] = leftSubArray[l];
		l++; merge++;
	}
	while (r < rightSubMax) {
		points[merge] = rightSubArray[r];
		r++; merge++;
	}
}

//use stopwatch to measure the time it takes to run the partitioning algorithm
double Grid::TimeToFindDivisions(bool tabulation) {
	chrono::steady_clock::time_point start = chrono::steady_clock::now();
	if (tabulation) FindOptimalPartitionings_Tabulation();
	else FindOptimalPartitionings_Memoization();
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	chrono::duration<float> timeSpent = chrono::duration_cast<chrono::duration<double>>(end - start);
	return timeSpent.count();
}

//returns the info for the point set as a string of (x_1, y_1) (x_2, y_2) etc
string Grid::GetPointInfo() {
	if (points.size() == 0) return "No points in grid.";
	string toReturn = "";
	for (unsigned int i = 0; i < points.size(); i++) {
		toReturn += "(" + to_string(points[i].x) + ", " + to_string(points[i].y) + ") ";
	}
	return toReturn;
}

double Grid::GetPenalty() {
	return penalty;
}

void Grid::SetPenalty(double newPenalty) {
	penalty = newPenalty;
}

int Grid::GetNumPoints() {
	return static_cast<int>(points.size());
}

int Grid::GetMaxXVal() {
	return largestX;
}