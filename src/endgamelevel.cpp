#include "endgamelevel.h"
#include "core.h"
#include "startlevel.h"
#include "shaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>

void EndGameLevel::init(std::shared_ptr<b2World> physWorld)
{
    Level::init(physWorld);

    glm::vec2 windowSize = Core::instance().get_window_size();

    FRect position{windowSize.x / 2 - 150.0f, windowSize.y / 2 - 65.5f, 300.0f, 131.0f};
    returnButton = std::unique_ptr<Button>(new Button(position, "textures/Return.png", "textures/Return_press.png", "textures/Return_hover.png"));

    auto returnFunc = []() {
        StartLevel *startLevel = new StartLevel();
        Core::instance().install_level(startLevel);
             };

    returnButton->OnPressed.connect(returnFunc);

    
}

void EndGameLevel::update(float dt)
{
    returnButton->update(dt);
}

void EndGameLevel::draw(std::shared_ptr<ShaderProgram> shader)
{
    glm::mat4 view;
    view = glm::translate(view, glm::vec3(0, 0, -3.0f));

    shader->set_mat4("view", view);
    returnButton->draw(shader);
}