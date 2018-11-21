#ifndef CLIENTLEVEL_H
#define CLIENTLEVEL_H

#include <string>
#include <boost/asio.hpp>

#include "gamelevel.h"

class b2World;

class ClientLevel : public GameLevel
{
public:
    ClientLevel(std::string map, boost::asio::ip::tcp::socket* playerSocket);
    ~ClientLevel() {}
    void init(std::shared_ptr<b2World> physWorld) override;

    virtual void input_handler(float dt) override;

private:
    virtual void send_data() override;
    virtual void handle_input_data() override;

    void do_read();
    void on_read(const boost::system::error_code &err, size_t bytes);
    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    boost::asio::ip::tcp::socket* playerSocket;
    char buff[512];
};

#endif // CLIENTLEVEL_H