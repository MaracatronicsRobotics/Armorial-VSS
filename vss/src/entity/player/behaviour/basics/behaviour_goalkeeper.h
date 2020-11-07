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

#ifndef BEHAVIOUR_GOALKEEPER_H
#define BEHAVIOUR_GOALKEEPER_H

#include <src/entity/player/behaviour/behaviour.h>
#include <src/entity/player/skill/vssskills.h>

class Behaviour_Goalkeeper : public Behaviour {
private:
    void configure();
    void run();

    // Skills
    Skill_GoTo *_sk_goto;
    Skill_RotateTo *_sk_rotate;
    Skill_InterceptBall *_sk_intercept;
    Skill_Spin *_sk_spin;

    // Transitions
    enum {
        STATE_GOTO,
        STATE_ROTATE,
        STATE_INTERCEPT,
        STATE_SPIN
    };

    // Parameters
    int _state;

    // Auxiliary Functions
    bool isBallComing();
    bool setSpinDirection();

public:
    Behaviour_Goalkeeper();
    QString name();
};

#endif // BEHAVIOUR_GOALKEEPER_H
