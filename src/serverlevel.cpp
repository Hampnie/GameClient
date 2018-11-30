#include "serverlevel.h"
#include <Box2D/Box2D.h>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <boost/bind.hpp>


#include <glm/gtc/matrix_transform.hpp>

#include "core.h"
#include "bullet.h"
#include "wall.h"
#include "input.h"

ServerLevel::ServerLevel(std::string map)
: 	acceptor(std::make_unique<boost::asio::ip::tcp::acceptor>(*Core::instance().get_context(), boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10058))),
	map(map)
{	
    glm::vec2 windowSize = Core::instance().get_window_size();
    screenX = windowSize.x;
    screenY = windowSize.y;
}

void ServerLevel::init(std::shared_ptr<b2World> physWorld)
{
	phWorld = physWorld;

    Level::init(physWorld);

    // Load map
    float x, y, width, height;
    std::stringstream stream(map);

    while(stream)
    {
        stream >> x >> y >> width >> height;
        FRect position1{x, y, width, height};
        Wall* wall = new Wall(position1, Entity::phys_body_type::STATIC, physWorld, "Wall");
        add_entity(wall);
    }

    // Add player
    FRect position{512, 384, 80.0f, 84.0f};
    mainFighter = std::make_shared<Fighter>(position, "Admin", physWorld);
    add_entity(mainFighter.get());

    // Start loop
    wait_new_player(); 
}

