#ifndef ROLE_DEFENDER_H
#define ROLE_DEFENDER_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class Role_Defender : public Role
{
private:
    // Behaviours

    Behaviour_Assistant *_bh_as;
    Behaviour_Barrier *_bh_br;
    Behaviour_StayBack *_bh_sb;

    // Behaviours ids!
    enum{
        BHV_STAYBACK,
        BHV_ASSISTANT,
        BHV_BARRIER
    };

    // Inherited functions
    void configure();
    void run();

public:
    Role_Defender();
    void initializeBehaviours();
    QString name();

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_SUPPORTER_H
