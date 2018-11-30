#ifndef END_GAME_LEVEL_H
#define END_GAME_LEVEL_H


#include "level.h"
#include "GUI/button.h"

class ShaderProgram;
class b2World;

class EndGameLevel : public Level
{
public:
    EndGameLevel() {}
    ~EndGameLevel() {}
    void init(std::shared_ptr<b2World> physWorld) override;
    void draw(std::shared_ptr<ShaderProgram> shader) override;
    void update(float dt) override;

private:
    std::unique_ptr<Button> returnButton;
};

#endif // END_GAME_LEVEL_H