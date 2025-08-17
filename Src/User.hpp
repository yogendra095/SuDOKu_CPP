#pragma once
#include <string>

class User {
private:
   std::string id, name;
    int score, diamonds;
    int unlockedLevel;
public:
    User();
    bool login(const std::string& name); //Helps in login
    void createNew(const std::string& name); //Creates a new user
    void save(); //Used to save means to write in a fiile

    
    std::string getId() const; //To get a id
    std::string getName() const; //To get a name
    int getScore() const; //To get a score
    void addScore(int n); //TO write a score in a file
    int getDiamonds() const; //To get a diamond information
    int getUnlockedLevel() const;  //TO get unlocked level information
    void addDiamonds(int n); //To add  a diamond while level gets completed in a file
    void reduceDiamonds(int n); //To reduce the diamond in a file

    void setUnlockedLevel(int lvl);  //To set new unlocked level in a file
    
};
