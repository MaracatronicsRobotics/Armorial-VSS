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

#include "playbook_halt.h"

QString Playbook_Halt::name() {
    return "Playbook_Halt";
}

Playbook_Halt::Playbook_Halt() {
}

int Playbook_Halt::maxNumPlayer() {
    return INT_MAX;
}

void Playbook_Halt::configure(int numPlayers) {
    for(int i = 0; i < numPlayers; i++) {
        Role_Halt *rl_halt = new Role_Halt();
        Role_Supporter *rl_supporter = new Role_Supporter();
        //usesRole(rl_halt);
        usesRole(rl_supporter);
        //_rl_halt.push_back(rl_halt);
        _rl_supporter.push_back(rl_supporter);
    }
}

void Playbook_Halt::run(int numPlayers) {
    for(int i = 0; i < numPlayers; i++){
        quint8 playerId = dist()->getPlayer();
        //setPlayerRole(playerId, _rl_halt.at(i));
        setPlayerRole(playerId, _rl_supporter.at(i));
    }
}
