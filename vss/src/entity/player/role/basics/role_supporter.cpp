#include "role_supporter.h"

QString Role_Supporter::name(){
    return "Role_Supporter";
}
Role_Supporter::Role_Supporter()
{
    _bh_br = nullptr;
    _bh_as = nullptr;
    _bh_dn = nullptr;
    _bh_tf = nullptr;

}
void Role_Supporter::initializeBehaviours(){
    usesBehaviour(BHV_ASSISTANT, _bh_as = new Behaviour_Assistant());
    usesBehaviour(BHV_BARRIER , _bh_br = new Behaviour_Barrier());
    usesBehaviour(BHV_DONOTHING, _bh_dn = new Behaviour_DoNothing());
    usesBehaviour(BHV_TAKEFOUL, _bh_tf = new Behaviour_TakeFoul());
}
void Role_Supporter::configure(){
    isNormalGame = true;
    canGoBackToNormalGame = true;
    lastFoul = VSSRef::KICKOFF;
    weTake = true;
    counter = 0;

    if(_positioning == BARRIER_PREDOMINANT){
        setBehaviour(BHV_BARRIER); //initial behaviour
        _bhv = BHV_BARRIER;
    }else if(_positioning == ASSIST_PREDOMINANT){
        setBehaviour(BHV_ASSISTANT);
        _bhv = BHV_ASSISTANT;
    }else{
        setBehaviour(BHV_BARRIER);
        _bhv = BHV_BARRIER;
    }
}
void Role_Supporter::run(){
    if(!canGoBackToNormalGame){
        if(counter >= 300){
            counter = 0;
            canGoBackToNormalGame = true;
        }else{
            counter++;
        }
        return;
    }

    switch(_positioning){
    case(FREE):{
        if(canGoBackToNormalGame){
            float enemyInOurFieldFactor = 0;
            if(EnemyNear(0.2f)) enemyInOurFieldFactor = 0.75f;
            float distBallFactor = 1.0f - loc()->distBallOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f));
            float distPlayerFactor = 1.0f - player()->distOurGoal()/WR::Utils::distance(loc()->fieldLeftTopCorner(), Position(true, loc()->fieldMaxX(), 0.0f, 0.0f));
            float ballVelFactor = loc()->ballVelocity().abs()/1.2f;

            float avgFactor = (distBallFactor + distPlayerFactor + ballVelFactor + enemyInOurFieldFactor)/4.0f;
            if(avgFactor > 0.5f && _bhv == BHV_ASSISTANT){
                setBehaviour(BHV_BARRIER);
                _bhv = BHV_BARRIER;
            }else if(avgFactor <= 0.5f && _bhv == BHV_BARRIER){
                setBehaviour(BHV_ASSISTANT);
                _bhv = BHV_ASSISTANT;
            }
        }
        break;
    }
    case(BARRIER_PREDOMINANT):{
        if(canGoBackToNormalGame){
            setBehaviour(BHV_BARRIER);
            if((!EnemyNear(0.2f) && player()->isNearbyPosition(loc()->ball(), 0.15f) && player()->isNearbyPosition(loc()->ourGoal(), 0.4f) && !BySideOfGoal() && (player()->distOurGoal() < WR::Utils::distance(loc()->ball(), loc()->ourGoal())))
                    || (!EnemyNear(player()->distBall() + 0.1f) && player()->distBall() < distAllyToBall())){
                emit sendSignal();
            }
        }
        break;
    }
    case(ASSIST_PREDOMINANT):{
        if(canGoBackToNormalGame){
            setBehaviour(BHV_ASSISTANT);
        }
        break;
    }
    }
}

float Role_Supporter::distAllyToBall(){
    quint8 gkId = 100, allyId = 100;
    float minDist = 100;
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::ourPlayerAvailable(x)){
            if(PlayerBus::ourPlayer(x)->distOurGoal() < minDist && PlayerBus::ourPlayer(x)->playerId() != player()->playerId()){
                allyId = gkId;
                gkId = PlayerBus::ourPlayer(x)->playerId();
                minDist = PlayerBus::ourPlayer(x)->distOurGoal();
            }else{
                allyId = PlayerBus::ourPlayer(x)->playerId();
            }
        }
    }
    return PlayerBus::ourPlayer(allyId)->distBall();
}

