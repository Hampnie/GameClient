#include "serverlevel.h"
#include "common.h"
#include "core.h"
#include "startlevel.h"

#include "tinyfiledialogs.h"

#include <Box2D/Box2D.h>
#include <boost/bind.hpp>
#include <string.h>
#include <sstream>
#include <fstream>

void StartLevel::init(b2World *physWorld)
{
    Level::init(physWorld);

    glm::vec2 windowSize = Core::instance().getWindowSize();

    FRect position{windowSize.x / 2 - 150.0f, windowSize.y * 1 / 5 - 65.5f , 300.0f, 131.0f};
    hostButton = std::unique_ptr<Button>(new Button(position, "textures/Start.png", "textures/Start_press.png", "textures/Start_hover.png"));

    hostButton->OnPressed.connect(boost::bind(&StartLevel::host_game, this));


    FRect position2{windowSize.x / 2 - 150.0f, windowSize.y * 4/ 5 - 65.5f, 300.0f, 131.0f};
    exitButton = std::unique_ptr<Button>(new Button(position2, "textures/Exit.png", "textures/Exit_press.png", "textures/Exit_hover.png"));
    auto exitFunc = []() {
        Core::instance().closeGame();
             };

    exitButton->OnPressed.connect(exitFunc);


    FRect position3{windowSize.x / 2 - 150.0f, windowSize.y * 2.5 / 5 - 65.5f, 300.0f, 131.0f};
    connectButton = std::unique_ptr<Button>(new Button(position3, "textures/Exit.png", "textures/Exit_press.png", "textures/Exit_hover.png"));

    connectButton->OnPressed.connect(boost::bind(&StartLevel::connect_to_game, this));

}

void StartLevel::update(float dt)
{
    hostButton->update(dt);
    connectButton->update(dt);
    exitButton->update(dt);

}

void StartLevel::draw(ShaderProgram *shader)
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
    Core::instance().installLevel(ptr);
}

void StartLevel::connect_to_game()
{
	char const * lTmp;
	lTmp = tinyfd_inputBox(
		"IP", "Enter an IP adress", NULL);

	// Connect to server
}