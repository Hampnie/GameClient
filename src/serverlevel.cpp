#include "serverlevel.h"
#include <Box2D/Box2D.h>
#include <fstream>
#include <stdlib.h>

#include <glm/gtc/matrix_transform.hpp>

#include "core.h"
#include "input.h"

ServerLevel::ServerLevel(std::string map)
: GameLevel(map)
{

}

void ServerLevel::init(b2World* physWorld)
{
    GameLevel::init(physWorld);
}

void ServerLevel::handle_input_data()
{
    // Handle data from all players
}

void ServerLevel::send_data()
{
    // Send data to all players
}

