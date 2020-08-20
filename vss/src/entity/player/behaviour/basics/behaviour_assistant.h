#ifndef BEHAVIOUR_ASSISTANT_H
#define BEHAVIOUR_ASSISTANT_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>
#include <src/const/constants.h>

class Behaviour_Assistant : public Behaviour{
private:
    void configure();
    void run();

    //Machine State
    int _state;
    enum{
        STATE_STAYBACK,
        STATE_GOTOBALL,
        STATE_STARTSPINNING
    };

    //States
    enum{
        STATE_GOTO,
        STATE_ROTATE,
        STATE_SPIN,
        STATE_PUSH
    };

    //Skills
    Skill_GoTo *_sk_goTo;
    Skill_Spin *_sk_spin;
    Skill_RotateTo *_sk_rotateTo;
    Skill_PushBall *_sk_pushBall;

    int loopsInSameRegionWithBall, loopsInSameRegionWithOpp; //initialize with 0
    Position lastPlayerPosition, ballPos, playerPos;

public:
    Behaviour_Assistant();
    QString name();

    bool isBehindBall(Position posObjective);
    bool setSpinDirection();
    bool checkIfShouldSpin();
};

#endif // BEHAVIOUR_ASSISTANT_H
