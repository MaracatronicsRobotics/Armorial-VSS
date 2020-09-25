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

QString Behaviour_Attacker::name() {
    return "Behaviour_Attacker";
}

Behaviour_Attacker::Behaviour_Attacker() {
}

void Behaviour_Attacker::configure() {
    usesSkill(_sk_goto = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());

    addTransition(0, _sk_goto, _sk_rotateTo);
    addTransition(1, _sk_rotateTo, _sk_goto);

    setInitialSkill(_sk_goto);
};

void Behaviour_Attacker::run() {
    Position behindBall = WR::Utils::threePoints(loc()->ball(), loc()->theirGoal(), 0.3f, GEARSystem::Angle::pi);

    if(!loc()->isOutsideField(behindBall)){
        if(player()->playerId() == 2) std::cout << "tá behind: " << isBehindBall(loc()->theirGoal()) << std::endl;
        if(isBehindBall(loc()->theirGoal())){
            Position chaseBall = WR::Utils::threePoints(player()->position(), loc()->ball(), 1.0f, 0.0f);
            _sk_goto->setGoToPos(chaseBall);
            enableTransition(1);
        }
        else{
            _sk_goto->setGoToPos(behindBall);
            enableTransition(1);
        }
    }
    else{
        enableTransition(1);
        Position chaseBall = WR::Utils::threePoints(player()->position(), loc()->ball(), 1.0f, 0.0f);
        _sk_goto->setGoToPos(chaseBall);
    }
}

bool Behaviour_Attacker::isBehindBall(Position posObjective){
    Position posBall = loc()->ball();
    Position posPlayer = player()->position();
    float anglePlayer = WR::Utils::getAngle(posBall, posPlayer);
    float angleDest = WR::Utils::getAngle(posBall, posObjective);
    float diff = WR::Utils::angleDiff(anglePlayer, angleDest);

    return (diff>GEARSystem::Angle::pi/2.0f);
}
