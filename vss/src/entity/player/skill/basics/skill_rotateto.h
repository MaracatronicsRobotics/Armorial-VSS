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

#ifndef SKILL_ROTATETO_H
#define SKILL_ROTATETO_H

#include <src/entity/player/skill/skill.h>

class Skill_RotateTo : public Skill{
private:
    void run();

    // Parameters
    Position _desiredPosition;
    float _angle;
    bool _rot;

public:
    Skill_RotateTo();
    QString name();

    void setDesiredPosition(Position pos) {
        _rot = false;
        _desiredPosition = pos; }
    void setDesiredAngle(float angle){
        _rot = true;
        _angle = angle;
    }
};

#endif // SKILL_ROTATETO_H
