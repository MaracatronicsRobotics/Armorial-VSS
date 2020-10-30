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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMutex>
#include <iostream>
#include <src/entity/controlmodule/controlmodule.h>
#include <src/entity/controlmodule/coach/basecoach.h>

class Coach : public ControlModule
{
public:
    Coach(VSSTeam *ourTeam, VSSTeam *theirTeam);
    virtual ~Coach();
    QString name();

    QString getAgressivity();
    void setStrategy(Strategy *strat);
    void setReferee(VSSReferee *referee);

    StrategyState* getStrategyState();
private:
    // run controller
    void run();

    // Game info
    CoachUtils *_utils;

    // Teams
    VSSTeam *_ourTeam;
    VSSTeam *_theirTeam;

    // Referee
    VSSReferee *_referee;

    // Strategy
    QMutex _mutexStrategy;
    Strategy *_strat;
    Strategy* strategy();
};

#endif // CONTROLLER_H
