#include "clientlevel.h"
#include <Box2D/Box2D.h>
#include <sstream>
#include <stdlib.h>
#include <glm/gtc/matrix_transform.hpp>

#include "core.h"
#include "input.h"
#include "connector.h"
#include "shaderProgram.h"
#include "endgamelevel.h"

ClientLevel::ClientLevel(std::string map, boost::asio::ip::tcp::socket* playerSocket)
: 	playerSocket(playerSocket),
    map(map)
{
    glm::vec2 windowSize = Core::instance().get_window_size();
    screenX = windowSize.x;
    screenY = windowSize.y;
}

void ClientLevel::init(std::shared_ptr<b2World> physWorld)
{
    // Load map ========================================================================================
    float x, y, width, height;
    std::stringstream stream(map);

    while(stream)
    {
        stream >> x >> y >> width >> height;
        FRect position1{x, y, width, height};
        EmptyEntity* wall = new EmptyEntity(position1, EmptyEntity::texture_type::Wall, "Wall");
        add_entity(wall);
    }
    //==================================================================================================

    out_commands.push_back(command_struct("new\n", playerSocket));
    do_read();
}

void ClientLevel::handle_input_data()
{
    // Handle data from server-----------------------------------------
    for ( command_struct &command : in_commands)
    {
        std::stringstream stream(command.str);

        std::string str;
        std::string ID;
        float x, y, width, height;
        float angle;

        stream >> str; // Get command
        if(str == "start")
        {
            stream >> ID >> x >> y >> angle;
            FRect position{x, y, 80.0f, 84.0f};
            mainFighter = std::make_shared<EmptyEntity>(position, EmptyEntity::texture_type::Player, ID);
            add_entity(mainFighter.get());
            players.push_back(mainFighter.get());
        }
        else if(str == "move")
        {
            stream >> ID >> x >> y >> angle;
            for (EmptyEntity* fighter : players)
            {
                if(fighter->get_ID() == ID)
                {
                    // Set position and angle
                    fighter->set_position(x, y);
                    fighter->set_angle(angle);
                    continue;
                }
            }

            // If player not found
            // Create him
            FRect position{x, y, 80.0f, 84.0f};
            EmptyEntity* fighter = new EmptyEntity (position, EmptyEntity::texture_type::Player, ID);
            fighter->set_angle(angle);
            add_entity(fighter);
            players.push_back(fighter);
        } 
        else if( str == "bullet")
        {
            stream >> ID >> x >> y;
            bool isFound = false;
            for (EmptyEntity* bullet : bullets)
            {
                if(bullet->get_ID() == ID)
                {
                    // Set position
                    bullet->set_position(x, y);
                    isFound = true;
                }
            }
            if(isFound)
                continue;

            // If bullet not found
            // Create it
            FRect position{x, y, 10.0f, 10.0f};
            EmptyEntity* bullet = new EmptyEntity (position, EmptyEntity::texture_type::Bullet, ID);
            add_entity(bullet);
            bullets.push_back(bullet);
        }
        else if( str == "delete")
        {
            stream >> ID;
            for (auto it = bullets.begin(); it != bullets.end(); )
            {
                if((*it)->get_ID() == ID)
                {
                    it = bullets.erase( it );
                    Core::instance().delete_entity(static_cast<EmptyEntity*>((*it)));
                } else it ++;
            }
            for (auto it = players.begin(); it != players.end(); )
            {
                if((*it)->get_ID() == ID)
                {
                    // If it is you
                    if(ID == mainFighter->get_ID())
                    {
                        // Create End game level
                        EndGameLevel *ptr = new EndGameLevel();
                        Core::instance().install_level(ptr);
                    }
                    Core::instance().delete_entity(static_cast<EmptyEntity*>((*it)));
                    it = players.erase( it );                    
                } else it ++;
            }
        }   
        else 
        {
            std::cout << "Undefined command: " << str << "\n";
        }              
    }
    in_commands.clear();
}

