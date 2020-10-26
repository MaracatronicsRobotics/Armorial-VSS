#include "behaviour_assistant.h"

#define BALLPREVISION_MINVELOCITY 0.02f
#define BALLPREVISION_VELOCITY_FACTOR 3.0f
#define BALLPREVISION_FACTOR_LIMIT 0.15f

QString Behaviour_Assistant::name() {
    return "Behaviour_Assistant";
}

Behaviour_Assistant::Behaviour_Assistant() {
    _sk_goTo = nullptr;
    _sk_spin = nullptr;
    _sk_rotateTo = nullptr;
    _sk_pushBall = nullptr;
}

void Behaviour_Assistant::configure() {
    //initializing variables
    loopsInSameRegionWithBall = 0;
    loopsInSameRegionWithOpp = 0;
    lastPlayerPosition.setPosition(50.0, 0.0, 0.0);
    lastPlayerPosition.setInvalid();
    ballPos = lastPlayerPosition;
    playerPos = lastPlayerPosition;

    //skills
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_spin = new Skill_Spin());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_pushBall = new Skill_PushBall());

    //initial state
    _state = STATE_GOTOBALL;
    _skill = GOTO;

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

    case STATE_GOTOBALL:{
        Position _aimPosition;
        _aimPosition.setUnknown();
        float behindBallAngle = 0;

        //error parameter to determine if player is looking to our goal
        float errorAngleToOurGoal = errorAngleToSegment(loc()->ourGoalLeftPost(), loc()->ourGoalRightPost());

        //error parameter to determine if player is looking to their side
        float errorAngleToTheirSide = 0;
        if(loc()->theirSide().isLeft()){
            errorAngleToTheirSide = errorAngleToSegment(loc()->fieldLeftBottomCorner(), loc()->fieldLeftTopCorner());
        }else{
            errorAngleToTheirSide = errorAngleToSegment(loc()->fieldRightTopCorner(), loc()->fieldRightBottomCorner());
        }

        //is player behind ball x? (reference: their goal)
        bool playerBehindBall = isBehindBallX(player()->playerId());
        if(playerBehindBall) _sk_goTo->setAvoidBall(false);
        else if(player()->isLookingTo(loc()->ourGoal(), errorAngleToOurGoal)) _sk_goTo->setAvoidBall(true);

        /*
         * considering angle between the ball and their goal to define the radius of a circle around the ball:
         * if ball is near their goal and far from the middle line: increase radius
         * the radius determine the maximum distance the player must have to the ball so he can start following its position
        */
        float angleBallToTheirGoal = abs(WR::Utils::getAngle(loc()->ball(), loc()->theirGoal()));
        float aroundBall;
        if(localIsLookingTo(loc()->theirGoal(), errorAngleToTheirSide) && playerBehindBall) aroundBall = 0.17f + 0.05f*(1.0f - angleBallToTheirGoal/Angle::pi);
        else aroundBall = 0.05f;

        //Defining ballOffset: distance from the ball to the point behindthe ball that the player must follow
        float ballOffset = 0;
        if(loc()->isInsideOurField(loc()->ball()) && !playerBehindBall){
            ballOffset = 0.16f;
            //if ball is inside our field, our player should be between our goal and the ball:
            _aimPosition = loc()->ourGoal();
            behindBallAngle = 0;
        }else if(player()->isNearbyPosition(loc()->ball(), aroundBall) && playerBehindBall){
            //if the player is behind ball x and should go to ball position
            ballOffset = 0.02f;
            //std::cout << "locball\n";
        }else{
            //std::cout << "behindball\n";
            ballOffset = 0.16f;
        }

        //if ball isn't inside our field, aim position is now their goal
        if(_aimPosition.isUnknown()){
            behindBallAngle = GEARSystem::Angle::pi;
            _aimPosition = loc()->theirGoal();
        }

        bool shouldGoToBall = canGoToBall();
        //calc behind ball
        Position behindBall;
        //if there's an ally closer to the ball or better positioned: keep some distance from ball
        if(!shouldGoToBall){
            behindBall = WR::Utils::threePoints(loc()->ball(), _aimPosition, 0.2f + ballOffset, behindBallAngle);
        }else{
            behindBall = WR::Utils::threePoints(loc()->ball(), _aimPosition, ballOffset, behindBallAngle);
        }
        //ball position projection based on its velocity and actual position
        if(loc()->ballVelocity().abs() > BALLPREVISION_MINVELOCITY){
            //calc unitary vector of velocity
            const Position velUni(true, loc()->ballVelocity().x()/loc()->ballVelocity().abs(), loc()->ballVelocity().y()/loc()->ballVelocity().abs(), 0.0);

            //calc velocity factor
            float factor = BALLPREVISION_VELOCITY_FACTOR*loc()->ballVelocity().abs();
            WR::Utils::limitValue(&factor, 0.0f, BALLPREVISION_FACTOR_LIMIT);

            //calc projected position
            const Position delta(true, factor*velUni.x(), factor*velUni.y(), 0.0);
            Position projectedPos(true, loc()->ball().x()+delta.x(), loc()->ball().y()+delta.y(), 0.0);

            if(isBehindBall(projectedPos)){
                //if there's an ally closer to the ball: keep some distance from ball
                if(!shouldGoToBall){
                    behindBall = WR::Utils::threePoints(projectedPos, _aimPosition, 0.2f + ballOffset, behindBallAngle);
                }else{
                    behindBall = WR::Utils::threePoints(projectedPos, _aimPosition, ballOffset, behindBallAngle);
                }
            }
        }

        //setting skill goTo position
        bool avoidOurArea = true, avoidTheirArea = allyInTheirArea();
        _sk_goTo->setAvoidOurGoalArea(avoidOurArea);
        _sk_goTo->setAvoidTheirGoalArea(avoidTheirArea);
        behindBall = projectPosOutsideGoalArea(behindBall, avoidOurArea, avoidTheirArea);
        _sk_goTo->setGoToPos(behindBall);

        //setting skill goTo velocity factor
        // Vx/Dx = Vy/Dy (V = velocity/ D = distance)
        float distFac = 1.0f - player()->distanceTo(behindBall)/WR::Utils::distance(loc()->fieldLeftTopCorner(), loc()->fieldRightBottomCorner());
        float velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(behindBall)) / (WR::Utils::distance(loc()->ball(), behindBall));
        if(velocityNeeded > ((1.0f+distFac)*4.0f)){
            _sk_goTo->setVelocityNeeded(1.5f*velocityNeeded);
        }else{
            _sk_goTo->setVelocityNeeded((1.0f+distFac)*4.0f);
        }

        //setting skill rotateTo
        _sk_rotateTo->setDesiredPosition(loc()->theirGoal());

        //error parameter to determine if player is looking to their goal
        float errorAngleToTheirGoal = 0.9f * errorAngleToSegment(loc()->theirGoalLeftPost(), loc()->theirGoalRightPost());

        //transitions
        if(_skill == PUSH){
            //std::cout << "PUSH" << std::endl;
            if(!playerBehindBall || !player()->isNearbyPosition(loc()->ball(), 0.15f) || !localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)){
                //ROTATE
                if(player()->isNearbyPosition(behindBall, 0.07f) && !localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) && localIsLookingTo(loc()->ball(), 0.3f)){
                    enableTransition(STATE_ROTATE);
                    _skill = ROT;
                }
                //GOTO
                else{
                    enableTransition(STATE_GOTO);
                    _skill = GOTO;
                }
            }
            //SPIN
            if((!player()->isLookingTo(loc()->theirGoal(), 0.8f*errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
                _sk_spin->setClockWise(!setSpinDirection());
                enableTransition(STATE_SPIN);
                _skill = SPIN;
            }
        }else if(_skill == ROT){
            //std::cout << "ROTATE" << std::endl;
            if(localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) || !localIsLookingTo(loc()->ball(), 0.4f)){
                //PUSH
                if(localIsLookingTo(loc()->ball(), 0.6f) && playerBehindBall && player()->isNearbyPosition(loc()->ball(), 0.1f) && localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)){
                    if(!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)){
                        _sk_pushBall->setSpeedAndOmega(-1.5, 0.0);
                    }else{
                        _sk_pushBall->setSpeedAndOmega(1.5, 0.0);
                    }
                    enableTransition(STATE_PUSH);
                    _skill = PUSH;
                }
                //GOTO
                else{
                    enableTransition(STATE_GOTO);
                    _skill = GOTO;
                }
            }
            //SPIN
            if((!player()->isLookingTo(loc()->theirGoal(), 0.8f*errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
                _sk_spin->setClockWise(!setSpinDirection());
                enableTransition(STATE_SPIN);
                _skill = SPIN;
            }
        }else if(_skill == GOTO){
            //std::cout << "GOTO" << std::endl;
            //PUSH
            if(localIsLookingTo(loc()->ball(), 0.6f) && playerBehindBall && player()->isNearbyPosition(loc()->ball(), 0.1f) && localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)){
                if(!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)){
                    _sk_pushBall->setSpeedAndOmega(-1.5, 0.0);
                }else{
                    _sk_pushBall->setSpeedAndOmega(1.5, 0.0);
                }
                enableTransition(STATE_PUSH);
                _skill = PUSH;
            }
            //ROTATE
            else if(player()->isNearbyPosition(behindBall, 0.03f) && !localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) && localIsLookingTo(loc()->ball(), 0.3f)){
                enableTransition(STATE_ROTATE);
                _skill = ROT;
            }
            //SPIN
            if((!player()->isLookingTo(loc()->theirGoal(), 0.8f*errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
                _sk_spin->setClockWise(!setSpinDirection());
                enableTransition(STATE_SPIN);
                _skill = SPIN;
            }
        }
        if(_skill == SPIN){
            if(!(!player()->isLookingTo(loc()->theirGoal(), 0.8f*errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
                enableTransition(STATE_GOTO);
                _skill = GOTO;
            }
        }

        break;
    }

    }
}

