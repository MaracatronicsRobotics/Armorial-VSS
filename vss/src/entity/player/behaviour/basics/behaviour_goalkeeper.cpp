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
    /*if (!loc()->isInsideOurArea(player()->position())) {
        // Positioning at goalcenter. A better positioning is recommended.
        const Position initialPosition = WR::Utils::threePoints(loc()->ourGoal(), player()->position(), 0.2f, 0.0f);
        _sk_goto->setGoToPos(initialPosition);
        enableTransition(STATE_GOTO);
    } else {
        if (loc()->isInsideOurArea(loc()->ball())) {
            bool spinDirection = setSpinDirection();
            _sk_spin->setClockWise(spinDirection);
            enableTransition(STATE_SPIN);
        } else {
            enableTransition(STATE_INTERCEPT);
        }
    }*/
    const Position initialPosition = WR::Utils::threePoints(loc()->ourGoal(), loc()->fieldCenter(), 0.055f, 0.0f);
    Position firstInterceptPoint(true, initialPosition.x(), -0.35f, 0.0f);
    Position secondInterceptPoint(true, initialPosition.x(), 0.35f, 0.0f);

    // GoTo setup
    _sk_goto->setGoToPos(initialPosition);

    // InterceptBall setup
    _sk_intercept->setInterceptSegment(firstInterceptPoint, secondInterceptPoint);

    switch(_state) {
    case STATE_GOTO: {
        if (abs(player()->position().x() - initialPosition.x()) < 0.1f) {
            if (loc()->isInsideOurArea(loc()->ball())) {
                bool spinDirection = setSpinDirection();
                _sk_spin->setClockWise(spinDirection);
                enableTransition(STATE_SPIN);
                _state = STATE_SPIN;
            } else {
                enableTransition(STATE_INTERCEPT);
                _state = STATE_INTERCEPT;
            }
        }
        std::cout << "Currently using Behaviour GoTo\n";
        break; }

    case STATE_INTERCEPT: {
        if (!(loc()->isInsideOurArea(player()->position()))) {
            enableTransition(STATE_GOTO);
            _state = STATE_GOTO;
        }
        //std::cout << "FON";
        else if (loc()->isInsideOurArea(loc()->ball())) {
            bool spinDirection = setSpinDirection();
            _sk_spin->setClockWise(spinDirection);
            enableTransition(STATE_SPIN);
            _state = STATE_SPIN;
        }
        std::cout << "Currently using Behaviour InterceptBall\n";
        break; }

    case STATE_SPIN: {
        if (!(loc()->isInsideOurArea(player()->position()))) {
            enableTransition(STATE_GOTO);
            _state = STATE_GOTO;
        }
        //std::cout << "FONZÃO";
        else if (!loc()->isInsideOurArea(loc()->ball())) {
            enableTransition(STATE_INTERCEPT);
            _state = STATE_INTERCEPT;
        }
        std::cout << "Currently using Behaviour Spin\n";
        break; }
    }
}

bool Behaviour_Goalkeeper::setSpinDirection() {
    // true if clockwise, false otherwise
    if (loc()->distBallOurRightPost() < loc()->distBallOurLeftPost())
        return true;
    else
        return false;
}
