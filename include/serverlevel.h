#ifndef SERVER_LEVEL_H
#define SERVER_LEVEL_H

#include <string>
#include "boost/asio.hpp"
#include <boost/enable_shared_from_this.hpp>

#include "level.h"
#include "fighter.h"
#include "bullet.h"
#include <list>
#include <vector>
#include <random>

class b2World;
class ShaderProgram;

class ServerLevel : public Level
{
public:
	using acc_ptr = std::unique_ptr<boost::asio::ip::tcp::acceptor>;

    ServerLevel(std::string map);
    virtual ~ServerLevel();

    virtual void init(std::shared_ptr<b2World> physWorld) override;
    virtual void update(float dt) override;
    virtual void draw(std::shared_ptr<ShaderProgram> shader) override;

private:
    // Nested class
    //===================================================================================
    class Client
    {
    public:
        Client()
        {
            socket = new boost::asio::ip::tcp::socket(*Core::instance().get_context());
        }
        ~Client()
        {
            delete socket;
        }

        boost::asio::ip::tcp::socket* get_socket(){return socket;}
        auto get_buffer() -> char (&)[512]
        {
            return buff;
        }
    private:
        boost::asio::ip::tcp::socket* socket;
        char buff[512];
    };
    //====================================================================================

	virtual void send_data() override;
    virtual void handle_input_data() override;

    void wait_new_player();
    void handle_accept(Client *client, const boost::system::error_code &err);

    void do_read(Client *client);
    void on_read(Client *client, const boost::system::error_code &err, size_t bytes);

    size_t read_complete(Client *client, const boost::system::error_code & err, size_t bytes);
    void on_send_message(const boost::system::error_code &err, size_t bytes);

    void delete_player(EmptyEntity* delete_entity);

    bool launch_bullet(glm::vec2 direction, Fighter* fighter);

    std::shared_ptr<b2World> phWorld;

    float screenX, screenY;
    float cooldown;
    std::string map;
    std::vector<command_struct> in_commands, out_commands; // Command for handle
    acc_ptr acceptor;

    std::list<Client*> clients;
    std::vector<Fighter*> players;
    std::vector<Bullet*> bullets;
    std::shared_ptr<Fighter> mainFighter;

    // Random generator
    std::random_device rd;

    std::string generate_random_id();    
};

#endif // SERVERLEVEL_H