float Behaviour_Assistant::errorAngleToSegment(Position leftPoint, Position rightPoint){
    float angLeft = (WR::Utils::getAngle(player()->position(), leftPoint));
    float angRight = (WR::Utils::getAngle(player()->position(), rightPoint));
    return abs(angRight - angLeft)/2;
}

bool Behaviour_Assistant::isBehindBallX(quint8 id){
    //is player behind ball x? (reference: their goal)
    bool playerBehindBall = false;
    if(loc()->ourSide().isRight()){
        if(PlayerBus::ourPlayer(id)->position().x() > loc()->ball().x()){
            playerBehindBall = true;
        }
    }else{
        if(PlayerBus::ourPlayer(id)->position().x() < loc()->ball().x()){
            playerBehindBall = true;
        }
    }
    return playerBehindBall;
}

bool Behaviour_Assistant::allyInTheirArea(){
    //find out if there's an ally inside their area
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::ourPlayerAvailable(x) && PlayerBus::ourPlayer(x)->playerId() != player()->playerId()){
            if(loc()->isInsideTheirArea(PlayerBus::ourPlayer(x)->position())){
                return true;
            }
        }
    }
    return false;
}

bool Behaviour_Assistant::localIsLookingTo(const Position &pos, float error){
    Angle angle1(true, WR::Utils::getAngle(player()->position(), pos));

    // Calc diff
    float dif = abs(WR::Utils::angleDiff(player()->orientation(), angle1));
    if(dif > Angle::pi/2) dif = abs(Angle::pi - dif);
    return (dif <= error);
}