void ClientLevel::send_data()
{
    for ( command_struct &command : out_commands)
    {
        command.playerSocket->async_write_some(boost::asio::buffer(command.str), boost::bind(&ClientLevel::on_send_message, this, _1, _2));
    }
    out_commands.clear();   
}

void ClientLevel::update(float dt) 
{
    Level::update(dt);
    // Send key to server
    if(Input::keyPressed(SDLK_w))
    {
        std::stringstream sendData;
        sendData << "keyPress w " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket));  
    }
    if(Input::keyPressed(SDLK_s))
    {        
        std::stringstream sendData;
        sendData << "keyPress s " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket)); 
    }
    if(Input::keyPressed(SDLK_a))
    {
        std::stringstream sendData;
        sendData << "keyPress a " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket)); 
    }
    if(Input::keyPressed(SDLK_d))
    {
        std::stringstream sendData;
        sendData << "keyPress d " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket)); 
    }
    if(Input::keyReleased(SDLK_w))
    {
        std::stringstream sendData;
        sendData << "keyRelease w " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket)); 
    }
    if(Input::keyReleased(SDLK_s))
    {        
        std::stringstream sendData;
        sendData << "keyRelease s "<< mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket));
    }
    if(Input::keyReleased(SDLK_a))
    {
        std::stringstream sendData;
        sendData << "keyRelease a " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket));
    }
    if(Input::keyReleased(SDLK_d))
    {
        std::stringstream sendData;
        sendData << "keyRelease d " << mainFighter->get_ID() << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket));
    }
    if(Input::buttonPressed(SDL_BUTTON_LEFT) && cooldown >= 1.0f)
    {
        glm::vec2 mousePos = Input::mousePosition();
        std::stringstream sendData;
        sendData << "bullet " << mainFighter->get_ID()<< " " << (mousePos.x - screenX/2) << " " << (mousePos.y - screenY/2) << "\n";
        out_commands.push_back(command_struct(sendData.str(), playerSocket));    
        cooldown = 0.0f;  
    }	

    if(mainFighter)
    {
        // Calculate and send angle
        glm::vec2 mousePos = Input::mousePosition();
        glm::vec2 playerCenter{screenX/2, screenY/2};
        float newAngle = atan2(playerCenter.y - mousePos.y, playerCenter.x - mousePos.x);

        std::stringstream angleData;
        angleData << "angle " << mainFighter->get_ID() << " " << newAngle << "\n";
        out_commands.push_back(command_struct(angleData.str(), playerSocket));
    }
    
    cooldown += dt;
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
    in_commands.push_back(command_struct(buff, playerSocket));
    do_read();
}

void ClientLevel::do_read()
{
    //playerSocket->async_read_some(boost::asio::buffer(buff), boost::bind(&ClientLevel::on_read, this, buff, _1, _2));

    boost::asio::async_read(*playerSocket, boost::asio::buffer(buff),
                          boost::bind(&ClientLevel::read_complete, this, _1, _2),
                           boost::bind(&ClientLevel::on_read, this, _1, _2));
}

void ClientLevel::on_send_message(const boost::system::error_code &err, size_t bytes)
{

}

void ClientLevel::draw(std::shared_ptr<ShaderProgram> shader)
{
    if(mainFighter)
    {
        glm::mat4 view;
        FRect rectangle = mainFighter->get_rectangle();
        view = glm::translate(view, glm::vec3(-(rectangle.x + rectangle.width / 2.0f) + screenX/2, -(rectangle.y + rectangle.height / 2.0f) + screenY/2, -3.0f));

        shader->set_mat4("view", view);
    }    

    Level::draw(shader);    
}

ClientLevel::~ClientLevel()
{
    for(auto &it : players) 
        delete it;
    players.clear();

    for(auto &it : bullets) 
        delete it;
    bullets.clear();

    if(playerSocket)
        delete playerSocket;
}