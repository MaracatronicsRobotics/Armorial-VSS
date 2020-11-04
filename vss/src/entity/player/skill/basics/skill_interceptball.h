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

#ifndef SKILL_INTERCEPTBALL_H
#define SKILL_INTERCEPTBALL_H

#include <src/entity/player/skill/skill.h>

class Skill_InterceptBall : public Skill {
private:
    void run();

    // Parameters
    Position _firstLimitationPoint;
    Position _secondLimitationPoint;
    Position _objectivePos; // Position where the player should be
    float _velocityNeeded;
    float _velocityFactor;
    bool _activateVelocityNeeded;

public:
    Skill_InterceptBall();
    QString name();

    void setInterceptSegment(Position firstPoint, Position secondPoint) { _firstLimitationPoint = firstPoint; _secondLimitationPoint = secondPoint; }
    void setDesiredVelocity(float velocityNeeded) { _velocityNeeded = velocityNeeded; }
    void setVelocityFactor(float velocityFActor) { _velocityFactor = velocityFActor; }
    bool selectVelocityNeeded(bool activateVelocityNeeded) { _activateVelocityNeeded = activateVelocityNeeded; }
    Position getIntercetPosition() { return _objectivePos; }
};

#endif // SKILL_INTERCEPTBALL_H
