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

    mainFighter->set_ID(std::string("Numb"));
    //std::string str = "new " +  mainFighter->get_ID() + "\n";
    //playerSocket->write_some(boost::asio::buffer(str)); // Send ID player
    //std::cout << str;
    //do_read();

    std::stringstream sendData;
    sendData << "new " << mainFighter->get_ID() << "\n";
    out_commands.push_back(command_struct(sendData.str(), playerSocket));
    do_read();
}

void ClientLevel::handle_input_data()
{
    // Handle data from all players-----------------------------------------
    for ( command_struct &command : in_commands)
    {
        std::stringstream stream(command.str);
                std::cout << buff << "\n";
        while(stream)
        {
            std::string command;
            std::string ID;
            //bool up, down, right, left;
            float x, y;
            float angle;
            stream >> command; // Get command
            if(command == "move")
            {
                //stream >> ID >> up >> down >> right >> left >> angle;
                stream >> ID >> x >> y >> angle;
                // Find fighter with that ID 
                bool found = false;   
                for (Fighter* fighter : players)
                {
                    if(fighter->get_ID() == ID)
                    {
                        // Set position and angle
                        found = true;
                        //fighter->set_direction(up, down, right, left);
                        fighter->set_position(x, y);
                        fighter->set_angle(angle);
                    }
                }
                if(found) continue;
                // If player wasn't found
                // Create him
                FRect position{x, y, 80.0f, 84.0f};
                if(ID == "Admin")
                {
                    Fighter* fighter = new Fighter (position, physWorld, playerSocket);
                }
                Fighter* fighter = new Fighter (position, physWorld);

                fighter->set_ID(ID);
                add_entity(fighter);
                players.push_back(fighter);

            } else if( command == "bullet")
            {
                glm::vec2 bulletPoint;
                glm::vec2 playerPosition;
                stream >> bulletPoint.x >> bulletPoint.y >> playerPosition.x >> playerPosition.y;
                launch_bullet(bulletPoint, false, playerPosition);
            }       
            
        }
    }
    in_commands.clear();
}

void ClientLevel::send_data()
{
    std::stringstream sendData;
    //bool up, down, left, right;
    //mainFighter->get_direction(up, down, right, left);
    sendData << "move " << mainFighter->get_ID() << " " << mainFighter->get_rect().x << " " << mainFighter->get_rect().y << " "<< mainFighter->get_angle() << "\n";
    playerSocket->async_write_some(boost::asio::buffer(sendData.str()), boost::bind(&ClientLevel::on_send_message, this, _1, _2));

    for ( command_struct &command : out_commands)
    {
        std::cout<<command.str;
        command.playerSocket->async_write_some(boost::asio::buffer(command.str), boost::bind(&ClientLevel::on_send_message, this, _1, _2));
    }
    out_commands.clear();    
}

void ClientLevel::input_handler(float dt) 
{
	GameLevel::input_handler(dt);


    if(Input::buttonPressed(SDL_BUTTON_LEFT))
    {
        glm::vec2 mousePos = Input::mousePosition();
        if(launch_bullet(glm::vec2(mousePos.x - screenX/2, mousePos.y - screenY/2), true))
        {
            std::stringstream sendData;
            sendData << "bullet " << (mousePos.x - screenX/2) << " " << (mousePos.y - screenY/2) << " " << mainFighter->get_rect().x << " " << mainFighter->get_rect().y <<"\n";
            out_commands.push_back(command_struct(sendData.str(), playerSocket));
        }        
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
    in_commands.push_back(command_struct(buff, playerSocket));
    do_read();
}

void ClientLevel::do_read()
{
    boost::asio::async_read(*playerSocket, boost::asio::buffer(buff),
                            boost::bind(&ClientLevel::read_complete, this, _1, _2),
                            boost::bind(&ClientLevel::on_read, this, _1, _2));
}

void ClientLevel::on_send_message(const boost::system::error_code &err, size_t bytes)
{

}