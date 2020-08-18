#include "behaviour_assistant.h"

#define BALLPREVISION_MINVELOCITY 0.02f
#define BALLPREVISION_VELOCITY_FACTOR 3.0f
#define BALLPREVISION_FACTOR_LIMIT 0.15f

QString Behaviour_Assistant::name() {
    return "Behaviour_Assistant";
}

Behaviour_Assistant::Behaviour_Assistant() {

}

void Behaviour_Assistant::configure() {
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_spin = new Skill_Spin());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_pushBall = new Skill_PushBall());

    //initial state
    _state = STATE_STAYBACK;

    //initial skill
    setInitialSkill(_sk_goTo);

    //transitions
    //goTo
    addTransition(STATE_GOTO, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTO, _sk_spin, _sk_goTo);
    addTransition(STATE_GOTO, _sk_pushBall, _sk_goTo);
    //spin
    addTransition(STATE_SPIN, _sk_rotateTo, _sk_spin);
    addTransition(STATE_SPIN, _sk_goTo, _sk_spin);
    addTransition(STATE_SPIN, _sk_pushBall, _sk_spin);
    //rotateTo
    addTransition(STATE_ROTATE, _sk_spin, _sk_rotateTo);
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_ROTATE, _sk_pushBall, _sk_rotateTo);
    //pushBall
    addTransition(STATE_PUSH, _sk_spin, _sk_pushBall);
    addTransition(STATE_PUSH, _sk_goTo, _sk_pushBall);
    addTransition(STATE_PUSH, _sk_rotateTo, _sk_pushBall);
};

void Behaviour_Assistant::run() {

    switch(_state) {

    case STATE_STAYBACK:{
        //setting skill rotateTo
        _sk_rotateTo->setDesiredPosition(loc()->ball());

        //setting skill goTo
        Position desiredPosition(false, 0, 0, 0);

        //if ball is in their field or if our team has ball poss but it isn't with this player
        if(loc()->isInsideTheirField(loc()->ball()) || (player()->team()->hasBallPossession() && !player()->hasBallPossession())){
            if(loc()->ourSide().isRight()){
                /*project ball position on the line that divides
                 * our field in the middle (x = 0.375 or x = -0.375)*/
                Position upperPoint(true, loc()->fieldMaxX() - (loc()->fieldLength()/4), loc()->fieldMaxY(), 0);
                Position lowerPoint(true, loc()->fieldMaxX() - (loc()->fieldLength()/4), loc()->fieldMinY(), 0);
                desiredPosition = (WR::Utils::projectPointAtSegment(upperPoint, lowerPoint, loc()->ball()));
            }else{
                /*project ball position on the line that divides
                 * our field in the middle (x = 0.375 or x = -0.375)*/
                Position upperPoint(true, loc()->fieldMinX() + (loc()->fieldLength()/4), loc()->fieldMaxY(), 0);
                Position lowerPoint(true, loc()->fieldMinX() + (loc()->fieldLength()/4), loc()->fieldMinY(), 0);
                desiredPosition = (WR::Utils::projectPointAtSegment(upperPoint, lowerPoint, loc()->ball()));
            }
            //If this player is at the desiredPosition: rotateTo(ballPosition)
            _sk_goTo->setGoToPos(desiredPosition);
            //default value to increase robot vellocity (needs adjustment)
            _sk_goTo->setGoToVelocityFactor(2.0);

            if(player()->isNearbyPosition(desiredPosition, 0.03f)){
                enableTransition(STATE_ROTATE);
            }else{
                enableTransition(STATE_GOTO);
            }
        }else{
            _state = STATE_GOTOBALL;
        }
        break;
    }

    case STATE_GOTOBALL:{

        if(loc()->isInsideTheirField(loc()->ball()) || (player()->team()->hasBallPossession() && !player()->hasBallPossession())){
            _state = STATE_STAYBACK;
            break;
        }else{
            if(loc()->isInsideOurField(loc()->ball())){
                Position _aimPosition;
                //setting skill rotateTo
                _sk_rotateTo->setDesiredPosition(loc()->ball());

                _aimPosition.setUnknown();

                // Discover their player that have poss
                for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
                    if(PlayerBus::theirPlayerAvailable(x)){
                        if(PlayerBus::theirPlayer(x)->hasBallPossession()){
                            _aimPosition = PlayerBus::theirPlayer(x)->position();
                            break;
                        }
                    }
                }

                // If they don't have poss, aim position is now their goal
                if(_aimPosition.isUnknown()) _aimPosition = loc()->theirGoal();

                // Calc behind ball
                Position behindBall = WR::Utils::threePoints(loc()->ball(), _aimPosition, 0.03f, GEARSystem::Angle::pi);

                if(loc()->ballVelocity().abs() > BALLPREVISION_MINVELOCITY){
                    // Calc unitary vector of velocity
                    const Position velUni(true, loc()->ballVelocity().x()/loc()->ballVelocity().abs(), loc()->ballVelocity().y()/loc()->ballVelocity().abs(), 0.0);

                    // Calc velocity factor
                    float factor = BALLPREVISION_VELOCITY_FACTOR*loc()->ballVelocity().abs();
                    WR::Utils::limitValue(&factor, 0.0f, BALLPREVISION_FACTOR_LIMIT);

                    // Calc projected position
                    const Position delta(true, factor*velUni.x(), factor*velUni.y(), 0.0);
                    Position projectedPos(true, behindBall.x()+delta.x(), behindBall.y()+delta.y(), 0.0);

                    if(isBehindBall(projectedPos)){
                        behindBall = projectedPos;
                        behindBall = WR::Utils::threePoints(projectedPos, loc()->ball(), 0.02, GEARSystem::Angle::pi);
                    }
                }

                // Vx/Dx = Vy/Dy (V = velocity/ D = distance)
                float velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(behindBall)) / (WR::Utils::distance(loc()->ball(), behindBall));

                //setting skill goTo position
                _sk_goTo->setGoToPos(behindBall);

                //setting skill goTo velocity factor
                if(2 < 1.5*velocityNeeded){
                    _sk_goTo->setGoToVelocityFactor(1.5*velocityNeeded);
                }else{
                    _sk_goTo->setGoToVelocityFactor(2.0);
                }

                //setting skill push
                _sk_pushBall->setSpeedAndOmega(1.0, 0.0);

                //setting skill rotateTo
                _sk_rotateTo->setDesiredPosition(loc()->ball());

                //transitions
                if(player()->isLookingTo(loc()->ball(), 1.5)){
                    //std::cout << "PUSH" << std::endl;
                    enableTransition(STATE_PUSH);
                }else if(player()->isNearbyPosition(behindBall, 0.03f)){
                    //std::cout << "ROTATE " << player()->isLookingTo(loc()->ball()) << std::endl;
                    enableTransition(STATE_ROTATE);
                }else{
                    //std::cout << "GO BEHIND BALL" << std::endl;
                    enableTransition(STATE_GOTO);
                }
            }
            break;
        }
    }

    }
}

bool Behaviour_Assistant::isBehindBall(Position posObjective){
    Position posBall = loc()->ball();
    Position posPlayer = player()->position();
    float anglePlayer = WR::Utils::getAngle(posBall, posPlayer);
    float angleDest = WR::Utils::getAngle(posBall, posObjective);
    float diff = WR::Utils::angleDiff(anglePlayer, angleDest);

    return (diff>GEARSystem::Angle::pi/2.0f);
}
