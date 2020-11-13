/***
 * Maracatronics Robotics
 * Federal University of Pernambuco (UFPE) at Recife
 * http://www.maracatronics.com/
 *
 * This file is part of Armorial project.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ***/

#include "role_goalkeeper.h"



QString Role_Goalkeeper::name(){
    return "Role_Goalkeeper";
}

Role_Goalkeeper::Role_Goalkeeper() {
    _bh_gk = nullptr;
    _bh_dn = nullptr;
    _bh_tf = nullptr;
    _bh_pb = nullptr;
}

void Role_Goalkeeper::initializeBehaviours(){
    usesBehaviour(BHV_GK, _bh_gk = new Behaviour_Goalkeeper());
    usesBehaviour(BHV_DONOTHING, _bh_dn = new Behaviour_DoNothing());
    usesBehaviour(BHV_TAKEFOUL, _bh_tf = new Behaviour_TakeFoul());
    usesBehaviour(BHV_PUSH, _bh_pb = new Behaviour_PushBall());
}

void Role_Goalkeeper::configure(){
    isGK(true);
    isNormalGame = true;
    canGoBackToNormalGame = true;
    lastFoul = VSSRef::KICKOFF;
    weTake = true;
    _backUp = false;
    limitTimer = 2.0f;
}

void Role_Goalkeeper::run(){
    if (_backUp) {
        timerGetOut.stop();
        if (timerGetOut.timesec() > 0.5) {
            _backUp = false;
        }
    } else isGKStucked();

    if((canGoBackToNormalGame || abs(player()->position().x()) <= (loc()->fieldMaxX() - loc()->fieldDefenseWidth() - 0.1f)) && !_backUp){
        setBehaviour(BHV_GK);
    }else{
        timer.stop();
        if(timer.timesec() > limitTimer){
            canGoBackToNormalGame = true;
        }
    }
}

void Role_Goalkeeper::penaltyKick(Position* pos, Angle* ang){
    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON, PENALTY)
    isNormalGame = false;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::PENALTY_KICK;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);
    //if it's a penalty against us
    if(!weTake) flagPenalti = 1;
    else flagPenalti = 0;

    //put our goalkeeper in the middle of the goal
    //if our side is right
    if(loc()->ourSide().isRight()){
        *pos = Position(true, loc()->fieldMaxX() - 0.05f, 0, 0);
        *ang = Angle(true, 0);
    }
    //if our side is left
    else{
        *pos = Position(true, loc()->fieldMinX() + 0.05f, 0, 0);
        *ang = Angle(true, 0);
    }

}

void Role_Goalkeeper::goalKick(Position* pos, Angle* ang){
    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON, PENALTY)
    isNormalGame = false;
    //update lastFoul variable (last foul different from GAME_ON, STOP)
    lastFoul = VSSRef::GOAL_KICK;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    if(weTake){
        //put our goalkeeper in the middle of the goal
        //if our side is right
        float distXBall = 0.15f, distFromCorner = 0.1f;
        if(loc()->ourSide().isRight()){
            Position ourAreaPostLeft(true, loc()->ourGoal().x(), loc()->ourGoal().y() + 0.2f, 0);
            Position ourAreaPostRight(true, loc()->ourGoal().x(), loc()->ourGoal().y() - 0.2f, 0);
            //goalkeeper has y < 0: put it to look down
            if(player()->position().y() < 0){
                *pos = WR::Utils::threePoints(Position(true, loc()->fieldMaxX()-distXBall, -0.35f, 0), ourAreaPostRight, distFromCorner, 0);
                *ang = WR::Utils::getAngle(*pos, Position(true, loc()->fieldMaxX()-distXBall, -0.35f, 0));
            }
            //goalkeeper has y >= 0: put it to look up
            else{
                *pos = WR::Utils::threePoints(Position(true, loc()->fieldMaxX()-distXBall, 0.35f, 0), ourAreaPostLeft, distFromCorner, 0);
                *ang = WR::Utils::getAngle(*pos, Position(true, loc()->fieldMaxX()-distXBall, 0.35f, 0));
            }
        }
        //if our side is left
        else{
            Position ourAreaPostLeft(true, loc()->ourGoal().x(), loc()->ourGoal().y() - 0.2f, 0);
            Position ourAreaPostRight(true, loc()->ourGoal().x(), loc()->ourGoal().y() + 0.2f, 0);
            //goalkeeper has y < 0: put it to look down
            if(player()->position().y() < 0){
                *pos = WR::Utils::threePoints(Position(true, loc()->fieldMinX()+distXBall, -0.35f, 0), ourAreaPostLeft, distFromCorner, 0);
                *ang = WR::Utils::getAngle(*pos, Position(true, loc()->fieldMinX()+distXBall, -0.35f, 0));
            }
            //goalkeeper has y >= 0: put it to look up
            else{
                *pos = WR::Utils::threePoints(Position(true, loc()->fieldMinX()+distXBall, 0.35f, 0), ourAreaPostRight, distFromCorner, 0);
                *ang = WR::Utils::getAngle(*pos, Position(true, loc()->fieldMinX()+distXBall, 0.35f, 0));
            }
        }
    }else{
        if(loc()->ourSide().isLeft()){
            *pos = Position(true, loc()->fieldMinX() + 0.07f, 0, 0);
            *ang = Angle(true, float(M_PI_2));
        }else{
            *pos = Position(true, loc()->fieldMaxX() - 0.07f, 0, 0);
            *ang = Angle(true, float(M_PI_2));
        }
    }
}

