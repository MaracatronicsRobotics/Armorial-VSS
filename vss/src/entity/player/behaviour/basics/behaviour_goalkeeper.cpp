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

#include "behaviour_goalkeeper.h"

#define GOAL_OFFSET 0.01f

QString Behaviour_Goalkeeper::name() {
    return "Behaviour_Goalkeeper";
}

Behaviour_Goalkeeper::Behaviour_Goalkeeper() {
    _sk_goto = nullptr;
    _sk_intercept = nullptr;
    _sk_spin = nullptr;
}

void Behaviour_Goalkeeper::configure() {
    usesSkill(_sk_goto = new Skill_GoTo());
    usesSkill(_sk_intercept = new Skill_InterceptBall());
    usesSkill(_sk_spin = new Skill_Spin());

    // Setting initial skill
    setInitialSkill(_sk_goto);

    // Transitions:
    // Skill GoTo active
    addTransition(STATE_GOTO, _sk_intercept, _sk_goto);
    addTransition(STATE_GOTO, _sk_spin, _sk_goto);

    // Skill InterceptBall active
    addTransition(STATE_INTERCEPT, _sk_goto, _sk_intercept);
    addTransition(STATE_INTERCEPT, _sk_spin, _sk_intercept);

    // Skill Spin active
    addTransition(STATE_SPIN, _sk_goto, _sk_spin);
    addTransition(STATE_SPIN, _sk_intercept, _sk_spin);

    _state = STATE_GOTO;
}

void Behaviour_Goalkeeper::run() {
    const Position initialPosition = WR::Utils::threePoints(loc()->ourGoal(), loc()->fieldCenter(), 0.055f, 0.0f);
    Position ProjectBallY;

    // Permitir o acompanhamento da bola, limitando até a projeção vertical das traves do gol
    if(loc()->ourSide().isLeft()){
        if(loc()->ball().y() <= loc()->ourGoalLeftPost().y()){
            ProjectBallY.setPosition(initialPosition.x() , loc()->ourGoalLeftPost().y() + GOAL_OFFSET , 0.0);
        }
        else if(loc()->ball().y() >= loc()->ourGoalRightPost().y()){
            ProjectBallY.setPosition(initialPosition.x() , loc()->ourGoalRightPost().y() - GOAL_OFFSET, 0.0);
        }
        else{
            ProjectBallY.setPosition(initialPosition.x() , loc()->ball().y() , 0.0);

        }
    } else {
        if(loc()->ball().y() >= loc()->ourGoalLeftPost().y()){
            ProjectBallY.setPosition(initialPosition.x() , loc()->ourGoalLeftPost().y() - GOAL_OFFSET , 0.0);
        }
        else if(loc()->ball().y() <= loc()->ourGoalRightPost().y()){
            ProjectBallY.setPosition(initialPosition.x() , loc()->ourGoalRightPost().y() + GOAL_OFFSET, 0.0);
        }
        else{
            ProjectBallY.setPosition(initialPosition.x() , loc()->ball().y() , 0.0);

        }
    }

    Position firstInterceptPoint(true, ProjectBallY.x(), -0.35f, 0.0f);
    Position secondInterceptPoint(true, ProjectBallY.x(), 0.35f, 0.0f);

    // InterceptBall setup
    _sk_intercept->setInterceptSegment(firstInterceptPoint, secondInterceptPoint);
    _sk_intercept->selectVelocityNeeded(false);
    _sk_intercept->setVelocityFactor(1.2f);
    _sk_intercept->setDesiredVelocity(1.0f);

    if (!loc()->isInsideOurArea(player()->position()) || loc()->isInsideTheirField(loc()->ball())) {
        _sk_goto->setGoToPos(ProjectBallY);
        enableTransition(STATE_GOTO);
    }
    else if (loc()->isInsideOurArea(loc()->ball()) && loc()->ballVelocity().abs() < 0.0001f) {
        _sk_goto->setGoToPos(loc()->ball());
        //_sk_goto->setMinVelocity(1.0f);
        enableTransition(STATE_GOTO);
    }
    else {
        Position interceptPosition = _sk_intercept->getIntercetPosition();
        if (player()->distanceTo(interceptPosition) < 0.05f && WR::Utils::distance(interceptPosition, loc()->ball()) < 0.1f) {
            bool spinDirection = setSpinDirection();
            _sk_spin->setClockWise(spinDirection);
            enableTransition(STATE_SPIN);
        } else {
            enableTransition(STATE_INTERCEPT);
        }
    }
}

bool Behaviour_Goalkeeper::setSpinDirection() {
    // true if clockwise, false otherwise
    //caso original
    /*if ((loc()->distBallOurRightPost() < loc()->distBallOurLeftPost()))
        return true;
    else
        return false;*/

    //problema: se a bola tiver em baixo do jogador mas perto do poste esquerdo,ele gira antihorario,q faz gol contra  , e o caso inverso tb
    //nessa implementaçao ele entra quase sempre no caso normal mas em casos de bola cruzada , inverte a rotaçao para devolver pro campo
    if(loc()->ourSide().isRight()){
        if(loc()->ball().y() >= player()->position().y()){
            return false;
        }
        else{
            return true;
        }
    } else {
        if(loc()->ball().y() >= player()->position().y()){
            return true;
        }
        else{
            return false;
        }
    }
}

