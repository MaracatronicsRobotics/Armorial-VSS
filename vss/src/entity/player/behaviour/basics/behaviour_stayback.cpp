#include "behaviour_stayback.h"

QString Behaviour_StayBack::name() {
    return "Behaviour_StayBack";
}

Behaviour_StayBack::Behaviour_StayBack(){
    _sk_goTo = nullptr;
    _sk_spin = nullptr;
    _sk_rotateTo = nullptr;
}

void Behaviour_StayBack::configure() {
    //initializing variables
    loopsInSameRegionWithBall = 0;
    loopsInSameRegionWithOpp = 0;
    lastPlayerPosition.setPosition(50.0, 0.0, 0.0);
    lastPlayerPosition.setInvalid();
    ballPos = lastPlayerPosition;
    playerPos = lastPlayerPosition;

    //skills
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_spin = new Skill_Spin());

    //initial skill
    setInitialSkill(_sk_goTo);

    //initial state
    _state = STATE_STAYBACK;

    //goTo
    addTransition(STATE_GOTO, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTO, _sk_spin, _sk_goTo);
    //spin
    addTransition(STATE_SPIN, _sk_rotateTo, _sk_spin);
    addTransition(STATE_SPIN, _sk_goTo, _sk_spin);
    //rotateTo
    addTransition(STATE_ROTATE, _sk_spin, _sk_rotateTo);
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
};

void Behaviour_StayBack::run(){

    //CHECKING IF SPIN STATE IS NEEDED
    checkIfShouldSpin();

    switch(_state) {
    case STATE_STARTSPINNING:{
        _sk_spin->setClockWise(setSpinDirection());
        enableTransition(STATE_SPIN);
        break;
    }

    case STATE_STAYBACK:{
        //setting skill goTo
        Position desiredPosition(false, 0, 0, 0);

        //if ball is in their field - change this condition
        int factor = 1;
        if(loc()->ourSide().isRight()) factor = 1;
        else factor = -1;
        Position upper(true, loc()->ourFieldTopCorner().x() - factor*loc()->fieldLength()*0.2f, loc()->ourFieldTopCorner().y(), 0.0);
        Position lower(true, loc()->ourFieldBottomCorner().x() - factor*loc()->fieldLength()*0.2f, loc()->ourFieldBottomCorner().y(), 0.0);
        desiredPosition = (WR::Utils::projectPointAtSegment(upper, lower, loc()->ball()));

        //Calculating limit value to desiredPosition.y()
        float tan = fabs(loc()->ball().y() - loc()->ourGoal().y())/fabs(loc()->ball().x() - loc()->ourGoal().x());
        float Ylim = tan * fabs(desiredPosition.x() - loc()->ourGoal().x());

        float posY = desiredPosition.y();
        WR::Utils::limitValue(&posY, -Ylim, Ylim);
        desiredPosition.setPosition(desiredPosition.x(), posY, desiredPosition.z());

        //setting skill rotateTo
        _sk_rotateTo->setDesiredPosition(loc()->ball());

        //setting skill goTo position
        desiredPosition = projectPosOutsideGoalArea(desiredPosition, true, false);
        _sk_goTo->setGoToPos(desiredPosition);
        //setting skill goTo velocity factor
        //default value to increase robot vellocity (needs adjustment)
        _sk_goTo->setGoToVelocityFactor(2.0);

        //transitions
        //if this player is at the desiredPosition: rotateTo(ballPosition)
        if(player()->isNearbyPosition(desiredPosition, 0.03f)){
            enableTransition(STATE_ROTATE);
        }else{
            enableTransition(STATE_GOTO);
        }
        break;
    }
    }
}

