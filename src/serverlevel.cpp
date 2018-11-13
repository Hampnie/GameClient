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

    boost::asio::ip::tcp::acceptor acceptor(Core::instance().get_context(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10058));

    //Connector connector(ID, Core::instance().get_context(), acceptor);
    //std::cout << "Host game\n";
    //connector.create_own_room();
}

void ServerLevel::handle_input_data()
{
    // Handle data from all players
}

void ServerLevel::send_data()
{
    // Send data to all players
}

