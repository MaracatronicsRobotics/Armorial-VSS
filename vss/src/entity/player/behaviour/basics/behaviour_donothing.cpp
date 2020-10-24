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

#include "behaviour_donothing.h"

QString Behaviour_DoNothing::name() {
    return "Behaviour_DoNothing";
}

Behaviour_DoNothing::Behaviour_DoNothing() {
}

void Behaviour_DoNothing::configure() {
    //usesSkill(_sk_interceptBall = new Skill_InterceptBall());
    //usesSkill(_sk_doNothing = new Skill_DoNothing());
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotate = new Skill_RotateTo());
    usesSkill(_sk_push = new Skill_PushBall());

    addTransition(STATE_ROTATETO, _sk_goTo, _sk_rotate);
    addTransition(STATE_ROTATETO, _sk_push, _sk_rotate);

    addTransition(STATE_GOTO, _sk_rotate, _sk_goTo);
    addTransition(STATE_GOTO, _sk_push , _sk_goTo);

    addTransition(STATE_PUSHBALL, _sk_goTo , _sk_push);
    addTransition(STATE_PUSHBALL, _sk_rotate , _sk_push);

    setInitialSkill(_sk_push);
};

void Behaviour_DoNothing::run() {
    Position behindBall = WR::Utils::threePoints(loc()->ball(), loc()->theirGoal(), 0.11f, GEARSystem::Angle::pi);
    Position ballPrevision(true, behindBall.x() + loc()->ballVelocity().x() / 2, behindBall.y() + loc()->ballVelocity().y() / 2, 0.0);

    _sk_goTo->setAvoidBall(true);
    _sk_goTo->setGoToVelocityFactor(5.0);

    _sk_rotate->setDesiredPosition(loc()->ball());

    float chord = player()->distanceTo(behindBall);
    float internalAngle = Angle::pi / 2 - rotateToBall();
    float spinRadius = chord / (2 * cosf(internalAngle));

    if (player()->isLookingTo(loc()->ball(), Angle::pi / 2)) {
        _sk_push->setSpeedAndOmega(spinRadius * rotateToBall(), rotateToBall());
    } else {
        _sk_push->setSpeedAndOmega(-spinRadius * rotateToBall(), rotateToBall());
    }
}

bool Behaviour_DoNothing::isInsideDashArea(){
    //Definição dos ângulos dos coeficientes angulares
    float leftPostAngle = WR::Utils::getAngle(loc()->ball() , loc()->theirGoalLeftMidPost());
    float rightPostAngle = WR::Utils::getAngle(loc()->ball() , loc()->theirGoalRightMidPost());

    //Definição dos coeficientes lineares
    float leftPost_b = loc()->ball().y() - tanf(leftPostAngle)*loc()->ball().x();
    float rightPost_b = loc()->ball().y() - tanf(rightPostAngle)*loc()->ball().x();

    //Definição dos limites verticais de posicionamento do jogador
    float leftPost_y = tanf(leftPostAngle) * player()->position().x() + leftPost_b;
    float rightPost_y = tanf(rightPostAngle) * player()->position().x() + rightPost_b;

    //Avaliação da região que o jogador se encontra
    if(loc()->ourSide().isRight()){
        if(player()->position().y() < leftPost_y && player()->position().y() > rightPost_y) return true;
    } else {
        if(player()->position().y() > leftPost_y && player()->position().y() < rightPost_y) return true;
    }
    return false;
}

float Behaviour_DoNothing::rotateToBall() {
    float rotateAngle = WR::Utils::getAngle(player()->position(), loc()->ball()) - player()->orientation().value();

    if(rotateAngle > float(M_PI)) rotateAngle -= 2.0f * float(M_PI);
    if(rotateAngle < float(-M_PI)) rotateAngle += 2.0f * float(M_PI);

    if(rotateAngle > float(M_PI_2)) rotateAngle -= float(M_PI);
    if(rotateAngle < float(-M_PI_2)) rotateAngle += float(M_PI);

    return rotateAngle;
}
