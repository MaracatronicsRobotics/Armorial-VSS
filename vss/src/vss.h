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

#ifndef VSS_H
#define VSS_H

// GEARSystem
#include <GEARSystem/gearsystem.hh>

// Utils
#include <iostream>
#include <src/utils/color/color.h>
#include <src/utils/fieldside/fieldside.h>
#include <src/utils/fields/fields.h>

// World
#include <src/entity/world/world.h>

// Coach
#include <src/entity/controlmodule/coach/coach.h>

// Strategy
#include <src/entity/controlmodule/strategy/essentials/mrcstrategy.h>

// Roles
#include <src/entity/player/role/vssroles.h>

class VSS
{
public:
    VSS(quint8 teamId, Colors::Color teamColor, FieldSide teamSide);

    // Start and stop methods
    bool start();
    void stop();

    // Server configuration
    void setServerAddress(QString ipAddress, int port);
private:
    // Team info
    const quint8 _teamId;
    const Colors::Color _teamColor;
    const FieldSide _teamSide;

    // Server connection
    QString _serverAddress;
    int _serverPort;

    // Internal methods
    bool connectToServer();

    // Modules
    Controller *_ctr;
    World *_world;
    Coach *_coach;

    // Teams
    void setupTeams(quint8 opTeamId, Colors::Color opTeamColor, FieldSide opTeamSide);
    void setupOurPlayers();
    void setupOpPlayers(quint8 opTeamId);
    VSSTeam *_ourTeam;
    VSSTeam *_opTeam;

    // GUI
};

#endif // VSS_H
