#include "serverlevel.h"
#include <Box2D/Box2D.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <boost/bind.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "core.h"
#include "input.h"

ServerLevel::ServerLevel(std::string map)
: 	GameLevel(map),
	acceptor(std::make_unique<boost::asio::ip::tcp::acceptor>(*Core::instance().get_context(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10058))),
	map(map)
{	
}

void ServerLevel::init(std::shared_ptr<b2World> physWorld)
{
	phWorld = physWorld;
    GameLevel::init(phWorld);


    wait_new_player(); 
}

void ServerLevel::handle_input_data()
{
    // Handle data from all players
    for ( command_struct &command : in_commands)
    {
		std::stringstream stream(command.str);
	    std::cout << buff << "\n";
	           
		while(stream)
	    {
	        std::string str;
	        std::string ID;
	        //bool up, down, right, left;
	        float x, y;
	        float angle;
	        stream >> str; // Get command

	        if(str == "new")
	        {
	        	stream >> ID;
	            FRect position{512, 600, 80.0f, 84.0f};
	            Fighter* fighter = new Fighter (position, phWorld, command.playerSocket);
	            fighter->set_ID(ID);
	            add_entity(fighter);
	            players.push_back(fighter);


	           	//Send admin data
	           	std::stringstream adminSendData;
		    	//mainFighter->get_direction(up, down, right, left);
		    	adminSendData << "move Admin " << x << " " << y << " "<< mainFighter->get_angle() << "\n";
		        command.playerSocket->async_write_some(boost::asio::buffer(adminSendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
	        } 
	        else if( str == "move")
	        {
	        	
				//stream >> ID >> up >> down >> right >> left >> angle;
				stream >> ID >> x >> y >> angle;
	            // Find fighter with that ID  
	            for (Fighter* fighter : players)
	            {
	                if(fighter->get_ID() == ID)
	                {
	                    // Set position and angle
	                    //fighter->set_direction(up, down, right, left);
	                    fighter->set_position(x, y);
                        fighter->set_angle(angle);
	                }
	            }   
	        }
	        else if( str == "bullet")
	        {
	        	glm::vec2 bulletPoint;
                glm::vec2 playerPosition;
                stream >> bulletPoint.x >> bulletPoint.y >> playerPosition.x >> playerPosition.y;
                launch_bullet(bulletPoint, false, playerPosition);

                // Send to player
                for (Fighter* fighter : players)
		        {
		        	if(fighter->get_socket() == command.playerSocket) // Himself
		        		continue;

			        fighter->get_socket()->async_write_some(boost::asio::buffer(command.str), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
		        }
	        }
	    }	    
	}
	in_commands.clear();
}

void ServerLevel::send_data()
{
    // Send data to all players
    for (Fighter* fighter : players)
    {
    	// Send admin data
    	std::stringstream adminSendData;
    	//bool up, down, right, left;
    	FRect position = mainFighter->get_rect();
    	//mainFighter->get_direction(up, down, right, left);

    	adminSendData << "move Admin " << mainFighter->get_rect().x << " " << mainFighter->get_rect().y << " "<< mainFighter->get_angle() << "\n";
        fighter->get_socket()->async_write_some(boost::asio::buffer(adminSendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));

        // Send other players data
        for (Fighter* otherFighter : players)
        {
        	if(otherFighter == fighter) // Himself
        		continue;

        	 // Send player data
			std::stringstream sendData;
			//otherFighter->get_direction(up, down, right, left);
    		sendData << "move " << otherFighter->get_ID() << " " << otherFighter->get_rect().x << " " << otherFighter->get_rect().y << " "<< otherFighter->get_angle() << "\n";	     
	        fighter->get_socket()->async_write_some(boost::asio::buffer(sendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
        }
    }

    for ( command_struct &command : out_commands)
    {
    	for (Fighter* fighter : players)
    	{
    		std::cout<<command.str;
        	fighter->get_socket()->async_write_some(boost::asio::buffer(command.str), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
    	}        
    }
    out_commands.clear();  
}

void ServerLevel::wait_new_player()
{
	sockets.push_back(new boost::asio::ip::tcp::socket(*Core::instance().get_context()));

	acceptor->async_accept(*sockets.back(), boost::bind(&ServerLevel::handle_accept, this, sockets.back(), _1));
	std::cout << "Waiting new players...\n";	
}

void ServerLevel::handle_accept(boost::asio::ip::tcp::socket* playerSocket, const boost::system::error_code &err)
{
	std::cout<< "Player connected.\n";

	//Send map to player
	playerSocket->async_write_some(boost::asio::buffer(map), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
	
	do_read(playerSocket);
	wait_new_player();	
}

void ServerLevel::on_read(boost::asio::ip::tcp::socket* playerSocket, const boost::system::error_code &err, size_t bytes)
{
	std::cout<< "Message was read.\n";
	in_commands.push_back(command_struct(buff, playerSocket));

	do_read(playerSocket);
}

void ServerLevel::do_read(boost::asio::ip::tcp::socket* playerSocket)
{
	std::cout<<"Do read\n";
	//memset(buff, 0, sizeof(buff));
	//player->async_read_some(boost::asio::buffer(buff), boost::bind(&ServerLevel::on_read, this, player, _1));
	boost::asio::async_read(*playerSocket, boost::asio::buffer(buff),
                            boost::bind(&ServerLevel::read_complete, this, _1, _2),
                            boost::bind(&ServerLevel::on_read, this, playerSocket, _1, _2));
}

size_t ServerLevel::read_complete(const boost::system::error_code & err, size_t bytes)
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

void ServerLevel::input_handler(float dt) 
{
	GameLevel::input_handler(dt);

    if(Input::buttonPressed(SDL_BUTTON_LEFT))
    {
        glm::vec2 mousePos = Input::mousePosition();
        if(launch_bullet(glm::vec2(mousePos.x - screenX/2, mousePos.y - screenY/2), true))
        {
        	std::stringstream sendData;
	        sendData << "bullet " << (mousePos.x - screenX/2) << " " << (mousePos.y - screenY/2) << " " << mainFighter->get_rect().x << " " << mainFighter->get_rect().y <<"\n";
	        out_commands.push_back(command_struct(sendData.str(), mainFighter->get_socket()));
        }        
    }

    if(Input::keyPressed(SDLK_e))
    {
    	std::cout<<"Here\n";
              mainFighter->set_position(0,0);
    }

}

void ServerLevel::on_send_message(const boost::system::error_code &err, size_t bytes)
{

}

