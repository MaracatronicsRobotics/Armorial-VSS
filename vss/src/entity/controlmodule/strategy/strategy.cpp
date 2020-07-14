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

#include "strategy.h"
#include <src/entity/controlmodule/coach/playersdistribution.h>
#include <src/entity/controlmodule/vssteam.h>
#include <src/entity/controlmodule/strategy/strategystate.h>
#include <src/entity/controlmodule/strategy/essentials/vssstrategy.h>

Strategy::Strategy()
{
    _kickerId = -1;
    _initialized = false;
    _dist = NULL;
    _lastStrategy = NULL;
}

Strategy::~Strategy(){
    // Load StrategyStates
    QList<StrategyState*> gameStates = _strategyStatesTable.values();

    // Delete one by one
    QList<StrategyState*>::const_iterator it;
    for(it=gameStates.constBegin(); it!=gameStates.constEnd(); it++)
        delete *it;

    // Clear for convenience
    _strategyStatesTable.clear();

    if(_dist!=NULL)
        delete _dist;
}

void Strategy::initialize(VSSTeam *ourTeam, VSSTeam *theirTeam, CoachUtils *utils){
    _ourTeam = ourTeam;
    _theirTeam = theirTeam;
    _utils = utils;

    _dist = new PlayersDistribution(_ourTeam, &_kickerId);

    // Configure strategy states
    configure();

    _initialized = true;

}

void Strategy::runStrategy(int gameState) {
    // Get current StrategyState and run
    StrategyState *strategyState = getStrategyState(gameState);
    if(strategyState!=NULL) {
        if(strategyState->isInitialized()==false)
            strategyState->initialize(_ourTeam, _theirTeam, _utils, _dist, &_kickerId);

        _lastStrategy = strategyState;
        strategyState->runStrategyState();
    }
}

void Strategy::setStrategyState(int gameState, StrategyState *strategyState) {
    // Check pointer
    if(strategyState==NULL)
        return;
    // Add to game states table
    if(_strategyStatesTable.contains(gameState))
        delete _strategyStatesTable.value(gameState);
    _strategyStatesTable.insert(gameState, strategyState);
}

StrategyState* Strategy::getStrategyState(int gameState) {
    // Check if child strategy set that game state
    if(_strategyStatesTable.contains(gameState)==false) {
        std::cout << "[ERROR] Strategy '" << name().toStdString() << " has no set state. ID: " << gameState << std::endl;
        return NULL;
    }
    // Return StrategyState
    return _strategyStatesTable.value(gameState);
}
