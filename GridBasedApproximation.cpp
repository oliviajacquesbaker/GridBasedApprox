//Olivia Jacques-Baker
//Assignment 2 - COT5405 Analysis of Algorithms
//Part 2: Interval-Based Constant Best Approximation

//Problem statement:    Given a set of N points (x_i, y_i) with integer values for x_i between 1 and M and real values for y_i, find
//                      a partitioning of the interval [1, M] into contiguous intervals such that the error of approximating points
//                      in each interval element (induced by the partitioning on X axis) by the average value of y in the interval
//                      element is minimized. Include a penalty factor proportional to the total number of intervals the solution has.

//Additional implementation details:    Experiment with both an array (tabulation) and a hash (memoization) version of the solution.

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Grid.h"
using namespace std;

void progInteractions();
void reset();
void badResponse();
void handleInitialSetupResponses(int response);
void handlePointInput();
template <class T>
bool setValIfValid(T amount, T* var, T minimum);
void manualOrAutomatic(int response);
void getBestPartitioning();
void timeAnalysis(bool tabulation);
void timeAnalysisSingle();
void runtimeResponse(int response);
void runtimeComplexityInteraction();
bool checkAllNumerical(string str, char delimiter, int expectedNumericalPieces);
vector<string> getSplitString(string str, char delimiter);
void viewInfo(Grid grid);

Grid grid = Grid();
bool manual = false;
int currInst = 0;
bool running = true;

int main() {
    while (running) {
        cout << "\n======================\n" << endl;
        cout << "Interval-Based Constant Best Approximation - Olivia Jacques-Baker" << endl;
        cout << "This is the main menu. Follow the instructions below to create points to partition." << endl;
        cout << "\n======================\n" << endl;

        progInteractions();
    }
    return 0;
}

//handles all interactions with the user as they create and partition point sets
void progInteractions() {
    double penalty = 0.f; int numPoints = 0; int maxCoordVal = 0;
    double response = 0.f;
    string instructions[5] = {
        "Please choose a main operation:\n1:create point set to partition\t\t2: see average runtime analysis\t\t0: quit program",
        "Before creating a point set to partition, please indicate whether you would like the points and interval penalty to be specified or automatic:\n1: manual input for these values\n2: use random points with default penalty value (10)\n3: use random points with manually input penalty value\n0: quit program",
        "Please input a positive integer value that indicates the amount to be subtracted for each additional interval partitioning.",
        "Please input a positive integer value that indicates the number of points you would like to randomly generate.",
        "Please input a positive integer value that indicates the upper bound on any x value for a randomly generated point."
    };

    //handle responses of the interactions specified above (instructions[])
    //response 0 indicates user wishes to quit the program, any other, handle appropriately.
    while (currInst < 5) {
        cout << instructions[currInst] << endl;
        cin >> response;
        if (currInst == 2) {
            if (setValIfValid<double>(response, &penalty, 0.0)) {
                grid.SetPenalty(penalty);
                if (manual) break;
            }
        }
        else if (response == 0) {
            running = false;
            return;
        }
        else if (currInst == 3) setValIfValid<int>(static_cast<int>(response), &numPoints, 0);
        else if (currInst == 4) setValIfValid<int>(static_cast<int>(response), &maxCoordVal, 1);
        else handleInitialSetupResponses(static_cast<int>(response));
    }

    if (manual) handlePointInput();
    else grid.AddRandomPoints(numPoints, maxCoordVal);

    //this loop marks where the grid object has been created and the user can now interact with the point set
    while (true) {
        cout << "Please select an action:\n1: find the best interval partitioning \n2: view the points and penalty information\n3: get runtimes \n4: return to main menu\n0: quit program" << endl;
        cin >> response;
        if (response == 1) getBestPartitioning();
        else if (response == 2) viewInfo(grid);
        else if (response == 3) timeAnalysisSingle();
        else if (response == 4) {
            reset();
            break;
        }
        else if (response == 0) {
            running = false;
            return;
        }
        else badResponse();
    }
}

