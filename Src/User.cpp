#include "User.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include<vector>

User::User() : score(0), diamonds(3),unlockedLevel(1) {}

bool User::login(const std::string& name) {
    std::ifstream file("users.txt");
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string fileId, fileName;
        int fileScore, fileDiamonds, fileLevel;
        
        if (iss >> fileId >> fileName >> fileScore >> fileDiamonds >> fileLevel) {
            if (fileName == name) {  // now it matches properly
                id = fileId;
                this->name = fileName;
                score = fileScore;
                diamonds = fileDiamonds;
                unlockedLevel = fileLevel;
                return true;
            }
        }
    }
    return false; // user not found
}


void User::createNew(const std::string& name) {
    std::ifstream file("users.txt");
    int count = 0;
    std::string temp;
    while (getline(file, temp)) ++count;
    id = "ID" + std::to_string(100 + count + 1);
    this->name = name;
    score = 0;
    diamonds = 3;
    unlockedLevel = 1;
    save();
}

void User::save() {
    std::vector<std::string> lines;
    std::ifstream inFile("users.txt");
    std::string line;
    bool userFound = false;
    
    // Read all existing data
    while (std::getline(inFile, line)) {
        std::istringstream iss(line);
        std::string fileId, fileName;
        int fileScore, fileDiamonds, fileLevel;
        
        // Read id as string since it's a string type
        if (iss >> fileId >> fileName >> fileScore >> fileDiamonds >> fileLevel) {
            if (fileId == this->id) {  // String comparison now works
                // Replace this user's data with updated info
                lines.push_back(id + " " + name + " " + 
                              std::to_string(score) + " " + std::to_string(diamonds) + 
                              " " + std::to_string(unlockedLevel));
                userFound = true;
            } else {
                // Keep existing line unchanged
                lines.push_back(line);
            }
        }
    }
    inFile.close();
    
    // If user wasn't found, add as new user
    if (!userFound) {
        lines.push_back(id + " " + name + " " + 
                       std::to_string(score) + " " + std::to_string(diamonds) + 
                       " " + std::to_string(unlockedLevel));
    }
    
    // Rewrite the entire file
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
