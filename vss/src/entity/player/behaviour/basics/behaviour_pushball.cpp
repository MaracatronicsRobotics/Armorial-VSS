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

#include "behaviour_pushball.h"

QString Behaviour_PushBall::name(){
    return "Behaviour_PushBall";
}

Behaviour_PushBall::Behaviour_PushBall(){
}

void Behaviour_PushBall::configure(){
    usesSkill(_sk_goTo = new Skill_GoTo());
    usesSkill(_sk_rotateTo = new Skill_RotateTo());
    usesSkill(_sk_pushBall = new Skill_PushBall());

    //initial skill
    setInitialSkill(_sk_goTo);

    //transitions
    addTransition(STATE_ROTATE, _sk_goTo, _sk_rotateTo);
    addTransition(STATE_PUSH, _sk_rotateTo, _sk_pushBall);
}

void Behaviour_PushBall::run(){
    const Position ball = loc()->ball();
    const Position theirGoal = loc()->theirGoal();
    const Position behindBall = WR::Utils::threePoints(ball, theirGoal, 0.2f, GEARSystem::Angle::pi);

    //setting skill goto
    _sk_goTo->setGoToPos(behindBall);

    //setting skill rotateto
    _sk_rotateTo->setDesiredPosition(ball);

    //setting skill push
    _sk_pushBall->setSpeedAndOmega(1.0, 0.0);

    //transitions
    if(player()->isLookingTo(ball)){
        enableTransition(STATE_PUSH);
    } else if(player()->isNearbyPosition(behindBall, 0.03f)){
        enableTransition(STATE_ROTATE);
    }
}
