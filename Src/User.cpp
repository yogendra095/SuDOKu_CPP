#include "User.hpp"
#include <fstream>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include<vector>

static std::string generateId() {
    return std::to_string(std::time(nullptr)) + std::to_string(rand() % 1000);
}               //This is only visible in this file so no naming conflict


User::User() : score(0), diamonds(3),unlockedLevel(1) {}

bool User::login(const std::string& name) {
    std::ifstream file("users.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string fileId, fileName;
        int fileScore, fileDiamonds, fileLevel;
        
        if (iss >> fileId >> fileName >> fileScore >> fileDiamonds >> fileLevel) {
            if (fileName == name) {  // found user
                id = fileId;
                this->name = fileName;
                score = fileScore;
                diamonds = fileDiamonds;
                unlockedLevel = fileLevel;
                return true;
            }
        }
    }
    return false; // not found
}


void User::createNew(const std::string& name) {
    // Make a unique ID
    id = generateId();
    
    this->name = name;
    score = 0;
    diamonds = 3;
    unlockedLevel = 1;

    // Add new user to file
    std::ofstream file("users.txt", std::ios::app);
    file << id << " " << this->name << " "
         << score << " " << diamonds << " "
         << unlockedLevel << "\n";
}

void User::save() {
    std::vector<std::string> lines;
    std::ifstream inFile("users.txt");
    std::string line;
    bool userFound = false;
    
    // Read all users
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string fileId, fileName;
        int fileScore, fileDiamonds, fileLevel;
        
    // Read id as string
        if (iss >> fileId >> fileName >> fileScore >> fileDiamonds >> fileLevel) {
            if (fileId == this->id) {  // found this user
                // Update this user's info if new data found
                lines.push_back(id + " " + name + " " + 
                              std::to_string(score) + " " + std::to_string(diamonds) + 
                              " " + std::to_string(unlockedLevel));
                userFound = true;
            } else {
                // Other wise keep the same
                lines.push_back(line);
            }
        }
    }
    inFile.close();
    
    // Adds the new user data to the list if it wasn't in the file.
    if (!userFound) {
        lines.push_back(id + " " + name + " " + 
                       std::to_string(score) + " " + std::to_string(diamonds) + 
                       " " + std::to_string(unlockedLevel));
    }
    
    // Save all users
    std::ofstream outFile("users.txt");
    for (const auto& l : lines) {
        outFile << l << "\n";
    }
}
std::string User::getId() const { return id; }
std::string User::getName() const { return name; }
int User::getScore() const { return score; }
int User::getDiamonds() const { return diamonds; }
int User::getUnlockedLevel() const { return unlockedLevel; }
void User::setUnlockedLevel(int lvl) { unlockedLevel = lvl; }
void User::addDiamonds(int n) { diamonds += n; save(); }
void User::reduceDiamonds(int n) { diamonds -= n; if(diamonds < 0) diamonds = 0; save(); }
void User::addScore(int n) { 
    score += n; 
    save(); 
}