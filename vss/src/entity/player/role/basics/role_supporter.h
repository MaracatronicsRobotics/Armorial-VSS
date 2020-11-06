#ifndef ROLE_SUPPORTER_H
#define ROLE_SUPPORTER_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class PosAng{
public:
    Position pos;
    Angle ang;
};

class Role_Supporter : public Role
{
    Q_OBJECT
private:
    // Behaviours

    Behaviour_Assistant *_bh_as;
    Behaviour_Barrier *_bh_br;
    Behaviour_DoNothing *_bh_dn;
    Behaviour_TakeFoul *_bh_tf;

    // Behaviours ids!
    enum{
        BHV_ASSISTANT,
        BHV_BARRIER,
        BHV_DONOTHING,
        BHV_TAKEFOUL
    };

    // positioning
    enum{
        BARRIER_PREDOMINANT,
        ASSIST_PREDOMINANT,
        FREE
    };

    // Inherited functions
    void configure();
    void run();

    bool EnemyInOurField();
    int _positioning;
    int _bhv;
    bool BySideOfGoal();
    bool ourTeamShouldTake(VSSRef::Color teamColor);
    void penaltyKick(Position* pos, Angle* ang);
    void goalKick(Position* pos, Angle* ang);
    void gameOn();
    void freeBall(Position* pos, Angle* ang, VSSRef::Quadrant quadrant);

    int lastFoul; //it can be FREE_KICK = 0, PENALTY_KICK = 1, GOAL_KICK = 2, FREE_BALL = 3, KICKOFF = 4
    bool isNormalGame, canGoBackToNormalGame;
    bool weTake;
    long int counter;

public:
    Role_Supporter();
    void initializeBehaviours();
    QString name();
    void setPositioning(int positioning);

signals:
    void sendSignal();

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_SUPPORTER_H