//reset globals before returning to the main menu
void reset() {
    manual = false;
    currInst = 0;
    grid = Grid();
}

//used whenever user input is either not a specified option for selecting choices
//or when user input is outside the specified range for a variable they are manually entering
void badResponse() {
    cout << "Please provide input within the bounds of the instructions." << endl;
}

//pass along response to function with appropriate error handling
//the case of response == 0 (user request to quit program) is handled before this function is ever called
void handleInitialSetupResponses(int response) {
    if (currInst == 0) runtimeResponse(response);
    else if (currInst == 1) manualOrAutomatic(response);
    else badResponse();
}

//reacts to user's decision to run time complexity experiments or to manually point sets to interact with
void runtimeResponse(int response) {
    if (response == 1) currInst++;
    else if (response == 2) runtimeComplexityInteraction();
    else badResponse();
}

//sets indicators for if one of the point creation and penalty decisions should be made manually or automatically
void manualOrAutomatic(int response) {
    if (response == 1) {
        manual = true;
        cout << "Manual input has been selected. ";
        currInst++;
    }
    else if (response == 2) {
        cout << "Random points with default penalty value has been selected. ";
        currInst+=2;
    }
    else if (response == 3) {
        cout << "Random points with a specified penalty value has been selected. ";
        currInst++;
    }
    else badResponse();
}

//if the val is within range, set the variable within progInteractions()
template <class T>
bool setValIfValid(T amount, T* var, T minimum) {
    if (amount >= minimum) {
        *var = amount;
        currInst++;
        cout << "Value set. ";
        return true;
    }
    else {
        badResponse();
        return false;
    }    
}

//if the user wants to run time complexity experiments, they can choose to run them with either the tabulation or memoization based functions
void runtimeComplexityInteraction() {
    int resp;
    cout << "What settings would you like to complete the experimental analysis with?\n1: Bottom-up / Tabulation type programming\n2: Top-down / Memoization type programming" << endl;
    cin >> resp;
    if (resp == 1 || resp == 2) {
        timeAnalysis(resp == 1);
        reset();
    }
    else badResponse();
}

//allow users to manyally add points to the set. points are 2D, x = integer > 1 and y = real number.
void handlePointInput() {
    cout << "Please now enter the information for each point you wish to add one by one in the form 'x,y'. Note that x must be a positive integer." << endl;
    cout << "When you have finished adding points, enter 'q' to exit this stage." << endl;

    string response = "";
    int x = 0; double y = 0;
    while (true) {
        cin >> response;
        if (response == "q" || response == "Q") {
            cout << "Point generation complete. Total of " << grid.GetNumPoints() << " points added." << endl;
            return;
        }
        vector<string> pieces = getSplitString(response, ',');
        //check we have both values + no more
        if (pieces.size() != 2) badResponse();
        else {
            //when checking all input data is numerical, remember that the real number is allowed to be negative
            string realNumCheck = (pieces[1][0] == '-') ? pieces[1].substr(1,pieces[1].length() -1) : pieces[1];
            if (!(checkAllNumerical(response, ',', 1) && checkAllNumerical(realNumCheck, '.', 2))) badResponse();
            else {
                //convert to appropriate types + add the point to our grid object
                x = stoi(pieces[0]);
                y = stod(pieces[1]);
                
                if (x < 1 ) badResponse();
                else {
                    if (grid.AddPoint(x, y)) cout << "Point added. Please add another point or enter 'q' to complete point generation." << endl;
                    else cout << "Point already exists in grid. Can not add again." << endl;
                }
            }
        }
    }
}

//split string based on given delimiter (used to split up point info on commas)
vector<string> getSplitString(string str, char delimiter) {
    stringstream ss(str); string piece;
    vector<string> pieces;
    while (ss.good()) {
        getline(ss, piece, delimiter);
        pieces.push_back(piece);
    }
    return pieces;
}