void Role_Goalkeeper::kickOff(Position* pos, Angle* ang){
    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON, PENALTY)
    isNormalGame = true;
    //update lastFoul variable (last foul different from GAME_ON, STOP)
    lastFoul = VSSRef::KICKOFF;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    //put our goalkeeper in the middle of the goal
    //if our side is right
    if(loc()->ourSide().isRight()){
        *pos = Position(true, loc()->fieldMaxX() - 0.05f, 0, 0);
        *ang = Angle(true, float(M_PI_2));
    }
    //if our side is left
    else{
        *pos = Position(true, loc()->fieldMinX() + 0.05f, 0, 0);
        *ang = Angle(true, float(M_PI_2));
    }
}

void Role_Goalkeeper::freeBall(Position *pos, Angle *ang, VSSRef::Quadrant quadrant){
    float gkYabs = 0.04f;

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON, PENALTY)
    isNormalGame = true;
    //update lastFoul variable (last foul different from GAME_ON, STOP)
    lastFoul = VSSRef::FREE_BALL;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    //put our goalkeeper in the middle of the goal
    //if our side is right
    if(loc()->ourSide().isRight()){
        if(quadrant == VSSRef::QUADRANT_1){
            *pos = Position(true, loc()->fieldMaxX() - 0.05f, -1*gkYabs, 0);
            *ang = Angle(true, float(M_PI_2));
        }else if(quadrant == VSSRef::QUADRANT_4){
            *pos = Position(true, loc()->fieldMaxX() - 0.05f, gkYabs, 0);
            *ang = Angle(true, float(M_PI_2));
        }else{
            *pos = Position(true, loc()->fieldMaxX() - 0.05f, 0, 0);
            *ang = Angle(true, float(M_PI_2));
        }
    }
    //if our side is left
    else{
        if(quadrant == VSSRef::QUADRANT_2){
            *pos = Position(true, loc()->fieldMinX() + 0.05f, -1*gkYabs, 0);
            *ang = Angle(true, float(M_PI_2));
        }else if(quadrant == VSSRef::QUADRANT_3){
            *pos = Position(true, loc()->fieldMinX() + 0.05f, gkYabs, 0);
            *ang = Angle(true, float(M_PI_2));
        }else{
            *pos = Position(true, loc()->fieldMinX() + 0.05f, 0, 0);
            *ang = Angle(true, float(M_PI_2));
        }
    }
}

bool Role_Goalkeeper::ourTeamShouldTake(VSSRef::Color teamColor){
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

void Role_Goalkeeper::gameOn(){
    if(!isNormalGame){
        canGoBackToNormalGame = false;
        timer.start();
        if(weTake){
            limitTimer = 2;
            setBehaviour(BHV_TAKEFOUL);
        }else{
            //if penalty against us
            if(flagPenalti){
                limitTimer = 1;
                _bh_pb->setAimPosition(loc()->ball());
                setBehaviour(BHV_PUSH);
            }
            limitTimer = 2;
            setBehaviour(BHV_DONOTHING);
        }
    }else{
        canGoBackToNormalGame = true;
    }
}

void Role_Goalkeeper::isGKStucked(){
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::ourPlayerAvailable(x)){
            if(PlayerBus::ourPlayer(x)->playerId() != player()->playerId()){
                // 0.084 é a distância de um robô aoutro quando este último está com ceucentro apontado para o lado do outro
                if(player()->distanceTo(PlayerBus::ourPlayer(x)->position()) < 0.084f && player()->distBall() > 0.75f){
                    emit getOut(x, player()->playerId());
                }
            }
        }
    }
}

void Role_Goalkeeper::receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor){
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

void Role_Goalkeeper::backUp(quint8 playerId) {
    if (!_backUp) {
        timerGetOut.start();
        _backUp = true;
        if (PlayerBus::ourPlayerAvailable(playerId)) {
            Position aim = PlayerBus::ourPlayer(playerId)->position();
            _bh_pb->setAimPosition(aim);
        }
        setBehaviour(BHV_PUSH);
    }
}
