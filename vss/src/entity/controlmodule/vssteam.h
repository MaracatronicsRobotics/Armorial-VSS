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

#ifndef VSSTEAM_H
#define VSSTEAM_H

#include <GEARSystem/gearsystem.hh>
#include <src/utils/color/color.h>
#include <src/utils/fieldside/fieldside.h>
#include <src/entity/player/vssplayer.h>

class VSSTeam
{
public:
    VSSTeam(quint8 teamId, WorldMap *wm);
    ~VSSTeam();

    // Getters
    quint8 teamId() const;
    Colors::Color teamColor() const;
    FieldSide teamSide() const;
    bool hasBallPossession();

    // Setters
    void setTeamColor(Colors::Color teamColor);
    void setTeamSide(FieldSide teamSide);

    // Opponent team info
    void setOpponentTeam(VSSTeam *team) { _opTeam = team; }
    VSSTeam *opTeam() const { return _opTeam; }

    // Players
    void addPlayer(VSSPlayer *player);

    // Available players access
    int avPlayersSize();
    QHash<quint8, VSSPlayer*> avPlayers();
    void updateAvailablePlayers();

    // WorldMap access
    WorldMap *wm() { return _wm; }
    Position ballPosition();
    Locations* loc() { return _loc; }

private:
    // Team info
    quint8 _teamId;
    Colors::Color _teamColor;
    FieldSide _teamSide;

    // OPTeam access
    VSSTeam *_opTeam;

    // Players
    QMutex _muxPlayers;
    QHash<quint8, VSSPlayer*> _players;
    QHash<quint8, VSSPlayer*> _avPlayers;

    // Locations
    Locations *_loc;

    // WorldMap
    WorldMap *_wm;
};

#endif // VSSTEAM_H
