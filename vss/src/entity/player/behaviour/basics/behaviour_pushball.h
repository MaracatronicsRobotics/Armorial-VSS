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

#ifndef BEHAVIOUR_PUSHBALL_H
#define BEHAVIOUR_PUSHBALL_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>

class Behaviour_PushBall : public Behaviour{
private:
    void configure();
    void run();

    bool _isBehindball, _isLookingToBall, _pushed;

    //States
    enum{
        STATE_GOTOPOS,
        STATE_ROTATE,
        STATE_PUSH
    };

    //Skills
    Skill_RotateTo *_sk_rotateTo;
    Skill_GoTo *_sk_goTo;
    Skill_PushBall *_sk_pushBall;

    // Parameters
    Position _aimPosition;

public:
    Behaviour_PushBall();
    QString name();

    void setAimPosition(Position aimPosition) { _aimPosition = aimPosition; }
};

#endif // BEHAVIOUR_PUSHBALL_H
