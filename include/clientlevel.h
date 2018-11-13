#ifndef CLIENTLEVEL_H
#define CLIENTLEVEL_H

#include <string>

#include "gamelevel.h"

class b2World;

class ClientLevel : public GameLevel
{
public:
    ClientLevel(std::string map);
    ~ClientLevel() {}
    void init(b2World* physWorld) override;

private:
    virtual void send_data() override;
    virtual void handle_input_data() override;
};

#endif // CLIENTLEVEL_H