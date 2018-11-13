#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

#include "../include/connector.h"

int main(int argc, char* argv[])
{
    if(!std::strcmp(argv[0], "host") || !std::strcmp(argv[0], "connect"))
    {
        std::cerr << "Usage: host or connect" << std::endl;
        return 1;
    }
    std::string str;
    std::cout << "Enter your login: ";
    std::getline(std::cin, str);


    boost::asio::io_context context;

 // Handle all connection

    // Host own room
    if(std::strcmp(argv[1], "host") == 0)
    {
        boost::asio::ip::tcp::acceptor acceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10058));

        Connector connector(str, context, acceptor);
        std::cout << "Create own room\n";
        connector.create_own_room();
    }
    // Connect to room
    else if (std::strcmp(argv[1], "connect") == 0)
    {
        boost::asio::ip::tcp::acceptor acceptor(context);

        Connector connector(str, context, acceptor);
        std::cout << "Connect to room\n";
        connector.connect_to_room(argv[2]);
    }

    return 0;
}