bool Role_Supporter::EnemyNear(float minDist){
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x)){
            if(PlayerBus::theirPlayer(x)->isNearbyPosition(loc()->ball(), minDist)){
                return true;
            }
        }
    }
    return false;
}

bool Role_Supporter::BySideOfGoal(){
    if(fabs(loc()->ball().y()) > (fabs(loc()->ourGoal().y()) + loc()->fieldDefenseLength()/2.0f)){
        return true;
    }else{
        return false;
    }
}

void Role_Supporter::setPositioning(int positioning){
    if(positioning == FREE){
        _positioning = FREE;
    }else if(positioning == BARRIER_PREDOMINANT){
        _positioning = BARRIER_PREDOMINANT;
    }else if(positioning == ASSIST_PREDOMINANT){
        _positioning = ASSIST_PREDOMINANT;
    }
}

bool Role_Supporter::ourTeamShouldTake(VSSRef::Color teamColor){
    if(player()->team()->teamColor() == Colors::Color::BLUE){
        if(teamColor == VSSRef::Color::BLUE) return true;
        else if(teamColor == VSSRef::Color::YELLOW) return false;
        else return true;
    }else if(player()->team()->teamColor() == Colors::Color::YELLOW){
        if(teamColor == VSSRef::Color::YELLOW) return true;
        else if(teamColor == VSSRef::Color::BLUE) return false;
        else return true;
    }else{
        return true;
    }
}

void Role_Supporter::penaltyKick(Position* pos, Angle* ang){
    float defenseXabs = (loc()->fieldMaxX() - 0.18f), defenseYabs = 0.165f;
    float takeXabs = (loc()->fieldMaxX()/2 - 0.2f), takeYabs = 0;
    float nearMiddleXabs = 0.2f;

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    isNormalGame = false;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::PENALTY_KICK;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    //if we should take the penalty kick foul
    if(weTake){
        //if this player is playing as assistant: put it in the penalty kick position and looking to their goal
        if(_positioning == ASSIST_PREDOMINANT){
            //if our side is right
            if(loc()->ourSide().isRight()){
                *pos = Position(true, -1*takeXabs, takeYabs, 0);
                *ang = Angle(true, Angle::pi);
            }
            //if our side is left
            else{
                *pos = Position(true, takeXabs, takeYabs, 0);
                *ang = Angle(true, 0);
            }
        }
        //if this player isn't playing as assistant: put it in our field in front of our goal
        else{
            //if our side is right
            if(loc()->ourSide().isRight()){
                *pos = Position(true, defenseXabs, 0, 0);
                *ang = Angle(true, Angle::pi);
            }
            //if our side is left
            else{
                *pos = Position(true, -1*defenseXabs, 0, 0);
                *ang = Angle(true, 0);
            }
        }
    }
    //if we should not take the penalty kick
    else{
        //if this player is playing as barrier: put it near the middle of the field and in the left side of our goal (not in our field side)
        if(_positioning == BARRIER_PREDOMINANT){
            //if our side is right
            if(loc()->ourSide().isRight()){
                *pos = Position(true, -1 * nearMiddleXabs, defenseYabs, 0);
                *ang = Angle(true, 0);
            }
            //if our side is left
            else{
                *pos = Position(true, nearMiddleXabs, -1*defenseYabs, 0);
                *ang = Angle(true, Angle::pi);
            }
        }
        //if this player isn't playing as barrier: put it near the middle of the field and in the right side of our goal (not in our field side)
        else{
            //if our side is right
            if(loc()->ourSide().isRight()){
                *pos = Position(true, -1 * nearMiddleXabs, -1*defenseYabs, 0);
                *ang = Angle(true, 0);
            }
            //if our side is left
            else{
                *pos = Position(true, nearMiddleXabs, defenseYabs, 0);
                *ang = Angle(true, Angle::pi);
            }
        }
    }
}

