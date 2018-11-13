#ifndef STARTLEVEL_H
#define STARTLEVEL_H

#include <memory>
#include <iostream>
#include "level.h"
#include "GUI/button.h"
class ShaderProgram;
class b2World;

class StartLevel : public Level
{
public:
    StartLevel() {}
    ~StartLevel() {std::cout << "Dctor\n";}
    void init(b2World* physWorld) override;
    void draw(ShaderProgram *shader) override;
    void update(float dt) override;
    void release() override {}

    void host_game();
    void connect_to_game();

private:
    std::unique_ptr<Button> hostButton;
    std::unique_ptr<Button> connectButton;
    std::unique_ptr<Button> exitButton;
};

#endif // STARTLEVEL_H
