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

        _assistantId = dist()->getPlayer();
        _barrierId = dist()->getPlayer();
        _goalkeeperId = dist()->getPlayer();

        timer = 0;
        changedAssistBarrier = false;
    }
}

void Playbook_Supporter::run(int numPlayers) {

    mutex.lock();
    if(changedAssistBarrier){
        timer ++;
        if(timer >= 300){
            changedAssistBarrier = false;
            timer = 0;
        }
    }
    mutex.unlock();

    mutex.lock();
    _rl_supporter.at(_assistantId)->_positioning = 1;
    setPlayerRole(_assistantId, _rl_supporter.at(_assistantId));

    _rl_supporter.at(_barrierId)->_positioning = 0;
    setPlayerRole(_barrierId, _rl_supporter.at(_barrierId));

    setPlayerRole(_goalkeeperId, _rl_goalkeeper.at(_goalkeeperId));
    mutex.unlock();

    mutex.lock();
    connect(_rl_supporter.at(_assistantId), SIGNAL(sendSignal()), this, SLOT(receiveSignal()), Qt::DirectConnection);
    mutex.unlock();
    mutex.lock();
    connect(_rl_supporter.at(_barrierId), SIGNAL(sendSignal()), this, SLOT(receiveSignal()), Qt::DirectConnection);
    mutex.unlock();
}

void Playbook_Supporter::receiveSignal(){
    mutex.lock();
    if(!changedAssistBarrier){
        changedAssistBarrier = true;
        quint8 assist = _assistantId;
        _assistantId = _barrierId;
        _barrierId = assist;
    }
    mutex.unlock();
}
