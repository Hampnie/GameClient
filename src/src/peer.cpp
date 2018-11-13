#include <iostream>
#include <boost/bind.hpp>
#include "../include/peer.h"

Peer::Peer(boost::asio::io_context *context) :
    socket(*context),
    started(false),
    username(username)
{

}

Peer::ptr Peer::create_peer(boost::asio::io_context *context)
{
    ptr peer(new Peer(context));
    return peer;
}

void Peer::start()
{
    started = true;
    do_read();
}

void Peer::stop()
{

}

void Peer::do_read()
{
    boost::asio::async_read(socket, boost::asio::buffer(read_buffer),
                            boost::bind(&Peer::read_complete, shared_from_this(),  _1, _2),
                            boost::bind(&Peer::on_read, shared_from_this(), _1, _2));
}

void Peer::on_read(const error_code & err, size_t bytes)
{
    std::string msg(read_buffer, bytes);
    std::cout << msg << std::endl;
    do_read();
}

size_t Peer::read_complete(const boost::system::error_code & err, size_t bytes) {
        if ( err) return 0;
        bool found = std::find(read_buffer, read_buffer + bytes, '\n') < read_buffer + bytes;
        // we read one-by-one until we get to enter, no buffering
        return found ? 0 : 1;
}
