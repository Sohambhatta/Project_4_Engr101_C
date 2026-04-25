// Name: Soham Bhatta
// Lab Section Number: 
// Date Submitted: April 24, 2026

//------------------------------------------------------
// AUTOGRADER INFO -- IGNORE BUT DO NOT REMOVE 
// test_cases: true
// feedback('all')
// 35b24e49-2842-4603-8ba7-0f656200c2d1
//------------------------------------------------------

#include <iostream>
#include <string>
#include <vector>
#include "route.h"

int main() {
    // Prompt user for input filenames
    std::string locationsFilename, namesFilename;

    std::cout << "Enter Locations Filename: ";
    std::cin  >> locationsFilename;

    std::cout << "Enter Names Filename: ";
    std::cin  >> namesFilename;

    // Grid dimensions and start/end positions read from the locations file
    int numRows, numCols;
    int startRow, startCol;
    int endRow,   endCol;
    std::vector<Planet> planets;

    // Read planet location data (out-of-range planets are skipped with a message)
    if (!readLocationsFile(locationsFilename,
                           numRows, numCols,
                           startRow, startCol,
                           endRow,   endCol,
                           planets)) {
        std::cout << "Input file could not be opened\n";
        return 1;
    }

    // Read and match planet names (fixes data corruption internally)
    if (!readNamesFile(namesFilename, planets)) {
        std::cout << "Input file could not be opened\n";
        return 1;
    }

    // Build the character map grid
    std::vector<std::vector<char>> grid = createGrid(numRows, numCols,
                                                     startRow, startCol,
                                                     endRow,   endCol,
                                                     planets);

    // Run the nearest-neighbor algorithm and write the output file
    writeJourney("journey.txt", grid,
                 startRow, startCol,
                 endRow,   endCol,
                 planets);

    return 0;
}
