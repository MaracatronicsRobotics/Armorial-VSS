#ifndef BEHAVIOUR_BARRIER_H
#define BEHAVIOUR_BARRIER_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>

class Behaviour_Barrier : public Behaviour
{
private:
    void configure();
    void run();
    int _state;

    // param
    float _d;
    float _radius;

    //skills

    Skill_GoTo *_sk_goto;
    Skill_InterceptBall *_sk_intercept;
    Skill_Spin *_sk_spin;

    enum{
        STATE_GOTO ,
        STATE_INTERCEPT ,
        STATE_SPIN
    };

    bool isBallComingToGoal( float minSpeed , float postsFactor = 1.0);

public:
    Behaviour_Barrier();
    QString name();

    void setD(float d) {_d = d ;}
    void setRadius(float radius) {_radius = radius;}
    Position projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea);
};

#endif // BEHAVIOUR_BARRIER_H