void Role_Supporter::goalKick(Position *pos, Angle *ang){
    float defenseXabs = (loc()->fieldMaxX() - 0.18f), defenseYabs = 0.3f;
    float nearTheMiddleXabs = (loc()->fieldMaxX()/2 - 0.15f);

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    isNormalGame = true;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::GOAL_KICK;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    //if we are charging the goal kick foul
    if(weTake){
        if(_positioning == BARRIER_PREDOMINANT){
            //if our side is right
            if(loc()->ourSide().isRight()){
                //barrier always on the left side of our goal
                *pos = Position(true, defenseXabs, defenseYabs, 0);
                *ang = Angle(true, 0);
            }else{
                //barrier always on the left side of our goal
                *pos = Position(true, -1*defenseXabs, -1*defenseYabs, 0);
                *ang = Angle(true, Angle::pi);
            }
        }else{
            //if our side is left
            if(loc()->ourSide().isRight()){
                //player not playing as barrier always on the right side of our goal
                *pos = Position(true, defenseXabs, -1*defenseYabs, 0);
                *ang = Angle(true, 0);
            }else{
                //player not playing as barrier always on the right side of our goal
                *pos = Position(true, -1*defenseXabs, defenseYabs, 0);
                *ang = Angle(true, Angle::pi);
            }
        }
    }
    //if we aren't charging the goal kick foul
    else{
        if(_positioning == BARRIER_PREDOMINANT){
            //barrier always in front of our goal
            if(loc()->ourSide().isRight()){
                *pos = Position(true, defenseXabs, 0, 0);
                *ang = Angle(true, Angle::pi);
            }else{
                *pos = Position(true, -1*defenseXabs, 0, 0);
                *ang = Angle(true, 0);
            }
        }else{
            //assistant always in front of our goal and near the middle of the field
            if(loc()->ourSide().isRight()){
                *pos = Position(true, nearTheMiddleXabs, 0, 0);
                *ang = Angle(true, Angle::pi);
            }else{
                *pos = Position(true, -1*nearTheMiddleXabs, 0, 0);
                *ang = Angle(true, 0);
            }
        }
    }
}

