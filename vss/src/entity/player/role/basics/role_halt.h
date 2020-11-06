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

#ifndef ROLE_HALT_H
#define ROLE_HALT_H

#include <src/entity/player/behaviour/vssbehaviours.h>
#include <src/entity/player/role/role.h>

class Role_Halt : public Role {
private:
    // Behaviours
    Behaviour_DoNothing *_bh_dn;
    Behaviour_PushBall *_bh_pb;
    Behaviour_Goalkeeper *_bh_gk;
    Behaviour_Assistant *_bh_as;
    Behaviour_Attacker *_bh_at;
    Behaviour_Barrier *_bh_bar;

    // Behaviours ids!
    enum{
        BHV_DONOTHING,
        BHV_PUSHBALL,
        BHV_GK,
        BHV_ASSISTANT,
        BHV_ATTACKER,
        BHV_BARRIER
    };

    // Inherited functions
    void configure();
    void run();

public:
    Role_Halt();
    void initializeBehaviours();
    QString name();

public slots:
    void receiveFoul(VSSRef::Foul foul, VSSRef::Quadrant quadrant, VSSRef::Color teamColor);
};

#endif // ROLE_HALT_H
