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
    usesSkill(_sk_pushBall = new Skill_PushBall());

    addTransition(STATE_ROTATETO, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_ROTATETO, _sk_pushBall, _sk_rotateTo);

    addTransition(STATE_GOTO, _sk_rotateTo, _sk_goTo);
    addTransition(STATE_GOTO, _sk_pushBall , _sk_goTo);

    addTransition(STATE_PUSHBALL, _sk_goTo , _sk_pushBall);
    addTransition(STATE_PUSHBALL, _sk_rotateTo , _sk_pushBall);

    setInitialSkill(_sk_goTo);

    _state = STATE_GOTO;
};

void Behaviour_Attacker::run() {
    Position behindBall = WR::Utils::threePoints(loc()->ball(), loc()->theirGoal(), 0.11f, GEARSystem::Angle::pi);
    Position ballPrevision(true, behindBall.x() + loc()->ballVelocity().x() / 2, behindBall.y() + loc()->ballVelocity().y() / 2, 0.0);

    _sk_goTo->setAvoidBall(true);
    _sk_goTo->setGoToVelocityFactor(5.0);

    _sk_rotateTo->setDesiredPosition(loc()->ball());

    switch(_state){
    case STATE_PUSHBALL: {
        std::cout << "Fonzinho\n";
        if (!player()->isNearbyPosition(loc()->ball(), 0.2f)) {
            _state = STATE_GOTO;
            break;
        } else {
            if ((abs(rotateToBall(loc()->ball())) > 1.2f)) {
                _state = STATE_ROTATETO;
                enableTransition(STATE_ROTATETO);
                break;
            } else {
                float angLeft = (WR::Utils::getAngle(player()->position(), loc()->theirGoalLeftPost()));
                float angRight = (WR::Utils::getAngle(player()->position(), loc()->theirGoalRightPost()));
                float errorAngleToTheirGoal = 0.9f*abs(angRight - angLeft)/2.0f;
                if (player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)) {
                    _sk_pushBall->setSpeedAndOmega(1.0, 0.0);
                } else if (abs(rotateToBall(loc()->theirGoal())) < errorAngleToTheirGoal) {
                    _sk_pushBall->setSpeedAndOmega(-1.0, 0.0);
                }
                enableTransition(STATE_PUSHBALL);
            }
        }
    }
        break;
    case STATE_ROTATETO: {
        std::cout << "Fon\n";
        if (!player()->isNearbyPosition(ballPrevision, 0.2f)) {
            _state = STATE_GOTO;
            break;
        }
        if (abs(rotateToBall(loc()->ball())) < 0.7f) {
            _state = STATE_PUSHBALL;
            break;
        }
    }
        break;
    case STATE_GOTO: {
        bool playerBehindBall = false;
        if(loc()->ourSide().isRight()){
            if(player()->position().x() > loc()->ball().x()) {
                _sk_goTo->setGoToPos(ballPrevision);
                playerBehindBall = true;
            } else {
                _sk_goTo->setGoToPos(loc()->ourGoal());
                playerBehindBall = false;
            }
        }else{
            if(player()->position().x() < loc()->ball().x()) {
                _sk_goTo->setGoToPos(ballPrevision);
                playerBehindBall = true;
            } else {
                _sk_goTo->setGoToPos(loc()->ourGoal());
                playerBehindBall = false;
            }
        }
        enableTransition(STATE_GOTO);
        if (playerBehindBall) {
            std::cout << "SAMBA\n";
            if (player()->isNearbyPosition(ballPrevision, 0.15f)) {
                std::cout << "RECIFE\n";
                if (abs(rotateToBall(loc()->ball())) < 1.2f) {
                    _state = STATE_PUSHBALL;
                } else {
                    _state = STATE_ROTATETO;
                    enableTransition(STATE_ROTATETO);
                }
            }
        }
    }
    }

    /*if (isInsideDashArea()) {
        if (lookingToBall().first) {
            float angLeft = (WR::Utils::getAngle(player()->position(), loc()->theirGoalLeftPost()));
            float angRight = (WR::Utils::getAngle(player()->position(), loc()->theirGoalRightPost()));
            float errorAngleToTheirGoal = 0.9f*abs(angRight - angLeft)/2.0f;
            if (!player()->isLookingTo(loc()->theirGoal(), errorAngleToTheirGoal)) {
                std::cout << "FOGO\n";
                _sk_pushBall->setSpeedAndOmega(-1.0, 0.0);
                std::cout << "FOGO\n";
                enableTransition(STATE_PUSHBALL);
            } else {
                std::cout << "ÁGUA\n";
                _sk_pushBall->setSpeedAndOmega(1.0, 0.0);
                std::cout << "ÁGUA\n";
                enableTransition(STATE_PUSHBALL);
            }
        } else {
            std::cout << "AR\n";
            enableTransition(STATE_ROTATETO);
        }
    } else {
        std::cout << "TERRA\n";
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

float Behaviour_Attacker::rotateToBall(Position something) {
    float rotateAngle = WR::Utils::getAngle(player()->position(), something) - player()->orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
}

