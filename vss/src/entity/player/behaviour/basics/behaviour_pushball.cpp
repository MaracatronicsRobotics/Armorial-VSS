#include "behaviour_pushball.h"

Behaviour_PushBall::Behaviour_PushBall(){

}

QString Behaviour_PushBall::name(){
    return "Behaviour_PushBall";
}

void Behaviour_PushBall::configure(){
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_pushBall = new Skill_PushBall());

    //initial skill
    setInitialSkill(_sk_goTo);

    //transitions
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_PUSH, _sk_rotateTo, _sk_pushBall);
}

void Behaviour_PushBall::run(){
    const Position ball = loc()->ball();
    const Position theirGoal = loc()->theirGoal();
    const Position behindBall = WR::Utils::threePoints(ball, theirGoal, 0.2f, GEARSystem::Angle::pi);

    //setting skill goto
    _sk_goTo->setGoToPos(behindBall);

    //setting skill rotateto
    _sk_rotateTo->setDesiredPosition(ball);

    //setting skill push
    _sk_pushBall->setSpeedAndOmega(1.0, 0.0);

    //transitions
    if(player()->isLookingTo(ball)){
        enableTransition(STATE_PUSH);
    } else if(player()->isNearbyPosition(behindBall, 0.03f)){
        enableTransition(STATE_ROTATE);
    }
}