Position Behaviour_Assistant::projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea){
    Position L1, L2, R1, R2;
    bool shouldProjectPos = false, isOurArea = false;
    float smallMargin = 0.05f;

    //if position is beyond field's walls
    if(pos.y() < loc()->fieldMinY()) pos.setPosition(pos.x(), loc()->fieldMinY(), pos.z());
    else if(pos.y() > loc()->fieldMaxY()) pos.setPosition(pos.x(), loc()->fieldMaxY(), pos.z());

    if(pos.x() < loc()->fieldMinX()) pos.setPosition(loc()->fieldMinX(), pos.y(), pos.z());
    else if(pos.x() > loc()->fieldMaxX()) pos.setPosition(loc()->fieldMaxX(), pos.y(), pos.z());

    if(abs(pos.x()) > (loc()->fieldMaxX() - loc()->fieldDefenseWidth()) && abs(pos.y()) < loc()->fieldDefenseLength()/2){
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
    //if should project position outside a defense area
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
        L1 = Position(true, -mult*loc()->fieldMaxX(), loc()->fieldDefenseLength()/2 + smallMargin, 0.0f);
        L2 = Position(true, L1.x() + mult * (loc()->fieldDefenseWidth() + smallMargin), L1.y(), 0.0f);
        //right segment points (defense area)
        R1 = Position(true, -mult*loc()->fieldMaxX(), -(loc()->fieldDefenseLength()/2 + smallMargin), 0.0f);
        R2 = Position(true, R1.x() + mult * (loc()->fieldDefenseWidth() + smallMargin), R1.y(), 0.0f);
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
        if(left.isValid() && abs(left.x()) >= (loc()->fieldMaxX() - (loc()->fieldDefenseWidth()+smallMargin)) && abs(left.x()) <= loc()->fieldMaxX()){
            //if initial position isn't between goal post and defense area post
            if(abs(pos.y()) < abs(loc()->ourGoalLeftPost().y())){
                return pointProjFront;
            }else{
                return pointProjLeft;
            }
        }
        //if there is an interception between playerPos->pos and R1->R2 on R1->R2
        else if(right.isValid() && abs(right.x()) >= (loc()->fieldMaxX() - (loc()->fieldDefenseWidth()+smallMargin)) && abs(right.x()) <= loc()->fieldMaxX()){
            //if initial position isn't between goal post and defense area post
            if(abs(pos.y()) < abs(loc()->ourGoalLeftPost().y())){
                return pointProjFront;
            }else{
                return pointProjRight;
            }
        }
        //if there is an interception between playerPos->pos and L2->R2 on L2->R2
        else if(front.isValid() && abs(front.x()) >= (loc()->fieldMaxX() - (loc()->fieldDefenseWidth()+smallMargin)) && abs(front.x()) <= loc()->fieldMaxX()){
            return pointProjFront;
        }else{
            return pos;
        }
    }else{
        return pos;
    }
}

