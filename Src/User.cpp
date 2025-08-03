#include "User.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

User::User() : score(0), diamonds(3) {}

bool User::login(const std::string& name) {
    std::ifstream file("users.txt");
    std::string n;
    while (file >> id >> n >> score >> diamonds) {
        if (n == name) {
           this->name = n;
            return true;
        }
    }
    return false;
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
    save();
}

void User::save() {
    std::ofstream file("users.txt", std::ios::app);
    file << id << " " << name << " " << score << " " << diamonds << "\n";
}

std::string User::getId() const { return id; }
std::string User::getName() const { return name; }
int User::getScore() const { return score; }
int User::getDiamonds() const { return diamonds; }
