#ifndef CLIENT_LEVEL_H
#define CLIENT_LEVEL_H

#include <string>
#include <boost/asio.hpp>

#include "emptyEntity.h"
#include "level.h"

class b2World;

class ClientLevel : public Level
{
public:
    ClientLevel(std::string map, boost::asio::ip::tcp::socket* playerSocket);
    ~ClientLevel(); 

    void init(std::shared_ptr<b2World> physWorld) override;

    virtual void update(float dt) override;

    virtual void draw(std::shared_ptr<ShaderProgram> shader) override;

private:
    virtual void send_data() override;
    virtual void handle_input_data() override;

    void do_read();
    void on_read(const boost::system::error_code &err, size_t bytes);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    void on_send_message(const boost::system::error_code &err, size_t bytes);

    boost::asio::ip::tcp::socket* playerSocket;
    std::vector<command_struct> in_commands, out_commands;; // Command for handle

    float screenX, screenY;
    std::vector<EmptyEntity*> players;
    std::vector<EmptyEntity*> bullets;
    float cooldown;
    std::string map;
    std::shared_ptr<EmptyEntity> mainFighter;
    const float player_width = 80.0f;
    const float player_height = 84.0f;
    
    char buff[512];
};

#endif // CLIENT_LEVEL_H