#ifndef ROLE_STRIKER_H
#define ROLE_STRIKER_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class Role_Striker : public Role{
private:
    //Behaviours
    Behaviour_Attacker *_bh_at;

    // Behaviours Enum
    enum{
        BHV_AT
    };
    // Inherited functions
    void configure();
    void run();
public:
    Role_Striker();
    QString name();
    void initializeBehaviours();
};

#endif // ROLE_STRIKER_H
