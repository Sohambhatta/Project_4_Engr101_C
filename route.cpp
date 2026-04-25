// Name: Soham Bhatta
// Lab Section Number: 
// Date Submitted: April 24, 2026

//------------------------------------------------------
// AUTOGRADER INFO -- IGNORE BUT DO NOT REMOVE 
// test_cases: true
// feedback('all')
// 35b24e49-2842-4603-8ba7-0f656200c2d1
//------------------------------------------------------

#include "route.h"
#include <iostream>
#include <fstream>

// --------------------------------------------------------------------------
// readLocationsFile
// Opens the locations file and reads grid dimensions, start/end positions,
// and planet entries. Any planet outside the grid is reported and skipped.
// --------------------------------------------------------------------------
bool readLocationsFile(const std::string& filename,
                       int& numRows, int& numCols,
                       int& startRow, int& startCol,
                       int& endRow,   int& endCol,
                       std::vector<Planet>& planets) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    // First three lines: grid size, start position, end position
    inFile >> numRows >> numCols;
    inFile >> startRow >> startCol;
    inFile >> endRow   >> endCol;

    // Remaining lines: one planet per line (row col symbol id)
    int row, col, id;
    char symbol;
    while (inFile >> row >> col >> symbol >> id) {
        // Check if planet is within the driver's grid (1-indexed)
        if (row < 1 || row > numRows || col < 1 || col > numCols) {
            std::cout << id << " out of range - ignoring\n";
        } else {
            Planet p;
            p.row     = row;
            p.col     = col;
            p.symbol  = symbol;
            p.id      = id;
            p.visited = false;
            planets.push_back(p);
        }
    }

    inFile.close();
    return true;
}

// --------------------------------------------------------------------------
// readNamesFile
// Opens the names file, looks up each planet ID in the planets vector,
// and assigns the corrected name.
// --------------------------------------------------------------------------
bool readNamesFile(const std::string& filename, std::vector<Planet>& planets) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        return false;
    }

    int id;
    std::string rawName;
    while (inFile >> id >> rawName) {
        std::string cleanName = fixDataCorruption(rawName);
        // Match ID to an in-range planet and assign the corrected name
        for (size_t i = 0; i < planets.size(); ++i) {
            if (planets[i].id == id) {
                planets[i].name = cleanName;
                break;
            }
        }
    }

    inFile.close();
    return true;
}

// --------------------------------------------------------------------------
// fixDataCorruption
// Removes all occurrences of "XX" and replaces underscores with spaces.
// --------------------------------------------------------------------------
std::string fixDataCorruption(const std::string& rawName) {
    std::string result = rawName;

    // Remove every pair of Xs
    size_t pos = result.find("XX");
    while (pos != std::string::npos) {
        result.erase(pos, 2);
        pos = result.find("XX");
    }

    // Replace underscores with spaces
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] == '_') {
            result[i] = ' ';
        }
    }

    return result;
}

// --------------------------------------------------------------------------
// createGrid
// Fills a numRows x numCols grid with '.', then places planet symbols,
// then the start ('S') and end ('E') markers.
// --------------------------------------------------------------------------
std::vector<std::vector<char>> createGrid(int numRows, int numCols,
                                          int startRow, int startCol,
                                          int endRow,   int endCol,
                                          const std::vector<Planet>& planets) {
    // Initialize every cell to '.'
    std::vector<std::vector<char>> grid(numRows,
                                        std::vector<char>(numCols, '.'));

    // Place each in-range planet symbol (convert 1-indexed to 0-indexed)
    for (size_t i = 0; i < planets.size(); ++i) {
        grid[planets[i].row - 1][planets[i].col - 1] = planets[i].symbol;
    }

    // Place start and end markers (they overwrite any planet at those cells)
    grid[startRow - 1][startCol - 1] = 'S';
    grid[endRow   - 1][endCol   - 1] = 'E';

    return grid;
}

// --------------------------------------------------------------------------
// findNearestPlanet
// Finds the index of the unvisited planet closest to (currentRow, currentCol).
// Uses squared Euclidean distance to avoid floating-point comparison issues.
// Ties are broken by choosing the planet with the lower ID.
// Returns -1 if all planets have been visited.
// --------------------------------------------------------------------------
int findNearestPlanet(int currentRow, int currentCol,
                      const std::vector<Planet>& planets) {
    int nearestIdx    = -1;
    int nearestDistSq = -1;
    int nearestId     = -1;

    for (size_t i = 0; i < planets.size(); ++i) {
        if (!planets[i].visited) {
            int dr     = planets[i].row - currentRow;
            int dc     = planets[i].col - currentCol;
            int distSq = dr * dr + dc * dc;

            // Select this planet if it is closer, or equally close with a lower ID
            if (nearestIdx == -1 ||
                distSq < nearestDistSq ||
                (distSq == nearestDistSq && planets[i].id < nearestId)) {
                nearestIdx    = static_cast<int>(i);
                nearestDistSq = distSq;
                nearestId     = planets[i].id;
            }
        }
    }

    return nearestIdx;
}

// --------------------------------------------------------------------------
// writeJourney
// Writes the journey.txt file:
//   1. The character map (one row per line, no spaces between columns)
//   2. "Start at r c"
//   3. For each planet in nearest-neighbor order: "Go to <name> at r c"
//   4. "End at r c"
//   5. A trailing blank line
// The nearest-neighbor algorithm is run here; planets[i].visited is updated.
// --------------------------------------------------------------------------
void writeJourney(const std::string& outputFilename,
                  const std::vector<std::vector<char>>& grid,
                  int startRow, int startCol,
                  int endRow,   int endCol,
                  std::vector<Planet>& planets) {
    std::ofstream outFile(outputFilename);

    // Print the map grid
    for (size_t r = 0; r < grid.size(); ++r) {
        for (size_t c = 0; c < grid[r].size(); ++c) {
            outFile << grid[r][c];
        }
        outFile << "\n";
    }

    // Starting location header
    outFile << "Start at " << startRow << " " << startCol << "\n";

    // Nearest-neighbor algorithm: visit every planet in closest-first order
    int currentRow = startRow;
    int currentCol = startCol;

    while (true) {
        int nextIdx = findNearestPlanet(currentRow, currentCol, planets);
        if (nextIdx == -1) {
            break; // All planets have been visited
        }

        // Mark visited and move to this planet
        planets[nextIdx].visited = true;
        currentRow = planets[nextIdx].row;
        currentCol = planets[nextIdx].col;

        outFile << "Go to " << planets[nextIdx].name
                << " at " << planets[nextIdx].row
                << " "    << planets[nextIdx].col << "\n";
    }

    // Ending location footer + required trailing blank line
    outFile << "End at " << endRow << " " << endCol << "\n";
    outFile << "\n";

    outFile.close();
}
