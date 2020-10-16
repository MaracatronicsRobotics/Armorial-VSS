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

#ifndef BEHAVIOUR_ATTACKER_H
#define BEHAVIOUR_ATTACKER_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>
#include <math.h>

class Behaviour_Attacker : public Behaviour {
private:
    void configure();
    void run();

    // Machine State
    int _state;
    enum{
        STATE_ATTACK ,
        STATE_WAIT
    };

    // States
    enum {
        STATE_GOTO ,
        STATE_ROTATETO,
        STATE_PUSHBALL,
        STATE_SPIN
    };

    // Skills
    Skill_GoTo *_sk_goTo;
    Skill_RotateTo *_sk_rotateTo;
    Skill_PushBall *_sk_pushBall;
    Skill_Spin *_sk_Spin;

    // Auxiliary Functions
    bool isBehindBall(Position posObjective);
    bool isInsideAreaBehindBall(Position behindBall);
    bool lookAtGoal();
    bool isInsideDashArea();
    bool isLookingToBall();

    void CheckIfAttack();


public:
    Behaviour_Attacker();
    QString name();
};

#endif // BEHAVIOUR_ATTACKER_H
