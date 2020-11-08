#include "behaviour_takefoul.h"

#define GKPREVISION_MINVELOCITY 0.02f
#define GKPREVISION_VELOCITY_FACTOR 3.0f
#define GKPREVISION_FACTOR_LIMIT 0.15f

QString Behaviour_TakeFoul::name(){
    return "Behaviour_TakeFoul";
}

Behaviour_TakeFoul::Behaviour_TakeFoul(){
}

void Behaviour_TakeFoul::configure(){
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_pushBall = new Skill_PushBall());

    //initial skill
    setInitialSkill(_sk_pushBall);
    _skill = PUSH;

    //transitions
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_ROTATE, _sk_pushBall, _sk_rotateTo);
    addTransition(STATE_PUSH, _sk_rotateTo, _sk_pushBall);
    addTransition(STATE_PUSH, _sk_goTo, _sk_pushBall);
    addTransition(STATE_GOTOPOS, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTOPOS, _sk_pushBall, _sk_goTo);
}

void Behaviour_TakeFoul::run(){

    if(player()->isLookingTo(loc()->ball(), float(M_PI_2))) _sk_pushBall->setSpeedAndOmega(2.0, 0.0);
    else _sk_pushBall->setSpeedAndOmega(-2.0, 0.0);

}

bool Behaviour_TakeFoul::localIsLookingTo(const Position &pos, float error){
    Angle angle1(true, WR::Utils::getAngle(player()->position(), pos));

    // Calc diff
    float dif = abs(WR::Utils::angleDiff(player()->orientation(), angle1));
    if(dif > Angle::pi/2) dif = abs(Angle::pi - dif);
    return (dif <= error);
}

