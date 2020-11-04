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
    bool avoidTheirArea = allyInTheirArea();
    Position ballPrevision(true, loc()->ball().x() + loc()->ballVelocity().x(), loc()->ball().y() + loc()->ballVelocity().y(), 0.0);
    Position behindBall;
    if (loc()->ourSide().isRight() && loc()->ballVelocity().x() > 0 && player()->position().x() > loc()->ball().x()) {
        behindBall = WR::Utils::threePoints(loc()->ball(), loc()->theirGoal(), 0.13f, GEARSystem::Angle::pi);
    }
    else if (loc()->ourSide().isLeft() && loc()->ballVelocity().x() < 0 && player()->position().x() < loc()->ball().x()) {
        behindBall = WR::Utils::threePoints(loc()->ball(), loc()->theirGoal(), 0.13f, GEARSystem::Angle::pi);
    }
    else {
        behindBall = WR::Utils::threePoints(ballPrevision, loc()->theirGoal(), 0.13f, GEARSystem::Angle::pi);
    }
    behindBall = projectPosOutsideGoalArea(behindBall, true, avoidTheirArea);
    float velocityNeeded;
    if (rotateToBall(behindBall) < 0.25f) velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(behindBall)) / (WR::Utils::distance(loc()->ball(), behindBall));
    else velocityNeeded = 0.0f;

    _sk_goTo->setVelocityNeeded(velocityNeeded);
    _sk_goTo->setGoToVelocityFactor(1.3f);
    _sk_goTo->setMinVelocity(0.7f);
    _sk_goTo->setAvoidOurGoalArea(true);
    _sk_goTo->setAvoidTheirGoalArea(avoidTheirArea);

    _sk_rotateTo->setDesiredPosition(loc()->ball());

    switch(_state){
    case STATE_PUSHBALL: {
        if ((!isInsideDashArea(loc()->ball(), false) && player()->distBall() > 0.5f) || abs(loc()->ball().y()) > 0.6f || abs(loc()->ball().x()) > 0.7f) {
            _state = STATE_GOTO;
            break;
        } else {
            if (((abs(rotateToBall(loc()->ball())) > 0.7f && player()->distBall() < 0.11f) || (abs(rotateToBall(loc()->ball())) > 1.5f))) {
                _state = STATE_ROTATETO;
                enableTransition(STATE_ROTATETO);
                break;
            } else {
                // Relação: velocidade linear / velocidade angular = raio de giro
                float chord;
                if (player()->distanceTo(behindBall) <= 0.07f) chord = player()->distanceTo(ballPrevision);
                else chord = player()->distanceTo(behindBall);
                float internalAngle = Angle::pi / 2 - rotateToBall(loc()->ball());
                float spinRadius = chord / (2 * cosf(internalAngle));

                // Configurações de velocidade linear mínima, pois a velocidade linear estará em função dela
                float angularSpeed = std::max(0.5f, abs(rotateToBall(loc()->ball())));
                float angularSignal = rotateToBall(loc()->ball()) / abs(rotateToBall(loc()->ball()));

                if (player()->isNearbyPosition(loc()->ball(), 0.11f)) {
                    // O robô avançará diretamente ao gol
                    if (player()->isLookingTo(loc()->ball(), Angle::pi / 2)) {
                        _sk_pushBall->setSpeedAndOmega(1.5f, 0.0f);
                    } else {
                        _sk_pushBall->setSpeedAndOmega(-1.5f, 0.0f);
                    }
                } else {
                    // O robô se desloca até o ponto desejado, baseado no raio de giro
                    if (player()->isLookingTo(loc()->ball(), Angle::pi / 2)) {
                        _sk_pushBall->setSpeedAndOmega(1.0f, angularSignal * 1.0f / spinRadius);
                    } else {
                        _sk_pushBall->setSpeedAndOmega(-1.0f, angularSignal * 1.0f / spinRadius);
                    }
                }
                enableTransition(STATE_PUSHBALL);
            }
        }
    }
        break;
    case STATE_ROTATETO: {
        if (!isInsideDashArea(ballPrevision, false)) {
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
        if (abs(ballPrevision.y()) > 0.6f || abs(ballPrevision.x()) > 0.7f) {
            _sk_goTo->setAvoidBall(false);
            _sk_goTo->setGoToPos(loc()->ball());
        } else {
            _sk_goTo->setAvoidBall(true);
            bool playerBehindBall = false;
            if(loc()->ourSide().isRight()){
                if(player()->position().x() > behindBall.x()) {
                    _sk_goTo->setGoToPos(behindBall);
                    playerBehindBall = true;
                } else {
                    _sk_goTo->setGoToPos(loc()->ourGoal());
                    playerBehindBall = false;
                }
            }else{
                if(player()->position().x() < behindBall.x()) {
                    _sk_goTo->setGoToPos(behindBall);
                    playerBehindBall = true;
                } else {
                    _sk_goTo->setGoToPos(loc()->ourGoal());
                    playerBehindBall = false;
                }
            }
        }
        enableTransition(STATE_GOTO);
        if (isInsideDashArea(ballPrevision, false)) {
            if (player()->isNearbyPosition(behindBall, 0.15f)) {
                if (abs(rotateToBall(loc()->ball())) < 0.8f) {
                    _state = STATE_PUSHBALL;
                } else {
                    _state = STATE_ROTATETO;
                    enableTransition(STATE_ROTATETO);
                }
            }
        }
    }
    }
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

bool Behaviour_Attacker::isInsideDashArea(Position reference, bool isBig){
    float leftPostAngle;
    float rightPostAngle;

    //Definição dos ângulos dos coeficientes angulares
    if (isBig) {
        leftPostAngle = WR::Utils::getAngle(reference , loc()->theirGoalLeftPost());
        rightPostAngle = WR::Utils::getAngle(reference , loc()->theirGoalRightPost());
    } else {
        leftPostAngle = WR::Utils::getAngle(reference , loc()->theirGoalLeftMidPost());
        rightPostAngle = WR::Utils::getAngle(reference , loc()->theirGoalRightMidPost());
    }

    //Definição dos coeficientes lineares
    float leftPost_b = reference.y() - tan(leftPostAngle)*reference.x();
    float rightPost_b = reference.y() - tan(rightPostAngle)*reference.x();

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

float Behaviour_Attacker::rotateToBall(Position reference) {
    float rotateAngle = WR::Utils::getAngle(player()->position(), reference) - player()->orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
}

bool Behaviour_Attacker::allyInTheirArea(){
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

Position Behaviour_Attacker::projectPosOutsideGoalArea(Position pos, bool avoidOurArea, bool avoidTheirArea){
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
