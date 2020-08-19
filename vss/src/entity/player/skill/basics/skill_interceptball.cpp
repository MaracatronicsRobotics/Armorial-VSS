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
}

void Skill_InterceptBall::run() {
    Position unitaryBallVelocity = Position(true, loc()->ballVelocity().x()/loc()->ballVelocity().abs(), loc()->ballVelocity().y()/loc()->ballVelocity().abs(), 0.0);
    Position objectivePos; // Position where the player should be
    // Check ball speed (maybe a error)
    if(loc()->ballVelocity().abs() <= 0.1f)
        objectivePos = loc()->ball(); // keep actual position
    else{
        // Now ball velocity line (pos + uni_velocity vector)
        Position ballVelocityLine = Position(true, loc()->ball().x()+unitaryBallVelocity.x(), loc()->ball().y()+unitaryBallVelocity.y(), 0.0);

        // Call utils to get projection
        objectivePos = WR::Utils::projectPointAtLine(loc()->ball(), ballVelocityLine, player()->position()); //Intercepta em 90 graus

        if (_firstLimitationPoint.isUnknown()) _firstLimitationPoint = loc()->ball();
        if (_secondLimitationPoint.isUnknown()) _secondLimitationPoint = objectivePos;

        objectivePos = WR::Utils::projectPointAtSegment(_firstLimitationPoint, _secondLimitationPoint, objectivePos);
    }

    // Vx/Dx = Vy/Dy (V = velocidade/ D = distância)
    float velocityNeeded = (loc()->ballVelocity().abs() * player()->distanceTo(objectivePos)) / (WR::Utils::distance(loc()->ball(), objectivePos));

    player()->goTo(objectivePos, velocityNeeded);
}