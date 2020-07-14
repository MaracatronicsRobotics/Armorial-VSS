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

#include "coach.h"

#include <src/entity/controlmodule/vssteam.h>
#include <src/entity/player/playerbus.h>
#include <src/entity/controlmodule/coach/coachutils.h>
#include <src/entity/controlmodule/strategy/strategy.h>

QString Coach::name(){
    return "VSSCoach";
}

Coach::Coach(VSSTeam *ourTeam, VSSTeam *theirTeam)
{
    _ourTeam = ourTeam;
    _theirTeam = theirTeam;

    // Initialize PlayerBus
    PlayerBus::initialize(ourTeam, theirTeam);

    // Coach utils
    _utils = new CoachUtils(ourTeam);

    // null strat
    _strat = NULL;
}

Coach::~Coach(){
    if(_strat != NULL)
        delete _strat;

    delete _utils;
}

StrategyState* Coach::getStrategyState(){
    return strategy()->getLastStrategy();
}

void Coach::run(){
    if(_ourTeam->avPlayersSize() == 0){
        std::cout << "[COACH] No players available!" << std::endl;
        return ;
    }

    // get strategy
    Strategy *strat = strategy();

    // run strategy
    if(strat != NULL){
        if(strat->isInitialized() == false){
            strat->initialize(_ourTeam, _theirTeam, _utils);
        }
        strat->runStrategy();
    }

}

void Coach::setStrategy(Strategy *strat){
    _mutexStrategy.lock();

    // deleting old strategy
    if(_strat != NULL){
        delete _strat;
    }

    //setting new
    _strat = strat;

    _mutexStrategy.unlock();
}

Strategy* Coach::strategy(){
    _mutexStrategy.lock();
    Strategy *s = _strat;
    _mutexStrategy.unlock();

    return s;
}
