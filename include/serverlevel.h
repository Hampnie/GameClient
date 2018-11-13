#ifndef SERVERLEVEL_H
#define SERVERLEVEL_H

#include <string>

#include "gamelevel.h"

class b2World;

class ServerLevel : public GameLevel
{
public:
    ServerLevel(std::string map);
    ~ServerLevel() {}
    void init(b2World* physWorld) override;

private:
	virtual void send_data() override;
    virtual void handle_input_data() override;
};

#endif // SERVERLEVEL_H
