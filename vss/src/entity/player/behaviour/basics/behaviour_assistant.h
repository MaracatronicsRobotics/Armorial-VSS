#ifndef BEHAVIOUR_ASSISTANT_H
#define BEHAVIOUR_ASSISTANT_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>

class Behaviour_Assistant : public Behaviour{
private:
    void configure();
    void run();

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

    //skill
    int _skill;
    enum{
        ROT,
        PUSH,
        GOTO,
        SPIN
    };

    bool isBehindBall(Position posPlayer, Position posObjective);
    bool setSpinDirection();
    bool canGoToBall();
    Position projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea);
    bool localIsLookingTo(const Position &pos, float error);
    bool allyInTheirArea();
    float errorAngleToSegment(Position pos1, Position pos2);
    bool reduceOffset;
    Position behindBall;
    float ballOffset;
    bool posPlayerInterceptTheirGoal(Position pos);

public:
    Behaviour_Assistant();
    QString name();
};

#endif // BEHAVIOUR_ASSISTANT_H
