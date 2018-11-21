#ifndef SERVERLEVEL_H
#define SERVERLEVEL_H

#include <string>
#include "boost/asio.hpp"
#include <boost/enable_shared_from_this.hpp>

#include "gamelevel.h"
#include "fighter.h"
#include <list>

class b2World;

class ServerLevel : public GameLevel
{
public:
	using acc_ptr = std::unique_ptr<boost::asio::ip::tcp::acceptor>;

    ServerLevel(std::string map);
    ~ServerLevel() {}
    void init(std::shared_ptr<b2World> physWorld) override;

    virtual void input_handler(float dt) override;

private:
	virtual void send_data() override;
    virtual void handle_input_data() override;

    void wait_new_player();
    void handle_accept(boost::asio::ip::tcp::socket* player, const boost::system::error_code &err);

    void do_read(boost::asio::ip::tcp::socket* player);
    void on_read(boost::asio::ip::tcp::socket* player, const boost::system::error_code &err, size_t bytes);
    void read(boost::asio::ip::tcp::socket* player);

    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    std::shared_ptr<b2World> phWorld;

    acc_ptr acceptor;
    std::list<Fighter*> players;
    char buff[512];
};

#endif // SERVERLEVEL_H