bool Behaviour_Assistant::canGoToBall(){
    float minorDistToBall = 1000;
    quint8 id = 100;
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::ourPlayerAvailable(x) && PlayerBus::ourPlayer(x)->position().isValid() &&
            PlayerBus::ourPlayer(x)->playerId() != player()->playerId() && !loc()->isInsideOurArea(PlayerBus::ourPlayer(x)->position())){
            if(PlayerBus::ourPlayer(x)->distBall() < minorDistToBall){
                id = PlayerBus::ourPlayer(x)->playerId();
                minorDistToBall = PlayerBus::ourPlayer(x)->distBall();
            }
        }
    }
    //if allyId is a valid id
    if(PlayerBus::ourPlayerAvailable(id)){
        //if there's an ally closer to the ball and he's moving: keep some distance from ball
        if(PlayerBus::ourPlayer(id)->distBall() < player()->distBall() && PlayerBus::ourPlayer(id)->velocity().abs() > BALLPREVISION_MINVELOCITY){
            //if the closest ally to ball isn't behind it, and this player is: we can go to ball
            if(!isBehindBallX(id) && isBehindBallX(player()->playerId())) return true;
            else return false;
        }else{
            return true;
        }
    }else{
        return true;
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
    //true if clockwise, false otherwise
    if (loc()->distBallOurRightPost() < loc()->distBallOurLeftPost())
        return true;
    else
        return false;
}

bool Behaviour_Assistant::checkIfShouldSpin(){
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
                _state = STATE_GOTOBALL;
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
                    _state = STATE_GOTOBALL;
                    returnBool = false;
                    loopsInSameRegionWithOpp = 0;
                }
            }
        }else{
            loopsInSameRegionWithOpp = 0;
        }
    }else{
        lastPlayerPosition = playerPos;
        _state = STATE_GOTOBALL;
        returnBool = false;
    }
    return returnBool;
}
