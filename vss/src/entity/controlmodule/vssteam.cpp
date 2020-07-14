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

#include "vssteam.h"
#include <src/entity/locations.h>

VSSTeam::VSSTeam(quint8 teamId, WorldMap *wm)
{
    _teamId = teamId;
    _wm = wm;

    _loc = new Locations(this);
}

VSSTeam::~VSSTeam(){
    // delete pointers here
    delete _loc;
}

quint8 VSSTeam::teamId() const{
    return _teamId;
}

Colors::Color VSSTeam::teamColor() const{
    return _teamColor;
}

FieldSide VSSTeam::teamSide() const{
    return _teamSide;
}

void VSSTeam::setTeamColor(Colors::Color teamColor){
    _teamColor = teamColor;
}

void VSSTeam::setTeamSide(FieldSide teamSide){
    _teamSide = teamSide;
}

void VSSTeam::addPlayer(VSSPlayer *player) {
    _players.insert(player->playerId(), player);
}

int VSSTeam::avPlayersSize() {
    _muxPlayers.lock();
    int result = _avPlayers.size();
    _muxPlayers.unlock();

    return result;
}

QHash<quint8,VSSPlayer*> VSSTeam::avPlayers() {
    _muxPlayers.lock();
    QHash<quint8,VSSPlayer*>result(_avPlayers);
    _muxPlayers.unlock();

    return result;
}


void VSSTeam::updateAvailablePlayers() {
    _muxPlayers.lock();
    // Clear hash
    _avPlayers.clear();

    // Update players
    QHash<quint8,VSSPlayer*>::iterator it;
    for(it=_players.begin(); it!=_players.end(); it++) {
        quint8 id = it.key();
        VSSPlayer *player = it.value();
        // If position is known, player is available
        if(player->position().isUnknown()==false)
            _avPlayers.insert(id, player);
    }

    _muxPlayers.unlock();
}

Position VSSTeam::ballPosition(){
    // Only one ball filtered, then return it's position
    return _wm->ballPosition(0);
}

bool VSSTeam::hasBallPossession(){
    // Iterating in players
    const QList<VSSPlayer*> players = _players.values();
    QList<VSSPlayer*>::const_iterator it;
    for(it = players.constBegin(); it != players.end(); it++){
        const VSSPlayer* player = *it;
        if(player->hasBallPossession()){
            return true;
        }
    }
    return false;
}