Position Behaviour_StayBack::projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea){
    Position L1, L2, R1, R2;
    bool shouldProjectPos = false, isOurArea = false;

    if(fabs(pos.x()) > (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && fabs(pos.y()) < loc()->fieldDefenseLength()/2){
        shouldProjectPos = true;
        //check if desiredPosition is inside our defense area and if we should avoid it
        if(loc()->isInsideOurField(pos) && avoidOurArea){
            shouldProjectPos = true;
            isOurArea = true;
        }
        //check if desiredPosition is inside their defense area and if we should avoid it
        else if(loc()->isInsideTheirField(pos) && avoidTheirArea){
            shouldProjectPos = true;
            isOurArea = false;
        }else{
            shouldProjectPos = false;
        }
    }
    //if should project position outside our defense area
    if(shouldProjectPos){
        //multiplying factor changes if area's team is the left team or the right team
        float mult = -1.0;
        if(isOurArea){
            if(loc()->ourSide().isLeft()) mult = 1.0;
        }else{
            if(loc()->theirSide().isLeft()) mult = 1.0;
        }
        //getting segments

        //left segment points (defense area)
        L1 = Position(true, -mult*loc()->fieldMaxX(), loc()->fieldDefenseLength()/2, 0.0f);
        L2 = Position(true, L1.x() + mult * loc()->fieldDefenseWidth(), L1.y(), 0.0f);
        //right segment points (defense area)
        R1 = Position(true, -mult*loc()->fieldMaxX(), -loc()->fieldDefenseLength()/2, 0.0f);
        R2 = Position(true, R1.x() + mult * loc()->fieldDefenseWidth(), R1.y(), 0.0f);
        //front segment is composed by L2 and R2 (defense area)

        //projecting position on segments L1->L2, R1->R2, L2->R2
        Position pointProjLeft = WR::Utils::projectPointAtSegment(L1, L2, pos);
        Position pointProjRight = WR::Utils::projectPointAtSegment(R1, R2, pos);
        Position pointProjFront = WR::Utils::projectPointAtSegment(L2, R2, pos);

        //interception points between the segment playerPos->pos and defense area segments (L1->L2, R1->R2, L2->R2)
        Position left = WR::Utils::hasInterceptionSegments(L1, L2, player()->position(), pos);
        Position right = WR::Utils::hasInterceptionSegments(R1, R2, player()->position(), pos);
        Position front = WR::Utils::hasInterceptionSegments(L2, R2, player()->position(), pos);

        //if there is an interception between playerPos->pos and L1->L2 on L1->L2
        if(left.isValid() && fabs(left.x()) >= (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && fabs(left.x()) <= loc()->fieldMaxX()){
            //if initial position isn't between goal post and defense area post
            if(fabs(pos.y()) < fabs(loc()->ourGoalLeftPost().y())){
                return pointProjFront;
            }else{
                return pointProjLeft;
            }
        }
        //if there is an interception between playerPos->pos and R1->R2 on R1->R2
        else if(right.isValid() && fabs(right.x()) >= (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && fabs(right.x()) <= loc()->fieldMaxX()){
            //if initial position isn't between goal post and defense area post
            if(fabs(pos.y()) < fabs(loc()->ourGoalLeftPost().y())){
                return pointProjFront;
            }else{
                return pointProjRight;
            }
        }
        //if there is an interception between playerPos->pos and L2->R2 on L2->R2
        else if(front.isValid() && fabs(front.x()) >= (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && fabs(front.x()) <= loc()->fieldMaxX()){
            return pointProjFront;
        }else{
            return pos;
        }
    }else{
        return pos;
    }
}

bool Behaviour_StayBack::setSpinDirection(){
    //true if clockwise, false otherwise
    if (loc()->distBallOurRightPost() < loc()->distBallOurLeftPost())
        return true;
    else
        return false;
}

bool Behaviour_StayBack::checkIfShouldSpin(){
    //update actual player and ball positions (if they're valid)
    if(player()->position().isValid()) playerPos = player()->position();
    if(loc()->ball().isValid()) ballPos = loc()->ball();

    //find nearest opponent
    Position oppPos(false, 1000, 0, 0);
    float minorDist = 10000.0;
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x) && PlayerBus::theirPlayer(x)->position().isValid()){
            if(PlayerBus::theirPlayer(x)->distanceTo(playerPos) < minorDist){
                oppPos = PlayerBus::theirPlayer(x)->position();
                minorDist = PlayerBus::theirPlayer(x)->distanceTo(playerPos);
            }
        }
    }

    bool isNearBall, isNearOpp, returnBool = false;

    //if last loop's position is valid and player is near both ball position and last position: increment variable
    isNearBall = (lastPlayerPosition.isValid() && (WR::Utils::distance(playerPos, lastPlayerPosition) <= 0.01f) && (WR::Utils::distance(playerPos, ballPos) <= 0.07f));
    //if last loop's position is valid and player is very close to the closest opponent: increment variable
    isNearOpp = (lastPlayerPosition.isValid() && oppPos.isValid() && (WR::Utils::distance(playerPos, oppPos) <= 0.1f));

    //setting _state variable according to isNearBall and isNearOpp
    if(isNearBall || isNearOpp){
        if(isNearBall){
            loopsInSameRegionWithBall++;
            //if isNearBall is true along multiple cycles
            if(loopsInSameRegionWithBall >= 300 && loopsInSameRegionWithBall <= 600){
                //std::cout << "SPINNING BALL" << std::endl;
                _state = STATE_STARTSPINNING;
                returnBool = true;
            }else{
                _state = STATE_STAYBACK;
                returnBool = false;
                loopsInSameRegionWithBall = 0;
            }
        }else{
            loopsInSameRegionWithBall = 0;
        }

        if(isNearOpp){
            loopsInSameRegionWithOpp++;
            //in case isNearBall = true and isNearOpp = true so they won't overwrite _state variable
            if(!isNearBall){
                //if isNearBall is true along multiple cycles
                if(loopsInSameRegionWithOpp >= 300 && loopsInSameRegionWithOpp <= 600){
                    //std::cout << "SPINNING OPP" << std::endl;
                    _state = STATE_STARTSPINNING;
                    returnBool = true;
                }else{
                    _state = STATE_STAYBACK;
                    returnBool = false;
                    loopsInSameRegionWithOpp = 0;
                }
            }
        }else{
            loopsInSameRegionWithOpp = 0;
        }
    }else{
        lastPlayerPosition = playerPos;
        _state = STATE_STAYBACK;
        returnBool = false;
    }
    return returnBool;
}
