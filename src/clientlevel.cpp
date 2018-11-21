#include "clientlevel.h"
#include <Box2D/Box2D.h>
#include <sstream>
#include <stdlib.h>

#include "core.h"
#include "input.h"
#include "connector.h"

ClientLevel::ClientLevel(std::string map, boost::asio::ip::tcp::socket* playerSocket)
: 	GameLevel(map),
	playerSocket(playerSocket)
{

}

void ClientLevel::init(std::shared_ptr<b2World> physWorld)
{
    GameLevel::init(physWorld);

    playerSocket->write_some(boost::asio::buffer("Hello\n"));

    boost::asio::async_read(*playerSocket, boost::asio::buffer(buff),
                            boost::bind(&ClientLevel::read_complete, this, _1, _2),
                            boost::bind(&ClientLevel::on_read, this, _1, _2));
}

void ClientLevel::handle_input_data()
{
    // Handle data from all players-----------------------------------------
}

void ClientLevel::send_data()
{
    // Send data to all players---------------------------------------------
}

void ClientLevel::input_handler(float dt) 
{
	GameLevel::input_handler(dt);


	if(Input::keyReleased(SDLK_e))
    {
        playerSocket->write_some(boost::asio::buffer("Hello\n"));
        std::cout<<"Write message\n";
    }	
}

size_t ClientLevel::read_complete(const boost::system::error_code & err, size_t bytes)
{
    if ( err)
    {
        std::cout << err << std::endl;
        return 0;
    }
    bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void ClientLevel::on_read(const boost::system::error_code &err, size_t bytes)
{
    std::stringstream stream(buff);

    while(stream)
    {
        std::string ID;
        int x,y;
        float angle;
        stream >> ID >> x >> y >> angle;
        // Find fighter with that ID               
        if(players.find(ID) == all_paths.end())   // If fighter was not found then add him to array
        {
            FRect position{x, y, 80.0f, 84.0f};
            Fighter* fighter = new Fighter (position, physWorld);
            add_entity(fighter);
            players.insert(std::pair<std::string, Fighter*>(ID, fighter));
        } else
        {
            std::map<std::string, Fighter*>::iterator it;
            it = players.find(ID);
            // Change position and angle
        }
        
    }
    do_read();
}

void ClientLevel::do_read()
{
    
}