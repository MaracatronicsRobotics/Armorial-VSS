#ifndef ROLE_STRIKER_H
#define ROLE_STRIKER_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class Role_Striker : public Role{
private:
    //Behaviours
    Behaviour_Attacker *_bh_at;
    Behaviour_DoNothing *_bh_dn;
    Behaviour_TakeFoul *_bh_tf;

    // Behaviours Enum
    enum{
        BHV_AT,
        BHV_DONOTHING,
        BHV_TAKEFOUL
    };
    // Inherited functions
    void configure();
    void run();

    //foul charge
    bool ourTeamShouldTake(VSSRef::Color teamColor);
    void penaltyKick(Position* pos, Angle* ang);
    void goalKick(Position* pos, Angle* ang);
    void gameOn();
    void freeBall(Position* pos, Angle* ang, VSSRef::Quadrant quadrant);
    void kickOff(Position* pos, Angle* ang);
    bool isReallyInsideTheirArea(quint8 id);

    int lastFoul; //it can be FREE_KICK = 0, PENALTY_KICK = 1, GOAL_KICK = 2, FREE_BALL = 3, KICKOFF = 4
    bool isNormalGame, canGoBackToNormalGame;
    bool weTake;
    long int counter;

public:
    Role_Striker();
    QString name();
    void initializeBehaviours();
public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_STRIKER_H
