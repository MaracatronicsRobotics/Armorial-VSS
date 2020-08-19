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
    //initializing variables
    loopsInSameRegionWithBall = 0;
    loopsInSameRegionWithOpp = 0;
    lastPlayerPosition.setPosition(50.0, 0.0, 0.0);
    lastPlayerPosition.setInvalid();
    ballPos = lastPlayerPosition;
    playerPos = lastPlayerPosition;
    prevOppId = 100;

    //skills
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

void Behaviour_Assistant::run(){

    //CHECKING IF SPIN STATE IS NEEDED
    checkIfShouldSpin();

    switch(_state) {
    case STATE_STARTSPINNING:{
        _sk_spin->setClockWise(setSpinDirection());
        enableTransition(STATE_SPIN);
        break;
    }

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
            enableTransition(STATE_GOTO);
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
                        //projecting point further than calculated position
                        behindBall = WR::Utils::threePoints(projectedPos, loc()->ball(), 0.02, GEARSystem::Angle::pi);
                    }
                }

                // Vx/Dx = Vy/Dy (V = velocity/ D = distance)
                float velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(behindBall)) / (WR::Utils::distance(loc()->ball(), behindBall));

                //setting skill goTo position
                _sk_goTo->setGoToPos(behindBall);

                //setting skill goTo velocity factor
                if(2 < 1.2*velocityNeeded){
                    _sk_goTo->setGoToVelocityFactor(1.2*velocityNeeded);
                }else{
                    _sk_goTo->setGoToVelocityFactor(2.0);
                }

                //setting skill push
                _sk_pushBall->setSpeedAndOmega(1.0, 0.0);

                //setting skill rotateTo
                _sk_rotateTo->setDesiredPosition(loc()->ball());

                //transitions
                if(player()->isLookingTo(loc()->ball(), 1.5)){
                    std::cout << "PUSH" << std::endl;
                    enableTransition(STATE_PUSH);
                }else if(player()->isNearbyPosition(behindBall, 0.03f)){
                    std::cout << "ROTATE " << player()->isLookingTo(loc()->ball()) << std::endl;
                    enableTransition(STATE_ROTATE);
                }else{
                    std::cout << "GO BEHIND BALL" << std::endl;
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

bool Behaviour_Assistant::setSpinDirection(){
    // true if clockwise, false otherwise
    if (loc()->distBallOurRightPost() < loc()->distBallOurLeftPost())
        return true;
    else
        return false;
}

bool Behaviour_Assistant::checkIfShouldSpin(){
    //update actual player and ball positions (if they're valid)
    if(player()->position().isValid()) playerPos = player()->position();
    if(loc()->ball().isValid()) ballPos = loc()->ball();

    Position oppPos(true, 1000, 0, 0);
    quint8 oppId = 99;
    float minorDist = 10000.0;
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x) && PlayerBus::theirPlayer(x)->position().isValid()){
            if(PlayerBus::theirPlayer(x)->distanceTo(playerPos) < minorDist){
                oppId = PlayerBus::theirPlayer(x)->playerId();
                oppPos = PlayerBus::theirPlayer(x)->position();
                minorDist = PlayerBus::theirPlayer(x)->distanceTo(playerPos);
            }
        }
    }

    //std::cout << WR::Utils::distance(playerPos, oppPos) << "   "<< oppId <<  std::endl;


    bool isNearBall, isNearOpp, returnBool = false;

    //if last loop's position is valid and player is near both ball position and last position: increment variable
    isNearBall = (lastPlayerPosition.isValid() && (WR::Utils::distance(playerPos, lastPlayerPosition) <= 0.02f) && (WR::Utils::distance(playerPos, ballPos) <= 0.07f));
    //if last loop's position is valid and player is near both opp position and last position: increment variable
    isNearOpp = (lastPlayerPosition.isValid() && (WR::Utils::distance(playerPos, lastPlayerPosition) <= 0.02f) && (WR::Utils::distance(playerPos, oppPos) <= 0.1f));

    if(isNearBall || isNearOpp){
        if(isNearBall){
            loopsInSameRegionWithBall++;
            //std::cout << loopsInSameRegionWithBall << std::endl;
            if(loopsInSameRegionWithBall >= 300){
                std::cout << "SPINNING BALL" << std::endl;
                _state = STATE_STARTSPINNING;
                returnBool = true;
            }else if(loc()->isInsideTheirField(loc()->ball()) || (player()->team()->hasBallPossession() && !player()->hasBallPossession())){
                _state = STATE_STAYBACK;
                returnBool = false;
            }else{
                _state = STATE_GOTOBALL;
                returnBool = false;
            }
        }else{
            loopsInSameRegionWithBall = 0;
        }

        if(isNearOpp){
            //loopsInSameRegionWithOpp++;
            if(loopsInSameRegionWithOpp >= 300){
                //std::cout << "SPINNING OPP" << std::endl;
                _state = STATE_STARTSPINNING;
                returnBool = true;
            }else if(loc()->isInsideTheirField(loc()->ball()) || (player()->team()->hasBallPossession() && !player()->hasBallPossession())){
                _state = STATE_STAYBACK;
                returnBool = false;
            }else{
                _state = STATE_GOTOBALL;
                returnBool = false;
            }
        }else{
            loopsInSameRegionWithOpp = 0;
        }
    }else{
        lastPlayerPosition = playerPos;
        if(loc()->isInsideTheirField(loc()->ball()) || (player()->team()->hasBallPossession() && !player()->hasBallPossession())){
            _state = STATE_STAYBACK;
            returnBool = false;
        }else{
            _state = STATE_GOTOBALL;
            returnBool = false;
        }
    }
    return returnBool;
}