//check if a set of given strings are made entirely of numerical responses
bool checkAllNumerical(string str, char delimiter, int expectedNumericalPieces) {
    vector<string> pieces = getSplitString(str, delimiter);
    int numpieces = static_cast<int>(pieces.size());
    //if there isnt actually a delimiter (no decimal point in a double for example) just check what we can for all numerical
    expectedNumericalPieces = (numpieces < expectedNumericalPieces) ? numpieces : expectedNumericalPieces;
    for (int i = 0; i < expectedNumericalPieces; i++) {
        if (!all_of(pieces[i].begin(), pieces[i].end(), [](char str) {return isdigit(str); })) return false;
    } 
    return true;
}

//finds the optimal partitioning + displays it in interval form
void getBestPartitioning() {
    if (grid.GetNumPoints() == 0) {
        cout << "No points in set to partition." << endl;
        return;
    }

    list<int> partitioning = grid.FindOptimalPartitionings_Memoization();
    cout << "The best 2D partitioning has been found! Here are its intervals:"<< endl;
    //if there are no partitions, the original [1,m] interval was not broken up
    if (partitioning.size() == 0) {
        cout << "[1, " << grid.GetMaxXVal() << "]" << endl;
    }
    //otherwise iterate through the partitions and form interval notation
    //handle start and end partitions separate to account for the [1,m] boundaries
    else {
        list<int>::iterator iterAhead = partitioning.begin();
        list<int>::iterator iter = partitioning.begin();
        iterAhead++;
        int intervalEnd = 0; int intervalStart = 0;
        intervalEnd = (*iter == 1) ? 1 : *iter - 1;
        cout << "[1, " << intervalEnd << "]";
        for (;  iterAhead != partitioning.end(); iter++, iterAhead++) {
            intervalStart = (*iter == 1) ? *iter +1 : *iter;
            intervalEnd = (iterAhead != partitioning.end()) ? *iterAhead - 1 : *iter;
            cout << "[" << intervalStart << ", " << intervalEnd << "] ";
        }
        cout << "[" << *iter << ", "<< grid.GetMaxXVal() << "]" << endl;
    }
}

//show user all previously set information for their current point set
void viewInfo(Grid grid) {
    cout << "Penalty for adding a grid element: " << grid.GetPenalty() << endl;
    cout << "Point set details: \n" << grid.GetPointInfo() << endl;
}

//automatic time analysis - used to generate data that can be used to 
//experiementally prove the run time O(n^2) for the dynamic programming algorithm in Grid.cpp
void timeAnalysis(bool tabulation) {
    string DPType = (tabulation) ? "bottom-up / tabulation" : "top-down / memoization";
    cout << "Timing finding the best partitioning of intervals for point sets up to size n = 5,000 with a penalty of -10 for each additional partition and a maximum x value of any point m = 5,000 using a " << DPType << " method, please wait..." << endl;
    cout << "======================" << endl;
    double time = 0; int numPoints;
    Grid grid = Grid();
    for (int i = 0; i <= 5000; i += 100) {
        numPoints = i;
        cout << "Time to find partitioning for point sets of size n = " << numPoints << " points: " << endl;
        for (int j = 0; j < 10; j++) {
            grid.AddRandomPoints(i, 5000); 
            time = grid.TimeToFindDivisions(tabulation);
            cout << fixed;
            cout << setprecision(9);
            cout << time << endl;
        }
        cout << "Complete. User is recommended to take the median of the above times. Moving on to next string size..." << endl;
    }
}

//manual (single) time analysis
//allows the user to see how long finding the best interval partitioning takes for their manually created point sets
void timeAnalysisSingle() {
    double time = grid.TimeToFindDivisions();
    cout << fixed;
    cout << setprecision(9);
    cout << "Time to find optimum partitioning for this point set: " << time << " seconds" << endl;
}



