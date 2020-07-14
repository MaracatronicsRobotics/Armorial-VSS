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

#ifndef VSSSTRATEGY_H
#define VSSSTRATEGY_H

#include <src/entity/controlmodule/strategy/strategy.h>

class VSSStrategy : public Strategy {
public:
    VSSStrategy();
    virtual ~VSSStrategy();
    virtual QString name() = 0;
protected:
    friend class Strategy;
    typedef enum {
        HALT,
        GAMEON,
        GAMEOFF,
        OURINDIRECTKICK,
        OURDIRECTKICK,
        OURKICKOFF,
        OURPENALTY,
        THEIRKICKOFF,
        THEIRPENALTY,
        THEIRDIRECTKICK,
        THEIRINDIRECTKICK,
        TIMEOUT,
        UNDEFINED
    } SSLGameState;
    void setStrategyState(VSSStrategy::SSLGameState gameState, StrategyState *strategyState);
private:
    virtual void configure() = 0;
    void runStrategy();
};


#endif // VSSSTRATEGY_H
