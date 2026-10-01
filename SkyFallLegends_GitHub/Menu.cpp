#include "ui/Menu.h"

Menu::Menu(std::initializer_list<std::string> items)
: entries(items) {
    if (entries.empty()) {
        selected = 0;
    } else {
        selected = 0;
    }
}

void Menu::moveSelection(int delta) {
    if (entries.empty()) return;
    selected += delta;
    if (selected < 0) selected = static_cast<int>(entries.size()) - 1;
    if (selected >= static_cast<int>(entries.size())) selected = 0;
}
