#include <boost/bind.hpp>
#include <iostream>


#include "../include/connector.h"

Connector::Connector(std::string login, boost::asio::io_context& context, boost::asio::ip::tcp::acceptor& acceptor)
    : context(context),
      acceptor(acceptor),
      ranAsync(false)
{
    create_myself_peer(login);
}


void Connector::wait_new_peer()
{
    std::cout << "Waiting connection\n";
    Peer::ptr peer = Peer::create_peer(&context, "1337");
    acceptor.async_accept(peer->get_socket(), boost::bind(&Connector::handle_accept, this, peer, _1));
}

void Connector::handle_accept(Peer::ptr peer, const boost::system::error_code &err)
{
    std::cout << "Smbd connected\n";
    //peer->start();
    //peers.push_back(peer);
    //wait_new_peer();
}

void Connector::create_myself_peer(std::string login)
{
    myself_peer = Peer::create_peer(&context, login);
    if(myself_peer) ;
    std::cout << "Create myselfpeer \n";
}

void Connector::on_connect_to_room(const boost::system::error_code& err)
{
    std::cout << err;
    //launch_write_send_thread();
    std::cout << "In room \n";
    /*
    if (!err)
    {
        send_msg("login " + myself_peer->get_username()); // Request all peers, and then connect to them
        // Connect
    }
    else // Stop connection
        ;
        */
}

void Connector::connect_to_room(std::string ip)
{
    std::cout << "Connect to: " << ip << "\n";
    acceptor.close();
    boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string("127.0.0.1"), 10058);
    //myself_peer->get_socket().async_connect(ep, boost::bind(&Connector::on_connect_to_room, this, _1));
    myself_peer->get_socket().connect(ep);
    //launch_write_send_thread();
    std::cout << "Connecting to room..." << std::endl;
}


void Connector::create_own_room()
{

    //launch_write_send_thread();
    wait_new_peer();
}

void Connector::launch_write_send_thread()
{
    std::cout << "Launch write thread\n";
    write_send_thread = boost::thread(boost::bind(&Connector::write_msg, this));
}


void Connector::send_msg(std::string msg)
{
    //for(array::iterator b = peers.begin(), e = peers.end(); b != e; ++b)
        //(*b)->get_socket().async_write_some(boost::asio::buffer(msg, msg.size()), boost::bind(&Connector::on_send_msg, shared_from_this(), _1, _2));
    std::cout << "Hampnie: " << msg << std::endl;
    write_msg();
}
/*
void Connector::on_send_msg(const boost::system::error_code & err, size_t bytes)
{
     context.post(boost::bind(&Connector::write_msg, shared_from_this()) );
}
*/
void Connector::write_msg()
{
    std::string request;
    std::getline(std::cin, request);

    send_msg(request + "\n");
}

