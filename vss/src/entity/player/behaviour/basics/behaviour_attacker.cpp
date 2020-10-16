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
    addTransition(STATE_ROTATETO, _sk_pushBall, _sk_rotateTo);

    addTransition(STATE_GOTO, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTO, _sk_pushBall , _sk_goTo);

    addTransition(STATE_PUSHBALL, _sk_goTo , _sk_pushBall);
    addTransition(STATE_PUSHBALL, _sk_rotateTo , _sk_pushBall);

    setInitialSkill(_sk_pushBall);
};

void Behaviour_Attacker::run() {
    CheckIfAttack();
    switch(_state){
    case STATE_WAIT:{
        std::cout << "Waiting...\n";
        //hold the position
        Position posWait(true , 0.375f , 0.0f , 0.0f);
        _sk_goTo->setGoToPos(posWait);
        enableTransition(STATE_GOTO);
        break;
    }
    case STATE_ATTACK:{
        std::cout << "Attacking! Let's take this guys down!\n";
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

                    if(isInsideDashArea()){ //threePoints apontando mt longe
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
    /*Position ballPrevision(true, loc()->ball().x() + 2 * loc()->ballVelocity().x(), loc()->ball().y() + 2 * loc()->ballVelocity().y(), 0.0);
    _sk_goTo->setGoToPos(loc()->ball());
    _sk_goTo->setAvoidBall(true);
    _sk_goTo->setGoToVelocityFactor(4.0);

    _sk_rotateTo->setDesiredPosition(loc()->ball());

    if (isInsideDashArea()) {
        if (abs(abs(WR::Utils::getAngle(player()->position(), loc()->ball()) - player()->orientation().value()) - Angle::pi) < 0.1f) {
            std::cout << "Me lambe\n";
            _sk_pushBall->setSpeedAndOmega(-1.0, 0.0);
            std::cout << "Me lambe\n";
            enableTransition(STATE_PUSHBALL);
        } else if (abs(WR::Utils::getAngle(player()->position(), loc()->ball()) - player()->orientation().value()) < 0.1f) {
            std::cout << "Chupa-me\n";
            _sk_pushBall->setSpeedAndOmega(1.0, 0.0);
            std::cout << "Chupa-me\n";
            enableTransition(STATE_PUSHBALL);
        } else {
            std::cout << "Roda a roda\n";
            enableTransition(STATE_ROTATETO);
        }
    } else {
        std::cout << "Dale\n";
        enableTransition(STATE_GOTO);
    }*/
}
void Behaviour_Attacker::CheckIfAttack(){
    Position posBall = loc()->ball();
    if(loc()->isInsideOurField(posBall)){

        _state = STATE_WAIT;
        //std::cout << "WAIT " << loc()->isInsideOurField(posBall) << std::endl;
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

    return (diff>GEARSystem::Angle::pi/18.0f);
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

bool Behaviour_Attacker::isInsideDashArea(){
    //Definição dos ângulos dos coeficientes angulares
    float leftPostAngle = WR::Utils::getAngle(loc()->ball() , loc()->theirGoalLeftMidPost());
    float rightPostAngle = WR::Utils::getAngle(loc()->ball() , loc()->theirGoalRightMidPost());

    //Definição dos coeficientes lineares
    float leftPost_b = loc()->ball().y() - tan(leftPostAngle)*loc()->ball().x();
    float rightPost_b = loc()->ball().y() - tan(rightPostAngle)*loc()->ball().x();

    //Definição dos limites verticais de posicionamento do jogador
    float leftPost_y = tan(leftPostAngle) * player()->position().x() + leftPost_b;
    float rightPost_y = tan(rightPostAngle) * player()->position().x() + rightPost_b;

    //Avaliação da região que o jogador se encontra
    if(loc()->ourSide().isRight()){
        if(player()->position().y() < leftPost_y && player()->position().y() > rightPost_y) return true;
    } else {
        if(player()->position().y() > leftPost_y && player()->position().y() < rightPost_y) return true;
    }
    return false;
}

bool Behaviour_Attacker::isLookingToBall() {
    float rotateAngle = WR::Utils::getAngle(player()->position(), loc()->ball()) - player()->orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
}

