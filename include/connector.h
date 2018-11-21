#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

#include "../include/peer.h"



class Connector : public boost::enable_shared_from_this<Connector>, boost::noncopyable
{
public:
    Connector(std::string login, boost::asio::io_context& context, boost::asio::ip::tcp::acceptor &acceptor);


    void handle_accept(Peer::ptr peer, const boost::system::error_code &err);

    void connect_to_room(std::string ip);

    void create_own_room();


private:    
    typedef boost::shared_ptr<Peer> peer_ptr;
    typedef std::vector<peer_ptr> array;

    void create_myself_peer(std::string login);
    void wait_new_peer();

    void launch_write_send_thread();

    void write_msg();
    void send_msg(std::string msg);
    void on_send_msg(const boost::system::error_code & err, size_t bytes);

    void on_connect_to_room(const boost::system::error_code &err);

    boost::asio::ip::tcp::acceptor& acceptor;
    boost::asio::io_context& context;
    peer_ptr myself_peer;
    array peers; // All peers
    bool ranAsync;

    boost::thread write_send_thread;
};

#endif
