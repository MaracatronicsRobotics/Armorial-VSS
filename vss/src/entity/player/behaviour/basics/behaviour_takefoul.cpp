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
    setInitialSkill(_sk_goTo);
    _skill = GOTO;
    canChangePosToLook = true;
    counter = 0;

    //transitions
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_ROTATE, _sk_pushBall, _sk_rotateTo);
    addTransition(STATE_PUSH, _sk_rotateTo, _sk_pushBall);
    addTransition(STATE_PUSH, _sk_goTo, _sk_pushBall);
    addTransition(STATE_GOTOPOS, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTOPOS, _sk_pushBall, _sk_goTo);
}

void Behaviour_TakeFoul::run(){
    //initializing position to look
    Position posToLook = loc()->theirGoal();
    Position behindBall;

    //if we haven't found a positionToLook yet
    if(canChangePosToLook){
        quint8 gkid = 100;
        //find their goalkeeper
        for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
            if(PlayerBus::theirPlayerAvailable(x) && isReallyInsideTheirArea(x)){
                gkid = x;
                break;
            }
        }

        //try to project next goalkeeper position based on its velocity and actual position
        if(PlayerBus::theirPlayerAvailable(gkid)){
            Position gkPos = PlayerBus::theirPlayer(gkid)->position();
            //considering ball velocity to define player position
            if(PlayerBus::theirPlayer(gkid)->velocity().abs() > GKPREVISION_MINVELOCITY){
                //calc unitary vector of velocity
                const Position velUni(true, PlayerBus::theirPlayer(gkid)->velocity().x()/PlayerBus::theirPlayer(gkid)->velocity().abs(), PlayerBus::theirPlayer(gkid)->velocity().y()/PlayerBus::theirPlayer(gkid)->velocity().abs(), 0.0);

                //calc velocity factor
                float factor = GKPREVISION_VELOCITY_FACTOR*PlayerBus::theirPlayer(gkid)->velocity().abs();
                WR::Utils::limitValue(&factor, 0.0f, GKPREVISION_FACTOR_LIMIT);

                //calc projected position
                const Position delta(true, factor*velUni.x(), factor*velUni.y(), 0.0);
                Position projectedPos(true, PlayerBus::theirPlayer(gkid)->position().x()+delta.x(), PlayerBus::theirPlayer(gkid)->position().y()+delta.y(), 0.0);
                gkPos = projectedPos;
            }

            Position gkPosProjectedOnGoal;
            gkPosProjectedOnGoal = Position(true, loc()->theirGoal().x(), gkPos.y(), 0);
            //check the best point to look (identify the farest post from goalkeeper and take a point far from it and near the post)
            if(WR::Utils::distance(gkPosProjectedOnGoal, loc()->theirGoalLeftPost()) > WR::Utils::distance(gkPosProjectedOnGoal, loc()->theirGoalRightPost())){
                float distToPost = WR::Utils::distance(gkPosProjectedOnGoal, loc()->theirGoalLeftPost());
                if(loc()->theirSide().isLeft()){
                    posToLook = Position(true, loc()->theirGoal().x(), (gkPosProjectedOnGoal.y() - 0.7f*distToPost), 0);
                }else{
                    posToLook = Position(true, loc()->theirGoal().x(), (gkPosProjectedOnGoal.y() + 0.7f*distToPost), 0);
                }
            }else{
                float distToPost = WR::Utils::distance(gkPosProjectedOnGoal, loc()->theirGoalRightPost());
                if(loc()->theirSide().isLeft()){
                    posToLook = Position(true, loc()->theirGoal().x(), (gkPosProjectedOnGoal.y() + 0.7f*distToPost), 0);
                }else{
                    posToLook = Position(true, loc()->theirGoal().x(), (gkPosProjectedOnGoal.y() - 0.7f*distToPost), 0);
                }
            }
        }
    }
    behindBall = WR::Utils::threePoints(loc()->ball(), posToLook, 0.12f, GEARSystem::Angle::pi);

    //setting skill goto
    _sk_goTo->setAvoidBall(false);
    _sk_goTo->setAvoidOpponents(false);
    _sk_goTo->setAvoidTeammates(false);
    _sk_goTo->setAvoidTheirGoalArea(false);
    _sk_goTo->setGoToPos(behindBall);
    _sk_goTo->setMinVelocity(0.7f);

    //setting skill rotateto
    _sk_rotateTo->setDesiredPosition(loc()->ball());

    //transitions
    if(_skill == GOTO){
        //std::cout<< "GO" << std::endl;
        if(player()->isNearbyPosition(behindBall, 0.015f)){
            counter = 0;
            enableTransition(STATE_ROTATE);
            _skill = ROT;
        }
    }else if(_skill == ROT){
        //in case our robot never find the right angle to the desired position: set push
        if(counter <= 180) counter++;
        //std::cout<< "ROT" << std::endl;
        canChangePosToLook = false;
        if(localIsLookingTo(loc()->ball(), 0.04f) || counter > 180){
            //setting skill pushball
            if(player()->isLookingTo(loc()->ball(), 0.1f)){
                _sk_pushBall->setSpeedAndOmega(2.0, 0.0);
            }else{
                _sk_pushBall->setSpeedAndOmega(-2.0, 0.0);
            }
            enableTransition(STATE_PUSH);
            _skill = PUSH;
        }
    }else if(_skill == PUSH){
        //std::cout<< "PUSH" << std::endl;
        if(!player()->isNearbyPosition(loc()->ball(), 0.3f)){
            enableTransition(STATE_GOTOPOS);
            _skill = GOTO;
        }
    }
}

bool Behaviour_TakeFoul::isReallyInsideTheirArea(quint8 id){
    if(loc()->theirSide().isRight()){
        if((PlayerBus::theirPlayer(id)->position().x() >= (loc()->fieldMaxX() - 0.15f)) && (abs(PlayerBus::theirPlayer(id)->position().y()) <= loc()->fieldDefenseLength()/2)){
            return true;
        }else{
            return false;
        }
    }else{
        if((PlayerBus::theirPlayer(id)->position().x() <= -1*(loc()->fieldMaxX() - 0.15f)) && (abs(PlayerBus::theirPlayer(id)->position().y()) <= loc()->fieldDefenseLength()/2)){
            return true;
        }else{
            return false;
        }
    }
}

bool Behaviour_TakeFoul::localIsLookingTo(const Position &pos, float error){
    Angle angle1(true, WR::Utils::getAngle(player()->position(), pos));

    // Calc diff
    float dif = abs(WR::Utils::angleDiff(player()->orientation(), angle1));
    if(dif > Angle::pi/2) dif = abs(Angle::pi - dif);
    return (dif <= error);
}
