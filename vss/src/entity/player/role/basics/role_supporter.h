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
    Behaviour_PushBall *_bh_pb;
    Behaviour_TakeFoul *_bh_tf;

    // Behaviours ids!
    enum{
        BHV_ASSISTANT,
        BHV_BARRIER,
        BHV_DONOTHING,
        BHV_PUSH,
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

    bool EnemyNear(float minDist);
    float distAllyToBall();
    int _positioning;
    int _bhv;
    bool BySideOfGoal();
    bool ourTeamShouldTake(VSSRef::Color teamColor);
    void penaltyKick(Position* pos, Angle* ang);
    void goalKick(Position* pos, Angle* ang);
    void gameOn();
    void freeBall(Position* pos, Angle* ang, VSSRef::Quadrant quadrant);
    void kickOff(Position* pos, Angle* ang, bool isFirstKickOff);
    bool isReallyInsideTheirArea(quint8 id);
    void isStuck();

    int lastFoul; //it can be FREE_KICK = 0, PENALTY_KICK = 1, GOAL_KICK = 2, FREE_BALL = 3, KICKOFF = 4
    float _distToGK;
    bool isNormalGame, canGoBackToNormalGame;
    bool weTake;
    bool _getOut;
    quint8 goalkeeperId;
    Timer timer, timerToChangeBhv, timerGetOut;

public:
    Role_Supporter();
    void initializeBehaviours();
    QString name();
    void setPositioning(int positioning);

signals:
    void refuted(quint8 playerId);
    void sendSignal();

public slots:
    void gettingOut(quint8 playerId, quint8 GK_ID);
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_SUPPORTER_H
