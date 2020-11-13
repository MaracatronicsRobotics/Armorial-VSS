#ifndef BEHAVIOUR_TAKEFOUL_H
#define BEHAVIOUR_TAKEFOUL_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>
#include <src/utils/freeangles/freeangles.h>
#include <src/const/constants.h>

class Behaviour_TakeFoul : public Behaviour{
private:
    void configure();
    void run();

    int _skill;
    bool localIsLookingTo(const Position &pos, float error);
    bool setSpinDirection();

    //Skills
    enum{
        GOTO,
        ROT,
        PUSH
    };

    //States
    enum{
        STATE_GOTOPOS,
        STATE_ROTATE,
        STATE_PUSH,
        STATE_SPIN
    };

    //Skills
    Skill_RotateTo *_sk_rotateTo;
    Skill_GoTo *_sk_goTo;
    Skill_PushBall *_sk_pushBall;
    Skill_Spin *_sk_spin;

    // Parameters
    bool _isPK;

public:
    Behaviour_TakeFoul();
    QString name();

    void isPK(bool isPK) { _isPK = isPK; }
};


#endif // BEHAVIOUR_TAKEFOUL_H
