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

class Role_Halt : public Role
{
private:
    // Behaviours
    Behaviour_DoNothing *_bh_dn;

    // Behaviours ids!
    enum{
        BHV_DONOTHING
    };

    // Inherited functions
    void configure();
    void run();

public:
    Role_Halt();
    void initializeBehaviours();
    QString name();
};

#endif // ROLE_HALT_H