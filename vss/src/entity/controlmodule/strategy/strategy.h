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

#ifndef STRATEGY_H
#define STRATEGY_H

#include <src/entity/controlmodule/coach/basecoach.h>

class Strategy
{
public:
    Strategy();
    virtual ~Strategy();

    bool isInitialized() { return _initialized; }
    void initialize(VSSTeam *ourTeam, VSSTeam *theirTeam, CoachUtils *utils, VSSReferee *referee);

    // Coach loop
    virtual void runStrategy() = 0;
    virtual QString name() = 0;

    // Auxiliary methods
    StrategyState* getLastStrategy() { return _lastStrategy; }

protected:
    virtual void setStrategyState(int gameState, StrategyState *strategyState);
    void runStrategy(int gameState);

private:
    StrategyState* getStrategyState (int gameState);

    // Children strategies
    virtual void configure() = 0;

    // Teams
    VSSTeam *_ourTeam;
    VSSTeam *_theirTeam;

    // Referee
    VSSReferee *_referee;

    // Player distribution
    // here goes things like kickerId, ref last state and playerdistribution itself
    qint8 _kickerId;
    PlayersDistribution *_dist;
    CoachUtils *_utils;

    // Game states table
    QHash<int, StrategyState*> _strategyStatesTable;

    // Strategy initialized
    bool _initialized;

    // Aux
    StrategyState *_lastStrategy;
};

#endif // STRATEGY_H