void Role_Supporter::freeBall(Position *pos, Angle *ang, VSSRef::Quadrant quadrant){
    float defenseXabs = (loc()->fieldMaxX() - 0.18f), defenseYabs = 0.165f;
    float freeBallXabs = (loc()->fieldMaxX() - 0.375), freeBallYabs = (loc()->fieldMaxY() - 0.25f), freeBallOffset = 0.2f;

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    isNormalGame = true;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::FREE_BALL;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    //if this player is playing as barrier
    if(_positioning == BARRIER_PREDOMINANT){
        if(quadrant == VSSRef::QUADRANT_1){
            if(loc()->ourSide().isRight()){
                //player in quadrant 4 near our goal
                *pos = Position(true, defenseXabs, -1*defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }else{
                //player in quadrant 2 near our goal
                *pos = Position(true, -1*defenseXabs, defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }
        }else if(quadrant == VSSRef::QUADRANT_2){
            if(loc()->ourSide().isRight()){
                //player in quadrant 1 near our goal
                *pos = Position(true, defenseXabs, defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }else{
                //player in quadrant 3 near our goal
                *pos = Position(true, -1*defenseXabs, -1*defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }
        }else if(quadrant == VSSRef::QUADRANT_3){
            if(loc()->ourSide().isRight()){
                //player in quadrant 4 near our goal
                *pos = Position(true, defenseXabs, -1*defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }else{
                //player in quadrant 2 near our goal
                *pos = Position(true, -1*defenseXabs, defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }
        }else if(quadrant == VSSRef::QUADRANT_4){
            if(loc()->ourSide().isRight()){
                //player in quadrant 1 near our goal
                *pos = Position(true, defenseXabs, defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }else{
                //player in quadrant 3 near our goal
                *pos = Position(true, -1*defenseXabs, -1*defenseYabs, 0);
                *ang = Angle(true, float(M_PI_2));
            }
        }
    }
    //if this player isn't playing as barrier: it is going to take the free ball foul
    else{
        if(quadrant == VSSRef::QUADRANT_1){
            if(loc()->ourSide().isRight()){
                //player in quadrant 1 near our goal (right)
                *pos = Position(true, (freeBallXabs + freeBallOffset), freeBallYabs, 0);
                *ang = Angle(true, Angle::pi);
            }else{
                //player in quadrant 1 near our goal (left)
                *pos = Position(true, (freeBallXabs - freeBallOffset), freeBallYabs, 0);
                *ang = Angle(true, 0);
            }
        }else if(quadrant == VSSRef::QUADRANT_2){
            if(loc()->ourSide().isRight()){
                //player in quadrant 2 near our goal (right)
                *pos = Position(true, (-1*freeBallXabs + freeBallOffset), freeBallYabs, 0);
                *ang = Angle(true, Angle::pi);
            }else{
                //player in quadrant 2 near our goal (left)
                *pos = Position(true, (-1*freeBallXabs - freeBallOffset), freeBallYabs, 0);
                *ang = Angle(true, 0);
            }
        }else if(quadrant == VSSRef::QUADRANT_3){
            if(loc()->ourSide().isRight()){
                //player in quadrant 3 near our goal (right)
                *pos = Position(true, (-1*freeBallXabs + freeBallOffset), -1*freeBallYabs, 0);
                *ang = Angle(true, Angle::pi);
            }else{
                //player in quadrant 3 near our goal (left)
                *pos = Position(true, (-1*freeBallXabs - freeBallOffset), -1*freeBallYabs, 0);
                *ang = Angle(true, 0);
            }
        }else if(quadrant == VSSRef::QUADRANT_4){
            if(loc()->ourSide().isRight()){
                //player in quadrant 4 near our goal (right)
                *pos = Position(true, (freeBallXabs + freeBallOffset), -1*freeBallYabs, 0);
                *ang = Angle(true, Angle::pi);
            }else{
                //player in quadrant 4 near our goal (left)
                *pos = Position(true, (freeBallXabs - freeBallOffset), -1*freeBallYabs, 0);
                *ang = Angle(true, 0);
            }
        }
    }
}

void Role_Supporter::kickOff(Position *pos, Angle *ang){
    float barrPosOffsetX = 0.25f, assistPosOffsetX = 0.25f;

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    if(weTake) isNormalGame = true;
    else isNormalGame = false;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::KICKOFF;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    if(_positioning == BARRIER_PREDOMINANT){
        //if our side is right
        if(loc()->ourSide().isRight()){
            //barrier near our goal (in front of it)
            *pos = Position(true, (loc()->fieldMaxX() - barrPosOffsetX), 0, 0);
            *ang = Angle(true, Angle::pi);
        }else{
            //barrier near our goal (in front of it)
            *pos = Position(true, (loc()->fieldMinX() + barrPosOffsetX), 0, 0);
            *ang = Angle(true, 0);
        }
    }else{
        //if our side is left
        if(loc()->ourSide().isRight()){
            //assistant near the middle of the field
            *pos = Position(true, assistPosOffsetX, 0, 0);
            *ang = Angle(true, Angle::pi);
        }else{
            //assistant near the middle of the field
            *pos = Position(true, -1*assistPosOffsetX, 0, 0);
            *ang = Angle(true, 0);
        }
    }
}

void Role_Supporter::gameOn(){
    if(!isNormalGame){
        canGoBackToNormalGame = false;
        if(weTake){
            setBehaviour(BHV_TAKEFOUL);
        }else{
            setBehaviour(BHV_DONOTHING);
        }
    }else{
        canGoBackToNormalGame = true;
    }
}

void Role_Supporter::receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor){
    if(isInitialized() && player() != NULL){
        weTake = ourTeamShouldTake(teamColor);
        Position pos;
        Angle ang;
        //PENALTY
        if(foul == VSSRef::PENALTY_KICK){
            penaltyKick(&pos, &ang);
            emit emitPosition(player()->playerId(), pos, ang);
        }
        //GOAL KICK
        else if(foul == VSSRef::GOAL_KICK){
            goalKick(&pos, &ang);
            emit emitPosition(player()->playerId(), pos, ang);
        }
        //STOP
        else if(foul == VSSRef::STOP){
            //set behaviour doNothing
            setBehaviour(BHV_DONOTHING);
        }
        //GAME ON
        else if(foul == VSSRef::GAME_ON){
            gameOn();
        }
        //FREE BALL
        else if(foul == VSSRef::FREE_BALL){
            freeBall(&pos, &ang, quadrant);
            emit emitPosition(player()->playerId(), pos, ang);
        }
        //KICK OFF
        else if(foul == VSSRef::KICKOFF){
            kickOff(&pos, &ang);
            emit emitPosition(player()->playerId(), pos, ang);
        }
    }
}
