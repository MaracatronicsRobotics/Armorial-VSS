#ifndef BEHAVIOUR_PUSHBALL_H
#define BEHAVIOUR_PUSHBALL_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>

class Behaviour_PushBall : public Behaviour{
private:
    void configure();
    void run();

    bool _isBehindball, _isLookingToBall, _pushed;

    //States
    enum{
        STATE_GOTOPOS,
        STATE_ROTATE,
        STATE_PUSH
    };

    //Skills
    Skill_RotateTo *_sk_rotateTo;
    Skill_GoTo *_sk_goTo;
    Skill_PushBall *_sk_pushBall;

public:
    Behaviour_PushBall();
    QString name();
};

#endif // BEHAVIOUR_PUSHBALL_H
