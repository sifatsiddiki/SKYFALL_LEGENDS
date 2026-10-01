#pragma once
#include <vector>
#include <string>

class Menu {
public:
    Menu() = default;
    Menu(std::initializer_list<std::string> items);

    const std::vector<std::string>& getItems() const { return entries; }
    int getSelectedIndex() const { return selected; }

    void moveSelection(int delta);

private:
    std::vector<std::string> entries;
    int selected = 0;
};
