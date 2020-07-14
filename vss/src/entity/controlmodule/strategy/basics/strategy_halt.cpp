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

#include "strategy_halt.h"
#pragma GCC diagnostic ignored "-Wunused-parameter"

QString Strategy_Halt::name() {
    return "Strategy_Halt";
}

Strategy_Halt::Strategy_Halt() {
    _pb_halt = NULL;
}

void Strategy_Halt::configure(int numOurPlayers) {
    usesPlaybook(_pb_halt = new Playbook_Halt());
}

void Strategy_Halt::run(int numOurPlayers) {
    QList<quint8> allPlayers = dist()->getAllPlayers();
    if(!allPlayers.isEmpty())
        _pb_halt->addPlayers(allPlayers);
}
