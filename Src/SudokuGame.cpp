#include "SudokuGame.hpp"
#include <random>
#include <algorithm>
#include <iostream>

// Utility functions for generating Sudoku
namespace {
    bool isSafe(const std::array<std::array<int,9>,9>& board, int row, int col, int num) {
        for(int x=0;x<9;x++)
            if(board[row][x]==num || board[x][col]==num) return false;
        int startRow=row-row%3, startCol=col-col%3;
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                if(board[startRow+i][startCol+j]==num) return false;
        return true;
    }

    bool fillBoard(std::array<std::array<int,9>,9>& board){
        for(int r=0;r<9;r++){
            for(int c=0;c<9;c++){
                if(board[r][c]==0){
                    std::vector<int> nums{1,2,3,4,5,6,7,8,9};
                    std::random_device rd; std::mt19937 g(rd());
                    std::shuffle(nums.begin(), nums.end(), g);
                    for(int n : nums){
                        if(isSafe(board,r,c,n)){
                            board[r][c]=n;
                            if(fillBoard(board)) return true;
                            board[r][c]=0;
                        }
                    }
                    return false;
                }
            }
        }
        return true;
    }

    void makePuzzle(std::array<std::array<int,9>,9>& board, int emptyCells){
        std::random_device rd; std::mt19937 g(rd());
        while(emptyCells>0){
            int r = g()%9; int c = g()%9;
            if(board[r][c]!=0){ board[r][c]=0; emptyCells--; }
        }
    }
}

// Constructor
SudokuGame::SudokuGame(sf::Font& font, int difficulty, int diamonds)
    : font(font), diamonds(diamonds)
{
    // Set time limit based on difficulty (in minutes)
    switch(difficulty) {
        case 1: timeLimitMinutes = 20; break;   // Easy
        case 2: timeLimitMinutes = 30; break;   // Medium
        case 3: timeLimitMinutes = 40; break;  // Hard
        case 4: timeLimitMinutes = 50; break;  // Very Hard
        default: timeLimitMinutes = 5; break;
    }

    gameClock.restart();
    generateBoard(difficulty); // generate your puzzle and solution
}

// Generate board
void SudokuGame::generateBoard(int difficulty) {
    fillBoard(solution);
    puzzle = solution;

    int emptyCells = 30 + (difficulty-1)*5; // Easy=30, Medium=35, Hard=40, etc
    makePuzzle(puzzle, emptyCells);

    // Set fixed cells & colors
    for(int r=0;r<9;r++)
        for(int c=0;c<9;c++){
            fixedCells[r][c] = puzzle[r][c]!=0;
            cellColors[r][c] = sf::Color::Black;
        }

    gameClock.restart();
}

// Draw the board
void SudokuGame::draw(sf::RenderWindow& window) {
    const float cellSize = 50.f;
    const float boardSize = cellSize * 9;
    sf::Vector2u winSize = window.getSize();
    sf::Vector2f startPos((winSize.x - boardSize) / 2.f, (winSize.y - boardSize) / 2.f);

    // Draw all small cells (thin lines)
    sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
    cell.setFillColor(sf::Color::White);
    cell.setOutlineColor(sf::Color::Black);
    cell.setOutlineThickness(1.f);

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            cell.setPosition({startPos.x + c * cellSize, startPos.y + r * cellSize});
            if (r == selectedRow && c == selectedCol) cell.setFillColor(sf::Color(200,200,255));
            else cell.setFillColor(sf::Color::White);
            window.draw(cell);

            // Draw number
            if (puzzle[r][c] != 0) {
                sf::Text text(font, std::to_string(puzzle[r][c]), 24);
                text.setFillColor(cellColors[r][c]);
                auto bounds = text.getLocalBounds();
                text.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
                text.setPosition(cell.getPosition() + sf::Vector2f(cellSize/2.f, cellSize/2.f - 5.f));
                window.draw(text);
            }
        }
    }

    // Draw thick lines for 3x3 subgrid boundaries
    sf::RectangleShape line(sf::Vector2f(boardSize, 3.f)); // horizontal line
    line.setFillColor(sf::Color::Black);
    for (int i = 0; i <= 9; i += 3) {
        line.setSize(sf::Vector2f(boardSize, 3.f));
        line.setPosition({startPos.x, startPos.y + i * cellSize - 1.5f}); // horizontal
        window.draw(line);

        line.setSize(sf::Vector2f(3.f, boardSize)); // vertical
        line.setPosition({startPos.x + i * cellSize - 1.5f, startPos.y});
        window.draw(line);
    }
}


// Mouse click
void SudokuGame::handleMouseClick(sf::Vector2f mousePos){
    const float cellSize = 50.f;
    const float boardSize = cellSize * 9;

    // Calculate top-left position of the centered board
    sf::Vector2f startPos((800 - boardSize) / 2.f, (600 - boardSize) / 2.f); // assuming 800x600 window

    int col = (mousePos.x - startPos.x) / cellSize;
    int row = (mousePos.y - startPos.y) / cellSize;

    if(row >= 0 && row < 9 && col >= 0 && col < 9 && !fixedCells[row][col]){
        selectedRow = row;
        selectedCol = col;
    }
}

// Keyboard input
void SudokuGame::handleKeyboardInput(int number){
    if(selectedRow == -1 || selectedCol == -1) return; // no cell selected
    if(fixedCells[selectedRow][selectedCol]) return;   // cannot change fixed cell

    puzzle[selectedRow][selectedCol] = number;

    if(solution[selectedRow][selectedCol] == number){
        cellColors[selectedRow][selectedCol] = sf::Color::Black;  // correct number
    } else {
        cellColors[selectedRow][selectedCol] = sf::Color::Red;    // wrong number
        if(diamonds > 0) diamonds--;                              // reduce diamonds
    }
}

void SudokuGame::handleArrowKeys(sf::Keyboard::Key key) {
    if(selectedRow == -1 || selectedCol == -1) {
        // Start from top-left if no cell is selected
        for(int r = 0; r < 9; r++) {
            for(int c = 0; c < 9; c++) {
                if(!fixedCells[r][c]) {
                    selectedRow = r;
                    selectedCol = c;
                    return;
                }
            }
        }
        return;
    }

    int newRow = selectedRow;
    int newCol = selectedCol;

    switch(key) {
        case sf::Keyboard::Key::Up:
            do { newRow = (newRow > 0 ? newRow - 1 : 8); } while(fixedCells[newRow][newCol]);
            break;
        case sf::Keyboard::Key::Down:
            do { newRow = (newRow < 8 ? newRow + 1 : 0); } while(fixedCells[newRow][newCol]);
            break;
        case sf::Keyboard::Key::Left:
            do { newCol = (newCol > 0 ? newCol - 1 : 8); } while(fixedCells[newRow][newCol]);
            break;
        case sf::Keyboard::Key::Right:
            do { newCol = (newCol < 8 ? newCol + 1 : 0); } while(fixedCells[newRow][newCol]);
            break;
        default:
            return;
    }

    selectedRow = newRow;
    selectedCol = newCol;
}

// Check completion
bool SudokuGame::isCompleted() const{
    for(int r=0;r<9;r++)
        for(int c=0;c<9;c++)
            if(puzzle[r][c]!=solution[r][c]) return false;
    return true;
}

// Remaining time in seconds
int SudokuGame::getRemainingTime() const {
    int elapsed = static_cast<int>(gameClock.getElapsedTime().asSeconds());
    int remaining = timeLimitMinutes * 60 - elapsed;
    return remaining > 0 ? remaining : 0;
}