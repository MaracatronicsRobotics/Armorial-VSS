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
    reduceOffset = false;
    ballOffset = 0.0f;
    behindBall.setUnknown();

    //skills
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_spin = new Skill_Spin());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_pushBall = new Skill_PushBall());

    //initial skill state
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

    Position _aimPosition, ballProjection;
    //aim position is their goal and player shoul be behind it and behind the ball
    float behindBallAngle = GEARSystem::Angle::pi;
    _aimPosition = loc()->theirGoal();

    //ball position projection based on its velocity and actual position
    if(loc()->ballVelocity().abs() > BALLPREVISION_MINVELOCITY){
        //calc vector of velocity
        const Position vel(true, loc()->ballVelocity().x(), loc()->ballVelocity().y(), 0.0);

        //calc projected position
        Position projectedPos(true, loc()->ball().x()+vel.x(), loc()->ball().y()+vel.y(), 0.0);

        if(isBehindBall(player()->position(), projectedPos)){
            ballProjection = projectedPos;
        }else{
            ballProjection = loc()->ball();
        }
    }else{
        ballProjection = loc()->ball();
    }

    //error parameter to determine if player is looking to our goal
    float errorAngleToOurGoal = errorAngleToSegment(loc()->ourGoalLeftPost(), loc()->ourGoalRightPost());

    //error parameter to determine if player is looking to their side
    float errorAngleToTheirSide = 0;
    bool playerBehindBall;
    if(loc()->theirSide().isLeft()){
        errorAngleToTheirSide = errorAngleToSegment(loc()->fieldLeftBottomCorner(), loc()->fieldLeftTopCorner());
        //is player behind ball? (rference: their goal)
        playerBehindBall = isBehindBall(player()->position(), Position(true, loc()->theirGoal().x() - 0.1f, loc()->theirGoal().y(), 0.0f));
    }else{
        errorAngleToTheirSide = errorAngleToSegment(loc()->fieldRightTopCorner(), loc()->fieldRightBottomCorner());
        //is player behind ball? (rference: their goal)
        playerBehindBall = isBehindBall(player()->position(), Position(true, loc()->theirGoal().x() + 0.1f, loc()->theirGoal().y(), 0.0f));
    }

    if(playerBehindBall) _sk_goTo->setAvoidBall(false);
    else if(player()->isLookingTo(loc()->ourGoal(), 1.1f*errorAngleToOurGoal)) _sk_goTo->setAvoidBall(true);

    //***** ballOffset definition *****
    //if we shouldn't make our player get really close to ball
    if(!reduceOffset){
        /*
             * considering distance between our player and ballProjection to define 'ballOffset' variable (which determines how distant will our player
             * try to be positioned from ballProjection position (and behind it)
            */
        float distBallProjFactor = (player()->distanceTo(ballProjection)/WR::Utils::distance(Position(true,loc()->fieldMinX(), 0.0f, 0.0f), Position(true, loc()->fieldMaxX(), loc()->fieldMinY(), 0.0f)));
        ballOffset = 0.06f+distBallProjFactor * 0.7f;
        /*
             * if (our player is on the line ball->theirGoal and it's behind ball's X and it's near the ball (0.25f))
             * or (our player is near 'behindBall' position (last position he was going to) and it's behind ball's X)
             * or (our player is near ballProjection position and it's behind ball's X)
             * reduce offset and get closer to ball!
            */
        if((WR::Utils::isPointAtLine(_aimPosition, ballProjection, player()->position()) && playerBehindBall && player()->isNearbyPosition(ballProjection, 0.25f)) ||
                (player()->isNearbyPosition(behindBall, 0.02f) && playerBehindBall) ||
                (playerBehindBall && player()->distBall() >= 0.13f && player()->isNearbyPosition(loc()->ball(), 0.15f))){
            reduceOffset = true;
            if(ballOffset > 0.04f) ballOffset = 0.04f;
        }
        //if we should make our player get closer to ball
    }else{
        //if our player is too far from ballProjection: stop reducing offset
        if(player()->distanceTo(loc()->ball()) > 0.5f || (!playerBehindBall || (playerBehindBall && player()->distBall() < 0.032f))){
            reduceOffset = false;
        }
        //if ballOffset is bigger than 0.035 and our player is very close to ballProjection: go to ball
        else if(ballOffset > 0.035f && player()->distanceTo(ballProjection) < 0.045f){
            ballOffset = 0.01f;
        }
        //if our player is no longer very close to ball: increase a little bit ballOffset
        else if(player()->distanceTo(ballProjection) > 0.065f && ballOffset < 0.035f){
            ballOffset = 0.04f;
        }
    }

    bool shouldGoToBall = canGoToBall();
    //if there's an ally closer to the ball or better positioned: keep some distance from ball
    if(!shouldGoToBall){
        behindBall = WR::Utils::threePoints(ballProjection, _aimPosition, 0.2f + ballOffset, behindBallAngle);
    }else{
        behindBall = WR::Utils::threePoints(ballProjection, _aimPosition, ballOffset, behindBallAngle);
    }

    //setting skill goTo position
    bool avoidOurArea = true, avoidTheirArea = allyInTheirArea();
    _sk_goTo->setAvoidOurGoalArea(avoidOurArea);
    _sk_goTo->setAvoidTheirGoalArea(avoidTheirArea);
    behindBall = projectPosOutsideGoalArea(behindBall, avoidOurArea, avoidTheirArea);
    _sk_goTo->setGoToPos(behindBall);

    //setting skill goTo velocity factor
    // Vx/Dx = Vy/Dy (V = velocity/ D = distance)

    float velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(behindBall)) / (WR::Utils::distance(loc()->ball(), ballProjection));
    if(!isnanf(velocityNeeded)){
        if(1.2f*velocityNeeded < 10.0f) _sk_goTo->setGoToVelocityFactor(10.0f);
        else _sk_goTo->setGoToVelocityFactor(1.2f*velocityNeeded);
    }else{
        _sk_goTo->setGoToVelocityFactor(10.0f);
    }

    //_sk_goTo->setGoToVelocityFactor(1.5f);
    //setting skill rotateTo
    _sk_rotateTo->setDesiredPosition(loc()->theirGoal());

    //error parameter to determine if player is looking to their goal
    float errorAngleToTheirGoal = 0.9f * errorAngleToSegment(loc()->theirGoalLeftPost(), loc()->theirGoalRightPost());

    //transitions
    if(_skill == PUSH){
        //std::cout << "PUSH" << std::endl;
        if(!playerBehindBall || !player()->isNearbyPosition(loc()->ball(), 0.15f) || !localIsLookingTo(loc()->theirGoal(), 1.2f*errorAngleToTheirGoal)){
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
        if((!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
            _sk_spin->setClockWise(!setSpinDirection());
            enableTransition(STATE_SPIN);
            _skill = SPIN;
        }
    }else if(_skill == ROT){
        //std::cout << "ROTATE" << std::endl;
        if(localIsLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) || !localIsLookingTo(loc()->ball(), 0.4f)){
            //PUSH
            if(localIsLookingTo(loc()->ball(), 0.6f) && playerBehindBall && player()->isNearbyPosition(loc()->ball(), 0.1f) && localIsLookingTo(loc()->theirGoal(), 1.2f*errorAngleToTheirGoal)){
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
        if((!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
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
        else if(player()->isNearbyPosition(behindBall, 0.03f) && !localIsLookingTo(loc()->theirGoal(), 1.2f*errorAngleToTheirGoal) && localIsLookingTo(loc()->ball(), 0.3f)){
            enableTransition(STATE_ROTATE);
            _skill = ROT;
        }
        //SPIN
        if((!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
            _sk_spin->setClockWise(!setSpinDirection());
            enableTransition(STATE_SPIN);
            _skill = SPIN;
        }
    }
    if(_skill == SPIN){
        if(!(!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal) && loc()->isInsideTheirArea(player()->position()) && player()->isNearbyPosition(loc()->ball(), 0.08f))){
            enableTransition(STATE_GOTO);
            _skill = GOTO;
        }
    }
}

float Behaviour_Assistant::errorAngleToSegment(Position leftPoint, Position rightPoint){
    float angLeft = (WR::Utils::getAngle(player()->position(), leftPoint));
    float angRight = (WR::Utils::getAngle(player()->position(), rightPoint));
    return abs(angRight - angLeft)/2;
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
            if(!isBehindBall(PlayerBus::ourPlayer(id)->position(), loc()->theirGoal()) && isBehindBall(player()->position(), loc()->theirGoal())) return true;
            else return false;
        }else{
            return true;
        }
    }else{
        return true;
    }
}

bool Behaviour_Assistant::isBehindBall(Position posPlayer, Position posObjective){
    Position posBall = loc()->ball();
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
