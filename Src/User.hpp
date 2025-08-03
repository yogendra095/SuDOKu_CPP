#pragma once
#include <string>

class User {
private:
    std::string id, name;
    int score, diamonds;
public:
    User();
    bool login(const std::string& name);
    void createNew(const std::string& name);
    std::string getId() const;
    std::string getName() const;
    int getScore() const;
    int getDiamonds() const;
    void save();
};
