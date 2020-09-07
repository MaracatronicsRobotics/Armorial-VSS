#ifndef BEHAVIOUR_STAYBACK_H
#define BEHAVIOUR_STAYBACK_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>
#include <src/const/constants.h>

class Behaviour_StayBack : public Behaviour{
private:
    void configure();
    void run();

    //Machine State
    int _state;
    enum{
        STATE_STARTSPINNING,
        STATE_STAYBACK
    };

    //States
    enum{
        STATE_GOTO,
        STATE_ROTATE,
        STATE_SPIN
    };

    //Skills
    Skill_GoTo *_sk_goTo;
    Skill_Spin *_sk_spin;
    Skill_RotateTo *_sk_rotateTo;

    long int loopsInSameRegionWithBall, loopsInSameRegionWithOpp; //initialize with 0
    Position lastPlayerPosition, ballPos, playerPos;

public:
    Behaviour_StayBack();
    QString name();

    bool setSpinDirection();
    bool checkIfShouldSpin();
    Position projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea);
};

#endif // BEHAVIOUR_STAYBACK_H
