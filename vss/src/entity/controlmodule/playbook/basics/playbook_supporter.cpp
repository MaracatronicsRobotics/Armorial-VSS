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

#include "playbook_supporter.h"

QString Playbook_Supporter::name() {
    return "Playbook_Supporter";
}

Playbook_Supporter::Playbook_Supporter() {
    _assistantId = DIST_INVALID_ID;
    _barrierId = DIST_INVALID_ID;
    _goalkeeperId = DIST_INVALID_ID;

    takeFirstIds = false;
}

int Playbook_Supporter::maxNumPlayer() {
    return INT_MAX;
}

void Playbook_Supporter::configure(int numPlayers) {
    for(int i = 0; i < numPlayers; i++) {
        Role_Supporter *rl_supporter = new Role_Supporter();
        Role_Goalkeeper *rl_goalkeeper = new Role_Goalkeeper();

        usesRole(rl_supporter);
        usesRole(rl_goalkeeper);

        _rl_supporter.push_back(rl_supporter);
        _rl_goalkeeper.push_back(rl_goalkeeper);

        _assistantId = DIST_INVALID_ID;
        _barrierId = DIST_INVALID_ID;
        _goalkeeperId = DIST_INVALID_ID;

        takeFirstIds = false;
        changedAssistBarrier = false;
    }

    // Connect every supporter with this playbook
    for(int i = 0; i < numPlayers; i++){
        connect(_rl_supporter.at(i), SIGNAL(sendSignal()), this, SLOT(receiveSignal()), Qt::DirectConnection);
    }

}

void Playbook_Supporter::run(int numPlayers) {
    // Take first ids (first it)
    if(!takeFirstIds){
        _goalkeeperId = dist()->getOneKNN(loc()->ourGoal());
        _assistantId = dist()->getPlayer();
        _barrierId = dist()->getPlayer();

        takeFirstIds = true;
    }

    // Check if signal has been received
    if(changedAssistBarrier){
        if(timer.timesec() >= 4){
            changedAssistBarrier = false;
            timer.stop();
        }
    }

    // Setting supporter
    _rl_supporter.at(_assistantId)->setPositioning(1);
    setPlayerRole(_assistantId, _rl_supporter.at(_assistantId));

    // Setting barrier
    _rl_supporter.at(_barrierId)->setPositioning(0);
    setPlayerRole(_barrierId, _rl_supporter.at(_barrierId));

    // Setting GK
    setPlayerRole(_goalkeeperId, _rl_goalkeeper.at(_goalkeeperId));
}

void Playbook_Supporter::receiveSignal(){
    mutex.lock();

    if(!changedAssistBarrier){
        changedAssistBarrier = true;
        timer.start();
        quint8 assist = _assistantId;
        _assistantId = _barrierId;
        _barrierId = assist;
    }

    mutex.unlock();
}
