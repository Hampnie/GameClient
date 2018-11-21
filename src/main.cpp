#include <iostream>

#include "core.h"
#include "startlevel.h"

int main()
{
    Core *gamecore = &Core::instance();
    gamecore->init(1024, 768, false);
    StartLevel *startLevel = new StartLevel();
    gamecore->install_level(startLevel);
    gamecore->mainloop();
    gamecore->release_resources();

    return 0;
}
