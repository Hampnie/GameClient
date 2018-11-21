#ifndef PEER_H
#define PEER_H

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class Peer : public boost::enable_shared_from_this<Peer>, boost::noncopyable
{
public:
    typedef boost::shared_ptr<Peer> ptr;
    typedef boost::system::error_code error_code;

    static ptr create_peer(boost::asio::io_context *context, std::string ID);

    void start();
    void stop();

    boost::asio::ip::tcp::socket & get_socket()
    {
        return socket;
    }

    std::string get_username() const
    {
        return username;
    }


private:
    Peer(boost::asio::io_context *context, std::string ID);

    void do_read();
    void on_read(const error_code & err, size_t bytes);

    size_t read_complete(const boost::system::error_code & err, size_t bytes);

    std::string username;
    boost::asio::ip::tcp::socket socket;
    enum {max_msg = 1024};
    char read_buffer[max_msg];
    char write_buffer[max_msg];
    bool started;
};

#endif
