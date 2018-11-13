#include "clientlevel.h"
#include <Box2D/Box2D.h>
#include <sstream>
#include <stdlib.h>

#include "core.h"
#include "input.h"

ClientLevel::ClientLevel(std::string map)
: GameLevel(map)
{

}

void ClientLevel::init(b2World* physWorld)
{
    GameLevel::init(physWorld);
}

void ClientLevel::handle_input_data()
{
    // Handle data from all players-----------------------------------------
}

void ClientLevel::send_data()
{
    // Send data to all players---------------------------------------------
}