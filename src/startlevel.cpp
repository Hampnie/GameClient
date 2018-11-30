#include "serverlevel.h"
#include "common.h"
#include "core.h"
#include "startlevel.h"
#include "clientlevel.h"

#include "tinyfiledialogs.h"

#include <Box2D/Box2D.h>
#include <boost/bind.hpp>
#include <string.h>
#include <sstream>
#include <fstream>

void StartLevel::init(std::shared_ptr<b2World> physWorld)
{
    Level::init(physWorld);

    glm::vec2 windowSize = Core::instance().get_window_size();

    FRect position{windowSize.x / 2 - 150.0f, windowSize.y * 1 / 5 - 65.5f , 300.0f, 131.0f};
    hostButton = std::unique_ptr<Button>(new Button(position, "textures/Host.png", "textures/Host_press.png", "textures/Host_hover.png"));

    hostButton->OnPressed.connect(boost::bind(&StartLevel::host_game, this));


    FRect position2{windowSize.x / 2 - 150.0f, windowSize.y * 4/ 5 - 65.5f, 300.0f, 131.0f};
    exitButton = std::unique_ptr<Button>(new Button(position2, "textures/Exit.png", "textures/Exit_press.png", "textures/Exit_hover.png"));
    auto exitFunc = []() {
        Core::instance().close_game();
             };

    exitButton->OnPressed.connect(exitFunc);


    FRect position3{windowSize.x / 2 - 150.0f, windowSize.y * 2.5 / 5 - 65.5f, 300.0f, 131.0f};
    connectButton = std::unique_ptr<Button>(new Button(position3, "textures/Connect.png", "textures/Connect_press.png", "textures/Connect_hover.png"));

    connectButton->OnPressed.connect(boost::bind(&StartLevel::connect_to_game, this));

}

void StartLevel::update(float dt)
{
    hostButton->update(dt);
    connectButton->update(dt);
    exitButton->update(dt);

}

void StartLevel::draw(std::shared_ptr<ShaderProgram> shader)
{
    hostButton->draw(shader);
    connectButton->draw(shader);
    exitButton->draw(shader);
}

void StartLevel::host_game()
{
	char const * lTheOpenFileName;
	char const * lFilterPatterns[2] = { "*.txt", "*.text" };
    lTheOpenFileName = tinyfd_openFileDialog(
		"level file",
		"",
		2,
		lFilterPatterns,
		NULL,
		0);

    // Load string from file
    std::ifstream levelFile(lTheOpenFileName);
    std::stringstream buffer;
    buffer << levelFile.rdbuf();

    ServerLevel *ptr = new ServerLevel(buffer.str());
    Core::instance().install_level(ptr);
}

void StartLevel::connect_to_game()
{

	char const * lTmp;
	lTmp = tinyfd_inputBox(
		"IP", "Enter an IP adress", " ");

	// Connect to server
    std::cout << "Connect to: " << lTmp << "\n";
    boost::asio::ip::tcp::endpoint ep( boost::asio::ip::address::from_string(lTmp), 10058);
    boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(*Core::instance().get_context());
    
    try {

        socket->connect(ep);
    } catch (boost::system::system_error const& e)
    {
        std::cout << "Warning: could not connect : " << e.what() << std::endl;
        exit(1);
    } 
    // Get map from server
    char buf[1024];
	socket->read_some(boost::asio::buffer(buf));


    ClientLevel *ptr = new ClientLevel(buf, socket);
    Core::instance().install_level(ptr);
}

void StartLevel::on_connect(const boost::system::error_code &err)
{
    std::cout << "On connect \n";
}

size_t StartLevel::read_complete(char * buf, const boost::system::error_code & err, size_t bytes) 
{
	if ( err) return 0;
	bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
	// we read one-by-one until we get to enter, no buffering
	return found ? 0 : 1;
}
