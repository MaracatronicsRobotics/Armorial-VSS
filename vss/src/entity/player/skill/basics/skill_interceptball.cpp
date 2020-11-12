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

#include "skill_interceptball.h"

QString Skill_InterceptBall::name() {
    return "Skill_InterceptBall";
}

Skill_InterceptBall::Skill_InterceptBall() {
    _firstLimitationPoint = Position(false, 0.0, 0.0, 0.0);
    _secondLimitationPoint = Position(false, 0.0, 0.0, 0.0);
    _objectivePos = Position(false, 0.0, 0.0, 0.0);
    _velocityNeeded = 0.0f;
    _velocityFactor = 1.0f;
    _activateVelocityNeeded = false;
}

void Skill_InterceptBall::run() {

    if(_firstLimitationPoint.y() > _secondLimitationPoint.y()){
        std::swap(_firstLimitationPoint, _secondLimitationPoint);
    }

    // Check ball speed (maybe a error)
    if(loc()->ballVelocity().abs() <= 0.01f) {
        //if ball stopped: get interception point between ball-ourGoal and interception limits
        _objectivePos = WR::Utils::hasInterceptionSegments(loc()->ball(), loc()->ourGoal(), _firstLimitationPoint, _secondLimitationPoint);
        if(_objectivePos.isValid()){
            //limiting interception point in our interception segment
            float objPosY = _objectivePos.y();
            WR::Utils::limitValue(&objPosY, _firstLimitationPoint.y(), _secondLimitationPoint.y());
            _objectivePos.setPosition(_objectivePos.x(), objPosY, 0);
        }
        //_objectivePos = player()->position(); // keep actual position
    } else {
        Position unitaryBallVelocity = Position(true, loc()->ballVelocity().x()/loc()->ballVelocity().abs(), loc()->ballVelocity().y()/loc()->ballVelocity().abs(), 0.0);
        // Now ball velocity line (pos + uni_velocity vector)
        Position ballVelocityLine = Position(true, loc()->ball().x()+unitaryBallVelocity.x(), loc()->ball().y()+unitaryBallVelocity.y(), 0.0);

        // Call utils to get projection
        Position inicialReference = WR::Utils::projectPointAtLine(loc()->ball(), ballVelocityLine, player()->position()); //Intercepta em 90 graus

        if (_firstLimitationPoint.isUnknown()) _firstLimitationPoint = loc()->ball();
        if (_secondLimitationPoint.isUnknown()) _secondLimitationPoint = inicialReference;

        //finalReference is the interception between the ball-projPlayerOnBallWay line and interceptionLimits
        Position finalReference = WR::Utils::hasInterceptionSegments(loc()->ball(), inicialReference, _firstLimitationPoint, _secondLimitationPoint);
        //limiting interception point in our interception segment
        if(finalReference.isValid()){
            float objPosY = finalReference.y();
            WR::Utils::limitValue(&objPosY, _firstLimitationPoint.y(), _secondLimitationPoint.y());
            finalReference.setPosition(finalReference.x(), objPosY, 0);
        }else{
            float ballY = loc()->ball().y();
            WR::Utils::limitValue(&ballY, _firstLimitationPoint.y(), _secondLimitationPoint.y());
            finalReference.setPosition(_firstLimitationPoint.x(), ballY, 0);
        }

        //left
        if(loc()->ourSide().isLeft()){
            if(loc()->ball().x() < loc()->ourGoalLeftPost().x() + 0.1f){
                float ballY = loc()->ball().y();
                WR::Utils::limitValue(&ballY, _firstLimitationPoint.y(), _secondLimitationPoint.y());
                finalReference.setPosition(_firstLimitationPoint.x(), ballY, 0);
            }
        }
        //right
        else{
            if(loc()->ball().x() > loc()->ourGoalRightPost().x() - 0.1f){
                float ballY = loc()->ball().y();
                WR::Utils::limitValue(&ballY, _firstLimitationPoint.y(), _secondLimitationPoint.y());
                finalReference.setPosition(_firstLimitationPoint.x(), ballY, 0);
            }
        }

        _objectivePos = finalReference;

    }

    // Condição de definição da velocidade usada para interceptar, onde a skill calcula, caso o Behaviour não selecione
    if (!_activateVelocityNeeded) {
        // Vx/Dx = Vy/Dy (V = velocidade/ D = distância)
        float velNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(_objectivePos)) / (WR::Utils::distance(loc()->ball(), _objectivePos));
        if(!isnanf(velNeeded)){
            _velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(_objectivePos)) / (WR::Utils::distance(loc()->ball(), _objectivePos));
            WR::Utils::limitValue(&_velocityNeeded, 1.0f, 10.0f);
        }else{
            _velocityNeeded = 1.0f;
        }
    }

    player()->goTo(_objectivePos, _velocityNeeded, _velocityFactor, 0.0, false, false, false, false, false, false);
}
