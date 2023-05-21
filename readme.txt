PROBLEM STATEMENT:
Given a set of N points (x_i, y_i) with integer values for x_i between 1 and M and real values for y_i, find
a partitioning of the interval [1, M] into contiguous intervals such that the error of approximating points
in each interval element (induced by the partitioning on X axis) by the average value of y in the interval
element is minimized. Include a penalty factor proportional to the total number of intervals the solution has.

HOW TO RUN:
Either open the precreated .exe in this same folder, or rebuild the solution yourself if you wish. If so, the command is
g++ Grid.cpp GridBasedApproximation.cpp -o GridBasedApproximation.exe

EXTRA INFO:
The real meat and potatos of this code is the Grid.cpp file; GridBasedApproximation.cpp is just the setup for the user
to interact with the rest of the code. Grid.cpp is where the problem statement is solved.

The problem statement was solved using two different approaches - both a bottom-up tabulation method and a top-down
memoization method. This was to allow comparison between the two methods during experiemental analysis.
