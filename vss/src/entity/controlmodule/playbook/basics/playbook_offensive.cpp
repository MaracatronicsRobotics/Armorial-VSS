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

#include "playbook_offensive.h"

QString Playbook_Offensive::name() {
    return "Playbook_Offensive";
}

Playbook_Offensive::Playbook_Offensive() {
    _rl_def = nullptr;
    _rl_sup = nullptr;
}

int Playbook_Offensive::maxNumPlayer() {
    return INT_MAX;
}

void Playbook_Offensive::configure(int numPlayers) {
    usesRole(_rl_def = new Role_Defender());
    usesRole(_rl_sup = new Role_Supporter());
}

void Playbook_Offensive::run(int numPlayers) {
    static quint8 strikerId = dist()->getPlayer();
    static quint8 supporterId = dist()->getPlayer();
    //Se num alcance de 0.5m da bola, o Supporter estiver dentro do alcance e o Striker estiver fora, as Roles se trocam
    if (PlayerBus::ourPlayer(supporterId)->distBall() < 0.5f && PlayerBus::ourPlayer(strikerId)->distBall() > 0.5f) {
        quint8 id = strikerId;
        strikerId = supporterId;
        supporterId = id;
    }

    setPlayerRole(strikerId, _rl_def);
    setPlayerRole(supporterId, _rl_sup);
}
