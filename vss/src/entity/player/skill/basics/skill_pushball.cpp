#include "skill_pushball.h"

Skill_PushBall::Skill_PushBall(){
    _state = STATE_MOVETOPOS;
}

QString Skill_PushBall::name(){
    return "Skill_PushBall";
}

void Skill_PushBall::run(){
    Position const ball = loc()->ball();
    Position const ourGoal = loc()->ourGoal();

    switch (_state) {
        case STATE_MOVETOPOS:{
            Position const behindBall = WR::Utils::threePoints(ball, ourGoal, 0.2f, GEARSystem::Angle::pi);
            player()->goTo(behindBall);

            if(player()->isNearbyPosition(behindBall, 0.03f))  _state = STATE_ROTATE;
            break;
        }

        case STATE_ROTATE:{
            player()->rotateTo(ball);

            if(player()->isLookingTo(ball)) _state = STATE_PUSH;
            break;
        }

        case STATE_PUSH:{
            player()->setSpeed(2.0, 0.0);

            break;
        }
    }
}
