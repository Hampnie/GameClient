#ifndef START_LEVEL_H
#define START_LEVEL_H

#include <memory>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include "level.h"
#include "GUI/button.h"
class ShaderProgram;
class b2World;

class StartLevel : public Level
{
public:
    StartLevel() {}
    ~StartLevel() {}
    void init(std::shared_ptr<b2World> physWorld) override;
    void draw(std::shared_ptr<ShaderProgram> shader) override;
    void update(float dt) override;

    void host_game();
    void connect_to_game();
    size_t read_complete(char * buf, const boost::system::error_code & err, size_t bytes);

    void on_connect(const boost::system::error_code &err);

private:
    std::unique_ptr<Button> hostButton;
    std::unique_ptr<Button> connectButton;
    std::unique_ptr<Button> exitButton;
};

#endif // START_LEVEL_H