void ServerLevel::handle_input_data()
{
    // Handle data from all players
    for ( command_struct &command : in_commands)
    {
		std::stringstream stream(command.str);
	           
		while(stream)
	    {
	        std::string str;
	        std::string ID;
	        float x, y;
	        float angle;

	        stream >> str; // Get command

	        if(str == "new")
	        {
	            FRect position{512, 600, 80.0f, 84.0f};

                Fighter* fighter = new Fighter (position, generate_random_id(), phWorld, command.playerSocket);
	            add_entity(fighter);
	            players.push_back(fighter);

	           	//Send player's new position and ID
	           	std::stringstream adminSendData;
		    	adminSendData << "start " << fighter->get_ID() << " " << position.x << " " << position.y << " " << " 0" << "\n";
		        command.playerSocket->async_write_some(boost::asio::buffer(adminSendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
	        } 
	        else if( str == "bullet")
	        {
                int x_dir, y_dir;                
				stream >> ID >> x_dir >> y_dir;
	            // Find fighter with that ID  
	            for (Fighter* fighter : players)
	            {
	                if(fighter->get_ID() == ID)
	                {
	                    launch_bullet(glm::vec2(x_dir, y_dir), fighter);
	                }
	            }   
	        }
            else if( str == "angle")
            {
                float newAngle;
                stream >> ID >> newAngle;
                // Find fighter with that ID  
                for (Fighter* fighter : players)
                {
                    if(fighter->get_ID() == ID)
                    {
                        fighter->set_angle(newAngle);
                    }
                }   
            }
            else if( str == "keyPress")
            {
                std::string key;
                stream >> key >> ID;

                // Find fighter with that ID  
                for (Fighter* fighter : players)
                {
                    if(fighter->get_ID() == ID)
                    {
                        if(key == "a")
                        {
                            fighter->set_true_velocity(Fighter::MOVE_DIRECTION::LEFT);
                        }
                        else if(key == "d")
                        {
                            fighter->set_true_velocity(Fighter::MOVE_DIRECTION::RIGHT);
                        }
                        else if(key == "s")
                        {
                            fighter->set_true_velocity(Fighter::MOVE_DIRECTION::DOWN);
                        }
                        else if(key ==  "w")
                        {
                            fighter->set_true_velocity(Fighter::MOVE_DIRECTION::UP);
                        }
                    }
                }                
            }
	        else if( str == "keyRelease")
	        {
	        	std::string key;
                stream >> key >> ID;

                // Find fighter with that ID  
                for (Fighter* fighter : players)
                {
                    if(fighter->get_ID() == ID)
                    {
                        if(key == "a")
                        {
                            fighter->set_false_velocity(Fighter::MOVE_DIRECTION::LEFT);
                        }
                        else if(key == "d")
                        {
                            fighter->set_false_velocity(Fighter::MOVE_DIRECTION::RIGHT);
                        }
                        else if(key == "s")
                        {
                            fighter->set_false_velocity(Fighter::MOVE_DIRECTION::DOWN);
                        }
                        else if(key ==  "w")
                        {
                            fighter->set_false_velocity(Fighter::MOVE_DIRECTION::UP);
                        }
                    }
                }                
	        }
	    }	    
	}
	in_commands.clear();
}

void ServerLevel::send_data()
{
    // Send data to each player
    for (Fighter* fighter : players)
    {
    	// Send admin data
    	std::stringstream adminSendData;
    	FRect position = mainFighter->get_rectangle();

    	adminSendData << "move Admin " << mainFighter->get_rectangle().x << " " << mainFighter->get_rectangle().y << " "<< mainFighter->get_angle() << "\n";
        fighter->get_socket()->async_write_some(boost::asio::buffer(adminSendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));

        // Send other players' position
        for (Fighter* otherFighter : players)
        {
             // Send player data
            std::stringstream sendData;
            sendData << "move " << otherFighter->get_ID() << " " << otherFighter->get_rectangle().x << " " << otherFighter->get_rectangle().y << " "<< otherFighter->get_angle() << "\n";        
            fighter->get_socket()->async_write_some(boost::asio::buffer(sendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
        }
        for (Bullet* bullet : bullets)
        {
             // Send player data
            std::stringstream sendData;
            sendData << "bullet " << bullet->get_ID() << " " << bullet->get_rectangle().x << " " << bullet->get_rectangle().y << "\n";        
            fighter->get_socket()->async_write_some(boost::asio::buffer(sendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
        }
    } 

    for (auto command : out_commands)
    {
        for (Fighter* fighter : players)
        {
            fighter->get_socket()->async_write_some(boost::asio::buffer(command.str), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
        }
    }
    out_commands.clear();
}

void ServerLevel::wait_new_player()
{
	clients.push_back(new Client());
	acceptor->async_accept(*clients.back()->get_socket(), boost::bind(&ServerLevel::handle_accept, this, clients.back(), _1));
	std::cout << "Waiting new players...\n";	
}

void ServerLevel::handle_accept(Client *client, const boost::system::error_code &err)
{
	std::cout<< "Player connected.\n";

	//Send map to player
	client->get_socket()->async_write_some(boost::asio::buffer(map), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
	
	do_read(client);
	wait_new_player();	
}

void ServerLevel::on_read(Client *client, const boost::system::error_code &err, size_t bytes)
{
	in_commands.push_back(command_struct(client->get_buffer(), client->get_socket()));
	do_read(client);
}

void ServerLevel::do_read(Client *client)
{
	boost::asio::async_read(*client->get_socket(), boost::asio::buffer(client->get_buffer()),
                            boost::bind(&ServerLevel::read_complete, this, client, _1, _2),
                            boost::bind(&ServerLevel::on_read, this, client, _1, _2));
}


size_t ServerLevel::read_complete(Client *client, const boost::system::error_code & err, size_t bytes)
{
    if ( err)
    {
        std::cout << err << std::endl;
        return 0;
    }
    bool found = std::find(client->get_buffer(), client->get_buffer() + bytes, '\n') < client->get_buffer() + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void ServerLevel::update(float dt) 
{
	Level::update(dt);

    if(Input::buttonPressed(SDL_BUTTON_LEFT) && cooldown >= 1.0f)
    {
        glm::vec2 mousePos = Input::mousePosition();
        launch_bullet(glm::vec2(mousePos.x - screenX/2, mousePos.y - screenY/2), mainFighter.get());
        cooldown = 0.0f;  
    }
    if(Input::keyPressed(SDLK_w))
    {
        mainFighter->set_true_velocity(Fighter::MOVE_DIRECTION::UP);
    }
    if(Input::keyPressed(SDLK_s))
    {        
        mainFighter->set_true_velocity(Fighter::MOVE_DIRECTION::DOWN);
    }
    if(Input::keyPressed(SDLK_a))
    {
        mainFighter->set_true_velocity(Fighter::MOVE_DIRECTION::LEFT);
    }
    if(Input::keyPressed(SDLK_d))
    {
        mainFighter->set_true_velocity(Fighter::MOVE_DIRECTION::RIGHT);
    }
    if(Input::keyReleased(SDLK_w))
    {
        mainFighter->set_false_velocity(Fighter::MOVE_DIRECTION::UP);
    }
    if(Input::keyReleased(SDLK_s))
    {        
        mainFighter->set_false_velocity(Fighter::MOVE_DIRECTION::DOWN);
    }
    if(Input::keyReleased(SDLK_a))
    {
        mainFighter->set_false_velocity(Fighter::MOVE_DIRECTION::LEFT);
    }
    if(Input::keyReleased(SDLK_d))
    {
        mainFighter->set_false_velocity(Fighter::MOVE_DIRECTION::RIGHT);
    }

    glm::vec2 mousePos = Input::mousePosition();
    glm::vec2 playerCenter{screenX/2, screenY/2};
    mainFighter->set_angle(atan2(playerCenter.y - mousePos.y, playerCenter.x - mousePos.x));

    cooldown += dt;

    // Send prepared to destroy entities
    for(auto const& entity: destroyEntities)
    {
        // First clear our vectors
        for (Fighter* fighter : players)
        {
            if(fighter->get_ID() == entity->get_ID())
            {
                // If admin was killed
                if(entity->get_ID() == mainFighter->get_ID())
                {
                    
                } else
                {
                    // Immediately delete and send it to players
                    delete_player(entity);                    
                }
            }
        }
        for (Bullet* bullet : bullets)
        {
            if(bullet->get_ID() == entity->get_ID())
            {
                bullets.erase( std::remove( bullets.begin(), bullets.end(), entity ), bullets.end() );
            }
        }        

        // Then send data
        std::stringstream sendData;
        sendData << "delete " << entity->get_ID() <<"\n";
        out_commands.push_back(command_struct(sendData.str(), mainFighter->get_socket()));
    }
}

void ServerLevel::on_send_message(const boost::system::error_code &err, size_t bytes)
{

}

bool ServerLevel::launch_bullet(glm::vec2 direction, Fighter* fighter)
{
    Bullet* ptr = new Bullet(physWorld, direction, fighter, generate_random_id());
    Core::instance().add_entity(ptr);
    bullets.push_back(ptr);
}

void ServerLevel::draw(std::shared_ptr<ShaderProgram> shader)
{
	glm::mat4 view;
	FRect rectangle = mainFighter->get_rectangle();
    view = glm::translate(view, glm::vec3(-(rectangle.x + rectangle.width / 2.0f) + screenX/2, -(rectangle.y + rectangle.height / 2.0f) + screenY/2, -3.0f));

    shader->set_mat4("view", view);

	Level::draw(shader);	
}

std::string ServerLevel::generate_random_id()
{
    std::mt19937 rng(rd());    
    std::uniform_int_distribution<int> uni(1,100000000); 

    int random_ID;

    while(true) 
    {
        random_ID = uni(rng); // Generate random ID

        // Check if ID already exist
        for (Fighter* fighter : players)
        {
            if(fighter->get_ID() == std::to_string(random_ID))
            {
                continue;
            }
        }

        for (Bullet* bullet : bullets)
        {
            if(bullet->get_ID() == std::to_string(random_ID))
            {
                continue;
            }
        }

        return std::to_string(random_ID);
    }    
}

void ServerLevel::delete_player(EmptyEntity* delete_entity)
{
    std::stringstream sendData;
    sendData << "delete " << dynamic_cast<Fighter*>(delete_entity)->get_ID() <<"\n";
    dynamic_cast<Fighter*>(delete_entity)->get_socket()->async_write_some(boost::asio::buffer(sendData.str()), boost::bind(&ServerLevel::on_send_message, this, _1, _2));
    players.erase( std::remove( players.begin(), players.end(), delete_entity ), players.end() );

    // Also delete socket and buffer
    auto it = std::find_if(clients.begin(), clients.end(),
             [delete_entity](Client * c) -> bool { return c->get_socket() == dynamic_cast<Fighter*>(delete_entity)->get_socket(); });
    clients.erase(it);
}

ServerLevel::~ServerLevel()
{
    for(auto &it : clients) 
        delete it;
    clients.clear();

    for(auto &it : players) 
        delete it;
    players.clear();

    for(auto &it : bullets) 
        delete it;
    bullets.clear();
}