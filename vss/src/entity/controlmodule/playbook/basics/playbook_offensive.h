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

#ifndef PLAYBOOK_OFFENSIVE_H
#define PLAYBOOK_OFFENSIVE_H

#include <src/entity/controlmodule/playbook/playbook.h>
#include <src/entity/player/role/vssroles.h>

class Playbook_Offensive : public Playbook {
private:
    // Roles
    Role_Defender *_rl_def;
    Role_Supporter *_rl_sup;

    void configure(int numPlayers);
    void run(int numPlayers);
    int maxNumPlayer();

    // Parameters
    quint8 previousSupporter;

public:
    Playbook_Offensive();
    QString name();
};

#endif // PLAYBOOK_OFFENSIVE_H
