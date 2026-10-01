#include "engine/CoreEngine.h"

int main() {
    CoreEngine engine;
    if (!engine.init()) {
        return 1;
    }
    engine.run();
    return 0;
}
