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
#include "role_striker.h"

QString Role_Striker::name(){
    return "Role_Striker";
}
Role_Striker::Role_Striker()
{
    _bh_at = nullptr;
}
void Role_Striker::initializeBehaviours(){
    usesBehaviour(BHV_AT, _bh_at = new Behaviour_Attacker());
    usesBehaviour(BHV_DONOTHING, _bh_dn = new Behaviour_DoNothing());
    usesBehaviour(BHV_TAKEFOUL, _bh_tf = new Behaviour_TakeFoul());
}
void Role_Striker::configure(){

}

void Role_Striker::run(){
    if(!canGoBackToNormalGame){
        if(timer.timesec() > 4){
            timer.stop();
            canGoBackToNormalGame = true;
        }
        return;
    }else{
        setBehaviour(BHV_AT);
    }
}

bool Role_Striker::isReallyInsideTheirArea(quint8 id){
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

void Role_Striker::penaltyKick(Position* pos, Angle* ang){
    float defenseYabs = 0.165f;
    float nearMiddleXabs = 0.2f;
    Position penaltyRightMark(true, 0.375, 0, 0), penaltyLeftMark(true, -0.375, 0, 0);

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    isNormalGame = false;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::PENALTY_KICK;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

    //find their goalkeeper
    Position gkPos;
    quint8 gkid = 100;
    for(quint8 x = 0; x < VSSConstants::qtPlayers(); x++){
        if(PlayerBus::theirPlayerAvailable(x)){
            if(isReallyInsideTheirArea(x)){
                gkid = x;
                break;
            }
        }
    }
    if(!PlayerBus::theirPlayerAvailable(gkid)) gkPos = loc()->theirGoal();
    else gkPos = PlayerBus::theirPlayer(gkid)->position();

    //if we should take the penalty kick foul
    if(weTake){
        //if our side is right
        if(loc()->ourSide().isRight()){
            Position ourPos;
            if(gkPos.y()>0){
                ourPos = WR::Utils::threePoints(penaltyLeftMark, loc()->theirGoalLeftMidPost(), 0.1f, Angle::pi);
                *pos = ourPos;
            }else{
                ourPos = WR::Utils::threePoints(penaltyLeftMark, loc()->theirGoalRightMidPost(), 0.1f, Angle::pi);
                *pos = ourPos;
            }
            *ang = WR::Utils::getAngle(ourPos, penaltyLeftMark);
        }
        //if our side is left
        else{
            Position ourPos;
            if(gkPos.y()>0){
                ourPos = WR::Utils::threePoints(penaltyRightMark, loc()->theirGoalRightMidPost(), 0.1f, Angle::pi);
                *pos = ourPos;
            }else{
                ourPos = WR::Utils::threePoints(penaltyRightMark, loc()->theirGoalLeftMidPost(), 0.1f, Angle::pi);
                *pos = ourPos;
            }
            *ang = WR::Utils::getAngle(ourPos, penaltyRightMark);
        }
    }
    //if we should not take the penalty kick
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

void Role_Striker::goalKick(Position *pos, Angle *ang){
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
    //if we aren't charging the goal kick foul
    else{
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

void Role_Striker::gameOn(){
    if(!isNormalGame){
        canGoBackToNormalGame = false;
        timer.start();
        if(weTake){
            setBehaviour(BHV_TAKEFOUL);
        }else{
            setBehaviour(BHV_DONOTHING);
        }
    }else{
        canGoBackToNormalGame = true;
    }
}

void Role_Striker::freeBall(Position *pos, Angle *ang, VSSRef::Quadrant quadrant){
    float freeBallXabs = (loc()->fieldMaxX() - 0.375), freeBallYabs = (loc()->fieldMaxY() - 0.25f), freeBallOffset = 0.2f;

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    isNormalGame = true;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::FREE_BALL;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

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

void Role_Striker::kickOff(Position *pos, Angle *ang){
    float assistPosOffsetX = 0.25f;

    //update isNormalGame variable (to false if foul is different from KICKOFF, STOP, GAME_ON or GOALKICK - because this role doesn't take this foul)
    if(weTake) isNormalGame = true;
    else isNormalGame = false;
    //update lastFoul variable (last foul different from GAME_ON, STOP or KICK_OFF)
    lastFoul = VSSRef::KICKOFF;
    //set behaviour doNothing so our player doesn't move from position emmited
    setBehaviour(BHV_DONOTHING);

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

bool Role_Striker::ourTeamShouldTake(VSSRef::Color teamColor){
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

void Role_Striker::receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor){
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
