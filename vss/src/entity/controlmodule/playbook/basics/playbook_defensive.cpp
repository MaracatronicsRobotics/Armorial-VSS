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

#include "playbook_defensive.h"


QString Playbook_Defensive::name() {
    return "Playbook_Defensive";
}

Playbook_Defensive::Playbook_Defensive() {
    _GK_ID = DIST_INVALID_ID;
}

int Playbook_Defensive::maxNumPlayer() {
    return INT_MAX;
}

void Playbook_Defensive::configure(int numPlayers) {
    usesRole(_rl_gk = new Role_Goalkeeper());
    usesRole(_rl_halt = new Role_Halt());
}

void Playbook_Defensive::run(int numPlayers) {
    // Taking the goalkeeper
    if(_GK_ID != DIST_INVALID_ID){
        dist()->removePlayer(_GK_ID);
        setPlayerRole(_GK_ID, _rl_gk);
    } else std::cout << "[PLAYBOOK DEFENSIVE] Goleiro Indisponível\n";

    for(int i = 0; i < numPlayers - 1; i++){
        quint8 playerId = dist()->getPlayer();
        setPlayerRole(playerId, _rl_halt);
    }
}
