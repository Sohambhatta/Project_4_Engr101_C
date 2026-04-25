// Name: Soham Bhatta
// Lab Section Number: 
// Date Submitted: April 24, 2026

//------------------------------------------------------
// AUTOGRADER INFO -- IGNORE BUT DO NOT REMOVE 
// test_cases: true
// feedback('all')
// 35b24e49-2842-4603-8ba7-0f656200c2d1
//------------------------------------------------------

#ifndef ROUTE_H
#define ROUTE_H

#include <string>
#include <vector>

// Struct to hold all data for a single planet
struct Planet {
    int row;        // 1-indexed row in the grid
    int col;        // 1-indexed column in the grid
    char symbol;    // single-character map symbol
    int id;         // unique planet ID from the locations file
    std::string name; // planet name (corrected for data corruption)
    bool visited;   // whether the nearest-neighbor algorithm has visited this planet
};

// Reads the locations file; fills grid dimensions, start/end positions, and planet list.
// Prints a message and skips any planet whose coordinates lie outside the grid.
// Returns false if the file cannot be opened.
bool readLocationsFile(const std::string& filename,
                       int& numRows, int& numCols,
                       int& startRow, int& startCol,
                       int& endRow,   int& endCol,
                       std::vector<Planet>& planets);

// Reads the names file; matches each ID to a planet in the vector and assigns
// the corrected name. Returns false if the file cannot be opened.
bool readNamesFile(const std::string& filename, std::vector<Planet>& planets);

// Removes all "XX" substrings and replaces underscores with spaces in rawName.
std::string fixDataCorruption(const std::string& rawName);

// Builds and returns a 2-D character grid (numRows x numCols) with:
//   '.' at empty cells, planet symbols at planet cells, 'S' at start, 'E' at end.
std::vector<std::vector<char>> createGrid(int numRows, int numCols,
                                          int startRow, int startCol,
                                          int endRow,   int endCol,
                                          const std::vector<Planet>& planets);

// Returns the index (into planets) of the closest unvisited planet to (currentRow, currentCol).
// Ties are broken by choosing the planet with the lower ID. Returns -1 if none remain.
int findNearestPlanet(int currentRow, int currentCol,
                      const std::vector<Planet>& planets);

// Writes the journey.txt output file: prints the map, then runs the nearest-neighbor
// algorithm to order the route stops, writing each stop line as it is determined.
void writeJourney(const std::string& outputFilename,
                  const std::vector<std::vector<char>>& grid,
                  int startRow, int startCol,
                  int endRow,   int endCol,
                  std::vector<Planet>& planets);

#endif
