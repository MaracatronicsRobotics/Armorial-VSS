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

#include "behaviour_attacker.h"
#define R 1.0f

QString Behaviour_Attacker::name() {
    return "Behaviour_Attacker";
}

Behaviour_Attacker::Behaviour_Attacker() {
    _sk_goTo = nullptr;
    _sk_rotateTo = nullptr;
    _sk_pushBall = nullptr;
}

void Behaviour_Attacker::configure() {
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_Spin = new Skill_Spin());

    addTransition(STATE_ROTATETO, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_ROTATETO, _sk_Spin, _sk_rotateTo);

    addTransition(STATE_GOTO, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTO, _sk_Spin , _sk_goTo);

    addTransition(STATE_SPIN , _sk_goTo , _sk_Spin);
    addTransition(STATE_SPIN , _sk_rotateTo , _sk_Spin);

    setInitialSkill(_sk_goTo);
};

void Behaviour_Attacker::run() {
    CheckIfAttack();
    switch(_state){
    case STATE_WAIT:{
        //hold the position
        Position posWait(true , 0.375f , 0.0f , 0.0f);
        _sk_goTo->setGoToPos(posWait);
        enableTransition(STATE_GOTO);
        break;
    }
    case STATE_ATTACK:{
        //goto behind
        Position behindBall = WR::Utils::threePoints(loc()->ball(), loc()->theirGoal(), 0.1f, GEARSystem::Angle::pi);
        double angleToGoal = WR::Utils::getAngle(loc()->ball() , loc()->theirGoal());
        double tg = tan(angleToGoal);
        if(!loc()->isOutsideField(behindBall)){
            //if(player()->playerId() == 2) std::cout << "tá behind: " << isBehindBall(loc()->theirGoal()) << std::endl;
            if(isBehindBall(loc()->theirGoal()) ){
                Position DesiredBall = WR::Utils::threePoints(player()->position(), loc()->ball(), 0.4f, 0.0f);
                //std::cout << "[BHV_ATTACKER]: " << DesiredBall.x() << "," << DesiredBall.y() << std::endl;

                if(isInsideAreaBehindBall(behindBall) ){
                     //std::cout << "[BHV_ATTACKER]: " <<"Entrou na BehindBallArea" << std::endl;


                    if(tg <= - 0.1f || tg >= 0.1f ){//gira o goto em direçao ao gol
                        if(tg>0){
                            DesiredBall.setPosition(DesiredBall.x() , DesiredBall.y() + 0.1 , DesiredBall.z());
                        }
                        else {
                            DesiredBall.setPosition(DesiredBall.x() , DesiredBall.y() - 0.1 , DesiredBall.z());
                        }
                    }
                    else{
                        DesiredBall.setPosition(DesiredBall.x() , DesiredBall.y() + tg , DesiredBall.z());
                    }

                    if(isInsideProjectArea(DesiredBall)){ //threePoints apontando mt longe
                         std::cout << "[BHV_ATTACKER]: " <<"Ta olhando pro gol" << std::endl; //ele entra nessa  mas o goto nao ta indo
                         _sk_goTo->setGoToPos(DesiredBall);
                         _sk_goTo->setGoToVelocityFactor(2.0f);
                         _sk_goTo->setAvoidBall(false);
                         enableTransition(STATE_GOTO);
                    }
                    else{
                        std::cout << "[BHV_ATTACKER]: " <<"Ta olhando pra fora" << std::endl;
                        _sk_goTo->setGoToPos(DesiredBall);
                        _sk_goTo->setGoToVelocityFactor(2.0f);
                        _sk_goTo->setAvoidBall(false);
                        enableTransition(STATE_GOTO);
                    }
                }


            }
            else{
                std::cout <<"Not behind ball" << std::endl;
                _sk_goTo->setGoToPos(behindBall);
                _sk_goTo->setGoToVelocityFactor(3.0f);
                _sk_goTo->setAvoidBall(true);
                enableTransition(STATE_GOTO);
            }
        }

        else{

            Position chaseBall = WR::Utils::threePoints(player()->position(), loc()->ball(), 1.0f, 0.0f);
            _sk_goTo->setGoToPos(chaseBall);
            _sk_goTo->setAvoidBall(false);
             enableTransition(STATE_GOTO);

        }
        break;

    }       //case ATTACK
    } // switch
    //enableTransition(0);
    //_sk_rotateTo->setDesiredPosition(loc()->ball());

    //_sk_goTo->setGoToPos(loc()->ball());
}
void Behaviour_Attacker::CheckIfAttack(){
    Position posBall = loc()->ball();
    if(loc()->isInsideOurField(posBall)){

        _state = STATE_WAIT;
        std::cout << "WAIT " << loc()->isInsideOurField(posBall) << std::endl;
    }
    else{
        _state = STATE_ATTACK;
        //std::cout << "ATTACK " << loc()->isInsideOurField(posBall) << std::endl;
    }
}
bool Behaviour_Attacker::isBehindBall(Position posObjective){
    Position posBall = loc()->ball();
    Position posPlayer = player()->position();
    float anglePlayer = WR::Utils::getAngle(posBall, posPlayer);
    float angleDest = WR::Utils::getAngle(posBall, posObjective);
    float diff = WR::Utils::angleDiff(anglePlayer, angleDest);

    return (diff>GEARSystem::Angle::pi/2.0f);
}

bool Behaviour_Attacker::isInsideAreaBehindBall(Position behindBall){
    float offset = 0.05f;
    if(player()->distanceTo(behindBall) <= offset && isBehindBall(loc()->theirGoal())){
            return true;
    }
    else{
        return false;
    }
}
bool Behaviour_Attacker::lookAtGoal(){
    return player()->isLookingTo(loc()->theirGoal() , 0.23f);
}

bool Behaviour_Attacker::isInsideProjectArea(Position posProjected){
    Position posBall = loc()->ball();
    Position posPlayer = player()->position();
    Position leftPost = loc()->theirGoalLeftPost();
    Position rightPost = loc()->theirGoalRightPost();
    float angleLeftPost = WR::Utils::getAngle(posBall , leftPost);
    float angleRightPost = WR::Utils::getAngle(posBall , rightPost );
    float Yl = tan(angleLeftPost)*posProjected.x() + posPlayer.y();
    float Yr = tan(angleRightPost)*posProjected.x() + posPlayer.y();
    if(loc()->ourSide().isRight()){
        if(WR::Utils::checkInterval(posProjected.y() , Yr , Yl)){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        if(WR::Utils::checkInterval(posProjected.y() , Yl , Yr)){
            return true;
        }
        else{
            return false;
        }
    }
}

