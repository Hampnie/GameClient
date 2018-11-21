#include "serverlevel.h"
#include <Box2D/Box2D.h>
#include <fstream>
#include <stdlib.h>
#include <boost/bind.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include "core.h"
#include "input.h"

ServerLevel::ServerLevel(std::string map)
: 	GameLevel(map),
	acceptor(std::make_unique<boost::asio::ip::tcp::acceptor>(*Core::instance().get_context(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10058)))
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
}

void ServerLevel::send_data()
{
    // Send data to all players
}

void ServerLevel::wait_new_player()
{
	FRect default_position{0.0f,0.0f,0.0f,0.0f};
	players.push_back(new Fighter(default_position, phWorld));
	std::cout << players.size() << "\n";

	acceptor->async_accept(*players.back()->get_socket(), boost::bind(&ServerLevel::handle_accept, this, players.back()->get_socket(), _1));
	std::cout << "Waiting new players...\n";	
}

void ServerLevel::handle_accept(boost::asio::ip::tcp::socket* player, const boost::system::error_code &err)
{
	std::cout<< "Player connected.\n";
	//players.back().async_read_some(boost::asio::buffer(buff), boost::bind(&ServerLevel::on_read, this, &players.back(), _1));

	do_read(player);
	wait_new_player();	
}

void ServerLevel::on_read(boost::asio::ip::tcp::socket* player, const boost::system::error_code &err, size_t bytes)
{
	std::cout<< "Read message.\n";
	std::cout<< buff;
	do_read(player);
}

void ServerLevel::do_read(boost::asio::ip::tcp::socket* player)
{
	std::cout<<"Do read\n";
	//memset(buff, 0, sizeof(buff));
	//player->async_read_some(boost::asio::buffer(buff), boost::bind(&ServerLevel::on_read, this, player, _1));
	boost::asio::async_read(*player, boost::asio::buffer(buff),
                            boost::bind(&ServerLevel::read_complete, this, _1, _2),
                            boost::bind(&ServerLevel::on_read, this, player, _1, _2));
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
}

