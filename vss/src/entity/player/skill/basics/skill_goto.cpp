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

#include "skill_goto.h"

QString Skill_GoTo::name(){
    return "Skill_GoTo";
}

Skill_GoTo::Skill_GoTo(){
    _goToPos = Position(true, 0.0, 0.0, 0.0);
    _goToVelocityFactor = 1.0;
    _avoidTeammates = true;
    _avoidOpponents = true;
}

void Skill_GoTo::run(){
    player()->goTo(_goToPos, _goToVelocityFactor, _minVelocity, _avoidTeammates, _avoidOpponents, _avoidBall, _avoidOurGoalArea, _avoidTheirGoalArea);
}
