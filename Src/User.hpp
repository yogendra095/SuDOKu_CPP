#pragma once
#include <string>

class User {
private:
    std::string id, name;
    int score, diamonds;
    int unlockedLevel;
public:
    User();
    bool login(const std::string& name);
    void createNew(const std::string& name);
    void save();

    //Getters
    std::string getId() const;
    std::string getName() const;
    int getScore() const;
    int getDiamonds() const;
    int getUnlockedLevel() const; 
    void addDiamonds(int n);
    void reduceDiamonds(int n);

    // Setter
    void setUnlockedLevel(int lvl); 
    
};
